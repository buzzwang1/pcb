
#include "main.h"



// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU, (80 MHz max)
// Rom 256KB
// Ram 64KB

//  PB09: Status Led

// I2C1: Temp, ACC, Gyro
//   PB06: I2C1 SCL
//   PB07: I2C1 SDA

//  U2:
//    PA08: U2 -- falsch Angesclossen ?
//    PA09: U2 Tx

// Analog:
//   PA0: ADC1_IN5: SensA
//   PA1: ADC1_IN5: SensB

//   PA4: DAC1: SensA
//   PA5: DAC1: SensB

//   PB0: FEN3
//   PB1: FEN2
//   PB2: FEN1

// Acc/Gyro
//   PB10: LRDY
//   PB11: LIN1
//   PB12: LIN2
//   PB13: GRDY
//   PB14: GINT


__IO uint32_t TimingDelay = 0;

cClockInfo    mcClockInfo;
LED<GPIOB_BASE, 9> lcLedRed;

//tcUart<USART1_BASE, GPIOA_BASE,  9, GPIOA_BASE, 10> mcComPort(38400, GPIO_AF7_USART2, 2048, 2048);

//tcGpPin<GPIOB_BASE, 3> mSpi1_SCK(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
//tcGpPin<GPIOB_BASE, 4> mSpi1_MISO(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
//tcGpPin<GPIOB_BASE, 5> mSpi1_CS(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);

// -------------------------- SPI ----------------------------------


class cFootSens
{
public:

  u16 SenA[8];
  u16 SenB[8];


  cFootSens()
  {
    vInitDigi();
    vInitDac();
    vInitADC();

    vAdr(0);

    vSetOffsetVolt(LL_DAC_CHANNEL_1, 3300/2);
    vSetOffsetVolt(LL_DAC_CHANNEL_2, 3300/2);
  }

  void vInitDac()
  {
    __HAL_RCC_DAC1_CLK_ENABLE();
    LL_DAC_InitTypeDef DAC_InitStructure;

    cGpPin mDac1(GPIOA_BASE, 4, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mDac2(GPIOA_BASE, 5, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);

    LL_DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE;
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &DAC_InitStructure);
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);

    LL_DAC_StructInit(&DAC_InitStructure);
    DAC_InitStructure.OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE;
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_2, &DAC_InitStructure);
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_2);
  }

  void vInitADC(void)
  {
    cGpPin mAdc1(GPIOA_BASE, 0, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mAdc2(GPIOA_BASE, 1, GPIO_MODE_ANALOG, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);


    __HAL_RCC_ADC_CLK_ENABLE();


    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV2);

    /* Set ADC data resolution */
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

    /* Set ADC conversion data alignment */
    LL_ADC_SetResolution(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

    /* Set ADC low power mode */
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    /* Set ADC group regular trigger source */
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

    /* Set ADC group regular continuous mode */
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    /* Set ADC group regular conversion data transfer */
    LL_ADC_REG_SetDMATransfer(ADC1, LL_ADC_REG_DMA_TRANSFER_NONE);

    /* Set ADC group regular overrun behavior */
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    /* Set ADC group regular sequencer length and scan direction */
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_DISABLE);

    /* Set ADC group regular sequence: channel on the selected sequence rank. */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_5);


    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SAMPLINGTIME_47CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_4, LL_ADC_SINGLE_ENDED);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SAMPLINGTIME_47CYCLES_5);
    LL_ADC_SetChannelSingleDiff(ADC1, LL_ADC_CHANNEL_5, LL_ADC_SINGLE_ENDED);

    //LL_ADC_EnableIT_OVR(ADC1);


      /* Disable ADC deep power down (enabled by default after reset state) */
    LL_ADC_DisableDeepPowerDown(ADC1);

    /* Enable ADC internal voltage regulator */
    LL_ADC_EnableInternalRegulator(ADC1);

    /* Delay for ADC internal voltage regulator stabilization.                */
    /* Compute number of CPU cycles to wait for, from delay in us.            */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    /* Note: If system core clock frequency is below 200kHz, wait time        */
    /*       is only a few CPU processing cycles.                             */
    u32 wait_loop_index = ((LL_ADC_DELAY_INTERNAL_REGUL_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
    while (wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /* Run ADC self calibration */
    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
    }

    /* Delay between ADC end of calibration and ADC enable.                   */
    /* Note: Variable divided by 2 to compensate partially                    */
    /*       CPU processing cycles (depends on compilation optimization).     */
    wait_loop_index = 1000;
    while (wait_loop_index != 0)
    {
      wait_loop_index--;
    }

    /* Enable ADC */
    LL_ADC_Enable(ADC1);

    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
    {
    }
  }


  void vAdc(u32 lu32Adr)
  {
    lu32Adr &= 7;
    vAdr(lu32Adr);

    for (u16 lu16t = 0; lu16t < 500; lu16t++)
    {
      __asm("nop");
    };


    /* Set ADC group regular sequence: channel on the selected sequence rank. */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_5);
    LL_ADC_REG_StartConversion(ADC1);

    while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0) {}
    LL_ADC_ClearFlag_EOC(ADC1);

    SenA[lu32Adr] = LL_ADC_REG_ReadConversionData12(ADC1);


    if (SenA[lu32Adr] == 0)
    {
      __asm("nop");
    }



    /* Set ADC group regular sequence: channel on the selected sequence rank. */
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_6);

    LL_ADC_REG_StartConversion(ADC1);

    while (LL_ADC_IsActiveFlag_EOC(ADC1) == 0) {}
    LL_ADC_ClearFlag_EOC(ADC1);


    SenB[lu32Adr] = LL_ADC_REG_ReadConversionData12(ADC1);
  }

  void vInitDigi()
  {
    cGpPin mOut1(GPIOB_BASE, 0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mOut2(GPIOB_BASE, 1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
    cGpPin mOut3(GPIOB_BASE, 2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_HIGH, 0);
  }

  // lu32Chl: LL_DAC_CHANNEL_1 oder LL_DAC_CHANNEL_2
  // lu32Value: Wert in [mV] 0..3300
  void vSetOffsetVolt(u32 lu32Chl, u32 lu32Value)
  {
    u32 lu32Value_digit;

    if (lu32Value > 3300) lu32Value = 3300;
    lu32Value_digit = (4096 * lu32Value) / 3300;

    LL_DAC_ConvertData12RightAligned(DAC1, lu32Chl, lu32Value_digit);
  }

  void vAdr(u32 lu32Adr)
  {
    lu32Adr &= 7;
    volatile u16 lu16Port = GPIOB->ODR;

    lu16Port &= ~7;

    if (lu32Adr & 1) lu16Port |= 4;
    if (lu32Adr & 2) lu16Port |= 2;
    if (lu32Adr & 4) lu16Port |= 1;

    GPIOB->ODR = lu16Port;
  }

  void vTick()
  {
    for (u8 lu8t = 0; lu8t < 8; lu8t++)
    {
      __asm("nop");
      vAdc(lu8t);
    }
  }
};

cFootSens mcFootSens;

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

    mcFootSens.vTick();
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
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue  = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState         = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK  | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
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

