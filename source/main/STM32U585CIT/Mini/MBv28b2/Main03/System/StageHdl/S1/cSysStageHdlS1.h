#ifndef __SYSTEM_STAGE_HANDLER_S1_H__
#define __SYSTEM_STAGE_HANDLER_S1_H__

#include "TypeDef.h"
#include "cSysStageHdlBase.h"
#include "cSysStages.h"

#ifdef __cplusplus
extern "C" {
#endif

class cSysStageHdlS1 : public cSysStageHdlBase
{
  public:

    cSysStageHdlS1(cSysStageHdlBase* lcpNext) : cSysStageHdlBase(lcpNext)
    {
    }

    void vRequestStart() override
    {
      cStage1::mcReq.vStart();
    }

    u8 u8GetRequest() override
    {
      return cStage1::mcReq.mu8Requests;
    }

    void vStartInit() override
    {
      // - SysTick(Referenz Zeitstempel für das System)
      // - BuRam(BL Info, Übergabe WUF vom BL, Clock1 Info)
      // - WUF(Wakeup Reasons, Wakeup by Timer, Wakeup by Pin Power
      //   Wakeup by Pin General, Reset)
      cStage1::vInit();
    }

    bool isInitFinished() override
    {
      return True;
    }

    void vDoneDeInit() override
    {
      cStage1::vEnterStandBy();
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
