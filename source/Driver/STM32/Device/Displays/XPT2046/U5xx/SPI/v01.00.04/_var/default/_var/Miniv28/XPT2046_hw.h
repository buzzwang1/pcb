#pragma once

/* Includes ------------------------------------------------------------------*/
#include "TYPEDEF.h"
#include "tGPPin.h"

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_exti.h"

class cXPT2046_HW
{
public:
  cGpPin mPinCs;
  cGpPin mIrq;


  cXPT2046_HW()
    : mPinCs(GPIOD_BASE, 7),
      mIrq(GPIOD_BASE,  11)
  {}

  void vInitIrq()
  {
    ////// 1. Clock für SBS (System Bridge Subsystem) aktivieren
    ////LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_SYSCFG);
    ////
    ////// 2. PD11 mit EXTI Linie 11 verknüpfen
    ////LL_EXTI_SetEXTISource(LL_EXTI_EXTI_PORTD, LL_EXTI_EXTI_LINE11);
    ////
    ////
    ////// 3. EXTI Linie 11 konfigurieren
    ////LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_11);          // Interrupt aktivieren
    ////LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_11); // Fallende Flanke
    ////
    ////// Priorität setzen (niedrige Zahl = hohe Priorität)
    //////NVIC_SetPriority(EXTI11_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 8, 8));
    ////
    ////// Interrupt im NVIC aktivieren
    //////NVIC_EnableIRQ(EXTI11_IRQn);
  }

  void vInitHw()
  {
    mPinCs.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);
    mIrq.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 1);

    vCsOff();

    vInitIrq();
  }

  // CS Low active
  void vCsOn()  { mPinCs.vSet0(); }
  void vCsOff() { mPinCs.vSet1(); }

  void vModeCmd()  {}
  void vModeData() {}
};


//#ifdef __cplusplus
//extern "C" {
//#endif
//
//  extern void EXTI11_IRQHandler(void);
//
//  void EXTI11_IRQHandler(void)
//  {
//    // Prüfen, ob der Interrupt wirklich von Linie 11 kam
//    if (LL_EXTI_IsActiveFallingFlag_0_31(LL_EXTI_LINE_11) != RESET)
//    {
//      cSysDPool::mBoard.mcGfx.u8TouchIrq = 1;
//
//      // Flag löschen (WICHTIG, sonst wird der Handler sofort wieder aufgerufen)
//      LL_EXTI_ClearFallingFlag_0_31(LL_EXTI_LINE_11);
//    }
//  }
//
//
//#ifdef __cplusplus
//}
//#endif

