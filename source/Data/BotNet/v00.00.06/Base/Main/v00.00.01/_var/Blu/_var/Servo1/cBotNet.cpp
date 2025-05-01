#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn)
{
  mcUpLink   = null;
}

cBotNet::~cBotNet() {}


bool cBotNet::bAddLink(cBotNet_LinkBase* lcLink)
{
  mcUpLink = lcLink;
  return lcLink->bAddedToBn(mcAdr.Get());
}


void cBotNet::vSetName(const char8* lszName)
{
  UNUSED(lszName);
}

// Muss alle 1ms aufgerufen werden
void cBotNet::vLinkSyncAll1ms()
{
  mcUpLink->vSync();
  mcUpLink->vTick1ms();
}

void cBotNet::vLinkMisc10ms()
{
  mcUpLink->vTick10ms();
}


void cBotNet::vTickHp1ms()
{
  vLinkSyncAll1ms();
}

void cBotNet::vTickLp10ms()
{
  vProcess10ms();
  vLinkMisc10ms();
  mcSpop.vProcess10ms();
}


bool cBotNet::bDecodeMsgType(cBotNetMsg_Base *lcMsg)
{
  bool lbMsgConsumed = True;

  switch (lcMsg->enGetMsgType())
  {
    case cBotNetMsgFrame::tenType::enMessage:
    //case cBotNetMsgFrame::tenType::enRMessage:
    {
      cBotNetMsg_MsgProt lcMsgProt(lcMsg);
      lcMsgProt.vDecode(mcAdr.Get(), mcAdr.Get());

      lbMsgConsumed = mcSpop.bMsg(lcMsgProt, this);
      lbMsgConsumed = True;
    }
    break;

    case cBotNetMsgFrame::tenType::enStream:
      //Stream Nachrichten zum Sender zurückschicken
      lcMsg->vFlipAdress();
      lcMsg->vEncode(lcMsg->iGetLenPayload());
      bForwardMsgToLink(mcUpLink, lcMsg);
      lbMsgConsumed = True;
      break;
    default:
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


void cBotNet::vProcess10ms()
{
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

  // Timeout-Handling für Nachrichten
  // Falls Nachrichten nicht zugestellt werden können, dann konsumieren.
  if (mcMsgRx.bDoAgeing())
  {
    lbMsgRxOk = True;
  }
}


