#pragma once

#include "FreeRTOS.h"
#include "cSysDPool.h"
#include "tGPPin.h"

class cComp3V3 : public cComponent
{
  public:

  cGpPin mMX22917_S1;
  cGpPin mMX22917_S2;

  cGpPin mTPS62125_S1;
  cGpPin mTPS62125_S2;
  cGpPin mTPS62125_PG;

  cComp3V3()
    : cComponent(cDepTreeCfg::cComp::nBoard3V3, cDepTreeCfg::cComp::nBase),
      mMX22917_S1(GPIOC_BASE,  5),
      mMX22917_S2(GPIOC_BASE,  4),
      mTPS62125_S1(GPIOE_BASE, 7),
      mTPS62125_S2(GPIOE_BASE, 8),
      mTPS62125_PG(GPIOE_BASE, 9)
  {
  }


  bool bInit() override
  {
    // S2   S1     R2     VOut
    //  0    0    620K   2V09
    //  0    1    447k   2V59
    //  1    0    409k   2V75
    //  1    1    325k   3V26

    mMX22917_S1.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mMX22917_S2.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mTPS62125_S1.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mTPS62125_S2.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);
    mTPS62125_PG.vInit(GPIO_MODE_INPUT, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

    cSysDPool::mBoard.mLogicPower.menLogicPower = cDPoolBoard::cLogicPower::nP1_2V09;

    if (mTPS62125_PG.ui8Get()) // 2V09 OK ?
    {
      // Try to set 2V75
      mTPS62125_S2.vSet1();

      if (mTPS62125_PG.ui8Get()) // 2V75 OK ?
      {
        // ExtPowerReqCnt = 0
        cSysDPool::mBoard.mLogicPower.menLogicPower = cDPoolBoard::cLogicPower::nP2_2V75;

        // Try to set 3V26
        mTPS62125_S1.vSet1();
        vTaskDelay(pdMS_TO_TICKS(1));
        if (mTPS62125_PG.ui8Get()) // 3V26 OK ?
        {
          cSysDPool::mBoard.mLogicPower.menLogicPower = cDPoolBoard::cLogicPower::nP3_3V26;
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

    if (cSysDPool::mBoard.mLogicPower.menLogicPower != cDPoolBoard::cLogicPower::nP1_2V09)
    {
      mMX22917_S1.vSet1();
      mMX22917_S2.vSet1();
      vTaskDelay(pdMS_TO_TICKS(1));
    }

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {

    mTPS62125_S1.vSet0();
    mTPS62125_S2.vSet0();

    mMX22917_S1.vSet0();
    mMX22917_S2.vSet0();

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




