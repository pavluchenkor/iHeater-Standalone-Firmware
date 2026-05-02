/*****************************************************************
************ DO NOT TOUCH UNLESS YOU KNOW WHAT YOU DO ************
********* Any changes here may break core functionality.**********
***************** Proceed only if you are 100% sure.**************
*****************************************************************/
#ifndef CONFIG_H
#define CONFIG_H

enum Mode
{
    MODE_0,
    MODE_1,
    MODE_2,
    MODE_3,
    MODE_4,
    MODE_5,
    MODE_6,
    MODE_7,
    CALIBRATION_STEP_1,
    CALIBRATION_STEP_2,
    MODE_PULSE  // Pulse build only: heating controlled by RMT command in pulse_air_target.
};

#define ERROR_NONE 0x00 // No error
// LED1=OFF, LED2=OFF, LED3=OFF

#define ERROR_FAULT_TEMP_TIMEOUT 0x01 // Air failed to reach the target temperature within timeout
// LED1=ON,  LED2=OFF, LED3=OFF

#define ERROR_FAULT_HEATER_NORESP 0x02 // Heater is not responding to increased power
// LED1=OFF, LED2=ON,  LED3=OFF

#define ERROR_MANUAL_SHUTDOWN 0x03 // Manual shutdown by user (?)
// LED1=ON,  LED2=ON,  LED3=OFF

#define ERROR_THERMISTOR_OPEN_AIR 0x04 // Air thermistor disconnected (open circuit)
// LED1=OFF, LED2=OFF, LED3=ON

#define ERROR_THERMISTOR_OPEN_HEATER 0x05 // Heater thermistor disconnected (open circuit)
// LED1=ON,  LED2=OFF, LED3=ON

#define ERROR_OVERHEAT_HEATER 0x06 // Heater overtemperature
// LED1=OFF, LED2=ON,  LED3=ON

#define ERROR_OVERHEAT_AIR 0x07 // Air overtemperature
// LED1=ON,  LED2=ON,  LED3=ON

#define ERROR_LINK_LOST 0x08 // RMT pulse input: no frames received within timeout (digital-pulse build only)
// LED1/LED2/LED3 blink synchronously at 4 Hz (pulse build)

#define ERROR_UNKNOWN 0xFF // Unexpected (unknown) error
// LED1/LED2/LED3 stay ON

#define ERROR_FLASH_ADDRESS (0x08007C00) // Last 1-KB Flash page on a 32K MCU

#define TIMER_CLOCK 48000000       // Timer clock frequency (Hz)
#define PWM_FREQUENCY 300          // Desired PWM frequency (Hz)
#define PWM_STEPS 100              // PWM resolution (number of steps)
#define DELTA 20                   // Delta between air and heater temperature
#define MIN_TEMP 0                 // Set according to enclosure material
#define MAX_TEMP 110               // Set according to enclosure material
#define MIN_AIR_TEMP 0             // Set according to enclosure material
#define MAX_AIR_TEMP 90            // Set according to enclosure material
#define PID_UPDATE_INTERVAL_MS 100 // PID update interval in milliseconds

/**************************************************
***************************************************
****************** USER SETTINGS ******************
***************************************************
***************************************************/

// Device profiles
#define DEVICE_PROFILE_HEATER 0
#define DEVICE_PROFILE_DRYER 1

#define DEVICE_PROFILE DEVICE_PROFILE_HEATER // DEVICE_PROFILE_HEATER or DEVICE_PROFILE_DRYER

#ifndef DEVICE_PROFILE
#define DEVICE_PROFILE DEVICE_PROFILE_HEATER
#endif

#define BOARD_REV_1_0 0
#define BOARD_REV_1_1 1
//! >>> Specify the hardware version: <<<
#ifndef BOARD_REVISION
#define BOARD_REVISION BOARD_REV_1_1 // BOARD_REV_1_1 or BOARD_REV_1_0 (default if not set via build_flags)
#endif

