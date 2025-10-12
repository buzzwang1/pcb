#ifndef __SYSTEM_TASK_CLOCK_H__
#define __SYSTEM_TASK_CLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

extern const osThreadAttr_t TaskClock_attributes;
extern void TaskClock(void* argument);

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_TASK_CLOCK_H__
