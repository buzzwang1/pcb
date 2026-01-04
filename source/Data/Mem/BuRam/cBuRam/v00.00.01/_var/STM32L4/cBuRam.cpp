#include "cBuRam.h"

tstBuRamDef* cBuRam::mBuRam = (tstBuRamDef*)((RTC_BASE + 0x50));

u8 cBuRam::mu8ValueLastPos = 0xFF;
u8 cBuRam::mu8ValueLastSys = 0xFF;

cBuRam::cBuRam()
{
  cBuRam::vEnable();
}

void cBuRam::vEnable()
{
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
}

