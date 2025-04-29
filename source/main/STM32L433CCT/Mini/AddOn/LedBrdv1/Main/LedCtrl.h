#ifndef __WS2812_CONTROL_H__
#define __WS2812_CONTROL_H__

#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_dma.h"

#include "tGPPin.h"
#include "LED.h"
#include "ClockInfo.h"


// PA02: Led_Sig (e.g. WS2812), 12xLEDs, TIM2_CH3, AF1, DMA1:CH1 TIM2 für WS2812   CS:4
// PA15: High side switch for LEDs



#define LEDCTRL_RGB_MEMSIZE(nLedCnt) 3 *  nLedCnt
#define LEDCTRL_LED_MEMSIZE(nLedCnt) 24 * nLedCnt + 1 // + 1 für 0xFF am Ende. Damit high level ausgegeben wird

class cWs2812
{
  public:

  typedef enum
  {
    nStStartupPrepare,
    nStStartupWaitDelay1,
    nStStartupPowerOn,
    nStStartupWaitDelay2,
    nStRunWaitStart,
    nstRunDmaStart,
    nStRunDmaWait,
    nStRunDmaFinished,
    nStDisabled
  }tenStates;

  // --- Pins
  cGpPin mcLedSig;
  cGpPin mcLed1;
  cGpPin mcLed2;
  cGpPin mcLed3;
  // --- WS2812
  u16    mu16LedCnt;
  u8     mu8BrigthnessSet; // 0 .. 100%
  float  mfBrigthness;     // 0 .. 100%


  tenStates mSLedSm;
  u8        mu8SmTick;

  bool   mbEnabled;

  // raw sensor inputs
  u8* mu8GfxData; // Gfx Buffer, 3 Byte per Pxl
  u8* mu8LedData; // Led Buffer, 24 Byte per Pxl

  cWs2812(u8* lu8RgbData, u8* lu8LedData, u16 lu16LedCnt)
    : mcLedSig(GPIOA_BASE, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mcLed1(GPIOA_BASE, 0, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mcLed2(GPIOA_BASE, 1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mcLed3(GPIOA_BASE, 2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    mu8GfxData = lu8RgbData;
    mu8LedData = lu8LedData;
    mu16LedCnt = lu16LedCnt;

    mu8BrigthnessSet   = 16;  //128; // 50%
    mfBrigthness       = 0.0f;   // 0%

    for (u16 lu16t = 0; lu16t < LEDCTRL_RGB_MEMSIZE(mu16LedCnt); lu16t++)
    {
      mu8GfxData[lu16t] = 3;
    }

    vShow();

    mcLedSig.vSetAF(GPIO_MODE_AF_PP, GPIO_AF14_TIM16);
    mcLed1.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM2);
    mcLed2.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM2);
    mcLed3.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM2);

    Dma_vInit();
    vInit_Tim2_PWM();
    vInit_Tim16_WS2812();

    mbEnabled = True;
    vDisable();
    mSLedSm = nStDisabled;
  }


  void vSetBrigthness(u8 lu8Val)
  {
    if (lu8Val > 100) lu8Val = 100;
    mu8BrigthnessSet = lu8Val;
    if (mu8BrigthnessSet > 0)
    {
      vEnable();
    }
  }

  virtual void vBrigthnessCtrl()
  {
    if (mfBrigthness < mu8BrigthnessSet)
    {
      mfBrigthness += 0.25f;
    }
    else
      if (mfBrigthness > mu8BrigthnessSet)
      {
        mfBrigthness -= 0.25f;
        if (mfBrigthness <= 0.0f)
        {
          mfBrigthness = 0.0f;
        }
      }
  }


  void vShow()
  {
    u16 lu16LedIdx = 0;
    u8  lu8NewValR;
    u8  lu8NewValG;
    u8  lu8NewValB;

    for (u16 lu16t = 0; lu16t < mu16LedCnt; lu16t++)
    {
      lu8NewValR = (u8)((float)mu8GfxData[lu16LedIdx] * mfBrigthness / 100.0f); lu16LedIdx++;
      lu8NewValG = (u8)((float)mu8GfxData[lu16LedIdx] * mfBrigthness / 100.0f); lu16LedIdx++;
      lu8NewValB = (u8)((float)mu8GfxData[lu16LedIdx] * mfBrigthness / 100.0f); lu16LedIdx++;

      vSetLedMem(lu16t, lu8NewValR, lu8NewValG, lu8NewValB);
    }
    mu8LedData[LEDCTRL_LED_MEMSIZE(mu16LedCnt) - 1] = 0x00;
  }

