#pragma once

#include "cSysDPool.h"
#include "tGPPin.h"


class cCompBase : public cComponent
{
  public:

  cClock mcClock;
  u8 mu8RunCnt;

  cCompBase()
    : cComponent(cDepTreeCfg::cComp::nBase, cDepTreeCfg::cComp::nBase),
      mcClock(False)
  {
  }

  void vResetWdog()
  {
    IWDG->KR = IWDG_KEY_RELOAD;
  }

  void vDeactivateIWgdDuringSleep()
  {
    FLASH_OBProgramInitTypeDef OB;
    HAL_FLASHEx_OBGetConfig(&OB);

    // check if
    if ((OB.USERConfig & FLASH_OPTR_IWDG_STDBY) ||
        (OB.USERConfig & FLASH_OPTR_IWDG_STOP))
    {
      HAL_FLASH_Unlock();
      HAL_FLASH_OB_Unlock();

      OB.OptionType = OPTIONBYTE_USER;
      OB.USERType = OB_USER_IWDG_STOP | OB_USER_IWDG_STDBY;
      OB.USERConfig = OB_IWDG_STOP_FREEZE | OB_IWDG_STDBY_FREEZE;

      if (HAL_FLASHEx_OBProgram(&OB) != HAL_OK)
      {
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
        return;
      }
      HAL_FLASH_OB_Launch();
    }
  }

  void vEnterStandBy()
  {
    u16 lu16WakeUpTime_s = cSysDPool::mSys.mcSleep.u8WakeUpTime_s;
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

    {
      //vSetRtcPC13();
      //vPreparePA0_Exti_For_StandBy();
      //vPrepareRtc_Exti(lu16WakeUpTime);

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
      vResetWdog();

      ////#ifdef PCB_PROJECTCFG_Test
      ////  #ifdef TESTSYSRUNTIME
      ////    mcPB00.vSet0();
      ////    mcTestClassTim[15].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
      ////  #endif
      ////#endif

      HAL_PWR_EnterSTANDBYMode();
    }
  }

  bool bInit() override
  {
    vResetWdog();
    //cSysDPool::mSys.mcClockInfo.Update();
    //cSysDPool::mSys.mcWufHandler.vInit();
    //cSysDPool::mSys.mcErr.vInit();

    mcClock.vInit();
    
    // Nach wakeup sollte die RTC direkt angehen
    // Nach POR, kann es bis zu 2s dauern
    mcClock.isInit2();
    cSysDPool::mSys.mpcClock = &mcClock;

    cSysDPool::mSys.mcSleep.u8WakeUpTime_s = 5;
    cSysDPool::mSys.mcSleep.u8NoSleepAfterPort_s = 5;

    mu8RunCnt = 0;

    cComponentList::mcList128ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {

    cComponentList::mcList128ms.vRemove(this->mu8Idx);

    // Vor dem Herunterfahren noch warten, bis die RTC geht.
    // Das kann bis zu 2s dauern.
    // Sonst funktioniert der Wakeup-Timer nicht, weil dieser von der RTC getaktet wird.
    vResetWdog();
    while (!mcClock.isInit2())
    {
      vTaskDelay(pdMS_TO_TICKS(50));
      vResetWdog();
    }

    __disable_irq(); // Globale Interrupts sperren
    vTaskSuspendAll(); // Blockiert alle anderen Tasks

    vDeactivateIWgdDuringSleep();
    vEnterStandBy();

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    vResetWdog();

    mu8RunCnt++;
    if (mu8RunCnt >= 9)
    {
      mcClock.vAdd1s();
      mu8RunCnt = 0;
    }
    // return True to signal finished
    return cComponent::bRun();
  };
};




