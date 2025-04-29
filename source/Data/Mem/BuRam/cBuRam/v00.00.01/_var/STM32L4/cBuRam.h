#ifndef _BURAM_H
#define _BURAM_H

#include "typedef.h"
#include "BuRamDef.h"

#include "stm32l433xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_pwr.h"

class cBuRam
{
  public:

  cBuRam();

  static tstBuRamDef* mBuRam;

  static void vEnable();
};



#endif /* _BURAM_H */
