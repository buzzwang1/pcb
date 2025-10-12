#include "cSysTaskGui.h"
#include "cSysStages.h"

const osThreadAttr_t TaskGui_attributes = {
  .name = "Gui",                           ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024*2),                  ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskGui(void* argument)
{
  UNUSED(argument);

  // Init
  cStage6::vDoInitGui();
  cStage6::mbInitGuiFinished = True;
  cStage6::mbRunGui = True;

  // Run
  while (cStage6::mbRunGui)
  {
    cStage6::vDoRunGui();
  }

  cStage6::vDoDeInitGui();
  cStage6::mbDeInitGuiFinished = True;

  for (;;)
  {
    osThreadSuspend(cStage6::mTaskGuiHandle);
  }
}



