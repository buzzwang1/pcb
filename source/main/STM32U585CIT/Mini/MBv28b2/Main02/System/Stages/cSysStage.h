#ifndef __SYSTEM_STAGE_H__
#define __SYSTEM_STAGE_H__

#include "TypeDef.h"

#ifdef __cplusplus
  extern "C" {
#endif


class cSysStage
{
  public:

  enum tenState: u8
  {
    UnInited = 0,
    InitStarted,
    InitFinished,
    DeInitStarted,
    DeInitFinished
  };

  tenState menState;
  u8       mu8Requests;

  cSysStage *mcpNext;
  cSysStage* mcpBefore;

  cSysStage(cSysStage* lcpNext)
  {
    mcpNext = lcpNext;

    if (lcpNext != null)
    {
      lcpNext->mcpBefore = this;
    }

    menState = UnInited;
    mu8Requests = 0;
  }

  void vRequestStart()
  {
    mu8Requests++;
  }

  void vSetInitStarted()
  {
    menState = InitStarted;
  }

  void vSetInitFinished()
  {
    menState = InitFinished;
  }

  virtual void vDoInit()
  {
  }

  virtual bool isInitFinished()
  {
    return False;
  }


  void vSetDeInitStarted()
  {
    menState = DeInitStarted;
  }

  void vSetDeInitFinished()
  {
    menState = DeInitFinished;
  }

  virtual void vDoDeInit()
  {
  }

  virtual bool isDeInitFinished()
  {
    return False;
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_H__
