#ifndef PULSE_INPUT_H
#define PULSE_INPUT_H

#include "config.h"

#if INPUT_MODE == INPUT_MODE_DIGITAL_PULSE

#include <stdbool.h>
#include <stdint.h>

// Configures PA3 as floating input with EXTI3 falling-edge interrupt and enables EXTI2_3_IRQn.
// Must be called after MX_GPIO_Init() / MX_ADC_Init() so the ADC MSP does not overwrite the
// pin configuration afterwards.
void PulseInput_Init(void);

// Periodic housekeeping: finalises the current frame once the line has been silent for at
// least PULSE_SYNC_MIN_MS, and raises the link-lost flag after PULSE_FRAME_TIMEOUT_MS.
// Call once per main-loop iteration.
void PulseInput_Tick(void);

// Returns true and writes the latest decoded code into *code exactly once per frame.
// Clears the internal "ready" flag, so two consecutive calls return false for the same frame.
bool PulseInput_TryGetFrame(uint8_t *code);

// True once PULSE_FRAME_TIMEOUT_MS has elapsed without a falling edge. Remains true until
// a new frame arrives. During the boot grace window (see PULSE_LINK_GRACE_MS) this returns
// false even with no frames yet, so the ESP-side iHeater-link has time to come up.
bool PulseInput_IsLinkLost(void);

// True from boot until either the first frame is decoded or PULSE_LINK_GRACE_MS elapses.
// Used by the LED renderer to show a "waiting for first frame" indication.
bool PulseInput_IsWaitingFirstFrame(void);

// Must be called from HAL_GPIO_EXTI_Callback when GPIO_Pin == GPIO_PIN_3.
void PulseInput_OnFallingEdge(void);

#endif // INPUT_MODE == INPUT_MODE_DIGITAL_PULSE

#endif // PULSE_INPUT_H
