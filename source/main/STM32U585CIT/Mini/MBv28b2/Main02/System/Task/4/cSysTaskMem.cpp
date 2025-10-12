#include "cSysTaskMem.h"
#include "cSysStages.h"

const osThreadAttr_t TaskMem_attributes = {
  .name = "Mem",                           ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = 256 * 4,                   ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskMem(void* argument)
{
  // Init
  cStages* lpcStages = (cStages*)argument;

  lpcStages->mcS4.vDoInitMem(lpcStages);

  // Run
  for (;;)
  {
    lpcStages->mcS4.vDoRunMem(lpcStages);
  }
}

