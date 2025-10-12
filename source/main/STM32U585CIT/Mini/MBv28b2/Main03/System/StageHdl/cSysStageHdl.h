#ifndef __SYSTEM_STAGE_HANDLER_H__
#define __SYSTEM_STAGE_HANDLER_H__

#include "TypeDef.h"

#include "cSysStageHdlS1.h"
#include "cSysStageHdlS2.h"
#include "cSysStageHdlS4.h"
#include "cSysStageHdlS5.h"
#include "cSysStageHdlS6.h"
#include "cSysStageHdlS8.h"

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
    cSysStageHdlBase* mcpWork;
    const cSysStageHdlBase* mcpFirstInit;

    osThreadId_t mTaskSysHdlHandle;

    cSysStageHdlS8 mcSysStageHdlS8;
    cSysStageHdlS6 mcSysStageHdlS6;
    cSysStageHdlS5 mcSysStageHdlS5;
    cSysStageHdlS4 mcSysStageHdlS4;
    cSysStageHdlS2 mcSysStageHdlS2;
    cSysStageHdlS1 mcSysStageHdlS1;

    cSysStageHdl()
      : mcSysStageHdlS8(null),
        mcSysStageHdlS6(&mcSysStageHdlS8),
        mcSysStageHdlS5(&mcSysStageHdlS6),
        mcSysStageHdlS4(&mcSysStageHdlS5),
        mcSysStageHdlS2(&mcSysStageHdlS4),
        mcSysStageHdlS1(&mcSysStageHdlS2)
    {
      mcpFirstInit   = &mcSysStageHdlS1;
      mcpWork        = (cSysStageHdlBase*)mcpFirstInit;
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
          case cSysStageHdlBase::UnInited:
            if (mcpWork->u8GetRequest())
            {
              mcpWork->vSetInitStarted();
              mcpWork->vStartInit();
            }
            else
            {
              mcpWork->vSetInitFinished();
            }
            lbLoop = True;
            break;
          case cSysStageHdlBase::InitStarted:
            if (mcpWork->isInitFinished())
            {
              mcpWork->vSetInitFinished();
              lbLoop = True;
            }
            break;
          case cSysStageHdlBase::InitFinished:
            {
              bool lbNext = False;
              if (mcpWork->mcpNext != null)
              {
                if (mcpWork->mcpNext->u8GetRequest())
                {
                  lbNext = True;
                }
              }

              if (lbNext)
              {
                mcpWork = mcpWork->mcpNext;
                lbLoop = True;
              }
              else
              {
                if (!mcpWork->u8GetRequest())
                {
                  mcpWork->vSetDeInitStarted();
                  lbLoop = True;
                }
              }
            }
            break;
          case cSysStageHdlBase::DeInitStarted:
            if (mcpWork->isDeInitFinished())
            {
              mcpWork->vSetDeInitFinished();
              lbLoop = True;
            }
            else
            {
              mcpWork->vDoDeInit();
            }
            break;
          case cSysStageHdlBase::DeInitFinished:
            mcpWork->vDoneDeInit();
            if (mcpWork->mcpBefore != null)
            {
              mcpWork = mcpWork->mcpBefore;
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
