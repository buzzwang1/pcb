
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz



//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//  Master:      BnAdr: 1.E.0.0 für MpHd
//
//  PB09  -> Status Led
//
//  PA00   -> Wakeup (100k Pull Down)
//
//  PA08   -> reserve
//  PA05   -> reserve
//  PB00   -> reserve
//
//  Power Control
//    Battery Control:
//      Balancer:
//      PB02  -> U_Bat2
//      PA15  -> U_Bat1
//
//    Regulator In:
//      PA04  -> Dac
//      PA07  -> En
//
//    Regulator Out:
//      PA05  -> Dac
//      PB08  -> En
//      PA06  -> U_Sys: ADC_IN11
//
//   Switch:
//      PC13: Battery MainSwitch
//      PA10: LowPower(3V3, ADC, I2C)
//      PA06: Led
//
//
// //      PB00  -> U_Bat4
//
//
//
//  I2C2  -> BotNet 1.0.0.0
//    PB06  -> I2C2 SCL
//    PB07  -> I2C2 SDA
//
//  U1    -> BotNet 1.E.0.0
//    PA09  -> U1 Tx/Rx
//
//  U2    -> Debug
//    PA02  -> U2 Tx
//    PA03  -> U2 Rx
//
//  I2C2  -> INA3221 2x, Display (Optional) , Apds9960 (Optional)
//    PB10  -> I2C2 SCL AF4
//    PB14  -> I2C2 SDA AF4
//
//
//  nRF905:
//  PB03: SPI1 Clock
//  PB04: SPI1 MISO
//  PB05: SPI1 MOSI
//  PA12: SPI1 Chip Select
//  PB12: TX or RX mode
//  PA11: Standby
//  PB15: Power up
//  PB13: Data Ready
//  PB00 (option)
//  PB01 (option)
//  PA15 (option)
//
//
//  Timer Usage:
//    TIM2  -> DiffTimer (optional)
//    TIM6  -> CyclicCaller
//    TIM7  -> BotCom nRf905
//    TIM16 -> BotCom MpHd
//
//  DMA Usage:
//    DMA1:
//      0:
//      1:
//      2: SPI1 Rx: NRF905 CS:1
//      3: SPI1 Tx: NRF905 CS:1
//      4: I2C2 Tx: Mainboard  CS:3
//      5: I2C2 Rx: Mainboard  CS:3
//      6: I2C1 Tx: BotNet CS:3 / Usart2 Rx BotNet CS:2
//      7: I2C1 Rx: BotNet CS:3 / Usart2 Tx BotNet CS:2
//
//    DMA2:
//      0:
//      1:
//      2:
//      3:
//      4:
//      5:
//      6: I2C1 Rx: BotNet CS:5 / Usart1 Tx BotNet CS:2
//      7: I2C1 Tx: BotNet CS:5 / Usart1 Rx BotNet CS:2
//
//  Interrupt Usage:
//    TIM6_DAC_IRQHandler:      CyclicCaller:  Prio: 15.8 => 1ms SysTick
//    I2C1_EV_IRQHandler:       BotCom:        Prio:  8.8
//    I2C1_ER_IRQHandler:       BotCom:        Prio:  8.8
//    I2C2_EV_IRQHandler:       Board:         Prio:  8.8
//    I2C2_ER_IRQHandler:       Board:         Prio:  8.8
//    EXTI15_10_IRQHandler:     BotNet nRF905: Prio:  9.8 => NRF905
//    DMA1_Channel2_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    DMA1_Channel3_IRQHandler: BotNet nRF905: Prio:  9.8 => NRF905
//    TIM7_IRQHandler:          BotNet nRF905: Prio:  9.8 => NRF905
//    DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio:  6.8 => U1
//    DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio:  6.8 => U1
//    USART1_IRQHandler:        BotNet U1:     Prio:  6.8 => U1
//    TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio:  6.8 => U1
//    USART2_IRQHandler:        BotCom U2:     Prio:  7.8 => U2
//
// Backup-Register Usage:
//    00: Clock Valid by Clock
//    01: Wakeup Counter
//
//
//    DiffTimer Max Cycle
//     0: I2C1_EV_IRQHandler(void)
//     1: I2C1_ER_IRQHandler(void)
//     2: I2C2_EV_IRQHandler(void)
//     3: I2C2_ER_IRQHandler(void)
//     4: EXTI15_10_IRQHandler(void)
//     5: DMA1_Channel2_IRQHandler(void)
//     6: DMA1_Channel3_IRQHandler(void)
//     7: TIM7_IRQHandler(void)
//     8: DMA2_Channel6_IRQHandler(void)
//     9: DMA2_Channel7_IRQHandler
//    10: USART1_IRQHandler
//    11: USART2_IRQHandler(void)
//    12: TIM1_UP_TIM16_IRQHandler(void)
//    13: MAIN_vTick1msHp
//    14: MAIN_vTick10msLp
//    15: MAIN_vTick100msLp
//    16: MAIN_vTick1000msLp


