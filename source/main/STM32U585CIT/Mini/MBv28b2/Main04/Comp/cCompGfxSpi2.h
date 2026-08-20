#pragma once

#include "FreeRTOS.h"
#include "cSysDPool.h"
#include "cSPi.h"


class cCompGfxSpi2 : public cComponent
{
  public:

  cGpPin mSCK;
  cGpPin mMISO;
  cGpPin mMOSI;
  cSpiMaster mcSpi2;

  cCompGfxSpi2()
    : cComponent(cDepTreeCfg::cComp::nGfxSpi2, cDepTreeCfg::cComp::n5V0),
      mSCK(GPIOA_BASE, 9),
      mMISO(GPIOD_BASE, 3),
      mMOSI(GPIOD_BASE, 4),
      mcSpi2(SPI2, 6, 7, 128, 0) // 160Mhz / 128 = 1,25MHz
     
  {
    cSysDPool::mBoard.mcSpi2 = &mcSpi2;
  }

  bool bInit() override
  {
    mSCK.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mMISO.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mMOSI.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    mSCK.vSetAF(GPIO_MODE_AF_PP, GPIO_AF3_SPI2);   // SCK
    mMISO.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI2);  // MISO
    mMOSI.vSetAF(GPIO_MODE_AF_PP, GPIO_AF5_SPI2);  // MOSI

    cSysDPool::mBoard.mcSpi2->vInit();

    cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList1ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    mcSpi2.vTick1ms();
    // return True to signal finished
    return cComponent::bRun();
  }
};


#ifdef __cplusplus
extern "C" {
#endif

  extern void SPI2_IRQHandler(void);

  void SPI2_IRQHandler(void)
  {
    cSysDPool::mBoard.mcSpi2->vIRQHandler();
  }


#ifdef __cplusplus
}
#endif

