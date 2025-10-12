#ifndef __SYSTEM_STAGE_S2_H__
#define __SYSTEM_STAGE_S2_H__

#include "TypeDef.h"
#include "tGPPin.h"
#include "cSysStageReq.h"


#ifdef __cplusplus
extern "C" {
#endif

class cDigiPower
{
public:
  //Digi Power
  static cGpPin mMX22917_S1;
  static cGpPin mMX22917_S2;
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

    static tenLogicPower menLogicPower;    
    static cDigiPower    mcDigiPower;

    static void vInit();
    static void vDeInit();
};

class cStage2
{
public:
  static cSysStageReq mcReq;

  static cBoard1 mcBoard1;

  static void vInit()
  {
    mcBoard1.vInit();
  }

  static void vDeInit()
  {
    mcBoard1.vDeInit();
  }

  static bool isInit()
  {
    return True;
  }

  static bool isDeInit()
  {
    return True;
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S2_H__
