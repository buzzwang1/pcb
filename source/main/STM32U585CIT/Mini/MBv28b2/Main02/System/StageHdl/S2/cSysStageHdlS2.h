#ifndef __SYSTEM_STAGE_HANDLER_S2_H__
#define __SYSTEM_STAGE_HANDLER_S2_H__

#include "TypeDef.h"
#include "cSysStages.h"

#ifdef __cplusplus
extern "C" {
#endif


class cSysStageHdlS2 : public cSysStage
{
  public:

    cStages* mpcStages;

    cSysStageHdlS2(cStages* lpcStages, cSysStage* lcpNext) : cSysStage(lcpNext)
    {
      mpcStages = lpcStages;
    }

    void vDoInit() override
    {
      // Init Board
      mpcStages->mcS2.vInit();
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

#endif  //__SYSTEM_STAGE_HANDLER_S2_H__
