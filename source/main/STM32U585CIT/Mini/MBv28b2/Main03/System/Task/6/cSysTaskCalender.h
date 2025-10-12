#ifndef __SYSTEM_TASK_CALENDER_H__
#define __SYSTEM_TASK_CALENDER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

extern const osThreadAttr_t TaskCalender_attributes;
extern void TaskCalender(void* argument);


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_TASK_CALENDER_H__
