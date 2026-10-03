#ifndef _COMPONENT_H
#define _COMPONENT_H

#include "typedef.h"
#include "cbArrayT.h"
#include "cDepTreeCfg.h"

#include <initializer_list>

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

  // Komponenten, von der diese Komponente abhängig ist
  tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> mDependencies;
  // Komponenten, die von dieser Komponente abhängig sind
  tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> mRequests;


  tcBitFieldArray<(cDepTreeRequester::nLast / 32) + 1> mOnRequester;

  char* mszName[10];

  tunFlags mFlags;
  u8 mu8TimeOut_100ms;
  u8 mu8ReqCntInit;

  u8 mu8Idx;

  cComponent(u8 lu8Idx, std::initializer_list<u8> lau8Deps)
  {
    mu8TimeOut_100ms = 0;
    mu8ReqCntInit = 0;
    mFlags.u8All  = 0;

    mu8Idx = lu8Idx;

    if (mu8Idx != cDepTreeCfg::cComp::nBase)
    {
      for (u8 lu8Dep : lau8Deps)
      {
        mDependencies.vSet(lu8Dep);
      }
    }
  }

  void vAdd()
  {
    cComponentList::macList[mu8Idx] = (cComponent *)this;

    if (mu8Idx == cDepTreeCfg::cComp::nBase) return;

    i8 li8Dep = 0;

    while (1)
    {
      li8Dep = mDependencies.i8GetNext(li8Dep);
      if (li8Dep < 0) return;

      cComponentList::macList[li8Dep]->mRequests.vSet(mu8Idx);
      li8Dep++;
    }
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

  // Überpruft, ob die übergebene Komponente in den Abgängigkeiten auftaucht.
  bool isDep(u8 lu8Dep)
  {
    return mDependencies.isSet(lu8Dep);
  }


  tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> &cGetDeps() { return mDependencies; };
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

          // Abhängigkeiten checken
          u8 lu8BitCnt = (u8)lpcWork->mDependencies.u8Count();
          switch (lu8BitCnt)
          {

            case 0: // keine Anhängigkeit -> nichts zu tun
              return;
              break;
            case 1: // eine Anhängigkeit -> mit dieser weiter machen
              lu8WorkIdx = (u8)lpcWork->cGetDeps().i8GetFirst();
              break;
            default: // mehrer Anhängigkeit -> mit der ersten direkt weiter machen, und die anderen rekursiv
              {
                tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> lcDepCopy;
                lpcWork->cGetDeps().vClone(lcDepCopy);

                lu8WorkIdx = (u8)lcDepCopy.i8GetFirst();
                lcDepCopy.vClear(lu8WorkIdx);

                while (1)
                {
                  i8 lu8NextIdx = lcDepCopy.i8GetFirst();
                  if (lu8NextIdx >= 0)
                  {
                    lcDepCopy.vClear(lu8NextIdx);
                    cComponentList::macList[lu8NextIdx]->vReleaseState(lu8nRequesterIdx);
                  }
                  else
                  {
                    break;
                  }
                }
              }
              break;
          }
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

          // Abhängigkeiten checken
          u8 lu8BitCnt = (u8)lpcWork->mDependencies.u8Count();
          switch (lu8BitCnt)
          {

            case 0: // keine Anhängigkeit -> nichts zu tun
              return;
              break;
            case 1: // eine Anhängigkeit -> mit dieser weiter machen
              lu8WorkIdx = (u8)lpcWork->mDependencies.i8GetFirst();
              break;
            default: // mehrer Anhängigkeit -> mit der ersten direkt weiter machen, und die anderen rekursiv
              {
                tcBitFieldArray<(cDepTreeCfg::cComp::nLast / 32) + 1> lcDepCopy;
                lpcWork->mDependencies.vClone(lcDepCopy);

                lu8WorkIdx = (u8)lcDepCopy.i8GetFirst();
                lcDepCopy.vClear(lu8WorkIdx);

                while (1)
                {
                  i8 lu8NextIdx = lcDepCopy.i8GetFirst();
                  if (lu8NextIdx >= 0)
                  {
                    lcDepCopy.vClear(lu8NextIdx);
                    cComponentList::macList[lu8NextIdx]->vRequestState(lu8nRequesterIdx);
                  }
                  else
                  {
                    break;
                  }
                }
              }
              break;
          }
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
