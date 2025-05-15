
#include "main.h"


// STM32U5XX
// ARM®-based Cortex®-M33 32b MCU
// Rom 1024KB/2048KB
// Ram 768KB
// 160Mhz
//
// Phase1: Img Bereich im iFlash
//  000000 - 001FFF:   8kb BL
//  002000 - 007FFF:  24kb BLU Img
//
//  008000 - 097FFF: 576kb BLU Img
//  098000 - 0FDFFF: 408kb BLU Img
//
//  0FE000 - 0FFFFF:   8kb Romconst


//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//
//  PB09  -> Status Led
//
//  PA00  -> Wakeup In (100k Pull Down)
//  PC09  -> Wakeup Out
//
//  PC08  -> reserve (auf Stecker)
//  PC09  -> reserve (auf Stecker)
//
//  Board
//    Power Control
//      PE09 <- TPS62125 PG Power Good
//      PE07 -> TPS62125 S1   S2   S1     R2    VOut
//      PE08 -> TPS62125 S2    0    0    620K   2V09
//                             0    1    409k   2V75
//                             1    0    447k   2V59
//                             1    1    325k   3V26
//
//      PC05 -> MX22917_S1: Versorgung I2C ExCom (Switch)
//      PC04 -> MX22917_S2: Versorgung INAs und I2C Board
//
//      PB12 -> Pomo2 TPS55288 enable
//
//    I2C Board
//      PB14 -> SDA2 Board
//      PB13 -> SCL2 Board
//
//    Flash
//      PE10 -> OSPI_P1_CLK
//      PE11 -> OSPI_P1_NCS
//      PE12 -> OSPI_P1_IO0
//      PE13 -> OSPI_P1_IO1
//      PE14 -> OSPI_P1_IO2
//      PE15 -> OSPI_P1_IO3
//
//  Kommunikation
//    ExIn
//      PD13 -> SDA4  (Input)
//      PD12 -> SCL4  (Input)
//      PD08 -> U3 TX (Input)
//
//    ExOut
//      PB00 -> CH1 Enable
//      PB01 -> CH2 Enable
//      PB02 -> CH3 Enable
//
//      PC01 -> SDA3  (Output)
//      PC00 -> SCL3  (Output)
//      PA02 -> U2 TX (Output)
//
//
//            nRF905:            Display:
//      PB03: SPI1 Clock         SPI1 Clock
//      PB04: SPI1 MISO          SPI1 MISO
//      PB05: SPI1 MOSI          SPI1 MOSI
//      PE06: SPI1 Chip Select   SPI1 Display Chip Select
//      PB08: TX or RX mode      SPI1 Touch Chip Select
//      PE00: Standby/CSN        Display Reset
//      PE01: Power up           Display DC
//      PE02: CLK                ---
//      PE03: Carrier Detected   ---
//      PE04: Adress Match       ---
//      PE05: Data Ready         Touch IRQ
//
//      INA1 Adr. 0x40
//        CH1: ExCom CH3
//        CH2: ExCom CH2
//        CH3: ExCom CH1
//
//      INA2 Adr. 0x41
//        CH1: SysIn
//        CH2: PomoIn
//        CH3: PomoOut
//
//      TPS55288 Adr. 0x74
//
//
//  Timer Usage:
//  #  TIM2  -> DiffTimer (optional)
//    TIM6  -> CyclicCaller
//    TIM7  -> BotCom nRf905
//  #  TIM16 -> BotCom MpHd
//
//  DMA Usage:
//    GPDMA1:
//      0:
//      1: I2C2 Board
//      2: I2C4 In
//      3: Uartx In
//      4: I2C3 Out
//      5: Uartx Out
//      6: Uartx Rx Debug
//      7: Uartx Tx Debug
//      8: SPI Rx nRf905
//      9: SPI Tx nRf905
//     10:
//     11:
//
//
//  Interrupt Usage:
//    #TIM6_IRQHandler:          CyclicCaller:  Prio: 15.8 => 1ms SysTick
//    #I2C1_EV_IRQHandler:       BotCom:        Prio:  8.8
//    #I2C1_ER_IRQHandler:       BotCom:        Prio:  8.8
//    #I2C2_EV_IRQHandler:       Board:         Prio:  8.8
//    #I2C2_ER_IRQHandler:       Board:         Prio:  8.8
//    #EXTI15_10_IRQHandler:     BotNet nRF905: Prio:  9.8 => NRF905
//    #DMA1_Channel2_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    #DMA1_Channel3_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    #TIM7_IRQHandler:          BotNet nRF905: Prio:  9.8 => NRF905
//    #DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio:  6.8 => U1
//    #DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio:  6.8 => U1
//    #USART1_IRQHandler:        BotNet U1:     Prio:  6.8 => U1
//    #TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio:  6.8 => U1
//    #USART2_IRQHandler:        BotCom U2:     Prio:  7.8 => U2
//
//

