#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcStreamSys(this),
  mcMsgRx(mu8MsgRxBuf, sizeof(mu8MsgRxBuf)),
  mcMsgTx(mu8MsgTxBuf, sizeof(mu8MsgTxBuf)),
  mcBtr(this),
  mcSpop(this),
  mcRRpt(this),
  mszName(mszBufName, 0, 16)
{

  mcCfg = lcCfg;

  mbMsgTxOk = True;
  mbMsgRxOk = True;

  
  mcpMsgProcess   = null;
  mcBtr.vAddMsgSys();
  mcSpop.vAddMsgSys();
  mcRRpt.vAddMsgSys();


  mstStatus.u32MsgCnt     = 0;
  mstStatus.u32MsgDropCnt = 0;

  mcUpLink   = null;

  mStartupDelayCnt = 100;  // [ms]

  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet() {}




void cBotNet::vSetHandleHardware(u8 lu8IdxDownLink, bool lbState)
{
  UNUSED(lu8IdxDownLink);
  UNUSED(lbState);
}

bool cBotNet::bAddLink(cBotNet_LinkBase* lcLink)
{
  // FlKa_20250818: Alles auskommentiert, sonst hat Sidelink 0xE000 nicht funktioniert
  //if (lcLink != null)
  //{
  //  switch (lcLink->menType)
  //  {
  //    case cBotNet_LinkBase::enUpLink:
        mcUpLink = lcLink;
        return lcLink->bAddedToBn(mcAdr.Get());
  //      break;
  //    default:
  //      break;
  //  }
  //}
  //return False;
}


bool cBotNet::bAddLink(cBotNet_LinkBase* lcLink, u16 lu16Adr)
{
  cBotNetAdress lcAdr(lu16Adr);

  if (lcLink != null)
  {
    // FlKa_20250818: Alles auskommentiert, sonst hat Sidelink 0xE000 nicht funktioniert
    // Master von mir ?
    //if (lcAdr.isMasterOf(mcAdr))
    //{
      // Direkter Master ?
      //if (lcAdr.isDirectMasterOf(mcAdr))
      //{
        mcUpLink = lcLink;
        return lcLink->bAddedToBn(mcAdr.Get());
      //}
    //}
  }
  return False;
}



u16 cBotNet::u16StreamPortAdd(cBotNetStreamPort* lcPort)
{
  return mcStreamSys.AddPort(lcPort);
}

void cBotNet::vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx)
{
  mcStreamSys.ConnectPort(lu8SourcePortIdx, lu8DestPortAdr, lu8DestPortIdx);
}

void cBotNet::vSetName(const char8* lszName)
{
  mszName = lszName;
}

void cBotNet::vLinkDynSyncReset(u16 lu16Idx)
{
  UNUSED(lu16Idx);
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

  // Wird weitergeleite an 1ms Tick der HW-Schnittstelle...
  // ...und bStartNext()
  mcUpLink->vTick1ms();
}

void cBotNet::vTickHp1ms()
{
  vLinkSyncAllHp1ms();
}


void cBotNet::vLinkMisc10ms()
{
  /* Startup Delay in ms */
  /* Nachrichten verarbeitung fängt erst dann an, aber links sind schon online */
  if (mStartupDelayCnt > 10) mStartupDelayCnt -= 10;
                        else mStartupDelayCnt  = 0;

  mcUpLink->vTick10ms();

  mcStreamSys.vTick10ms();
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
  bool lbMsgConsumed = False;

  switch (lcMsg->enGetMsgType())
  {
    case cBotNetMsgFrame::tenType::enMessage:
    {
      cBotNetMsg_MsgProt lcMsgProt(lcMsg);
      lcMsgProt.vDecode(mcAdr.Get(), mcAdr.Get());

      bCallMsgHandler(lcMsgProt);

      lbMsgConsumed = True;
    }
    break;

    case cBotNetMsgFrame::tenType::enStream:
      lbMsgConsumed = mcStreamSys.bPut(lcMsg);
      break;

    case cBotNetMsgFrame::tenType::enPacket: // Noch nicht implementiert
      lbMsgConsumed = True;
      break;

    case cBotNetMsgFrame::tenType::enMultiMessage: // Noch nicht implementiert
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
      /* Weiterleiten an Master */
      return bForwardMsgToLink(mcUpLink, &mcMsgRx);
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
    // Weiterleiten an Master
    return bForwardMsgToLink(mcUpLink, lcMsg);
  }
  return False;
}


bool cBotNet::bForwardTxMsg()
{
  mcMsgTx.vDecode(mcAdr.Get(), mcAdr.Get());
  return bSendMsg(&mcMsgTx);
}


void cBotNet::vProcessData()
{
  if (mStartupDelayCnt) return;

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
        if (mbMsgRxOk) mstStatus.u32MsgCnt++;
      }
      else
      {
        break;
      }
    }
    while (mbMsgRxOk);
  }


  mcStreamSys.vProcess();
  do
  {
    if (mbMsgTxOk)
    {
      mcStreamSys.get(&mcMsgTx);
      mcMsgTx.vClearAge();
    }
    if (mcMsgTx.Len() > 0)
    {
      mbMsgTxOk = bForwardTxMsg();
      if (mbMsgTxOk) mstStatus.u32MsgCnt++;
    }
    else
    {
      break;
    }
  }
  while (mbMsgTxOk);
}

void cBotNet::vMessageAgeing10ms()
{
  // Timeout-Handling für Nachrichten
  // Falls Nachrichten nicht zugestellt werden können, dann konsumieren.
  if (mcMsgTx.bDoAgeing())
  {
    mbMsgTxOk = True;
    mstStatus.u32MsgDropCnt++;
  }

  if (mcMsgRx.bDoAgeing())
  {
    mbMsgRxOk = True;
    mstStatus.u32MsgDropCnt++;
  }
}

void cBotNet::vProcess(u32 lu32DiffTime_us)
{
  mu32Time_us += lu32DiffTime_us;

  while (mu32Time_us >= 1000)
  {
    mu32Time_us -= 1000;
    mu32Time_ms++;

    vProcessData();

    if (mu32Time_ms >= 10)
    {
      mu32Time_ms = 0;
      vLinkMisc10ms();
      vMessageAgeing10ms();
      mcBtr.vProcess10ms();
      mcSpop.vProcess10ms();
      mcRRpt.vProcess10ms();
    }
  }
}


