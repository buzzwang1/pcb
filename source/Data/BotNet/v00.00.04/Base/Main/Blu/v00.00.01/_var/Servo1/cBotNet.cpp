#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcUpLink(lcCfg->mu16BnAdr, &mcUartMpHdU0),
  mcStreamSys(this),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcMsgTxDyn(cBotNet_MsgSize), mcMsgTx(&mcMsgTxDyn)
{
  mcCfg = lcCfg;
	mcUpLink.vAddedToBn(this);
}

cBotNet::~cBotNet() {}

void cBotNet::vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx)
{
  mcStreamSys.ConnectPort(lu8SourcePortIdx, lu8DestPortAdr, lu8DestPortIdx);
}

void cBotNet::vSetName(const char8* lszName)
{
  UNUSED(lszName);
}

// Muss alle 1ms aufgerufen werden
void cBotNet::vLinkSyncAll1ms()
{
  mcUpLink.vSync();
  mcUpLink.vTick1ms();
}

void cBotNet::vLinkMisc10ms()
{
  u16 lu16t;

  mcUpLink.vTick10ms();
  for (lu16t = 0; lu16t < cBotNet_StreamSysPortsCnt; lu16t++)
  {
    mcStreamSys.mcPorts[lu16t]->vTick10ms();
  }
}


void cBotNet::vTickHp1ms()
{
  vLinkSyncAll1ms();
}

void cBotNet::vTickLp10ms()
{
  vProcess10ms();
  vLinkMisc10ms();
}


bool cBotNet::bDecodeMsgType(cBotNetMsg_Base *lcMsg)
{
  bool lbMsgConsumed = False;

  if (lcMsg->enGetMsgType() == cBotNetMsgFrame::tenType::enStream)
  {
    if (mcStreamSys.mcRxComBuf.isFitting(lcMsg))
    {
      lbMsgConsumed = mcStreamSys.bPut(lcMsg);
    }
  }
  else
  {
    lbMsgConsumed = True;
  }

  return lbMsgConsumed;
}


bool cBotNet::bDecodeAndDispatch(cBotNetMsg_Base* lcMsg)
{
  lcMsg->vDecode(mcAdr.Get(), mcAdr.Get());

  if (mcAdr.isMe(lcMsg->cGetDAdr()))
  {
    // Nachricht für mich
    return bDecodeMsgType(lcMsg);
  }
  else
  {
    // Weiterleiten an Master
    if (!mcAdr.isMasterOf(lcMsg->cGetDAdr()))
    {
      return mcUpLink.bPut(lcMsg);
    }
  }
  return True;
}

void cBotNet::vResetCom(bool lbAlways)
{
  mcUpLink.vResetCom(lbAlways);
}


void cBotNet::vProcess10ms()
{
  static bool lbMsgTxOk = True;
  static bool lbMsgRxOk = True;

  // UpLink
  do
  {
    if (lbMsgRxOk)
    {
      //Neue Nachricht holen
      mcUpLink.u16Get(&mcMsgRx);
      mcMsgRx.vClearAge();
    }

    if (mcMsgRx.Len() > 0)
    {
      //Wird ok, wenn Nachricht zugeteilt wurde
      lbMsgRxOk = bDecodeAndDispatch(&mcMsgRx);
    }
    else
    {
      break;
    }
  }
  while (lbMsgRxOk);



  mcStreamSys.vProcess();
  do
  {
    if (lbMsgTxOk)
    {
      mcStreamSys.get(&mcMsgTx);
      mcMsgTx.vClearAge();
    }
    if (mcMsgTx.Len() > 0)
    {
      lbMsgTxOk = bDecodeAndDispatch(&mcMsgTx);
    }
    else
    {
      break;
    }
  }
  while (lbMsgTxOk);

  // Timeout-Handling für Nachrichten
  // Falls Nachrichten nicht zugestellt werden können, dann konsumieren.
  if (mcMsgTx.bDoAgeing())
  {
    lbMsgTxOk = True;
  }

  if (mcMsgRx.bDoAgeing())
  {
    lbMsgRxOk = True;
  }
}


