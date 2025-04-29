

#ifdef ITPM_ENABLED

/***************************************************************************
* HEADER-FILES (Only those that are needed in this file)
****************************************************************************/
/* System-headerfiles */

/* Foreign headerfiles */

/* Own headerfiles */
#include  "ITPM1c1.h"                                  /* Export interface (Own interface)*/


#warning "DON'T FORGET TO DEACTIVATE INTERNAL TIMING MEASURING FOR FINAL RELEASE !!!!!!!!"


/* Variables to control the measuring */
    /* Delay of start of the measurement, e.g. to don't have the start up phase in the measurement */
    volatile uint32 ITPM_u32MeasuringDelay;

    /* Duration of the measurement. */
    volatile uint32 ITPM_u32MeasuringTimeCnt;

    /* For calculating the overall measuring time */
    uint32 ITPM_u32FrtTest_Start; /* Timestamp taken after Start of the measuring */
    uint32 ITPM_u32FrtTest;


    /* flag indicating if reset was already performed, in case ITPM_nMeassuringDelay > 0 */
    bool ITPM__boLogEnable;

    #if (0 != ITPM_nboTaskTimingNoIrq)
      uint32 ITPM__u32IRQ_Runtime; /* Accumulated IRQ runtime */
    #endif



/* Variables to control the "measure points" measuring */
    /*Call TimeStamp*/

  #if (0 != ITPM_nboResoursekTiming)
    typedef struct
    {
      uint32 u32Start;
      uint32 u32Stop;
    }ITPM__tstMesPnt_TSmp;

    typedef struct
    {
      ITPM__tstMesPnt_TSmp       stTSmp;  /*Call TimeStamp*/
    }ITPM__tstMesPnt;

    typedef struct
    {
      ITPM__tstMesPnt       stMesPnt[ITPM_nLastMeasureIndex];
    }ITPM__tstMesPnts;

    ITPM__tstMesPnts ITPM__stMesPnts;
  #endif

  uint32 ITPM__u32Task_ActiveTaskIdx;
  uint32 ITPM__u32Task_ActiveContIdx;
  #if (0 != ITPM_nboTaskTiming)
    /* Variables to control the task measuring */

    /*Call Counter*/
    typedef struct
    {
      uint32 u32Pre;
      uint32 u32Act;  /*Activate*/
      uint32 u32Tte;  /*Terminate*/
      uint32 u32Post;
    }ITPM__tstTask_Cnt;

    /*Call TimeStamp*/
    typedef struct
    {
      uint32 u32ActTask;
      uint32 u32ActTaskLast;
      uint32 u32PreTask;
      uint32 u32PostTask;
    }ITPM__tstTask_TSmp;

    /*Cycle Time */
    typedef struct
    {
      uint32 u32Max;
      uint32 u32Min;
    }ITPM__tstTask_Cyc;

    /*Runtime*/
    typedef struct
    {
      uint32 u32All;

      //Min. Max. Time between Pre Post
      uint32 u32PrePostMax;
      uint32 u32PrePostMin;

      //Min. Max. Time between Activate and terminate
      uint32 u32AtTtTemp;
      uint32 u32AtTtMax;
      uint32 u32AtTtMin;

    }ITPM__tstTask_RT;


    /*Latency*/
    typedef struct
    {
      uint32 u32StartCnt;
      uint32 u32MultipleActCnt;
      uint32 u32Max;
      uint32 u32Min;
      uint32 u32Total;
    }ITPM__tstTask_Lat;


    #if ITPM_nboTaskRTContainer
      /* Container Runtime */
      typedef struct
      {
        uint32 u32MaxRT;            // Max Container runtime
        uint32 u32MaxFctPtr;        // Function Pointer of the Container with the max runtime
        uint32 u32LastContIdx;      // Temp variable to measure the max runtime
        uint32 u32LastContStart;    // Temp variable to measure the max runtime
      }ITPM__tstTask_Cont;
    #endif

    typedef struct
    {
      ITPM__tstTask_Cnt    stCnt;   /*Call Counter*/
      ITPM__tstTask_TSmp   stTSmp;  /*Call TimeStamp*/
      ITPM__tstTask_Cyc    stCyc;   /*Cycle Time */
      ITPM__tstTask_RT     stRT;    /*Runtime*/
      ITPM__tstTask_Lat    stLat;   /*Latency*/
	  #if ITPM_nboTaskRTContainer
        ITPM__tstTask_Cont   stCont;  /*Containers*/
	  #endif
    }ITPM__tstTask;

    typedef struct
    {
      ITPM__tstTask       stTask[ITPM_nNoOfTasks];
    }ITPM__tstTasks;

    ITPM__tstTasks ITPM__stTasks;
  #endif




