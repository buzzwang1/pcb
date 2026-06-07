#pragma once

class cCompBoardI2C2 : public cComponent
{
  public:

  cCompBoardI2C2()
    : cComponent(cDepTreeCfg::cComp::nBoardI2C2, cDepTreeCfg::cComp::nBoard3V3)
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




