# Trigger — automatic iHeater start by bed temperature

## What it is and why

Trigger is an automatic on/off function for the iHeater based on the 3D printer bed temperature. iHeater monitors the bed temperature through an additional thermistor (TH2) and:

- **Turns on** the heater when the bed starts warming up (print begins)
- **Turns off** the heater when the bed cools down (print finished)

You don't need to manually turn iHeater on and off — everything happens automatically.

!!! warning "Requirements"
    The trigger function is only available on the **iHeater rev 1.1** board, which has the third thermistor input (TH2).

---

## How to connect

Install a thermistor (NTC 100K) on the 3D printer bed and connect it to the **TH2** input on the iHeater board.

The thermistor should be mounted on a metal or aluminum part of the bed that conducts heat well — as close to the heating zone as possible. It cannot be attached directly to the heating element, but the better the thermal contact with the heated surface, the more accurately the trigger will work. The thermistor readings will be lower than the actual heater temperature — this is normal, the thresholds are calibrated for the specific mounting location.

---

## How it works

```
Bed temperature (°C)
    ^
 90 |         ┌────────────────┐  Printing
 85 |        /                  \
 80 |-------/-----OFF iHeater----\-------- OFF threshold (80°C)
 75 |------/----------------------\------- ON threshold (75°C)
 50 |     /    iHeater running     \
 25 |____/                          \____
    └─────────────────────────────────────> Time
         Heating    Printing    Cooling
```

**Operating scenario (ON threshold = 75°C, OFF threshold = 80°C):**

1. Printer starts printing, bed heats up
2. Bed temperature reaches **75°C** (ON threshold) → iHeater **turns on** and starts heating the chamber
3. Bed temperature continues rising and passes **80°C** (OFF threshold) → iHeater registers **goal reached**
4. Printing in progress, bed is hot — iHeater maintains chamber temperature
5. Print finished, bed starts cooling down
6. Bed temperature drops to **80°C** (OFF threshold) → iHeater **turns off**
7. Bed cools to room temperature — system is ready for a new cycle

!!! info "Why the OFF threshold doesn't have to be higher than the ON threshold"
    The system operates in **two phases**, and iHeater can only be turned off **after the goal is reached** (step 3):

    **Phase 1** — waiting to turn on: iHeater is off, the system waits for the temperature to rise to the ON threshold. The OFF threshold is **ignored** at this stage.

    **Phase 2** — after goal reached: the temperature has passed through the OFF threshold upward, the goal is registered. Now, when the temperature **drops** below the OFF threshold — iHeater will turn off.

    Since the phases run sequentially and don't overlap, the OFF threshold can be either higher or lower than the ON threshold:

    - **OFF threshold = 80°C, ON threshold = 75°C** — bed must heat up to 80°C for the goal to be registered, turns off when cooling to 80°C
    - **OFF threshold = 70°C, ON threshold = 75°C** — goal is registered immediately (bed is already above 70°C at the moment of turning on), turns off when cooling to 70°C

!!! info "Why iHeater won't turn on again during cooling"
    When the bed cools down and passes through the ON threshold (e.g., 75°C) — iHeater **will not turn on again**. The system is protected by the goal reached flag:

    1. iHeater turned off when temperature dropped to the OFF threshold (80°C) — but the goal reached flag **remains active**
    2. Bed continues cooling and passes 75°C (ON threshold) — turning on is **blocked** because the flag is still active
    3. Temperature drops **below** the ON threshold (below 75°C) — only then the flag is **reset**, and the system is ready for a new cycle

    Therefore, to turn on again, the bed must **fully cool down** below the ON threshold and then heat up to it again — meaning a new print must start.

!!! danger "OFF threshold must not fall within iHeater's heating zone"
    After printing ends, the printer bed turns off and starts cooling. But iHeater is still running at this point and radiates heat itself. If the TH2 thermistor is positioned so that iHeater warms it with its own heat — the temperature on the thermistor may not drop below the OFF threshold, and iHeater **will never turn off**.

    **Make sure** that the OFF threshold is set **below** the temperature that the TH2 thermistor reads when the printer bed is off but iHeater is running. In other words — the OFF threshold must be below the temperature that iHeater itself is capable of heating the thermistor mounting area to.

---

## Default settings

| Parameter | Value | Description |
|-----------|-------|-------------|
| ON threshold | **75°C** | Bed temperature at which iHeater turns on |
| OFF threshold | **80°C** | Bed temperature at which iHeater turns off |
| Heating mode | **MODE 2** (60°C) | Mode in which iHeater operates when triggered |

When triggered, iHeater turns on in **mode 2** — target air temperature 60°C. Indication: **LED2** is on.

---

## Trigger threshold calibration

