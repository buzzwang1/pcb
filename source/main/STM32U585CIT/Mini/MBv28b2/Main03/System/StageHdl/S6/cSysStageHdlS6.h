#ifndef __SYSTEM_STAGE_HANDLER_S6_H__
#define __SYSTEM_STAGE_HANDLER_S6_H__

#include "TypeDef.h"
#include "cSysStageHdlBase.h"
#include "cSysStages.h"
#include "cSysTaskS6.h"

#ifdef __cplusplus
extern "C" {
#endif

class cSysStageHdlS6 : public cSysStageHdlBase
{
  public:

    cSysStageHdlS6(cSysStageHdlBase* lcpNext) : cSysStageHdlBase(lcpNext)
    { 
    }

    void vRequestStart() override
    {
      cStage6::mcReq.vStart();
    }

    u8 u8GetRequest() override
    {
      return cStage6::mcReq.mu8Requests;
    }

    void vStartInit() override
    {
      cStage6::vInit();

      cStage6::mTaskPowerHandle    = osThreadNew(TaskPower,    (void*)null, &TaskPower_attributes);
      cStage6::mTaskGuiHandle      = osThreadNew(TaskGui,      (void*)null, &TaskGui_attributes);
      cStage6::mTaskCalenderHandle = osThreadNew(TaskCalender, (void*)null, &TaskCalender_attributes);
    }

    bool isInitFinished() override
    {
      return cStage6::isInit();
    }

    void vDoneDeInit() override
    {
      osThreadTerminate(cStage6::mTaskPowerHandle);
      osThreadTerminate(cStage6::mTaskGuiHandle);
      osThreadTerminate(cStage6::mTaskCalenderHandle);
    }

    void vDoDeInit() override
    {
      cStage6::mbRunPower = False;
      cStage6::mbRunGui = False;
      cStage6::mbRunCalender = False;
      if (osThreadGetState(cStage6::mTaskPowerHandle) == osThreadBlocked)    osThreadResume(cStage6::mTaskPowerHandle);
      if (osThreadGetState(cStage6::mTaskGuiHandle) == osThreadBlocked)      osThreadResume(cStage6::mTaskGuiHandle);
      if (osThreadGetState(cStage6::mTaskCalenderHandle) == osThreadBlocked) osThreadResume(cStage6::mTaskCalenderHandle);
    }

    bool isDeInitFinished() override
    {
      return cStage6::isDeInit();
    }
};

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_HANDLER_S6_H__
