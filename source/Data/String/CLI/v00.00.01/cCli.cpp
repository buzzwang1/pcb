#include "cCli.h"



cCliCmd_Version::cCliCmd_Version():cCliCmd((const char*)"Ver", (const char*)"Shows version info") {}
bool cCliCmd_Version::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lbFirstCall);
  UNUSED(lCallerAdr);
  lcCli->bPrintLn((const char*)"CLI 1.2.3");
  return True;
}



cCliCmd_Help::cCliCmd_Help():cCliCmd((const char*)"Help", (const char*)"Shows list of commands") {}
bool cCliCmd_Help::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lCallerAdr);
  if (lbFirstCall)
  {
    mu16Cnt = 0;
    mu16CmdCnt = 1;
    mu16State = 0;
    mcCmdList = lcCli->mcCmdList;
  }

  while (mcCmdList)
  {
    while (mu16Cnt < mcCmdList->mCmdListCnt)
    {
      if (mcCmdList->mCmdList[mu16Cnt] != null)
      {
        while (mu16State < 1)
        {
          cStr_Create(lcValue, 8);

          lcValue.Itoa(mu16CmdCnt, 10);
          lcValue += (const char*)".) ";
          lcCli->bPrint(lcValue);
          lcCli->bPrint(mcCmdList->mCmdList[mu16Cnt]->mcCmd);
          lcCli->bPrint((const char*) " : ");
          lcCli->bPrintLn(mcCmdList->mCmdList[mu16Cnt]->mcCmdInfo);
          mu16State++;
          return False;
        }
      }
      mu16State = 0;
      mu16Cnt++;
      mu16CmdCnt++;
    }

    mu16Cnt = 0;
    mcCmdList = mcCmdList->mCmdListNext;
  }
  return True;
}



