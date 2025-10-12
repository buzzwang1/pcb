#ifndef __SYSTEM_TASK_APP_H__
#define __SYSTEM_TASK_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

extern const osThreadAttr_t TaskApp_attributes;
extern void TaskApp(void* argument);


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_TASK_APP_H__
