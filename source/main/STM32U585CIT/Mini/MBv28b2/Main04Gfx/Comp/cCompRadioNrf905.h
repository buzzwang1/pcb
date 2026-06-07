#pragma once

class cCompRadioNrf905 : public cComponent
{
  public:

  cCompRadioNrf905()
    : cComponent(cDepTreeCfg::cComp::nRadioNrf905, cDepTreeCfg::cComp::nRadioSpi1)
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




