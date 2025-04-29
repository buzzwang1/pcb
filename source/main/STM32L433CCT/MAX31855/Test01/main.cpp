
#include "main.h"



// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU, (80 MHz max)
// Rom 256KB
// Ram 64KB

// SPI1
// PB2: CS
// PB3: SPI1 CLK:  AF5
// PB4: SPI1 MISO: AF5


__IO uint32_t TimingDelay = 0;

cClockInfo    mcClockInfo;
LED<GPIOC_BASE, 13> lcLedRed;
tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE, 10> mcComPort(38400, GPIO_AF7_USART2, 2048, 2048);

tcGpPin<GPIOB_BASE, 3> mSpi1_SCK(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
tcGpPin<GPIOB_BASE, 4> mSpi1_MISO(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
tcGpPin<GPIOB_BASE, 5> mSpi1_CS(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);

// -------------------------- SPI ----------------------------------


class cMax31855
{
public:
  typedef struct
  {
    u32 Open : 1;
    u32 ShortGnd : 1;
    u32 ShortVss : 1;
    u32 Res2 : 1;
    u32 IntTempVal : 12;
    u32 Fault : 1;
    u32 Res1 : 1;
    u32 ExtTempVal : 14;
  }tstMax31855;

  typedef union
  {
    u32         u32Data;
    tstMax31855 stData;
  }tunMax31855;

  tunMax31855 munMax31855;

  float mfExtTemp;
  float mfIntTemp;

  void vDecode(u32 lu32DigiValue)
  {
    munMax31855.u32Data = lu32DigiValue;

    i16 li16ZwErg = (munMax31855.stData.ExtTempVal << 4);
    mfExtTemp = (li16ZwErg >> 4) /  4.0f;

    li16ZwErg = (munMax31855.stData.IntTempVal << 6);
    mfIntTemp = (li16ZwErg >> 6) / 16.0f;
  }
};


cMax31855 mcMax31855;


inline void vWaitBussy()
{
  while (SPI1->SR & SPI_FLAG_BSY);
}

inline void vWaitTxFifo()
{
  while (SPI1->SR & SPI_FLAG_FTLVL);
}


inline void vClearRxBuf()
{
  // FIFO Buffer is 4 Byte
  volatile u8 lu8Dummy;
  lu8Dummy = *(__IO uint8_t*) ((uint32_t)SPI1 + 0x0C);
  lu8Dummy = *(__IO uint8_t*) ((uint32_t)SPI1 + 0x0C);
  lu8Dummy = *(__IO uint8_t*) ((uint32_t)SPI1 + 0x0C);
  lu8Dummy = *(__IO uint8_t*) ((uint32_t)SPI1 + 0x0C);
  UNUSED(lu8Dummy);
}
/*
u32 u32Read_Max31855()
{
  u32 lu32Data;

  vClearRxBuf();
  vClearRxBuf();

  mSpi1_CS.vSet0();

  while ((SPI1->SR & SPI_FLAG_TXE) == 0); // TX Fifi Empty (Front Test)

  // Put 4 Byte into the fifo
  SPI1->DR = 0x55; vWaitBussy(); lu32Data  = SPI1->DR; lu32Data <<= 16;
  SPI1->DR = 0x55; vWaitBussy(); lu32Data += SPI1->DR; //lu32Data <<= 16;

  //SPI1->DR = 0x55;
  //SPI1->DR = 0x55;



  //lu32Data += SPI1->DR;
  //lu32Data <<= 8;
  //lu32Data += SPI1->DR;
  //lu32Data <<= 8;
  //lu32Data += SPI1->DR;

  mSpi1_CS.vSet1();

  return lu32Data;
}*/


u32 u32Read_Max31855()
{
  volatile u8  lu8Dummy = 0x55;
  volatile u8  lu8Data[4];
  volatile u32 lu32Data = 0x0;

  mSpi1_CS.vSet0();

  vClearRxBuf();

  DMA1_Channel3->CNDTR = 4;
  DMA1_Channel3->CMAR = (uint32)(&lu8Dummy);
  DMA1_Channel3->CCR &= (0xFFFF & (~DMA_CCR_MINC)); // No Memory Inc

  DMA1_Channel2->CNDTR = 4;
  DMA1_Channel2->CMAR = (uint32)(lu8Data);

  DMA1_Channel2->CCR |= (DMA_CCR_EN/* | DMA_CCR_TCIE*/);
  DMA1_Channel3->CCR |= DMA_CCR_EN;

  while (!(DMA1->ISR & DMA_ISR_TCIF2));

  mSpi1_CS.vSet1();

  DMA1_Channel2->CCR &= ~15;
  DMA1_Channel3->CCR &= ~15;
  DMA1->IFCR = (DMA_ISR_GIF2 | DMA_ISR_HTIF2 | DMA_ISR_TCIF2 | DMA_ISR_GIF3 | DMA_ISR_HTIF3 | DMA_ISR_TCIF3);


  lu32Data = (lu8Data[0] << 24) + (lu8Data[1] << 16) + (lu8Data[2] << 8)+ lu8Data[3];

  return lu32Data;
}



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


void Delay(__IO uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  HAL_IncTick();
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


void USART1_IRQHandler(void)
{
  lcLedRed.Toggle();
  //mcComPort.vIRQHandler();
}

void USART2_IRQHandler(void)
{
  lcLedRed.Toggle();
  //mcComPort.vIRQHandler();
}


void MAIN_vTick1msHp(void)
{
}

void MAIN_vTick1msLp(void)
{
}

void MAIN_vTick10msLp(void)
{
}


void MAIN_vTick100msLp(void)
{

}


void MAIN_vTick1000msLp(void)
{
  mcMax31855.vDecode(u32Read_Max31855());
  lcLedRed.Toggle();
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

  mSpi1_SCK.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);
  mSpi1_MISO.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI1);

  SPI_HandleTypeDef lhSpi = {};

  __HAL_RCC_SPI1_CLK_ENABLE();

  /* SD_SPI Config */
  lhSpi.Instance = SPI1;
  lhSpi.Init.Mode = SPI_MODE_MASTER;
  lhSpi.Init.Direction = SPI_DIRECTION_2LINES; // SPI_DIRECTION_1LINE;
  lhSpi.Init.DataSize = SPI_DATASIZE_8BIT;
  lhSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
  lhSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
  lhSpi.Init.NSS = SPI_NSS_SOFT;

  switch (cClockInfo::mstClocks.HCLK_Frequency)
  {
  case 80000000: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; break;     //80Mhz   /  16 =>   5.0 Mhz
  case 16000000: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;  break;     //16Mhz   /   8 =>   2.0 Mhz
  default: lhSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; break;
  }

  lhSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
  lhSpi.Init.TIMode = SPI_TIMODE_DISABLE;
  lhSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  lhSpi.Init.CRCPolynomial = 7;
  lhSpi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  lhSpi.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&lhSpi) != HAL_OK)
  {
    //Error_Handler();
  }

  SPI1->CR1 |= (SPI_CR1_SPE);
  SPI1->CR2 |= (SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);


  //----------------------------------------------------------
  DMA_HandleTypeDef lhDma = {};

  __HAL_RCC_DMA1_CLK_ENABLE();

  lhDma.Instance = DMA1_Channel2;

  /* Configure DMA Channel data length */
  lhDma.Instance->CNDTR = 1;
  /* Configure DMA Channel destination address */
  lhDma.Instance->CPAR = (uint32_t) & (SPI1->DR);
  /* Configure DMA Channel source address */
  lhDma.Instance->CMAR = 0;

  lhDma.Init.Request = DMA_REQUEST_1;
  lhDma.Init.Direction = DMA_PERIPH_TO_MEMORY;
  lhDma.Init.PeriphInc = DMA_PINC_DISABLE;
  lhDma.Init.MemInc = DMA_MINC_ENABLE;
  lhDma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  lhDma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  lhDma.Init.Mode = DMA_NORMAL;
  lhDma.Init.Priority = DMA_PRIORITY_LOW;

  if (HAL_DMA_Init(&lhDma) != HAL_OK)
  {
    //Error_Handler();
  }
  __HAL_DMA_ENABLE_IT(&lhDma, (DMA_IT_TC /*| DMA_IT_HT | DMA_IT_TE*/));

  lhDma.Instance = DMA1_Channel3;
  /* Configure DMA Channel data length */
  lhDma.Instance->CNDTR = 1;
  /* Configure DMA Channel destination address */
  lhDma.Instance->CPAR = (uint32_t) & (SPI1->DR);
  /* Configure DMA Channel source address */
  lhDma.Instance->CMAR = 0;
  lhDma.Init.Direction = DMA_MEMORY_TO_PERIPH;
  if (HAL_DMA_Init(&lhDma) != HAL_OK)
  {
    //Error_Handler();
  }


  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp  /*1ms_LP*/,
                MAIN_vTick10msLp   /*10ms_LP*/,
                MAIN_vTick100msLp /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);
}



int main(void)
{
  MAIN_vInitSystem();

  //mcComPort.vSend("Start\n\r");

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config(void)
{
  // SystemClock = HSI (== 16Mhz)
  // kein Pll

  RCC_OscInitTypeDef RCC_OscInitStruct = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {};

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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = //RCC_PERIPHCLK_USART3| // Wird nicht benötigt
    RCC_PERIPHCLK_USART2 |
    RCC_PERIPHCLK_USART1 |
    //RCC_PERIPHCLK_I2C3|   // Wird nicht benötigt
    RCC_PERIPHCLK_I2C2 |
    RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  //PeriphClkInit.Usart3ClockSelection = RCC_USART2CLKSOURCE_HSI; // Wird nicht benötigt
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  //PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_HSI; // Wird nicht benötigt
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  HAL_Init();
  SystemClock_Config(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

