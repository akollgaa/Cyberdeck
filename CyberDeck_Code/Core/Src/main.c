/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "ili9341.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define BUTTON_DEBOUNCE_MS 20U
#define BUTTON_COUNT       7U

#define RECTANGLE_X        40U
#define RECTANGLE_Y        40U
#define RECTANGLE_WIDTH    160U
#define RECTANGLE_HEIGHT   80U
#define RECTANGLE_ON_COLOR 0xF800U /* RGB565 red. */
#define RECTANGLE_OFF_COLOR 0x0000U /* RGB565 black. */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static const uint16_t button_pins[BUTTON_COUNT] = {
  GPIO_PIN_2,
  GPIO_PIN_3,
  GPIO_PIN_4,
  GPIO_PIN_5,
  GPIO_PIN_6,
  GPIO_PIN_7,
  GPIO_PIN_8
};
static GPIO_PinState button_stable_states[BUTTON_COUNT];
static GPIO_PinState button_last_samples[BUTTON_COUNT];
static uint32_t button_last_change_ticks[BUTTON_COUNT];
static uint8_t rectangle_on;

static const ILI9341_HandleTypeDef ili9341_display = {
  .data_port = {
    GPIOB, GPIOB, GPIOB, GPIOB,
    GPIOB, GPIOB, GPIOB, GPIOB
  },
  .data_pin = {
    GPIO_PIN_7,  /* D0 */
    GPIO_PIN_8,  /* D1 */
    GPIO_PIN_9,  /* D2 */
    GPIO_PIN_10, /* D3 */
    GPIO_PIN_12, /* D4 */
    GPIO_PIN_13, /* D5 */
    GPIO_PIN_14, /* D6 */
    GPIO_PIN_15  /* D7 */
  },
  .wr_port = GPIOB,
  .wr_pin = GPIO_PIN_4,
  .rd_port = GPIOB,
  .rd_pin = GPIO_PIN_5,
  .dc_port = GPIOB,
  .dc_pin = GPIO_PIN_3,
  .cs_port = GPIOB,
  .cs_pin = GPIO_PIN_1,
  .reset_port = GPIOB,
  .reset_pin = GPIO_PIN_6
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* Keep the read strobe inactive and deselect the display while idle. */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5,
                    GPIO_PIN_SET);
  ILI9341_Init(&ili9341_display);
  ILI9341_FillScreen(&ili9341_display, RECTANGLE_OFF_COLOR);

  uint32_t initial_tick = HAL_GetTick();

  for (uint32_t button_index = 0U; button_index < BUTTON_COUNT; button_index++)
  {
    button_stable_states[button_index] =
        HAL_GPIO_ReadPin(GPIOA, button_pins[button_index]);
    button_last_samples[button_index] = button_stable_states[button_index];
    button_last_change_ticks[button_index] = initial_tick;
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    uint32_t current_tick = HAL_GetTick();

    for (uint32_t button_index = 0U; button_index < BUTTON_COUNT; button_index++)
    {
      GPIO_PinState button_sample =
          HAL_GPIO_ReadPin(GPIOA, button_pins[button_index]);

      if (button_sample != button_last_samples[button_index])
      {
        button_last_samples[button_index] = button_sample;
        button_last_change_ticks[button_index] = current_tick;
      }
      else if ((button_sample != button_stable_states[button_index]) &&
               ((current_tick - button_last_change_ticks[button_index]) >=
                BUTTON_DEBOUNCE_MS))
      {
        button_stable_states[button_index] = button_sample;

        if (button_stable_states[button_index] == GPIO_PIN_SET)
        {
          HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);

          rectangle_on = (uint8_t)!rectangle_on;
          ILI9341_FillRect(&ili9341_display,
                           RECTANGLE_X,
                           RECTANGLE_Y,
                           RECTANGLE_WIDTH,
                           RECTANGLE_HEIGHT,
                           rectangle_on ? RECTANGLE_ON_COLOR
                                        : RECTANGLE_OFF_COLOR);
        }
      }
    }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

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
#ifdef USE_FULL_ASSERT
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
