#include "SysPkgMsg.h"
#include "System.h"


cCliCmd_SysInfo2::cCliCmd_SysInfo2() 
  :cCliCmd((const char*)"SysInfo2", (const char*)"System Info")
{}

bool cCliCmd_SysInfo2::bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  lcCli->bPrintLn((const char8*)"Clock:");
  lszStr.Setf((const char8*)"Core: %dMHz", cClockInfo::u32GetClockCore() / 1000000); lcCli->bPrintLn(lszStr);
  lszStr.Setf((const char8*)"Sys:  %dMHz", cClockInfo::u32GetClockSys() / 1000000); lcCli->bPrintLn(lszStr);
  lszStr.Setf((const char8*)"Port: %dMHz", cClockInfo::u32GetClockPort() / 1000000); lcCli->bPrintLn(lszStr);

  if (FLASH->ACR & FLASH_ACR_ICEN) { lszStr.Setf((const char8*)"Flash Data cache: enable");  lcCli->bPrintLn(lszStr); }
  else { lszStr.Setf((const char8*)"Flash Data cache: disable"); lcCli->bPrintLn(lszStr); }

  if (FLASH->ACR & FLASH_ACR_DCEN) { lszStr.Setf((const char8*)"Flash Instruction cache: enable");  lcCli->bPrintLn(lszStr); }
  else { lszStr.Setf((const char8*)"Flash Instruction cache: disable"); lcCli->bPrintLn(lszStr); }

  if (FLASH->ACR & FLASH_ACR_PRFTEN) { lszStr.Setf((const char8*)"Flash Prefetch: enable");  lcCli->bPrintLn(lszStr); }
  else { lszStr.Setf((const char8*)"Flash Prefetch: disable"); lcCli->bPrintLn(lszStr); }

  u8 lu8Lat = (u8)((u32)(FLASH->ACR & FLASH_ACR_LATENCY_Msk) >> FLASH_ACR_LATENCY_Pos);
  lszStr.Setf((const char8*)"Flash Latency: %d", lu8Lat); lcCli->bPrintLn(lszStr);

  return True;
}



cCliCmd_SysClock::cCliCmd_SysClock()
  :cCliCmd((const char*)"Clock", (const char*)"System Time")
{}

bool cCliCmd_SysClock::bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  lcCli->bPrintLn((const char8*)"Time:");
  //lszStr.Setf((const char8*)"%02d:%02d:%02d", __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC)), __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC)), __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC))); lcCli->bPrintLn(lszStr);
  lcCli->bPrintLn(mcSys.mcClock.mClock.toString(lszStr));
  if (mcSys.mcClock.mClock.IsValid()) lcCli->bPrintLn((const char8*)"Is Valid");
                                 else lcCli->bPrintLn((const char8*)"Is Not Valid");

  return True;
}






cCliCmd_SleepStatus::cCliCmd_SleepStatus()
  :cCliCmd((const char*)"sleepstatus", (const char*)"Sleep Status")
{}

bool cCliCmd_SleepStatus::bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcCli);
  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  if (lbFirstCall)
  {
    ////lszStr.Setf((rsz)"ClockResyncTimeout: %d", (u16)mcSys.mcClock.mu32ClockResyncTimeout);                 lcCli->bPrintLn(lszStr);
    ////lszStr.Setf((rsz)"NoSleepCounter:     %d", (u16)mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter);  lcCli->bPrintLn(lszStr);
    ////lszStr.Setf((rsz)"WakeUp Pin:         %d", (u16)mcSys.mcBoard.mcWakeup.ui8Get());                      lcCli->bPrintLn(lszStr);
    ////lszStr.Setf((rsz)"SideLink Status:    %d", (u16)mcSys.mcCom.mcBn_0x1000.mcSideLink->mStatus.IsOnline); lcCli->bPrintLn(lszStr);
    ////lszStr.Setf((rsz)"LipoMon:            %d", (u16)mcSys.mcBoard.mcLipoMon.bPowerDown(lszStr));           lcCli->bPrintLn(lszStr);
  }
  return True;
}


cCliCmd_ErrorCounter::cCliCmd_ErrorCounter()
  :cCliCmd((const char*)"ErrCnt", (const char*)"Error Counter")
{}

