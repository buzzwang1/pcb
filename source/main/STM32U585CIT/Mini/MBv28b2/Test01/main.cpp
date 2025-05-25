#include <stdlib.h>
#include "main.h"

__IO uint32_t TimingDelay = 0;

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


//// 32Mhz Startup + Libs auf 32Mhz
//// Sys/Wuf/Def
//// Sys/Wuf/WufHandler
//// Mem/BuRam/cBuRam/
//// Mem/BuRam/Def/
//// Sys/Err/cErr
//// Sys/Err/Def
//// v00.00.06/Base/Misc/Spop/_var/STM32L433U5xx
//// Device/Com/nRF905/Int/v01.00.00/_var/MBv28b2_STM32U5xx
//// UartMpHd
//// RomConst/v00.00.02/STM32U5xx ...
//// Data/DateTime/Clock/v00.00.03/.../STM32U5xx

//  Mainboard01. BotNetId 21 = 0x15
//  Master:      BnAdr: 1.0.0.0 für I2C
//  Master:      BnAdr: 1.E.0.0 für MpHd
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
//      2: I2Cx In
//      3: Uartx In
//      4: I2Cx Out
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

cGpPin mcWakeup(GPIOA_BASE,  0, GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

LED<GPIOB_BASE, 9> mcLed;
LED<GPIOA_BASE, 4> mcLed4;
LED<GPIOA_BASE, 5> mcLed5;

cClockInfo mcClkInfo;


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


class cSystemPowerDown
{
public:
  virtual bool bContinue() = 0;           // Continue after PowerDown
  virtual bool bExitRun() = 0;            // 1. stage PowerDown. Should be used to inform connected devices to go to powerdown
  virtual bool bPreparePowerdown() = 0;   // 2. stage. Stop communication
  virtual bool bGotoPowerDown() = 0;      // 3. goto powerdeon. Turn everything off, that is not needed.
                                          //    Bring everything to a defined/safe state
                                          //    Set WakeUp Sources

  virtual void vEnterPowerDown(u16 lu16WakeUpTime) = 0;
};

class cPowerManager
{
public:
  typedef enum
  {
    nStIdle = 0,
    nStWaitForRun,
    nStWaitForExitRun,
    nStWaitForPreparePowerdown,
    nStWaitForGotoPowerDown
  }tenStates;

  typedef enum
  {
    nStInit = 0,
    nStRun,
    nStPreparePowerDown,
    nStWaitForPowerDown,
  }tenSysStates;


  u16      mu16WakeUpTime;
  u16      mu16DRunTimer;
  u16      mu16DRunTimerReload;
  u16      mu16DExitRunTimer;
  u16      mu16DExitRunTimerReload;
  u16      mu16PreparePowerDownTimer;
  u16      mu16PreparePowerDownTimerReload;


  cSystemPowerDown* mcPowerDown;

  tenStates    enSm;
  tenSysStates enSysState;


  cPowerManager(u16 lu16DRunTimer, u16 lu16DExitRunTimer,
    u16 lu16PreparePowerDownTimer, u16 lu16WakeUpTime,
    cSystemPowerDown* lcPowerDown);

  void vContinueAfterWakeup();


  void vStart();
  void vStart(u16 lu16DRunTimerReload);

  void vTick10ms();
};


class cMySystemPowerDown : public cSystemPowerDown
{
public:
  u32   mu32NoSleepCounter;
  u32   mu32NoSleepCounterReload;
  char8 mszNoSleepReason_Buf[128];
  cStr  mszNoSleepReason;

  cMySystemPowerDown()
    : mszNoSleepReason((const char8*)mszNoSleepReason_Buf, 0, sizeof(mszNoSleepReason_Buf))
  {

  }


  void vInit();

  void vTick1000ms();

  void EnableRTC(void);
  void vPreparePA0_Exti_For_Stop();
  void vPreparePA0_Exti_For_StandBy();
  void vSetRtcPC13();
  void vPrepareRtc_Exti(u16 lu16WakeUpTime);

  // 1. stage PowerDown. Should be used to inform connected
  //    devices to go to powerdown
  bool bExitRun() override;

  // 2. stage. Stop communication
  bool bPreparePowerdown() override;

  // 3. goto PowerDown. Turn everything off, that is not needed.
  //    Bring everything to a defined/safe state
  bool bGotoPowerDown() override;

  // 4. Set WakeUp Sources
  void vEnterPowerDown(u16 lu16WakeUpTime);

  // Continue after PowerDown
  bool bContinue() override;
};


class cSysPkgSMan
{
public:

  cMySystemPowerDown     mcMySystemPowerDown;
  cPowerManager          mcPowerManager;
  cWufHandler            mcWufHandler;

  cSysPkgSMan();

  void vInit1(void);
  void vInit2(void);

  void vTick1msHp(void);
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
};


cPowerManager::cPowerManager(u16 lu16DRunTimer, u16 lu16DExitRunTimer,
                             u16 lu16PreparePowerDownTimer, u16 lu16WakeUpTime,
                             cSystemPowerDown* lcPowerDown)
{
  enSm = nStIdle;
  enSysState = nStInit;

  mu16DRunTimer = lu16DRunTimer;
  mu16DRunTimerReload = mu16DRunTimer;
  mu16DExitRunTimer = lu16DExitRunTimer;
  mu16DExitRunTimerReload = mu16DExitRunTimer;
  mu16PreparePowerDownTimer = lu16PreparePowerDownTimer;
  mu16PreparePowerDownTimerReload = mu16PreparePowerDownTimer;

  mu16WakeUpTime = lu16WakeUpTime;

  mcPowerDown = lcPowerDown;
}

void cPowerManager::vContinueAfterWakeup()
{
  if (mcPowerDown->bContinue())
  {
    enSm = nStWaitForRun;
    vStart();
  }
}


void cPowerManager::vStart()
{
  if ((enSm == nStIdle) || 
      (enSm == nStWaitForRun) ||
      (enSm == nStWaitForExitRun))
  {
    mu16DRunTimer = mu16DRunTimerReload;
    enSm = nStWaitForRun;
  }
}

void cPowerManager::vStart(u16 lu16DRunTimer)
{
  if ((enSm == nStIdle) || 
      (enSm == nStWaitForRun) ||
      (enSm == nStWaitForExitRun))
  {
    if (mu16DRunTimer < lu16DRunTimer)
    {
      mu16DRunTimer = lu16DRunTimer;
    }
    enSm = nStWaitForRun;
  }
}

void cPowerManager::vTick10ms()
{
  bool lbLoop = True;

  while (lbLoop)
  {
    lbLoop = False;
    switch (enSm)
    {
    case nStIdle:
      break;
    case nStWaitForRun:
      enSysState = nStRun;

      if (mu16DRunTimer)
      {
        mu16DRunTimer--;
      }
      else
      {
        if (mcPowerDown->bExitRun())
        {
          enSm = nStWaitForExitRun;
          mu16DExitRunTimer = mu16DExitRunTimerReload;
        }
      }
      break;
    case nStWaitForExitRun:
      if (mu16DExitRunTimer)
      {
        mu16DExitRunTimer--;
      }
      else
      {
        enSysState = nStPreparePowerDown;
        if (mcPowerDown->bPreparePowerdown())
        {
          enSm = nStWaitForPreparePowerdown;
          mu16PreparePowerDownTimer = mu16PreparePowerDownTimerReload;
        }
      }
      break;
    case nStWaitForPreparePowerdown:
      if (mu16PreparePowerDownTimer)
      {
        mu16PreparePowerDownTimer--;
      }
      else
      {
        enSysState = nStWaitForPowerDown;
        if (mcPowerDown->bGotoPowerDown())
        {
          enSm = nStWaitForGotoPowerDown;
          lbLoop = True;
        }
      }
      break;
    case nStWaitForGotoPowerDown:

      mcPowerDown->vEnterPowerDown(mu16WakeUpTime);

      if (mcPowerDown->bContinue())
      {
        enSm = nStWaitForRun;
        vStart();
      }
      break;
    default:
      break;
    }
  }
}



void cMySystemPowerDown::vInit()
{
  mu32NoSleepCounterReload = 1 * 10; // 60s * 10
  //if (mcWakeup.ui8Get() == 1)
  {
    mu32NoSleepCounter = mu32NoSleepCounterReload;
  }
  //else
  //{
  //  mu32NoSleepCounter = 0;
  //}
}

void cMySystemPowerDown::vTick1000ms()
{
  if (mu32NoSleepCounter) mu32NoSleepCounter--;
}

void cMySystemPowerDown::EnableRTC(void)
{
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);
  LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_RTCAPB);
  LL_PWR_EnableBkUpAccess();
}


