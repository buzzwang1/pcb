#pragma once

#include "TYPEDEF.h"
#include "tGPPin.h"
#include "cTim.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"
#include "stm32u5xx_ll_spi.h"
#include "stm32u5xx_ll_dma.h"
#include "stm32u5xx_ll_exti.h"

#include "ClockInfo.h"




typedef tcTimBasic_1us<TIM7_BASE, TIM7_IRQn, 8, 8> cNRF905_Timer;

class cNRF905_Pins
{
  public:

  //Outputs
  cGpPin mTRx_Cn; // Chip select
  cGpPin mTx_En;  // TX or RX (1 = Tx Mode, 0 = Rx Mode)
  cGpPin mPWR;    // Power
  cGpPin mCS_SPI; // Chip select SPI

  //Inputs
  cGpPin mCD;  // Carrier Detected // CD and AM are n.A. So just put it to DR.
  cGpPin mAM;  // Adress Match
  cGpPin mDR;  // Data Ready*/

  cNRF905_Pins()
    : mTRx_Cn(GPIOE_BASE, 0),
      mTx_En(GPIOB_BASE, 8),
      mPWR(GPIOE_BASE, 1),
      mCS_SPI(GPIOE_BASE, 6),

      //Inputs
      mCD(GPIOE_BASE, 3),
      mAM(GPIOE_BASE, 4),
      mDR(GPIOE_BASE, 5)
  {
  };


  void vInit()
  {
    // DR (Data Ready) is connected to PE05

    // Tell system that you will use PE05 for EXTI_Line5
    // EXTI->EXTICR[0] EXTI0..3
    // EXTI->EXTICR[0] EXTI4..7
    // EXTI->EXTICR[0] EXTI8..11
    // EXTI->EXTICR[0] EXTI12..15
    //
    //
    // EXTI_Line5 => EXTI->EXTICR[1] 2nd Byte
    // PA = 0; PB = 1, PC = 2, PD = 3
    // => PE = 4

    //u32 lu32Temp;
    //lu32Temp  = EXTI->EXTICR[1] & 0xFFFF00FF;
    //lu32Temp &= 4 << 8;
    //EXTI->EXTICR[1] = lu32Temp;

    mTRx_Cn.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mTx_En.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,  GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mPWR.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,    GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mCS_SPI.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);

    mCD.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mAM.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mDR.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    LL_EXTI_SetEXTISource(LL_EXTI_EXTI_PORTE, LL_EXTI_EXTI_LINE5);

    ////
    ////// PE05 is connected to EXTI_Line5
    LL_EXTI_InitTypeDef EXTI_InitStruct;
    LL_EXTI_StructInit(&EXTI_InitStruct);

    EXTI_InitStruct.Line_0_31   = LL_EXTI_LINE_5;
    EXTI_InitStruct.LineCommand = ENABLE;
    EXTI_InitStruct.Mode        = LL_EXTI_MODE_IT;
    EXTI_InitStruct.Trigger     = LL_EXTI_TRIGGER_RISING;
    LL_EXTI_Init(&EXTI_InitStruct);

    // Add IRQ vector to NVIC
    HAL_NVIC_SetPriority(EXTI5_IRQn, 8, 8);
    HAL_NVIC_EnableIRQ(EXTI5_IRQn);
  }

  inline void vEnableIrqDR()
  {
    EXTI->IMR1 |= (1<<5);
  }

  inline void vDisableIrqDR()
  {
    EXTI->IMR1 &= ~(1<<5);
    LL_EXTI_DisableIT_0_31(LL_EXTI_LINE_5);
  }
};


