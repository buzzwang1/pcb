#ifndef __SYSTEM_STAGE_HANDLER_S2_H__
#define __SYSTEM_STAGE_HANDLER_S2_H__

#include "TypeDef.h"
#include "cSysStageHdlBase.h"
#include "cSysStages.h"

#ifdef __cplusplus
extern "C" {
#endif


class cSysStageHdlS2 : public cSysStageHdlBase
{
  public:

    cSysStageHdlS2(cSysStageHdlBase* lcpNext) : cSysStageHdlBase(lcpNext)
    {
    }

    void vRequestStart() override
    {
      cStage2::mcReq.vStart();
    }

    u8 u8GetRequest() override
    {
      return cStage2::mcReq.mu8Requests;
    }

    void vStartInit() override
    {
      // Init Board
      cStage2::vInit();
    }

    bool isInitFinished() override
    {
      return True;
    }

    void vDoneDeInit() override
    {
      cStage2::vDeInit();
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