void cMySystemPowerDown::vPreparePA0_Exti_For_StandBy()
{
  // Configurate Wakeup Pin
  {cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); }

  // Disable all used wakeup sources: WKUP pin
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);

  // Clear wake up Flag
  __HAL_PWR_CLEAR_FLAG(PWR_WAKEUP_FLAG1);


  PWR->WUCR1 |= 1; // WUPEN1
  PWR->WUCR2 = 0; // all rising esge
  PWR->WUCR3 = 0; // PA01
}

void cMySystemPowerDown::vSetRtcPC13()
{
  LL_RTC_DisableWriteProtection(RTC);
  LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMB);
  LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_PUSHPULL);
  LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_LOW); // LOW sets PC13 indirect to high
}

void cMySystemPowerDown::vPrepareRtc_Exti(u16 lu16WakeUpTime_s)
{
  if (lu16WakeUpTime_s)
  {
    /* Configure wakeup timer clock source: RTC/2 clock is selected  */
    LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_DIV_16);

    /* Configure the Wake up timer to periodically wake up the system every 3 seconds.
     * Wakeup timer auto-reload value (WUT[15:0] in RTC_WUTR) is calculated as follow:
     * If LSE is used as RTC clock source and RTC/2 clock is selected (prescaler = 2): auto-reload = (3u * (32768u / 2u)) - 1 = 49151.
     * If LSI is used as RTC clock source and RTC/2 clock is selected (prescaler = 2): auto-reload = (3u * (32000u / 2u)) - 1 = 47999.
     * Wakeup auto-reload output clear value (WUTOCLR[15:0] in RTC_WUTR) is set in order to
     * automatically clear wakeup timer flag (WUTF) by hardware.(Please refer to reference manual for further details)*/
    u32 lu32WutTime = lu16WakeUpTime_s * (32768u / 16u);
    LL_RTC_WAKEUP_SetAutoReload(RTC, (uint32_t)((u16)lu32WutTime | ((u16)lu32WutTime << RTC_WUTR_WUTOCLR_Pos)));


    /* Clear all wake up Flag */
    LL_PWR_ClearFlag_WU();

    // WKUP7.WKUPx_3 (WUSELx = 11) => RTC_ALRA, RTC_ALRB, RTC_WUT or RTC_TS
    /* Set the wakeup pin selection 3 */
    LL_PWR_SetWakeUpPinSignal3Selection(LL_PWR_WAKEUP_PIN7);
    /* Set wakeup pin polarity */
    LL_PWR_SetWakeUpPinPolarityHigh(LL_PWR_WAKEUP_PIN7);
    /* Enable wakeup pin */
    LL_PWR_EnableWakeUpPin(LL_PWR_WAKEUP_PIN7);


    /* ######## ENABLE WUT #################################################*/
    /* Disable RTC registers write protection */
    LL_RTC_DisableWriteProtection(RTC);

    /* Enable wake up counter and wake up interrupt */
    /* Note: Periodic wakeup interrupt should be enabled to exit the device
       from low-power modes.*/
    LL_RTC_EnableIT_WUT(RTC);
    LL_RTC_WAKEUP_Enable(RTC);

    /* Enable RTC registers write protection */
    LL_RTC_EnableWriteProtection(RTC);
  }
}




