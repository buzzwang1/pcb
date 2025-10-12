#include "cSysTaskCalender.h"
#include "cSysStages.h"

const osThreadAttr_t TaskCalender_attributes = {
  .name = "Calender",                      ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024*2),                  ///< size of stack
  .priority = (osPriority_t)osPriorityLow, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};


void TaskCalender(void* argument)
{
  UNUSED(argument);

  // Init
  cStage6::vDoInitCalender();
  cStage6::mbInitCalenderFinished = True;
  cStage6::mbRunCalender = True;

  // Run
  while (cStage6::mbRunCalender)
  {
    cStage6::vDoRunCalender();
  }

  cStage6::vDoDeInitCalender();
  cStage6::mbDeInitCalenderFinished = True;

  for (;;)
  {
    osThreadSuspend(cStage6::mTaskCalenderHandle);
  }
}



