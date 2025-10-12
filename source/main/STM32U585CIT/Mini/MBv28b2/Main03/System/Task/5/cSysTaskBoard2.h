#ifndef __SYSTEM_TASK_BOARD2_H__
#define __SYSTEM_TASK_BOARD2_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"

extern const osThreadAttr_t TaskBoard2_attributes;
extern void TaskBoard2(void* argument);


#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_TASK_BOARD2_H__
