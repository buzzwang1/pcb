#include "cBnStreamSysPortCmd.h"
#include "cBotnet.h"



void cCliCmd_Ports::vPrintPort(u16 lu16Idx, cBotNetStreamPort* lcPort, cStr& lszStr, cCli *lcCli)
{
  lszStr.Setf((const char*) "%d) ", lu16Idx);
  lcCli->bPrint(lszStr);
  lcCli->bPrintLn(lcPort->mszName);

  // externe Connection
  if (lcPort->mcBnDestAdr.isSet())
  {
    lszStr.Setf((const char*) "  ext. Cnx: %s: %d", lcPort->mcBnDestAdr.ToString(), lcPort->mcBnDestPortIdx);
    lcCli->bPrintLn(lszStr);
  }

  bool lbIntCnx;

  lbIntCnx = False;
  for (u8 liCnxIdx = 0; liCnxIdx < cBotNet_StreamSysPortsCnxCnt; liCnxIdx++)
  {
    // interne Connection
    if (lcPort->mcConnection[liCnxIdx] != 0)
    {
      lbIntCnx = True;
      break;
    }
  }

  if (lbIntCnx)
  {
    lcCli->bPrintLn((const char*)"  int. Cnx:");
    for (u8 liCnxIdx = 0; liCnxIdx < cBotNet_StreamSysPortsCnxCnt; liCnxIdx++)
    {
      // interne Connection
      if (lcPort->mcConnection[liCnxIdx] != 0)
      {
        lszStr.Setf((const char*) "    %d) ", liCnxIdx);
        lcCli->bPrint(lszStr);
        lcCli->bPrintLn(lcPort->mcConnection[liCnxIdx]->mszName);
      }
    }
  }
}


bool cCliCmd_Ports::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lbFirstCall);

  cStr_Create(lszStr, 24);
  cBotNet* lcBotNet = (cBotNet*)lCallerAdr;
 
  for (u8 liPortIdx = 0; liPortIdx < cBotNet_StreamSysPortsCnt; liPortIdx++)
  {
    if (lcBotNet->mcStreamSys.mcPorts[liPortIdx] != null)
    {
      vPrintPort(liPortIdx + 1, lcBotNet->mcStreamSys.mcPorts[liPortIdx], lszStr, lcCli);
    }
  }

  return True;
}


void cCliCmd_Links::vPrintLink(u16 lu16Idx, cBotNet_LinkBase* lcLink, cStr& lszStr, cCli *lcCli)
{
  if (lu16Idx)
  {
    lszStr.Setf((const char*) "%d) ", lu16Idx);
  }
  else
  {
    lszStr = " ";
  }


  if (lcLink != null)
  {
    lszStr += lcLink->mcAdr.ToString();
    lszStr += (const char8*)": ";
    lcCli->bPrint(lszStr);

    if (lcLink->IsInit()) lcCli->bPrint((char8*)"Init ");
                   else   lcCli->bPrint((char8*)"Off ");

    if (lcLink->IsOnline()) lcCli->bPrintLn((char8*)"On");
                   else     lcCli->bPrintLn((char8*)"Off");
  }
  else
  {
    lszStr += (const char8*)"na.";
    lcCli->bPrintLn(lszStr);
  }
}


bool cCliCmd_Links::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lbFirstCall);

  cStr_Create(lszStr, 24);
  cBotNet* lcBotNet = (cBotNet*)lCallerAdr;

  lcCli->bPrint((const char8*)"UpLinks:");
  vPrintLink(0, lcBotNet->mcUpLink, lszStr, lcCli);
  lcCli->bPrint((const char8*)"SideLinks:");
  vPrintLink(0, lcBotNet->mcSideLink, lszStr, lcCli);
  lcCli->bPrintLn((const char8*)"DownLinks:");

  for (u8 liLinkIdx = 0; liLinkIdx < cBotNet_DownLinkCnt; liLinkIdx++)
  {
    vPrintLink(liLinkIdx + 1, lcBotNet->mcDownLinks[liLinkIdx], lszStr, lcCli);
  }

  return True;
}




bool cCliCmd_Status::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lbFirstCall);

  cStr_Create(lszStr, 24);

  cBotNet* lcBotNet = (cBotNet*)lCallerAdr;

  lszStr.Setf((const char8*)"Name: %s", lcBotNet->mszName.ToString());
  lcCli->bPrintLn(lszStr);

  lszStr.Setf((const char8*)"MsgCnt: %d", lcBotNet->mstStatus.u32MsgCnt);
  lcCli->bPrintLn(lszStr);

  lszStr.Setf((const char8*)"MsgDropCnt: %d", lcBotNet->mstStatus.u32MsgDropCnt);
  lcCli->bPrintLn(lszStr);

  return True;
}



/*const cCliCmd* cBotNetStreamPort_BotNetCmdPort::mcMyCmds[] = 
                   {
                      &mcCliCmd_Test,//&cBotNetStreamPort_BotNetCmdPort::mcCliCmd_Ver, 
                      null //&cBotNetStreamPort_BotNetCmdPort::mcCliCmd_Help
                    };*/


cBotNetStreamPort_BotNetCmdPort::cBotNetStreamPort_BotNetCmdPort(cBotNet* lcBotNet)
 : cCli(&mcRxRingBuf, &mcTextOut, &mcMyCmdList, 2, (void*)lcBotNet),
   mcTextOut(mpui8TextBufOut, 512),
   mcMyCmdList(20)
{
  mszName = (const char8*)"BnCmdPort v01.00.02";

  bAddCmd(&mcCliCmd_Ver);
  bAddCmd(&mcCliCmd_Help);
  bAddCmd(&mcCliCmd_Ports);
  bAddCmd(&mcCliCmd_Links);
  bAddCmd(&mcCliCmd_Status);

  lcBotNet = lcBotNet;

  mcStrPrompt = lcBotNet->mcAdr.ToString();
  mcStrPrompt += (const char8*)": ";
}


