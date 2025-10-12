#ifndef __SYSTEM_STAGE_HANDLER_H__
#define __SYSTEM_STAGE_HANDLER_H__

#include "TypeDef.h"

#include "cSysStageHdlS1.h"
#include "cSysStageHdlS2.h"
#include "cSysStageHdlS4.h"
#include "cSysStageHdlS5.h"

#ifdef __cplusplus
  extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

extern const osThreadAttr_t TaskSysHdl_attributes;
extern void TaskSysHdl(void* argument);

class cSysStageHdl
{
  public:
    cSysStage* mcpWork;
    const cSysStage* mcpFirstInit;
    const cSysStage* mcpFirstDeInit;

    osThreadId_t mTaskSysHdlHandle;

    cSysStageHdlS1 mcSysStageHdlS1; 
    cSysStageHdlS2 mcSysStageHdlS2; 
    cSysStageHdlS4 mcSysStageHdlS4; 
    cSysStageHdlS5 mcSysStageHdlS5; 

    cStages*       mpcStages;

    cSysStageHdl(cStages* lpcStages)
      : mcSysStageHdlS1(lpcStages, &mcSysStageHdlS2),
        mcSysStageHdlS2(lpcStages, &mcSysStageHdlS4),
        mcSysStageHdlS4(lpcStages, &mcSysStageHdlS5),
        mcSysStageHdlS5(lpcStages, null)
    {
      mpcStages      = lpcStages;
      mcpFirstInit   = &mcSysStageHdlS1;
      mcpFirstDeInit = null;
      mcpWork        = (cSysStage*)mcpFirstInit;
      mcpWork->vRequestStart();
    }

    void vStart()
    {
      /* Init scheduler */
      osKernelInitialize();

      mTaskSysHdlHandle = osThreadNew(TaskSysHdl, this, &TaskSysHdl_attributes);

      /* Start scheduler */
      osKernelStart();
      /* We should never get here as control is now taken by the scheduler */
    }


    void vDoProcess()
    {
      bool lbLoop = True;

      while (lbLoop)
      {
        lbLoop = False;

        switch (mcpWork->menState)
        {
          case cSysStage::UnInited:
            mcpWork->vSetInitStarted();
            if (mcpWork->menState == cSysStage::InitStarted)
            {
              mcpWork->vDoInit();
            }
            lbLoop = True;
            break;
          case cSysStage::InitStarted:
            if (mcpWork->isInitFinished())
            {
              mcpWork->vSetInitFinished();
              lbLoop = True;
            }
            break;
          case cSysStage::InitFinished:
            if (mcpWork->mcpNext != null)
            {
              mcpWork = mcpWork->mcpNext;
              lbLoop = True;
            }
          default:
            break;
        }
      }
    }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_HANDLER_H__
