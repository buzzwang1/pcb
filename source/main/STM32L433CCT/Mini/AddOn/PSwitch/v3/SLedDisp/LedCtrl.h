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

  // --- Pins
  cGpPin mcLedSig;
  cGpPin mcEn;

  // --- WS2812
  u16    mu16LedCnt;
  u8     mu8BrigthnessSet; // 0..255 = 0 .. 100%
  u8     mu8Brigthness;    // 0..255 = 0 .. 100%

  u8     mu8StateProcess;

  // raw sensor inputs
  u8* mu8GfxData; // Gfx Buffer, 3 Byte per Pxl
  u8* mu8LedData; // Led Buffer, 24 Byte per Pxl

  cWs2812(u8* lu8RgbData, u8* lu8LedData, u16 lu16LedCnt)
    : mcLedSig(GPIOA_BASE,  2, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0),
      mcEn(    GPIOA_BASE, 15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    mu8GfxData = lu8RgbData;
    mu8LedData = lu8LedData;
    mu16LedCnt = lu16LedCnt;

    mu8BrigthnessSet = 16;  //128; // 50%
    mu8Brigthness    = 0;   // 0%

    for (u16 lu16t = 0; lu16t < LEDCTRL_RGB_MEMSIZE(mu16LedCnt); lu16t++)
    {
      mu8GfxData[lu16t] = 0;
    }

    vShow();

    mcLedSig.vSetAF(GPIO_MODE_AF_PP, GPIO_AF1_TIM2);

    Dma_vInit();
    vInit_Tim2_WS2812();
  }


  void vSetBrigthness(u8 lu8Val)
  {
    mu8BrigthnessSet = lu8Val;
  }

  virtual void vBrigthnessCtrl()
  {
    if (mu8Brigthness < mu8BrigthnessSet) mu8Brigthness++;
    else
    if (mu8Brigthness > mu8BrigthnessSet) mu8Brigthness--;
  }

  void vTurnOnOffCtrl(void)
  {
    switch (mu8StateProcess)
    {
      case 1: //Reset WS2812: 10ms '0' mit dem Timer ausgeben
        TIM2->CCR3      = 0;
        mu8StateProcess = 2;
        break;
      case 2: //DMA Ausgabe starten. Wird von DMA IRQ beendet
        // Start DMA
        TIM2->EGR |= TIM_EGR_UG; // Setze Udate flag

        DMA1_Channel1->CNDTR = LEDCTRL_LED_MEMSIZE(mu16LedCnt);
        DMA1_Channel1->CMAR  = (u32)mu8LedData;
        DMA1_Channel1->CCR  |= (DMA_CCR_EN | DMA_CCR_TCIE);
        mu8StateProcess = 3;
        break;
      case 3:
        //Wait for DMA
        mu8StateProcess = 0;
        break;
      case 4:
        mu8StateProcess = 0;
        break;
      default:
        mu8StateProcess = 0;
    }
  }


  void vShow()
  {
    u16 lu16LedIdx = 0;
    u16 lu16NewValR;
    u16 lu16NewValG;
    u16 lu16NewValB;

    if (mu8StateProcess == 0)
    {
      for (u16 lu16t = 0; lu16t < mu16LedCnt; lu16t++)
      {
        lu16NewValR = (u16)mu8GfxData[lu16LedIdx] * (u16)mu8Brigthness; lu16LedIdx++;
        lu16NewValG = (u16)mu8GfxData[lu16LedIdx] * (u16)mu8Brigthness; lu16LedIdx++;
        lu16NewValB = (u16)mu8GfxData[lu16LedIdx] * (u16)mu8Brigthness; lu16LedIdx++;

        lu16NewValR /= 256;
        lu16NewValG /= 256;
        lu16NewValB /= 256;

        vSetLedMem(lu16t, lu16NewValR, lu16NewValG, lu16NewValB);
      }
      mu8LedData[LEDCTRL_LED_MEMSIZE(mu16LedCnt) - 1] = 0xFF;

      mu8StateProcess = 1;
    }
  }

  void vTick10msLp(void)
  {
    vBrigthnessCtrl();
    vShow();
    vTurnOnOffCtrl();
  }

  void vSetLedPxl(u16 lu16LedIdx, u8 lu8R, u8 lu8G, u8 lu8B)
  {
    lu16LedIdx = 3 * lu16LedIdx;
      
    mu8GfxData[lu16LedIdx] = lu8R; lu16LedIdx++;
    mu8GfxData[lu16LedIdx] = lu8G; lu16LedIdx++;
    mu8GfxData[lu16LedIdx] = lu8B;
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

  void vEnable()
  {
    mcEn.vSet1();
  }

  void vDisable()
  {
    mcEn.vSet0();
  }

  void vInit_Tim2_WS2812(void)
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
    TimHandle.Init.Prescaler = (cClockInfo::mstClocks.HCLK_Frequency / 8000000) - 1; // 0,125 us = 8Mhz
    TimHandle.Init.Period = 10 - 1; // => 1,25 is = 800khz
    //TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    //TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    //TimHandle.Init.RepetitionCounter = 0;

    HAL_TIM_Base_Init(&TimHandle);

    TIM_OC_InitTypeDef sConfigOC = {};
    HAL_TIM_PWM_Init(&TimHandle);

    sConfigOC.OCMode      = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity  = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode  = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_3);

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    HAL_TIM_Base_Start(&TimHandle);


    __HAL_TIM_ENABLE_OCxPRELOAD(&TimHandle, TIM_DMA_CC3);
    __HAL_TIM_ENABLE_DMA(&TimHandle, TIM_DMA_CC3);

    TIM2->CR2 |= TIM_CR2_CCDS;

    // Normal
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_3);
  }

  void Dma_vIrqTc()
  {
    DMA1_Channel1->CCR &= ~DMA_CCR_EN;
    DMA1->IFCR          =  DMA_FLAG_TC1;
    mu8StateProcess = 4;
    vTurnOnOffCtrl();
  }

  // TIM2 für WS2812   CS:4
  void Dma_vInit(void)
  {
    __HAL_RCC_DMA1_CLK_ENABLE();

    DMA_HandleTypeDef lhDMA = {};

    lhDMA.Instance = DMA1_Channel1;

    lhDMA.Init.Request             = DMA_REQUEST_4;
    lhDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
    lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
    lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    lhDMA.Init.Mode                = DMA_NORMAL;
    lhDMA.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Configure DMA Channel destination address */
    lhDMA.Instance->CPAR = (u32) & (TIM2->CCR3);

    HAL_DMA_Init(&lhDMA);

    /* Set the TIMx priority */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 8, 8);
    /* Enable the TIMx global Interrupt */
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  }
};


#endif  //__WS2812_CONTROL_H__
