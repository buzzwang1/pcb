#include "cDepTree.h"


osThreadId_t cDepTree::mTaskBaseHandle  = 0;
osThreadId_t cDepTree::mTaskInit1Handle = 0;
osThreadId_t cDepTree::mTaskInit2Handle = 0;

osThreadId_t cDepTree::mTask1msHandle = 0;


osMutexId_t  cDepTree::xMutex = 0;

bool cDepTree::mTaskInit1Ready = True;
bool cDepTree::mTaskInit2Ready = True;


const osThreadAttr_t TaskcDepTreeBase_attributes = {
  .name = "cDepTreeBase",                  ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024 * 2),                ///< size of stack
  .priority = (osPriority_t)osPriorityNormal, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};

const osThreadAttr_t TaskcDepTreeInit1_attributes = {
  .name = "cDepTreeInit1",                  ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024 * 2),                ///< size of stack
  .priority = (osPriority_t)osPriorityNormal, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};

const osThreadAttr_t TaskcDepTreeInit2_attributes = {
  .name = "cDepTreeInit2",                  ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024 * 2),                ///< size of stack
  .priority = (osPriority_t)osPriorityNormal, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};

// Optionale Attribute (Namen vergeben für Debugging)
const osMutexAttr_t cDepTree_mutex_attr = {
  "cDepTreeMutex",                       // Name des Mutex
  osMutexRecursive | osMutexPrioInherit, // Eigenschaften
  NULL,                                  // Speicher für Control Block (NULL = automatisch)
  0                                      // Größe des Control Blocks
};

const osThreadAttr_t TaskcDepTree1ms_attributes = {
  .name = "cDepTree1ms",                   ///< name of the thread
  .attr_bits = 0,                          ///< attribute bits
  .cb_mem = 0,                             ///< memory for control block
  .cb_size = 0,                            ///< size of provided memory for control block
  .stack_mem = 0,                          ///< memory for stack
  .stack_size = (1024 * 2),                ///< size of stack
  .priority = (osPriority_t)osPriorityNormal, ///< initial thread priority (default: osPriorityNormal)
  .tz_module = 0,                          ///< TrustZone module identifier
  .reserved = 0                            ///< reserved (must be 0)
};

void TaskcDepTreeBase(void* argument)
{
  UNUSED(argument);

  while (1)
  {
    cDepTree::vBaseTask();
  }
}


void TaskcDepTreeInit1(void* argument)
{
  UNUSED(argument);
  vTaskSuspend(NULL);
  while (1)
  {
    cDepTree::vInit1Task();
  }
}


void TaskcDepTreeInit2(void* argument)
{
  UNUSED(argument);
  vTaskSuspend(NULL);
  while (1)
  {
    cDepTree::vInit2Task();
  }
}


void TaskcDepTree1ms(void* argument)
{
  UNUSED(argument);
  //vTaskSuspend(NULL);
  while (1)
  {
    cDepTree::v1msTask();
  }
}




