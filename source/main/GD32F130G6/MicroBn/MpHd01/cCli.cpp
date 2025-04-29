#include "cCli.h"



cCliCmd_Version::cCliCmd_Version():cCliCmd((const char*)"Ver", (const char*)"Shows version info") {}
void cCliCmd_Version::vProzessCmd(cStr &lcParam, cCli *lcCli, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lCallerAdr);
  lcCli->bPrintLn((const char*)"CLI 1.2.3");
}


cCliCmd_Help::cCliCmd_Help():cCliCmd((const char*)"Help", (const char*)"Shows list of commands") {}
void cCliCmd_Help::vProzessCmd(cStr &lcParam, cCli *lcCli, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lCallerAdr);

  u16          lu16Cnt;
  
  lu16Cnt = 0;

  while (lu16Cnt < lcCli->mcCmdList.mCmdListCnt)
  {
    if (lcCli->mcCmdList.mCmdList[lu16Cnt] != null)
    {
      char8 lszValue[8];
      cStr lcValue((char8*)lszValue, 8);
      lcValue.Itoa(lu16Cnt, 10);
      lcValue += (const char*)".) ";
      lcCli->bPrint(lcValue);
      lcCli->bPrint(lcCli->mcCmdList.mCmdList[lu16Cnt]->mcCmd);
      lcCli->bPrint((const char*) " : ");
      lcCli->bPrintLn(lcCli->mcCmdList.mCmdList[lu16Cnt]->mcCmdInfo);
    }

    lu16Cnt++;
  }
}



