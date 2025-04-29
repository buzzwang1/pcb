
#ifndef __SYS_PKG_STATE_MANAGER_H__
#define __SYS_PKG_STATE_MANAGER_H__


#include "TypeDef.h"
#include "tGPPin.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

#include "cErr.h"
#include "cWufHandler.h"
#include "cStrT.h"




class cWorkMode
{
  public:

  typedef enum
  {
    nStManuel,
    nStAuto,
  }tenState;

  tenState mMode;
  u16      mCounter;

  cWorkMode();

  void vSet(cWorkMode::tenState lMode);

  bool IsAuto();

  void vTick1000ms();

  const char* ToString();
};


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
  cWorkMode              mcOpMode;
  cWufHandler            mcWufHandler;
  cErr                   mcErr;

  cSysPkgSMan();

  void vInit1(void);
  void vInit2(void);

  void vTick1msHp(void);
  void vTick1msLp(void);
  void vTick10msLp(void);
  void vTick100msLp(void);
  void vTick1000msLp(void);
};


#endif // __SYS_PKG_STATE_MANAGER_H__
