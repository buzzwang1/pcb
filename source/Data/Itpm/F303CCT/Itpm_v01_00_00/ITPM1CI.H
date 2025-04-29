#ifndef ITPM_CI_H
#define ITPM_CI_H


#ifdef __cplusplus
  extern "C" {
#endif

#ifdef ITPM_ENABLED

/* System-headerfiles */
#include  "Typedef.h"


/* Delay of start of the measurment, e.g. to don't have the start up phase in the measurement */
#define ITPM_MEASUREMENT_DELAY         30      /* set to 30s, for testing... */

/* Duration of the measurment */
#define ITPM_MEASUREMENT_TIME          100      /* integrator can change as needed... */

/* as ITPM adds a lot of overhead in the system, it's functionalities can be switched off independently, if needed */
#define ITPM_nboResoursekTiming       (0)    /* RAM usage: ITPM_nLastMeasureIndex *  8Byte, e.g. 560A HL 24* 8B =   192 */
#define ITPM_nboTaskTiming            (0)    /* RAM usage: osdNumberOfAllTasks    * 72Byte, e.g. 560A HL 23*84B =  1932 */
  #define ITPM_nboTaskTimingNoIrq       (0)  /* Subtracts the IRQ runtime from the task runtime. !Only possible, if nested interrupts aren't used */
  #define ITPM_nboTaskRTContainer       (0)  /* RAM usage: osdNumberOfAllTasks    * 20Byte, e.g. 560A HL 23*20B =   460 */
#define ITPM_nboScheduleTiming        (0)    /* RAM usage: 40Byte                           e.g. 560A HL    40B =    40 */
  #define ITPM_nboScheduleHighScore     (0)  /* RAM usage: 5 * 12 for Highscore             e.g. 560A HL    60B =    60 */
#define ITPM_nboIRQTiming             (0)    /* RAM usage: ITPM_nNoOfInterrupts   * 38Byte, e.g. 560A HL 26*38B =  1012 */
#define ITPM_nboIRQLockTiming         (0)    /* ! Not use that, in case FRT timestamp reference */
                                             /* ! Additional, Memory Protection must be deactivated  */


/* Log Events */
#define ITPM_nboLogEvents_System      (0)
#define ITPM_nboLogEvents_Power       (0)
#define ITPM_nboLogEvents_Res         (0)
#define ITPM_nboLogEvents_ResReq      (0)

#define ITPM_nboLogEvents_IRQs        (0)
#define ITPM_nboLogEvents_IRQLocks    (0) /* ! Not use that, in case FRT timestamp reference */
                                          /* ! Additional, Memory Protection must be deactivated  */

#define ITPM_nboLogEvents_Scheduler   (0)

#define ITPM_nboLogEvents_Task_All    (0)
#define   ITPM_nboLogEvents_Task_AT_Entry   (0 || ITPM_nboLogEvents_Task_All)
#define   ITPM_nboLogEvents_Task_AT_Exit    (0 || ITPM_nboLogEvents_Task_All)
#define   ITPM_nboLogEvents_Task_PreHook    (0 || ITPM_nboLogEvents_Task_All)
#define   ITPM_nboLogEvents_Task_PostHook   (0 || ITPM_nboLogEvents_Task_All)
#define   ITPM_nboLogEvents_Task_TT_Entry   (0 || ITPM_nboLogEvents_Task_All)
#define   ITPM_nboLogEvents_Task_TT_Exit    (0 || ITPM_nboLogEvents_Task_All)
#define   ITPM_nboLogEvents_Task_Cont_Entry (0 || ITPM_nboLogEvents_Task_All)
#define   ITPM_nboLogEvents_Task_Cont_Exit  (0 || ITPM_nboLogEvents_Task_All)
#define ITPM_nboLogEvents_User        (0)

/* Only for test */
/* Can be used to toggle a Pin at a certain point */
#define ITPM_nTogglePin01On()   DIO_nUBuffCmdDirOutFastREAR_SEAT_C(0);DIO_nUBuffOutPhyFastREAR_SEAT_C(1);
#define ITPM_nTogglePin01Off()  DIO_nUBuffOutPhyFastREAR_SEAT_C(0);


/* Can be used, e.g. to toggle a Pin at a certain position and do scope measurements */
/* e.g. Toggling a pin in ITPM_Reset, the delay from reset/wakeup until Start of ITPM can be measured by scope*/
#define ITPM_nUserAction_Entry_ITPM_Reset(tParam)       //ITPM_nTogglePin01On()
#define ITPM_nUserAction_Exit_ITPM_Reset(tParam)        //ITPM_nTogglePin01Off()

#define ITPM_nUserAction_Entry_System_Start(tParam)
#define ITPM_nUserAction_Exit_System_Start(tParam)
#define ITPM_nUserAction_Entry_System_Stop(tParam)
#define ITPM_nUserAction_Exit_System_Stop(tParam)

#define ITPM_nUserAction_Entry_Scheduler_Entry(tParam)
#define ITPM_nUserAction_Exit_Scheduler_Entry(tParam)
#define ITPM_nUserAction_Entry_Scheduler_Exit(tParam)
#define ITPM_nUserAction_Exit_Scheduler_Exit(tParam)

#define ITPM_nUserAction_Entry_Power_Start(tParam)
#define ITPM_nUserAction_Exit_Power_Start(tParam)
#define ITPM_nUserAction_Entry_Power_Stop(tParam)
#define ITPM_nUserAction_Exit_Power_Stop(tParam)

#define ITPM_nUserAction_Entry_IRQs_Entry(tParam)
#define ITPM_nUserAction_Exit_IRQs_Entry(tParam)
#define ITPM_nUserAction_Entry_IRQs_Exit(tParam)
#define ITPM_nUserAction_Exit_IRQs_Exit(tParam)

#define ITPM_nUserAction_Entry_IRQLocks_Entry(tParam)
#define ITPM_nUserAction_Exit_IRQLocks_Entry(tParam)
#define ITPM_nUserAction_Entry_IRQLocks_Exit(tParam)
#define ITPM_nUserAction_Exit_IRQLocks_Exit(tParam)

#define ITPM_nUserAction_Entry_Res_Start(tParam)
#define ITPM_nUserAction_Exit_Res_Start(tParam)
#define ITPM_nUserAction_Entry_Res_Stop(tParam)
#define ITPM_nUserAction_Exit_Res_Stop(tParam)

#define ITPM_nUserAction_Entry_Res_Set(tParam)
#define ITPM_nUserAction_Exit_Res_Set(tParam)
#define ITPM_nUserAction_Entry_Res_Clear(tParam)
#define ITPM_nUserAction_Exit_Res_Clear(tParam)


#define ITPM_nUserAction_Entry_Task_AT_Entry(tParam)
#define ITPM_nUserAction_Exit_Task_AT_Entry(tParam)
#define ITPM_nUserAction_Entry_Task_AT_Exit(tParam)
#define ITPM_nUserAction_Exit_Task_AT_Exit(tParam)
#define ITPM_nUserAction_Entry_Task_PreHook(tParam)
#define ITPM_nUserAction_Exit_Task_PreHook(tParam)
#define ITPM_nUserAction_Entry_Task_PostHook(tParam)
#define ITPM_nUserAction_Exit_Task_PostHook(tParam)
#define ITPM_nUserAction_Entry_Task_TT_Entry(tParam)
#define ITPM_nUserAction_Exit_Task_TT_Entry(tParam)
#define ITPM_nUserAction_Entry_Task_TT_Exit(tParam)
#define ITPM_nUserAction_Exit_Task_TT_Exit(tParam)
#define ITPM_nUserAction_Entry_Task_Cont_Entry(tParam)
#define ITPM_nUserAction_Exit_Task_Cont_Entry(tParam)
#define ITPM_nUserAction_Entry_Task_Cont_Exit(tParam)
#define ITPM_nUserAction_Exit_Task_Cont_Exit(tParam)
#define ITPM_nUserAction_Entry_User(tParam)
#define ITPM_nUserAction_Exit_User(tParam)

#endif //ITPM_ENABLED

#ifdef __cplusplus
}
#endif

#endif


