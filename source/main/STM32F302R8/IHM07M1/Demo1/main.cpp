/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "adc.h"
#include "gpio.h"

/*
Nucleo - 64 Boards
================

The Nucleo - F302R8 is a member of the Nucleo - 64 board family.The Nucleo - 64
is a standard board for use with several STM32 parts in the LQFP64 package.
Variants include

Order code    Targeted STM32
------------ - --------------
NUCLEO - F030R8 STM32F030R8T6
NUCLEO - F070RB STM32F070RBT6
NUCLEO - F072RB STM32F072RBT6
NUCLEO - F091RC STM32F091RCT6
NUCLEO - F103RB STM32F103RBT6
NUCLEO - F302R8 STM32F302R8T6
NUCLEO - F303RE STM32F303RET6
NUCLEO - F334R8 STM32F334R8T6
NUCLEO - F401RE STM32F401RET6
NUCLEO - F410RB STM32F410RBT6
NUCLEO - F411RE STM32F411RET6
NUCLEO - F446RE STM32F446RET6
NUCLEO - L053R8 STM32L053R8T6
NUCLEO - L073RZ STM32L073RZT6
NUCLEO - L152RE STM32L152RET6
NUCLEO - L452RE STM32L452RET6
NUCLEO - L476RG STM32L476RGT6

Configurations
==============

ihm07m1_f32 and ihm07m1_b16:
----------------------------

These examples are dedicated for the X - NUCLEO - IHM07M1 expansion board
based on L6230 DMOS driver for three - phase brushless DC motors.

X - NUCLEO - IHM07M1 must be configured to work with FOC and 3 - shunt
resistors.See ST documentation for details.

Pin configuration for the X - NUCLEO - IHM07M1(TIM1 configuration) :

  Board Function   Chip Function      Chip Pin Number
  ------------ - ----------------   ---------------- -
  Phase U high     TIM1_CH1           PA8
  Phase U enable   GPIO_PC10          PC10
  Phase V high     TIM1_CH2           PA9
  Phase V enable   GPIO_PC11          PC11
  Phase W high     TIM1_CH3           PA10
  Phase W enable   GPIO_PC12          PC12
  DIAG / EN        GPIO_PA11          PA11
  Current U        ADC1_IN1           PA0
  Current V        ADC1_IN7           PC1
  Current W        ADC1_IN6           PC0
  Temperature      ADC1_IN8           PC2
  VBUS             ADC1_IN2           PA1
  BEMF1(NU)        GPIO               PC3
  BEMF2(NU)        GPIO               PB0
  BEMF3(NU)        GPIO               PA7
  LED              GPIO_PB2           PB2
  + 3V3(CN7_16)
  GND(CN7_20)
  GPIO_BEMF(NU)    GPIO               PC9
  ENCO_A / HALL_H1 TIM2_CH1           PA15
  ENCO_B / HALL_H2 TIM2_CH2           PB3
  ENCO_Z / HALL_H3 TIM2_CH3           PB10
  GPIO1(NU)        GPIO               PB13
  GPIO2(NU)        GPIO               PB5
  GPIO3(NU)        GPIO               PA5
  CPOUT(NU)        GPIO               PA12
  BKIN1(NU)        GPIO               PB14
  POT              ADC1_IN12          PB1
  CURR_REF(NU)     GPIO               PB4
  DAC              DAC1_CH1           PA4
  DEBUG0           GPIO               PB8
  DEBUG1           GPIO               PB9
  DEBUG2           GPIO               PC6
  DEBUG3           GPIO               PC5
  DEBUG4           GPIO               PC8

  Current shunt resistance = 0.33
  Current sense gain = -1.53 (inverted current)
  Vbus sense gain = 9.31k / (9.31k + 169k) = 0.0522124390107
  Vbus min = 8V
  Vbus max = 48V
  Iout max = 1.4A RMS

  IPHASE_RATIO = 1 / (R_shunt * gain) = -1.98
  VBUS_RATIO = 1 / VBUS_gain = 19.152

  For now only 3 - shunt resistors configuration is supported. */


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */
uint16_t duty=0;
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
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  uint8_t h1,h2,h3;
  uint8_t hall_state=0;
  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK) {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
  if(HAL_ADC_Start(&hadc1) != HAL_OK){
    Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if( HAL_ADC_PollForConversion(&hadc1, 1000) == HAL_OK )
    {
      duty = HAL_ADC_GetValue(&hadc1)*999/4095;
    }
    h1=HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15);
    h2=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);
    h3=HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10);
    hall_state = ((h1<<2)|(h2<<1)|h3);
    switch (hall_state) {
    case 1://U>W
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,duty);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
      break;
    case 2://V>U
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,duty);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
      break;
    case 3://V>W
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,duty);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
      break;
    case 4:
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,duty);
      break;
    case 5:
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_RESET);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,duty);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,0);
      break;
    case 6:
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET);
      HAL_GPIO_WritePin(GPIOC,GPIO_PIN_12,GPIO_PIN_SET);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,0);
      __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,duty);
      break;
    }
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
