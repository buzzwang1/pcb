#ifndef __SYSTEM_STAGE_S6_H__
#define __SYSTEM_STAGE_S6_H__

#include "TypeDef.h"
#include "cSysStageReq.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

class cStage6
{
public:

  static cSysStageReq mcReq;

  /* ------------------------ Calender -------------------------*/
  static osThreadId_t mTaskCalenderHandle;
  static bool         mbInitCalenderFinished;
  static bool         mbRunCalender;
  static bool         mbDeInitCalenderFinished;

  /* ------------------------ Gui -------------------------*/
  static osThreadId_t mTaskGuiHandle;
  static bool         mbInitGuiFinished;
  static bool         mbRunGui;
  static bool         mbDeInitGuiFinished;

  /* ------------------------ Power -------------------------*/
  static osThreadId_t mTaskPowerHandle;
  static bool         mbInitPowerFinished;
  static bool         mbRunPower;
  static bool         mbDeInitPowerFinished;


  static void vInit()
  {
  }

  static void vDoInitCalender();
  static void vDoRunCalender();
  static void vDoDeInitCalender();

  static void vDoInitGui();
  static void vDoRunGui();
  static void vDoDeInitGui();

  static void vDoInitPower();
  static void vDoRunPower();
  static void vDoDeInitPower();

  static bool isInit()
  {
    return (mbInitCalenderFinished && mbInitGuiFinished && mbInitPowerFinished);
  }

  static bool isDeInit()
  {
    return (mbDeInitCalenderFinished && mbDeInitGuiFinished && mbDeInitPowerFinished);
  }
};



#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_STAGE_S6_H__
