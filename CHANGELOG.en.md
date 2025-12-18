Changelog
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