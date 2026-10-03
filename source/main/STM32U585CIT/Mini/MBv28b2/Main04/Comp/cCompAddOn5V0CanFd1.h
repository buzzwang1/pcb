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
    RCC_PeriphCLKInitTypeDef PeriphClkInit;

    cMemTools::vMemSet((u8*)&PeriphClkInit, 0, sizeof(PeriphClkInit));
    
    mCanTx.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mCanRx.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    mCanTx.vSetAF(GPIO_MODE_AF_PP, GPIO_AF9_FDCAN1);  // SCK
    mCanRx.vSetAF(GPIO_MODE_AF_PP, GPIO_AF9_FDCAN1);  // MISO

    // FDCAN-Taktquelle auf PLL1Q setzen (LL_RCC_FDCAN_CLKSOURCE_PLL1)
    // -------------------------------------------------------------------------
    // Das FDCANSEL-Bitfeld befindet sich im RCC_CCIPR1-Register.
    // Bits 25:24 im CCIPR1: 00 = HSE, 01 = PLL1_Q, 10 = PLL2_Q
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN1;
    PeriphClkInit.Fdcan1ClockSelection = RCC_FDCAN1CLKSOURCE_PLL1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      //Error_Handler();
    }

    /* Peripheral clock enable */
    __HAL_RCC_FDCAN1_CLK_ENABLE();

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    __HAL_RCC_FDCAN1_CLK_DISABLE();

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




