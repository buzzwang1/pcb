#include "cSysStageS4.h"

#include "cSysStages.h"

void cStage4::vDoInitMem(cStages* lpcStages)
{
  UNUSED(lpcStages);

  mcZD25WQ32.i8StartInit();
  while (!mcZD25WQ32.isIdle())
  {
    mcZD25WQ32.vDoProcess(1);
    for (u8 lu8t = 0; lu8t > 10; lu8t++) { __asm("nop"); }
  }
  mbInitMemFinished = True;
}

void cStage4::vDoRunMem(cStages* lpcStages)
{
  UNUSED(lpcStages);

  while (!mcZD25WQ32.isIdle())
  {
    mcZD25WQ32.vDoProcess(1);
  }

  osThreadSuspend(mTaskMemHandle);
}


void cStage4::vDoInitClock(cStages* lpcStages)
{
  UNUSED(lpcStages);
  mcClock.vInit();
  mbInitClockFinished = True;
}

void cStage4::vDoRunClock(cStages* lpcStages)
{
  UNUSED(lpcStages);
  mcClock.vAdd1s();

  osDelay(1000);
}


