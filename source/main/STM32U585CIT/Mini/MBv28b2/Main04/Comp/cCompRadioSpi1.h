#pragma once

class cCompRadioSpi1 : public cComponent
{
  public:

  cCompRadioSpi1()
    : cComponent(cDepTreeCfg::cComp::nRadioSpi1, cDepTreeCfg::cComp::nBoard3V3)
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




