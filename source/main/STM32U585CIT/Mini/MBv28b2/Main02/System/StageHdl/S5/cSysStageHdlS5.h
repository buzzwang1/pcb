#ifndef __SYSTEM_STAGE_HANDLER_S5_H__
#define __SYSTEM_STAGE_HANDLER_S5_H__

#include "TypeDef.h"
#include "cSysStages.h"
#include "cSysTaskS5.h"

#ifdef __cplusplus
extern "C" {
#endif

class cSysStageHdlS5 : public cSysStage
{
  public:

    cStages* mpcStages;

    cSysStageHdlS5(cStages* lpcStages, cSysStage* lcpNext) : cSysStage(lcpNext) 
    { 
      mpcStages = lpcStages; 
    }

    void vDoInit() override
    {
      mpcStages->mcS5.vInit();

      mpcStages->mcS5.mTaskBoard2Handle = osThreadNew(TaskBoard2, (void*)mpcStages, &TaskBoard2_attributes);

      if (mcpNext) mcpNext->vRequestStart();
    }

    bool isInitFinished() override
    {
      return mpcStages->mcS5.isInit();
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

#endif  //__SYSTEM_STAGE_HANDLER_S5_H__
