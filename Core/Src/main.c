/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "stm32f0xx_hal.h"
#include "thermistor.h"
#include "config.h"
#include "pid.h"
#include "ema_filter.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
#if BOARD_REVISION == BOARD_REV_1_0
  #define ADC_CHANNEL_COUNT 2
#elif BOARD_REVISION == BOARD_REV_1_1
  #define ADC_CHANNEL_COUNT 3
#else
  #error "Unknown board revision"
#endif

#define ADC_SAMPLES 10

uint16_t adc_dma_buffer[ADC_CHANNEL_COUNT * ADC_SAMPLES];
float adc_sum[ADC_CHANNEL_COUNT] = {0};
float adc_value[ADC_CHANNEL_COUNT] = {0};
float temperatures[ADC_CHANNEL_COUNT] = {0};

Thermistor thermistors[ADC_CHANNEL_COUNT];
EmaFilter filters[ADC_CHANNEL_COUNT];

const float pullups[] = {
    PULLUP_TH0,
    PULLUP_TH1,
#if ADC_CHANNEL_COUNT == 3
    PULLUP_TH2,
#endif
};

const float inline_resistors[] = {
    INLINE_RESISTOR_TH0,
    INLINE_RESISTOR_TH1,
#if ADC_CHANNEL_COUNT == 3
    INLINE_RESISTOR_TH2,
#endif
};

enum Mode mode = MODE_0;

float adc_value0 = 0.0f;
float adc_value1 = 0.0f;
float adc_value2 = 0.0f;

float air_temp = 0.0f;
float heater_temp = 0.0f;
float trigger_temp = 0.0f;
uint32_t previous_time = 0;

volatile bool trigger_heater_enabled = false;
volatile bool trigger_state_reached = false;

//* Температуры для режимов
const float mode_temperatures[] = {MODE_TEMP_0, MODE_TEMP_1, MODE_TEMP_2, MODE_TEMP_3, MODE_TEMP_4, MODE_TEMP_5, MODE_TEMP_6, MODE_TEMP_7};

volatile float output = 0.0f;
volatile uint8_t timer_counter = 0;

uint32_t heater_start_time = 0;
float heater_start_temp = 0.0f;
bool heater_warming_up = false;
bool heater_fault = false;


uint8_t last_mode = MODE_0;
uint32_t air_target_timer = 0;
bool air_target_tracking = false;
bool was_mode_zero = true;


bool air_fault = false;
bool air_warming_up = false;
uint32_t air_start_time = 0;
float air_start_temp = 0.0f;


//! TEST
float heater_setpoint = 0.0f;
float air_target = 0.0f;
float pwm = 0.0f;

uint32_t pulse = 0;
uint32_t period =  0;
float pwm_percent2 = 0.0f;
float smart_setpoint = 0.0f;
//! TEST

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
uint32_t LoadErrorCode(void);
void ClearErrorCode(void);
float GetTemperatureByMode(uint8_t mode);
void Update_LEDs(uint8_t mode, float current_temp, uint32_t speed);
void reset_LEDs(uint8_t repeats);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//* WDT
IWDG_HandleTypeDef hiwdg;

Thermistor therm0, therm1, therm2;

PID_Controller pid_air;
PID_Controller pid_heater;

