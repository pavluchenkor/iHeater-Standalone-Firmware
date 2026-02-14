# iHeater Trigger Logic Update

## Purpose

Automatic on/off control of the iHeater based on the 3D printer bed temperature.

---

## Physical Process

```
Bed temperature (°C)
    ^
 90 |         ┌────────────────┐ Printing
 85 |        /                  \
 80 |-------/--------------------\-------- TRIGGER_OFF_TEMP
 75 |------/----------------------\------- TRIGGER_ON_TEMP
 50 |     /                        \
 25 |____/                          \____
    └─────────────────────────────────────> Time
         Heating    Printing   Cooling
```

**Scenario:**
1. The printer starts a print, and the bed heats up from 25°C
2. At 75°C (`TRIGGER_ON_TEMP`) - iHeater turns on
3. The bed reaches 85°C, printing continues, iHeater is running
4. Printing is finished, bed heater turns off, cooling begins
5. At 80°C (`TRIGGER_OFF_TEMP`) - iHeater turns off
6. The bed cools down to room temperature

---

## Code Logic (`trigger_state_reached`)

### State `trigger_state_reached = false` (waiting for heat-up)

```c
if (trigger_temp >= TRIGGER_ON_TEMP && mode == MODE_0)
{
    trigger_heater_enabled = true;
    mode = TRIGGER_MODE;  // Heater turned on
}

if (trigger_heater_enabled && mode != MODE_0 && trigger_temp > TRIGGER_OFF_TEMP)
{
    trigger_state_reached = true;  // Bed reached operating temperature
}
```

### State `trigger_state_reached = true` (tracking cooldown)

```c
if (trigger_temp <= TRIGGER_OFF_TEMP && mode != MODE_0)
{
    mode = MODE_0;
    trigger_heater_enabled = false;  // Heater turned off
}

if (mode == MODE_0 && trigger_temp <= TRIGGER_ON_TEMP)
{
    trigger_state_reached = false;  // Ready for a new cycle
}
```

---

## Trigger Temperature Calibration

### Enter calibration mode

**MODE_0 + long press (>2 sec)** -> `CALIBRATION_STEP_1`

### Step 1: Set `TRIGGER_ON_TEMP`

- **Indication**: 1 LED blinks at 10 Hz
- Heat the bed to the desired turn-on temperature (~75°C)
- **Long press** -> save and go to step 2

### Step 2: Set `TRIGGER_OFF_TEMP`

- **Indication**: 2 LEDs blink at 10 Hz
- Heat the bed to the desired turn-off temperature (~80°C)
- **Long press** -> save to NVS and exit
- **Success indication**: all 3 LEDs blink 3 times

### Cancel calibration

**Short press** at any step -> cancel and roll back changes (`NVS_Load`)

---

## Default Values

```c
#define TRIGGER_ON_TEMP  75.0f  // Turn on iHeater
#define TRIGGER_OFF_TEMP 80.0f  // Turn off iHeater
```


---

## Safety

- In calibration mode, iHeater is **off** (`PWM = 0`)
- If calibration is canceled, values are **rolled back** from NVS
- Watchdog (IWDG) is reset during LED indication

---

## State Diagram

```
                    ┌─────────────────────────────────────┐
                    │                                     │
                    ▼                                     │
┌─────────┐  long  ┌───────────────────┐  long  ┌───────────────────┐
│  MODE_0 │ ────── │ CALIBRATION_STEP_1│ ────── │ CALIBRATION_STEP_2│
└─────────┘        │    (1 LED 10Hz)   │        │    (2 LED 10Hz)   │
     ▲             └───────────────────┘        └───────────────────┘
     │                      │                           │
     │    short (cancel)    │      short (cancel)       │ long (save)
     └──────────────────────┴───────────────────────────┘
                                                        │
                                              ┌─────────▼─────────┐
                                              │  NVS_Save()       │
                                              │  3 LED x 3 times  │
                                              │  -> MODE_0        │
                                              └───────────────────┘
```
