#ifndef __SYSTEM_STAGE_S8_H__
#define __SYSTEM_STAGE_S8_H__

#include "TypeDef.h"
#include "cSysStageReq.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"


class cStage8
{
public:

  static cSysStageReq mcReq;

  /* ------------------------ App -------------------------*/
  static osThreadId_t mTaskAppHandle;
  static bool         mbInitAppFinished;
  static bool         mbRunApp;
  static bool         mbDeInitAppFinished;

  static void vInit()
  {
  }

  static void vDoInitApp();
  static void vDoRunApp();
  static void vDoDeInitApp();

  static bool isInit()
  {
    return (mbInitAppFinished && True);
  }

  static bool isDeInit()
  {
    return (mbDeInitAppFinished && True);
  }
};



#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S8_H__