/* Variables to control the interrupt measuring */
  #if (0 != ITPM_nboIRQTiming)

    /* Used for interrupt runtime. Information parsed from */

    /*Call Counter*/
    typedef struct
    {
      uint32 u32Start;
      uint32 u32Stop;
    }ITPM__tstInt_Cnt;

    /*Call TimeStamp*/
    typedef struct
    {
      uint32 u32Start;
      uint32 u32StartLast;
      uint32 u32Stop;
    }ITPM__tstInt_TSmp;

    /*Cycle Time */
    typedef struct
    {
      uint32 u32Max;
      uint32 u32Min;
    }ITPM__tstInt_Cyc;

    /*Runtime*/
    typedef struct
    {
      uint32 u32All;
      uint32 u32Max;
      uint32 u32Min;
    }ITPM__tstInt_RT;


    /* Call Tree */
    /* Holds the call depth of the interrupts */
    typedef struct
    {
      uint32 u32Depth;
      uint32 u32DepthMax;
    }ITPM__tstInts_Call;

    typedef struct
    {
      ITPM__tstInt_Cnt  stCnt;   /*Call Counter*/
      ITPM__tstInt_TSmp stTSmp;  /*Call TimeStamp*/
      ITPM__tstInt_Cyc  stCyc;   /*Cycle Time */
      ITPM__tstInt_RT   stRT;    /*Runtime*/
    }ITPM__tstInt;

    typedef struct
    {
      ITPM__tstInt       stInt[ITPM_nNoOfInterrupts];
      ITPM__tstInts_Call stCall;
    }ITPM__tstInts;

    ITPM__tstInts ITPM__stInts;
  #endif

  uint32 ITPM__u32Task_ActiveIRQIdx;
    /* Variables to control the interrupt lock time measuring */
  #if (0 != ITPM_nboIRQLockTiming)
  /* Used for interrupt lock time calculation. Information parsed from */
  /* 5.4	Timing Constraints, "Max. interrupt lock time" */
    #if SYSID_nboIsHL()
    uint32 ITPM__u32MaxIRQLockTime = ITPM_xUSecToValue(50);        /* 50 µs */
    #else
    uint32 ITPM__u32MaxIRQLockTime = ITPM_xUSecToValue(100);       /* 100 µs */
    #endif

    /*Call Counter*/
    typedef struct
    {
      uint32 u32Start;
    }ITPM__tstIRQLock_Cnt;

    /*Call TimeStamp*/
    typedef struct
    {
      uint32 u32Start;
      uint32 u32Stop;
    }ITPM__tstIRQLock_TSmp;

    /*Runtime*/
    typedef struct
    {
      uint32 u32All;
      uint32 u32Max;
      uint32 u32Min;
    }ITPM__tstIRQLock_RT;

    typedef struct
    {
      ITPM__tstIRQLock_Cnt  stCnt;   /*Call Counter*/
      ITPM__tstIRQLock_TSmp stTSmp;  /*Call TimeStamp*/
      ITPM__tstIRQLock_RT   stRT;    /*Runtime*/
    }ITPM__tstIRQLock;

    ITPM__tstIRQLock ITPM__stIRQLock;
  #endif



  #if (0 != ITPM_nboScheduleTiming)
  /* Used for schedule calls at the moment. Information parsed from */
    /*Call Counter*/
    typedef struct
    {
      uint32 u32Start;
    }ITPM__tstScheduler_Cnt;

    /*Call TimeStamp*/
    typedef struct
    {
      uint32 u32Start;
      uint32 u32Stop;
    }ITPM__tstScheduler_TSmp;

    /*Runtime*/
    typedef struct
    {
      // Scheduler Runtime
      uint32 u32Min;
      uint32 u32Max;
      uint32 u32All;

      //Runtime between Scheduler
      uint32 u32MinBreak;
      uint32 u32MaxBreak;
      uint32 u32AllBreak;
    }ITPM__tstScheduler_RT;

    // Highcore: 5 worst functions
    #if (0 != ITPM_nboScheduleHighScore)
      #define ITPM__nSchedulerHighscoreCnt 5
      typedef struct
      {
        uint32 u32Entries;
        uint32 u32FctPtr[ITPM__nSchedulerHighscoreCnt];
        uint32 u32FctRT[ITPM__nSchedulerHighscoreCnt];
        uint32 u32Task[ITPM__nSchedulerHighscoreCnt];
      }ITPM__tstScheduler_HS;
    #endif

    typedef struct
    {
      ITPM__tstScheduler_Cnt  stCnt;   /*Call Counter*/
      ITPM__tstScheduler_TSmp stTSmp;  /*Call TimeStamp*/
      ITPM__tstScheduler_RT   stRT;    /*Runtime*/
      #if (0 != ITPM_nboScheduleHighScore)
        ITPM__tstScheduler_HS   stHighScore;
      #endif
      uint32 u32Active;
    }ITPM__tstScheduler;

    ITPM__tstScheduler ITPM__stScheduler;
  #endif

uint32 ITMP_nTsArray[ITMP_nTsArrayMax];



