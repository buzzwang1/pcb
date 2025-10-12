#ifndef __SYSTEM_STAGE_HANDLER_S5_H__
#define __SYSTEM_STAGE_HANDLER_S5_H__

#include "TypeDef.h"
#include "cSysStageHdlBase.h"
#include "cSysStages.h"
#include "cSysTaskS5.h"

#ifdef __cplusplus
extern "C" {
#endif

class cSysStageHdlS5 : public cSysStageHdlBase
{
  public:

    cSysStageHdlS5(cSysStageHdlBase* lcpNext) : cSysStageHdlBase(lcpNext)
    { 
    }

    void vRequestStart() override
    {
      cStage5::mcReq.vStart();
    }

    u8 u8GetRequest() override
    {
      return cStage5::mcReq.mu8Requests;
    }

    void vStartInit() override
    {
      cStage5::vInit();

      cStage5::mTaskBoard2Handle = osThreadNew(TaskBoard2, (void*)null, &TaskBoard2_attributes);
      cStage5::mTaskComHandle    = osThreadNew(TaskCom,    (void*)null, &TaskCom_attributes);
    }

    bool isInitFinished() override
    {
      return cStage5::isInit();
    }

    void vDoDeInit() override
    {
      cStage5::mbRunBoard2 = False;
      cStage5::mbRunCom = False;
      if (osThreadGetState(cStage5::mTaskBoard2Handle) == osThreadBlocked) osThreadResume(cStage5::mTaskBoard2Handle);
      if (osThreadGetState(cStage5::mTaskComHandle) == osThreadBlocked)    osThreadResume(cStage5::mTaskComHandle);
    }

    void vDoneDeInit() override
    {
      osThreadTerminate(cStage5::mTaskBoard2Handle);
      osThreadTerminate(cStage5::mTaskComHandle);
    }

    bool isDeInitFinished() override
    {
      return cStage5::isDeInit();;
    }
};

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_HANDLER_S5_H__
