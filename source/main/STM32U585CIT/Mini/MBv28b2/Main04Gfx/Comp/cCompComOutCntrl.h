#pragma once

class cCompComOutCntrl : public cComponent
{
  public:

  cCompComOutCntrl()
    : cComponent(cDepTreeCfg::cComp::nComOutCntrl, cDepTreeCfg::cComp::nBoard3V3)
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




