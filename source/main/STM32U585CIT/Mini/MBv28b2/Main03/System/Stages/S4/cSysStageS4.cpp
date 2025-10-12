#include "cSysStageS4.h"

#include "cSysStages.h"

cSysStageReq cStage4::mcReq;

cClock       cStage4::mcClock(False);
cZD25WQ32    cStage4::mcZD25WQ32;

osThreadId_t cStage4::mTaskClockHandle = 0;
osThreadId_t cStage4::mTaskMemHandle = 0;

bool         cStage4::mbInitClockFinished = False;
bool         cStage4::mbRunClock = True;
bool         cStage4::mbDeInitClockFinished = False;

bool         cStage4::mbInitMemFinished = False;
bool         cStage4::mbRunMem = True;
bool         cStage4::mbDeInitMemFinished = False;

void cStage4::vDoInitMem()
{
  mcZD25WQ32.i8StartInit();
  while (!mcZD25WQ32.isIdle())
  {
    mcZD25WQ32.vDoProcess(1);
    for (u8 lu8t = 0; lu8t > 10; lu8t++) { __asm("nop"); }
  }
}

void cStage4::vDoRunMem()
{
  while (!mcZD25WQ32.isIdle())
  {
    mcZD25WQ32.vDoProcess(1);
  }

  osThreadSuspend(mTaskMemHandle);
}

void cStage4::vDoDeInitMem()
{
}




void cStage4::vDoInitClock()
{
  mcClock.vInit();
}

void cStage4::vDoRunClock()
{
  mcClock.vAdd1s();
  osDelay(1000);
}

void cStage4::vDoDeInitClock()
{
}


