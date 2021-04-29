
#include "CycleCaller.h"


#if (CYCCALL_CNT == 1)
  volatile static uint8 CycCall_mui8_1ms;
  volatile static uint8 CycCall_mui8_10ms;
  volatile static uint8 CycCall_mui8_100ms;
  volatile static uint8 CycCall_mui8_1s;
#endif


volatile static uintx CycCall_muixWait_1ms;


volatile static uint8 CycCall_mui8_Call1ms;
volatile static uint8 CycCall_mui8_Call10ms;
volatile static uint8 CycCall_mui8_Call100ms;
volatile static uint8 CycCall_mui8_Call1s;


volatile static uintx  guixSystemTicks;


// High Priority Callbacks
volatile CycCall_Callback gptrCycCall_Callback_All_1ms_HP;
volatile CycCall_Callback gptrCycCall_Callback_All_10ms_HP;
volatile CycCall_Callback gptrCycCall_Callback_All_100ms_HP;
volatile CycCall_Callback gptrCycCall_Callback_All_1s_HP;

// Low Priority Callbacks
volatile CycCall_Callback gptrCycCall_Callback_All_1ms_LP;
volatile CycCall_Callback gptrCycCall_Callback_All_10ms_LP;
volatile CycCall_Callback gptrCycCall_Callback_All_100ms_LP;
volatile CycCall_Callback gptrCycCall_Callback_All_1s_LP;



CYCCALL_INLINE void CycCall_Waitms(uintx luixTime)
{
  CycCall_muixWait_1ms = luixTime;
  while (CycCall_muixWait_1ms) {};
}



/* ----------------------- High Priority ----------------------*/

CYCCALL_INLINE void  CycCall_All_1ms_HP(void)
{
  CycCall_mui8_Call1ms   = 1;

  /* User Code Begin */

  if (gptrCycCall_Callback_All_1ms_HP != NULL)
  {
    gptrCycCall_Callback_All_1ms_HP();
  }

  /* User Code End */
}

CYCCALL_INLINE void  CycCall_All_10ms_HP(void)
{
  CycCall_mui8_Call10ms  = 1;

  /* User Code Begin */

  if (gptrCycCall_Callback_All_10ms_HP != NULL)
  {
    gptrCycCall_Callback_All_10ms_HP();
  }

  /* User Code End */
}

CYCCALL_INLINE void  CycCall_All_100ms_HP(void)
{
  CycCall_mui8_Call100ms = 1;

  /* User Code Begin */

  if (gptrCycCall_Callback_All_100ms_HP != NULL)
  {
    gptrCycCall_Callback_All_100ms_HP();
  }

  /* User Code End */
}

CYCCALL_INLINE void  CycCall_All_1s_HP(void)
{
  CycCall_mui8_Call1s    = 1;

  /* User Code Begin */

  if (gptrCycCall_Callback_All_1s_HP != NULL)
  {
    gptrCycCall_Callback_All_1s_HP();
  }

  /* User Code End */
}


/* -------------------- Low Priority ----------------------*/

CYCCALL_INLINE void  CycCall_All_1ms_LP(void)
{
  /* User Code Begin */

  if (gptrCycCall_Callback_All_1ms_LP != NULL)
  {
    gptrCycCall_Callback_All_1ms_LP();
  }

  /* User Code End */

  CycCall_mui8_Call1ms  = 0;
}

CYCCALL_INLINE void  CycCall_All_10ms_LP(void)
{
  /* User Code Begin */

  if (gptrCycCall_Callback_All_10ms_LP != NULL)
  {
    gptrCycCall_Callback_All_10ms_LP();
  }


  /* User Code End */

  CycCall_mui8_Call10ms  = 0;
}

CYCCALL_INLINE void  CycCall_All_100ms_LP(void)
{
  /* User Code Begin */

  if (gptrCycCall_Callback_All_100ms_LP != NULL)
  {
    gptrCycCall_Callback_All_100ms_LP();
  }

  /* User Code End */

  CycCall_mui8_Call100ms = 0;
}

CYCCALL_INLINE void  CycCall_All_1s_LP(void)
{
  /* User Code Begin */

  if (gptrCycCall_Callback_All_1s_LP != NULL)
  {
    gptrCycCall_Callback_All_1s_LP();
  }

  /* User Code End */

  CycCall_mui8_Call1s    = 0;
}

