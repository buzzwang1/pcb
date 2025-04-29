
#include "main.h"



// STM32L433
// ARM®-based Cortex®-M7 32b MCU, (280 MHz max)
// Rom 128KB
// Ram 1024KB


LED<GPIOE_BASE, 3> lcLedRed;

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}

extern PCD_HandleTypeDef hpcd_USB_OTG_HS;

void OTG_HS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_HS_IRQn 0 */

  /* USER CODE END OTG_HS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_HS);
  /* USER CODE BEGIN OTG_HS_IRQn 1 */

  /* USER CODE END OTG_HS_IRQn 1 */
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PE3_GPIO_Port, PE3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LCD_CS_Pin | LCD_WR_RS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(F_CS_GPIO_Port, F_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : PEPin PEPin PEPin */
  GPIO_InitStruct.Pin = PE3_Pin | LCD_CS_Pin | LCD_WR_RS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = KEY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(KEY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = F_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(F_CS_GPIO_Port, &GPIO_InitStruct);
}

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    //Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x12;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    //Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
  sDate.Month = RTC_MONTH_JUNE;
  sDate.Date = 0x1;
  sDate.Year = 0x22;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  if (rtcHandle->Instance == RTC)
  {
    /* USER CODE BEGIN RTC_MspInit 0 */

    /* USER CODE END RTC_MspInit 0 */

    /** Initializes the peripherals clock
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      //Error_Handler();
    }

    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
    /* USER CODE BEGIN RTC_MspInit 1 */

    /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if (rtcHandle->Instance == RTC)
  {
    /* USER CODE BEGIN RTC_MspDeInit 0 */

    /* USER CODE END RTC_MspDeInit 0 */
      /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
    /* USER CODE BEGIN RTC_MspDeInit 1 */

    /* USER CODE END RTC_MspDeInit 1 */
  }
}


SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi4;

/* SPI1 init function */
void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}
/* SPI4 init function */
void MX_SPI4_Init(void)
{

  /* USER CODE BEGIN SPI4_Init 0 */

  /* USER CODE END SPI4_Init 0 */

  /* USER CODE BEGIN SPI4_Init 1 */

  /* USER CODE END SPI4_Init 1 */
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_1LINE;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 0x0;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN SPI4_Init 2 */

  /* USER CODE END SPI4_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  if (spiHandle->Instance == SPI1)
  {
    /* USER CODE BEGIN SPI1_MspInit 0 */

    /* USER CODE END SPI1_MspInit 0 */

    /** Initializes the peripherals clock
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      //Error_Handler();
    }

    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PD7     ------> SPI1_MOSI
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN SPI1_MspInit 1 */

    /* USER CODE END SPI1_MspInit 1 */
  }
  else if (spiHandle->Instance == SPI4)
  {
    /* USER CODE BEGIN SPI4_MspInit 0 */

    /* USER CODE END SPI4_MspInit 0 */

    /** Initializes the peripherals clock
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI4;
    PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      //Error_Handler();
    }

    /* SPI4 clock enable */
    __HAL_RCC_SPI4_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI4 GPIO Configuration
    PE12     ------> SPI4_SCK
    PE14     ------> SPI4_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* USER CODE BEGIN SPI4_MspInit 1 */

    /* USER CODE END SPI4_MspInit 1 */
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if (spiHandle->Instance == SPI1)
  {
    /* USER CODE BEGIN SPI1_MspDeInit 0 */

    /* USER CODE END SPI1_MspDeInit 0 */
      /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PD7     ------> SPI1_MOSI
    PB3     ------> SPI1_SCK
    PB4     ------> SPI1_MISO
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3 | GPIO_PIN_4);

    /* USER CODE BEGIN SPI1_MspDeInit 1 */

    /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if (spiHandle->Instance == SPI4)
  {
    /* USER CODE BEGIN SPI4_MspDeInit 0 */

    /* USER CODE END SPI4_MspDeInit 0 */
      /* Peripheral clock disable */
    __HAL_RCC_SPI4_CLK_DISABLE();

    /**SPI4 GPIO Configuration
    PE12     ------> SPI4_SCK
    PE14     ------> SPI4_MOSI
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_12 | GPIO_PIN_14);

    /* USER CODE BEGIN SPI4_MspDeInit 1 */

    /* USER CODE END SPI4_MspDeInit 1 */
  }
}


