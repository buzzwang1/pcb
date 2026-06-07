#ifndef _DEPTREE_H
#define _DEPTREE_H


#include "typedef.h"
#include "cDepTreeCfg.h"
#include "cComponent.h"
#include "typedef.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"


extern const osMutexAttr_t cDepTree_mutex_attr;

extern const osThreadAttr_t TaskcDepTreeBase_attributes;
extern void  TaskcDepTreeBase(void* argument);

extern const osThreadAttr_t TaskcDepTreeInit1_attributes;
extern void  TaskcDepTreeInit1(void* argument);
extern const osThreadAttr_t TaskcDepTreeInit2_attributes;
extern void  TaskcDepTreeInit2(void* argument);

extern const osThreadAttr_t TaskcDepTree1ms_attributes;
extern void  TaskcDepTree1ms(void* argument);
//extern const osThreadAttr_t TaskcDepTree16ms_attributes;
//extern void  TaskcDepTree16ms(void* argument);
//extern const osThreadAttr_t TaskcDepTree128ms_attributes;
//extern void  TaskcDepTree128ms(void* argument);
//extern const osThreadAttr_t TaskcDepTree1024ms_attributes;
//extern void  TaskcDepTree1024ms(void* argument);


class cDepTree
{
  enum tenDirection : u8
  {
    nDown = 0,
    nUp,
    nSame
  };

public:
  cDepTree()
  {
  }

  static osThreadId_t mTaskBaseHandle;

  static osThreadId_t mTaskInit1Handle;
  static bool         mTaskInit1Ready;
  static osThreadId_t mTaskInit2Handle;
  static bool         mTaskInit2Ready;

  static osThreadId_t mTask1msHandle;
  //static osThreadId_t mTask16msHandle;
  //static osThreadId_t mTask128msHandle;
  //static osThreadId_t mTask1024msHandle;

  static osMutexId_t  xMutex;

  static void vInit()
  {
    cComponentList::mReqRun.vClearAll();
    cComponentList::mReqState.vClearAll();
  }

  static void vStart()
  {
    /* Init scheduler */
    osKernelInitialize();

    xMutex = osMutexNew(&cDepTree_mutex_attr);

    // Start Task
    mTaskBaseHandle = osThreadNew(TaskcDepTreeBase, (void*)null, &TaskcDepTreeBase_attributes);

    mTaskInit1Handle = osThreadNew(TaskcDepTreeInit1, (void*)null, &TaskcDepTreeInit1_attributes);
    mTaskInit2Handle = osThreadNew(TaskcDepTreeInit2, (void*)null, &TaskcDepTreeInit2_attributes);

    mTask1msHandle    = osThreadNew(TaskcDepTree1ms,    (void*)null, &TaskcDepTree1ms_attributes);
    //mTask16msHandle   = osThreadNew(TaskcDepTree16ms,   (void*)null, &TaskcDepTree16ms_attributes);
    //mTask128msHandle  = osThreadNew(TaskcDepTree128ms,  (void*)null, &TaskcDepTree128ms_attributes);
    //mTask1024msHandle = osThreadNew(TaskcDepTree1024ms, (void*)null, &TaskcDepTree1024ms_attributes);

    /* Start scheduler */
    osKernelStart();
    /* We should never get here as control is now taken by the scheduler */
  }

