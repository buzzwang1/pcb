
#include "CycleCallerHw.h"

CycCallTimer_Callback mptrCycCall_Callback_500us_HP;




void CycCallHw_vInit(CycCallTimer_Callback lptrCycCall_Callback_500us_HP)
{
  mptrCycCall_Callback_500us_HP = lptrCycCall_Callback_500us_HP;
}