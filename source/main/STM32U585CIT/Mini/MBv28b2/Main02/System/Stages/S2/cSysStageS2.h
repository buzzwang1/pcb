#ifndef __SYSTEM_STAGE_S2_H__
#define __SYSTEM_STAGE_S2_H__

#include "TypeDef.h"

#include "tGPPin.h"

#include "DiffTimer.h"
#include "cSysStage.h"


#ifdef __cplusplus
extern "C" {
#endif

class cDigiPower
{
public:
  //Digi Power
  cGpPin mMX22917_S1;
  cGpPin mMX22917_S2;

  cDigiPower();
};

class cBoard1
{
  public:

    enum tenLogicPower : u8
    {
      nP1 = 0,
      nP2,
      nP3
    };

    tenLogicPower menLogicPower;
    cDigiPower    mcDigiPower;

    cBoard1()
      : mcDigiPower()
    {
    }

    void vDelay_us(u16 lu16Delay_us)
    {
      cDiffTimer mcDt;

      while (!(mcDt.isValue(lu16Delay_us))) 
      {}
    }

    void vInit();
};

class cStage2
{
public:
  cBoard1 mcBoard1;

  cStage2()
    : mcBoard1()
  {}

  void vInit()
  {
    mcBoard1.vInit();
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S2_H__
