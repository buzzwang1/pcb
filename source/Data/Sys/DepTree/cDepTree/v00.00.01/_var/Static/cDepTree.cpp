#include "cDepTree.h"


osMutexId_t  cDepTree::xMutex = 0;

bool cDepTree::mTaskInit1Ready = True;
bool cDepTree::mTaskInit2Ready = True;


// Optionale Attribute (Namen vergeben für Debugging)
const osMutexAttr_t cDepTree_mutex_attr = {
  "cDepTreeMutex",                       // Name des Mutex
  osMutexRecursive | osMutexPrioInherit, // Eigenschaften
  NULL,                                  // Speicher für Control Block (NULL = automatisch)
  0                                      // Größe des Control Blocks
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




