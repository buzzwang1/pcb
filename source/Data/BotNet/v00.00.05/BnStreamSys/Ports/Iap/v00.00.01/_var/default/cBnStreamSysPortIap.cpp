#include "cBnStreamSysPortIap.h"

#include "cBotnet.h"
#include "ClockInfo.h"


bool cBnCliCmd_IAP_SysInfo::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  cStr_Create(lszStr, 32);

  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);


  lcCli->bPrintLn((char8*)InAppProg_Platform_Name);
  lcCli->bPrintLn((char8*)InAppProg_Platform_Info);

  lcCli->bPrintLn((const char8*)"Clock:");
  lszStr.Setf((const char8*)"Core: %dMHz",  cClockInfo::u32GetClockCore() / 1000000); lcCli->bPrintLn(lszStr);
  lszStr.Setf((const char8*)"Sys:  %dMHz",  cClockInfo::u32GetClockSys()  / 1000000); lcCli->bPrintLn(lszStr);
  lszStr.Setf((const char8*)"Port: %dMHz",  cClockInfo::u32GetClockPort() / 1000000); lcCli->bPrintLn(lszStr);

  lcCli->bPrintLn((const char8*)"Memory:");

  cBotNet* lcBn = (cBotNet*)lCallerAdr;

  if (lcBn != null)
  {
    // Port[0] = CmdPort
    // Port[1] = MemPort
    cBotNetStreamPort_BotNetMemPort_Base* lcMemPort = (cBotNetStreamPort_BotNetMemPort_Base *) lcBn->mcStreamSys.mcPorts[1];

    if (lcMemPort != null)
    {
      if (lcMemPort->mszName.Instr(0, (const char8*) "BnMemPort") == 0)
      {
        for (u16 lu16Block = 0; lu16Block < lcMemPort->mu8PartitionCnt; lu16Block++)
        {
          lszStr.Setf((const char8*)"%s: %dkb", (char8*)lcMemPort->mstPartitions[lu16Block].szName, lcMemPort->mstPartitions[lu16Block].ui32Size / 1024);
          lcCli->bPrintLn(lszStr);
        }
      }
    }
  }

  return True;
}


void cBotNetStreamPort_BotNetMemPort::vAddedToBn(cBotNet *lcBotNet)
{
  lcBotNet->mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SysInfo);
}