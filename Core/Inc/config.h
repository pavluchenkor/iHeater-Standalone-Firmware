#define ERROR_NONE                      0x00  // No error
// LED1=OFF, LED2=OFF, LED3=OFF

#define ERROR_FAULT_TEMP_TIMEOUT        0x01  // Air failed to reach the target temperature within timeout
// LED1=ON,  LED2=OFF, LED3=OFF

#define ERROR_FAULT_HEATER_NORESP       0x02  // Heater is not responding to increased power
// LED1=OFF, LED2=ON,  LED3=OFF

#define ERROR_MANUAL_SHUTDOWN           0x03  // Manual shutdown by user (?)
// LED1=ON,  LED2=ON,  LED3=OFF

#define ERROR_THERMISTOR_OPEN_AIR       0x04  // Air thermistor disconnected (open circuit)
// LED1=OFF, LED2=OFF, LED3=ON

#define ERROR_THERMISTOR_OPEN_HEATER    0x05  // Heater thermistor disconnected (open circuit)
// LED1=ON,  LED2=OFF, LED3=ON

#define ERROR_OVERHEAT_HEATER           0x06  // Heater overtemperature
// LED1=OFF, LED2=ON,  LED3=ON

#define ERROR_OVERHEAT_AIR              0x07  // Air overtemperature
// LED1=ON,  LED2=ON,  LED3=ON

#define ERROR_UNKNOWN                   0xFF  // Unexpected (unknown) error
// LED1/LED2/LED3 stay ON

#define ERROR_FLASH_ADDRESS  (0x08007C00)  // Last 1-KB Flash page on a 32K MCU

#define TIMER_CLOCK 48000000        // Timer clock frequency (Hz)
#define PWM_FREQUENCY 300           // Desired PWM frequency (Hz)
#define PWM_STEPS 100               // PWM resolution (number of steps)
#define DELTA 20                    // Delta between air and heater temperature
#define MIN_TEMP 0                  // Set according to enclosure material
#define MAX_TEMP 110                // Set according to enclosure material
#define MIN_AIR_TEMP 0              // Set according to enclosure material
#define MAX_AIR_TEMP 90             // Set according to enclosure material
#define PID_UPDATE_INTERVAL_MS 100  // PID update interval in milliseconds

#define HEATER_RESPONSE_TIMEOUT_MS (1000 * 60 * 3)      // Time to wait for heater response
#define HEATER_MIN_TEMP_DELTA     0.1f                  // Minimum required temperature increase
#define HEATER_MIN_PWM            80.0f                 // Minimum PWM value for response check

#define AIR_MIN_TEMP_DELTA     2.0f   // Minimum required air temperature increase
#define AIR_RESPONSE_TIMEOUT_MS (1000 * 60 * 10) // 10min

#define FAN_ON_TEMP     40.0f
#define FAN_OFF_TEMP    35.0f


// Temperature setpoints per mode
#define MODE0 0.0f
#define MODE1 55.0f
#define MODE2 60.0f
#define MODE3 65.0f
#define MODE4 70.0f
#define MODE5 75.0f
#define MODE6 80.0f
#define MODE7 85.0f

// Thermistor type selection macros
#define THERMISTOR_TYPE_0 0  // ATC Semitec 104GT-2
#define THERMISTOR_TYPE_1 1  // ATC Semitec 104NT-4-R025H42G
#define THERMISTOR_TYPE_2 2  // EPCOS 100K B57560G104F
#define THERMISTOR_TYPE_3 3  // Generic 3950
#define THERMISTOR_TYPE_4 4  // SliceEngineering 450
#define THERMISTOR_TYPE_5 5  // TDK NTCG104LH104JT1

//!! Select thermistor type here
#define SELECTED_THERMISTOR_TYPE THERMISTOR_TYPE_3
