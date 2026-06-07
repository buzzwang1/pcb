#include "SysPkgMsg.h"
#include "System.h"




cBnMsgHandler::cBnMsgHandler()
  : cBotNet_MsgSysProcess(&mcSys.mcCom.mcBn)
{
}


bool cBnMsgHandler::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  bool lbConsumed = False;

  switch (lcMsg.mu16Idx)
  {
    case 17: // 0x11 Legacy clock message
      // Überprüfen, ob es eine Clock Nachricht ist
      //                         0    1  2    3    4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19    20 21 22 23 24
      //                              0  1    2    3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18
      //  Clock:                11 | DH DL | 10 | 00.XX.XX.XX XX.XX.XX.XX YH.YL.MM.DD HH.MM.SS.VV || 00 00 SA SA 80
      // Clock Nachricht ?
      if ((lcMsg.cGetSAdr() == 0x1000) &&
          (lcMsg.u8Len() == 25))
      {
        u16 lu16SourceAdr = (lcMsg.mcPayload[0] << 8) + lcMsg.mcPayload[1];
        if ((lu16SourceAdr == 0x08) &&     // Clock Device Adress
            (lcMsg.mcPayload[2] == 0x10))  // Clock Signal
        {
          ////#ifdef PCB_PROJECTCFG_Test
            ////mcTestClassTim[8].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
          ////#endif
          mcSys.mcClock.mClock.vDeserialize(&lcMsg.mcPayload[11]);

          // Noch 2 sekunden bis zum Schlafen, damit noch die 1s Task mindestens einmal drankommen kann
          mcSys.mcClock.mu32ClockResyncTimeout_s = 2;

          lbConsumed = True;
        }
      }
      break;
      case 32: // Request message
        switch (lcMsg.mcPayload[0])
        {
          case 0: // Status
            // Status.Version
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[13];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[ 0] = 0; // R1
              lu8Data[ 1] = 0; // S1
              lu8Data[ 2] = 0; // S2

              // HV: HW Version
              lu8Data[ 3] = u16GetRomConstHwInfo() >> 8;
              lu8Data[ 4] = u16GetRomConstHwInfo() & 0xFF;;
              // SV: SW Version
              lu8Data[ 5] = 0;
              lu8Data[ 6] = 0;
              // BT Board Type
              lu8Data[ 7] = u8GetRomConstBoardType();

              lu8Data[ 8] = 0;
              lu8Data[ 9] = 0;
              lu8Data[10] = 0;
              lu8Data[11] = 0;
              lu8Data[12] = 0;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            else
            // Status.Software
            if ((lcMsg.mcPayload[1] == 1) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[13];
              lcMsg.vFlipAdress();

              lcMsg.mu16Idx = 33; // Response Message

              lu8Data[ 0] = 0; // R1
              lu8Data[ 1] = 1; // S1
              lu8Data[ 2] = 0; // S2

              // TS: Time to sleep [s]
              lu8Data[ 3] = u16GetRomConstHwInfo() >> 8;
              lu8Data[ 4] = u16GetRomConstHwInfo() & 0xFF;;
              // SM: system mode (bitcoded, B0 = automatic/manuel, B1 = testmode)
              lu8Data[ 5] = 0;
              // ER: Error indicator
              lu8Data[ 6] = 0;
              lu8Data[ 7] = 0;

              lu8Data[ 8] = 0;
              lu8Data[ 9] = 0;
              lu8Data[10] = 0;
              lu8Data[11] = 0;
              lu8Data[12] = 0;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            break;
          case 1: // Clock
            // Clock.DateTime
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[13];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[0] = 1; // R1
              lu8Data[1] = 0; // S1
              lu8Data[2] = 0; // S2

              lu8Data[ 3] = mcSys.mcClock.mClock.IsValid();
              lu8Data[ 4] = mcSys.mcClock.mClock.mu8ClockSource;

              i32 li32Temp = mcSys.mcClock.mClock.i32DaysDiff(mcSys.mcClock.mClock.mLastSync);
              if (li32Temp < 0) li32Temp = -li32Temp;
              lu8Data[5] = (u8)li32Temp;

              mcSys.mcClock.mClock.vSerialize(&lu8Data[6]);

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 1))
            {
              u8 lu8Data[13];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[0] = 1; // R1
              lu8Data[1] = 0; // S1
              lu8Data[2] = 1; // S2

              lu8Data[3] = 0;
              lu8Data[4] = 0;
              lu8Data[5] = 0;

              mcSys.mcClock.mClock.vSerializeLastSync(&lu8Data[6]);

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            else
            // Clock.Calender
            if ((lcMsg.mcPayload[1] == 1) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[13];
              lcMsg.vFlipAdress();

              lcMsg.mu16Idx = 33; // Response Message

              lu8Data[0] = 1; // R1
              lu8Data[1] = 1; // S1
              lu8Data[2] = 0; // S2

              // TBD
              lu8Data[ 3] = 0;
              lu8Data[ 4] = 0;
              lu8Data[ 5] = 0;
              lu8Data[ 6] = 0;
              lu8Data[ 7] = 0;
              lu8Data[ 8] = 0;
              lu8Data[ 9] = 0;
              lu8Data[10] = 0;
              lu8Data[11] = 0;
              lu8Data[12] = 0;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            break;
        }
        break;

      case 34: // Set message
        switch (lcMsg.mcPayload[0])
        {
          case 0: // Sys
            // Sys: Alive
            if ((lcMsg.mcPayload[1] == 2) && (lcMsg.mcPayload[2] == 0))
            {

              u16 lu16NoSleepCnt = (lcMsg.mcPayload[3] << 8) + lcMsg.mcPayload[4];

              if (mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter < lu16NoSleepCnt)
              {
                mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter = lu16NoSleepCnt;
              }

              lbConsumed = True;
            }
            else
            // Sys: Wakeup Sim
            if ((lcMsg.mcPayload[1] == 2) && (lcMsg.mcPayload[2] == 1))
            {
              u32 lu32Data = (lcMsg.mcPayload[4] << 24) + (lcMsg.mcPayload[5] << 16) + (lcMsg.mcPayload[6] << 8) + lcMsg.mcPayload[7];
              mcSys.mcSMsg.mcWakeupSim.vSet(lcMsg.mcPayload[4], lu32Data);
              lbConsumed = True;
            }
            break;
          case 1: // DateTime
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
               mcSys.mcClock.mClock.vDeserialize(&lcMsg.mcPayload[6]);

               // Noch 2 sekunden bis zum Schlafen, damit noch die 1s Task mindestens einmal drankommen kann
               mcSys.mcClock.mu32ClockResyncTimeout_s = 2;

               lbConsumed = True;
            }
            break;
        }
        break;
  }

  return lbConsumed;
}

cSysPkgMsg::cSysPkgMsg()
  : mcBnMsgHandler()
{

}

void cSysPkgMsg::vInit1()
{
  mcBnMsgHandler.vAddMsgSys();
}

void cSysPkgMsg::vInit2()
{
}


void cSysPkgMsg::vTick1msHp()
{
}

void cSysPkgMsg::vTick1msLp(void)
{
}

void cSysPkgMsg::vTick10msLp(void)
{
}

void cSysPkgMsg::vTick100msLp(void)
{
}

void cSysPkgMsg::vTick1000msLp(void)
{
}

