
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