//* Объявление фильтров
EmaFilter filter_heater;
EmaFilter filter_air;
EmaFilter filter_trigger;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_TIM2_Init();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc);
  if (HAL_ADC_Start_DMA(&hadc, (uint32_t*)adc_dma_buffer, ADC_CHANNEL_COUNT * ADC_SAMPLES) != HAL_OK) {
      Error_Handler();
  }

  //* Инициализация термисторов
  const ThermistorData *data = get_thermistor_data(SELECTED_THERMISTOR_TYPE);
  if (data == NULL) {
      Error_Handler(); //* Если тип некорректен
  }

  for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
      thermistor_init(&thermistors[i], pullups[i], inline_resistors[i]);
      thermistor_set_coefficients(&thermistors[i],
          data->t1, data->r1,
          data->t2, data->r2,
          data->t3, data->r3);
  }

  //* Инициализация ПИД-регулятора
  PID_Init(&pid_air, 6.0f, 0.1f, 12.0f, 10.0f, MAX_TEMP, 0.25f, 110.0f);
  PID_Init(&pid_heater, 3800.0f, 200.5f, 17650.0f, 0.0f, 100.0f, 0.25f, 100.0f);

  for (int ch = 0; ch < ADC_CHANNEL_COUNT; ch++)
  {
    ema_filter_init(&filters[ch], ALPHA);
  }

  mode = MODE_0;
  Update_LEDs(mode, air_temp, 0);

  MX_IWDG_Init();
  // HAL_IWDG_Refresh(&hiwdg);
  IWDG->KR = 0xAAAA; //* Рефреш

  uint32_t code = LoadErrorCode();
  // code = 0x06;
  if (code != ERROR_NONE && code != 0xFFFFFFFF)
  {
    float fake_temp = (code == ERROR_UNKNOWN) ? 100.0f : 0.0f;

    HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET);

    IWDG->KR = 0xAAAA;
    while (HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin) == GPIO_PIN_SET)
    {
      Update_LEDs(code, fake_temp, 50);
      IWDG->KR = 0xAAAA;
    }
    
    while (HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin) == GPIO_PIN_RESET)
    {
      reset_LEDs(3);
    }

    ClearErrorCode();
    HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
  }

  reset_LEDs(10);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    if (mode == MODE_0)
    {
        //* В режиме 0 смотрим только на нагреватель
        if (heater_temp > FAN_ON_TEMP)
        {
            HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET);
        }
        else if (heater_temp < FAN_OFF_TEMP)
        {
            HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
        }
    }
    else
    {
        //* В активных режимах учитываем обе температуры
        if (heater_temp > FAN_ON_ACTIVE_MODE_TEMP || air_temp > FAN_ON_ACTIVE_MODE_TEMP)
        {
            HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET);
        }
        else if (heater_temp < FAN_OFF_TEMP && air_temp < FAN_OFF_TEMP)
        {
            HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
        }
    }

    //* Обнуляем суммы перед накоплением
    for (int ch = 0; ch < ADC_CHANNEL_COUNT; ch++) {
        adc_sum[ch] = 0.0f;
    }

    for (int i = 0; i < ADC_SAMPLES; i++) {
        for (int ch = 0; ch < ADC_CHANNEL_COUNT; ch++) {
            adc_sum[ch] += adc_dma_buffer[i * ADC_CHANNEL_COUNT + ch];
        }
    }

    for (int ch = 0; ch < ADC_CHANNEL_COUNT; ch++) {
        adc_value[ch] = adc_sum[ch] / ADC_SAMPLES / 4095.0f;
        float filtered = ema_filter_update(&filters[ch], adc_value[ch]);
        temperatures[ch] = thermistor_calc_temp(&thermistors[ch], filtered);
    }

    heater_temp   = temperatures[0];
    air_temp      = temperatures[1];

    uint32_t current_time = HAL_GetTick();

    if ((current_time - previous_time) >= PID_UPDATE_INTERVAL_MS && mode != 0) {
        previous_time = current_time;

        float now = current_time / 1000.0f;
        // float dt = PID_UPDATE_INTERVAL_MS / 1000.0f;

        air_target = GetTemperatureByMode(mode);

        //*Внешний PID: воздух -> температура нагревателя
        float air_error = air_target - air_temp;
        float heater_margin = heater_temp - air_temp;

        //* Ограничение эффективности внешнего PID при холодном нагревателе
        if (heater_margin < 5.0f) {
            float scale = heater_margin / 5.0f;
            if (scale < 0.2f) scale = 0.2f;
            air_error *= scale;
        }

        // // Дополнительное усиление на этапе прогрева
        // if (air_error > 0.0f) {
        //     float boost = 1.0f + air_error / 10.0f;
        //     air_error *= boost;
        // }

        //* Финальная цель для PID
        smart_setpoint = air_temp + air_error;

        //* Умная логика принятия решения
        // float temp_error = air_target - air_temp;

        if (air_temp > air_target + 0.1f)
        {
          //* Сильно перегрет - сбрасываем
          heater_setpoint = 0.0f;
        }
        else
        {
          //* Основной PID - всегда, пока не перегрет
          heater_setpoint = PID_Compute(&pid_air, air_temp, smart_setpoint, now);

          float min_setpoint = air_target + 10.0f;
          if (air_temp < air_target - 5.0f && heater_setpoint < min_setpoint)
            heater_setpoint = min_setpoint;

          heater_setpoint += clamp(heater_setpoint, 30.0f, MAX_TEMP);
          heater_setpoint /=2;
        }

      
        //* Внутренний PID: нагреватель -> PWM
        pwm = PID_Compute(&pid_heater, heater_temp, heater_setpoint, now);

        //* Установка PWM
        period = __HAL_TIM_GET_AUTORELOAD(&htim2);
        pulse = (uint32_t)((pwm / 100.0f) * period);
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
    }

