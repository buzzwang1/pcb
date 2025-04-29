
#include "CycleCaller.h"


static uint8 CycCall_mui8_Call1ms;

// High Priority Callbacks
CycCall_Callback gptrCycCall_Callback_All_1ms_HP;

// Low Priority Callbacks
CycCall_Callback gptrCycCall_Callback_All_1ms_LP;

u32 SysTime_u32Time_us;
u32 SysTime_u32TimeMs_us;
u16 SysTime_u16TimeOld_us;
u32 SysTime_u32Time_ms;

void SysTime_vInit(void)
{
  SysTime_u32Time_us    = 0;
  SysTime_u32TimeMs_us  = 0;
  SysTime_u16TimeOld_us = 0;
  SysTime_u32Time_ms    = 0;
}

u32  SysTime_u32GetDiffTime(u32& lu32TimeLast, u32 lu32TimeNew)
{
  u32 lu32DiffTime;

  if (lu32TimeNew > lu32TimeLast)
  {
    lu32DiffTime = lu32TimeNew - lu32TimeLast;
  }
  else
  {
    lu32DiffTime = lu32TimeNew + (U32MAX - lu32TimeLast);
  }

  lu32TimeLast = lu32TimeNew;
  return lu32DiffTime;
}


void SysTime_vUpdateTime()
{
  u16 lu16HwTime;
  u16 lu16DiffTime;

  _dai();
  lu16HwTime = CycCallHw_u16GetCnt();
  if (lu16HwTime > SysTime_u16TimeOld_us)
  {
    lu16DiffTime = lu16HwTime - SysTime_u16TimeOld_us;
  }
  else
  {
    // 1000 wegen Cycle-Time vom Timer
    lu16DiffTime = lu16HwTime + (1000 - SysTime_u16TimeOld_us);
  }
  SysTime_u16TimeOld_us = lu16HwTime;
  _eai();

  SysTime_u32Time_us   += lu16DiffTime;
  SysTime_u32TimeMs_us += lu16DiffTime;

  if (SysTime_u32TimeMs_us > 1000)
  {
    SysTime_u32TimeMs_us -= 1000;
    SysTime_u32Time_ms++;
  }
}


void SysTime_Delay_us(uint32 lui32Delay)
{
  u32 lu32Systime_Old;
  u32 lu32Systime_Diff;

  lu32Systime_Diff = 0;
  SysTime_vUpdateTime();
  lu32Systime_Old  = SysTime_u32Time_us;
  while (lui32Delay > lu32Systime_Diff)
  {
    lui32Delay -= lu32Systime_Diff;
    SysTime_vUpdateTime();
    lu32Systime_Diff = SysTime_u32GetDiffTime(lu32Systime_Old, SysTime_u32Time_us);
  }
}

void SysTime_Delay_ms(uint32 lui32Delay)
{
  u32 lu32Systime_Old;
  u32 lu32Systime_Diff;

  lu32Systime_Diff = 0;
  SysTime_vUpdateTime();
  lu32Systime_Old = SysTime_u32Time_ms;
  while (lui32Delay > lu32Systime_Diff)
  {
    lui32Delay -= lu32Systime_Diff;
    SysTime_vUpdateTime();
    lu32Systime_Diff = SysTime_u32GetDiffTime(lu32Systime_Old, SysTime_u32Time_ms);
  }
}

u32 SysTime_u32TimeGetTime_us(void) { return SysTime_u32Time_us; }
u32 SysTime_u32TimeGetTime_ms(void) { return SysTime_u32Time_ms; }


/* ----------------------- High Priority ----------------------*/

void CycCall_All_1ms_HP(void)
{
  CycCall_mui8_Call1ms   = 1;

  /* User Code Begin */

  if (gptrCycCall_Callback_All_1ms_HP != NULL)
  {
    gptrCycCall_Callback_All_1ms_HP();
  }

  /* User Code End */
}

/* -------------------- Low Priority ----------------------*/

void CycCall_All_1ms_LP(void)
{
  /* User Code Begin */

  if (gptrCycCall_Callback_All_1ms_LP != NULL)
  {
    gptrCycCall_Callback_All_1ms_LP();
  }

  /* User Code End */

  CycCall_mui8_Call1ms  = 0;
}


//Dieser Function sollte alle 1000us aufgerufen werden
void CycCall_vMain_Hp_1ms(void)
{
  SysTime_vUpdateTime();
  CycCall_All_1ms_HP();
}


void CycCall_Start(CycCall_Callback lptrCycCall_Callback_All_1ms_HP,
                   CycCall_Callback lptrCycCall_Callback_All_1ms_LP)
{
  CycCall_mui8_Call1ms   = 0;

  gptrCycCall_Callback_All_1ms_HP = lptrCycCall_Callback_All_1ms_HP;
  gptrCycCall_Callback_All_1ms_LP = lptrCycCall_Callback_All_1ms_LP;

  SysTime_vInit();
  CycCallHw_vInit();
}


void CycCall_vIdle()
{
  SysTime_vUpdateTime();
  if (CycCall_mui8_Call1ms)
  {
    CycCall_All_1ms_LP();
  }
}
