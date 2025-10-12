#ifndef __SYSTEM_STAGE_HANDLER_S8_H__
#define __SYSTEM_STAGE_HANDLER_S8_H__

#include "TypeDef.h"
#include "cSysStageHdlBase.h"
#include "cSysStages.h"
#include "cSysTaskS8.h"

#ifdef __cplusplus
extern "C" {
#endif

class cSysStageHdlS8 : public cSysStageHdlBase
{
  public:

    cSysStageHdlS8(cSysStageHdlBase* lcpNext) : cSysStageHdlBase(lcpNext)
    { 
    }

    void vRequestStart() override
    {
      cStage8::mcReq.vStart();
    }

    u8 u8GetRequest() override
    {
      return cStage8::mcReq.mu8Requests;
    }

    void vStartInit() override
    {
      cStage8::vInit();
      cStage8::mTaskAppHandle      = osThreadNew(TaskApp,      (void*)null, &TaskApp_attributes);
    }

    bool isInitFinished() override
    {
      return cStage8::isInit();
    }

    void vDoneDeInit() override
    {
      osThreadTerminate(cStage8::mTaskAppHandle);
    }

    void vDoDeInit() override
    {
      cStage8::mbRunApp = False;
      if (osThreadGetState(cStage8::mTaskAppHandle) == osThreadBlocked) osThreadResume(cStage8::mTaskAppHandle);
    }

    bool isDeInitFinished() override
    {
      return cStage8::isDeInit();
    }
};

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_HANDLER_S8_H__