TIM_HandleTypeDef htim1;

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef* tim_pwmHandle)
{

  if (tim_pwmHandle->Instance == TIM1)
  {
    /* USER CODE BEGIN TIM1_MspInit 0 */

    /* USER CODE END TIM1_MspInit 0 */
      /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
    /* USER CODE BEGIN TIM1_MspInit 1 */

    /* USER CODE END TIM1_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  if (timHandle->Instance == TIM1)
  {
    /* USER CODE BEGIN TIM1_MspPostInit 0 */

    /* USER CODE END TIM1_MspPostInit 0 */

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PE10     ------> TIM1_CH2N
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* USER CODE BEGIN TIM1_MspPostInit 1 */

    /* USER CODE END TIM1_MspPostInit 1 */
  }
}


/* TIM1 init function */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 14 - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000 - 1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    //Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    //Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    //Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}



void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef* tim_pwmHandle)
{
  if (tim_pwmHandle->Instance == TIM1)
  {
    /* USER CODE BEGIN TIM1_MspDeInit 0 */

    /* USER CODE END TIM1_MspDeInit 0 */
      /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();
    /* USER CODE BEGIN TIM1_MspDeInit 1 */

    /* USER CODE END TIM1_MspDeInit 1 */
  }
}

OSPI_HandleTypeDef hospi1;
/* W25XXXÏµÁÐ */
#define W25X10_FLASH_ID            0x1130EF  // 1M bit
#define W25X20_FLASH_ID            0x1230EF  // 2M bit
#define W25X40_FLASH_ID            0x1330EF  // 4M bit
#define W25X80_FLASH_ID            0x1440EF  // 8M bit
#define W25Q16_FLASH_ID1           0x1530EF  // 16M bit
#define W25Q16_FLASH_ID2           0x1540EF  // 16M bit
#define W25Q32_FLASH_ID1           0x1640EF  // 32M bit
#define W25Q32_FLASH_ID2           0x1660BA  // 32M bit
#define W25Q64_FLASH_ID1           0x1740EF  // 64M bit
#define W25Q64_FLASH_ID2           0x1760EF  // 64M bit
#define W25Q128_FLASH_ID1          0x1840EF  // 128M bit
#define W25Q128_FLASH_ID2          0x1860EF  // 128M bit
#define W25Q256_FLASH_ID1          0x1940EF  // 256M bit
#define W25Q256_FLASH_ID2          0x1960EF  // 256M bit
#define SPI_FLASH_SectorSize       4096
volatile u32 mu32FlashID;
volatile u32 mu32FlashSize;
volatile u32 mu32FlashSectorCount;
volatile u32 mu32FlashSectorSize;
volatile u8  mu8FlashStatus[3];
volatile u8  mu8MyPage1[64] = "Hello world.";
volatile u8  mu8MyPage2[64];