  void vSLedSm_10ms()
  {
    bool lbLoop = True;

    while (lbLoop)
    {
      lbLoop = False;

      switch(mSLedSm)
      {
        case nStStartupPrepare:
          // Hier sollte Power aus sein
          mu8SmTick            = 0;
          mfBrigthness         = 0.0f;
          TIM16->CCR1 = 0x0; // WS2812 Sig = Gnd
          vSetPwm(1, 0);

          mSLedSm = nStStartupWaitDelay1;
          break;
        case nStStartupWaitDelay1:
          mu8SmTick++;
          if (mu8SmTick > 10) // 100ms
          {
            mu8SmTick = 0;
            mSLedSm = nStStartupPowerOn;
            lbLoop = True;
          }
          break;
        case nStStartupPowerOn:
          vSetPwm(1, 100);
          mSLedSm = nStStartupWaitDelay2;
          lbLoop = True;
          break;
        case nStStartupWaitDelay2:
          mu8SmTick++;
          if (mu8SmTick > 10) // 100ms
          {
            mu8SmTick = 0;
            mSLedSm = nStRunWaitStart;
          }
          break;
        
          // -------------- Run --------
        case nStRunWaitStart:
          vBrigthnessCtrl();
          vShow();
          TIM16->CCR1 = 0; //Reset WS2812: 10ms '0' mit dem Timer ausgeben

          mSLedSm = nstRunDmaStart;
          break;
        case nstRunDmaStart:
          //DMA Ausgabe starten. Wird von DMA IRQ beendet
          DMA1_Channel3->CNDTR  = LEDCTRL_LED_MEMSIZE(mu16LedCnt);
          DMA1_Channel3->CMAR   = (u32)mu8LedData;
          DMA1_Channel3->CCR   |= (DMA_CCR_EN | DMA_CCR_TCIE);
          TIM16->DIER |= (TIM_DMA_UPDATE);
          mSLedSm = nStRunDmaWait;
          lbLoop = True;
          break;
        case nStRunDmaWait:
          break;
        case nStRunDmaFinished:
          // Wird vom DMA gesetzt
          if ((mfBrigthness <= 0.0f) && (mu8BrigthnessSet == 0))
          {
            mSLedSm = nStDisabled;
            lbLoop = True;
          }
          else
          {
            mSLedSm = nStRunWaitStart;
          }          
          break;

        case nStDisabled:
          vDisable();
          break;
      }
    }
  }



  void vTick10msLp(void)
  {
    vSLedSm_10ms();
  }


  void vSetLedPxl(u16 lu16LedIdx, u8 lu8R, u8 lu8G, u8 lu8B)
  {
    lu16LedIdx = 3 * lu16LedIdx;

    mu8GfxData[lu16LedIdx] = lu8R; lu16LedIdx++;
    mu8GfxData[lu16LedIdx] = lu8G; lu16LedIdx++;
    mu8GfxData[lu16LedIdx] = lu8B;
  }


  void vLedXY(u8 x, u8 y, u8 r, u8 g, u8 b)
  {
    u16 Idx = y * 16 + x;
    vSetLedPxl(Idx, r, g, b);
  }


  void vRect(u8 x1, u8 y1, u8 x2, u8 y2, u8 r, u8 g, u8 b)
  {
    u8 x, y;

    for (x = x1; x <= x2; x++)
    {
      vLedXY(x, y1, r, g, b);
      vLedXY(x, y2, r, g, b);
    }

    for (y = (y1 + 1); y <= (y2 - 1); y++)
    {
      vLedXY(x1, y, r, g, b);
      vLedXY(x2, y, r, g, b);
    }
  }