CYCCALL_INLINE uintx CycCall_uixTimeGetTime(void)
{
  return guixSystemTicks;
}


//Dieser Function sollte alle 1000us aufgerufen werden
CYCCALL_INLINE void CycCall_vMain_1ms(void)
{
  static uint8 lui8_1ms_10 = 10;
  static uint8 lui8_10ms_10 = 10;
  static uint8 lui8_100ms_10 = 10;


  #if (CYCCALL_CNT == 1)
    CycCall_mui8_1ms++;
  #endif

  #ifdef CYCCALL_WAIT
    if (CycCall_muixWait_1ms) CycCall_muixWait_1ms--;
  #endif

  lui8_1ms_10--;
  guixSystemTicks++;

  CycCall_All_1ms_HP();

  if (lui8_1ms_10 == 0)
  {
    lui8_1ms_10 = 10;
    #if (CYCCALL_CNT == 1)
      CycCall_mui8_10ms++;
    #endif
    lui8_10ms_10--;

    CycCall_All_10ms_HP();

    if (lui8_10ms_10 == 0)
    {
      lui8_10ms_10 = 10;
      #if (CYCCALL_CNT == 1)
      CycCall_mui8_100ms++;
      #endif
      lui8_100ms_10--;

      CycCall_All_100ms_HP();

      if (lui8_100ms_10 == 0)
      {
        lui8_100ms_10  = 10;
        #if (CYCCALL_CNT == 1)
          CycCall_mui8_1s++;
        #endif
        CycCall_All_1s_HP();
      }
    }
  }
}




CYCCALL_INLINE void CycCall_Start(CycCall_Callback lptrCycCall_Callback_All_1ms_HP,
                                  CycCall_Callback lptrCycCall_Callback_All_10ms_HP,
                                  CycCall_Callback lptrCycCall_Callback_All_100ms_HP,
                                  CycCall_Callback lptrCycCall_Callback_All_1s_HP,

                                  CycCall_Callback lptrCycCall_Callback_All_1ms_LP,
                                  CycCall_Callback lptrCycCall_Callback_All_10ms_LP,
                                  CycCall_Callback lptrCycCall_Callback_All_100ms_LP,
                                  CycCall_Callback lptrCycCall_Callback_All_1s_LP)
{
  CycCall_mui8_Call1ms   = 0;
  CycCall_mui8_Call10ms  = 0;
  CycCall_mui8_Call100ms = 0;
  CycCall_mui8_Call1s    = 0;

  #if (CYCCALL_CNT == 1)
    CycCall_mui8_1ms   = 0;
    CycCall_mui8_10ms  = 0;
    CycCall_mui8_100ms = 0;
    CycCall_mui8_1s    = 0;
  #endif

  CycCall_muixWait_1ms = 0;

  guixSystemTicks = 0;

  gptrCycCall_Callback_All_1ms_HP   = lptrCycCall_Callback_All_1ms_HP;
  gptrCycCall_Callback_All_10ms_HP  = lptrCycCall_Callback_All_10ms_HP;
  gptrCycCall_Callback_All_100ms_HP = lptrCycCall_Callback_All_100ms_HP;
  gptrCycCall_Callback_All_1s_HP    = lptrCycCall_Callback_All_1s_HP;


  gptrCycCall_Callback_All_1ms_LP   = lptrCycCall_Callback_All_1ms_LP;
  gptrCycCall_Callback_All_10ms_LP  = lptrCycCall_Callback_All_10ms_LP;
  gptrCycCall_Callback_All_100ms_LP = lptrCycCall_Callback_All_100ms_LP;
  gptrCycCall_Callback_All_1s_LP    = lptrCycCall_Callback_All_1s_LP;

  CycCallHw_vInit(CycCall_vMain_1ms);
}


CYCCALL_INLINE void CycCall_vIdle(void)
{
  if (CycCall_mui8_Call1ms)   CycCall_All_1ms_LP();
  if (CycCall_mui8_Call10ms)  CycCall_All_10ms_LP();
  if (CycCall_mui8_Call100ms) CycCall_All_100ms_LP();
  if (CycCall_mui8_Call1s)    CycCall_All_1s_LP();
}
