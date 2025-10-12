#include "cSysTaskCom.h"
#include "cSysStages.h"

const osThreadAttr_t TaskCom_attributes = {
  .name = "Com",                           ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< Comory for control block
  .cb_size = 0,                            ///< size of provided Comory for control block
  .stack_mem = 0,                          ///< Comory for stack
  .stack_size = (1024*2),                  ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskCom(void* argument)
{
  UNUSED(argument);

  // Init
  cStage5::vDoInitCom();
  cStage5::mbInitComFinished = True;
  cStage5::mbRunCom = True;

  // Run
  while (cStage5::mbRunCom)
  {
    cStage5::vDoRunCom();
  }

  cStage5::vDoDeInitCom();
  cStage5::mbDeInitComFinished = True;

  for (;;)
  {
    osThreadSuspend(cStage5::mTaskComHandle);
  }
}



