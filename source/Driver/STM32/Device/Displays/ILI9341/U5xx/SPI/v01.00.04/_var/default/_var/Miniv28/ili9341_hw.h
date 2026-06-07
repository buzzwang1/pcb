#pragma once

/* Includes ------------------------------------------------------------------*/
#include "TYPEDEF.h"
#include "tGPPin.h"

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_tim.h"
#include "stm32u5xx_ll_tim.h"

class cILI9341_HW
{
public:
  cGpPin mPinA0;
  cGpPin mPinRst;
  cGpPin mPinCs;
  cGpPin mLigth;

  cILI9341_HW()
    : mPinA0(GPIOD_BASE,  6),
      mPinRst(GPIOD_BASE, 2),
      mPinCs(GPIOC_BASE, 11),
      mLigth(GPIOB_BASE,  6)
  {}

  void vInitTim4()
  {
    // 2. Timer 4 Clock aktivieren
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

    // 3. Zeitbasis einstellen (200 Hz Grundfrequenz)
    LL_TIM_SetPrescaler(TIM4, 159);          // Takt auf 1 MHz senken
    LL_TIM_SetAutoReload(TIM4, 4999);        // 1 MHz / 5000 = 200 Hz
    LL_TIM_SetCounterMode(TIM4, LL_TIM_COUNTERMODE_UP);

    // 4. PWM Modus für Kanal 1 konfigurieren
    LL_TIM_OC_SetMode(TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_OCMODE_PWM1);
    LL_TIM_OC_SetPolarity(TIM4, LL_TIM_CHANNEL_CH1, LL_TIM_OCPOLARITY_HIGH);

    // 5. Duty Cycle setzen (z.B. 50%)
    // Duty Cycle = (Compare Value / ARR) * 100
    // 50% von 5000 = 2500
    LL_TIM_OC_SetCompareCH1(TIM4, 2500);

    // 6. Preload aktivieren (damit Änderungen am Duty Cycle synchron zum Periodenende übernommen werden)
    LL_TIM_OC_EnablePreload(TIM4, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableARRPreload(TIM4);

    // 7. Kanal und Timer aktivieren
    LL_TIM_CC_EnableChannel(TIM4, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM4);
  }


  void vInitHw()
  {
    mPinA0.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mPinRst.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mPinCs.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mLigth.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    mLigth.vSetAF(GPIO_MODE_AF_PP, GPIO_AF2_TIM4);   // SCK

    vResetOff();
    vCsOff();
    vInitTim4();
    vSetLigth(0);
  }


  // Reset Low active
  void vResetOn()  { mPinRst.vSet0(); }
  void vResetOff() { mPinRst.vSet1(); }

  // CS Low active
  void vCsOn()  { mPinCs.vSet0(); }
  void vCsOff() { mPinCs.vSet1(); }

  void vModeCmd()  { mPinA0.vSet0(); }
  void vModeData() { mPinA0.vSet1(); }


  void vSetLigth(u8 lu8Dim_percent)
  {
    // 1. Sicherheits-Check: Wert auf 0 - 100 % begrenzen
    if (lu8Dim_percent > 100) 
    {
      lu8Dim_percent = 100;
    }

    // 2. Umrechnung in den passenden Registerwert
    // (TIM4->ARR + 1) entspricht den 5000 Gesamtschritten
    u32 ccr_value = (lu8Dim_percent * (TIM4->ARR + 1)) / 100;

    // 3. Wert direkt in das Capture/Compare Register schreiben
    LL_TIM_OC_SetCompareCH1(TIM4, ccr_value);
  }
};

