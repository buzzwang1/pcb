#ifndef __SYSTEM_STAGE_S1_H__
#define __SYSTEM_STAGE_S1_H__

#include "TypeDef.h"
#include "cBuRam.h"
#include "cWufHandler.h"
#include "cErr.h"
#include "cSysTick.h"
#include "cSysStage.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"

class cStage1
{
public:
  cClockInfo   mcClockInfo;
  cSysTick     mcSysTick;
  cBuRam       mcBuRam;
  cWufHandler  mcWufHandler;
  cErr         mcErr;

  cStage1()
    : mcClockInfo(),
      mcSysTick(0),
      mcBuRam(),
      mcWufHandler(),
      mcErr()
  {}

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

  void vInit()
  {
    mcClockInfo.Update();

    vResetWdog();
    vDeactivateIWgdDuringSleep();

    mcSysTick.vInit();
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S1_H__
