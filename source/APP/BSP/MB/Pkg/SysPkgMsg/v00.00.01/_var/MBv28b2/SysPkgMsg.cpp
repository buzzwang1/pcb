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

  ////if (FLASH->ACR & FLASH_ACR_ICEN) { lszStr.Setf((const char8*)"Flash Data cache: enable");  lcCli->bPrintLn(lszStr); }
  ////else { lszStr.Setf((const char8*)"Flash Data cache: disable"); lcCli->bPrintLn(lszStr); }
  ////
  ////if (FLASH->ACR & FLASH_ACR_DCEN) { lszStr.Setf((const char8*)"Flash Instruction cache: enable");  lcCli->bPrintLn(lszStr); }
  ////else { lszStr.Setf((const char8*)"Flash Instruction cache: disable"); lcCli->bPrintLn(lszStr); }
  ////
  ////if (FLASH->ACR & FLASH_ACR_PRFTEN) { lszStr.Setf((const char8*)"Flash Prefetch: enable");  lcCli->bPrintLn(lszStr); }
  ////else { lszStr.Setf((const char8*)"Flash Prefetch: disable"); lcCli->bPrintLn(lszStr); }
  ////
  ////u8 lu8Lat = (u8)((u32)(FLASH->ACR & FLASH_ACR_LATENCY_Msk) >> FLASH_ACR_LATENCY_Pos);
  ////lszStr.Setf((const char8*)"Flash Latency: %d", lu8Lat); lcCli->bPrintLn(lszStr);

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



cCliCmd_SetVolt::cCliCmd_SetVolt()
  :cCliCmd((const char*)"SetVolt", (const char*)"idx,1=on/0=off,[mV]")
{}

bool cCliCmd_SetVolt::bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
{
  char8  lszStrBuf[16];
  cStr   lszStr(lszStrBuf, 16);


  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  if (mcSys.mcSMan.mcOpMode.IsAuto())
  {
    lcCli->bPrintLn((const char8*)"Manual mode required");
  }
  else
  {
    mcSys.mcSMan.mcOpMode.vSet(cWorkMode::nStManuel);

    if (lcParam.Len() != 0)
    {
      u16 lu16PosKomma1 = lcParam.Instr(0, ',');
      u16 lu16PosKomma2 = lcParam.Instr(lu16PosKomma1 + 1, ',');

      u16 lu16Idx = 0xFF;
      u16 lu16Mode = 0;
      u16 lu16Volt = 0;;

      if ((lu16PosKomma1 != (u16)-1) && (lu16PosKomma2 != (u16)-1))
      {
        lszStr = lcParam;
        lszStr.Mid(0, lu16PosKomma1 - 0);
        lu16Idx = lszStr.Atoi();

        lszStr = lcParam;
        lszStr.Mid(lu16PosKomma1 + 1, lu16PosKomma2 - lu16PosKomma1 - 1);
        lu16Mode = lszStr.Atoi();

        lszStr = lcParam;
        lszStr.Mid(lu16PosKomma2 + 1, 0xFFFF);
        lu16Volt = lszStr.Atoi();
      }

      cVoltageConverter* lcVc = null;
      if (lu16Idx == 0) lcVc = mcSys.mcBoard.mcLipoMon.mcPowerIn;
      if (lu16Idx == 1) lcVc = mcSys.mcBoard.mcLipoMon.mcPowerOut;

      if (lcVc != null)
      {
        if (lu16Mode == 0) lcVc->vDisable();
        if (lu16Mode == 1) lcVc->vEnable();
        lcVc->vSetHwOutputVoltage(lu16Volt);
        lcCli->bPrintLn((rsz)"ok");
      }
    }
  }
  return True;
}



cCliCmd_HwStatus::cCliCmd_HwStatus()
  : cCliCmd((const char*)"hwstatus", (const char*)"Status")
{}

