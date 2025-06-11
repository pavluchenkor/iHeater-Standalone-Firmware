[![Release](https://img.shields.io/github/v/release/pavluchenkor/iHeater-Standalone-Firmware?include_prereleases&label=latest)](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)
[![Docs](https://img.shields.io/badge/docs-view--online-green?logo=readthedocs)](https://docs.idryer.org/iHeater/README_ru/)
[![Telegram](https://img.shields.io/badge/Telegram-Join%20Chat-blue?logo=telegram)](https://t.me/iDryer)


# Standalone iHeater Firmware

iHeater is a compact and affordable solution for 3D printers without an active heated chamber or with limited connection options to the mainboard. It's especially useful for printers with proprietary boards lacking free ports for fans, heaters, or thermistors. iHeater solves this with a simple and universal approach.

It can operate standalone with its own firmware or be integrated with Klipper over USB.

![iHeater](imgweb/iHeater.png)

---

## About the Firmware

**iHeater** is firmware for a standalone device that manages the temperature of a 3D printer's heated chamber.

---

## Purpose

The device ensures heating and stable temperature maintenance inside the printer's chamber, which is critical for printing ABS, ASA, and other shrinkage-sensitive and adhesion-sensitive filaments.

---

## How to Configure the Firmware

### File `config.h`

This file contains all configurable parameters:

| Parameter                                 | Description                                                                                   |
| ----------------------------------------- | --------------------------------------------------------------------------------------------- |
| `SELECTED_THERMISTOR_TYPE`                | Select thermistor type (0-5, see below)                                                       |
| `MODE_TEMP_0`-`MODE_TEMP_7`                           | Setpoint temperatures for each mode. Can be any float value, e.g. `MODE_TEMP_4 67.5`                |
| `MAX_TEMP`, `MIN_TEMP`                    | Allowed temperature range for the heater                                                      |
| `MAX_AIR_TEMP`, `MIN_AIR_TEMP`            | Allowed temperature range for air                                                             |
| `HEATER_MIN_PWM`                          | Minimum PWM required to start heater response monitoring                                      |
| `HEATER_RESPONSE_TIMEOUT_MS`              | Timeout for detecting heater response (default 5 seconds)                                     |
| `AIR_TARGET_TIMEOUT_MS`                   | Maximum time allowed to reach air target temperature (default 10 minutes)                    |
| `TIMER_CLOCK`, `PWM_FREQUENCY`, `PWM_STEPS` | PWM configuration (frequency and resolution)                                                |

### Thermistors

Selected using `SELECTED_THERMISTOR_TYPE`:

| ID | Type                          | Notes                              |
|----|-------------------------------|-------------------------------------|
| 0  | ATC Semitec 104GT-2           | Standard used by E3D                |
| 1  | ATC Semitec 104NT-4-R025H42G  | 104GT alternative                   |
| 2  | EPCOS 100K B57560G104F        | Widely used                         |
| 3  | Generic 3950                  | **Default**, common Chinese type    |
| 4  | SliceEngineering 450          | High-temp support                   |
| 5  | TDK NTCG104LH104JT1           | Medical-grade accuracy              |

### Temperature Modes and LEDs

Each mode sets a target temperature and is displayed via binary LEDs:

| Mode   | Temperature | LED3 | LED2 | LED1 |
|--------|-------------|------|------|------|
| MODE_TEMP_0  | 0.0°C       | 0    | 0    | 0    |
| MODE_TEMP_1  | 55.0°C      | 0    | 0    | 1    |
| MODE_TEMP_2  | 60.0°C      | 0    | 1    | 0    |
| MODE_TEMP_3  | 65.0°C      | 0    | 1    | 1    |
| MODE_TEMP_4  | 70.0°C      | 1    | 0    | 0    |
| MODE_TEMP_5  | 75.0°C      | 1    | 0    | 1    |
| MODE_TEMP_6  | 80.0°C      | 1    | 1    | 0    |
| MODE_TEMP_7  | 85.0°C      | 1    | 1    | 1    |

---
### Automatic enabling and disabling r1.1

Macros

```
#define TRIGGER_ON_TEMP  45.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_2
```

These parameters define the activation and deactivation temperatures for the heater, as well as the mode to be enabled upon activation.

Assuming the trigger thermistor is located near the heated bed.
With the current settings, the chamber heater will turn on when the bed temperature reaches 45°C and will turn off when the bed temperature drops below 80°C. These parameters are suitable for most filaments that require an actively heated chamber and do not cause logical conflicts during operation.

It is also possible to select the mode that will be activated when the trigger is engaged. For example, TRIGGER_MODE MODE_2 is currently selected, which means that MODE_TEMP_2 will be activated, setting the chamber temperature to 60.0°C.


---

## How to Flash the Firmware

### Download

[Скачайте прошивку со страницы релизов](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


### Using Precompiled Binary

[Download the firmware from the releases page](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases)


This firmware is pre-configured with the temperature modes from MODE_TEMP_0 to MODE_TEMP_7 (as described above) and designed to work with a Generic 3950 thermistor (type 3 in `config.h`).

### Using DFU Mode (USB)

=== "r1"

    1. Set the BOOT0 jumper
    2. Connect USB, the device will be recognized as DFU 3.1 Upload the firmware using STM32CubeProgrammer:

    3.1.1 Specify the path to the `.bin` firmware file (e.g.: `Firmware/iHeater_v1.2.3.bin`) 3.1.2 Set the download address: `0x08000000`

    3.1.3 Click "Start Programming"

    or

    3.2 Upload the firmware via command line:

    ```bash
    dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
    ```

    4. Remove the jumper and power cycle the device

=== "r1.1"

    1. Press and hold the BOOT button
    2. Connect USB, the device will be recognized as DFU 
    3. Upload the firmware using STM32CubeProgrammer:
    
        3.1.1 Specify the path to the `.bin` firmware file (e.g.: `Firmware/iHeater_v1.2.3.bin`)

        3.1.2 Set the download address: `0x08000000`

        3.1.3 Click "Start Programming"

        or

        3.2 Upload the firmware via command line:

        ```bash
        dfu-util -a 0 -s 0x08000000 -D Firmware/iHeater_vX.Y.Z.bin
        ```

    4. Power cycle the device


---

## How to Use

=== "r1"

    * **Short press** of the MODE button - switches modes (0-7)
    * **Long press (2+ sec)** - resets the mode to 0 (OFF)
    * **LED indicators** display the current mode according to the table above
    * **Flashing LEDs** - mode is active but target temperature not yet reached
    * **Solid light** - target temperature reached

=== "r1.1"

    !!! note "Revision Differences"

        revision r1.1 features a trigger port for automatic chamber heating control. When a thermistor is connected to the trigger port, heating will turn on and off according to the temperatures specified in config.h, by default 45 and 80 for heating activation and deactivation.


    * **Short press** of the MODE button - switches modes (0-7)
    * **Long press (2+ sec)** - resets the mode to 0 (OFF)
    * **LED indicators** display the current mode according to the table above
    * **Flashing LEDs** - mode is active but target temperature not yet reached
    * **Solid light** - target temperature reached

---

## Error Handling

The device auto-resets and shows an error code using LED indicators.

| Code  | Description                               | LED1 | LED2 | LED3  |
|-------|-------------------------------------------|------|------|-------|
| 0x01  | Air failed to reach target in time        | ON   | OFF  | OFF   |
| 0x02  | Heater not responding                     | OFF  | ON   | OFF   |
| 0x03  | Manual shutdown (unused)                  | ON   | ON   | OFF   |
| 0x04  | Air thermistor disconnected                | OFF  | OFF  | ON    |
| 0x05  | Heater thermistor disconnected             | ON   | OFF  | ON    |
| 0x06  | Heater overtemperature                     | OFF  | ON   | ON    |
| 0x07  | Air overtemperature                        | ON   | ON   | ON    |
| 0xFF  | Unknown error                              | ALL  | SOLID| ON    |

### Resetting an Error

Hold the MODE button during startup until the LEDs change. The stored error will be cleared from flash memory, and the device will resume normal operation.

---

## Fine Tuning

If there is confirmation that the thermocouple readings deviate from the actual temperature, the fine tuning function can be used. For this purpose, it is necessary to measure the resistance of the corresponding resistors and record these values instead of the default ones.


```C++ title="Actual resistance of the pull-up resistors"
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f
```

---

This software is licensed under the GNU General Public License v3.0 for non-commercial use only.