  void vSetLedMem(u16 lu16LedIdx, u8 lu8R, u8 lu8G, u8 lu8B)
  {
    lu16LedIdx = 24 * lu16LedIdx;

    if (lu8G & 128) mu8LedData[lu16LedIdx + 0] = 7; else mu8LedData[lu16LedIdx + 0] = 3;
    if (lu8G &  64) mu8LedData[lu16LedIdx + 1] = 7; else mu8LedData[lu16LedIdx + 1] = 3;
    if (lu8G &  32) mu8LedData[lu16LedIdx + 2] = 7; else mu8LedData[lu16LedIdx + 2] = 3;
    if (lu8G &  16) mu8LedData[lu16LedIdx + 3] = 7; else mu8LedData[lu16LedIdx + 3] = 3;
    if (lu8G &   8) mu8LedData[lu16LedIdx + 4] = 7; else mu8LedData[lu16LedIdx + 4] = 3;
    if (lu8G &   4) mu8LedData[lu16LedIdx + 5] = 7; else mu8LedData[lu16LedIdx + 5] = 3;
    if (lu8G &   2) mu8LedData[lu16LedIdx + 6] = 7; else mu8LedData[lu16LedIdx + 6] = 3;
    if (lu8G &   1) mu8LedData[lu16LedIdx + 7] = 7; else mu8LedData[lu16LedIdx + 7] = 3;

    lu16LedIdx += 8;

    if (lu8R & 128) mu8LedData[lu16LedIdx + 0] = 7; else mu8LedData[lu16LedIdx + 0] = 3;
    if (lu8R &  64) mu8LedData[lu16LedIdx + 1] = 7; else mu8LedData[lu16LedIdx + 1] = 3;
    if (lu8R &  32) mu8LedData[lu16LedIdx + 2] = 7; else mu8LedData[lu16LedIdx + 2] = 3;
    if (lu8R &  16) mu8LedData[lu16LedIdx + 3] = 7; else mu8LedData[lu16LedIdx + 3] = 3;
    if (lu8R &   8) mu8LedData[lu16LedIdx + 4] = 7; else mu8LedData[lu16LedIdx + 4] = 3;
    if (lu8R &   4) mu8LedData[lu16LedIdx + 5] = 7; else mu8LedData[lu16LedIdx + 5] = 3;
    if (lu8R &   2) mu8LedData[lu16LedIdx + 6] = 7; else mu8LedData[lu16LedIdx + 6] = 3;
    if (lu8R &   1) mu8LedData[lu16LedIdx + 7] = 7; else mu8LedData[lu16LedIdx + 7] = 3;

    lu16LedIdx += 8;

    if (lu8B & 128) mu8LedData[lu16LedIdx + 0] = 7; else mu8LedData[lu16LedIdx + 0] = 3;
    if (lu8B &  64) mu8LedData[lu16LedIdx + 1] = 7; else mu8LedData[lu16LedIdx + 1] = 3;
    if (lu8B &  32) mu8LedData[lu16LedIdx + 2] = 7; else mu8LedData[lu16LedIdx + 2] = 3;
    if (lu8B &  16) mu8LedData[lu16LedIdx + 3] = 7; else mu8LedData[lu16LedIdx + 3] = 3;
    if (lu8B &   8) mu8LedData[lu16LedIdx + 4] = 7; else mu8LedData[lu16LedIdx + 4] = 3;
    if (lu8B &   4) mu8LedData[lu16LedIdx + 5] = 7; else mu8LedData[lu16LedIdx + 5] = 3;
    if (lu8B &   2) mu8LedData[lu16LedIdx + 6] = 7; else mu8LedData[lu16LedIdx + 6] = 3;
    if (lu8B &   1) mu8LedData[lu16LedIdx + 7] = 7; else mu8LedData[lu16LedIdx + 7] = 3;
  }

  void vInit_Tim2_PWM(void)
  {
    __HAL_RCC_TIM2_CLK_ENABLE();

    TIM_HandleTypeDef TimHandle = {};

    /* Set TIM1 instance */
    TimHandle.Instance = TIM2;

    /* Initialize TIM2 peripheral as follows:
         + Period = 10000 - 1
         + Prescaler = (SystemCoreClock/1000000) - 1
         + ClockDivision = 0
         + Counter direction = Up
    */
    TimHandle.Init.Prescaler = (cClockInfo::mstClocks.HCLK_Frequency / 1000000) - 1; // 1us = 1Mhz
    TimHandle.Init.Period = 100 - 1; // 1Mhz/100 = 10khz
    //TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    //TimHandle.Init.RepetitionCounter = 0;

    HAL_TIM_Base_Init(&TimHandle);

    TIM_OC_InitTypeDef sConfigOC = {};
    HAL_TIM_PWM_Init(&TimHandle);

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_OC_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_3);

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    HAL_TIM_Base_Start(&TimHandle);

    // Complementary
    //HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_1);
    //HAL_TIMEx_PWMN_Start(&TimHandle, TIM_CHANNEL_2);

