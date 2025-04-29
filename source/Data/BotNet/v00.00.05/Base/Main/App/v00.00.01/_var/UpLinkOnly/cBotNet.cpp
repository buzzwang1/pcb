#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess *lcpMsgProcess)
 :mcAdr(lcCfg->mu16BnAdr),
  mcStreamSys(this),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcMsgTxDyn(cBotNet_MsgSize), mcMsgTx(&mcMsgTxDyn),
  mszName(mszBufName, 16)
{
  mcCfg = lcCfg;
  mcpMsgProcess   = lcpMsgProcess;

  mcUpLink   = null;

  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet()
{}



bool cBotNet::bAddLink(cBotNet_LinkBase* lcLink)
{
  if (lcLink != null)
  {
    switch (lcLink->menType)
    {
      case cBotNet_LinkBase::enUpLink:
        mcUpLink = lcLink;
        return lcLink->bAddedToBn(mcAdr.Get());
        break;
      default:
        break;
    }
  }
  return False;
}


void cBotNet::vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx)
{
  mcStreamSys.ConnectPort(lu8SourcePortIdx, lu8DestPortAdr, lu8DestPortIdx);
}

void cBotNet::vSetName(const char8* lszName)
{
  mszName = lszName;
}

// Muss alle 1ms aufgerufen werden
void cBotNet::vLinkSyncAll1ms()
{
  mcUpLink->vSync();
  mcUpLink->vTick1ms();
}

void cBotNet::vLinkMisc10ms()
{
  u16 lu16t;

  mcUpLink->vTick10ms();
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
  bool lbMsgConsumed = True;

  switch (lcMsg->enGetMsgType())
  {
    case cBotNetMsgFrame::tenType::enMessage:
    case cBotNetMsgFrame::tenType::enRMessage:
    {
      cBotNetMsg_MsgProt lcMsgProt(lcMsg);
      lcMsgProt.vDecode(mcAdr.Get(), mcAdr.Get());
      if (lcMsgProt.mu16Idx > 63)
      {
        mcpMsgProcess->vMsg(lcMsgProt, this);
      }
    }
    break;

    case cBotNetMsgFrame::tenType::enStream:
      if (mcStreamSys.mcRxComBuf.isFitting(lcMsg))
      {
        lbMsgConsumed = mcStreamSys.bPut(lcMsg);
      }
      break;

    default:
    case cBotNetMsgFrame::tenType::enMultiMessage: // Noch nicht implementiert
    case cBotNetMsgFrame::tenType::enPacket: // Noch nicht implementiert
      lbMsgConsumed = True;
      break;
  }

  return lbMsgConsumed;
}


bool cBotNet::bForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg)
{
  if (lpcLink)
  {
    return lpcLink->bPut(lpcMsg);
  }
  else
  {
    //Wenn Link nicht da ist, dann konsumieren
    return True;
  }
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
      return mcUpLink->bPut(lcMsg);
    }
  }

  // Nachricht konnte zugeteil werden
  // => Nachricht konsumieren => return True
  return True;
}

bool cBotNet::bSendMsg(cBotNetMsg_Base* lcMsg)
{
  return bForwardMsgToLink(mcUpLink, lcMsg);
}


bool cBotNet::bForwardTxMsg()
{
  return bForwardMsgToLink(mcUpLink, &mcMsgTx);
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
      mcUpLink->u16Get(&mcMsgRx);
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


