#ifndef ITPM_CE_H
#define ITPM_CE_H

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef ITPM_ENABLED

#include "Typedef.h"


typedef enum
{
  ITMP_nTsArray00 = 0,
  ITMP_nTsArray01,
  ITMP_nTsArray02,
  ITMP_nTsArray03,
  ITMP_nTsArray04,
  ITMP_nTsArray05,
  ITMP_nTsArray06,
  ITMP_nTsArray07,
  ITMP_nTsArray08,
  ITMP_nTsArray09,
  ITMP_nTsArrayMax
}ITPM_tenTsArray;


typedef enum
{
	ITMP_nIrqReset = 0,
  //ITMP_nIrqNMI,
  //ITMP_nIrqHardFault,
  //ITMP_nIrqMemManage,
  //ITMP_nIrqBusFault,
  //ITMP_nIrqUsageFault,
  //ITMP_nIrqSVC,
  //ITMP_nIrqDebugMon,
  //ITMP_nIrqPendSV,
  //ITMP_nIrqSysTick,
  //ITMP_nIrqWWDG_IRQ,
  //ITMP_nIrqPVD_IRQ,
  //ITMP_nIrqTAMPER_STAMP_IRQ,
  //ITMP_nIrqRTC_WKUP_IRQ,
  //ITMP_nIrqFLASH_IRQ,
  //ITMP_nIrqRCC_IRQ,
  //ITMP_nIrqEXTI0_IRQ,
  //ITMP_nIrqEXTI1_IRQ,
  //ITMP_nIrqEXTI2_TS_IRQ,
  //ITMP_nIrqEXTI3_IRQ,
  //ITMP_nIrqEXTI4_IRQ,
  //ITMP_nIrqDMA1_Channel1_IRQ,
  //ITMP_nIrqDMA1_Channel2_IRQ,
  //ITMP_nIrqDMA1_Channel3_IRQ,
  ITMP_nIrqDMA1_Channel4_IRQ,
  ITMP_nIrqDMA1_Channel5_IRQ,
  ITMP_nIrqDMA1_Channel6_IRQ,
  ITMP_nIrqDMA1_Channel7_IRQ,
  //ITMP_nIrqADC1_2_IRQ,
  //ITMP_nIrqUSB_HP_CAN1_TX_IRQ,
  //ITMP_nIrqUSB_LP_CAN1_RX0_IRQ,
  //ITMP_nIrqCAN1_RX1_IRQ,
  //ITMP_nIrqCAN1_SCE_IRQ,
  //ITMP_nIrqEXTI9_5_IRQ,
  //ITMP_nIrqTIM1_BRK_TIM15_IRQ,
  //ITMP_nIrqTIM1_UP_TIM16_IRQ,
  //ITMP_nIrqTIM1_TRG_COM_TIM17_IRQ,
  //ITMP_nIrqTIM1_CC_IRQ,
  //ITMP_nIrqTIM2_IRQ,
  //ITMP_nIrqTIM3_IRQ,
  //ITMP_nIrqTIM4_IRQ,
  ITMP_nIrqI2C1_EV_IRQ,
  ITMP_nIrqI2C1_ER_IRQ,
  ITMP_nIrqI2C2_EV_IRQ,
  ITMP_nIrqI2C2_ER_IRQ,
  //ITMP_nIrqSPI1_IRQ,
  //ITMP_nIrqSPI2_IRQ,
  //ITMP_nIrqUSART1_IRQ,
  //ITMP_nIrqUSART2_IRQ,
  //ITMP_nIrqUSART3_IRQ,
  //ITMP_nIrqEXTI15_10_IRQ,
  //ITMP_nIrqRTC_Alarm_IRQ,
  //ITMP_nIrqUSBWakeUp_IRQ,
  //ITMP_nIrqTIM8_BRK_IRQ,
  //ITMP_nIrqTIM8_UP_IRQ,
  //ITMP_nIrqTIM8_TRG_COM_IRQ,
  //ITMP_nIrqTIM8_CC_IRQ,
  //ITMP_nIrqADC3_IRQ,
  //ITMP_nIrqSPI3_IRQ,
  //ITMP_nIrqUART4_IRQ,
  //ITMP_nIrqUART5_IRQ,
  //ITMP_nIrqTIM6_DAC_IRQ,
  //ITMP_nIrqTIM7_IRQ,
  //ITMP_nIrqDMA2_Channel1_IRQ,
  //ITMP_nIrqDMA2_Channel2_IRQ,
  //ITMP_nIrqDMA2_Channel3_IRQ,
  //ITMP_nIrqDMA2_Channel4_IRQ,
  //ITMP_nIrqDMA2_Channel5_IRQ,
  //ITMP_nIrqADC4_IRQ,
  //ITMP_nIrqCOMP1_2_3_IRQ,
  //ITMP_nIrqCOMP4_5_6_IRQ,
  //ITMP_nIrqCOMP7_IRQ,
  //ITMP_nIrqUSB_HP_IRQ,
  //ITMP_nIrqUSB_LP_IRQ,
  //ITMP_nIrqUSBWakeUp_RMP_IRQ,
	//ITMP_nIrqFPU_IRQ,
	ITMP_nIrqCount
}ITPM_tenIrqs;

typedef enum
{
	ITMP_nTaskMain = 0,
	ITMP_nTaskIdle,
	ITMP_nTaskCount
}ITPM_tenTasks;


/* configure the number of interrupts available in the system */
#define ITPM_nNoOfInterrupts        ITMP_nIrqCount
#define ITPM_nNoOfTasks             ITMP_nTaskCount


typedef enum
{
  /* System States */
  ITPM_nenReset = 0,
  ITPM_nenSini,
  ITPM_nenSwini,

  /* Internal resources */

  /* External resources */

  /* Power States */

  ITPM_nLastMeasureIndex

} ITPM_tenMeasurePoints;


typedef enum
{
  ITPM_nenPreTaskHook = 0,
  ITPM_nenPostTaskHook,

  ITPM_nenActivateTask,
  ITPM_nenTerminateTask,

  ITPM_nenDummy

} ITPM_tenCalcTaskConsts;

#endif // ITPM_ENABLED

#ifdef __cplusplus
}
#endif



#endif