//    DiffTimer Timing
//     0: Nach SystemInit
//     1: Start von Main
//     2: Vor I2C Bausteine Initialisieren
//     3: Nach I2C Bausteine Initialisieren
//     4: frei
//     5: frei
//     6: frei
//     7: frei
//     8: Uhrzeit gesetzt
//     9: frei
//    10: frei
//    11: frei
//    12: Start von bExitRun
//    13: Start von PreparePowerdown
//    14: Start von GotoPowerDown
//    15: vor Sleep

cClockInfo mcClockInfo;

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




void MAIN_vTick1msHp(void)
{
  mcSys.vTick1msHp();
}


void MAIN_vTick1msLp(void)
{
  mcSys.vTick1msLp();
}


void MAIN_vInitSystem(void)
{
  mcSys.vInit1();

  CycCall_Start(MAIN_vTick1msHp,
                MAIN_vTick1msLp);

  mcSys.vInit2();
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  #ifdef PCB_PROJECTCFG_Test
    #ifdef TESTSYSRUNTIME
      mcPB00.vSet1();
      mcTestClassTim[1].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
    #endif
  #endif

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

void SystemClock_Config_HSE(void)
{
  // SystemClock = HSE (== 24Mhz) => witd im Options-file gesetzt => "-DHSE_VALUE=24000000"
  // kein Pll

  RCC_OscInitTypeDef RCC_OscInitStruct   = {};
  RCC_ClkInitTypeDef RCC_ClkInitStruct   = {};

  // Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState            = RCC_HSI_ON; // HSI ON für I2C
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }

  // Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_RCC_RTCAPB_CLK_ENABLE();

  // Configure the main internal regulator output voltage
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}


// This is called from the Startup Code, before the c++ constructors
void MainSystemInit()
{
  #ifdef PCB_PROJECTCFG_Test
    // Messung um die PowerUp Starttime zu messen
    // Wird vor Sleep wieder ausgeschaltet
    // Set PB01
  cSysPkgPMon::vPA01_Init();
  cSysPkgPMon::vPA01_Set1();
  #endif

  SystemInit();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set0();
  #endif
  HAL_Init();
  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set1();
  #endif
  SystemClock_Config_HSE();
  SystemCoreClockUpdate();

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set0();
  #endif

  #ifdef PCB_PROJECTCFG_Test
    // Timer aufziehen nachdem der uC auf 16Mhz ist
    // Nach dem Reset, braucht der es ~6.3ms bis dieser Punkt
    //   + 5ms bei einer 10k/1uF Reset-Beschaltung
    //   + 1,3ms Laufzeit bis hierhin
    cSysPkgPMon::vInitTimer(6300);
  #endif

  if (!RomConst_IsValid())
  {
    // RomConst Error
    SysError_Handler();
  }

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set1();
  #endif

  cBuRam::vEnable();
  cErr::vInit();

  // 0x20007800
  //   Heap 32k
  // 0x2000F800
  //   Stack 2k
  // 0x20010000
  #ifdef PCB_PROJECTCFG_Test
    //cMemTools::vMemSet((u8*)0x2000F800, 0x55, 0x800 - 128);
    //cMemTools::vMemSet((u8*)0x20007800, 0x55, 0x8000);
  #endif

  #ifdef PCB_PROJECTCFG_Test
    cSysPkgPMon::vPA01_Set0();
  #endif
}