// Input mode for channel 3 (PA3):
//   INPUT_MODE_ANALOG        — PA3 is ADC_IN3 / TH0 (heater thermistor). Default.
//   INPUT_MODE_DIGITAL_PULSE — PA3 is a digital EXTI input that decodes an RMT pulse-train
//                              from ESP. Heater thermistor is re-wired to the TH2 connector
//                              (PB1), the trigger feature is disabled in this build.
#define INPUT_MODE_ANALOG 0
#define INPUT_MODE_DIGITAL_PULSE 1
#ifndef INPUT_MODE
#define INPUT_MODE INPUT_MODE_ANALOG
#endif

#if INPUT_MODE == INPUT_MODE_DIGITAL_PULSE
// Pulse-train receiver tuning. Physical line is held HIGH by external pull-up;
// ESP pulls it LOW to produce a pulse. We count FALLING edges on PB1.
// Frame layout (reference): SYNC LOW ~120 ms, then N × (LOW 2 ms + HIGH 2 ms), then IDLE HIGH.
#define PULSE_SYNC_MIN_MS 50U        // Silence (HIGH) between falling edges that marks a frame boundary.
#define PULSE_FRAME_TIMEOUT_MS 1500U // No falling edges for this long → link considered lost.
#define PULSE_MAX_COUNT 255U         // Saturation of the in-frame counter.
#define PULSE_OFF_CODE 10U           // Agreed "heater off" command from the iHeater-link ESP firmware.
#define PULSE_MIN_TARGET 45U         // Minimum accepted heating setpoint (°C). Codes below this are ignored (except 0 and PULSE_OFF_CODE).
#define PULSE_LINK_GRACE_MS 20000U   // Boot grace period: link-lost is suppressed while we are still waiting for the first frame.
#endif

#define HEATER_MIN_TEMP_DELTA 0.1f                  // Minimum required temperature increase
#define HEATER_RESPONSE_TIMEOUT_MS (1000 * 60 * 30) // Time to wait for heater response
#define HEATER_MIN_PWM 80.0f                        // Minimum PWM value for response check

#define AIR_MIN_TEMP_DELTA 1.0f                  // Minimum required air temperature increase
#define AIR_RESPONSE_TIMEOUT_MS (1000 * 60 * 30) // Time to wait for air response

#define FAN_ON_TEMP 55.0f
#define FAN_OFF_TEMP 35.0f
#define FAN_ON_ACTIVE_MODE_TEMP 40.0f

#define TRIGGER_ON_TEMP 75.0f
#define TRIGGER_OFF_TEMP 80.0f
#define TRIGGER_MODE MODE_3

#if DEVICE_PROFILE == DEVICE_PROFILE_DRYER
#define HEATER_DELTA_ABS 35.0f
#define HEATER_DELTA_PERCENT 0.30f
#define HEATER_MIN_DELTA 5.0f
// #else
// #define HEATER_DELTA_ABS        0.0f
// #define HEATER_DELTA_PERCENT    0.0f
// #define HEATER_MIN_DELTA        0.0f
#endif

// Temperature setpoints per mode
#define MODE_TEMP_0 0.0f
#define MODE_TEMP_1 55.0f
#define MODE_TEMP_2 60.0f
#define MODE_TEMP_3 65.0f
#define MODE_TEMP_4 70.0f
#define MODE_TEMP_5 75.0f
#define MODE_TEMP_6 80.0f
#define MODE_TEMP_7 85.0f

// Thermistor type selection macros
#define THERMISTOR_TYPE_0 0 // ATC Semitec 104GT-2
#define THERMISTOR_TYPE_1 1 // ATC Semitec 104NT-4-R025H42G
#define THERMISTOR_TYPE_2 2 // EPCOS 100K B57560G104F
#define THERMISTOR_TYPE_3 3 // Generic 3950
#define THERMISTOR_TYPE_4 4 // SliceEngineering 450
#define THERMISTOR_TYPE_5 5 // TDK NTCG104LH104JT1

//! Select thermistor type here
#define SELECTED_THERMISTOR_TYPE THERMISTOR_TYPE_3

// Actual resistance of the pull-up resistors
#define PULLUP_TH0 4700.0f
#define PULLUP_TH1 4700.0f
#define PULLUP_TH2 4700.0f

// Resistor wire resistance
#define INLINE_RESISTOR_TH0 0.0f
#define INLINE_RESISTOR_TH1 0.0f
#define INLINE_RESISTOR_TH2 0.0f

#define ALPHA 0.005f

#endif /* CONFIG_H */
