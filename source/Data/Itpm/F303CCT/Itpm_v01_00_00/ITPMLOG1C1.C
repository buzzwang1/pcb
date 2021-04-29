
#ifdef ITPM_ENABLED


/* System-headerfiles */


/* Foreign headerfiles */

/* Own headerfiles */
#include "ITPM1ce.h"
#include "ITPMLOG1c1.h"

/*
const char* const ITPMLOG_chEvents[ITPMLOG_nenEvt_Last] =
{
  "NoEvent",

  "Res_InitEntry",
  "Res_InitExit",
  "Res_DeinitEntry",
  "Res_DeinitExit",

  "Res_Set",
  "Res_Clear",

  "SystemEntry",
  "SystemExit",

  "PowerStateEntry",
  "PowerStateExit",


  "Task_ActivateEntry",
  "Task_ActivateExit",
  "Task_TerminateEntry",
  "Task_TerminateExit",
  "Task_PreHook",
  "Task_PostHook",
  "Task_ContEntry",
  "Task_ContExit",

  "Int_Entry",
  "Int_Exit",

  "IntLock_Entry",
  "IntLock_Exit",

  "Scheduler_Entry",
  "Scheduler_Exit",

  "User"
};*/

ITPMLOG_tstEventRingBuffer ITPMLOG_stEventRingBuffer;


void ITPMLOG_vRingBufInit(void);
void ITPMLOG_vRingBufClear(void);
void ITPMLOG_vRingBufEnQueue(ITPMLOG_tenEvents enEvent, uint32 u32AddInfo);


void ITPMLOG_vRingBufInit(void)
{
  ITPMLOG_stEventRingBuffer.u16Size   = ITPMLOG_nNumberOfElements;
  ITPMLOG_stEventRingBuffer.u16Count  = 0;
  ITPMLOG_stEventRingBuffer.u16Inpos  = 0;
  ITPMLOG_stEventRingBuffer.u16Outpos = 0;
  ITPMLOG_stEventRingBuffer.u16LogCountDown = UINT16_MAX;
}

void ITPMLOG_vRingBufClear(void)
{
  ITPMLOG_vRingBufInit();
}

void ITPMLOG_vRingBufEnQueue(ITPMLOG_tenEvents enEvent, uint32 u32AddInfo)
{
  // start critical section, because recursive calls (e.g. by task <=> interrupt) corrupts the event stack
  if  ((enEvent != ITPMLOG_nenEvt_IntLock_Entry) &&
       (enEvent != ITPMLOG_nenEvt_IntLock_Exit))
  {
    ITPM_vDisableAndSave_Int();
  }

  // if fifo is full, check if logging is continued
#if (ITPMLOG_nLogForEver == 1)
  if (ITPMLOG_stEventRingBuffer.u16LogCountDown > 0)
  {
    if (ITPMLOG_stEventRingBuffer.u16LogCountDown != UINT16_MAX)
    {
      ITPMLOG_stEventRingBuffer.u16LogCountDown--;
    }

    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u32TimeStamp = ITPM_xValueToUSec(ITPM_u32GetTimeStamp());
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].enEvent      = enEvent;
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u8AddInfo[0] = (uint8)(u32AddInfo >>  0); //Bit  0..7
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u8AddInfo[1] = (uint8)(u32AddInfo >>  8); //Bit  8..15
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u8AddInfo[2] = (uint8)(u32AddInfo >> 16); //Bit 16..23

    ITPMLOG_stEventRingBuffer.u16Inpos++; // increase in-position counter
    if (ITPMLOG_stEventRingBuffer.u16Inpos >= ITPMLOG_stEventRingBuffer.u16Size) // if we have reached the end of queue, go to beginning
    {
      ITPMLOG_stEventRingBuffer.u16Inpos = 0;
    }

    if (ITPMLOG_stEventRingBuffer.u16Count < ITPMLOG_stEventRingBuffer.u16Size)
    {
      ITPMLOG_stEventRingBuffer.u16Count++; // number of bytes in buffer
    }
    else
    {
      ITPMLOG_stEventRingBuffer.u16Outpos++;
      if (ITPMLOG_stEventRingBuffer.u16Outpos >= ITPMLOG_stEventRingBuffer.u16Size)
      {
        ITPMLOG_stEventRingBuffer.u16Outpos = 0;
      }
    }
  }
#endif


  // if fifo is full, check if logging is continued
#if (ITPMLOG_nLogForEver == 0)
  if (ITPMLOG_stEventRingBuffer.u16Count < ITPMLOG_stEventRingBuffer.u16Size)
  {
    // store data
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u32TimeStamp = ITPM_xValueToUSec(ITPM_u32GetTimeStamp());
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].enEvent = enEvent;
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u8AddInfo[0] = (uint8)(u32AddInfo >>  0); //Bit  0..7
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u8AddInfo[1] = (uint8)(u32AddInfo >>  8); //Bit  8..15
    ITPMLOG_stEventRingBuffer.stEvents[ITPMLOG_stEventRingBuffer.u16Inpos].u8AddInfo[2] = (uint8)(u32AddInfo >> 16); //Bit 16..23

    ITPMLOG_stEventRingBuffer.u16Inpos++; // increase in-position counter
    ITPMLOG_stEventRingBuffer.u16Count++; // number of bytes in buffer
  }
#endif

  // end critical section
  if  ((enEvent != ITPMLOG_nenEvt_IntLock_Entry) &&
       (enEvent != ITPMLOG_nenEvt_IntLock_Exit))
  {
    ITPM_vRestore_Int();
  }
}



void ITPMLOG_vReset(void)
{
  ITPMLOG_vRingBufInit();
}

void ITPMLOG_vSini(void)
{
}

void ITPMLOG_vSwini(void)
{
}

void ITPMLOG_vSaveToBURAM(void)
{
}

void ITPMLOG_vRestoreFromBURAM(void)
{
}

void ITPMLOG_vEvent_SetCountdown(uint16 u16CountDown)
{
    ITPMLOG_stEventRingBuffer.u16LogCountDown = u16CountDown;
}


void ITPMLOG_vEvent_ClearAll(void)
{
  ITPMLOG_vRingBufClear();
}

void ITPMLOG_vEvent_Add(ITPMLOG_tenEvents enEvent, uint32 u32AddInfo)
{
  ITPMLOG_vRingBufEnQueue(enEvent, u32AddInfo);
}

/*
const char* ITPMLOG_pchEvent_toString(ITPMLOG_tenEvents enEvent)
{
  if ((enEvent > ITPMLOG_nenEvt_NoEvent) && (enEvent < ITPMLOG_nenEvt_Last))
  {
    return (const char*)(ITPMLOG_chEvents[enEvent]);
  }
  else
  {
    return (const char*)(ITPMLOG_chEvents[0]);
  }
}


void ITPMLOG_vEvent_PrintAll(void)
{
}
*/

#endif