void ITPM__vClearValues(void)
{
  uint32 u32Cnt;

  ITPM_vInitSysTimer();
  ITPM_vInitCycCounter();

  #if (0 != ITPM_nboTaskTimingNoIrq)
    ITPM__u32IRQ_Runtime = 0;
  #endif

  /* Measuring Points */
  #if (0 != ITPM_nboResoursekTiming)
    for (u32Cnt = 0; u32Cnt < ITPM_nLastMeasureIndex; u32Cnt++)
    {
      ITPM__stMesPnts.stMesPnt[u32Cnt].stTSmp.u32Start = 0;
      ITPM__stMesPnts.stMesPnt[u32Cnt].stTSmp.u32Stop  = 0;
    }
  #endif

  /* Tasks */
  ITPM__u32Task_ActiveTaskIdx = UINT32MAX;
  ITPM__u32Task_ActiveContIdx = UINT32MAX;
  #if (0 != ITPM_nboTaskTiming)
    for (u32Cnt = 0; u32Cnt < ITPM_nNoOfTasks; u32Cnt++)
    {
      /*Call Counter*/
      ITPM__stTasks.stTask[u32Cnt].stCnt.u32Act  = 0;
      ITPM__stTasks.stTask[u32Cnt].stCnt.u32Tte = 0;
      ITPM__stTasks.stTask[u32Cnt].stCnt.u32Post = 0;
      ITPM__stTasks.stTask[u32Cnt].stCnt.u32Pre  = 0;

      /*Call TimeStamp*/
      ITPM__stTasks.stTask[u32Cnt].stTSmp.u32ActTask =     0;
      ITPM__stTasks.stTask[u32Cnt].stTSmp.u32ActTaskLast = 0;
      ITPM__stTasks.stTask[u32Cnt].stTSmp.u32PreTask     = 0;
      ITPM__stTasks.stTask[u32Cnt].stTSmp.u32PostTask    = 0;

      /*Cyclic*/
      ITPM__stTasks.stTask[u32Cnt].stCyc.u32Max = 0;
      ITPM__stTasks.stTask[u32Cnt].stCyc.u32Min = UINT32MAX;

      /*Latency*/
      ITPM__stTasks.stTask[u32Cnt].stLat.u32Max = 0;
      ITPM__stTasks.stTask[u32Cnt].stLat.u32Min = UINT32MAX;
      ITPM__stTasks.stTask[u32Cnt].stLat.u32StartCnt = 0;
      ITPM__stTasks.stTask[u32Cnt].stLat.u32MultipleActCnt = 0;
      ITPM__stTasks.stTask[u32Cnt].stLat.u32Total    = 0;

      /*Runtime*/
      ITPM__stTasks.stTask[u32Cnt].stRT.u32All        = 0;
      ITPM__stTasks.stTask[u32Cnt].stRT.u32PrePostMax = 0;
      ITPM__stTasks.stTask[u32Cnt].stRT.u32PrePostMin = UINT32MAX;

      ITPM__stTasks.stTask[u32Cnt].stRT.u32AtTtTemp   = 0;
      ITPM__stTasks.stTask[u32Cnt].stRT.u32AtTtMax    = 0;
      ITPM__stTasks.stTask[u32Cnt].stRT.u32AtTtMin    = UINT32MAX;


      #if ITPM_nboTaskRTContainer
        /* Container Runtime */
        ITPM__stTasks.stTask[u32Cnt].stCont.u32MaxRT = 0;
        ITPM__stTasks.stTask[u32Cnt].stCont.u32MaxFctPtr = 0;
        ITPM__stTasks.stTask[u32Cnt].stCont.u32LastContIdx = 0;
        ITPM__stTasks.stTask[u32Cnt].stCont.u32LastContStart = 0;
      #endif
    }
  #endif

  /* Interrupt */
  ITPM__u32Task_ActiveIRQIdx = UINT32MAX;
  #if (0 != ITPM_nboIRQTiming)
    for (u32Cnt = 0; u32Cnt < ITPM_nNoOfInterrupts; u32Cnt++)
    {
      /*Call Counter*/
      ITPM__stInts.stInt[u32Cnt].stCnt.u32Start = 0;
      ITPM__stInts.stInt[u32Cnt].stCnt.u32Stop = 0;

      /*Call TimeStamp*/
      ITPM__stInts.stInt[u32Cnt].stTSmp.u32Start = 0;
      ITPM__stInts.stInt[u32Cnt].stTSmp.u32StartLast = 0;
      ITPM__stInts.stInt[u32Cnt].stTSmp.u32Stop = 0;

      /*Cycle Time */
      ITPM__stInts.stInt[u32Cnt].stCyc.u32Min = UINT32MAX;
      ITPM__stInts.stInt[u32Cnt].stCyc.u32Max = 0;

      /*Runtime*/
      ITPM__stInts.stInt[u32Cnt].stRT.u32Min = UINT32MAX;
      ITPM__stInts.stInt[u32Cnt].stRT.u32Max = 0;
      ITPM__stInts.stInt[u32Cnt].stRT.u32All = 0;
    }
    /* Call Depth */
    ITPM__stInts.stCall.u32Depth    = 0;
    ITPM__stInts.stCall.u32DepthMax = 0;
  #endif

  #if (0 != ITPM_nboIRQLockTiming)
    ITPM__stIRQLock.stCnt.u32Start  = 0;

    ITPM__stIRQLock.stTSmp.u32Start = 0;
    ITPM__stIRQLock.stTSmp.u32Stop  = 0;

    ITPM__stIRQLock.stRT.u32All      = 0;
    ITPM__stIRQLock.stRT.u32Min      = UINT32MAX;
    ITPM__stIRQLock.stRT.u32Max      = 0;
  #endif

  #if (0 != ITPM_nboScheduleTiming)
    ITPM__stScheduler.stCnt.u32Start  = 0;

    ITPM__stScheduler.stTSmp.u32Start = 0;
    ITPM__stScheduler.stTSmp.u32Stop  = 0;

    ITPM__stScheduler.stRT.u32All      = 0;
    ITPM__stScheduler.stRT.u32Min      = UINT32MAX;
    ITPM__stScheduler.stRT.u32Max      = 0;

    ITPM__stScheduler.stRT.u32AllBreak = 0;
    ITPM__stScheduler.stRT.u32MinBreak = UINT32MAX;
    ITPM__stScheduler.stRT.u32MaxBreak = 0;

    #if (0 != ITPM_nboScheduleHighScore)
    {
      uint8 u8Counter;

      ITPM__stScheduler.stHighScore.u32Entries = 0;
      for (u8Counter = 0; u8Counter < ITPM__nSchedulerHighscoreCnt; u8Counter++)
      {
        ITPM__stScheduler.stHighScore.u32FctPtr[u8Counter] = 0;
        ITPM__stScheduler.stHighScore.u32FctRT[u8Counter]  = 0;
        ITPM__stScheduler.stHighScore.u32Task[u8Counter]   = UINT32MAX;
      }
    }
    #endif

    ITPM__stScheduler.u32Active = 0;
  #endif

  ITPMLOG_vReset();
}


void ITPM_vReset (void)
{

  ITPM_nUserAction_Entry_ITPM_Reset(0);

  ITPM_u32MeasuringDelay   = ITPM_MEASUREMENT_DELAY; /* This is given in seconds */
  ITPM_u32MeasuringTimeCnt = ITPM_MEASUREMENT_TIME;  /* This is given in seconds */

  if (ITPM_u32MeasuringDelay > 0)
  {
      ITPM__boLogEnable = False;
  }
  else
  {
      ITPM__boLogEnable = True;
  }

  ITPM__vClearValues();

  ITPM_nUserAction_Exit_ITPM_Reset(0);
}


void ITPM_vWaitCycles(uint32 lui32Ticks)
{
  // CYCLES_PER_LOOP 4
  uint32 l = lui32Ticks/4;
  asm volatile( "0:" "SUBS %[count], 1;" "BNE 0b;" :[count]"+r"(l) );
}


void ITPM_vInitSysTimer(void)
{
  ITPM_vMeasurementTimer_Reset();
  ITPM_u32FrtTest_Start = ITPM_u32GetTimeStamp();
}


void Itpm_vTsArraySet(ITPM_tenTsArray lenIndex, ITPM_tenTsArray lenIndexRef)
{
  if (lenIndex != lenIndexRef)
  {
    ITMP_nTsArray[lenIndex] = ITPM_u32GetTimeStamp() - ITMP_nTsArray[lenIndexRef];
  }
  else
  {
    ITMP_nTsArray[lenIndex] = ITPM_u32GetTimeStamp();
  }
}


void ITPM_vInitCycCounter(void)
{
  ITPM_InitCycleCounter();
  ITPM_ResetCycleCounter();
  ITPM_EnableCycleCounter();
}


void Itpm_vCycArraySet(ITPM_tenTsArray lenIndex, ITPM_tenTsArray lenIndexRef)
{
  if (lenIndex != lenIndexRef)
  {
    ITMP_nTsArray[lenIndex] = ITPM_GetCycleCounter() - ITMP_nTsArray[lenIndexRef];
  }
  else
  {
    ITMP_nTsArray[lenIndex] = ITPM_GetCycleCounter();
  }
}




