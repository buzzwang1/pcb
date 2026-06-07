#pragma once

class cCompAddOn5V0CanFd1 : public cComponent
{
  public:

  cCompAddOn5V0CanFd1()
    : cComponent(cDepTreeCfg::cComp::nCanFd, cDepTreeCfg::cComp::n5V0)
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

  bool bEnable() override
  {
    // return True to signal finished
    return cComponent::bEnable();
  }

  bool bDisable() override
  {
    // return True to signal finished
    return cComponent::bDisable();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




