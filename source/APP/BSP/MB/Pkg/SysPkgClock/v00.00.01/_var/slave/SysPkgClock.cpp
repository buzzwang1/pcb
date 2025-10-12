#include "SysPkgClock.h"

#include "System.h"


cSysPkgClock::cSysPkgClock()
{
};

void cSysPkgClock::vInit1(void)
{
  mu32ClockResyncTimeout_s = 0;
}

void cSysPkgClock::vInit2(void)
{
  if (mClock.bResync())
  {
    mu32ClockResyncTimeout_s = 3 * 60;
    mClock.vSetSyncAttempt();
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

  if (mClock.bResync())
  {
    mu32ClockResyncTimeout_s = 3 * 60;
    mClock.vSetSyncAttempt();
  }

  if (mu32ClockResyncTimeout_s)
  {
    mu32ClockResyncTimeout_s--;
  }

  if (mu32ClockResyncTimeout_s)
  {
    // Nur alle 4s machen, sonst kommt man mit den 2s-Timeout nach dem übernehmen
    // der Zeit in Konflikt
    if ((mu32ClockResyncTimeout_s & 3) == 0)
    {
      if (mcSys.mcCom.mcBn.mcAdr.Get() != 0x1000) // Nicht an sich selbst schicken
      {
        // Request Zeit
        cBotNetMsg_Static_MsgProt_Create_Prepare(lcMsgReqClock, 16, mcSys.mcCom.mcBn.mcAdr.Get(), 0x1000, 32);

        // TX 02 | 00 | 00 | RM.1M.2M.3M
        lcMsgReqClock.mcPayload[0] = 1;
        lcMsgReqClock.mcPayload[1] = 0;
        lcMsgReqClock.mcPayload[2] = 0;

        lcMsgReqClock.vEncode();
        mcSys.mcCom.mcBn.bSendMsg(&lcMsgReqClock);
      }
    }
  }
}