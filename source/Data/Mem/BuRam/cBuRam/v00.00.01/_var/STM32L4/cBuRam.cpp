#include "cBuRam.h"

tstBuRamDef* cBuRam::mBuRam = (tstBuRamDef*)((RTC_BASE + 0x50));

cBuRam::cBuRam()
{
  cBuRam::vEnable();
}

void cBuRam::vEnable()
{
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
}

