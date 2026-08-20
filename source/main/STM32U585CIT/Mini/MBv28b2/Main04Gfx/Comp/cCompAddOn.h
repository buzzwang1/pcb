#pragma once

#include "cSysDPool.h"

class cCompAddOn : public cComponent
{
  public:

  cCompAddOn()
    : cComponent(cDepTreeCfg::cComp::nAddOn, cDepTreeCfg::cComp::nBoardMonitor)
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




