#ifndef __WS2812_CONTROL_H__
#define __WS2812_CONTROL_H__

#include "TypeDef.h"
#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_dma.h"

#include "tGPPin.h"
#include "LED.h"
#include "ClockInfo.h"


class cWs2812
{
  public:

  // --- Pins
  cGpPin mcLedSig;

  enum
  {
    nResetCnt = 24 * 3,
    nDataCnt  = 24 * (256 + 16)
  };

  // raw sensor inputs
  u8 u8LedData[nResetCnt + nDataCnt];

  cWs2812()
    : mcLedSig(GPIOA_BASE, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    for (u16 lu16t = 0; lu16t < sizeof(u8LedData); lu16t++)
    {
      u8LedData[lu16t] = 3;
    }
    for (u16 lu16t = 0; lu16t < nResetCnt; lu16t++)
    {
      u8LedData[lu16t] = 0;
    }

    mcLedSig.vSetAF(GPIO_MODE_AF_PP, GPIO_AF14_TIM16);
    Dma_vInit();
    TIM_vInit();
  }

  void vSetLed(u16 lu16LedIdx, u8 lu8R, u8 lu8G, u8 lu8B)
  {
    lu16LedIdx = 24 * lu16LedIdx + nResetCnt;
  
    if (lu8G & 128) u8LedData[lu16LedIdx + 0] = 7; else u8LedData[lu16LedIdx + 0] = 3;
    if (lu8G &  64) u8LedData[lu16LedIdx + 1] = 7; else u8LedData[lu16LedIdx + 1] = 3; 
    if (lu8G &  32) u8LedData[lu16LedIdx + 2] = 7; else u8LedData[lu16LedIdx + 2] = 3;
    if (lu8G &  16) u8LedData[lu16LedIdx + 3] = 7; else u8LedData[lu16LedIdx + 3] = 3;
    if (lu8G &   8) u8LedData[lu16LedIdx + 4] = 7; else u8LedData[lu16LedIdx + 4] = 3;
    if (lu8G &   4) u8LedData[lu16LedIdx + 5] = 7; else u8LedData[lu16LedIdx + 5] = 3;
    if (lu8G &   2) u8LedData[lu16LedIdx + 6] = 7; else u8LedData[lu16LedIdx + 6] = 3;
    if (lu8G &   1) u8LedData[lu16LedIdx + 7] = 7; else u8LedData[lu16LedIdx + 7] = 3;

    lu16LedIdx += 8;

    if (lu8R & 128) u8LedData[lu16LedIdx + 0] = 7; else u8LedData[lu16LedIdx + 0] = 3;
    if (lu8R &  64) u8LedData[lu16LedIdx + 1] = 7; else u8LedData[lu16LedIdx + 1] = 3;
    if (lu8R &  32) u8LedData[lu16LedIdx + 2] = 7; else u8LedData[lu16LedIdx + 2] = 3;
    if (lu8R &  16) u8LedData[lu16LedIdx + 3] = 7; else u8LedData[lu16LedIdx + 3] = 3;
    if (lu8R &   8) u8LedData[lu16LedIdx + 4] = 7; else u8LedData[lu16LedIdx + 4] = 3;
    if (lu8R &   4) u8LedData[lu16LedIdx + 5] = 7; else u8LedData[lu16LedIdx + 5] = 3;
    if (lu8R &   2) u8LedData[lu16LedIdx + 6] = 7; else u8LedData[lu16LedIdx + 6] = 3;
    if (lu8R &   1) u8LedData[lu16LedIdx + 7] = 7; else u8LedData[lu16LedIdx + 7] = 3;

    lu16LedIdx += 8;

    if (lu8B & 128) u8LedData[lu16LedIdx + 0] = 7; else u8LedData[lu16LedIdx + 0] = 3;
    if (lu8B &  64) u8LedData[lu16LedIdx + 1] = 7; else u8LedData[lu16LedIdx + 1] = 3;
    if (lu8B &  32) u8LedData[lu16LedIdx + 2] = 7; else u8LedData[lu16LedIdx + 2] = 3;
    if (lu8B &  16) u8LedData[lu16LedIdx + 3] = 7; else u8LedData[lu16LedIdx + 3] = 3;
    if (lu8B &   8) u8LedData[lu16LedIdx + 4] = 7; else u8LedData[lu16LedIdx + 4] = 3;
    if (lu8B &   4) u8LedData[lu16LedIdx + 5] = 7; else u8LedData[lu16LedIdx + 5] = 3;
    if (lu8B &   2) u8LedData[lu16LedIdx + 6] = 7; else u8LedData[lu16LedIdx + 6] = 3;
    if (lu8B &   1) u8LedData[lu16LedIdx + 7] = 7; else u8LedData[lu16LedIdx + 7] = 3;
  }

  void TIM_vInit(void)
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
    TimHandle.Init.Prescaler         = (cClockInfo::mstClocks.HCLK_Frequency / 8000000) -1; // 0,125 us = 8Mhz
    TimHandle.Init.Period            = 10 - 1; // => 1,25 is = 800khz
    //TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    //TimHandle.Init.RepetitionCounter = 0;

    HAL_TIM_Base_Init(&TimHandle);

    TIM_OC_InitTypeDef sConfigOC = {};
    HAL_TIM_PWM_Init(&TimHandle);

    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&TimHandle, &sConfigOC, TIM_CHANNEL_1);

    /*##-2- Start the TIM Base generation in interrupt mode ####################*/
    HAL_TIM_Base_Start(&TimHandle);

   __HAL_TIM_ENABLE_DMA(&TimHandle, TIM_DMA_CC1);
    
    // Normal
    HAL_TIM_PWM_Start(&TimHandle, TIM_CHANNEL_1);
  }

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
    lhDMA.Init.Mode                = DMA_CIRCULAR;
    lhDMA.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Configure DMA Channel data length */
    lhDMA.Instance->CNDTR = sizeof(u8LedData);
    /* Configure DMA Channel destination address */
    lhDMA.Instance->CPAR = (u32)&(TIM16->CCR1);
    /* Configure DMA Channel source address */
    lhDMA.Instance->CMAR = (u32)u8LedData;

    HAL_DMA_Init(&lhDMA);
    DMA1_Channel3->CCR |= DMA_CCR_EN;
  }
};


#endif  //__WS2812_CONTROL_H__