bool cMySystemPowerDown::bExitRun()
{
  bool lbRet = True;
  cStr_Create(lszStrBuf, 32);

  mszNoSleepReason = "";

  //Warten bis WakeUp Pin Pa01 low ist
  if (mcWakeup.ui8Get() == 1)
  {
    lbRet = False;
    if (mu32NoSleepCounter < mu32NoSleepCounterReload)
    {
      mu32NoSleepCounter = mu32NoSleepCounterReload;
    }
    if (mszNoSleepReason.Len() > 0) mszNoSleepReason += (rsz)", ";
    mszNoSleepReason += (rsz)"Wup Pin";
  }



  //if (lbRet)
  //{
  //  lbRet = mcClock.isReadyForSleep(mszNoSleepReason);
  //}

  //if (lbRet)
  //{
  //  lbRet = mcSys.mcCom.isReadyForSleep(mszNoSleepReason);
  //}
  //
  //if (lbRet)
  //{
  //  lbRet = mcSys.mcBoard.isReadyForSleep(mszNoSleepReason);
  //}

  //Warten bis Counter abgelaufen ist
  if (mu32NoSleepCounter)
  {
    lbRet = False;
    if (mszNoSleepReason.Len() > 0) mszNoSleepReason += (rsz)", ";
    lszStrBuf.Setf((rsz)"NoSlp-TO: %d", mu32NoSleepCounter);
    mszNoSleepReason += lszStrBuf;
  }

  mszNoSleepReason.ToString();

  return lbRet;
}