bool cCliCmd_HwStatus::bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  if (lbFirstCall)
  {
    u16 lu16Mode;
    lu16Mode = 0;
    if (mcSys.mcBoard.mcLipoMon.mcPowerIn->isEnable()) lu16Mode = 1;
    lszStr.Setf((rsz)"Power In: %d, %d mV", lu16Mode, mcSys.mcBoard.mcLipoMon.mcPowerIn->u16GetVoltage()); lcCli->bPrintLn(lszStr);
    lu16Mode = 0;
    if (mcSys.mcBoard.mcLipoMon.mcPowerOut->isEnable()) lu16Mode = 1;
    lszStr.Setf((rsz)"Power Out: %d, %d mV", lu16Mode, mcSys.mcBoard.mcLipoMon.mcPowerOut->u16GetVoltage()); lcCli->bPrintLn(lszStr);

    lcCli->bPrintLn((rsz)"");
    lcCli->bPrintLn((rsz)"Switches:");
    lszStr.Setf((rsz)"Bat:  %d", (u16)mcSys.mcBoard.mcLipoMon.lenSmS[cLipoMon::enS1_Lipo]); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Bal1: %d", (u16)mcSys.mcBoard.mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo1]); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Bal2: %d", (u16)mcSys.mcBoard.mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo2]); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Bal3: %d", (u16)mcSys.mcBoard.mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo3]); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Bal4: %d", (u16)mcSys.mcBoard.mcLipoMon.lenSmS[cLipoMon::enS_Balancer_Lipo4]); lcCli->bPrintLn(lszStr);
    return False;
  }
  else
  {
    lcCli->bPrintLn((rsz)"");
    lcCli->bPrintLn((rsz)"Bat:");
    u8 lu8CellCnt = mcSys.mcBoard.mcLipoMon.mCellCnt;
    lszStr.Setf((rsz)"Cnt:  %d",             (i16)lu8CellCnt); lcCli->bPrintLn(lszStr);
    for (u8 lu8CellIdx = 0; lu8CellIdx < lu8CellCnt; lu8CellIdx++)
    {
      lszStr.Setf((rsz)"Bat1: %d mV -> %d mV", (i16)mcSys.mcBoard.mcLipoMon.i16GetBatVoltageAbs(lu8CellIdx + 1),
                                               (i16)mcSys.mcBoard.mcLipoMon.i16GetBatVoltageRel(lu8CellIdx + 1));
      lcCli->bPrintLn(lszStr);
    }

    lcCli->bPrintLn((rsz)"");
    lcCli->bPrintLn((rsz)"Ina:");
    lszStr.Setf((rsz)"Board U: %d", (i16)mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(1)); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Board I: %d", (i16)mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(1)); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Bat   U: %d", (i16)mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(2)); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Bat   I: %d", (i16)mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(2)); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Sys:  U: %d", (i16)mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(3)); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"Sys:  I: %d", (i16)mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(3)); lcCli->bPrintLn(lszStr);

    lcCli->bPrintLn((rsz)"");
    lszStr.Setf((const char8*)"Error: ");
    lcCli->bPrintLn(mcSys.mszErrorInfo);


    lcCli->bPrintLn((rsz)"");
    lszStr.Setf((const char8*)"Mode %s (%d)", mcSys.mcSMan.mcOpMode.ToString(), mcSys.mcSMan.mcOpMode.mCounter);
    lcCli->bPrintLn(lszStr);
  }
  return True;
}



cCliCmd_SleepStatus::cCliCmd_SleepStatus()
  :cCliCmd((const char*)"sleepstatus", (const char*)"Sleep Status")
{}

bool cCliCmd_SleepStatus::bProzessCmd(cStr& lcParam, cCli* lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  if (lbFirstCall)
  {
    lszStr.Setf((rsz)"ClockResyncTimeout: %d", (u16)mcSys.mcClock.mu32ClockResyncTimeout_s);               lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"NoSleepCounter:     %d", (u16)mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter);  lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"WakeUp Pin:         %d", (u16)mcSys.mcBoard.mcWakeup.ui8Get());                      lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"SideLink Status:    %d", (u16)mcSys.mcCom.mcBn.mcSideLink->mStatus.IsOnline); lcCli->bPrintLn(lszStr);
    lszStr.Setf((rsz)"LipoMon:            %d", (u16)mcSys.mcBoard.mcLipoMon.bPowerDown(lszStr));           lcCli->bPrintLn(lszStr);
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



cCliCmd_CcStatus::cCliCmd_CcStatus():cCliCmd((const char*)"cc_status", (const char*)"Light Control Status")
{}

