#include "cSysTaskClock.h"
#include "cSysStages.h"

const osThreadAttr_t TaskClock_attributes = {
  .name = "Clock",                         ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024 * 2),                ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskClock(void* argument)
{
  UNUSED(argument);

  // Init
  cStage4::vDoInitClock();
  cStage4::mbInitClockFinished = True;
  cStage4::mbRunClock = True;

  // Run
  while (cStage4::mbRunClock)
  {
    cStage4::vDoRunClock();
  }

  cStage4::vDoDeInitClock();
  cStage4::mbDeInitClockFinished = True;

  for (;;)
  {
    osThreadSuspend(cStage4::mTaskClockHandle);
  }
}



