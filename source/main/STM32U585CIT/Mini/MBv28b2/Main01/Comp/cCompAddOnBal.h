#pragma once

class cCompAddOnBalancer : public cComponent
{
  public:

  cCompAddOnBalancer()
    : cComponent(cDepTreeCfg::cComp::nBal, cDepTreeCfg::cComp::nBalPio)
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