/*======================ПРОВЕРКИ============================*/
    if (air_temp <= MIN_AIR_TEMP)
    {
      SaveErrorCode(ERROR_THERMISTOR_OPEN_AIR);
      NVIC_SystemReset();
    }

    if (air_temp >= MAX_AIR_TEMP)
    {
      SaveErrorCode(ERROR_OVERHEAT_AIR);
      NVIC_SystemReset();
    }

    if (heater_temp <= MIN_TEMP)
    {
      SaveErrorCode(ERROR_THERMISTOR_OPEN_HEATER);
      NVIC_SystemReset();
    }

    if (heater_temp >= MAX_TEMP + 10)
    {
      SaveErrorCode(ERROR_OVERHEAT_HEATER);
      NVIC_SystemReset();
    }

    if (!heater_fault && mode != MODE_TEMP_0) {
        if (pwm > HEATER_MIN_PWM) {
            if (!heater_warming_up) {
                //* Засекаем момент начала нагрева
                heater_start_time = HAL_GetTick();
                heater_start_temp = heater_temp;
                heater_warming_up = true;
            } else {
                //* Уже греем - проверим прогресс
                if (heater_temp > heater_start_temp + HEATER_MIN_TEMP_DELTA) {
                    //* Температура начала расти - всё норм
                    heater_warming_up = false;
                } else if (HAL_GetTick() - heater_start_time > HEATER_RESPONSE_TIMEOUT_MS) {
                    //* Время вышло, а температура не изменилась - ошибка
                    heater_fault = true;
                    heater_warming_up = false;
                }
            }
        } else {
            //* PWM упал - сбрасываем ожидание
            heater_warming_up = false;
        }
    }

    if (heater_fault)
    {
      SaveErrorCode(ERROR_FAULT_HEATER_NORESP);
      NVIC_SystemReset();  
    }

    if (!air_fault && mode != MODE_TEMP_0)
    {
      //* Если температура уже достигнута - не отслеживаем
      if (air_temp >= air_target - 5.0f)
      {
        air_warming_up = false;
      }
      else if (!air_warming_up)
      {
        //* Начинаем отслеживание
        air_start_time = HAL_GetTick();
        air_start_temp = air_temp;
        air_warming_up = true;
      }
      else
      {
        //* Уже отслеживаем - проверим прогресс
        if (air_temp > air_start_temp + AIR_MIN_TEMP_DELTA)
        {
          //* Нагрев пошёл - всё хорошо
          air_warming_up = false;
        }
        else if (HAL_GetTick() - air_start_time > AIR_RESPONSE_TIMEOUT_MS)
        {
          //* Время вышло - ошибка
          air_fault = true;
          air_warming_up = false;
        }
      }
    }
    else
    {
      air_warming_up = false;
    }

    if (air_fault)
    {
        SaveErrorCode(ERROR_FAULT_TEMP_TIMEOUT);
        NVIC_SystemReset();  
    }