void ITPM_vStopMeasureCont(ITPM_tenMeasurePoints enMeasurePoint)
{
  #if (0 != ITPM_nboResoursekTiming)
  if (!ITPM__stMesPnts.stMesPnt[(uint8)enMeasurePoint].stTSmp.u32Stop)
  {
    /* store actual timestamp in storage array */
    ITPM__stMesPnts.stMesPnt[(uint8)enMeasurePoint].stTSmp.u32Stop = ITPM_u32GetTimeStamp();
  }
  #endif
}

void ITPM_vStartMeasureCont(ITPM_tenMeasurePoints enMeasurePoint)
{
  #if (0 != ITPM_nboResoursekTiming)
  /* set global index to actual measure point */
  if (!ITPM__stMesPnts.stMesPnt[(uint8)enMeasurePoint].stTSmp.u32Start)
  {
    /* store actual timestamp in storage array */
    ITPM__stMesPnts.stMesPnt[(uint8)enMeasurePoint].stTSmp.u32Start = ITPM_u32GetTimeStamp();
  }
  #endif
}


void ITPM_vStopMeasureRes(ITPM_tenMeasurePoints enMeasurePoint)
{
  ITPM_nUserAction_Entry_Res_Stop(enMeasurePoint);
  if (enMeasurePoint < ITPM_nLastMeasureIndex)
  {
    #if (ITPM_nboLogEvents_Res == 1)
      ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Res_DeinitEntry, enMeasurePoint);
    #endif

    ITPM_vStopMeasureCont(enMeasurePoint);
  }
  ITPM_nUserAction_Exit_Res_Stop(enMeasurePoint);
}

void ITPM_vStartMeasureRes(ITPM_tenMeasurePoints enMeasurePoint)
{
  ITPM_nUserAction_Entry_Res_Start(enMeasurePoint);
  if (enMeasurePoint < ITPM_nLastMeasureIndex)
  {
    #if (ITPM_nboLogEvents_Res == 1)
      ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Res_InitEntry, enMeasurePoint);
    #endif

    ITPM_vStartMeasureCont(enMeasurePoint);
  }
  ITPM_nUserAction_Exit_Res_Start(enMeasurePoint);
}


void ITPM_vStopMeasureSystem(ITPM_tenMeasurePoints enMeasurePoint)
{
  ITPM_nUserAction_Entry_System_Stop(enMeasurePoint);
  if (enMeasurePoint < ITPM_nLastMeasureIndex)
  {
    #if (ITPM_nboLogEvents_System == 1)
      ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_SystemExit, enMeasurePoint);
    #endif

    ITPM_vStopMeasureCont(enMeasurePoint);
  }
  ITPM_nUserAction_Exit_System_Stop(enMeasurePoint);
}

void ITPM_vStartMeasureSystem(ITPM_tenMeasurePoints enMeasurePoint)
{
  ITPM_nUserAction_Entry_System_Start(enMeasurePoint);
  if (enMeasurePoint < ITPM_nLastMeasureIndex)
  {
    #if (ITPM_nboLogEvents_System == 1)
      ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_SystemEntry, enMeasurePoint);
    #endif

    ITPM_vStartMeasureCont(enMeasurePoint);
  }
  ITPM_nUserAction_Exit_System_Start(enMeasurePoint);
}


void ITPM_vStopMeasurePower(ITPM_tenMeasurePoints enMeasurePoint)
{
  ITPM_nUserAction_Entry_Power_Stop(enMeasurePoint);
  if (enMeasurePoint < ITPM_nLastMeasureIndex)
  {
    #if (ITPM_nboLogEvents_Power == 1)
      ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_PowerStateExit, enMeasurePoint);
    #endif

    ITPM_vStopMeasureCont(enMeasurePoint);
  }
  ITPM_nUserAction_Exit_Power_Stop(enMeasurePoint);
}

void ITPM_vStartMeasurePower(ITPM_tenMeasurePoints enMeasurePoint)
{
  ITPM_nUserAction_Entry_Power_Start(enMeasurePoint);
  if (enMeasurePoint < ITPM_nLastMeasureIndex)
  {
    #if (ITPM_nboLogEvents_Power == 1)
      ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_PowerStateEntry, enMeasurePoint);
    #endif

    ITPM_vStartMeasureCont(enMeasurePoint);
  }
  ITPM_nUserAction_Exit_Power_Start(enMeasurePoint);
}


void ITPM_vRes_Set(uint8 u8ResIndex)
{
  ITPM_nUserAction_Entry_Res_Set(u8ResIndex);
  #if (ITPM_nboLogEvents_ResReq == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Res_Set, u8ResIndex);
  #endif
  ITPM_nUserAction_Exit_Res_Set(u8ResIndex);
}


void ITPM_vRes_Clear(uint8 u8ResIndex)
{
  ITPM_nUserAction_Entry_Res_Clear(u8ResIndex);
  #if (ITPM_nboLogEvents_ResReq == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Res_Clear, u8ResIndex);
  #endif
  ITPM_nUserAction_Exit_Res_Clear(u8ResIndex);
}



