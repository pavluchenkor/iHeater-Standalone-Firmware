<div align="center">

<img src="docs/img/iDryer_logo_small.png" width="220" alt="iDryer">

# iHeater Standalone Firmware

**Firmware for the iHeater heater. Press the button, it heats.**

[![Documentation](https://img.shields.io/badge/docs-idryer.org-e7352c)](https://docs.idryer.org/en/projects/iheater/standalone/) [![Telegram](https://img.shields.io/badge/Telegram-iDryer-2ca5e0)](https://t.me/iDryer) [![Discord](https://img.shields.io/badge/Discord-join-5865f2)](https://discord.gg/jGce5eeHHz) [![License](https://img.shields.io/badge/license-GPL--3.0--or--later-blue)](LICENSE)

<img src="docs/img/iHeater_200.png" width="480" alt="iHeater">

</div>

---

## What this is

Firmware for the [iHeater](https://github.com/pavluchenkor/iHeater) heater. It turns the heater into a standalone device: press the button and it heats, press again for a different temperature, hold to switch it off.

Three LEDs show what is going on. Nothing else is required to run it.

> **The full description is in the [firmware documentation](https://docs.idryer.org/en/projects/iheater/standalone/).** It covers the mode table with temperatures, the LED blink patterns, error codes, every configuration parameter and the flashing procedure.

## What it solves

An actively heated chamber is required for ABS, ASA and other filaments sensitive to warping and adhesion. The heater has to reach temperature and hold it for the whole print.

With this firmware it does that on its own, independently of the printer. Pick a temperature and it runs.

## Autostart from bed temperature

The main feature, and the main reason to choose this firmware.

Put a thermistor on the printer bed and connect it to input **TH2**. From there the heater runs itself: the bed starts heating, so a print has started and the chamber comes up to temperature. The bed cools down, the print is over and heating stops.

The switch-on and switch-off temperatures, and the mode that gets started, are set in the configuration. By default heating starts at 75 °C on the bed and stops at 80 °C.

> Autostart is available on board revision **1.1**, which has the input for the third thermistor.

→ [How to set up the trigger](https://docs.idryer.org/en/projects/iheater/standalone/trigger/)

## Build for iHeater Link

A separate firmware variant for anyone who needs the heater controlled from outside.

In it the temperature does not come from the button but as a digital command on the input. The setpoint is supplied by [iHeater Link](https://github.com/pavluchenkor/iHeater-Link): it reads the setpoint from the printer and passes it to the heater. That adds the portal, the mobile app and print-driven automation.

If the link is lost for longer than 5 seconds, the heater removes heating and turns on the fan. An unattended heater left without control is not acceptable. When commands arrive again, operation resumes on its own, without a reset or a button press.

## Control

There is one control: the **MODE** button.

- **Short press** switches between modes.
- **Long press** switches the heater off.

Three LEDs show the selected mode. Blinking means heating is in progress and the target has not been reached. Steady means the chamber is at temperature.

Eight modes, from off up to 85 °C. The values are not hard-wired: any mode can be given its own temperature in the configuration, 67.5 °C included.

→ [Mode table and indication reference](https://docs.idryer.org/en/projects/iheater/standalone/)

## Thermistors

Six types are supported, including ATC Semitec 104GT-2, EPCOS 100K, Generic 3950, SliceEngineering 450 and TDK. The type is selected in the configuration. Prebuilt binaries use Generic 3950, the most common one.

## Protection

The firmware watches more than overheating. It checks that the heater actually responds to the applied power, and that the chamber reaches the target temperature within a reasonable time. If it does not, operation stops.

The temperature limits for the heater and for the air are set separately and cap everything else.

The same three LEDs show what happened: thermistor open circuit or short, overheating, heater not responding. Each case has its own pattern. To clear an error, hold the MODE button while power is applied.

→ [Error codes and what they mean](https://docs.idryer.org/en/projects/iheater/standalone/)

> **This is an electric heating appliance.** Never leave the device running unattended.

## Flashing the board

Prebuilt binaries are on the [releases page](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases). They are enough if the standard modes and the Generic 3950 thermistor suit you.

Flashing is done over USB in DFU mode: on revision 1 close the BOOT0 jumper, on revision 1.1 hold the BOOT button. Then use STM32CubeProgrammer or `dfu-util`.

Your own temperatures, your own thermistor, your own trigger thresholds are edited in `config.h` and require a rebuild.

→ [Build and flashing](https://docs.idryer.org/en/projects/iheater/standalone/assembly/)

## Place in the ecosystem

| Layer | What it does | Repository |
|---|---|---|
| Heater | Board, mechanics, schematics | [iHeater](https://github.com/pavluchenkor/iHeater) |
| Standalone firmware | Operation without a host — **this repository** | iHeater-Standalone-Firmware |
| Link | Wi-Fi, portal, print-driven automation | [iHeater Link](https://github.com/pavluchenkor/iHeater-Link) |
| Cloud | Portal, app, integrations | [portal.idryer.org](https://portal.idryer.org/) |

## What you need

| Component | Qty | Note | Where to get it |
|---|---|---|---|
| iHeater board | 1 | autostart requires revision 1.1 | [store.idryer.org](https://store.idryer.org/) |
| NTC 100K thermistor | 1 | on the printer bed, autostart only | — |
| USB cable | 1 | for flashing, any cable works | — |

## Status

Working firmware, prebuilt binaries on the [releases page](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases). Two builds: button-controlled and for iHeater Link.

## What is in the repository

| Path | What it is |
|---|---|
| `Core/` | Firmware logic: modes, control loop, trigger, indication |
| `Lib/` | Thermistors, PID, filtering |
| `Drivers/` | HAL and CMSIS from STMicroelectronics |
| `Firmware/` | Built binaries |
| `boards/` | Board revision definitions |

## License

Code is under the [GNU GPL v3.0 or later](LICENSE), [NOTICE](NOTICE).

The iDryer name is not covered by the license, see [TRADEMARKS.md](https://github.com/pavluchenkor/idryer-core/blob/main/TRADEMARKS.md).

The hardware design is licensed separately.

## Help

- [Telegram](https://t.me/iDryer)
- [Discord](https://discord.gg/jGce5eeHHz)
- [Documentation](https://docs.idryer.org/en/projects/iheater/standalone/)

Guides and teardowns on the channel: [YouTube](https://www.youtube.com/@iDryerProject) · [Rutube](https://rutube.ru/channel/34401569/)

## Contributing

If you have tuned the trigger thresholds for your printer, added a thermistor or found a bug, open an issue or send a pull request.

## Next

[Download a prebuilt firmware image](https://github.com/pavluchenkor/iHeater-Standalone-Firmware/releases) and flash it over USB. Nothing needs to be built.
