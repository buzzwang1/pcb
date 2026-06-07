#ifndef _COMPONENT_H
#define _COMPONENT_H

#include "typedef.h"
#include "cbArrayT.h"
#include "cDepTreeCfg.h"

class cComponent;

struct cComponentList
{
  static cComponent* macList[cDepTreeCfg::cComp::nLast];

  static tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> mReqRun;
  static tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> mReqState;

  static void vAdd(cComponent* lpcComp);

  static SBArry<u8, cDepTreeCfg::cComp::nLast>  mcList1ms;
  static SBArry<u8, cDepTreeCfg::cComp::nLast>  mcList16ms;
  static SBArry<u8, cDepTreeCfg::cComp::nLast>  mcList128ms;
  static SBArry<u8, cDepTreeCfg::cComp::nLast>  mcList1024ms;

  static void v1msAdd(cDepTreeCfg::cComp lu8Idx)    {if (!mcList1ms.Contains(lu8Idx)) mcList1ms.Add(lu8Idx);}
  static void v1msRemove(cDepTreeCfg::cComp lu8Idx) {mcList1ms.vRemove(lu8Idx); }

  static void v16msAdd(cDepTreeCfg::cComp lu8Idx)    { if (!mcList16ms.Contains(lu8Idx)) mcList16ms.Add(lu8Idx); }
  static void v16msRemove(cDepTreeCfg::cComp lu8Idx) { mcList16ms.vRemove(lu8Idx); }

  static void v128msAdd(cDepTreeCfg::cComp lu8Idx)    { if (!mcList128ms.Contains(lu8Idx)) mcList128ms.Add(lu8Idx); }
  static void v128msRemove(cDepTreeCfg::cComp lu8Idx) { mcList128ms.vRemove(lu8Idx); }

  static void v1024msAdd(cDepTreeCfg::cComp lu8Idx)    { if (!mcList1024ms.Contains(lu8Idx)) mcList1024ms.Add(lu8Idx); }
  static void v1024msRemove(cDepTreeCfg::cComp lu8Idx) { mcList1024ms.vRemove(lu8Idx); }
};


class cComponent
{
public:
  // Off <--Init--> Ready <--Enable--> On

  enum cState : u8
  {
    nStOff = 0,
    nStOn,
  };

  typedef union
  {
    u8 u8All;
    struct
    {
      //
      cState StateReq  : 2;
      cState StateReal : 2;

      u8 RunReq   : 1; // Run Request Flag
      u8 Error    : 1; // Es gab irgendwo einen Fehler
      u8 InitPend : 1; // Ein Übergang von Off->On oder On->Off läuft gerade
      u8 Reserve  : 1;
    }stFlags;
  }tunFlags;

  public:

  tcBitFieldArray<(cDepTreeRequester::nLast / 32) + 1> mOnRequester;

  char* mszName[10];

  tunFlags mFlags;
  u8 mu8TimeOut_100ms;
  u8 mu8ReqCntInit;

  u8 mu8Idx;
  u8 mu8Dep;

  cComponent(u8 lu8Idx, u8 lu8Dep)
  {
    mu8TimeOut_100ms = 0;
    mu8ReqCntInit = 0;
    mFlags.u8All  = 0;

    mu8Idx = lu8Idx;
    mu8Dep = lu8Dep;

    mOnRequester.vClearAll();
  }

  void vSetTimeout_ms(u16 lu16Timeout_ms)
  {
    if (lu16Timeout_ms)
    {
      u16 lu16Temp = lu16Timeout_ms /= 100;

      if ((lu16Temp * 100) != lu16Timeout_ms)
      {
        lu16Temp++;
      }
      if (lu16Temp > 255) lu16Temp = 255;
      mu8TimeOut_100ms = (u8)lu16Temp;
    }
  }

  u8 u8GetDep() { return mu8Dep; };
  u8 u8GetIdx() { return mu8Idx; };

  bool isOff()   { return mFlags.stFlags.StateReal == cState::nStOff; }
  bool isOn()    { return mFlags.stFlags.StateReal == cState::nStOn;}

