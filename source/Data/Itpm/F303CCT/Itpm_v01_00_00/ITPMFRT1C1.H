#ifndef ITPMFRT_C1_H
#define ITPMFRT_C1_H

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef ITPM_ENABLED

/* System-headerfiles */
#include  "Typedef.h"

#include "stm32f30x_rcc.h"
#include "stm32f30x_tim.h"

#define ITPM_vDisableAndSave_Int()
#define ITPM_vRestore_Int()

/* 1us * 32bit = 4.294.967.296us = 4294s = 71,58 min = 1,19h */

/* Convert a usec value to a number
This is performed in the time domain the 32 bit HW counter is operating
*/
#define ITPM_xUSecToValue(PuSec) (PuSec) /* 1 Tick = 1us */


/* Convert a Value measured by the HW timer to usec */
#define ITPM_xValueToUSec(PVal)  (PVal)  /* 1 Tick = 1us */

/* Convert a Value measured by the HW timer to msec */
#define ITPM_xValueToMSec(PVal)  (ITPM_xValueToUSec(PVal) / 1000)
#define ITPM_xValueToSec(PVal)   (ITPM_xValueToMSec(PVal) / 1000)

#define ITPM_nSecond            1000 * 1000

#define ITPM_u32GetTimeStamp() ITPM_u32MeasurementTimer_Read()

/* Description : This macro reads the time-stamp from the 32 bit HW timer, which is a 32-bit up counter
*/
uint32 ITPM_u32MeasurementTimer_Read(void);

/* Descripton: Init a 32 bit hardware timer. The prescaler is actually set in the pman1ce.h file
There are 4 prescalers for alltogether 4 pieces of 32 bit HW TAUJ clocks
Currently the prescaler sets it to a 1.25MHz resolution
*/
void   ITPM_vMeasurementTimer_Reset(void);

#endif // ITPM_ENABLED

#ifdef __cplusplus
}
#endif

#endif



