
#include "main.h"

// STM32L433CCT
// ARM®-based Cortex®-M4 32b MCU
// Rom 256KB
// Ram 64KB
// Max: 80Mhz, HSI: 16Mhz, HSE: 24Mhz



//  Power Switch v3 LED add on
//
//  PB09  -> Status Led
//
//
//  OutPuts:
//    PA01: High side switch for 3V3
//  OutPuts:
//    PB00: High side switch for CH1
//    PB01: High side switch for CH2
//    PB02: High side switch for CH3
//
//  I2C3  -> Board (Eeprom Adr.:0x50, INA3221 Adr.: 0x40)
//    PA07  -> I2C3 SCL  AF4
//    PB04  -> I2C3 SDA  AF4
//
//  Ext. Flash
//    SPI1
//      PA10: CS
//      PA11: Miso AF5
//      PA12: Mosi AF5
//      PB03: Clk  AF5
//
//  Power Module
//    PA04: DAC
//    PA05: Enable
//
//  BotNet DownLink
//    I2C2
//      PB13  -> I2C2 SCL AF4
//      PB14  -> I2C2 SDA AF4
//    U3
//      PB10  -> TX/RX    AF7
//
//  BotNet UpLink
//    I2C1
//      PB06  -> I2C1 SCL AF4
//      PB07  -> I2C1 SDA AF4
//    U1
//      PA09  -> TX/RX    AF7
//
//  Timer Usage:
//    TIM1  -> PWM für WS2812 for DMA1.Chl2
//    TIM2  -> PWM für WS2812 for DMA1.Chl3
//    TIM6  -> CyclicCaller
//    TIM15 -> BotCom MpHd UpLink
//    TIM16 -> BotCom MpHd DownLink

//
//  DMA Usage:
//    DMA1:
//      0:
//      1: TIM2 für WS2812   CS:4
//      2: I2C3 Tx: Board    CS:3 / U3 Tx DownLink CS:2 / TIM1 for WS2812 CS:7
//      3: I2C3 Rx: Board    CS:3 / U3 Rx DownLink CS:2
//      4: I2C2 Tx: DownLink CS:3 / U1 Tx UpLink CS:2
//      5: I2C2 Rx: DownLink CS:3 / U1 Rx UpLink CS:2
//      6: I2C1 Tx: UpLink   CS:3
//      7: I2C1 Rx: UpLink   CS:3
//
//    DMA2:
//      0:
//      1:
//      2:
//      3: SPI1 Rx  CS:4
//      4: SPI1 Tx  CS:4
//      5:
//      6: I2C1 Rx: BotNet CS:5 / U1 Tx UpLink CS:2
//      7: I2C1 Tx: BotNet CS:5 / U1 Rx UpLink CS:2
//
//  Interrupt Usage:
//    DMA1_Channel1_IRQHandler: WS2812:        Prio: 8.8
//    I2C1_EV_IRQHandler:       BotCom:        Prio: 8.8
//    I2C1_ER_IRQHandler:       BotCom:        Prio: 8.8
//    I2C2_EV_IRQHandler:       Board:         Prio: 8.8
//    I2C2_ER_IRQHandler:       Board:         Prio: 8.8
//    DMA2_Channel6_IRQHandler: BotNet U1 Tx:  Prio: 6.8 => U1
//    DMA2_Channel7_IRQHandler: BotNet U1 Rx:  Prio: 6.8 => U1
//    USART1_IRQHandler:        BotNet U1:     Prio: 6.8 => U1
//    TIM1_UP_TIM16_IRQHandler: BotCom U1:     Prio: 6.8 => U1
//
//


// Call first to get the system clocks
// This is even called before the inlcuded classs in the header
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
  ////static u8 lu8ms = 0;
  mcSys.vTick1msLp();

  ////lu8ms++;
  ////if (lu8ms > 5)
  ////{
  ////  lu8ms = 0;
  ////
  ////  cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsg, 64, mcSys.mcCom.mcBn.mcAdr.Get(), 0x1110, 42);
  ////
  ////  // TX 00 | 00 | 00 | ES.P1.AI.RR.GG.BB.TH.TL
  ////  for (u8 lu8t = 0; lu8t < 58; lu8t++)
  ////  {
  ////    lcMsg.mcPayload[lu8t] = 58-lu8t;
  ////  }
  ////
  ////  lcMsg.vEncode();
  ////  mcSys.mcCom.mcBn.bSendMsg(&lcMsg);
  ////}
}


void MAIN_vInitSystem(void)
{
  mcSys.vInit1();

  CycCall_Start(MAIN_vTick1msHp,
                MAIN_vTick1msLp);

  mcSys.vInit2();

  ////mcSys.mcBoard.mcBoardCntrl.lcEnCh1.vSet(1);
  ////mcSys.mcBoard.mcBoardCntrl.lcEnCh2.vSet(1);
  ////mcSys.mcBoard.mcBoardCntrl.lcEnCh3.vSet(1);
  ////mcSys.mcBoard.mcBoardCntrl.mcPowerOut->vEnable();
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

  ////RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  ////GPIOA->MODER &= ~(3 << (2 * 8));
  ////GPIOA->MODER |=  (1 << (2 * 8));
  ////GPIOA->MODER &= ~(3 << (2 * 15));
  ////GPIOA->MODER |=  (1 << (2 * 15));
  ////RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
  ////GPIOB->MODER &= ~(3 << (2 * 8));
  ////GPIOB->MODER |= (1 << (2 * 8));

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

