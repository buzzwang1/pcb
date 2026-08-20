#pragma once


#include "cSysDPool.h"

#include "cDepTreeCfg.h"
#include "cComponent.h"



#ifdef __cplusplus
extern "C" {
#endif

  extern const osMutexAttr_t cDepTree_mutex_attr;

  extern void  TaskcDepTreeBase(void* argument);
  extern void  TaskcDepTreeInit1(void* argument);
  extern void  TaskcDepTreeInit2(void* argument);
  extern void  TaskcDepTree1ms(void* argument);


class cDepTree
{
  enum tenDirection : u8
  {
    nDown = 0,
    nUp,
    nSame
  };

public:
  cDepTree() {}

  static bool         mTaskInit1Ready;
  static bool         mTaskInit2Ready;

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

    cSysDPool::mSys.mcTasks.DepTree.Base.vInit();
    const osThreadAttr_t TaskcDepTreeBase_attributes = {
      .name       = "cDepTreeBase",                                   ///< name of the thread
      .attr_bits  = 0,                                                ///< attribute bits
      .cb_mem     = &cSysDPool::mSys.mcTasks.DepTree.Base.Tcb,        ///< memory for control block
      .cb_size    = sizeof(cSysDPool::mSys.mcTasks.DepTree.Base.Tcb), ///< size of provided memory for control block
      .stack_mem  = cSysDPool::mSys.mcTasks.DepTree.Base.Stack,       ///< memory for stack
      .stack_size = cSysDPool::mSys.mcTasks.DepTree.Base.StackSize(), ///< size of stack
      .priority   = (osPriority_t)osPriorityNormal,          ///< initial thread priority (default: osPriorityNormal)
      .tz_module  = 0,                                       ///< TrustZone module identifier
      .reserved   = 0                                        ///< reserved (must be 0)
    };
    cSysDPool::mSys.mcTasks.DepTree.Base.Handle = osThreadNew(TaskcDepTreeBase, (void*)null, &TaskcDepTreeBase_attributes);

    cSysDPool::mSys.mcTasks.DepTree.Init1.vInit();
    const osThreadAttr_t TaskDepTreeInit1_attributes = {
      .name       = "cDepTreeInit1",                                     ///< name of the thread
      .attr_bits  = 0,                                                   ///< attribute bits
      .cb_mem     = &cSysDPool::mSys.mcTasks.DepTree.Init1.Tcb,        ///< memory for control block
      .cb_size    = sizeof(cSysDPool::mSys.mcTasks.DepTree.Init1.Tcb), ///< size of provided memory for control block
      .stack_mem  = cSysDPool::mSys.mcTasks.DepTree.Init1.Stack,       ///< memory for stack
      .stack_size = cSysDPool::mSys.mcTasks.DepTree.Init1.StackSize(), ///< size of stack
      .priority   = (osPriority_t)osPriorityNormal,          ///< initial thread priority (default: osPriorityNormal)
      .tz_module  = 0,                                       ///< TrustZone module identifier
      .reserved   = 0                                        ///< reserved (must be 0)
    };
    cSysDPool::mSys.mcTasks.DepTree.Init1.Handle = osThreadNew(TaskcDepTreeInit1, (void*)null, &TaskDepTreeInit1_attributes);

    cSysDPool::mSys.mcTasks.DepTree.Init2.vInit();
    const osThreadAttr_t TaskDepTreeInit2_attributes = {
      .name       = "cDepTreeInit2",                                     ///< name of the thread
      .attr_bits  = 0,                                                   ///< attribute bits
      .cb_mem     = &cSysDPool::mSys.mcTasks.DepTree.Init2.Tcb,        ///< memory for control block
      .cb_size    = sizeof(cSysDPool::mSys.mcTasks.DepTree.Init2.Tcb), ///< size of provided memory for control block
      .stack_mem  = cSysDPool::mSys.mcTasks.DepTree.Init2.Stack,       ///< memory for stack
      .stack_size = cSysDPool::mSys.mcTasks.DepTree.Init2.StackSize(), ///< size of stack
      .priority   = (osPriority_t)osPriorityNormal,          ///< initial thread priority (default: osPriorityNormal)
      .tz_module  = 0,                                       ///< TrustZone module identifier
      .reserved   = 0                                        ///< reserved (must be 0)
    };
    cSysDPool::mSys.mcTasks.DepTree.Init2.Handle = osThreadNew(TaskcDepTreeInit2, (void*)null, &TaskDepTreeInit2_attributes);

    cSysDPool::mSys.mcTasks.DepTree.Tick1ms.vInit();
    const osThreadAttr_t TaskDepTreeTick1ms_attributes = {
      .name       = "cDepTree1ms",                                       ///< name of the thread
      .attr_bits  = 0,                                                   ///< attribute bits
      .cb_mem     = &cSysDPool::mSys.mcTasks.DepTree.Tick1ms.Tcb,        ///< memory for control block
      .cb_size    = sizeof(cSysDPool::mSys.mcTasks.DepTree.Tick1ms.Tcb), ///< size of provided memory for control block
      .stack_mem  = cSysDPool::mSys.mcTasks.DepTree.Tick1ms.Stack,       ///< memory for stack
      .stack_size = cSysDPool::mSys.mcTasks.DepTree.Tick1ms.StackSize(), ///< size of stack
      .priority   = (osPriority_t)osPriorityNormal,          ///< initial thread priority (default: osPriorityNormal)
      .tz_module  = 0,                                       ///< TrustZone module identifier
      .reserved   = 0                                        ///< reserved (must be 0)
    };
    cSysDPool::mSys.mcTasks.DepTree.Tick1ms.Handle = osThreadNew(TaskcDepTree1ms, (void*)null, &TaskDepTreeTick1ms_attributes);

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
        osThreadResume(cSysDPool::mSys.mcTasks.DepTree.Init1.Handle);
      }
      else
      {
        if (mTaskInit2Ready)
        {
          osThreadResume(cSysDPool::mSys.mcTasks.DepTree.Init2.Handle);
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
};

#ifdef __cplusplus
}
#endif