    // Normal
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
  }

  void vSetPwmLed1(u8 lu8PwmLed)
  {
    if (lu8PwmLed > 100) lu8PwmLed = 100;
    TIM2->CCR1 = lu8PwmLed;
  }

  void vSetPwmLed2(u8 lu8PwmLed)
  {
    if (lu8PwmLed > 100) lu8PwmLed = 100;
    TIM2->CCR2 = lu8PwmLed;
  }

  void vSetPwmLed3(u8 lu8PwmLed)
  {
    if (lu8PwmLed > 100) lu8PwmLed = 100;
    TIM2->CCR3 = lu8PwmLed;
  }

  u8 u8GetPwmLed1() { return TIM2->CCR1; }
  u8 u8GetPwmLed2() { return TIM2->CCR2; }
  u8 u8GetPwmLed3() { return TIM2->CCR3; }

  void vSetPwm(u8 lu8LedIdx, u8 lu8PwmValue)
  {
    switch (lu8LedIdx)
    {
      case 1: vSetPwmLed1(lu8PwmValue); break;
      case 2: vSetPwmLed2(lu8PwmValue); break;
      case 3: vSetPwmLed3(lu8PwmValue); break;
    }
  }

  u8 u8GetPwm(u8 lu8LedIdx)
  {
    switch (lu8LedIdx)
    {
      case 1: return u8GetPwmLed1(); break;
      case 2: return u8GetPwmLed2(); break;
      case 3: return u8GetPwmLed3(); break;
    }
    return 0;
  }

  void vSetPwm(u8 lu8PwmLed1, u8 lu8PwmLed2, u8 lu8PwmLed3)
  {
    vSetPwmLed1(lu8PwmLed1);
    vSetPwmLed2(lu8PwmLed2);
    vSetPwmLed3(lu8PwmLed3);
  }

  void vEnable()
  {
    if (!mbEnabled)
    {
      mbEnabled = True;
      mSLedSm = nStStartupPrepare;
    }
  }

  void vDisable()
  {
    if (mbEnabled)
    {
      mbEnabled = False;

      TIM16->DIER        &= ~(TIM_DMA_UPDATE);
      DMA1_Channel3->CCR &= ~DMA_CCR_EN;
      DMA1->IFCR          = DMA_FLAG_TC3;

      vSetPwm(1, 0);
      TIM16->CCR1 = 0x00;
    }
  }

  void vInit_Tim16_WS2812(void)
  {
    __HAL_RCC_TIM16_CLK_ENABLE();

    TIM_HandleTypeDef TimHandle = {};

    /* Set TIM1 instance */
    TimHandle.Instance = TIM16;

    /* Initialize TIM2 peripheral as follows:
         + Period = 10000 - 1
         + Prescaler = (SystemCoreClock/1000000) - 1
         + ClockDivision = 0
         + Counter direction = Up
    */
    TimHandle.Init.Prescaler = (cClockInfo::mstClocks.HCLK_Frequency / 8000000) - 1; // 0,125 us = 8Mhz
    TimHandle.Init.Period            = 10 - 1; // => 1,25 is = 800khz
    TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    //TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    TimHandle.Init.RepetitionCounter = 0;

    HAL_TIM_Base_Init(&TimHandle);

    TIM_OC_InitTypeDef sConfigOC = {};
    HAL_TIM_PWM_Init(&TimHandle);

    sConfigOC.OCMode      = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode  = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_1);

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    HAL_TIM_Base_Start(&TimHandle);


    __HAL_TIM_ENABLE_OCxPRELOAD(&TimHandle, TIM_DMA_CC3);
    __HAL_TIM_ENABLE_DMA(&TimHandle, TIM_DMA_CC3);

    TIM16->CR2 |= TIM_CR2_CCDS;
    TIM16->CCR1 = 0x0;

    // Normal
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
  }

  void Dma_vIrqTc()
  {
    TIM16->DIER &= ~(TIM_DMA_UPDATE);
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR          =  DMA_FLAG_TC3;

    mSLedSm = nStRunDmaFinished;
    vSLedSm_10ms();
  }

  // TIM2 für WS2812   CS:4
  void Dma_vInit(void)
  {
    __HAL_RCC_DMA1_CLK_ENABLE();

    DMA_HandleTypeDef lhDMA = {};

    lhDMA.Instance = DMA1_Channel3;

    lhDMA.Init.Request             = DMA_REQUEST_4;
    lhDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
    lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
    lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    lhDMA.Init.Mode                = DMA_NORMAL;
    lhDMA.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Configure DMA Channel destination address */
    lhDMA.Instance->CPAR = (u32) & (TIM16->CCR1);

    HAL_DMA_Init(&lhDMA);

    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 8, 8);
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
  }
};


#endif  //__WS2812_CONTROL_H__
