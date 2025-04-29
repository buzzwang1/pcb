#include "cBuRam.h"

tstBuRamDef* cBuRam::mBuRam = (tstBuRamDef*)(&(TAMP->BKP0R));

cBuRam::cBuRam()
{
  cBuRam::vEnable();
}

void cBuRam::vEnable()
{
  LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
}

