#ifndef __SYSTEM_STAGE_HANDLER_S4_H__
#define __SYSTEM_STAGE_HANDLER_S4_H__

#include "TypeDef.h"
#include "cSysStageHdlBase.h"
#include "cSysStages.h"
#include "cSysTaskS4.h"

#ifdef __cplusplus
extern "C" {
#endif



class cSysStageHdlS4 : public cSysStageHdlBase
{
  public:

    cSysStageHdlS4(cSysStageHdlBase* lcpNext) : cSysStageHdlBase(lcpNext)
    {
    }

    void vRequestStart() override
    {
      cStage4::mcReq.vStart();
    }

    u8 u8GetRequest() override
    {
      return cStage4::mcReq.mu8Requests;
    }

    void vStartInit() override
    {
      cStage4::vInit();

      cStage4::mTaskClockHandle = osThreadNew(TaskClock, (void*)null, &TaskClock_attributes);
      cStage4::mTaskMemHandle   = osThreadNew(TaskMem,   (void*)null, &TaskMem_attributes);
    }

    bool isInitFinished() override
    {
      return cStage4::isInit();
    }

    void vDoDeInit() override
    {
      cStage4::mbRunMem = False;
      cStage4::mbRunClock = False;
      if (osThreadGetState(cStage4::mTaskClockHandle) == osThreadBlocked) osThreadResume(cStage4::mTaskClockHandle);
      if (osThreadGetState(cStage4::mTaskMemHandle) == osThreadBlocked)   osThreadResume(cStage4::mTaskMemHandle);
    }

    void vDoneDeInit() override
    {
      osThreadTerminate(cStage4::mTaskClockHandle);
      osThreadTerminate(cStage4::mTaskMemHandle);
    }

    bool isDeInitFinished() override
    {
      return cStage4::isDeInit();
    }
};

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_HANDLER_S4_H__
