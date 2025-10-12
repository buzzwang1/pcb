#include "cSysStageS2.h"


cDigiPower::cDigiPower()
  : mMX22917_S1(GPIOC_BASE, 5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0),
    mMX22917_S2(GPIOC_BASE, 4, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
{
}

void cBoard1::vInit()
{
  // S2   S1     R2     VOut
  //  0    0    620K   2V09
  //  0    1    409k   2V75
  //  1    0    447k   2V59
  //  1    1    325k   3V26
  cGpPin mTPS62125_S1(GPIOE_BASE, 7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  cGpPin mTPS62125_S2(GPIOE_BASE, 8, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
  cGpPin mTPS62125_PG(GPIOE_BASE, 9, GPIO_MODE_INPUT,     GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

  menLogicPower = nP1;

  if (mTPS62125_PG.ui8Get()) // 2V09 OK ?
  {
    // Try to set 2V75
    mTPS62125_S1.vSet1();
    vDelay_us(500);
    if (mTPS62125_PG.ui8Get()) // 2V75 OK ?
    {
      // ExtPowerReqCnt = 0
      menLogicPower = nP2;
  
      // Try to set 3V26
      mTPS62125_S2.vSet1();
      vDelay_us(500);
      if (mTPS62125_PG.ui8Get()) // 3V26 OK ?
      {
        menLogicPower = nP3;
      }
    }
  }

  //if (menLogicPower == nP1)
  //{
    // if (ExtPowerReqCnt < 3)
    //   ExtPowerReqCnt++
    //   Request external power source.
    //   Wait 100ms
    // else
    //  long sleep
  //}

  if (menLogicPower != nP1)
  {
    mcDigiPower.mMX22917_S1.vSet1();
    mcDigiPower.mMX22917_S2.vSet1();
    vDelay_us(500);
  }
}
