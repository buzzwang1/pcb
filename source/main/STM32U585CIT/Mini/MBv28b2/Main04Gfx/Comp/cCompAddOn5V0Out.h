#pragma once

#include "cSysDPool.h"

class cCompAddOn5V0Out : public cComponent
{
  public:

  cGpPin m5VOutCh1;
  cGpPin m5VOutCh2;

  cCompAddOn5V0Out()
    : cComponent(cDepTreeCfg::cComp::n5V0Out, cDepTreeCfg::cComp::n5V0),
      m5VOutCh1(GPIOA_BASE, 5),
      m5VOutCh2(GPIOA_BASE, 4)
  {
  }

  bool bInit() override
  {
    cSysDPool::mBoard.mcAddon.mcOut.mOut5V0Ch1 = &m5VOutCh1;
    cSysDPool::mBoard.mcAddon.mcOut.mOut5V0Ch2 = &m5VOutCh2;

    m5VOutCh1.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    m5VOutCh2.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

    m5VOutCh1.vSet1();
    m5VOutCh2.vSet1();

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    m5VOutCh1.vSet0();
    m5VOutCh2.vSet0();
    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