/*=================ПРОВЕРКИ  КОНЕЦ============================*/

#if BOARD_REVISION == BOARD_REV_1_1
/*=========================ТРИГГЕР============================*/
    trigger_temp  = temperatures[2];
    //* Один раз срабатывает при первом превышении
    if (!trigger_heater_enabled && mode == MODE_0 && trigger_temp >= TRIGGER_ON_TEMP)
    {
      trigger_heater_enabled = true;
      mode = TRIGGER_MODE;
    }

    //* При достижении целевой температуры стола + 10С
    if (!trigger_state_reached && trigger_heater_enabled && mode != MODE_0 && trigger_temp > TRIGGER_OFF_TEMP + 5)
    {
      trigger_state_reached = true;
    }

    //* Выключаем, если температура упала ниже OFF
    if (trigger_state_reached && trigger_temp <= TRIGGER_OFF_TEMP && mode != MODE_0)
    {
      mode = MODE_0;
      trigger_heater_enabled = false;
      //* НЕ сбрасываем trigger_state_reached - он останется true
      //* пока не наступит новое задание (печать)
    }

    //* Сброс флага включения триггера для повторного запуска нагрева
    if (mode == MODE_0 && trigger_state_reached && trigger_temp <= TRIGGER_ON_TEMP - 5)
    {
      //* Не активен в этом режиме, сбрасываем флаг
      trigger_state_reached = false;
    }
/*=====================ТРИГГЕР КОНЕЦ==========================*/
#endif

    static uint32_t button_press_time = 0;
    static bool button_was_pressed = false;
    static bool long_press_handled = false;

    uint32_t now = HAL_GetTick();

    if (HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin) == GPIO_PIN_RESET) 
    {
        if (!button_was_pressed) {
            button_was_pressed = true;
            button_press_time = now;
            long_press_handled = false;
        } else {  //* Долгое удержание - сброс
            if (!long_press_handled && (now - button_press_time > 2000)) {
                mode = MODE_0;
                heater_setpoint = MODE_TEMP_0;
                air_target = 0.0f;
                long_press_handled = true;
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
                pwm = 0;
                
                while (HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin) == GPIO_PIN_RESET)
                {
                  reset_LEDs(3);
                }
                
            }
        }
    }
    else //* кнопка отпущена
    {
        if (button_was_pressed) {
            if (!long_press_handled && (now - button_press_time < 2000)) {
                //* Короткое нажатие
                mode = (mode + 1) % 8;
            }
            if (mode == MODE_0)
            {
              reset_LEDs(1);
            }
            //* Сброс флагов
            button_was_pressed = false;
            long_press_handled = false;
            HAL_Delay(300); //* антидребезг
        }
    }


    Update_LEDs(mode, air_temp, 500);

    //* На всякий случай, а случаи разные бывают
    if (mode == MODE_0)
    {
        // // Сброс состояний триггера
        // if(trigger_heater_enabled || trigger_state_reached)
        //   {
        //     trigger_heater_enabled = false;
        //     trigger_state_reached = false;
        //   }

        heater_setpoint = 0.0f;
        air_target = 0.0f;
        SetHeaterPWM(0.0f); //* или __HAL_TIM_SET_COMPARE(...) напрямую
    }

    // HAL_IWDG_Refresh(&hiwdg);
    IWDG->KR = 0xAAAA;

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = ENABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
#if BOARD_REVISION == BOARD_REV_1_1
  sConfig.Channel = ADC_CHANNEL_9;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
