#pragma once

#include "cSysDPool.h"

class cCompComSideCanFd1Offline : public cComponent
{
  public:

  cCompComSideCanFd1Offline()
    : cComponent(cDepTreeCfg::cComp::nComSideOffline, { cDepTreeCfg::cComp::nCom, cDepTreeCfg::cComp::nCanFd })
  {
  }

  bool bInit() override
  {
    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




