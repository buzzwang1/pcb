#ifndef _BURAM_H
#define _BURAM_H

#include "typedef.h"
#include "BuRamDef.h"

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_rtc.h"
#include "stm32u5xx_ll_pwr.h"

class cBuRam
{
  public:

  cBuRam();

  static tstBuRamDef* mBuRam;

  static void vEnable();
};



#endif /* _BURAM_H */