int32_t OSPI_ReadSingleData(u8 lu8Instruction, u8* lpu8Dest, u8 lu8Size, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef s_command;

  s_command.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.Instruction = lu8Instruction;
  s_command.AddressMode = HAL_OSPI_ADDRESS_NONE;
  s_command.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = HAL_OSPI_DATA_1_LINE;
  s_command.DummyCycles = 0;
  s_command.NbData = lu8Size; // Width of useful ID
  s_command.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  /* Reception of the data */
  if (HAL_OSPI_Receive(hospi1, lpu8Dest, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }
  return 0;
}

int32_t OSPI_AutoPollingMemReady(OSPI_HandleTypeDef* hospi1, u8 lu8Match, u8 lu8Mask)
{
  OSPI_AutoPollingTypeDef sConfig = {0};
  OSPI_RegularCmdTypeDef s_command = {0};

  s_command.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
  s_command.FlashId = HAL_OSPI_FLASH_ID_1;
  s_command.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;
  s_command.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;
  s_command.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  s_command.Instruction = 0x5;
  s_command.AddressMode = HAL_OSPI_ADDRESS_NONE;
  s_command.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
  s_command.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  s_command.DataMode = HAL_OSPI_DATA_1_LINE;
  s_command.DummyCycles = 0;
  s_command.NbData = 1;
  s_command.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&s_command, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  // Bit S0: Busy
  // Bit S1: WEL Write Enable

  sConfig.Match           = lu8Match;
  sConfig.Mask            = lu8Mask;
  sConfig.MatchMode       = HAL_OSPI_MATCH_MODE_AND;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_OSPI_AutoPolling(hospi1, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  return 0;
}

static uint8_t OSPI_WriteEnable(OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef sCommand = { 0 };

  /* Enable write operations ------------------------------------------ */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction        = 06; // Write Enable
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles        = 0;

  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    __asm("NOP");
    return -1;
  }


  if (OSPI_AutoPollingMemReady(hospi1, 2, 2)) // Write Enable
  {
    __asm("NOP");
    return -1;
  }

  __asm("NOP");
  return 0;
}



uint8_t OSPI_Erase_Chip(OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef sCommand = { 0 };


  if (OSPI_WriteEnable(hospi1))
  {
    return -1;
  }

  /* Erasing Sequence --------------------------------- */
  sCommand.Instruction        = 0xC7;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;
  sCommand.DataMode    = HAL_OSPI_DATA_NONE;
  sCommand.DummyCycles = 0;

  if (HAL_OSPI_Command(hospi1, (OSPI_RegularCmdTypeDef*)&sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  if (OSPI_AutoPollingMemReady(hospi1, 0, 1) != HAL_OK)
  {
    return -1;
  }

  return 0;
}


// see page 84
int32_t OSPI_ResetMemory(OSPI_HandleTypeDef *hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;

  sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;      /*!< It indicates if the configuration applies to the common registers or
                                                                 to the registers for the write operation (these registers are only
                                                                 used for memory-mapped mode).
                                                                 This parameter can be a value of @ref OSPI_OperationType */
  sCommand.FlashId = HAL_OSPI_FLASH_ID_1;                   /*!< It indicates which external device is selected for this command (it
                                                                 applies only if Dualquad is disabled in the initialization structure).
                                                                 This parameter can be a value of @ref OSPI_FlashID */
  sCommand.Instruction = 0x66;                              /*!< It contains the instruction to be sent to the device.
                                                                 This parameter can be a value between 0 and 0xFFFFFFFF */
  sCommand.InstructionMode = HAL_OSPI_INSTRUCTION_1_LINE;   /*!< It indicates the mode of the instruction.
                                                                 This parameter can be a value of @ref OSPI_InstructionMode */
  sCommand.InstructionSize = HAL_OSPI_INSTRUCTION_8_BITS;   /*!< It indicates the size of the instruction.
                                                                 This parameter can be a value of @ref OSPI_InstructionSize */
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;  /*!< It enables or not the DTR mode for the instruction phase.
                                                                        This parameter can be a value of @ref OSPI_InstructionDtrMode */
  sCommand.Address = 0u;                                    /*!< It contains the address to be sent to the device.
                                                                 This parameter can be a value between 0 and 0xFFFFFFFF */
  sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;             /*!< It indicates the mode of the address.
                                                                 This parameter can be a value of @ref OSPI_AddressMode */
  sCommand.AddressSize = 0u;                                /*!< It indicates the size of the address.
                                                                 This parameter can be a value of @ref OSPI_AddressSize */
  sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;   /*!< It enables or not the DTR mode for the address phase.
                                                                 This parameter can be a value of @ref OSPI_AddressDtrMode */
  sCommand.AlternateBytes = 0u;                             /*!< It contains the alternate bytes to be sent to the device.
                                                                 This parameter can be a value between 0 and 0xFFFFFFFF */
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE; /*!< It indicates the mode of the alternate bytes.
                                                                 This parameter can be a value of @ref OSPI_AlternateBytesMode */
  sCommand.AlternateBytesSize = 0u;                         /*!< It indicates the size of the alternate bytes.
                                                                 This parameter can be a value of @ref OSPI_AlternateBytesSize */
  sCommand.AlternateBytesDtrMode = HAL_OSPI_ALTERNATE_BYTES_DTR_DISABLE; /*!< It enables or not the DTR mode for the alternate bytes phase.
                                                                 This parameter can be a value of @ref OSPI_AlternateBytesDtrMode */
  sCommand.DataMode = HAL_OSPI_DATA_NONE;                   /*!< It indicates the mode of the data.
                                                                 This parameter can be a value of @ref OSPI_DataMode */
  sCommand.NbData = 0u;                                     /*!< It indicates the number of data transferred with this command.
                                                                 This field is only used for indirect mode.
                                                                 This parameter can be a value between 1 and 0xFFFFFFFF */
  sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;         /*!< It enables or not the DTR mode for the data phase.
                                                                 This parameter can be a value of @ref OSPI_DataDtrMode */
  sCommand.DummyCycles = 0u;                                /*!< It indicates the number of dummy cycles inserted before data phase.
                                                                 This parameter can be a value between 0 and 31 */
  sCommand.DQSMode = HAL_OSPI_DQS_DISABLE;                  /*!< It enables or not the data strobe management.
                                                                 This parameter can be a value of @ref OSPI_DQSMode */
  sCommand.SIOOMode = HAL_OSPI_SIOO_INST_EVERY_CMD;         /*!< It enables or not the SIOO mode.
                                                                 This parameter can be a value of @ref OSPI_SIOOMode */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
    return 1;
  }

  sCommand.Instruction = 0x99;
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
    return 1;
  }

  // Wait at least tRST: 30us
  for (uint32_t temp = 0; temp < 500000; temp++)
  {
    __NOP();
  }


  if (OSPI_AutoPollingMemReady(hospi1, 0, 0x01))     // Write in progress
  {
    //Error_Handler();
    __asm("NOP");
  }

  // Read Status Register-1
  if (OSPI_ReadSingleData(0x05, (u8*)&mu8FlashStatus[0], 1, hospi1))
  {
    //Error_Handler();
    __asm("NOP");
  }

  // Read Status Register-2
  if (OSPI_ReadSingleData(0x35, (u8*)&mu8FlashStatus[1], 1, hospi1))
  {
    //Error_Handler();
    __asm("NOP");
  }

  // Read Status Register-3
  if (OSPI_ReadSingleData(0x15, (u8*)&mu8FlashStatus[2], 1, hospi1))
  {
    //Error_Handler();
    __asm("NOP");
  }

  return 0;
}


int32_t OSPI_ReadId(OSPI_HandleTypeDef* hospi1)
{
  if (OSPI_ReadSingleData(0x9F, (u8*)&mu32FlashID, 3, hospi1))
  {
    //Error_Handler();
    __asm("NOP");
  }

  mu32FlashSize        = 0;
  mu32FlashSectorCount = 0x00;
  mu32FlashSectorSize  = 0x00;

  switch(mu32FlashID)
  {
    /* W25XXXÏµÁÐÐ¾Æ¬ */
    case W25X10_FLASH_ID:    /* 0xEF3011-----1M bit */
      mu32FlashSize = 1;
      break;

    case W25X20_FLASH_ID:    /* 0xEF3012-----2M bit */
      mu32FlashSize = 2;
      break;

    case W25X40_FLASH_ID:    /* 0xEF3013-----4M bit */
      mu32FlashSize = 4;
      break;

    case W25X80_FLASH_ID:    /* 0xEF4014-----8M bit */
      mu32FlashSize = 8;
      break;

    case W25Q16_FLASH_ID1:   /* 0xEF3015-----16M bit */
    case W25Q16_FLASH_ID2:   /* 0xEF4015-----16M bit */
      mu32FlashSize = 16;
      break;

    case W25Q32_FLASH_ID1:   /* 0xEF4016-----32M bit */
    case W25Q32_FLASH_ID2:   /* 0xEF6016-----32M bit */
      mu32FlashSize = 32;
      break;

    case W25Q64_FLASH_ID1:   /* 0xEF4017-----64M bit */
    case W25Q64_FLASH_ID2:   /* 0xEF6017-----64M bit */
      mu32FlashSize = 64;
      break;

    case W25Q128_FLASH_ID1:  /* 0xEF4018-----128M bit */
    case W25Q128_FLASH_ID2:  /* 0xEF6018-----128M bit */
      mu32FlashSize = 128;
      break;

    case W25Q256_FLASH_ID1:  /* 0xEF4019-----256M bit */
    case W25Q256_FLASH_ID2:  /* 0xEF6019-----256M bit */
      mu32FlashSize = 256;
      break;
    default:
      if( (mu32FlashID != 0xFFFFFFFF ) || (mu32FlashID != 0x00000000 ) )
      {
        mu32FlashSize = 16;
      }
      else
      {
        mu32FlashSize = 0x00;
      }
      break;
  }
  mu32FlashSize = ( (uint32_t)mu32FlashSize * 1024 ) * ( (uint32_t)1024 / 8 );

  if (mu32FlashSize)
  {
    mu32FlashSectorCount = mu32FlashSize / SPI_FLASH_SectorSize;
    mu32FlashSectorSize  = SPI_FLASH_SectorSize;
  }

  return 0;
}


int32_t OSPI_SetupQuadSpi(OSPI_HandleTypeDef* hospi1)
{
  UNUSED(hospi1);

  // The Quad Enable(QE) bit is a non - volatile read / write bit in the status register (S9)that allows Quad SPI
  // and QPI operation.When the QE bit is set to a 0 state(factory default for part numbers with ordering options
  // “IM”& “JM”), the / WP pin and / HOLD are enabled.When the QE bit is set to a 1(factory default for Quad
  //  Enabled part numbers with ordering option “IQ”& “JQ”), , the Quad IO2and IO3 pins are enabled, and / WP
  //  and / HOLD functions are disabled.
  if (mu8FlashStatus[1] & 2)
  {
    // Nothing todo, QE is already set
  }
  {
    // Set QE
    //// Todo
  }
  return 0;
}

int32_t OSPI_SetupDummyCycles(OSPI_HandleTypeDef* hospi1)
{
  UNUSED(hospi1);
  // NA for W25Q64JV

  // Fast Read(0Bh)
  //   The Fast Read instruction is similar to the Read Data instruction except that it can operate at the highest
  //   possible frequency of FR(see AC Electrical Characteristics).This is accomplished by adding eight “dummy”
  //   clocks after the 24 - bit address as shown in Figure 16. The dummy clocks allow the devices internal circuits
  //   additional time for setting up the initial address.During the dummy clocks the data value on the DO pin is
  //   a “don’t care”.

  // See C0h

  return 0;
}


int32_t OSPI_SetupQuadDDRMode(OSPI_HandleTypeDef* hospi1)
{
  UNUSED(hospi1);
  // NA for W25Q64JV
  // DTR not need to be activated. It is controlled via Cmd.
  // DTR Fast Read Quad: EDh:  24Bit Adr: 8Bit Dummy: nx 8 Bit Data
  return 0;
}



int32_t OSPI_EnableMemoryMappedMode(OSPI_HandleTypeDef* hospi1)
{
  OSPI_MemoryMappedTypeDef s_mem_mapped_cfg;
  OSPI_RegularCmdTypeDef sCommand = {0};

  /* Initialize memory-mapped mode for read operations */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_READ_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
  sCommand.DummyCycles        = 4;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
  sCommand.Instruction        = 0xED; //  DTR Fast Read Quad;
  //sCommand.Instruction        = 0x6B; //  Fast Read Quad Output;
  //sCommand.Instruction        = 0xEB; //  Fast Read Quad I/O;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_ENABLE;
  sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_ENABLE;
  sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_ENABLE;
  sCommand.DQSMode            = HAL_OSPI_DQS_ENABLE;

  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  //OSPI_MemoryMappedTypeDef sMemMappedCfg = { 0 };
  /* Initialize memory-mapped mode for write operations */
  /*sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
  sCommand.Instruction   = OCTAL_PAGE_PROG_CMD;
  sCommand.DummyCycles   = 0;
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) !=
    HAL_OK)
  {
    return -1;
  }*/

  /* Configure the memory mapped mode with TimeoutCounter Disabled*/
  s_mem_mapped_cfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE;
  s_mem_mapped_cfg.TimeOutPeriod     = 10;
  if (HAL_OSPI_MemoryMapped(hospi1, &s_mem_mapped_cfg) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
    return 1;
  }


  return 0;
}

int32_t OSPI_WriteMemory(uint8_t* buffer, uint32_t address, uint32_t buffer_size, OSPI_HandleTypeDef* hospi1)
{

  OSPI_RegularCmdTypeDef  sCommand;
  //uint32_t end_addr, current_size, current_addr;

  /* Calculation of the size between the write address and the end of the page */
  //current_addr = 0;

  //
  // while (current_addr <= address) {
  //   current_addr += 256;
  // }
  // current_size = current_addr - address;

  /* Check if the size of the data is less than the remaining place in the page */
  //if (current_size > buffer_size) {
  //  current_size = buffer_size;
  //}

  /* Initialize the adress variables */
  //current_addr = address;
  //end_addr = address + buffer_size;

  // The Write Enable instruction (Figure 5) sets the Write Enable Latch (WEL) bit in the Status Register to a 1.
  // The WEL bit must be set prior to every Page Program, Quad Page Program, Sector Erase, Block Erase,
  // Chip Erase, Write Status Registerand Erase / Program Security Registers instruction.The Write Enable
  // instruction is entered by driving / CS low, shifting the instruction code “06h” into the Data Input(DI) pin on
  // the rising edge of CLK, and then driving / CS high.
  if (OSPI_WriteEnable(hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
    return 1;
  }

  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
  //sCommand.Instruction        = 0x2;   // Singe Input Page Program
  sCommand.Instruction        = 0x32;   // Quad Input Page Program
                                        // 8Bit Instr (1 Line) + 24Bit Address (1 Lines) + Data (4 Lines)
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_1_LINE;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;
  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
  //sCommand.DataMode           = HAL_OSPI_DATA_1_LINE;
  sCommand.NbData             = buffer_size;
  sCommand.Address            = address;
  sCommand.DummyCycles        = 0;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    __asm("NOP");
    return -1;
  }

  if (HAL_OSPI_Transmit(hospi1, buffer, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    __asm("NOP");
    return -1;
  }

  if (OSPI_AutoPollingMemReady(hospi1, 0, 0x01))     // Write in progress
  {
    //Error_Handler();
    __asm("NOP");
  }
  __asm("NOP");
  return 0;
}


uint8_t OSPI_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size, OSPI_HandleTypeDef* hospi1)
{
  OSPI_RegularCmdTypeDef  sCommand;

  /* Initialize the read command */
  sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
  sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
  sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
  sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
  sCommand.Instruction        = 0xEB; // Fast Read Quad I/O => 8Bit Instr (1 Line) + 24Bit Address (4 Lines) + 8Bit Mode (4 Lines) + 4 Cycles Dummy + Data (4 Lines)
  sCommand.AddressMode        = HAL_OSPI_ADDRESS_4_LINES;
  sCommand.AddressSize        = HAL_OSPI_ADDRESS_24_BITS;
  sCommand.Address            = ReadAddr;
  sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_4_LINES;
  sCommand.AlternateBytes     = 0;
  sCommand.AlternateBytesSize = 1;
  sCommand.DataMode           = HAL_OSPI_DATA_4_LINES;
  sCommand.DummyCycles        = 4U;
  sCommand.NbData             = Size;
  sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

  /* Configure the command */
  if (HAL_OSPI_Command(hospi1, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  /* Set S# timing for Read command */
  //MODIFY_REG(hHAL_OSPI.Instance->DCR, QUADSPI_DCR_CSHT, HAL_OSPI_CS_HIGH_TIME_5_CYCLE);

  if (HAL_OSPI_Receive(hospi1, pData, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    return -1;
  }

  /* Restore S# timing for nonRead commands */
  //MODIFY_REG(hHAL_OSPI.Instance->DCR, QUADSPI_DCR_CSHT, HAL_OSPI_CS_HIGH_TIME_6_CYCLE);

  return 0;
}


/* USER CODE END OCTOSPI1_Init 2 */


/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{

  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  OSPIM_CfgTypeDef sOspiManagerCfg;

  // STM32H7 Core clock: 280Mhz
  //         OSPI clock: 140Mhz
  //
  // HAL_OSPI WinBond W25Q64JV-DTR 8MB = 2^23 = 8388608
  // Mircon Mode, weil Daten komment als u8 (D0 D1 D2 D3) und nicht als u16 (D1 D0 D3 D2) raus
  //
  //

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold         = 1;
  hospi1.Init.DualQuad              = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType            = HAL_OSPI_MEMTYPE_MICRON;
  hospi1.Init.DeviceSize            = 23;   // 8MB = 2^23 = 8388608
  //hospi1.Init.ChipSelectHighTime    = 4;    // t_SHSL2 = 50ns (DS S.89), 140Mhz / 2(Prescale) = 70Mhz    => 14.3ns; 14.3ns * 4 = 57ns
  //hospi1.Init.ChipSelectHighTime    = 3;    // t_SHSL2 = 50ns (DS S.89), 140Mhz / 3(Prescale) = 46,67Mhz => 21.4ns; 21.4ns * 3 = 64ns
  hospi1.Init.ChipSelectHighTime    = 3;    // t_SHSL2 = 50ns (DS S.89), 140Mhz / 4(Prescale) = 35Mhz    => 28.6ns; 28.6ns * 2 = 57ns
  hospi1.Init.FreeRunningClock      = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode             = HAL_OSPI_CLOCK_MODE_0; // Mode 0 = Low Level; Mode 3 = High Level; W25Q64JV supports both (DS S.10)
  hospi1.Init.WrapSize              = HAL_OSPI_WRAP_NOT_SUPPORTED;
  //hospi1.Init.ClockPrescaler        = 2;    // 140Mhz / 2 = 70Mhz
  //hospi1.Init.ClockPrescaler        = 3;    // 140Mhz / 3 = 46,67Mhz
  //hospi1.Init.ClockPrescaler        = 4;    // 140Mhz / 4 = 35Mhz
  hospi1.Init.ClockPrescaler = 140;    // 140Mhz / 140 = 1Mhz
  hospi1.Init.SampleShifting        = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE; // HAL_OSPI_DHQC_DISABLE;   HAL_OSPI_DHQC_ENABLE is recommended when using DTR-Mode
  hospi1.Init.ChipSelectBoundary    = 0;
  hospi1.Init.DelayBlockBypass      = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi1.Init.MaxTran               = 0;
  hospi1.Init.Refresh               = 0;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    asm("NOP");
  }
  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.NCSPort = 1;
  sOspiManagerCfg.IOLowPort  = HAL_OSPIM_IOPORT_1_LOW;
  sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  if (OSPI_ResetMemory(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }


  if (OSPI_ReadId(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }



  if (OSPI_SetupQuadSpi(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }





  if (OSPI_SetupDummyCycles(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }

  if (OSPI_SetupQuadDDRMode(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }


  //OSPI_Erase_Chip(&hospi1);
  //__asm("NOP");

  OSPI_WriteMemory((u8*)mu8MyPage1, 0, 64, &hospi1);
  __asm("NOP");


  OSPI_Read((u8*)mu8MyPage2, 0, 64, &hospi1);
  __asm("NOP");

  /*if (OSPI_EnableMemoryMappedMode(&hospi1) != HAL_OK)
  {
    //Error_Handler();
    __asm("NOP");
  }*/
  /* USER CODE END OCTOSPI1_Init 2 */

}

void HAL_OSPI_MspInit(OSPI_HandleTypeDef* ospiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
  if (ospiHandle->Instance == OCTOSPI1)
  {
    /* USER CODE BEGIN OCTOSPI1_MspInit 0 */

    /* USER CODE END OCTOSPI1_MspInit 0 */

    /** Initializes the peripherals clock
    */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
    PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_D1HCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      //Error_Handler();
    }

    /* OCTOSPI1 clock enable */
    __HAL_RCC_OCTOSPIM_CLK_ENABLE();
    __HAL_RCC_OSPI1_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**OCTOSPI1 GPIO Configuration
    PE2     ------> OCTOSPIM_P1_IO2
    PB2     ------> OCTOSPIM_P1_CLK
    PD11     ------> OCTOSPIM_P1_IO0
    PD12     ------> OCTOSPIM_P1_IO1
    PD13     ------> OCTOSPIM_P1_IO3
    PB6     ------> OCTOSPIM_P1_NCS
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN OCTOSPI1_MspInit 1 */

    /* USER CODE END OCTOSPI1_MspInit 1 */
  }
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef* ospiHandle)
{

  if (ospiHandle->Instance == OCTOSPI1)
  {
    /* USER CODE BEGIN OCTOSPI1_MspDeInit 0 */

    /* USER CODE END OCTOSPI1_MspDeInit 0 */
      /* Peripheral clock disable */
    __HAL_RCC_OCTOSPIM_CLK_DISABLE();
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PE2     ------> OCTOSPIM_P1_IO2
    PB2     ------> OCTOSPIM_P1_CLK
    PD11     ------> OCTOSPIM_P1_IO0
    PD12     ------> OCTOSPIM_P1_IO1
    PD13     ------> OCTOSPIM_P1_IO3
    PB6     ------> OCTOSPIM_P1_NCS
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_2);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2 | GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13);

    /* USER CODE BEGIN OCTOSPI1_MspDeInit 1 */

    /* USER CODE END OCTOSPI1_MspDeInit 1 */
  }
}




void MAIN_vTick1000msLp(void)
{
  //static u8 lu85s = 0;
  //
  //
  //if (lu85s == 5)
  //{
  //  lu85s = 0;
    lcLedRed.Toggle();
  //}
  //else
  //{
  //  lu85s++;
  //}
}


void MAIN_vInitSystem(void)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  MX_GPIO_Init();
  MX_RTC_Init();
  MX_SPI4_Init();
  MX_TIM1_Init();
  MX_OCTOSPI1_Init();
  MX_SPI1_Init();
  //MX_USB_DEVICE_Init();


  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                NULL               /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config_HSI_64MHZ(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                                       |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                                       |RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart16ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart234578ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c123ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}


void SystemClock_Config_HSE_240Mhz(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /** Supply configuration update enable
   */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  /** Configure LSE Drive Capability
   */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 112;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    //Error_Handler();
  }
}

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  SystemClock_Config_HSE_240Mhz();
  SystemCoreClockUpdate();
}

