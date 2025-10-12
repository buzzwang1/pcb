#ifndef __SYSTEM_STAGE_S4_H__
#define __SYSTEM_STAGE_S4_H__

#include "TypeDef.h"
#include "cSysStageReq.h"
#include "cClock.h"
#include "FlashGeneral.h"
#include "ZD25WQ32.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"


class cStage4
{
public:
  static cSysStageReq mcReq;

  static cClock       mcClock;
  static cZD25WQ32    mcZD25WQ32;

  static osThreadId_t mTaskClockHandle;
  static osThreadId_t mTaskMemHandle;

  static bool         mbInitClockFinished;
  static bool         mbRunClock;
  static bool         mbDeInitClockFinished;

  static bool         mbInitMemFinished;
  static bool         mbRunMem;
  static bool         mbDeInitMemFinished;


  static void vInit()
  {
    mbInitClockFinished = mbInitMemFinished = False;
    mbDeInitClockFinished = mbDeInitMemFinished = False;
  }

  static void vDoInitMem();
  static void vDoRunMem();
  static void vDoDeInitMem();

  static void vDoInitClock();
  static void vDoRunClock();
  static void vDoDeInitClock();

  static bool isInit()
  {
    return (mbInitClockFinished && mbInitMemFinished);
  }

  static bool isDeInit()
  {
    return (mbDeInitClockFinished && mbDeInitMemFinished);
  }
};


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S4_H__