  cState cGetStateReal() { return mFlags.stFlags.StateReal; }
  cState cGetStateReq() { return mFlags.stFlags.StateReq; }

  void vReleaseState(u8 lu8nRequesterIdx)
  {   
    switch (mFlags.stFlags.StateReal)
    {
      case cState::nStOff:
      {
      }
      break;

      case cState::nStOn:
      {
        // Man kann nur releasen, wenn man einen request hat.
        if (!(mOnRequester.isSet(lu8nRequesterIdx)))
        {
          return;
        }

        mOnRequester.vClear(lu8nRequesterIdx);

        // Status ändert sich von On zu Off
        // Dann muss der Baum nachgezogen werden
        u8 lu8WorkIdx = mu8Idx;
        while (1)
        {
          cComponent* lpcWork = cComponentList::macList[lu8WorkIdx];
          lpcWork->vReleaseStateInt();
          if (lu8WorkIdx == lpcWork->mu8Dep) return;
          lu8WorkIdx = lpcWork->mu8Dep;
        }
      }
      break;
    }
  }

  void vRequestState(u8 lu8nRequesterIdx)
  {
    switch (mFlags.stFlags.StateReal)
    {
      case cState::nStOff:
      {
        // Status ändert sich von Off zu On

        // Man kann nur requesten, wenn man noch nicht requested hat.
        if ((mOnRequester.isSet(lu8nRequesterIdx)))
        {
          return;
        }

        mOnRequester.vSet(lu8nRequesterIdx);

        // Dann muss der Baum nachgezogen werden
        u8 lu8WorkIdx = mu8Idx;
        while (1)
        {
          cComponent* lpcWork = cComponentList::macList[lu8WorkIdx];
          lpcWork->vReqStateInt();
          if (lu8WorkIdx == lpcWork->mu8Dep) return;
          lu8WorkIdx = lpcWork->mu8Dep;
        }
      }
      break;

      case cState::nStOn:
      {
        // Man kann nur requesten, wenn man noch nicht requested hat.
        if ((mOnRequester.isSet(lu8nRequesterIdx)))
        {
          return;
        }

        mOnRequester.vSet(lu8nRequesterIdx);

        vReqStateInt();
      }
      break;
    }
  }

  void vReqRun()
  {
    mFlags.stFlags.RunReq = 1;
    cComponentList::mReqRun.vSet(mu8Idx);
  }


  virtual bool bInit()
  {
    // return True to signal finished
    mFlags.stFlags.StateReal = cState::nStOn;
    mFlags.stFlags.InitPend  = False;
    return True;
  }

  virtual bool bDeInit()
  {
    // return True to signal finished
    mFlags.stFlags.StateReal = cState::nStOff;
    mFlags.stFlags.InitPend  = False;
    return True;
  }

  virtual bool bRun()
  {
    // return True to signal finished
    return True;
  };

  private:

  void vReleaseStateInt()
  {
    u8 lu8ReqCntInit = mu8ReqCntInit;

    cDepTreeLog::vAdd(cDepTreeLog::nReleaseOn, (cDepTreeCfg::cComp)mu8Idx);
    if (mu8ReqCntInit) mu8ReqCntInit--;


    if (((lu8ReqCntInit == 1) && (mu8ReqCntInit == 0)))
    {
      mFlags.stFlags.StateReq = cState::nStOff;
      cComponentList::mReqState.vSet(mu8Idx);
    }
  }

  void vReqStateInt()
  {
    u8 lu8ReqCntInit = mu8ReqCntInit;

    mu8ReqCntInit++;
    cDepTreeLog::vAdd(cDepTreeLog::nRequestOn, (cDepTreeCfg::cComp)mu8Idx);

    // Änderungs Request nur setzten, wenn sich was geändert hat.
    if (((lu8ReqCntInit == 0) && (mu8ReqCntInit == 1)))
    {
      mFlags.stFlags.StateReq = cState::nStOn;
      cComponentList::mReqState.vSet(mu8Idx);
    }
  }
};




#endif /* _COMPONENT_H */
