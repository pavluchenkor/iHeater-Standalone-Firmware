iHeater Standalone Firmware v0.5.0

## iHeater Standalone Firmware v0.5.0

This release introduces a new build variant `iheater_rev1_1_pulse` for integration with the ESP-based iHeater-link module and consolidates the firmware version across all build environments.

### Added
- New build environment `iheater_rev1_1_pulse` for pairing with iHeater-link
- RMT pulse-input receiver on the TH2 connector (PB1, EXTI falling-edge) — the chamber setpoint is delivered as a pulse train where the pulse count equals the air temperature in °C
- Accepted command set: `0` = off, `10` = off (agreed "off-code" with iHeater-link), `45..90` = heating setpoint in °C, other values are ignored
- Dedicated `MODE_PULSE` with its own LED indication (chasing light LED1 → LED2 → LED3 at 500 ms step)
- Link-alive heartbeat in `MODE_0` of the pulse build: LED1 stays solid while LED3 flashes ~80 ms on every accepted frame
- New fault `ERROR_LINK_LOST` (0x08): all three LEDs blink at 0.25 Hz when no frames arrive within 1.5 s; the device resets with the heater off
- 20-second boot grace window on the pulse build to let iHeater-link finish its Wi-Fi/MQTT bring-up: while waiting for the first frame the LEDs show a slow chasing light (1000 ms step); after 20 s without any frame the fault fires as usual

### Changed
- In the pulse build the MODE button no longer cycles modes or enters calibration — its only runtime function is clearing a persisted error at startup
- `config.h` — new macros: `INPUT_MODE`, `PULSE_SYNC_MIN_MS`, `PULSE_FRAME_TIMEOUT_MS`, `PULSE_OFF_CODE`, `PULSE_MIN_TARGET`, new enum value `MODE_PULSE`, new error code `ERROR_LINK_LOST`
- `README.ru.md` / `README.en.md` — new section "Работа с iHeater-link" / "Working with iHeater-link"

### Notes
- Standard protections (overheat, open thermistor, heater-no-response) remain active in the pulse build
- Heater and air thermistors stay on their normal connectors (TH0/PA3 and TH1/PA0); trigger feature is disabled in the pulse build
- Existing analog builds (`iheater_rev1_0`, `iheater_rev1_1`, `idryer_rev1_1`) are unchanged except for the version string

### Firmware
Validated firmware in this release:
- `iheater_rev1_0_v0.5.0.bin`
- `iheater_rev1_1_v0.5.0.bin`
- `iheater_rev1_1_pulse_v0.5.0.bin`
- `idryer_rev1_1_v0.5.0.bin`
