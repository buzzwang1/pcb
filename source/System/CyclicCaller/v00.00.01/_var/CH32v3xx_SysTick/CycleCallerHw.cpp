
#include "CycleCallerHw.h"

CycCallTimer_Callback mptrCycCall_Callback_1ms_HP;


void SysTick_Handler(void)
{
  if (SysTick->SR == 1)
  {
    SysTick->SR = 0; //clear State flag
    mptrCycCall_Callback_1ms_HP();
  }
}


void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_1ms_HP)
{
  mptrCycCall_Callback_1ms_HP = lptrCycCall_Callback_1ms_HP;

  SysTick->SR &= ~(1 << 0);//clear State flag
  SysTick->CMP = cClockInfo::u32GetClockSys() / 8000000 * 1000; //  8Mhz => 1000
                                                                // 16Mhz => 2000
  SysTick->CTLR = (1 << 5) | (1 << 3) | (1 << 1) | (1 << 0); // Init(Counter == 0), Upcounting, Enable, Irq Enable, Autoreload

  // Prio 0(höchste) 7(kleinste)
  NVIC_SetPriority(SysTicK_IRQn, (7 << 5) | (0x01 << 4));
  NVIC_EnableIRQ(SysTicK_IRQn);
}