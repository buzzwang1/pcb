#pragma once

#include "cSysDPool.h"

class cCompAddOnBatOut : public cComponent
{
  public:

  cGpPin mBatOut;

  cCompAddOnBatOut()
    : cComponent(cDepTreeCfg::cComp::nBatOut, cDepTreeCfg::cComp::nAddOn),
      mBatOut(GPIOC_BASE, 13)
  {
  }

  bool bInit() override
  {
    mBatOut.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

    mBatOut.vSet1();

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    mBatOut.vSet0();

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




