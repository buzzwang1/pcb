#pragma once

class cCompAddOnBalancerPio : public cComponent
{
  public:

  cCompAddOnBalancerPio()
    : cComponent(cDepTreeCfg::cComp::nBalPio, cDepTreeCfg::cComp::nBalIna)
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