bool cCliCmd_CcStatus::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  lszStr.Setf((const char8*)"Mode %s (%d)", mcSys.mcSMan.mcOpMode.ToString(), mcSys.mcSMan.mcOpMode.mCounter);
  lcCli->bPrintLn(lszStr);

  /*lszStr.Setf((const char8*)"Dreh-Mot: %d-%d", mcMot_TurmDrehung.mui16ActualPos, mcMot_TurmDrehung.mui16TargetPos);
    mcPrinter->vPrintLn(lszStr.ToString());
    lszStr.Setf((const char8*)"Neig-Mot: %d-%d", mcMot_TurmNeigung.mui16ActualPos, mcMot_TurmNeigung.mui16TargetPos);
    mcPrinter->vPrintLn(lszStr.ToString());*/
  return True;
}




cCliCmd_CcSetMode::cCliCmd_CcSetMode()
  :cCliCmd((const char*)"cc_setmode", (const char*)"a=auto m=manual")
{}

bool cCliCmd_CcSetMode::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);

  if (lcParam.Instr(0, (const char8*)"a") == 0)
  {
    lcCli->bPrintLn((const char8*)"ok");
    mcSys.mcSMan.mcOpMode.vSet(cWorkMode::nStAuto);
  }
  else if (lcParam.Instr(0, (const char8*)"m") == 0)
  {
    lcCli->bPrintLn((const char8*)"ok");
    mcSys.mcSMan.mcOpMode.vSet(cWorkMode::nStManuel);
  }
  return True;
}




cCliCmd_CcSetAdc::cCliCmd_CcSetAdc()
  :cCliCmd((const char*)"cc_setdac", (const char*)"x=value in mV")
{}

bool cCliCmd_CcSetAdc::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lcCli);
  UNUSED(lCallerAdr);

  /*if (mcSys.mcSMan.mcOpMode.IsAuto())
  {
    lcCli->bPrintLn((const char8*)"Manual mode required");
  }
  else
  {
    mcSys.mcSMan.mcOpMode.vSet(cWorkMode::nStManuel);
    i16 li16Value = (lcParam.Atoi() - 1);
    mu8DispPageX = ((u8)li16Value);
  }*/

  return True;
}



cCliCmd_CcSetSwitch::cCliCmd_CcSetSwitch():cCliCmd((const char*)"ttm", (const char*)"toggle testmode")
{}

bool cCliCmd_CcSetSwitch::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lcCli);
  UNUSED(lCallerAdr);

  //if (mcSys.mcSMan.mcOpMode.IsAuto())
  //{
  //  lcCli->bPrintLn((const char8*)"Manual mode required");
  //}
  //else
  {
    mcSys.mcSMan.mcOpMode.vSet(cWorkMode::nStManuel);
    mcSys.mcBoard.mcLipoMon.mTestMode = !mcSys.mcBoard.mcLipoMon.mTestMode;
    lszStr.Setf((const char8*)"Testmode: %d", mcSys.mcBoard.mcLipoMon.mTestMode);
    lcCli->bPrintLn(lszStr);
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
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_HwStatus);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SleepStatus);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_ErrorCounter);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_SetVolt);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcStatus);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcSetMode);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcSetAdc);
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcBnCliCmd_CcSetSwitch);
}


void cMiniCli::vProcess(void)
{
}