#endif

  /** Configure the analog watchdog
  */
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_SINGLE_REG;
  AnalogWDGConfig.Channel = ADC_CHANNEL_0;
  AnalogWDGConfig.ITMode = DISABLE;
  AnalogWDGConfig.HighThreshold = 0;
  AnalogWDGConfig.LowThreshold = 0;
  if (HAL_ADC_AnalogWDGConfig(&hadc, &AnalogWDGConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */

static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */
   //* Рассчитываем параметры для таймера
  uint32_t prescaler = (TIMER_CLOCK / (PWM_FREQUENCY * PWM_STEPS)) - 1;
  uint32_t period = PWM_STEPS - 1;
  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = prescaler;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = period;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, FAN_Pin|LED3_Pin|LED2_Pin|LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : FAN_Pin LED3_Pin LED2_Pin LED1_Pin */
  GPIO_InitStruct.Pin = FAN_Pin|LED3_Pin|LED2_Pin|LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : MODE_Pin */
  GPIO_InitStruct.Pin = MODE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MODE_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void Update_LEDs(uint8_t mode, float current_temp, uint32_t speed)
{
    static uint32_t last_toggle_time = 0;
    static bool blink_state = false;

    float target_temp = GetTemperatureByMode(mode);
    bool at_target = current_temp >= (target_temp - 5.0f); //* допуск, например 5C

    uint32_t tick = HAL_GetTick();

    //* Моргаем, если температура еще не достигнута
    if (!at_target) {
        if (tick  - last_toggle_time >= speed) { //* 2 Гц = каждые 500 мс, т.е. toggle каждые 250 мс
            blink_state = !blink_state;
            last_toggle_time = tick;
        }
    } else {
        blink_state = true; //* зажечь стабильно
        last_toggle_time = tick;
    }

    //* Отобразить состояние на светодиодах
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, ((mode & 0x01) && blink_state) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, ((mode & 0x02) && blink_state) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, ((mode & 0x04) && blink_state) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void reset_LEDs(uint8_t repeats)
{
  for (int i = 0; i < repeats; i++)
  {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);

    HAL_Delay(200);
    IWDG->KR = 0xAAAA;
  }
}


//* Функция для получения температуры по текущему режиму
float GetTemperatureByMode(uint8_t mode) {
    uint8_t max_modes = sizeof(mode_temperatures) / sizeof(mode_temperatures[0]);
    if (mode < max_modes) {
        return mode_temperatures[mode]; //* Возвращаем значение из массива
    } else {
        return 0.0f; //* Защита от выхода за пределы массива
    }
}


void SetHeaterPWM(float pwm_percent)
{
    // pwm_percent2 = clamp(pwm_percent, 0.0f, 100.0f);
    pwm_percent2 = pwm_percent;

    //* Получить значение ARR (Period)
    period = __HAL_TIM_GET_AUTORELOAD(&htim2);

    //* Рассчитать значение сравнения
    pulse = (uint32_t)((pwm_percent2 / 100.0f) * period);
    // pulse = 60;

    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
}


void MX_IWDG_Init(void)
{
    // hiwdg.Instance = IWDG;
    // hiwdg.Init.Prescaler = IWDG_PRESCALER_64;
    // hiwdg.Init.Reload = 1250; //1250  ~2 секунды при LSI = 40kHz
    // if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
    // {
    //     Error_Handler();
    // }

    // Активация
    IWDG->KR = 0xCCCC;  //* Enable
    IWDG->KR = 0x5555;  //* Allow write
    IWDG->PR = 0x03;    //* Prescaler: 64
    IWDG->RLR = 2000;   //* Reload value
    while (IWDG->SR != 0) {} //* Ждём готовности
    IWDG->KR = 0xAAAA;  //* Первый сброс
}


void SaveErrorCode(uint32_t code)
{
    HAL_FLASH_Unlock();

    //* Стереть страницу
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError = 0;

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = ERROR_FLASH_ADDRESS;
    eraseInit.NbPages = 1;

    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return; //* ошибка стирания
    }

    //* Записываем  4 байта
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ERROR_FLASH_ADDRESS, code) != HAL_OK) {
        HAL_FLASH_Lock();
        return; //* ошибка записи
    }

    HAL_FLASH_Lock();
}


uint32_t LoadErrorCode(void)
{
    return *(uint32_t*)ERROR_FLASH_ADDRESS;
}


void ClearErrorCode(void)
{
    SaveErrorCode(ERROR_NONE);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
