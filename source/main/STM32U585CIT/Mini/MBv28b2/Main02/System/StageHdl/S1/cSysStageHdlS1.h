#ifndef __SYSTEM_STAGE_HANDLER_S1_H__
#define __SYSTEM_STAGE_HANDLER_S1_H__

#include "TypeDef.h"
#include "cSysStages.h"

#ifdef __cplusplus
extern "C" {
#endif

class cSysStageHdlS1 : public cSysStage
{
  public:

    cStages* mpcStages;

    cSysStageHdlS1(cStages* lpcStages, cSysStage* lcpNext) : cSysStage(lcpNext)
    {
      mpcStages = lpcStages;
    }

    void vDoInit() override
    {
      // - SysTick(Referenz Zeitstempel für das System)
      // - BuRam(BL Info, Übergabe WUF vom BL, Clock1 Info)
      // - WUF(Wakeup Reasons, Wakeup by Timer, Wakeup by Pin Power
      //   Wakeup by Pin General, Reset)
      mpcStages->mcS1.vInit();
      if (mcpNext) mcpNext->vRequestStart();
    }

    bool isInitFinished() override
    {
      return True;
    }

    void vDoDeInit() override
    {
    }

    bool isDeInitFinished() override
    {
      return True;
    }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_HANDLER_S1_H__
