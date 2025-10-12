#ifndef __SYSTEM_STAGE_HANDLER_S4_H__
#define __SYSTEM_STAGE_HANDLER_S4_H__

#include "TypeDef.h"
#include "cSysStages.h"
#include "cSysTaskS4.h"

#ifdef __cplusplus
extern "C" {
#endif



class cSysStageHdlS4 : public cSysStage
{
  public:

    cStages* mpcStages;

    cSysStageHdlS4(cStages* lpcStages, cSysStage* lcpNext) : cSysStage(lcpNext)
    {
      mpcStages = lpcStages;
    }

    void vDoInit() override
    {
      mpcStages->mcS4.vInit();

      mpcStages->mcS4.mTaskClockHandle = osThreadNew(TaskClock, (void*)mpcStages, &TaskClock_attributes);
      mpcStages->mcS4.mTaskMemHandle   = osThreadNew(TaskMem,   (void*)mpcStages, &TaskMem_attributes);

      if (mcpNext) mcpNext->vRequestStart();
    }

    bool isInitFinished() override
    {
      return mpcStages->mcS4.isInit();
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

#endif  //__SYSTEM_STAGE_HANDLER_S4_H__
