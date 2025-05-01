#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess *lcpMsgProcess)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mszName(mszBufName, 0, 16)
{

  mcCfg = lcCfg;
  mcpMsgProcess   = lcpMsgProcess;
  mcSideLink = null;
  mcUpLink   = null;
  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet() {}


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


bool cBotNet::bAddLink(cBotNet_LinkBase* lcLink, u16 lu16Adr)
{
  cBotNetAdress lcAdr(lu16Adr);

  if (lcLink != null)
  {
    // Master von mir ?
    if (lcAdr.isMasterOf(mcAdr))
    {
      // Direkter Master ?
      if (lcAdr.isDirectMasterOf(mcAdr))
      {
        mcUpLink = lcLink;
        return lcLink->bAddedToBn(mcAdr.Get());
      }
    }
    // kein Master und kein Slave ? Also SideLink
    else
    {
      mcSideLink = lcLink;
      return lcLink->bAddedToBn(lu16Adr);
    }
  }
  return False;
}


void cBotNet::vSetName(const char8* lszName)
{
  mszName = lszName;
}

void cBotNet::vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync)
{
  if (lpcLink != null)
  {
    if ((lpcLink->IsInitAndOnline()) || (lbForceSync))
    {
      lpcLink->vSync();
    }
  }
}

// Muss alle 1ms aufgerufen werden
void cBotNet::vLinkSyncAll1ms()
{
  vLinkSync(mcUpLink, True);
  vLinkSync(mcSideLink, True);
  if (mcUpLink   != null) mcUpLink->vTick1ms();
  if (mcSideLink != null) mcSideLink->vTick1ms();
}

void cBotNet::vLinkMisc10ms()
{
  if (mcUpLink   != null) mcUpLink->vTick10ms();
  if (mcSideLink != null) mcSideLink->vTick10ms();
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

bool cBotNet::bDecodeAndDispatch()
{
  mcMsgRx.vDecode(mcAdr.Get(), mcAdr.Get());

  if (mcAdr.isMe(mcMsgRx.cGetDAdr()))
  {
    /* Nachricht für mich */
    return bDecodeMsgType(&mcMsgRx);
  }
  else
  {
    // Ist die Destination der SideLink ?
    if ((mcSideLink->mcAdr.isMe(mcMsgRx.cGetDAdr())) ||
        (mcSideLink->mcAdr.isMasterOf(mcMsgRx.cGetDAdr())))
    {
      // Weiterleiten an SideLink
      return bForwardMsgToLink(mcSideLink, &mcMsgRx);
    }
    else
    {
      {
        /* Weiterleiten an Master */
        return bForwardMsgToLink(mcUpLink, &mcMsgRx);
      }
    }
  }

  // Nachricht konnte zugeteil werden
  // => Nachricht konsumieren => return True
  return True;
}

bool cBotNet::bSendMsg(cBotNetMsg_Base* lcMsg)
{
  if (mcAdr.isMe(lcMsg->cGetDAdr()))
  {
    // Nachricht für mich
    return bDecodeMsgType(lcMsg);
  }
  else
  {
    // Ist die Destination der SideLink ?
    if ((mcSideLink->mcAdr.isMe(lcMsg->cGetDAdr())) ||
        (mcSideLink->mcAdr.isMasterOf(lcMsg->cGetDAdr())))
    {
      // Weiterleiten an SideLink
      return bForwardMsgToLink(mcSideLink, lcMsg);
    }
    else
    {
      {
        // Weiterleiten an Master
        return bForwardMsgToLink(mcUpLink, lcMsg);
      }
    }
  }
  return False;
}


void cBotNet::vProcess10ms()
{
  static bool lbMsgRxOk = True;

  // UpLink
  if (mcUpLink)
  {
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
        lbMsgRxOk = bDecodeAndDispatch();
      }
      else
      {
        break;
      }
    }
    while (lbMsgRxOk);
  }

  // SideLink
  if (mcSideLink)
  {
    do
    {
      if (lbMsgRxOk)
      {
        mcSideLink->u16Get(&mcMsgRx);
        mcMsgRx.vClearAge();
      }

      if (mcMsgRx.Len() > 0)
      {
        lbMsgRxOk = bDecodeAndDispatch();
      }
      else
      {
        break;
      }
    }
    while (lbMsgRxOk);
  }
  while (lbMsgRxOk);

  // Timeout-Handling für Nachrichten
  // Falls Nachrichten nicht zugestellt werden können, dann konsumieren.
  if (mcMsgRx.bDoAgeing())
  {
    lbMsgRxOk = True;
  }
}


