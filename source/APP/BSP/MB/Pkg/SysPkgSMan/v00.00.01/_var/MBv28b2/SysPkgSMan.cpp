
#include "SysPkgSMan.h"

#include "System.h"


cWorkMode::cWorkMode()
{
  mCounter = 0;
  vSet(cWorkMode::nStAuto);
}

void cWorkMode::vSet(cWorkMode::tenState lMode)
{
  mMode = lMode;
  if (mMode == cWorkMode::nStManuel)
  {
    mCounter = 60 * 10; // 10min
  }
}

bool cWorkMode::IsAuto()
{
  return (mMode == cWorkMode::nStAuto);
}

void cWorkMode::vTick1000ms()
{
  if (mCounter)
  {
    mCounter--;
    if (mCounter == 0)
    {
      vSet(cWorkMode::nStAuto);
    }
  }
}

const char* cWorkMode::ToString()
{
  if (mMode == cWorkMode::nStManuel)
  {
    return (const char*)"manuel";
  }
  else
  {
    return (const char*)"auto";
  }
}


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
  mu32NoSleepCounterReload = 60 * 10; // 60s * 10
  if (mcSys.mcBoard.mcWakeup.ui8Get() == 1)
  {
    mu32NoSleepCounter = mu32NoSleepCounterReload;
  }
  else
  {
    mu32NoSleepCounter = 0;
  }
}

void cMySystemPowerDown::vTick1000ms()
{
  if (mu32NoSleepCounter) mu32NoSleepCounter--;
}

void cMySystemPowerDown::EnableRTC(void)
{
  /*##-1- Enables the PWR Clock and Enables access to the backup domain #######*/
  /* To change the source clock of the RTC feature (LSE, LSI), you have to:
      - Enable the power clock
      - Enable write access to configure the RTC clock source (to be done once after reset).
      - Reset the Back up Domain
      - Configure the needed RTC clock source */
  ////LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  ////LL_PWR_EnableBkUpAccess();
}