static void ITPM__vTask_StartTskCalculation(uint8 u8TaskIndex, ITPM_tenCalcTaskConsts enCondition)
{
  #if (0 != ITPM_nboTaskTiming)
  uint32         u32Temp;
  ITPM__tstTask* pstTask;

  pstTask = &(ITPM__stTasks.stTask[u8TaskIndex]);

  if (enCondition == ITPM_nenActivateTask)
  {

    if (!pstTask->stLat.u32StartCnt)
    {
      pstTask->stLat.u32StartCnt = 1;
    }

    if (pstTask->stCnt.u32Act > (pstTask->stCnt.u32Tte + 1))
    {
      pstTask->stLat.u32MultipleActCnt++;
    }

    pstTask->stRT.u32AtTtTemp = 0;
  }

  /* cyclic timing */
  if ((pstTask->stTSmp.u32ActTaskLast) && (enCondition == ITPM_nenActivateTask))
  {
    u32Temp = pstTask->stTSmp.u32ActTask - pstTask->stTSmp.u32ActTaskLast;

    if (u32Temp > pstTask->stCyc.u32Max)  pstTask->stCyc.u32Max = u32Temp;
    if (u32Temp < pstTask->stCyc.u32Min)  pstTask->stCyc.u32Min = u32Temp;
  }

  /* latency timing */
  if ((pstTask->stLat.u32StartCnt) && (enCondition == ITPM_nenPreTaskHook))
  {
    /* task was activated before, calculate the latency to this PreTaskHook (ready -> running state) */
    u32Temp = pstTask->stTSmp.u32PreTask - pstTask->stTSmp.u32ActTask;

    if (u32Temp > pstTask->stLat.u32Max)  pstTask->stLat.u32Max = u32Temp;
    if (u32Temp < pstTask->stLat.u32Min)  pstTask->stLat.u32Min = u32Temp;

    /* cumulate latency time for statistics */
    pstTask->stLat.u32Total += u32Temp;

    /* reinitialize this variable so multiple interruptions on this task is not counting as latency */
    pstTask->stLat.u32StartCnt = 0;
  }
  #endif
}


static void ITPM__vTask_StopTskCalculation(uint8 u8TaskIndex, ITPM_tenCalcTaskConsts enCondition)
{
  #if (0 != ITPM_nboTaskTiming)
  uint32 u32Temp;
  ITPM__tstTask* pstTask;

  pstTask = &(ITPM__stTasks.stTask[u8TaskIndex]);

  /* calculate RT */
  if ((pstTask->stCnt.u32Post >= pstTask->stCnt.u32Pre) && (pstTask->stCnt.u32Pre > 0))
  {
    if (enCondition == ITPM_nenPostTaskHook)
    {
      #if (0 != ITPM_nboTaskTimingNoIrq)
        u32Temp = (pstTask->stTSmp.u32PostTask - pstTask->stTSmp.u32PreTask) - ITPM__u32IRQ_Runtime;
      #else
        u32Temp = (pstTask->stTSmp.u32PostTask - pstTask->stTSmp.u32PreTask);
      #endif // (0 != ITPM_nboTaskTimingNoIrq)

      /* cumulate runtime for statistics */
      pstTask->stRT.u32All       += u32Temp;
      pstTask->stRT.u32AtTtTemp  += u32Temp;

      //if (/*(u8TaskIndex == 18)*/ (ITPM__boLogEnable) && (u32Temp > ITPM_xUSecToValue(4000)))
      //{
      //  static uint8 u8Test = 0;
      //
      //  if (!u8Test)
      //  {
      //    ITPMLOG_vEvent_SetCountdown(50);
      //    u8Test = 1;
      //  }
      //}

      if (u32Temp > pstTask->stRT.u32PrePostMax)  pstTask->stRT.u32PrePostMax = u32Temp;
      if (u32Temp < pstTask->stRT.u32PrePostMin)  pstTask->stRT.u32PrePostMin = u32Temp;
    }
  }

  if (enCondition == ITPM_nenTerminateTask)
  {
    if (pstTask->stRT.u32AtTtTemp > pstTask->stRT.u32AtTtMax)  pstTask->stRT.u32AtTtMax = pstTask->stRT.u32AtTtTemp;
    if (pstTask->stRT.u32AtTtTemp < pstTask->stRT.u32AtTtMin)  pstTask->stRT.u32AtTtMin = pstTask->stRT.u32AtTtTemp;
  }

  #endif
}


void ITPM_vTask_ActivateTask_Entry(uint8 u8TaskIndex)
{
  ITPM_nUserAction_Entry_Task_AT_Entry(u8TaskIndex);
  #if (ITPM_nboLogEvents_Task_AT_Entry == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_ActivateEntry, u8TaskIndex);
  #endif

  #if (0 != ITPM_nboTaskTiming)
    if (u8TaskIndex < ITPM_nNoOfTasks)
    {
      //Increase calling counter
      ITPM__stTasks.stTask[u8TaskIndex].stCnt.u32Act++;

      // Save Timestamp of the last call, e.g. can be used to calculate the cyclic time
      ITPM__stTasks.stTask[u8TaskIndex].stTSmp.u32ActTaskLast = ITPM__stTasks.stTask[u8TaskIndex].stTSmp.u32ActTask;
      ITPM__stTasks.stTask[u8TaskIndex].stTSmp.u32ActTask = ITPM_u32GetTimeStamp();

      ITPM__vTask_StartTskCalculation(u8TaskIndex, ITPM_nenActivateTask);
    }
  #endif
  ITPM_nUserAction_Exit_Task_AT_Entry(u8TaskIndex);
}

void ITPM_vTask_ActivateTask_Exit(uint8 u8TaskIndex)
{
  ITPM_nUserAction_Entry_Task_AT_Exit(u8TaskIndex);
  #if (ITPM_nboLogEvents_Task_AT_Exit == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_ActivateExit, u8TaskIndex);
  #endif
  ITPM_nUserAction_Exit_Task_AT_Exit(u8TaskIndex);
}

void ITPM_vTask_TerminateTask_Entry(uint8 u8TaskIndex)
{
  ITPM_nUserAction_Entry_Task_TT_Entry(u8TaskIndex);
  #if (ITPM_nboLogEvents_Task_TT_Entry == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_TerminateEntry, u8TaskIndex);
  #endif
  ITPM_nUserAction_Exit_Task_TT_Entry(u8TaskIndex);
}

void ITPM_vTask_TerminateTask_Exit(uint8 u8TaskIndex)
{
  ITPM_nUserAction_Entry_Task_TT_Exit(u8TaskIndex);

  #if (ITPM_nboLogEvents_Task_TT_Exit == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_TerminateExit, u8TaskIndex);
  #endif

  #if (0 != ITPM_nboTaskTiming)
    if (u8TaskIndex < ITPM_nNoOfTasks)
    {
      ITPM__stTasks.stTask[u8TaskIndex].stCnt.u32Tte++;
      ITPM__vTask_StopTskCalculation(u8TaskIndex, ITPM_nenTerminateTask);
    }
  #endif
  ITPM_nUserAction_Exit_Task_TT_Exit(u8TaskIndex);
}


