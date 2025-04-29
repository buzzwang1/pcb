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
}


bool cCliCmd_Ports::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lbFirstCall);

  cStr_Create(lszStr, 24);
  cBotNet* lcBotNet = (cBotNet*)lCallerAdr;

  lszStr.Setf((const char8*)"Name: %s", lcBotNet->mszName.ToString());
  lcCli->bPrintLn(lszStr);
 
  for (u8 liPortIdx = 0; liPortIdx < cBotNet_StreamSysPortsCnt; liPortIdx++)
  {
    if (lcBotNet->mcStreamSys.mcPorts[liPortIdx] != null)
    {
      vPrintPort(liPortIdx + 1, lcBotNet->mcStreamSys.mcPorts[liPortIdx], lszStr, lcCli);
    }
  }

  return True;
}

/*
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
    lszStr += lcLink->cGetDAdr().ToString();
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

  lszStr.Setf((const char8*)"Name: %s", lcBotNet->mszName.ToString());
  lcCli->bPrintLn(lszStr);

  lcCli->bPrint((const char8*)"UpLinks:");
  lszStr = " ";

	lszStr += lcBotNet->mcUpLink.cGetDAdr().ToString();
	lszStr += (const char8*)": ";
	lcCli->bPrint(lszStr);

	if (lcBotNet->mcUpLink.IsInit()) lcCli->bPrint((char8*)"Init ");
							              	else lcCli->bPrint((char8*)"Off ");

	if (lcBotNet->mcUpLink.IsOnline()) lcCli->bPrintLn((char8*)"On");
					           			      else lcCli->bPrintLn((char8*)"Off");

  return True;
}*/




/*bool cCliCmd_Status::bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr)
{
  UNUSED(lcParam);
  UNUSED(lbFirstCall);

  cStr_Create(lszStr, 24);

  cBotNet* lcBotNet = (cBotNet*)lCallerAdr;

  lszStr.Setf((const char8*)"Name: %s", lcBotNet->mszName.ToString());
  lcCli->bPrintLn(lszStr);

  //lszStr.Setf((const char8*)"MsgCnt: %d", lcBotNet->mstStatus.u32MsgCnt);
  //lcCli->bPrintLn(lszStr);

  //lszStr.Setf((const char8*)"MsgDropCnt: %d", lcBotNet->mstStatus.u32MsgDropCnt);
  //lcCli->bPrintLn(lszStr);

  return True;
}*/


cBotNetStreamPort_BotNetCmdPort::cBotNetStreamPort_BotNetCmdPort(cBotNet* lcBotNet)
 : cCli(&mcRxRingBuf, &mcTextOut, &mcMyCmdList, 2, (void*)lcBotNet),
   mcTextOut(mpui8TextBufOut, 256),
   mcMyCmdList(10)
{
  mszName = (const char8*)"BnCmdPort v01.00.02";

  bAddCmd(&mcCliCmd_Ver);
  bAddCmd(&mcCliCmd_Help);
  bAddCmd(&mcCliCmd_Ports);
  //bAddCmd(&mcCliCmd_Links);
  //bAddCmd(&mcCliCmd_Status);

  lcBotNet = lcBotNet;

  mcStrPrompt = lcBotNet->mcAdr.ToString();
  mcStrPrompt += (const char8*)": ";
}