void cMySystemPowerDown::vPreparePA0_Exti_For_Stop()
{
  //// // Configurate Wakeup Pin
  //// {cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL,   GPIO_SPEED_FREQ_LOW, 0); }
  //// //{cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_PULLDOWN, GPIO_SPEED_FREQ_LOW, 0); } // Only in case of no external Pull Down Resistor
  //// 
  //// __HAL_GPIO_EXTI_CLEAR_IT(LL_EXTI_LINE_0);
  //// 
  //// // Enable clock for SYSCFG
  //// __HAL_RCC_SYSCFG_CLK_ENABLE();
  //// // Tell system that you will use PA0 for EXTI_Line0
  //// LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTA, LL_SYSCFG_EXTI_LINE0);
  //// 
  //// //EXTI init
  //// LL_EXTI_InitTypeDef EXTI_InitStruct;
  //// // PA0 is connected to EXTI_Line0
  //// EXTI_InitStruct.Line_0_31   = LL_EXTI_LINE_0;
  //// EXTI_InitStruct.Line_32_63  = LL_EXTI_LINE_NONE;
  //// EXTI_InitStruct.LineCommand = ENABLE;
  //// EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
  //// EXTI_InitStruct.Trigger     = LL_EXTI_TRIGGER_RISING;
  //// LL_EXTI_Init(&EXTI_InitStruct);
  //// 
  //// //NVIC init
  //// // Add IRQ vector to NVIC
  //// HAL_NVIC_SetPriority(EXTI0_IRQn, 15, 15);  // Niedere Prio, wegen busy waiting
  //// HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void cMySystemPowerDown::vPreparePA0_Exti_For_StandBy()
{
  //// // Configurate Wakeup Pin
  //// {cGpPin Dummy(GPIOA_BASE, 0, GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0); }
  //// 
  //// /* Disable all used wakeup sources: WKUP pin */
  //// HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  //// 
  //// /* Clear wake up Flag */
  //// __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
  //// 
  //// PWR->CR3 |= PWR_CR3_EWUP1; // Enable Wakeup pin WKUP1 (PA0)
}

void cMySystemPowerDown::vSetRtcPC13()
{
  //// LL_RTC_DisableWriteProtection(RTC);
  //// LL_RTC_SetAlarmOutEvent(RTC, LL_RTC_ALARMOUT_ALMB);
  //// LL_RTC_SetAlarmOutputType(RTC, LL_RTC_ALARM_OUTPUTTYPE_PUSHPULL);
  //// LL_RTC_SetOutputPolarity(RTC, LL_RTC_OUTPUTPOLARITY_PIN_LOW); // LOW sets PC13 indirect to high
}

void cMySystemPowerDown::vPrepareRtc_Exti(u16 lu16WakeUpTime)
{
  if (lu16WakeUpTime)
  {
    //// // EXTI init for WakeupTimer WUT -> EXTI line 20
    //// LL_EXTI_InitTypeDef EXTI_InitStruct;
    //// // PA0 is connected to EXTI_Line0
    //// EXTI_InitStruct.Line_0_31 = LL_EXTI_LINE_20;
    //// EXTI_InitStruct.Line_32_63 = LL_EXTI_LINE_NONE;
    //// EXTI_InitStruct.LineCommand = ENABLE;
    //// EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
    //// EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING;
    //// LL_EXTI_Init(&EXTI_InitStruct);
    //// 
    //// //EXTI->SWIER1 |= 1 << 20; // Software interrupt on EXTI20
    //// 
    //// // NVIC init
    //// // Add IRQ vector to NVIC
    //// HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 15, 15);  // Niedere Prio, wegen busy waiting
    //// HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
    //// 
    //// LL_RTC_DisableWriteProtection(RTC);
    //// 
    //// // Disable wake up timer to modify it
    //// LL_RTC_WAKEUP_Disable(RTC);
    //// 
    //// while (LL_RTC_IsActiveFlag_WUTW(RTC) != 1) {}
    //// 
    //// LL_RTC_WAKEUP_SetAutoReload(RTC, lu16WakeUpTime);
    //// LL_RTC_WAKEUP_SetClock(RTC, LL_RTC_WAKEUPCLOCK_CKSPRE);
    //// 
    //// LL_RTC_WAKEUP_Enable(RTC);
    //// LL_RTC_EnableIT_WUT(RTC);
    //// 
    //// // Enable RTC registers write protection
    //// LL_RTC_EnableWriteProtection(RTC);
    //// 
    //// LL_RTC_ClearFlag_WUT(RTC);
    //// 
    //// //__HAL_RCC_RTCAPB_CLK_SLEEP_ENABLE();   // RTC APB clock enabled by the clock gating(1) during Sleep and Stop modes
    //// //__HAL_RTC_WAKEUPTIMER_EXTI_ENABLE_IT();
  }
}




bool cMySystemPowerDown::bExitRun()
{
  bool lbRet = True;
  cStr_Create(lszStrBuf, 32);

  mszNoSleepReason = "";

  ////#ifdef PCB_PROJECTCFG_Test
  ////  mcTestClassTim[12].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
  ////#endif

  //Warten bis WakeUp Pin Pa01 low ist
  if (mcSys.mcBoard.mcWakeup.ui8Get() == 1)
  {
    lbRet = False;
    if (mu32NoSleepCounter < mu32NoSleepCounterReload)
    {
      mu32NoSleepCounter = mu32NoSleepCounterReload;
    }
    if (mszNoSleepReason.Len() > 0) mszNoSleepReason += (rsz)", ";
    mszNoSleepReason += (rsz)"Wup Pin";
  }



  if (lbRet)
  {
    lbRet = mcSys.mcClock.isReadyForSleep(mszNoSleepReason);
  }


  if (lbRet)
  {
    lbRet = mcSys.mcCom.isReadyForSleep(mszNoSleepReason);
  }

  if (lbRet)
  {
    lbRet = mcSys.mcBoard.isReadyForSleep(mszNoSleepReason);
  }

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
  ////#ifdef PCB_PROJECTCFG_Test
  ////  mcTestClassTim[13].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
  ////#endif
  return True;
}

// 3. goto PowerDown. Turn everything off, that is not needed.
//    Bring everything to a defined/safe state
bool cMySystemPowerDown::bGotoPowerDown()
{
  ////#ifdef PCB_PROJECTCFG_Test
  ////  mcTestClassTim[14].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
  ////#endif
  HAL_SuspendTick();
  mcSys.mcBoard.mcStatusLed.Off();
  mcSys.mcBoard.mcDigiPower.vPowerOff();

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
  cBnSpop_vResetWdog();

  ////#ifdef PCB_PROJECTCFG_Test
  ////  #ifdef TESTSYSRUNTIME
  ////    mcPB00.vSet0();
  ////    mcTestClassTim[15].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
  ////  #endif
  ////#endif

  HAL_PWR_EnterSTANDBYMode();
}

bool cMySystemPowerDown::bContinue()
{
  return True;
}



cSysPkgSMan::cSysPkgSMan()
  : mcMySystemPowerDown(),
    mcPowerManager(  1 /* *[10ms] RunTimer */, 
                     0 /* *[10ms] ExitRunTimer*/, 
                     0 /* *[10ms] PreparePowerDownTimer*/, 
                    15 /* [s] WakeUpTime*/, &mcMySystemPowerDown),
    mcOpMode(),
    mcWufHandler()
{
  // Debugger pause
  // Wenn man zu schnell Schlafen geht, dann kann der Debugger nicht mehr connected
  // Daher nach POR etwas länger warten
  if ((cWufHandler::munWakeupSources->stWakeupSources.isWuPinRst) ||
      (cWufHandler::munWakeupSources->stWakeupSources.isWuSftRst))
  {
    mcPowerManager.mu16DRunTimer       = 1000; /* *[10ms] = 10s */
    mcPowerManager.mu16DRunTimerReload = 1000; /* *[10ms] = 10s */
  }
};


void cSysPkgSMan::vInit1(void)
{
  if ((cWufHandler::munWakeupSources->stWakeupSources.isWuPinRst) ||
      (cWufHandler::munWakeupSources->stWakeupSources.isWuSftRst))
  {
    // Software oder Hardwarereset
    mcSys.mcClock.mClock.vClear();
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
  mcOpMode.vTick1000ms();

  mcMySystemPowerDown.vTick1000ms();
}