void ITPM_vTask_Containter_Entry(uint32 u32FctPointer)
{
  // Avoid change of 'ITPM__u32Task_ActiveTaskIdx' during measuring
  #if (ITPM_nboTaskRTContainer == 1)
    uint32 u32Task_ActiveTaskIdx;
  #endif

  ITPM_nUserAction_Entry_Task_Cont_Entry(u32FctPointer);

  // Avoid CAM_1ms Interrupt
  if (ITPM__u32Task_ActiveIRQIdx == UINT32MAX)
  {
    #if (ITPM_nboTaskRTContainer == 1)
      u32Task_ActiveTaskIdx       = ITPM__u32Task_ActiveTaskIdx;
    #endif
    ITPM__u32Task_ActiveContIdx = u32FctPointer;
  }
  else
  {
    #if (ITPM_nboTaskRTContainer == 1)
      u32Task_ActiveTaskIdx = UINT32MAX;
    #endif
  }


  #if (ITPM_nboLogEvents_Task_Cont_Entry == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_ContainerEntry, u32FctPointer);
  #endif

  #if (ITPM_nboTaskRTContainer == 1)
    if (u32Task_ActiveTaskIdx < ITPM_nNoOfTasks)
    {
      ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32LastContIdx   = u32FctPointer;
      ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32LastContStart = ITPM_u32GetTimeStamp();
    }
  #endif
  ITPM_nUserAction_Exit_Task_Cont_Entry(u32FctPointer);
}

void ITPM_vTask_Container_Exit(uint32 u32FctPointer)
{
  #if (ITPM_nboTaskRTContainer == 1)
    // Avoid change of 'ITPM__u32Task_ActiveTaskIdx' during measuring
    uint32 u32Task_ActiveTaskIdx;
    // Avoid CAM_1ms Interrupt
    if (ITPM__u32Task_ActiveIRQIdx == UINT32MAX) u32Task_ActiveTaskIdx = ITPM__u32Task_ActiveTaskIdx;
                                             else u32Task_ActiveTaskIdx = UINT32MAX;
  #endif

  if (ITPM__u32Task_ActiveIRQIdx == UINT32MAX)
  {
    ITPM__u32Task_ActiveContIdx = UINT32MAX;
  }

  ITPM_nUserAction_Entry_Task_Cont_Exit(u32FctPointer);

  #if (ITPM_nboLogEvents_Task_Cont_Exit == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_ContainerExit, u32FctPointer);
  #endif

  #if (ITPM_nboTaskRTContainer == 1)
    if (u32Task_ActiveTaskIdx < ITPM_nNoOfTasks)
    {
      if (ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32LastContIdx == u32FctPointer)
      {
        if (ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32LastContStart > 0)
        {
          uint32 u32Temp;

          u32Temp = ITPM_u32GetTimeStamp() - ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32LastContStart;

          if (u32Temp > ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32MaxRT)
          {
            ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32MaxRT     = u32Temp;
            ITPM__stTasks.stTask[u32Task_ActiveTaskIdx].stCont.u32MaxFctPtr = u32FctPointer;
          }
        }
      }
    }
  #endif
  ITPM_nUserAction_Exit_Task_Cont_Exit(u32FctPointer);
}


void ITPM_vTask_PreTaskHook(uint8 u8TaskIndex)
{
  ITPM_nUserAction_Entry_Task_PreHook(u8TaskIndex);

  ITPM__u32Task_ActiveTaskIdx = u8TaskIndex;

  #if (ITPM_nboLogEvents_Task_PreHook == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_PreHook, u8TaskIndex);
  #endif

  #if (0 != ITPM_nboTaskTimingNoIrq)
    ITPM__u32IRQ_Runtime       = 0;
  #endif

  #if (0 != ITPM_nboScheduleTiming)
    if (oskTcbTaskPreemption[u8TaskIndex] == osdTaskTypeNonPreempt)
    {
      ITPM__stScheduler.u32Active = 1;
      ITPM__stScheduler.stTSmp.u32Stop = 0;
    }
  #endif

  #if (0 != ITPM_nboTaskTiming)
    if (u8TaskIndex < ITPM_nNoOfTasks)
    {
      ITPM__stTasks.stTask[u8TaskIndex].stCnt.u32Pre++;
      ITPM__stTasks.stTask[u8TaskIndex].stTSmp.u32PreTask = ITPM_u32GetTimeStamp();

      ITPM__vTask_StartTskCalculation(u8TaskIndex, ITPM_nenPreTaskHook);
    }
  #endif

  ITPM_nUserAction_Exit_Task_PreHook(u8TaskIndex);
}

void ITPM_vTask_PostTaskHook(uint8 u8TaskIndex)
{
  ITPM_nUserAction_Entry_Task_PostHook(u8TaskIndex);
  ITPM__u32Task_ActiveTaskIdx = UINT32MAX;

  #if (ITPM_nboLogEvents_Task_PostHook == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Task_PostHook, u8TaskIndex);
  #endif

  #if (0 != ITPM_nboScheduleTiming)
    ITPM__stScheduler.u32Active = 0;
  #endif


  #if (0 != ITPM_nboTaskTiming)
    if (u8TaskIndex < ITPM_nNoOfTasks)
    {
      ITPM__stTasks.stTask[u8TaskIndex].stCnt.u32Post++;
      ITPM__stTasks.stTask[u8TaskIndex].stTSmp.u32PostTask = ITPM_u32GetTimeStamp();

      ITPM__vTask_StopTskCalculation(u8TaskIndex, ITPM_nenPostTaskHook);
    }
  #endif
  ITPM_nUserAction_Exit_Task_PostHook(u8TaskIndex);
}

