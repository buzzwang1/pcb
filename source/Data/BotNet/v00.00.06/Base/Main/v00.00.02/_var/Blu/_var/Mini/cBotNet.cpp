#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcBtr(this),
  mcSpop(this),
  mszName(mszBufName, 0, 16)
{

  mcCfg = lcCfg;
  mcpMsgProcess   = null;
  mcBtr.vAddMsgSys();
  mcSpop.vAddMsgSys();
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
void cBotNet::vLinkSyncAllHp1ms()
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
  vLinkSyncAllHp1ms();
}

bool cBotNet::bCallMsgHandler(cBotNetMsg_MsgProt& lcMsgProt)
{
  bool lbMsgConsumed = False;

  cBotNet_MsgSysProcess* mcpMsgHandlerWork = mcpMsgProcess;

  while ((mcpMsgHandlerWork != null) && (!lbMsgConsumed))
  {
    lbMsgConsumed = mcpMsgHandlerWork->bMsg(lcMsgProt);
    mcpMsgHandlerWork = mcpMsgHandlerWork->mcMsgSysNext;
  }
  
  return lbMsgConsumed;
}

bool cBotNet::bDecodeMsgType(cBotNetMsg_Base *lcMsg)
{
  switch (lcMsg->enGetMsgType())
  {
    case cBotNetMsgFrame::tenType::enMessage:
    {
      cBotNetMsg_MsgProt lcMsgProt(lcMsg);
      lcMsgProt.vDecode(mcAdr.Get(), mcAdr.Get());

      bCallMsgHandler(lcMsgProt);
    }
    break;

    case cBotNetMsgFrame::tenType::enStream:
      //Stream Nachrichten zum Sender zurückschicken
      lcMsg->vFlipAdress();
      lcMsg->vEncode(lcMsg->iGetLenPayload());
      bForwardMsgToLink(mcUpLink, lcMsg);
      break;
    default:
      break;
  }

  return True;
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

  // Echo-Unterdrückung 
  // Nur Nachrichten verarbeiten, die nicht von mir selbst geschickt wurden
  if (!mcAdr.isMe(mcMsgRx.cGetSAdr()))
  {
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
        // Ist die Destination ein Slave von mir ?
        if (mcAdr.isMasterOf(mcMsgRx.cGetDAdr()))
        {
          return True;

        }
        else
        {
          /* Weiterleiten an Master */
          return bForwardMsgToLink(mcUpLink, &mcMsgRx);
        }
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
      // Ist die Destination ein Slave von mir ?
      if (mcAdr.isMasterOf(lcMsg->cGetDAdr()))
      {
        // Weiterleiten an Slave
        return True;
      }
      else
      {
        // Weiterleiten an Master
        return bForwardMsgToLink(mcUpLink, lcMsg);
      }
    }
  }
  return False;
}


void cBotNet::vProcessData()
{

  // UpLink
  if (mcUpLink)
  {
    do
    {
      if (mbMsgRxOk)
      {
        //Neue Nachricht holen
        mcUpLink->u16Get(&mcMsgRx);
        mcMsgRx.vClearAge();
      }

      if (mcMsgRx.Len() > 0)
      {
        //Wird ok, wenn Nachricht zugeteilt wurde
        mbMsgRxOk = bDecodeAndDispatch();
      }
      else
      {
        break;
      }
    }
    while (mbMsgRxOk);
  }

  // SideLink
  if (mcSideLink)
  {
    do
    {
      if (mbMsgRxOk)
      {
        mcSideLink->u16Get(&mcMsgRx);
        mcMsgRx.vClearAge();
      }

      if (mcMsgRx.Len() > 0)
      {
        mbMsgRxOk = bDecodeAndDispatch();
      }
      else
      {
        break;
      }
    }
    while (mbMsgRxOk);
  }
  while (mbMsgRxOk);

  // Timeout-Handling für Nachrichten
  // Falls Nachrichten nicht zugestellt werden können, dann konsumieren.
  if (mcMsgRx.bDoAgeing())
  {
    mbMsgRxOk = True;
  }
}

void cBotNet::vProcess(u32 lu32DiffTime_us)
{
  mu32Time_us += lu32DiffTime_us;

  while (mu32Time_us > 1000)
  {
    mu32Time_us -= 1000;
    mu32Time_ms++;

    vProcessData();

    if (mu32Time_ms >= 10)
    {
      mu32Time_ms = 0;
      vLinkMisc10ms();
      mcBtr.vProcess10ms();
      mcSpop.vProcess10ms();
    }
  }
}


