#pragma once

class cCompAddOn5V0CanFd1 : public cComponent
{
  public:

  cGpPin mCanTx;
  cGpPin mCanRx;

  cCompAddOn5V0CanFd1()
    : cComponent(cDepTreeCfg::cComp::nCanFd, { cDepTreeCfg::cComp::n5V0 }),
      mCanTx(GPIOD_BASE, 1),
      mCanRx(GPIOD_BASE, 0)
  {
  }

  bool bInit() override
  {
    mCanTx.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mCanRx.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    mCanTx.vSetAF(GPIO_MODE_AF_PP, GPIO_AF9_FDCAN1);  // SCK
    mCanRx.vSetAF(GPIO_MODE_AF_PP, GPIO_AF9_FDCAN1);  // MISO

    // FDCAN-Taktquelle auf PLL1Q setzen (LL_RCC_FDCAN_CLKSOURCE_PLL1)
    // -------------------------------------------------------------------------
    // Das FDCANSEL-Bitfeld befindet sich im RCC_CCIPR1-Register.
    // Bits 25:24 im CCIPR1: 00 = HSE, 01 = PLL1_Q, 10 = PLL2_Q
    LL_RCC_SetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE_PLL1);

    // FDCAN1 Clock Enable (im RCC_APB1ENR1)
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_FDCAN1);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