#if (0 != ITPM_nboScheduleTiming)
#if (0 != ITPM_nboScheduleHighScore)
  void ITPM_vSchedule__HighScore_Add(uint32 u32FctPtr, uint32 u32TaskIdx, uint32 u32FctRuntime)
  {
    uint8 u8Counter;
    uint8 u8SearchIdx;
    uint8  u8SearchIdxSmallestEntry;
    uint32 u32SearchValueSmallestEntry;

    u8SearchIdx = 0xFF;
    u8SearchIdxSmallestEntry = 0xFF;
    u32SearchValueSmallestEntry = UINT32MAX;

    //Check if Function is already in
    for (u8Counter = 0; u8Counter < ITPM__stScheduler.stHighScore.u32Entries; u8Counter++)
    {
      if (ITPM__stScheduler.stHighScore.u32FctPtr[u8Counter] == u32FctPtr)
      {
        u8SearchIdx = u8Counter;
      }

      if (u32SearchValueSmallestEntry > ITPM__stScheduler.stHighScore.u32FctRT[u8Counter])
      {
        u32SearchValueSmallestEntry = ITPM__stScheduler.stHighScore.u32FctRT[u8Counter];
        u8SearchIdxSmallestEntry    = u8Counter;
      }
    }

    //Found ?
    if (u8SearchIdx < ITPM__stScheduler.stHighScore.u32Entries)
    {
      //Check for runtime
      if (ITPM__stScheduler.stHighScore.u32FctRT[u8SearchIdx] < u32FctRuntime)
      {
        ITPM__stScheduler.stHighScore.u32FctRT[u8SearchIdx] = u32FctRuntime;
      }
    }
    else
    {
      // Is there a free slot in Highscore
      if (ITPM__stScheduler.stHighScore.u32Entries < ITPM__nSchedulerHighscoreCnt)
      {
        u8Counter = ITPM__stScheduler.stHighScore.u32Entries - 1;
        ITPM__stScheduler.stHighScore.u32Entries++;
        ITPM__stScheduler.stHighScore.u32FctPtr[u8Counter] = u32FctPtr;
        ITPM__stScheduler.stHighScore.u32FctRT[u8Counter]  = u32FctRuntime;
        ITPM__stScheduler.stHighScore.u32Task[u8Counter]   = u32TaskIdx;
      }
      else
      {
        //Hihgscore is alreay full, overwrite the smallest runtime
        if (u32SearchValueSmallestEntry < u32FctRuntime)
        {
          ITPM__stScheduler.stHighScore.u32FctPtr[u8SearchIdxSmallestEntry] = u32FctPtr;
          ITPM__stScheduler.stHighScore.u32FctRT[u8SearchIdxSmallestEntry]  = u32FctRuntime;
          ITPM__stScheduler.stHighScore.u32Task[u8SearchIdxSmallestEntry]   = u32TaskIdx;
        }
      }
    }
  }
#endif
#endif


void ITPM_vSchedule_Entry(void)
{
  #if (0 != ITPM_nboScheduleTiming)
    uint32 u32Temp;
    uint32 u32Task_ActiveTaskIdx;
    uint32 u32Task_ActiveContIdx;
  #endif

  ITPM_nUserAction_Entry_Scheduler_Entry(0);

  #if (0 != ITPM_nboScheduleTiming)

    u32Task_ActiveContIdx = ITPM__u32Task_ActiveContIdx;
    if (ITPM__u32Task_ActiveIRQIdx == UINT32MAX) u32Task_ActiveTaskIdx = ITPM__u32Task_ActiveTaskIdx;
                                             else u32Task_ActiveTaskIdx = UINT32MAX;
  #endif

  #if (ITPM_nboLogEvents_Scheduler == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Scheduler_Entry, 0);
  #endif


  #if (0 != ITPM_nboScheduleTiming)

    if (ITPM__stScheduler.u32Active)
    {
      ITPM__stScheduler.stCnt.u32Start++;
      ITPM__stScheduler.stTSmp.u32Start = ITPM_u32GetTimeStamp();

      if (ITPM__stScheduler.stTSmp.u32Stop > 0)
      {
        u32Temp = ITPM__stScheduler.stTSmp.u32Start - ITPM__stScheduler.stTSmp.u32Stop;

        ITPM__stScheduler.stRT.u32AllBreak += u32Temp;
        if (u32Temp < ITPM__stScheduler.stRT.u32MinBreak)  ITPM__stScheduler.stRT.u32MinBreak = u32Temp;
        if (u32Temp > ITPM__stScheduler.stRT.u32MaxBreak)  ITPM__stScheduler.stRT.u32MaxBreak = u32Temp;

        #if (0 != ITPM_nboScheduleHighScore)
          if (u32Temp > ITPM__u32MaxTimeBetweenSchedule)
          {
            /* For Test */
            if (u32Task_ActiveContIdx != 0xFFFFFFFF)
            {
              ITPM_vSchedule__HighScore_Add(u32Task_ActiveContIdx, u32Task_ActiveTaskIdx, u32Temp);
            }
          }
        #endif
      }
    }
  #endif

  ITPM_nUserAction_Exit_Scheduler_Entry(0);
}


void ITPM_vSchedule_Exit(void)
{
  #if (0 != ITPM_nboScheduleTiming)
    uint32 u32Temp;
  #endif

  ITPM_nUserAction_Entry_Scheduler_Exit(0);

  #if (ITPM_nboLogEvents_Scheduler == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Scheduler_Exit, 0);
  #endif

  #if (0 != ITPM_nboScheduleTiming)

    if (ITPM__stScheduler.u32Active)
    {
      ITPM__stScheduler.stTSmp.u32Stop      = ITPM_u32GetTimeStamp();

      u32Temp = ITPM__stScheduler.stTSmp.u32Stop - ITPM__stScheduler.stTSmp.u32Start;

      ITPM__stScheduler.stRT.u32All += u32Temp;

      if (u32Temp > ITPM__stScheduler.stRT.u32Max)  ITPM__stScheduler.stRT.u32Max = u32Temp;
      if (u32Temp < ITPM__stScheduler.stRT.u32Min)  ITPM__stScheduler.stRT.u32Min = u32Temp;
    }
  #endif

  ITPM_nUserAction_Exit_Scheduler_Exit(0);
}


