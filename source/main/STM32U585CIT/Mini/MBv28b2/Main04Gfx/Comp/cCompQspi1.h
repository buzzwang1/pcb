#pragma once

#include "cSysDPool.h"

cZD25WQ32 mcZD25WQ32;

class cCompQSpi1 : public cComponent
{
  public:

  cCompQSpi1()
    : cComponent(cDepTreeCfg::cComp::nQSpi1, { cDepTreeCfg::cComp::nBoard3V3 })
  {
    cSysDPool::mFs.mpcZD25WQ32 = &mcZD25WQ32;
  }

  bool bInit() override
  {
    mcZD25WQ32.i8StartInit();
    mcZD25WQ32.vDoProcess(0);

    while (!mcZD25WQ32.isIdle())
    {
      if (mcZD25WQ32.isError()) break;
      vTaskDelay(pdMS_TO_TICKS(1));
      mcZD25WQ32.vDoProcess(1000);
    }

    //mcZD25WQ32.i8StartRead(mcZD25WQ32.u32GetBaseAdr(), cSysDPool::mFs.mau8Sector, 4096);

    //cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    //cComponentList::mcList1ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    //mcZD25WQ32.vDoProcess(1000);
    return cComponent::bRun();
  }
};

