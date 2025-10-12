#include "cSysStageS6.h"
#include "cSysStages.h"



cSysStageReq cStage6::mcReq;


// ------------------ cStage6 init ----------------------------

osThreadId_t cStage6::mTaskCalenderHandle = 0;
bool         cStage6::mbInitCalenderFinished = False;
bool         cStage6::mbRunCalender = True;
bool         cStage6::mbDeInitCalenderFinished = False;

osThreadId_t cStage6::mTaskGuiHandle = 0;
bool         cStage6::mbInitGuiFinished = False;
bool         cStage6::mbRunGui = True;
bool         cStage6::mbDeInitGuiFinished = False;

osThreadId_t cStage6::mTaskPowerHandle = 0;
bool         cStage6::mbInitPowerFinished = False;
bool         cStage6::mbRunPower = True;
bool         cStage6::mbDeInitPowerFinished = False;


// ------------------- Task Calender --------------------------------

void cStage6::vDoInitCalender()
{
}

void cStage6::vDoRunCalender()
{
  osDelay(1000);
}

void cStage6::vDoDeInitCalender()
{
}


// ------------------- Task Gui --------------------------------
void cStage6::vDoInitGui()
{
}

void cStage6::vDoRunGui()
{
  osDelay(1000);
}

void cStage6::vDoDeInitGui()
{
}

// ------------------- Task Power --------------------------------
void cStage6::vDoInitPower()
{
}

void cStage6::vDoRunPower()
{
  osDelay(1000);
}

void cStage6::vDoDeInitPower()
{
}



