#ifndef ITPM_C1_H
#define ITPM_C1_H

#ifdef __cplusplus
  extern "C" {
#endif

#ifdef ITPM_ENABLED

/* System-headerfiles */
#include  "Typedef.h"

/* Own headerfiles */
#include  "ITPM1ce.h"
#include  "ITPM1ci.h"
#include  "ITPMLog1c1.h"


/*!< CYCCNTENA bit in DWT_CONTROL register */
#define ITPM_DWT_CYCCNTENA_BIT         (1UL<<DWT_CTRL_CYCCNTENA_Pos)
#define ITPM_DWT_CPIEVTENA_BIT         (1UL<<DWT_CTRL_CPIEVTENA_Pos)

/*!< Trace enable bit in DEMCR register */
#define ITPM_M4TRCENA_BIT              (1UL<<CoreDebug_DEMCR_TRCENA_Pos)


/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */
#define ITPM_InitCycleCounter()    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Pos
/*!< Reset cycle counter */
#define ITPM_ResetCycleCounter()   DWT->CYCCNT = 0
/*!< Enable cycle counter */
#define ITPM_EnableCycleCounter()  DWT->CTRL |= (ITPM_DWT_CYCCNTENA_BIT  /*| ITPM_DWT_CPIEVTENA_BIT*/)
/*!< Disable cycle counter */
#define ITPM_DisableCycleCounter() DWT->CTRL &= ~(ITPM_DWT_CYCCNTENA_BIT /*| ITPM_DWT_CPIEVTENA_BIT*/)
/*!< Read cycle counter register */
/*
  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0337e/ch11s05s01.html
  [31:0]	CYCCNT	
  Current PC Sampler Cycle Counter count value. When enabled, this counter counts the number of core cycles, except when the core is halted.
  CYCCNT is a free running counter, counting upwards. It wraps around to 0 on overflow.
  
  The debugger must initialize this to 0 when first enabling.
*/

#define ITPM_GetCycleCounter()     DWT->CYCCNT




#define ITPM_vSTM(x) ITPM_vStartMeasureCont(x)
#define ITPM_vSPM(x) ITPM_vStopMeasureCont(x)

#define ITPM_vSTM_Res(x) ITPM_vStartMeasureRes(x)
#define ITPM_vSPM_Res(x) ITPM_vStopMeasureRes(x)

#define ITPM_vSTM_Sys(x) ITPM_vStartMeasureSystem(x)
#define ITPM_vSPM_Sys(x) ITPM_vStopMeasureSystem(x)

#define ITPM_vSTM_Pwr(x) ITPM_vStartMeasurePower(x)
#define ITPM_vSPM_Pwr(x) ITPM_vStopMeasurePower(x)

#define ITPM_vSini()     ITPM_vSTM_Sys(ITPM_nenSini)
#define ITPM_vSwini()    ITPM_vSTM_Sys(ITPM_nenSwini)


void ITPM_vReset(void);
//void ITPM_vSini(void);
//void ITPM_vSwini(void);
void ITPM_vCalculation_Tsk(void);

void Itpm_vTsArraySet(ITPM_tenTsArray lenIndex, ITPM_tenTsArray lenIndexRef);
void Itpm_vCycArraySet(ITPM_tenTsArray lenIndex, ITPM_tenTsArray lenIndexRef);

//General measuring points
void ITPM_vStopMeasureCont(ITPM_tenMeasurePoints enMeasurePoint);
void ITPM_vStartMeasureCont(ITPM_tenMeasurePoints enMeasurePoint);
  //Resource Tree measuring points
  void ITPM_vStopMeasureRes(ITPM_tenMeasurePoints enMeasurePoint);
  void ITPM_vStartMeasureRes(ITPM_tenMeasurePoints enMeasurePoint);
  //System measuring points
  void ITPM_vStopMeasureSystem(ITPM_tenMeasurePoints enMeasurePoint);
  void ITPM_vStartMeasureSystem(ITPM_tenMeasurePoints enMeasurePoint);
  //Power measuring points
  void ITPM_vStopMeasurePower(ITPM_tenMeasurePoints enMeasurePoint);
  void ITPM_vStartMeasurePower(ITPM_tenMeasurePoints enMeasurePoint);

void ITPM_vRes_Set(uint8 u8ResIndex);
void ITPM_vRes_Clear(uint8 u8ResIndex);

//Task
void ITPM_vTask_PreTaskHook(uint8 u8TaskIndex);
void ITPM_vTask_PostTaskHook(uint8 u8TaskIndex);

void ITPM_vTask_ActivateTask_Entry(uint8 u8TaskIndex);
void ITPM_vTask_ActivateTask_Exit(uint8 u8TaskIndex);
void ITPM_vTask_TerminateTask_Entry(uint8 u8TaskIndex);
void ITPM_vTask_TerminateTask_Exit(uint8 u8TaskIndex);
void ITPM_vTask_Containter_Entry(uint32 u32FctPointer);
void ITPM_vTask_Container_Exit(uint32 u32FctPointer);

//Scheduler
void ITPM_vSchedule_Entry(void);
void ITPM_vSchedule_Exit(void);

//Interrupts
void ITPM_vIRQ_Entry(uint8 u8IRQIndex);
void ITPM_vIRQ_Exit(uint8 u8IRQIndex);
void ITPM_vIRQLock_Entry(void);
void ITPM_vIRQLock_Exit(void);

// User Test Point
void ITPM_vUser_Log(uint32 u32AnyNumber);

void ITPM_vInitSysTimer (void);
void ITPM_vInitCycCounter(void);
void ITPM_vWaitCycles(uint32 lui32Ticks);

#endif //ITPM_ENABLED

#ifdef __cplusplus
}
#endif

#endif



