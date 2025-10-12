#include "cSysTaskBoard2.h"
#include "cSysStages.h"

const osThreadAttr_t TaskBoard2_attributes = {
  .name = "Board2",                        ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = 128 * 4,                   ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskBoard2(void* argument)
{
  // Init
  cStages* lpcStages = (cStages*)argument;

  lpcStages->mcS5.vDoInitBoard2(lpcStages);

  // Run
  for (;;)
  {
    lpcStages->mcS5.vDoRunBoard2(lpcStages);
  }
}