// 2. stage. Stop communication
bool cMySystemPowerDown::bPreparePowerdown()
{
  return True;
}

// 3. goto PowerDown. Turn everything off, that is not needed.
//    Bring everything to a defined/safe state
bool cMySystemPowerDown::bGotoPowerDown()
{
  HAL_SuspendTick();
  //mcSys.mcBoard.mcStatusLed.Off();
  //mcSys.mcBoard.mcDigiPower.vPowerOff();

  return True;
}


void cMySystemPowerDown::vEnterPowerDown(u16 lu16WakeUpTime)
{
  vSetRtcPC13();
  vPreparePA0_Exti_For_StandBy();
  vPrepareRtc_Exti(lu16WakeUpTime);

  // LSI wird im Shutdown deaktiviert, LSE geht noch im Shutdown
  //          | LSI | LSE | IWDG
  // ---------+-----+-----+-----
  // Standby  |  x  |  X  |  X
  // Shutdown |  -  |  X  |  -
  //
  // IWDG kann in den Option Bytes im Standby deaktiviert werden
  //
  //
  //
  __HAL_FLASH_PREFETCH_BUFFER_DISABLE();

  HAL_PWR_EnterSTANDBYMode();
}

bool cMySystemPowerDown::bContinue()
{
  return True;
}



cSysPkgSMan::cSysPkgSMan()
  : mcMySystemPowerDown(),
    mcPowerManager(1 /* *[10ms] RunTimer */,
      0 /* *[10ms] ExitRunTimer*/,
      0 /* *[10ms] PreparePowerDownTimer*/,
      15 /* [s] WakeUpTime*/, &mcMySystemPowerDown),
    mcWufHandler()
{
  // Debugger pause
  // Wenn man zu schnell Schlafen geht, dann kann der Debugger nicht mehr connected
  // Daher nach POR etwas länger warten
  if ((cWufHandler::munWakeupSources->stWakeupSources.isWuPinRst) ||
      (cWufHandler::munWakeupSources->stWakeupSources.isWuSftRst))
  {
    mcPowerManager.mu16DRunTimer = 1000; /* *[10ms] = 10s */
    mcPowerManager.mu16DRunTimerReload = 1000; /* *[10ms] = 10s */
  }
};


void cSysPkgSMan::vInit1(void)
{
  if ((cWufHandler::munWakeupSources->stWakeupSources.isWuPinRst) ||
      (cWufHandler::munWakeupSources->stWakeupSources.isWuSftRst))
  {
    // Software oder Hardwarereset
    mcPowerManager.vStart();

    // WakeUp Counter zurücksetzen
    cBuRam::mBuRam->u32WakeupCnt = 0;
  }
  else
  {
    if ((cWufHandler::munWakeupSources->stWakeupSources.isWuTimerRtc) ||
        (cWufHandler::munWakeupSources->stWakeupSources.isWuStandBy))
    {
      // WakeUp nach Sleep, entweder von Clock oder WakeUp-Pin
      mcPowerManager.vContinueAfterWakeup();
      // WakeUp Counter um eins erhöhen
      cBuRam::mBuRam->u32WakeupCnt++;
    }
    else
    {
      // keine Ahnung
      mcPowerManager.vStart();
    }
  }

  // Return PC13 control to gpio
  mcMySystemPowerDown.EnableRTC();
  LL_RTC_DisableWriteProtection(RTC);
  LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_DISABLE);

  mcMySystemPowerDown.vInit();
}


