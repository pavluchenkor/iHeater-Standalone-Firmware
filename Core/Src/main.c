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
#include "stm32f0xx_hal.h"
#include "thermistor.h"
#include "config.h"
#include "pid.h"
#include "median_filter.h"
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
uint8_t mode = 0;

float adc_value1 = 0.0f;
float adc_value2 = 0.0f;

float air_temp = 0.0;
float heater_temp = 0.0;

uint32_t previous_time = 0;

// Температуры для режимов
const float mode_temperatures[] = {MODE0, MODE1, MODE2, MODE3, MODE4, MODE5, MODE6, MODE7};

volatile float output = 0.0f;
volatile uint8_t timer_counter = 0;

uint32_t heater_start_time = 0;
float heater_start_temp = 0.0f;
bool heater_warming_up = false;
bool heater_fault = false;


uint8_t last_mode = 0;
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
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// WDT
IWDG_HandleTypeDef hiwdg;

Thermistor therm1, therm2;

PID_Controller pid_air;
PID_Controller pid_heater;

// Объявление фильтров
CombinedFilter combined_heater;
CombinedFilter combined_air;
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
  uint16_t adc_dma_buffer[2]; // Буфер для двух каналов
  if (HAL_ADC_Start_DMA(&hadc, (uint32_t*)adc_dma_buffer, 2) != HAL_OK) {
      Error_Handler(); // Если ADC не запустился
  }

  // Инициализация термисторов
  const ThermistorData *data = get_thermistor_data(SELECTED_THERMISTOR_TYPE);
  if (data == NULL) {
      Error_Handler(); // Если тип некорректен
  }

  // Инициализируем первый термистор
  thermistor_init(&therm1, 4700.0f, 0.0f);
  thermistor_set_coefficients(&therm1,
      data->t1, data->r1,
      data->t2, data->r2,
      data->t3, data->r3);

  // Инициализируем второй термистор
  thermistor_init(&therm2, 4700.0f, 0.0f);
  thermistor_set_coefficients(&therm2,
      data->t1, data->r1,
      data->t2, data->r2,
      data->t3, data->r3);

  // Инициализация ПИД-регулятора
  PID_Init(&pid_air, 6.0f, 3.0f, 3.0f, 30.0f, 110.0f, 0.25f, 110.0f);
  PID_Init(&pid_heater, 2.0f, 0.5f, 0.2f, 0.0f, 100.0f, 0.25f, 100.0f);


  // Инициализация комбинированных фильтров с размером буфера 11 для медианного и скользящего среднего
  if (!combined_filter_init(&combined_heater, 11, 11))
  {
    Error_Handler();
  }

  if (!combined_filter_init(&combined_air, 11, 11))
  {
    combined_filter_deinit(&combined_heater);
    Error_Handler();
  }

  // Инициализация начальных значений фильтров
  for (uint8_t i = 0; i < 11; i++)
  {
    combined_filter_update(&combined_heater, 25.0f); // Предположим начальную температуру 25°C
    combined_filter_update(&combined_air, 25.0f);
  }
  mode = 0;
  Update_LEDs(mode, air_temp, 0);

  MX_IWDG_Init();
  // HAL_IWDG_Refresh(&hiwdg);
  IWDG->KR = 0xAAAA; // Рефреш

  uint32_t code = LoadErrorCode();

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
    
    for (int i = 0; i < 3; i++)
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

    ClearErrorCode();
    HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    if (HAL_GPIO_ReadPin(FAN_GPIO_Port, FAN_Pin) == GPIO_PIN_RESET)
    {
      if (heater_temp > FAN_ON_TEMP || air_temp > FAN_ON_TEMP)
      {
        HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_SET);
      }
    }
    else
    {
      if (heater_temp < FAN_OFF_TEMP && air_temp < FAN_OFF_TEMP)
      {
        HAL_GPIO_WritePin(FAN_GPIO_Port, FAN_Pin, GPIO_PIN_RESET);
      }
    }

    adc_value1 = (float)adc_dma_buffer[0] / 4095.0f;

    adc_value2 = (float)adc_dma_buffer[1] / 4095.0f;

    // Обновление комбинированных фильтров
    float filtered_air = combined_filter_update(&combined_air, adc_value1);
    float filtered_heater = combined_filter_update(&combined_heater, adc_value2);

    air_temp     = thermistor_calc_temp(&therm2, filtered_air);    // датчик воздуха
    heater_temp  = thermistor_calc_temp(&therm1, filtered_heater);  // датчик нагревателя

    uint32_t current_time = HAL_GetTick();

    if ((current_time - previous_time) >= PID_UPDATE_INTERVAL_MS && mode != 0) {
        previous_time = current_time;

        float now = current_time / 1000.0f;
        float dt = PID_UPDATE_INTERVAL_MS / 1000.0f;

        air_target = GetTemperatureByMode(mode);

        //Внешний PID: воздух -> температура нагревателя
        float air_error = air_target - air_temp;
        float heater_margin = heater_temp - air_temp;

        // Ограничение эффективности внешнего PID при холодном нагревателе
        if (heater_margin < 5.0f) {
            float scale = heater_margin / 5.0f;
            if (scale < 0.2f) scale = 0.2f;
            air_error *= scale;
        }

        // Дополнительное усиление на этапе прогрева
        if (air_error > 0.0f) {
            float boost = 1.0f + air_error / 10.0f;
            air_error *= boost;
        }

        // Финальная цель для PID
        float smart_setpoint = air_temp + air_error;

        // Умная логика принятия решения
        float temp_error = air_target - air_temp;

        if (air_temp > air_target + 0.1f)
        {
          // Сильно перегрет — сбрасываем
          heater_setpoint = 0.0f;
        }
        else
        {
          // Основной PID — всегда, пока не перегрет
          heater_setpoint = PID_Compute(&pid_air, air_temp, smart_setpoint, now);

          float min_setpoint = air_target + 10.0f;
          if (air_temp < air_target - 5.0f && heater_setpoint < min_setpoint)
            heater_setpoint = min_setpoint;

          heater_setpoint = clamp(heater_setpoint, 30.0f, MAX_TEMP);
        }

        // Внутренний PID: нагреватель -> PWM
        pwm = PID_Compute(&pid_heater, heater_temp, heater_setpoint, now);

        // Установка PWM
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

    if (heater_temp >= MAX_TEMP)
    {
      SaveErrorCode(ERROR_OVERHEAT_HEATER);
      NVIC_SystemReset();
    }

    if (!heater_fault) {
        if (pwm > HEATER_MIN_PWM) {
            if (!heater_warming_up) {
                // Засекаем момент начала нагрева
                heater_start_time = HAL_GetTick();
                heater_start_temp = heater_temp;
                heater_warming_up = true;
            } else {
                // Уже греем — проверим прогресс
                if (heater_temp > heater_start_temp + HEATER_MIN_TEMP_DELTA) {
                    // Температура начала расти — всё норм
                    heater_warming_up = false;
                } else if (HAL_GetTick() - heater_start_time > HEATER_RESPONSE_TIMEOUT_MS) {
                    // Время вышло, а температура не изменилась — ошибка
                    heater_fault = true;
                    heater_warming_up = false;
                }
            }
        } else {
            // PWM упал — сбрасываем ожидание
            heater_warming_up = false;
        }
    }

    if (heater_fault)
    {
      SaveErrorCode(ERROR_FAULT_HEATER_NORESP);
      NVIC_SystemReset();  
    }

    if (!air_fault && mode != 0)
    {
      // Если температура уже достигнута — не отслеживаем
      if (air_temp >= air_target - 5.0f)
      {
        air_warming_up = false;
      }
      else if (!air_warming_up)
      {
        // Начинаем отслеживание
        air_start_time = HAL_GetTick();
        air_start_temp = air_temp;
        air_warming_up = true;
      }
      else
      {
        // Уже отслеживаем — проверим прогресс
        if (air_temp > air_start_temp + AIR_MIN_TEMP_DELTA)
        {
          // Нагрев пошёл — всё хорошо
          air_warming_up = false;
        }
        else if (HAL_GetTick() - air_start_time > AIR_RESPONSE_TIMEOUT_MS)
        {
          // Время вышло — ошибка
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
        } else {
            if (!long_press_handled && (now - button_press_time > 2000)) {
                mode = 0; // Долгое удержание — сброс
                long_press_handled = true;
                __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
            }
        }
    }
    else // кнопка отпущена
    {
        if (button_was_pressed) {
            if (!long_press_handled && (now - button_press_time < 2000)) {
                // Короткое нажатие
                mode = (mode + 1) % 8;
            }
            // Сброс флагов
            button_was_pressed = false;
            long_press_handled = false;
            HAL_Delay(300); // антидребезг
        }
    }

    Update_LEDs(mode, air_temp, 500);

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
  // RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSI14 | RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  // Проверка, что LSI запустился
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
  {
    Error_Handler(); // LSI не стартанул
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
   // Рассчитываем параметры для таймера
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
    bool at_target = current_temp >= (target_temp - 5.0f); // допуск, например 5C

    uint32_t tick = HAL_GetTick();

    // Моргаем, если температура еще не достигнута
    if (!at_target) {
        if (tick  - last_toggle_time >= speed) { // 2 Гц = каждые 500 мс, т.е. toggle каждые 250 мс
            blink_state = !blink_state;
            last_toggle_time = tick;
        }
    } else {
        blink_state = true; // зажечь стабильно
        last_toggle_time = tick;
    }

    // Отобразить состояние на светодиодах
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, ((mode & 0x01) && blink_state) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, ((mode & 0x02) && blink_state) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, ((mode & 0x04) && blink_state) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


// Функция для получения температуры по текущему режиму
float GetTemperatureByMode(uint8_t mode) {
    uint8_t max_modes = sizeof(mode_temperatures) / sizeof(mode_temperatures[0]);
    if (mode < max_modes) {
        return mode_temperatures[mode]; // Возвращаем значение из массива
    } else {
        return 0.0f; // Защита от выхода за пределы массива
    }
}


void SetHeaterPWM(float pwm_percent)
{
    // pwm_percent2 = clamp(pwm_percent, 0.0f, 100.0f);
    pwm_percent2 = pwm_percent;

    // Получить значение ARR (Period)
    period = __HAL_TIM_GET_AUTORELOAD(&htim2);

    // Рассчитать значение сравнения
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
    IWDG->KR = 0xCCCC;  // Enable
    IWDG->KR = 0x5555;  // Allow write
    IWDG->PR = 0x03;    // Prescaler: 64
    IWDG->RLR = 2000;   // Reload value
    while (IWDG->SR != 0) {} // Ждём готовности
    IWDG->KR = 0xAAAA;  // Первый сброс
}


void SaveErrorCode(uint32_t code)
{
    HAL_FLASH_Unlock();

    // Стереть страницу
    FLASH_EraseInitTypeDef eraseInit;
    uint32_t pageError = 0;

    eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = ERROR_FLASH_ADDRESS;
    eraseInit.NbPages = 1;

    if (HAL_FLASHEx_Erase(&eraseInit, &pageError) != HAL_OK) {
        HAL_FLASH_Lock();
        return; // ошибка стирания
    }

    // Записываем  4 байта
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, ERROR_FLASH_ADDRESS, code) != HAL_OK) {
        HAL_FLASH_Lock();
        return; // ошибка записи
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