bool cCliCmd_ErrorCounter::bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);


  if (lbFirstCall)
  {
    if (lcParam.Len() != 0)
    {
      cBotNet_LinkBase* lcLink = null;
      lcParam.Lower();

      if (lcParam.Instr(0, (rsz)"ul") != (u16)-1)          lcLink = mcSys.mcCom.mcBn.mcUpLink;
      else if (lcParam.Instr(0, (rsz)"sl") != (u16)-1)     lcLink = mcSys.mcCom.mcBn.mcSideLink;
      else if (lcParam.Instr(0, (rsz)"dl1") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[0];
      else if (lcParam.Instr(0, (rsz)"dl2") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[1];
      else if (lcParam.Instr(0, (rsz)"dl3") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[2];
      else if (lcParam.Instr(0, (rsz)"dl4") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[3];
      else if (lcParam.Instr(0, (rsz)"dl5") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[4];
      else if (lcParam.Instr(0, (rsz)"dl6") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[5];
      else if (lcParam.Instr(0, (rsz)"dl7") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[6];
      else if (lcParam.Instr(0, (rsz)"dl8") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[7];
      else if (lcParam.Instr(0, (rsz)"dl9") != (u16)-1)    lcLink = mcSys.mcCom.mcBn.mcDownLinks[8];
      else if (lcParam.Instr(0, (rsz)"dl10") != (u16)-1)   lcLink = mcSys.mcCom.mcBn.mcDownLinks[9];
      else if (lcParam.Instr(0, (rsz)"dl11") != (u16)-1)   lcLink = mcSys.mcCom.mcBn.mcDownLinks[10];
      else if (lcParam.Instr(0, (rsz)"dl12") != (u16)-1)   lcLink = mcSys.mcCom.mcBn.mcDownLinks[11];
      else if (lcParam.Instr(0, (rsz)"dl13") != (u16)-1)   lcLink = mcSys.mcCom.mcBn.mcDownLinks[12];
      else if (lcParam.Instr(0, (rsz)"dl14") != (u16)-1)   lcLink = mcSys.mcCom.mcBn.mcDownLinks[13];

      if (lcLink != null)
      {
        if (lcParam.Instr(0, (const char8*)"r")) lcLink->cBnErrCnt::vReset();

        for (u8 lu8Err = 0; lu8Err < (u8)cBnErrCnt::tenErr::enErrCnt; lu8Err++)
        {
          lcLink->cBnErrCnt::vGet((cBnErrCnt::tenErr)lu8Err, lszStr);
          lcCli->bPrintLn(lszStr);
        }
      }
    }
  }
  return True;
}


cMiniCli::cMiniCli()
{
  mszName = (const char*)"BnBoardCntl v01.00.00";
}

void cMiniCli::vAddedToBn(cBotNet* lcBotNet) 
{
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SysInfo2);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_Clock);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SleepStatus);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_ErrorCounter);
}


void cMiniCli::vProcess(void)
{
}


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
        if (lu16SourceAdr == 0x08) // Clock Device Adress
        {
          ////#ifdef PCB_PROJECTCFG_Test
            ////mcTestClassTim[8].vSetMaxTimer(cDiffTimerHw::u32GetTimer());
          ////#endif
          mcSys.mcClock.mClock.vDeserialize(&lcMsg.mcPayload[11]);
          mcSys.mcClock.mu32ClockResyncTimeout_s = 0;
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

              mcSys.mcClock.mClock.vSerialize(&lcMsg.mcPayload[11]);
              mcSys.mcClock.mu32ClockResyncTimeout_s = 0;
              lbConsumed = True;

              lu8Data[ 3] = 0;
              lu8Data[ 4] = 0;
              mcSys.mcClock.mClock.vSerialize(&lu8Data[5]);

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
  }

  return lbConsumed;
}

cSysPkgMsg::cSysPkgMsg()
  : mcBnMsgHandler(),
    mcMiniCli()
{

}

void cSysPkgMsg::vInit1()
{
  mcBnMsgHandler.vAddMsgSys();
  mcSys.mcCom.mcBn.u16StreamPortAdd(&mcMiniCli);
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

