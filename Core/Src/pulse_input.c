#include "pulse_input.h"

#if INPUT_MODE == INPUT_MODE_DIGITAL_PULSE

#include "main.h"
#include "stm32f0xx_hal.h"

// The RMT pulse input is wired to the TH2 connector (PB1 / ADC_IN9 in analog builds).
// In the pulse build this pin is reconfigured as a digital EXTI input; the trigger-thermistor
// feature is disabled, heater/air thermistors stay on their usual pins (PA3 and PA0).
#define PULSE_GPIO_Port TH2_GPIO_Port
#define PULSE_Pin TH2_Pin

// NOTE: no `static` on purpose — STM32CubeMonitor reads symbols from the ELF by name and
// cannot see file-scope statics. Keep these as translation-unit globals so live probes work.
volatile uint32_t s_last_edge_ms;
volatile uint32_t s_frame_start_ms;
volatile uint16_t s_frame_counter;
volatile uint8_t s_last_code;
volatile bool s_frame_ready;
volatile bool s_link_lost;
volatile bool s_in_frame;
volatile bool s_first_frame_received;
volatile uint32_t s_init_ms;

static void finalize_frame_unsafe(void)
{
    if (s_frame_counter == 0U)
    {
        return;
    }
    uint16_t code = s_frame_counter;
    if (code > PULSE_MAX_COUNT)
    {
        code = PULSE_MAX_COUNT;
    }
    s_last_code = (uint8_t)code;
    s_frame_ready = true;
    s_frame_counter = 0U;
    s_in_frame = false;
    s_first_frame_received = true;
}

void PulseInput_Init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    GPIO_InitTypeDef init = {0};
    init.Pin = PULSE_Pin;
    init.Mode = GPIO_MODE_IT_FALLING;
    init.Pull = GPIO_NOPULL; // external pull-up from the TH2 divider holds the line HIGH when idle
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(PULSE_GPIO_Port, &init);

    s_init_ms = HAL_GetTick();
    s_last_edge_ms = s_init_ms;
    s_frame_start_ms = s_init_ms;
    s_frame_counter = 0U;
    s_last_code = 0U;
    s_frame_ready = false;
    s_link_lost = false;
    s_in_frame = false;
    s_first_frame_received = false;

    // PB1 → EXTI line 1 → EXTI0_1_IRQn on STM32F0
    HAL_NVIC_SetPriority(EXTI0_1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void PulseInput_OnFallingEdge(void)
{
    uint32_t now = HAL_GetTick();
    uint32_t delta = now - s_last_edge_ms;

    if (delta >= PULSE_SYNC_MIN_MS)
    {
        // Long silence → previous frame (if any) ends here, a new one begins.
        finalize_frame_unsafe();
        s_frame_start_ms = now;
        s_in_frame = true;
    }

    if (s_frame_counter < PULSE_MAX_COUNT)
    {
        s_frame_counter++;
    }
    s_last_edge_ms = now;
    s_link_lost = false;
}

void PulseInput_Tick(void)
{
    uint32_t now = HAL_GetTick();
    uint32_t silence = now - s_last_edge_ms;

    if (s_in_frame && silence >= PULSE_SYNC_MIN_MS)
    {
        __disable_irq();
        // Re-read under critical section in case an edge arrived between the checks.
        if (s_in_frame && (HAL_GetTick() - s_last_edge_ms) >= PULSE_SYNC_MIN_MS)
        {
            finalize_frame_unsafe();
        }
        __enable_irq();
    }

    if (silence >= PULSE_FRAME_TIMEOUT_MS)
    {
        s_link_lost = true;
    }
}

bool PulseInput_TryGetFrame(uint8_t *code)
{
    if (!s_frame_ready)
    {
        return false;
    }
    __disable_irq();
    if (code != NULL)
    {
        *code = s_last_code;
    }
    s_frame_ready = false;
    __enable_irq();
    return true;
}

bool PulseInput_IsLinkLost(void)
{
    // Suppress link-lost while we are still waiting for the very first frame inside the
    // boot grace window — gives the ESP-side iHeater-link time to bring up Wi-Fi/MQTT.
    if (!s_first_frame_received && (HAL_GetTick() - s_init_ms) < PULSE_LINK_GRACE_MS)
    {
        return false;
    }
    return s_link_lost;
}

bool PulseInput_IsWaitingFirstFrame(void)
{
    if (s_first_frame_received)
    {
        return false;
    }
    return (HAL_GetTick() - s_init_ms) < PULSE_LINK_GRACE_MS;
}

#endif // INPUT_MODE == INPUT_MODE_DIGITAL_PULSE
