#pragma once

#include "cSysDPool.h"

class cCompLed : public cComponent
{
  public:

    typedef enum
    {
      nStToggle100ms = 0x5555,
      nStToggle200ms = 0x3333,
      nStToggle400ms = 0x0F0F,
      nStToggle800ms = 0x00FF,
      nStOn  = 0xFF,
      nStOff = 0,
    }tenMode;

  cGpPin mLed;

  u16 mModeWork;
  u16 mModeSet;

  cCompLed()
    : cComponent(cDepTreeCfg::cComp::nLed, cDepTreeCfg::cComp::nBoard3V3),
      mLed(GPIOB_BASE, 9)
  {
  }


  bool bInit() override
  {
    mLed.vInit(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0);

    mModeSet  = cSysDPool::mBoard.mu16LedMode = tenMode::nStToggle800ms;
    mModeWork = mModeSet;

    cComponentList::mcList128ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList128ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    if (mModeSet != cSysDPool::mBoard.mu16LedMode)
    {
      mModeSet  = cSysDPool::mBoard.mu16LedMode;
      mModeWork = mModeSet;
    }

    // return True to signal finished
    mLed.vSet(mModeWork & 1);

    if (mModeWork & 1)
    {
      mModeWork >>= 1;
      mModeWork |= 0x8000;
    }
    else
    {
      mModeWork >>= 1;
    }
  
    return cComponent::bRun();
  };
};