If the default values (75°C / 80°C) don't suit your printer, you can adjust the thresholds for your bed operating temperature.

### When calibration is needed

- You want iHeater to turn on earlier or later
- The thermistor is located far from the bed heater and shows a lower temperature

### Step-by-step instructions

!!! note "Important"
    During calibration, the iHeater heating element is **off** — calibration is safe.

#### Step 1: Enter calibration mode

- Make sure iHeater is in **standby mode** (MODE 0 — all LEDs off)
- **Hold the button for 2+ seconds** → enter calibration
- **Indication**: LED1 blinks rapidly (~10 Hz) — you are in ON threshold setup mode

#### Step 2: Set the ON threshold

- Heat the printer bed to the temperature at which you want iHeater to **turn on**
- Wait for the temperature to stabilize
- **Hold the button for 2+ seconds** → value saved
- **Indication**: LED1 + LED2 blink rapidly — you are in OFF threshold setup mode

#### Step 3: Set the OFF threshold

- Heat the bed to a temperature slightly **above** the ON threshold (recommended +5°C)
- **Hold the button for 2+ seconds** → value saved to flash memory
- **Success indication**: all 3 LEDs blink 3 times
- iHeater will automatically return to standby mode (MODE 0)

#### Cancel calibration

At **any step** — **short press the button** → calibration cancelled, previous values restored.

### Calibration example (bed 100°C for ABS)

| Step | Action | Bed temperature |
|------|--------|-----------------|
| 1 | Long press in MODE 0 | — |
| 2 | Heat bed, long press | **75°C** (ON threshold) |
| 3 | Heat bed higher, long press | **80°C** (OFF threshold) |

---

## LED indication

| State | LED1 | LED2 | LED3 | Description |
|-------|------|------|------|-------------|
| Standby (MODE 0) | ![off](img/ball_gifs/black_ball.gif) | ![off](img/ball_gifs/black_ball.gif) | ![off](img/ball_gifs/black_ball.gif) | iHeater off, trigger waiting |
| Trigger activated (heating) | ![off](img/ball_gifs/black_ball.gif) | ![blink](img/ball_gifs/blinking_ball.gif) | ![off](img/ball_gifs/black_ball.gif) | iHeater heating to target temperature |
| Trigger activated (ready) | ![off](img/ball_gifs/black_ball.gif) | ![on](img/ball_gifs/red_ball.gif) | ![off](img/ball_gifs/black_ball.gif) | iHeater reached target temperature (60°C) |
| Calibration step 1 | ![blink](img/ball_gifs/blinking_ball_10hz.gif) | ![off](img/ball_gifs/black_ball.gif) | ![off](img/ball_gifs/black_ball.gif) | Setting ON threshold |
| Calibration step 2 | ![blink](img/ball_gifs/blinking_ball_10hz.gif) | ![blink](img/ball_gifs/blinking_ball_10hz.gif) | ![off](img/ball_gifs/black_ball.gif) | Setting OFF threshold |
| Calibration complete | ![blink](img/ball_gifs/blinking_ball_10hz.gif) | ![blink](img/ball_gifs/blinking_ball_10hz.gif) | ![blink](img/ball_gifs/blinking_ball_10hz.gif) | 3 times — success |

---

## State diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                                                                 │
│  ┌───────────┐     Bed temperature ≥ ON threshold               │
│  │  Standby  │ ─────────────────────────────────┐               │
│  │  (MODE 0) │                                  │               │
│  │  LED: all │  ◄─────────┐                     ▼               │
│  │  off      │            │          ┌──────────────────┐       │
│  └───────────┘            │          │  iHeater running │       │
│        ▲                  │          │  (TRIGGER MODE)  │       │
│        │                  │          │  LED2 on         │       │
│  Bed cooled               │          └──────────────────┘       │
│  below ON threshold       │                     │               │
│  (ready for new           │    Bed temperature  │               │
│   cycle)                  │    ≤ OFF threshold  │               │
│                           │                     ▼               │
│                  ┌────────┴─────────────────────────┐           │
│                  │  iHeater turns off → MODE 0      │           │
│                  └──────────────────────────────────┘           │
└─────────────────────────────────────────────────────────────────┘
```

---

## FAQ

**Trigger doesn't activate even though the bed is hot**

- Check the TH2 thermistor connection
- Make sure the bed temperature actually reaches the ON threshold
- Calibrate the thresholds lower if your bed temperature is not high enough

**iHeater turns on and immediately turns off**

- The ON and OFF thresholds are too close. Calibrate with at least 5°C difference

**I want to change the heating mode when triggered**

- In the current firmware, the trigger mode is fixed — **MODE 2** (60°C). It can only be changed by recompiling the firmware (`TRIGGER_MODE` parameter in `config.h`)