cBnMsgHandler::cBnMsgHandler()
  : cBotNet_MsgSysProcess(&mcSys.mcCom.mcBn)
{
  mcFastPolling_s = 0;
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
          case 2: // Power
            // Power.Status
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[15];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[0] = 2; // R1
              lu8Data[1] = 0; // S1
              lu8Data[2] = 0; // S2

              // Input, battery and output voltage[mV] and current[mA] set values
              // Board
              lu8Data[ 3] = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(1) >> 8;
              lu8Data[ 4] = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(1) & 0xFF;
              lu8Data[ 5] = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(1) >> 8;
              lu8Data[ 6] = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(1) & 0xFF;

              // Bat
              lu8Data[ 7] = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(2) >> 8;
              lu8Data[ 8] = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(2) & 0xFF;
              lu8Data[ 9] = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(2) >> 8;
              lu8Data[10] = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(2) & 0xFF;

              // System
              lu8Data[11] = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(3) >> 8;
              lu8Data[12] = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(3) & 0xFF;
              lu8Data[13] = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(3) >> 8;
              lu8Data[14] = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(3) & 0xFF;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            break;
          case 3: // BMS
            // BMS.Status
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[6];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[0] = 3; // R1
              lu8Data[1] = 0; // S1
              lu8Data[2] = 0; // S2

              lu8Data[3] = u8GetRomConstChrgCellCount();
              lu8Data[4] = mcSys.mcBoard.mcLipoMon.mS_Lipo.ui8Get();
              lu8Data[5] = (u8)mcSys.mcBoard.mcLipoMon.lenSmLipo;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            else
            // BMS.Cell x status
            if ((lcMsg.mcPayload[1] == 1) && (lcMsg.mcPayload[2] < u8GetRomConstChrgCellCount()))
            {
              u8 lu8Data[7];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              u8 lu8CellIdx = lcMsg.mcPayload[2];

              lu8Data[0] = 3; // R1
              lu8Data[1] = 1; // S1
              lu8Data[2] = lu8CellIdx; // S2

              lu8CellIdx++;

              // CV: Cell voltages [mV];
              lu8Data[ 3] = mcSys.mcBoard.mcLipoMon.i16GetBatVoltageRel(lu8CellIdx) >> 8;
              lu8Data[ 4] = mcSys.mcBoard.mcLipoMon.i16GetBatVoltageRel(lu8CellIdx) & 0xFF;
              // TS: Battery temperature;
              lu8Data[ 5] = 25;
              // BS: Balancer Switch Status;
              lu8Data[ 6] = mcSys.mcBoard.mcLipoMon.u8Get_S_Balancer(lu8CellIdx);

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            break;
          case 4: // PoMo Powermodule
            // PoMo.Mode.Status
            if ((lcMsg.mcPayload[1] == 0) && (lcMsg.mcPayload[2] == 0))
            {
              u8 lu8Data[4];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[0] = 4; // R1
              lu8Data[1] = 0; // S1
              lu8Data[2] = 0; // S2

              // PC:  Power Module Count
              lu8Data[ 3] = 2;

              lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);

              lbConsumed = True;
            }
            else
            // PoMo.Module[x].Set_Values.Status
            if ((lcMsg.mcPayload[1] == 1) && (lcMsg.mcPayload[2] < 2))
            {
              u8 lu8Data[29];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[ 0] = 4; // R1
              lu8Data[ 1] = 1; // S1
              lu8Data[ 2] = lcMsg.mcPayload[2]; // S2

              // Status 1
              lu8Data[ 3] = 1;

              cVoltageConverter* mcPoMo = null;
              switch (lcMsg.mcPayload[2])
              {
                case 0: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerIn; break;
                case 1: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerOut; break;
              }

              // Power Module Type
              lu8Data[ 4] = mcPoMo->mType;

              // ST: Enable state
              lu8Data[ 5] = (u8)mcPoMo->isEnable();

              // IM: Input mode
              // IVIV, IIII : Input voltage and current
              lu8Data[ 6] = mcPoMo->mu8PidModeIn;
              lu8Data[ 7] = ((i16)mcPoMo->mfPidSetVoltIn) >> 8;
              lu8Data[ 8] = ((i16)mcPoMo->mfPidSetVoltIn) & 0xFF;
              lu8Data[ 9] = ((i16)mcPoMo->mfPidSetCurrIn) >> 8;
              lu8Data[10] = ((i16)mcPoMo->mfPidSetCurrIn) & 0xFF;

              // OM : Input mode
              // OVOV, OIOI : Output voltageand current
              lu8Data[11] = mcPoMo->mu8PidModeOut;
              lu8Data[12] = ((i16)mcPoMo->mfPidSetVoltOut) >> 8;
              lu8Data[13] = ((i16)mcPoMo->mfPidSetVoltOut) & 0xFF;
              lu8Data[14] = ((i16)mcPoMo->mfPidSetCurrOut) >> 8;
              lu8Data[15] = ((i16)mcPoMo->mfPidSetCurrOut) & 0xFF;

              // TH.TL: Temperature value (°C,  int16)
              lu8Data[16] = 0;
              lu8Data[17] = 25;

              lcMsg.mcPayload.Set(lu8Data, 18);
              lcMsg.vEncode();
              mcBn->bSendMsg(&lcMsg);


              // Status 2
              lu8Data[3] = 2;
              for (u8 lu8PidIdx = 0;  lu8PidIdx < 4; lu8PidIdx++)
              {
                u8* lu8Temp;
                cFloatLowPass* mcLp = null;
                cFloatPid*     mcPid = null;

                switch (lu8PidIdx)
                {
                  case 0: mcLp = &mcPoMo->mcLpInCurr;  mcPid = &mcPoMo->mcPidInCurr; break;
                  case 1: mcLp = &mcPoMo->mcLpInVolt;  mcPid = &mcPoMo->mcPidInVolt; break;
                  case 2: mcLp = &mcPoMo->mcLpOutCurr; mcPid = &mcPoMo->mcPidOutCurr; break;
                  case 3: mcLp = &mcPoMo->mcLpOutVolt; mcPid = &mcPoMo->mcPidOutVolt; break;
                }


                // PI = PID index [0..3];
                lu8Data[ 4] = lu8PidIdx;

                // IL = input lowpass
                lu8Temp = (u8*)&mcLp->mFactor;
                lu8Data[ 5] = *lu8Temp++;
                lu8Data[ 6] = *lu8Temp++;
                lu8Data[ 7] = *lu8Temp++;
                lu8Data[ 8] = *lu8Temp++;

                // KP = p - value;
                lu8Temp = (u8*)&mcPid->mfKp;
                lu8Data[ 9] = *lu8Temp++;
                lu8Data[10] = *lu8Temp++;
                lu8Data[11] = *lu8Temp++;
                lu8Data[12] = *lu8Temp++;

                // KI = i value;
                lu8Temp = (u8*)&mcPid->mfKi;
                lu8Data[13] = *lu8Temp++;
                lu8Data[14] = *lu8Temp++;
                lu8Data[15] = *lu8Temp++;
                lu8Data[16] = *lu8Temp++;

                // KI_cutoff = i cutoff value;
                lu8Temp = (u8*)&mcPid->mfOutputLimit;
                lu8Data[17] = *lu8Temp++;
                lu8Data[18] = *lu8Temp++;
                lu8Data[19] = *lu8Temp++;
                lu8Data[20] = *lu8Temp++;

                // KD = d value
                lu8Temp = (u8*)&mcPid->mfKd;
                lu8Data[21] = *lu8Temp++;
                lu8Data[22] = *lu8Temp++;
                lu8Data[23] = *lu8Temp++;
                lu8Data[24] = *lu8Temp++;

                // OL = output lowpass
                lu8Temp = (u8*)&mcPid->mLpOutput.mFactor;
                lu8Data[25] = *lu8Temp++;
                lu8Data[26] = *lu8Temp++;
                lu8Data[27] = *lu8Temp++;
                lu8Data[28] = *lu8Temp++;

                lcMsg.mcPayload.Set(lu8Data, sizeof(lu8Data));
                lcMsg.vEncode();
                mcBn->bSendMsg(&lcMsg);
              }

              lbConsumed = True;
            }
            else
              // PoMo.Module[x].Current_Values.Status
            if ((lcMsg.mcPayload[1] == 2) && (lcMsg.mcPayload[2] < 2))
            {
              u8 lu8Data[14];

              // Response Message
              lcMsg.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 33);

              lu8Data[0] = 4; // R1
              lu8Data[1] = 2; // S1
              lu8Data[2] = lcMsg.mcPayload[2]; // S2

              cVoltageConverter* mcPoMo = null;
              switch (lcMsg.mcPayload[2])
              {
                case 0: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerIn; break;
                case 1: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerOut; break;
              }

              i16 li16VoltIn  = 0;
              i16 li16VoltOut = 0;
              i16 li16CurrIn  = 0;
              i16 li16CurrOut = 0;

              switch (lcMsg.mcPayload[2])
              {
                case 0:
                  li16VoltIn  = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(1);
                  li16CurrIn  = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(1);
                  li16VoltOut = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(2);
                  li16CurrOut = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(2);
                  break;
                case 1:
                  li16VoltIn  = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(2);
                  li16CurrIn  = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(2);
                  li16VoltOut = mcSys.mcBoard.mcLipoMon.i16GetSysVoltageAbs(3);
                  li16CurrOut = mcSys.mcBoard.mcLipoMon.i16GetSysCurrentAbs(3);
                  break;
              }

              // ST: Enable state
              lu8Data[ 3] = (u8)mcPoMo->mbEnable;

              // IM: Input mode
              // IVIV, IIII : Input voltage and current
              lu8Data[ 4] = li16VoltIn >> 8;
              lu8Data[ 5] = li16VoltIn & 0xFF;
              lu8Data[ 6] = li16CurrIn >> 8;
              lu8Data[ 7] = li16CurrIn & 0xFF;

              // OM : Input mode
              // OVOV, OIOI : Output voltageand current
              lu8Data[ 8] = li16VoltOut >> 8;
              lu8Data[ 9] = li16VoltOut & 0xFF;
              lu8Data[10] = li16CurrOut >> 8;
              lu8Data[11] = li16CurrOut & 0xFF;

              // TH.TL: Temperature value (°C,  int16)
              lu8Data[12] =  0;
              lu8Data[13] = 25;

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
          case 0: // Sys: Alive
            // Power.Status
            if ((lcMsg.mcPayload[1] == 1) && (lcMsg.mcPayload[2] == 0))
            {

              u16 lu16NoSleepCnt = (lcMsg.mcPayload[3] << 8) + lcMsg.mcPayload[4];

              if (mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter < lu16NoSleepCnt)
              {
                mcSys.mcSMan.mcMySystemPowerDown.mu32NoSleepCounter = lu16NoSleepCnt;
              }

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
          case 4: // Power Modules
            // PoMo.Module[x].Set_Values.Status
            if ((lcMsg.mcPayload[1] == 1) && (lcMsg.mcPayload[2] < 2) &&
                (lcMsg.mcPayload[3] == 1))
            {

              cVoltageConverter* mcPoMo = null;
              switch (lcMsg.mcPayload[2])
              {
                case 0: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerIn;  break;
                case 1: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerOut; break;
              }

              // ST: Enable state
              // IM: Input mode
              // IVIV, IIII : Input voltageand current
              // OM : Input mode
              // OVOV, OIOI : Output voltageand current

              u8  lu8EnableMode  = lcMsg.mcPayload[4];
              u8  lu8InputMode   = lcMsg.mcPayload[5];
              u16 lu16VoltIn_mV  = (lcMsg.mcPayload[6] << 8)  + lcMsg.mcPayload[7];
              u16 lu16CurrIn_mA  = (lcMsg.mcPayload[8] << 8)  + lcMsg.mcPayload[9];

              u8  lu8OutputMode  = lcMsg.mcPayload[10];
              u16 lu16VoltOut_mV = (lcMsg.mcPayload[11] << 8) + lcMsg.mcPayload[12];
              u16 lu16CurrOut_mA = (lcMsg.mcPayload[13] << 8) + lcMsg.mcPayload[14];

              if (lu16VoltOut_mV != 0)
              {
                if (lu16VoltIn_mV  != 0xFFFF) mcPoMo->mfPidSetVoltIn  = (i16)lu16VoltIn_mV;
                if (lu16VoltOut_mV != 0xFFFF) mcPoMo->mfPidSetVoltOut = (i16)lu16VoltOut_mV;
                if (lu16CurrIn_mA  != 0xFFFF) mcPoMo->mfPidSetCurrIn  = (i16)lu16CurrIn_mA;
                if (lu16CurrOut_mA != 0xFFFF) mcPoMo->mfPidSetCurrOut = (i16)lu16CurrOut_mA;


                if (lu8InputMode  != 0xFF) mcPoMo->mu8PidModeIn  = lu8InputMode;
                if (lu8OutputMode != 0xFF) mcPoMo->mu8PidModeOut = lu8OutputMode;

                if (lu8EnableMode != 0xFF)
                {
                  if (lu8EnableMode)
                  {
                    mcPoMo->vEnable();
					if (mcPoMo == mcSys.mcBoard.mcLipoMon.mcPowerOut)
					{
                      mcFastPolling_s = 5;
                      mcBn->mu16DownLinkSyncCnt_ms = 0;
                      mcBn->mu16DownLinkSyncCntReload_ms = 100;
					}
                  }
                  else
                  {
                    mcPoMo->vDisable();
                  }
                }
              }
              else
              {
                mcPoMo->vDisable();
              }

              lbConsumed = True;
            }
            else
            // PoMo.Module[x].Set_Values.Status
            if ((lcMsg.mcPayload[1] == 1) && (lcMsg.mcPayload[2] < 2) &&
                (lcMsg.mcPayload[3] == 2) && (lcMsg.mcPayload[4] < 4))
            {
              cVoltageConverter* mcPoMo = null;
              switch (lcMsg.mcPayload[2])
              {
                case 0: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerIn; break;
                case 1: mcPoMo = mcSys.mcBoard.mcLipoMon.mcPowerOut; break;
              }

              u8 lu8PidIdx = lcMsg.mcPayload[4];

              cFloatLowPass* mcLp = null;
              cFloatPid*     mcPid = null;

              switch (lu8PidIdx)
              {
                case 0: mcLp = &mcPoMo->mcLpInCurr;  mcPid = &mcPoMo->mcPidInCurr; break;
                case 1: mcLp = &mcPoMo->mcLpInVolt;  mcPid = &mcPoMo->mcPidInVolt; break;
                case 2: mcLp = &mcPoMo->mcLpOutCurr; mcPid = &mcPoMo->mcPidOutCurr; break;
                case 3: mcLp = &mcPoMo->mcLpOutVolt; mcPid = &mcPoMo->mcPidOutVolt; break;
              }

              float lfTemp;
              u8* lu8Temp;

              // IL = input lowpass
              lu8Temp = (u8*)&lfTemp;
              *lu8Temp++ = lcMsg.mcPayload[5];
              *lu8Temp++ = lcMsg.mcPayload[6];
              *lu8Temp++ = lcMsg.mcPayload[7];
              *lu8Temp++ = lcMsg.mcPayload[8];
              mcLp->mFactor = lfTemp;

              // KP = p - value;
              lu8Temp = (u8*)&lfTemp;
              *lu8Temp++ = lcMsg.mcPayload[9];
              *lu8Temp++ = lcMsg.mcPayload[10];
              *lu8Temp++ = lcMsg.mcPayload[11];
              *lu8Temp++ = lcMsg.mcPayload[12];
              mcPid->mfKp = lfTemp;

              // KI = i value;
              lu8Temp = (u8*)&lfTemp;
              *lu8Temp++ = lcMsg.mcPayload[13];
              *lu8Temp++ = lcMsg.mcPayload[14];
              *lu8Temp++ = lcMsg.mcPayload[15];
              *lu8Temp++ = lcMsg.mcPayload[16];
              mcPid->mfKi = lfTemp;

              // KI_cutoff = i cutoff value;
              lu8Temp = (u8*)&lfTemp;
              *lu8Temp++ = lcMsg.mcPayload[17];
              *lu8Temp++ = lcMsg.mcPayload[18];
              *lu8Temp++ = lcMsg.mcPayload[19];
              *lu8Temp++ = lcMsg.mcPayload[20];
              mcPid->mfOutputLimit = lfTemp;

              // KD = d value
              lu8Temp = (u8*)&lfTemp;
              *lu8Temp++ = lcMsg.mcPayload[21];
              *lu8Temp++ = lcMsg.mcPayload[22];
              *lu8Temp++ = lcMsg.mcPayload[23];
              *lu8Temp++ = lcMsg.mcPayload[24];
              mcPid->mfKd = lfTemp;


              // OL = output lowpass
              lu8Temp = (u8*)&lfTemp;
              *lu8Temp++ = lcMsg.mcPayload[25];
              *lu8Temp++ = lcMsg.mcPayload[26];
              *lu8Temp++ = lcMsg.mcPayload[27];
              *lu8Temp++ = lcMsg.mcPayload[28];
              mcPid->mLpOutput.mFactor = lfTemp;

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

  if (mcBnMsgHandler.mcFastPolling_s)
  {
    mcBnMsgHandler.mcFastPolling_s--;
  }
  else
  {
    // Wieder auf normales Pollingintervall zurückwechseln
    mcSys.mcCom.mcBn.mu16DownLinkSyncCntReload_ms = cBotNet_DownLinkSyncAll;
  }
}