LED<GPIOB_BASE, 9> mcLed;
// S2   S1     R2     VOut
//  0    0    620K   2V09
//  0    1    409k   2V75
//  1    0    447k   2V59
//  1    1    325k   3V26
cGpPin mTPS62125_S1(GPIOE_BASE,  7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cGpPin mTPS62125_S2(GPIOE_BASE,  8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cGpPin mTPS62125_PG(GPIOE_BASE,  9, GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);

cGpPin mMX22917_S1(GPIOC_BASE, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);
cGpPin mMX22917_S2(GPIOC_BASE, 4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);

u32   mu32SpopCounter;


// BotNet
cBotNetCfg mcMyBotNetCfg((rsz)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);
cBotNet mcBn(&mcMyBotNetCfg);

cNRF905                   mcNRF905(0x00010110, 0x00010100);
cBotNet_UpLinknRf905      mcSideLinkRf(&mcNRF905);
cBotNet_UpLinknRf905Net   mcSideLink(&mcSideLinkRf, &mcBn, 1);


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



  void EXTI5_IRQHandler(void)
  {
    if(__HAL_GPIO_EXTI_GET_IT(LL_EXTI_LINE_5) != 0x00u)
    {
      __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_5);
      mcSideLink.bEventHandler(cNRF905::NRF905_EvDataReady);
    }
  }



void SPI1_IRQHandler(void)
{
  if (SPI1->SR & SPI_FLAG_EOT)
  {
    SPI1->CR1 &= ~(SPI_CR1_SPE);
    SPI1->IFCR = 0xFFFF; // Clear all flags
  }

  if (mcNRF905.mSpi.mu8ModeTx == 1)
  {
    mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaTxReady);
  }
  else
  {
    mcSideLink.bEventHandler(cNRF905::NRF905_EvSpiDmaRxReady);
  }
}

void TIM7_IRQHandler(void)
{
  if(TIM7->SR & TIM_SR_UIF) // if UIF flag is set
  {
    TIM7->SR &= ~TIM_SR_UIF; // clear UIF flag
    TIM7->CR1 &= ~(TIM_CR1_CEN); //disable/stop timer
    mcSideLink.bEventHandler(cNRF905::NRF905_EvTimer);
  }
}


void MAIN_vTick1msHp(void)
{
  mcBn.vTickHp1ms();
}



void MAIN_vTick1msLp(void)
{
  mcBn.vProcess(1000);

  if (mcBn.mcSpop.isEnable()) mu32SpopCounter = 1000 * 60 * 2; // 2 min

  if (mu32SpopCounter > 10)
  {
    mu32SpopCounter--;
  }
  else
  {
    mu32SpopCounter = 0;
    // 2min no Spop, then reset
    cBnSpop_vJumpToApp(0);
  }
}

void MAIN_vTick100msLp(void)
{
  mcLed.Toggle();
  cBnSpop_vResetWdog();
}

void MAIN_vTick1000msHp(void)
{
}

void MAIN_vTick1000msLp(void)
{
}


void MAIN_vInitSystem(void)
{
  //u8 lu8t;

  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);
  cBnSpop_vResetWdog();

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

  mcBn.bAddLink((cBotNet_LinkBase*)&mcSideLink, 0xE000);
  mcSideLink.vSetTiming(15 * 1000, 50); // 15s Ping Interval, 50ms warten auf eine Session nach Ping.
  
  mu32SpopCounter = 1000 * 60 * 2;

  //{
  //  u8 lszData[16];
  //
  //  cMemTools::vMemSet((u8*)lszData, 0, 16);
  //  lszData[0] = 'B';
  //  lszData[1] = 'L';
  //  lszData[2] = 'U';
  //
  //  mcSideLink.vSetStatus(0x11, (u8*)lszData);
  //}

  CycCall_Start(MAIN_vTick1msHp /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                MAIN_vTick1msLp               /*1ms_LP*/,
                NULL   /*10ms_LP*/,
                MAIN_vTick100msLp  /*100ms_LP*/,
                NULL /*1s_LP*/);
  cBnSpop_vResetWdog();
}


/* Main functions ---------------------------------------------------------*/
int main(void)
{
  if (RomConst_stDevice_Info->u8PartitionCnt < 3)
  {
    // RomConst Error
    mcBn.mcAdr.Set(0x1EEE);
  }

  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    __asm("wfi");
  }
}

void SysError_Handler()
{
  while (1)
  {
    __asm("nop");
  }
}

static void SystemClock_Config_32MHz(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;


  // EPOD booster enable wird erst ab 55Mhz benötigt

  // Range is wichtig für SRAM zugriff
  // Range1 für <160 Mhz, bei Range 1 kann man aber flash waitstates auf 0 setzen
  // Range3 für  <55 Mhz, bei Range 3 muss man aber flash waitstates auf 1 setzen
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2
                              | RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

  /* Set systick to 1ms with frequency set to 160MHz */
  LL_Init1msTick(32000000);

  /* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  LL_SetSystemCoreClock(32000000);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();

  //DCACHE_HandleTypeDef hdcache1;
  //hdcache1.Instance = DCACHE1;
  //hdcache1.Init.ReadBurstType = DCACHE_READ_BURST_WRAP;
  //HAL_DCACHE_Init(&hdcache1);
}



// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  cBnSpop_vResetWdog();
  SystemInit();
  HAL_Init();
  SystemClock_Config_32MHz(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}

