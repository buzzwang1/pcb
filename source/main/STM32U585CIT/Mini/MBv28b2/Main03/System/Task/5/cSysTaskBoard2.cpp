#include "cSysTaskBoard2.h"
#include "cSysStages.h"

const osThreadAttr_t TaskBoard2_attributes = {
  .name = "Board2",                        ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< Board2ory for control block
  .cb_size = 0,                            ///< size of provided Board2ory for control block
  .stack_mem = 0,                          ///< Board2ory for stack
  .stack_size = (1024*2),                  ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskBoard2(void* argument)
{
  UNUSED(argument);

  // Init
  cStage5::vDoInitBoard2();
  cStage5::mbInitBoard2Finished = True;
  cStage5::mbRunBoard2 = True;

  // Run
  while (cStage5::mbRunBoard2)
  {
    cStage5::vDoRunBoard2();
  }

  cStage5::vDoDeInitBoard2();
  cStage5::mbDeInitBoard2Finished = True;

  for (;;)
  {
    osThreadSuspend(cStage5::mTaskBoard2Handle);
  }
}



