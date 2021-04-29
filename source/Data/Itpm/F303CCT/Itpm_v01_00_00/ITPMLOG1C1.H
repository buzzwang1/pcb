#ifndef ITPMLOG_C1_H
#define ITPMLOG_C1_H


#ifdef __cplusplus
 extern "C" {
#endif

#ifdef ITPM_ENABLED

#include  "Typedef.h"
#include  "ITPMLOG1CI.H"
#include  "ITPMFRT1C1.H"


typedef enum
{
  ITPMLOG_nenEvt_NoEvent = 0,

  ITPMLOG_nenEvt_Res_InitEntry,    /* Init/Deinit of resources*/
  ITPMLOG_nenEvt_Res_InitExit,
  ITPMLOG_nenEvt_Res_DeinitEntry,
  ITPMLOG_nenEvt_Res_DeinitExit,

  ITPMLOG_nenEvt_Res_Set,
  ITPMLOG_nenEvt_Res_Clear,


  ITPMLOG_nenEvt_SystemEntry,  /* like Sini, Swini, Reset,.SysStartupTime, SysInitTime, OSEKStartupTime, SysWakeupTime */
  ITPMLOG_nenEvt_SystemExit,   /* like Sini, Swini, Reset,.SysStartupTime, SysInitTime, OSEKStartupTime, SysWakeupTime */

  ITPMLOG_nenEvt_PowerStateEntry,  /*  UV2, UV1, ... OV1 ....*/
  ITPMLOG_nenEvt_PowerStateExit,   /*  UV2, UV1, ... OV1 ....*/


  ITPMLOG_nenEvt_Task_ActivateEntry,
  ITPMLOG_nenEvt_Task_ActivateExit,
  ITPMLOG_nenEvt_Task_TerminateEntry,
  ITPMLOG_nenEvt_Task_TerminateExit,
  ITPMLOG_nenEvt_Task_PreHook,
  ITPMLOG_nenEvt_Task_PostHook,
  ITPMLOG_nenEvt_Task_ContainerEntry,
  ITPMLOG_nenEvt_Task_ContainerExit,

  ITPMLOG_nenEvt_Int_Entry,
  ITPMLOG_nenEvt_Int_Exit,

  ITPMLOG_nenEvt_IntLock_Entry,
  ITPMLOG_nenEvt_IntLock_Exit,

  ITPMLOG_nenEvt_Scheduler_Entry,
  ITPMLOG_nenEvt_Scheduler_Exit,

  ITPMLOG_nenEvt_User,

  ITPMLOG_nenEvt_Last

} ITPMLOG_tenEvents;



typedef struct
{
  uint32            u32TimeStamp;   /* Timestamp in us */
  ITPMLOG_tenEvents enEvent;        /* type of Event */
  uint8             u8AddInfo[3];   /* Additional info like: Interrupt number or Task ID */
} ITPMLOG_tstEvents;


typedef struct
{
  ITPMLOG_tstEvents stEvents[ITPMLOG_nNumberOfElements];

  uint16 u16Size;    // Total size of buffer
  uint16 u16Count;   // Current number of bytes in buffer
  uint16 u16Inpos;   // current in and out position
  uint16 u16Outpos;
  uint16 u16LogCountDown; // Stop logging after that counts
} ITPMLOG_tstEventRingBuffer;


extern ITPMLOG_tstEventRingBuffer ITPMLOG_stEventRingBuffer;


void ITPMLOG_vReset(void);

void ITPMLOG_vSini(void);
void ITPMLOG_vSwini(void);

void ITPMLOG_vSaveToBURAM(void);
void ITPMLOG_vRestoreFromBURAM(void);

void ITPMLOG_vEvent_ClearAll(void);
void ITPMLOG_vEvent_SetCountdown(uint16 u16CountDown);
void ITPMLOG_vEvent_Add(ITPMLOG_tenEvents enEvent, uint32 u32AddInfo);

/*
const char* ITPMLOG_pchEvent_toString(ITPMLOG_tenEvents enEvent);
void ITPMLOG_vEvent_PrintAll(void);
*/

#endif // ITPM_ENABLED

#ifdef __cplusplus
}
#endif

#endif


