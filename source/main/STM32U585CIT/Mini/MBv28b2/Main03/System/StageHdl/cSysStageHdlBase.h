#ifndef __SYSTEM_STAGE_HANDLER_BASE_H__
#define __SYSTEM_STAGE_HANDLER_BASE_H__

#include "TypeDef.h"

#ifdef __cplusplus
  extern "C" {
#endif


class cSysStageHdlBase
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

  cSysStageHdlBase*mcpNext;
  cSysStageHdlBase* mcpBefore;

  cSysStageHdlBase(cSysStageHdlBase* lcpNext)
  {
    mcpNext = lcpNext;

    if (lcpNext != null)
    {
      lcpNext->mcpBefore = this;
    }

    menState = UnInited;
  }

  virtual void vRequestStart()
  {
  }

  virtual u8 u8GetRequest()
  {
    return 0;
  }

  void vSetInitStarted()
  {
    menState = InitStarted;
    if (!u8GetRequest())
    {
      vSetInitFinished();
    }
  }

  void vSetInitFinished()
  {
    menState = InitFinished;
  }

  virtual void vStartInit()
  {
  }

  virtual bool isInitFinished()
  {
    return False;
  }


  void vSetDeInitStarted()
  {    
    if (!u8GetRequest())
    {
      menState = DeInitStarted;
    }
  }

  void vSetDeInitFinished()
  {
    menState = DeInitFinished;
  }

  virtual void vDoDeInit()
  {
  }

  virtual void vDoneDeInit()
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

#endif  //__SYSTEM_STAGE_HANDLER_BASE_H__
