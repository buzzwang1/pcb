#include "cSysTaskPower.h"
#include "cSysStages.h"

const osThreadAttr_t TaskPower_attributes = {
  .name = "Power",                        ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024*2),                  ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskPower(void* argument)
{
  UNUSED(argument);

  // Init
  cStage6::vDoInitPower();
  cStage6::mbInitPowerFinished = True;
  cStage6::mbRunPower = True;

  // Run
  while (cStage6::mbRunPower)
  {
    cStage6::vDoRunPower();
  }

  cStage6::vDoDeInitPower();
  cStage6::mbDeInitPowerFinished = True;

  for (;;)
  {
    osThreadSuspend(cStage6::mTaskPowerHandle);
  }
}



