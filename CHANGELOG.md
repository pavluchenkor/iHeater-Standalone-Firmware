Changelog

**v0.5.0**
- New build `iheater_rev1_1_pulse` for pairing with the ESP-based iHeater-link module
- Chamber setpoint is received over a single GPIO line as a pulse train on the TH2 connector (PB1, EXTI); code equals air temperature in °C
- Accepted command range: `0` = off, `10` = off (agreed with iHeater-link), `45..90` = heating setpoint in °C, other values ignored
- Heater and air thermistors stay on their normal connectors (TH0/PA3 and TH1/PA0); trigger feature is disabled in the pulse build
- New dedicated mode `MODE_PULSE` with its own LED pattern (chasing light LED1→LED2→LED3 at 500 ms step); in `MODE_0` LED1 stays solid as a link-alive heartbeat and LED3 flashes ~80 ms on every accepted frame
- New fault `ERROR_LINK_LOST` (0x08): all three LEDs blink at 0.25 Hz when no pulse frames arrive within 1.5 s; device resets with heater off
- 20-second boot grace window on the pulse build: link-lost is suppressed while waiting for the very first frame from iHeater-link (slow chasing light, 1000 ms step). After 20 s without a frame the fault fires as usual.
- In the pulse build the MODE button no longer cycles modes or triggers calibration — it only clears a persisted error at startup
- Standard protections (overheat, open thermistor, heater-no-response) remain active in the pulse build
- Existing analog builds (`iheater_rev1_0`, `iheater_rev1_1`, `idryer_rev1_1`) are unchanged except for the version string

**v0.3.0**
- Trigger temperature calibration mode via button
- Long press in MODE_0 enters calibration
- Step 1: set TRIGGER_ON_TEMP (1 LED blinks 10Hz)
- Step 2: set TRIGGER_OFF_TEMP (2 LEDs blink 10Hz)
- Success indication: all 3 LEDs blink 3 times
- Short press cancels calibration and rolls back changes
- Settings saved to NVS (flash memory)

**v0.2.2**
 - Fixed temperature data source
 - comments translated to English

**v0.2.1**
- Thermistor Steinhart-Hart coefficients have been adapted from Klipper's configuration

**v0.2.0**
 - Selectable automatic activation temperature
 - Selectable automatic deactivation temperature
 - Selectable heating mode for automatic activation

**v0.1.3**
 - Correction of PID controller
 - Fix ERROR_OVERHEAT_HEATER error
 - General fixes to improve stability
 - Air temperature fluctuations +-0.5C


**v0.1.0 - Initial Public Draft**

- Basic standalone firmware structure for iHeater

- Temperature mode control

- Temperature mode indication using 3 LEDs

- Dual thermistor support (air + heater)

- Basic error detection and reset logic

- Default config: Generic 3950 thermistor

- The firmware is ready for initial testing