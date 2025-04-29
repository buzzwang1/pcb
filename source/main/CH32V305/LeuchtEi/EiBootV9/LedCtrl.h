#ifndef __WS2812_CONTROL_H__
#define __WS2812_CONTROL_H__

#include "TypeDef.h"
#include "ch32v30x.h"
#include "ch32v30x_adc.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_tim.h"
#include "ch32v30x_dma.h"
#include "ch32v30x_misc.h"

#include "tGPPin.h"
#include "LED.h"
#include "ClockInfo.h"


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
  u8 *mu8GfxData; // Gfx Buffer, 3 Byte per Pxl
  u8* mu8LedData; // Led Buffer, 24 Byte per Pxl

  cWs2812(u8* lu8RgbData, u8* lu8LedbData, u16 lu16LedCnt)
    : mcLedSig(GPIOA_BASE,  8, GPIO_Mode_AF_PP,  GPIO_Speed_10MHz, 0),
      mcEn(    GPIOC_BASE,  8, GPIO_Mode_Out_PP, GPIO_Speed_10MHz, 0)
  {
    mu8GfxData  = lu8RgbData;
    mu8LedData  = lu8LedbData;
    mu16LedCnt  = lu16LedCnt;


    // configure analog inputs
    cGpCfg::vSet(GPIOA, 5, cGpCfg::nAnalog, cGpCfg::nInput); // Ntc
    cGpCfg::vSet(GPIOA, 6, cGpCfg::nAnalog, cGpCfg::nInput); // Shunt

    mu8BrigthnessSet = 128; // 50%
    mu8Brigthness    = 0;   // 0%

    for (u16 lu16t = 0; lu16t < LEDCTRL_RGB_MEMSIZE(mu16LedCnt); lu16t++)
    {
      mu8GfxData[lu16t] = 0;
    }

    vShow();

    vTim1_Dma_vInit();
    vTim1_Init_WS2812();

    mu8StateProcess = 0;
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
        TIM1->CH1CVR = 0;
        mu8StateProcess = 2;
        break;
      case 2: //DMA Ausgabe starten. Wird von DMA IRQ beendet
        // Start DMA
        DMA1_Channel2->CNTR = LEDCTRL_LED_MEMSIZE(mu16LedCnt);
        DMA1_Channel2->MADDR = (u32)mu8LedData;
        DMA1_Channel2->CFGR |= ((1 << 0)  | (1 << 1)); // EN + TCIE
        TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
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


  void vTim1_Init_WS2812(void)
  {
    TIM_OCInitTypeDef       TIM_OCInitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);


    // 96 Mhz / 8Mhz = 12
    TIM_TimeBaseInitStructure.TIM_Prescaler     = (cClockInfo::mstClocks.PCLK2_Frequency / 8000000) - 1; // 0,125 us = 8Mhz
    TIM_TimeBaseInitStructure.TIM_Period        = 10 - 1; // = > 1,25us is = 800khz
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode       = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState  = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_OCIdleState  = TIM_OCNIdleState_Reset;

    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity   = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_DMACmd(TIM1, TIM_DMA_CC1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
  }

  void Dma_vIrqTc()
  {
    TIM_DMACmd(TIM1, TIM_DMA_CC1, DISABLE);
    DMA1_Channel2->CFGR &= ~(1 << 0); // EN
    DMA1->INTFCR = DMA1_IT_TC2;
    mu8StateProcess = 4;
    vTurnOnOffCtrl();
  }

  void vTim1_Dma_vInit(void)
  {
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel2);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&TIM1->CH1CVR);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)mu8LedData;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = LEDCTRL_LED_MEMSIZE(mu16LedCnt);
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel2, &DMA_InitStructure);

    NVIC_SetPriority(DMA1_Channel2_IRQn, (2 << 5));
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
  }
};


#endif  //__WS2812_CONTROL_H__