void cSysPkgSMan::vInit2(void)
{
}


void cSysPkgSMan::vTick1msHp(void)
{
}

void cSysPkgSMan::vTick1msLp(void)
{
}

void cSysPkgSMan::vTick10msLp(void)
{
  mcPowerManager.vTick10ms();
}

void cSysPkgSMan::vTick100msLp(void)
{
}

void cSysPkgSMan::vTick1000msLp(void)
{
  mcMySystemPowerDown.vTick1000ms();
}

cSysPkgSMan  mcSMan;  // SMan vorne, weil hier BuRam und WUF geladen werden


void MAIN_vInit1()
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);


  bool lu8Reset = False;
  if ((cWufHandler::munWakeupSources->stWakeupSources.isWuPinRst) ||
      (cWufHandler::munWakeupSources->stWakeupSources.isWuSftRst)) lu8Reset = True;

  if (lu8Reset)
  {
    mcLed4.Toggle();
    LL_RCC_ForceBackupDomainReset();
    LL_RCC_ReleaseBackupDomainReset();
    LL_RCC_LSE_Enable();

    while (!LL_RCC_LSE_IsReady()) {};

    LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

    /*##-3- Enable RTC peripheral Clocks #######################################*/
    /* Enable RTC Clock */
    LL_RCC_EnableRTC();

    LL_RTC_InitTypeDef lstRtc;
    LL_RTC_StructInit(&lstRtc);

    lstRtc.HourFormat = LL_RTC_HOURFORMAT_24HOUR;

    // 32768 = 256*128
    // ck_apre = LSEFreq / (ASYNC prediv + 1) = 256Hz
    // ck_apre = 32768Hz / (127 + 1)          = 256Hz
    lstRtc.AsynchPrescaler = ((uint32_t)0x7F);

    // ck_spre = ck_apre / (SYNC prediv + 1) = 1 Hz
    // ck_spre = 256Hz   / (255 + 1)         = 1 Hz
    lstRtc.SynchPrescaler = ((uint32_t)0x00FF);
    LL_RTC_Init(RTC, &lstRtc);
  }


  mcSMan.vInit1(); // State Manager vorne wegen Wakeup handling
  //mcClock.vInit1();
}


void MAIN_vInit2()
{
  //mcClock.vInit2();
  mcSMan.vInit2();
}


void MAIN_vTick1msHp(void)
{
  //mcClock.vTick1msHp();
  mcSMan.vTick1msHp();
}

void MAIN_vTick1000msLp(void)
{
  //mcClock.vTick1000msLp();
  mcSMan.vTick1000msLp();
  mcLed.vToggle();
}


void MAIN_vTick100msLp(void)
{
  //mcClock.vTick100msLp();
  mcSMan.vTick100msLp();
}

void MAIN_vTick10msLp(void)
{
  //mcClock.vTick10msLp();
  mcSMan.vTick10msLp();
}


void MAIN_vTick1msLp(void)
{
  static u16 lu16_1ms = 0;

  //mcClock.vTick1msLp();
  mcSMan.vTick1msLp();

  lu16_1ms++;
  if ((lu16_1ms % 10) == 0)
  {
    MAIN_vTick10msLp();

    if ((lu16_1ms % 100) == 0)
    {
      MAIN_vTick100msLp();

      if ((lu16_1ms % 1000) == 0)
      {
        MAIN_vTick1000msLp();
        lu16_1ms = 0;
      }
    }
  }
}


void MAIN_vInitSystem(void)
{
  MAIN_vInit1();

  CycCall_Start(MAIN_vTick1msHp,
                MAIN_vTick1msLp);

  MAIN_vInit2();


  //if (mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter > 10)
  //{
  //  mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounterReload = 10; // 10
  //  mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter = mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounterReload;
  //}
}

/* Main functions ---------------------------------------------------------*/
int main(void)
{
  MAIN_vInitSystem();

  // Uncomment to avoid losing connection with debugger after wakeup from Standby (Consumption will be increased)
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STANDBY);

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





