#include <stdlib.h>
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


//// v00.00.06/Base/Misc/Spop/_var/STM32L433U5xx
//// UartMpHd


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

LED<GPIOB_BASE, 9> mcLedRed;

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


cGpPin mTPS55288_EN(GPIOB_BASE, 12, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 1);

cClockInfo mcClkInfo;

cGpPin     mcI2c2_SCL_Board(GPIOB_BASE, 13, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
cGpPin     mcI2c2_SDA_Board(GPIOB_BASE, 14, GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

cI2cMaster mcI2C2_Board(I2C2, &mcI2c2_SCL_Board, &mcI2c2_SDA_Board, 1, 8);

cINA3221  mcINA3221_1(&mcI2C2_Board, INA3221_I2C_ADDRESS_CONF_0, 10, 10, 10);
cINA3221  mcINA3221_2(&mcI2C2_Board, INA3221_I2C_ADDRESS_CONF_1, 10, 10, 10);
cTPS55288 mcTPS55288(&mcI2C2_Board, TPS55288_I2C_ADDRESS_CONF_0);



//SRAM1: 192k: 0x2000.0000 - 0x2002.FFFF
//SRAM2:  64k: 0x2003.0000 - 0x2003.FFFF
//SRAM3: 512k: 0x2004.0000 - 0x200B.FFFF

//cDiffTimer                               mcDiffTimer;




void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  cErr::munErr->stErr.isHardFault = 1;
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  cErr::munErr->stErr.isMemManage = 1;
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  cErr::munErr->stErr.isBusFault = 1;
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  cErr::munErr->stErr.isUsageFault = 1;
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  cErr::munErr->stErr.isSVC = 1;
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  cErr::munErr->stErr.isDebugMon = 1;
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  cErr::munErr->stErr.isPendSV = 1;
  while (1)
  {
  }
}

void SysTick_Handler(void)
{
  HAL_IncTick();
}



/*
static void Error_Handler(void)
{
  while (1)
  {
  }
}*/


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(u8 *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif




//u32 mu32SystemTime = 0;
//
//void MAIN_vTick1msLp(void)
//{
//  mcI2C2_Board.bStartNext();
//  //mcI2C2_Board.vTick1ms();
//}
//
//
//void MAIN_vTick10msHp(void)
//{
//}
//
//void MAIN_vTick1msHp(void)
//{
//}
//
//void MAIN_vTick10msLp(void)
//{
//  mcI2C2_Board.vTick10ms();
//
//  for (u8 lui8i = 1; lui8i <= 3; lui8i++)
//  {
//    mcINA3221_1.i8ReadVShunt_digit(lui8i);
//    mcINA3221_1.i8ReadVBus_digit(lui8i);
//
//    mcINA3221_2.i8ReadVShunt_digit(lui8i);
//    mcINA3221_2.i8ReadVBus_digit(lui8i);
//  }
//}


//void MAIN_vTick1000msLp(void)
//{
//  mcLedRed.Toggle();
//  static volatile u16 lu16Test = 5000;
//  mcTPS55288.i8SetVoltage_mV(lu16Test);
//}

void MAIN_vTick1msHp(void)
{
  mcSys.vTick1msHp();
}


void MAIN_vTick1msLp(void)
{
  //static u8 lu8Tick1msCnt = 0;
  mcSys.vTick1msLp();

  //lu8Tick1msCnt++;
  //if ((lu8Tick1msCnt % 10) == 0)
  //{
  //  mcData.vTick10ms();
  //}
}


void MAIN_vInitSystem(void)
{
  mcSys.vInit1();

  //mcBnMsgHandlerApp.vAddMsgSys();
  CycCall_Start(MAIN_vTick1msHp,
                MAIN_vTick1msLp);

  mcSys.vInit2();

  //    Time required after VCC is stable before the device can accept commands. 100 µs
  // u8 lu8EepRetrys = 10;
  // while ((mcData.mbError) && (lu8EepRetrys > 0))
  // {
  //   mcData.mbError = False;
  //   while ((mcData.mbLoad) && (!mcData.mbError))
  //   {
  //     CycCall_vIdle();
  //   }
  //   lu8EepRetrys--;
  // }

  if (mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter > 10)
  {
    mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounterReload = 10; // 10
    mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter = mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounterReload;
  }
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();

    // __asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {
    __asm("nop");
  }
}

/*
//@brief  System Clock Configuration
//         The system Clock is configured as follows :
//            System Clock source            = PLL (MSI)
//            SYSCLK(Hz)                     = 160000000
//            HCLK(Hz)                       = 160000000
//            AHB Prescaler                  = 1
//            APB1 Prescaler                 = 1
//            APB2 Prescaler                 = 1
//            MSI Frequency(Hz)              = 4000000
//            PLL_MBOOST                     = 1
//            PLL_M                          = 1
//            PLL_N                          = 80
//            PLL_Q                          = 2
//            PLL_R                          = 2
//            PLL_P                          = 2
//            Flash Latency(WS)              = 4

static void SystemClock_Config_160Mhz(void)
{
  // Enable PWR clock
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);

  // Set the regulator supply output voltage
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while (LL_PWR_IsActiveFlag_VOS() == 0)
  {
  }

  // Switch to SMPS regulator instead of LDO
  //LL_PWR_SetRegulatorSupply(LL_PWR_SMPS_SUPPLY);
  //while (LL_PWR_IsActiveFlag_REGULATOR() != 1)
  //{
  //}

  // Enable MSI oscillator
  LL_RCC_MSIS_SetRange(LL_RCC_MSISRANGE_4); // => 4Mhz
  LL_RCC_MSI_SetCalibTrimming(10, LL_RCC_MSI_OSCILLATOR_0);
  LL_RCC_MSIS_Enable();
  while (LL_RCC_MSIS_IsReady() != 1)
  {
  }

  // Set FLASH latency
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  // Configure PLL clock source
  LL_RCC_PLL1_SetMainSource(LL_RCC_PLL1SOURCE_MSIS);

  // Enable the EPOD to reach max frequency
  LL_PWR_EnableEPODBooster();
  while (LL_PWR_IsActiveFlag_BOOST() == 0)
  {
  }

  // Main PLL configuration and activation
  // 4Mhz (MSI)  * 80 / 2 => 160Mhz
  LL_RCC_PLL1_EnableDomain_SYS();
  LL_RCC_PLL1FRACN_Disable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_4_8);
  LL_RCC_SetPll1EPodPrescaler(LL_RCC_PLL1MBOOST_DIV_1);
  LL_RCC_PLL1_SetDivider(1);
  LL_RCC_PLL1_SetN(80);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(2);
  LL_RCC_PLL1_SetR(2);

  LL_RCC_PLL1_Enable();
  while (LL_RCC_PLL1_IsReady() != 1)
  {
  }

  // Set AHB, APB1, APB2 and APB3 prescalers
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetAPB3Prescaler(LL_RCC_APB3_DIV_1);

  // Set PLL1 as System Clock Source
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  }

  // Set systick to 1ms with frequency set to 160MHz
  LL_Init1msTick(160000000);

  // Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function)
  LL_SetSystemCoreClock(160000000);

  LL_ICACHE_SetMode(LL_ICACHE_1WAY);
  LL_ICACHE_Enable();
}*/



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
  SystemInit();
  SystemClock_Config_32MHz(); // Decomment for 16Mhz HSI
  SystemCoreClockUpdate();
}





