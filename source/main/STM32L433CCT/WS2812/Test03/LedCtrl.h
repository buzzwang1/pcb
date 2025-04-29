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
    nResetCnt = 3 * 4,
    nDataCnt  = 256 * 3,
  };

  // raw sensor inputs
  u8 *mu8LedData;

  cWs2812(u8* lu8LedData)
    : mcLedSig(GPIOA_BASE, 6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0)
  {
    mu8LedData = lu8LedData;
    for (u16 lu16t = 0; lu16t < nResetCnt + nDataCnt; lu16t++)
    {
      mu8LedData[lu16t] = 3;
    }
    for (u16 lu16t = 0; lu16t < nResetCnt; lu16t++)
    {
      mu8LedData[lu16t] = 0;
    }

    mcLedSig.vSetAF(GPIO_MODE_AF_PP, GPIO_AF14_TIM16);
    Dma_vInit();
    TIM_vInit();
  }

  void vSetLed(u16 lu16LedIdx, u8 lu8R, u8 lu8G, u8 lu8B)
  {
    lu16LedIdx += nResetCnt;  
    mu8LedData[lu16LedIdx + 0] = lu8G;
    mu8LedData[lu16LedIdx + 1] = lu8R;
    mu8LedData[lu16LedIdx + 2] = lu8B;
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
    TimHandle.Init.Period            = 10 - 1; // => 1,25us is = 800khz
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
    lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD; //BitBand is 32Bit wide
    lhDMA.Init.Mode                = DMA_CIRCULAR;
    lhDMA.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Configure DMA Channel data length */
    lhDMA.Instance->CNDTR = nResetCnt + nDataCnt;
    /* Configure DMA Channel destination address */
    lhDMA.Instance->CPAR = (u32)&(TIM16->CCR1);
    /* Configure DMA Channel bit band source address */
    lhDMA.Instance->CMAR = 0x22000000 + (((u32)mu8LedData - 0x20000000) * 32);

    HAL_DMA_Init(&lhDMA);
    DMA1_Channel3->CCR |= DMA_CCR_EN;
  }
};


#endif  //__WS2812_CONTROL_H__