  static void vBaseTask()
  {
    //
    // Check Requests
    if (cComponentList::mReqState.isSet())
    {
      if (mTaskInit1Ready)
      {
        mTaskInit1Ready = False;
        osThreadResume(mTaskInit1Handle);
      }
      else
      {
        if (mTaskInit2Ready)
        {
          osThreadResume(mTaskInit2Handle);
        }
      }
    }


    // Check Requests
    while (cComponentList::mReqRun.isSet())
    {
      i8 iu8ReqComp = cComponentList::mReqRun.i8GetFirst();

      if (cComponentList::macList[cDepTreeCfg::cComp::nBase]->bRun())
      {
        cComponentList::mReqRun.vClear(iu8ReqComp);
      }
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }


  static void vWaitForParent(cComponent* lpcMe)
  {
    // Falls ich die Basiskomponente bin, dann braucht man nicht warten
    if (cDepTreeCfg::cComp::nBase != lpcMe->u8GetIdx())
    {
      // Wartet, bis die Abhängigkeit erfüllt ist
      cComponent* lpcDep = cComponentList::macList[lpcMe->u8GetDep()];

      while (!lpcDep->isOn())
      {
        vTaskDelay(pdMS_TO_TICKS(1));
      }
    }
  }

  // -1 Down
  //  0 Same
  // +1 Up
  static tenDirection en8GetDirection(i8 li8ReqComp)
  {
    if (li8ReqComp < 0) return tenDirection::nSame;

    cComponent* lpcMe = cComponentList::macList[li8ReqComp];

    switch (lpcMe->cGetStateReal())
    {
      case cComponent::cState::nStOff:
      {
        switch (lpcMe->cGetStateReq())
        {
          case cComponent::cState::nStOn:
            return tenDirection::nUp;
            break;
          default: break;
        }
      }
      break;
      case cComponent::cState::nStOn:
      {
        switch (lpcMe->cGetStateReq())
        {
          case cComponent::cState::nStOff:
            return tenDirection::nDown;
            break;
          default: break;
        }
      }
      break;
    }

    return tenDirection::nSame;
  }

  static u8 u8SearchDown(u8 lu8ReqComp)
  {
    // Den Baum nach unten suchen, bis die erste nicht initialsierte Komponente gefunden wird
    cComponent* lpcWork = cComponentList::macList[lu8ReqComp];

    // Kann nicht weiter runter
    if (lpcWork->mu8Idx == lpcWork->mu8Dep) return lpcWork->mu8Idx;
    while (1)
    {
      cComponent* lpcWorkNext = cComponentList::macList[lpcWork->mu8Dep];

      // Komponente noch nicht im Zielzustand und wird nicht gerade bearbeitet
      if ((lpcWorkNext->cGetStateReal() != lpcWorkNext->cGetStateReq()) &&
          (!lpcWorkNext->mFlags.stFlags.InitPend))
      {
        lpcWork = lpcWorkNext;
      }
      else
      {
        return lpcWork->mu8Idx;
      }

      if (lpcWork->mu8Idx == lpcWork->mu8Dep) return lpcWork->mu8Idx;
    }
  }


  static void vDoUp(u8 lu8ReqComp)
  {
    u8 lu8ReqCompNew =  u8SearchDown(lu8ReqComp);
    cComponent* lpcMe = cComponentList::macList[lu8ReqCompNew];

    switch (lpcMe->cGetStateReal())
    {
      case cComponent::cState::nStOff:
      {
        switch (lpcMe->cGetStateReq())
        {
          case cComponent::cState::nStOn:
            lpcMe->mFlags.stFlags.InitPend = True;
            break;
          default: break;
        }
      }
      break;
      default: break;
    }

    cComponentList::mReqState.vClear((u8)lu8ReqCompNew);
    osMutexRelease(xMutex);

    switch (lpcMe->cGetStateReal())
    {
      case cComponent::cState::nStOff:
      {
        switch (lpcMe->cGetStateReq())
        {
          case cComponent::cState::nStOn:
            // Eventuell warten, bis die Komponente, von der diese abhängig ist, fertig ist.
            vWaitForParent(lpcMe);
            cDepTreeLog::vAdd(cDepTreeLog::nStartInit, (cDepTreeCfg::cComp)lpcMe->mu8Idx);
            lpcMe->bInit();
            cDepTreeLog::vAdd(cDepTreeLog::nFinishInit, (cDepTreeCfg::cComp)lpcMe->mu8Idx);
            break;
          default: break;
        }
      }
      break;
      default: break;
    }
  }

  static void vWaitForChildren(u8 lu8ReqComp)
  {
    // 0 = Basis kann übersprungen werden.
    for (i8 li8Idx = 1; li8Idx < cDepTreeCfg::cComp::nLast; li8Idx++)
    {
      cComponent* lpcWork = cComponentList::macList[li8Idx];
      if (lpcWork != null)
      {
        if ((lpcWork->cGetStateReal() != cComponent::cState::nStOff) &&
            (lpcWork->u8GetDep() == lu8ReqComp) &&
            (lpcWork->mFlags.stFlags.InitPend))
        {
          vTaskDelay(pdMS_TO_TICKS(1));
        }
      }
    }
    return;
  }

  // Durchsucht alle Komponenten nach einem Child das noch nicht Off ist
  static i8 i8SearchUpDep(u8 lu8ReqComp)
  {
    i8 li8Ret = -1;

    // 0 = Basis kann übersprungen werden.
    for (i8 li8Idx = 1; li8Idx < cDepTreeCfg::cComp::nLast; li8Idx++)
    {
      cComponent* lpcWork = cComponentList::macList[li8Idx];
      if (lpcWork != 0)
      {
        if ((lpcWork->cGetStateReal() != cComponent::cState::nStOff) &&
           (lpcWork->u8GetDep() == lu8ReqComp) &&
           (!lpcWork->mFlags.stFlags.InitPend))
        {
          return li8Idx;;
        }
      }
    }

    return li8Ret;
  }

  static u8 u8SearchUp(u8 lu8ReqComp)
  {
    // Den Baum nach oben suchen, bis die erste noch nicht deinitialsierte Komponente gefunden wird
    i8 li8ReqCompNew = i8SearchUpDep(lu8ReqComp);
    if (li8ReqCompNew < 0) return lu8ReqComp;
    while (1)
    {
      lu8ReqComp = li8ReqCompNew;
      li8ReqCompNew = i8SearchUpDep(lu8ReqComp);
      if (li8ReqCompNew < 0) return lu8ReqComp;
    }
  }

  static void vDoDown(u8 lu8ReqComp)
  {
    u8 lu8ReqCompNew = u8SearchUp(lu8ReqComp);
    cComponent* lpcMe = cComponentList::macList[lu8ReqCompNew];

    switch (lpcMe->cGetStateReal())
    {
      case cComponent::cState::nStOn:
      {
        switch (lpcMe->cGetStateReq())
        {
          case cComponent::cState::nStOff:
            lpcMe->mFlags.stFlags.InitPend = True;
            break;
          default: break;
        }
      }
      break;
      default: break;
    }

    cComponentList::mReqState.vClear((u8)lu8ReqCompNew);
    osMutexRelease(xMutex);


    switch (lpcMe->cGetStateReal())
    {
      case cComponent::cState::nStOn:
      {
        switch (lpcMe->cGetStateReq())
        {
          case cComponent::cState::nStOff:
            vWaitForChildren(lu8ReqCompNew);
            cDepTreeLog::vAdd(cDepTreeLog::nStartDeInit, (cDepTreeCfg::cComp)lpcMe->mu8Idx);
            lpcMe->bDeInit();
            cDepTreeLog::vAdd(cDepTreeLog::nFinishDeInit, (cDepTreeCfg::cComp)lpcMe->mu8Idx);
            break;
          default: break;
        }
      }
      break;
      default: break;
    }
  }

  static void vDoRequest()
  {
    osMutexAcquire(xMutex, osWaitForever);
    if (cComponentList::mReqState.isSet())
    {
      i8 li8ReqComp = cComponentList::mReqState.i8GetFirst();

      // Überprüfen ob die Komponente hoch oder runtergefahren wird.
      tenDirection lenDir = en8GetDirection(li8ReqComp);
      switch (lenDir)
      {
        case tenDirection::nUp:
          vDoUp((u8)li8ReqComp);
          break;
        case tenDirection::nDown:
          vDoDown((u8)li8ReqComp);
          break;
        case tenDirection::nSame:
          // Nichts zu tun
          if (li8ReqComp >= 0)
          {
            cComponentList::mReqState.vClear((u8)li8ReqComp);
          }
          osMutexRelease(xMutex);
          break;
      }
    }
  }

  static void vInit1Task()
  {
    mTaskInit1Ready = False;

    vDoRequest();

    mTaskInit1Ready = True;
    vTaskSuspend(NULL);
  }

  static void vInit2Task()
  {
    mTaskInit2Ready = False;

    vDoRequest();

    mTaskInit2Ready = True;
    vTaskSuspend(NULL);
  }

  static void v1msTask()
  {
    static u16 lu16Tick1ms = 0;

    vTaskDelay(pdMS_TO_TICKS(1));
    lu16Tick1ms++;

    for (u8 lu8Slot = 0; lu8Slot < cComponentList::mcList1ms.mLen; lu8Slot++)
    {
      u8 lu8Comp = cComponentList::mcList1ms.mData[lu8Slot];
      cComponent* lpccomp = cComponentList::macList[lu8Comp];

      if (lpccomp->isOn()) lpccomp->bRun();
    }

    if ((lu16Tick1ms & 15) == 15)
    {
      for (u8 lu8Slot = 0; lu8Slot < cComponentList::mcList16ms.mLen; lu8Slot++)
      {
        u8 lu8Comp = cComponentList::mcList16ms.mData[lu8Slot];
        cComponent* lpccomp = cComponentList::macList[lu8Comp];

        if (lpccomp->isOn()) lpccomp->bRun();
      }

      if ((lu16Tick1ms & 127) == 127)
      {
        for (u8 lu8Slot = 0; lu8Slot < cComponentList::mcList128ms.mLen; lu8Slot++)
        {
          u8 lu8Comp = cComponentList::mcList128ms.mData[lu8Slot];
          cComponent* lpccomp = cComponentList::macList[lu8Comp];

          if (lpccomp->isOn()) lpccomp->bRun();
        }

        if ((lu16Tick1ms & 1023) == 1023)
        {
          for (u8 lu8Slot = 0; lu8Slot < cComponentList::mcList1024ms.mLen; lu8Slot++)
          {
            u8 lu8Comp = cComponentList::mcList1024ms.mData[lu8Slot];
            cComponent* lpccomp = cComponentList::macList[lu8Comp];

            if (lpccomp->isOn()) lpccomp->bRun();
          }
        }
      }
    }
  }

  //static void v16msTask()
  //{
  //  vTaskSuspend(NULL);
  //}
  //
  //static void v128msTask()
  //{
  //  vTaskSuspend(NULL);
  //}
  //
  //static void v1024msTask()
  //{
  //  vTaskSuspend(NULL);
  //}
};

#ifdef __cplusplus
}
#endif

#endif /* _DEPTREE_H */

