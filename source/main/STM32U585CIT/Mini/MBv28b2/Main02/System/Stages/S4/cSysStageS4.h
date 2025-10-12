#ifndef __SYSTEM_STAGE_S4_H__
#define __SYSTEM_STAGE_S4_H__

#include "TypeDef.h"
#include "cSysStage.h"
#include "cClock.h"
#include "FlashGeneral.h"
#include "ZD25WQ32.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"


class cStages;

class cStage4
{
public:
  cClock       mcClock;
  cZD25WQ32    mcZD25WQ32;

  osThreadId_t mTaskClockHandle;
  osThreadId_t mTaskMemHandle;

  bool         mbInitClockFinished;
  bool         mbInitMemFinished;

  cStage4()
    : mcClock(),
      mcZD25WQ32()
  {
    mbInitClockFinished = mbInitMemFinished = False;
  }

  void vInit()
  {
  }

  void vDoInitMem(cStages* lpcStages);
  void vDoRunMem(cStages* lpcStages);


  void vDoInitClock(cStages* lpcStages);
  void vDoRunClock(cStages* lpcStages);

  bool isInit()
  {
    return (mbInitClockFinished && mbInitMemFinished);
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S4_H__
