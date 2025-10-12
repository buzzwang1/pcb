#include "cSysStageS8.h"
#include "cSysStages.h"

cSysStageReq cStage8::mcReq;

// ------------------ cStage8 init ----------------------------

osThreadId_t cStage8::mTaskAppHandle = 0;
bool         cStage8::mbInitAppFinished   = False;
bool         cStage8::mbRunApp = True;
bool         cStage8::mbDeInitAppFinished = False;


// ------------------- Task Gui --------------------------------
void cStage8::vDoInitApp()
{
}

void cStage8::vDoRunApp()
{
  bool lu8Reset = False;
  if ((cStage1::mcWufHandler.munWakeupSources.stWakeupSources.isWuPinRst) ||
      (cStage1::mcWufHandler.munWakeupSources.stWakeupSources.isWuSftRst)) lu8Reset = True;

  if (lu8Reset)
  {
    osDelay(10000);
  }
  else
  {
    osDelay(5000);
  }

  cStage5::mcCom.mcSideLink.vKeepReceiverOnWhileWaiting(True);


  //cStage1::mcReq.vReset();
  //cStage2::mcReq.vReset();
  //cStage4::mcReq.vReset();
  //cStage5::mcReq.vReset();
  //cStage6::mcReq.vReset();
  //cStage8::mcReq.vReset();
  //
  //osThreadSuspend(mTaskAppHandle);
}


void cStage8::vDoDeInitApp()
{
}


