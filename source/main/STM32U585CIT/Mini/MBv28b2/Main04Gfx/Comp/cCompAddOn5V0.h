#pragma once

#include "cSysDPool.h"

class cCompAddOn5V0 : public cComponent
{
  public:

  cGpPin mTPS630701_EN;

  cCompAddOn5V0()
    : cComponent(cDepTreeCfg::cComp::n5V0, cDepTreeCfg::cComp::nAddOn),
      mTPS630701_EN(GPIOA_BASE, 6)
  {
  }

  bool bInit() override
  {
    mTPS630701_EN.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mTPS630701_EN.vSet1();

    //kurz warten, für die Spannung zum stabilisieren
    vTaskDelay(pdMS_TO_TICKS(1));

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    mTPS630701_EN.vSet0();
    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




