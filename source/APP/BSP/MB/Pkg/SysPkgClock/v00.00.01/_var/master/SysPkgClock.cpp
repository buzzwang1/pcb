#include "SysPkgClock.h"

#include "System.h"


cSysPkgClock::cSysPkgClock()
{
};

void cSysPkgClock::vInit1(void)
{
  mu32ClockResyncTimeout_s = 0;
  mcSys.mcCom.mcSideLink.vKeepReceiverOnWhileWaiting(False);
}

void cSysPkgClock::vInit2(void)
{
  if (mcSys.mcCom.mcSideLink.mStatus.IsInit)  
  {
    // Funk-Empfänger vorhanden
    if (mClock.bResync())
    {
      mu32ClockResyncTimeout_s = 3 * 60;
      mcSys.mcCom.mcSideLink.vKeepReceiverOnWhileWaiting(True);
      mClock.vSetSyncAttempt();
    }
  }
}

bool cSysPkgClock::isReadyForSleep(cStr& lcStatus)
{
  bool lbRet = True;
  cStr_Create(lszStrBuf, 32);

  // Warten bis Uhrzeit empfangen wurde
  if (mu32ClockResyncTimeout_s > 0)
  {
    lbRet = False;
    lszStrBuf.Setf((rsz)"ClkSync-TO: %d", mu32ClockResyncTimeout_s);
    if (lcStatus.Len() > 0) lcStatus += (rsz)", ";
    lcStatus += lszStrBuf;
  }

  return lbRet;
}

void cSysPkgClock::vTick1msHp(void)
{
}

void cSysPkgClock::vTick1msLp(void)
{
}

void cSysPkgClock::vTick10msLp(void)
{
}

void cSysPkgClock::vTick100msLp(void)
{
}

void cSysPkgClock::vTick1000msLp(void)
{
  mClock.vAdd1s();

  if (mcSys.mcCom.mcSideLink.mStatus.IsInit)  
  {
    // Funk-Empfänger vorhanden

    if (mClock.bResync())
    {
      mu32ClockResyncTimeout_s = 3 * 60;
      mcSys.mcCom.mcSideLink.vKeepReceiverOnWhileWaiting(True);
      mClock.vSetSyncAttempt();
    }
  }

  if (mu32ClockResyncTimeout_s)
  {
    mu32ClockResyncTimeout_s--;
  }
  else
  {
    mcSys.mcCom.mcSideLink.vKeepReceiverOnWhileWaiting(False);
  }
}