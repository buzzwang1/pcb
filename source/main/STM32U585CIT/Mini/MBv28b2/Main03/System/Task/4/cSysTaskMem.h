#ifndef __SYSTEM_TASK_MEM_H__
#define __SYSTEM_TASK_MEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

extern const osThreadAttr_t TaskMem_attributes;
extern void TaskMem(void* argument);


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_TASK_MEM_H__