void ITPM_vIRQ_Entry(uint8 u8IRQIndex)
{
  #if (0 != ITPM_nboIRQTiming)
    uint32        u32Temp;
    ITPM__tstInt* pstInt;
  #endif

  ITPM_nUserAction_Entry_IRQs_Entry(u8IRQIndex);

  ITPM__u32Task_ActiveIRQIdx = u8IRQIndex;

  #if (ITPM_nboLogEvents_IRQs == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Int_Entry, u8IRQIndex);
  #endif


  #if (0 != ITPM_nboIRQTiming)
    ITPM__stInts.stCall.u32Depth++;
    if (ITPM__stInts.stCall.u32Depth > ITPM__stInts.stCall.u32DepthMax)
    {
      ITPM__stInts.stCall.u32DepthMax = ITPM__stInts.stCall.u32Depth;
    }

    if (ITPM__stInts.stCall.u32Depth == 2)
    {
      //ITPMLOG_vEvent_SetCountdown(100);
      ITPM__stInts.stCall.u32DepthMax = ITPM__stInts.stCall.u32Depth;
    }

    if (u8IRQIndex < ITPM_nNoOfInterrupts)
    {
      pstInt = &(ITPM__stInts.stInt[u8IRQIndex]);

      pstInt->stCnt.u32Start++;

      pstInt->stTSmp.u32StartLast = pstInt->stTSmp.u32Start;
      pstInt->stTSmp.u32Start = ITPM_u32GetTimeStamp();


      if (pstInt->stTSmp.u32StartLast)
      {
        u32Temp = pstInt->stTSmp.u32Start - pstInt->stTSmp.u32StartLast;

        if (u32Temp > pstInt->stCyc.u32Max)  pstInt->stCyc.u32Max = u32Temp;
        if (u32Temp < pstInt->stCyc.u32Min)  pstInt->stCyc.u32Min = u32Temp;
      }
    }
  #endif


  ITPM_nUserAction_Exit_IRQs_Entry(u8IRQIndex);
}


void ITPM_vIRQ_Exit(uint8 u8IRQIndex)
{
  #if (0 != ITPM_nboIRQTiming)
    uint32        u32Temp;
    ITPM__tstInt* pstInt;
  #endif

  ITPM_nUserAction_Entry_IRQs_Exit(u8IRQIndex);

  ITPM__u32Task_ActiveIRQIdx = UINT32MAX;

  #if (ITPM_nboLogEvents_IRQs == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_Int_Exit, u8IRQIndex);
  #endif

  #if (0 != ITPM_nboIRQTiming)
    ITPM__stInts.stCall.u32Depth--;

    if (u8IRQIndex < ITPM_nNoOfInterrupts)
    {
      pstInt = &(ITPM__stInts.stInt[u8IRQIndex]);

      if (pstInt->stCnt.u32Stop < pstInt->stCnt.u32Start)
      {
        pstInt->stCnt.u32Stop++;
        pstInt->stTSmp.u32Stop = ITPM_u32GetTimeStamp();

        u32Temp = pstInt->stTSmp.u32Stop - pstInt->stTSmp.u32Start;

        #if (0 != ITPM_nboTaskTimingNoIrq)
          //avoid to sum up nested interrupts
          if (ITPM__stInts.stCall.u32Depth == 0)
          {
            ITPM__u32IRQ_Runtime+= u32Temp;
          }
        #endif

        pstInt->stRT.u32All += u32Temp;

        if (u32Temp > pstInt->stRT.u32Max)  pstInt->stRT.u32Max = u32Temp;
        if (u32Temp < pstInt->stRT.u32Min)  pstInt->stRT.u32Min = u32Temp;
      }
    }
  #endif
  ITPM_nUserAction_Exit_IRQs_Exit(u8IRQIndex);
}


void ITPM_vIRQLock_Entry(void)
{
  #if (ITPM_nboLogEvents_IRQLocks == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_IntLock_Entry, 0);
  #endif

  ITPM_nUserAction_Entry_IRQLocks_Entry(0);

  #if (0 != ITPM_nboIRQLockTiming)
    ITPM__stIRQLock.stCnt.u32Start++;
    ITPM__stIRQLock.stTSmp.u32Start = ITPM_u32GetTimeStamp();
  #endif
  ITPM_nUserAction_Entry_IRQLocks_Entry(0);
}

void ITPM_vIRQLock_Exit(void)
{
  #if (0 != ITPM_nboIRQLockTiming)
    uint32 u32Temp;
  #endif

  ITPM_nUserAction_Entry_IRQLocks_Exit(0);

  #if (ITPM_nboLogEvents_IRQLocks == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_IntLock_Exit, 0);
  #endif

  #if (0 != ITPM_nboIRQLockTiming)

    ITPM__stIRQLock.stTSmp.u32Stop  = ITPM_u32GetTimeStamp();

    u32Temp = ITPM__stIRQLock.stTSmp.u32Stop - ITPM__stIRQLock.stTSmp.u32Start;

    ITPM__stIRQLock.stRT.u32All += u32Temp;

    if (u32Temp > ITPM__stIRQLock.stRT.u32Max)  ITPM__stIRQLock.stRT.u32Max = u32Temp;
    if (u32Temp < ITPM__stIRQLock.stRT.u32Min)  ITPM__stIRQLock.stRT.u32Min = u32Temp;

  #endif
  ITPM_nUserAction_Exit_IRQLocks_Exit(0);
}

void ITPM_vUser_Log(uint32 u32AnyNumber)
{
  ITPM_nUserAction_Entry_User(u32AnyNumber);

  #if (ITPM_nboLogEvents_User == 1)
    ITPMLOG_vEvent_Add(ITPMLOG_nenEvt_User, u32AnyNumber);
  #endif

  ITPM_nUserAction_Exit_User(u32AnyNumber);
}


void ITPM_vCalculation_Tsk(void)
{
  ITPM_u32FrtTest = ITPM_u32GetTimeStamp();

  if (ITPM_u32MeasuringDelay > 0)
  {
      if (((ITPM_u32FrtTest-ITPM_u32FrtTest_Start) >= (ITPM_u32MeasuringDelay * ITPM_nSecond)) && (!ITPM__boLogEnable))
      {
        ITPM_vDisableAndSave_Int();
        ITPM__vClearValues();
        ITPM__boLogEnable = True;

        #if (0 != ITPM_nboTaskTiming)
          //ITPM__stTasks.stTask[IDLE_vLoop_Tsk].stCnt.u32Pre++;
          //ITPM__stTasks.stTask[IDLE_vLoop_Tsk].stTSmp.u32PreTask = ITPM_u32GetTimeStamp();
        #endif

        ITPM_vRestore_Int();
      }
  }

  if (((ITPM_u32FrtTest-ITPM_u32FrtTest_Start) >= (ITPM_u32MeasuringTimeCnt * ITPM_nSecond))  && (ITPM__boLogEnable))
  {
    ITPM__boLogEnable = false;
  }
}



#endif
