#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcStreamSys(this),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcMsgTxDyn(cBotNet_MsgSize), mcMsgTx(&mcMsgTxDyn),
  mcBtr(this),
  mcSpop(this),
  mcRRpt(this),
  mszName(mszBufName, 0, 16)
{

  mcCfg = lcCfg;

  mbMsgTxOk = True;
  mbMsgRxOk = True;

  mu16DownLinkSyncCnt_ms = 0;
  mu16DownLinkSyncCntReload_ms = cBotNet_DownLinkSyncAll;
  
  mcpMsgProcess   = null;
  mcBtr.vAddMsgSys();
  mcSpop.vAddMsgSys();
  mcRRpt.vAddMsgSys();


  mu8DownLinkCnt = 0;

  mstStatus.u32MsgCnt     = 0;
  mstStatus.u32MsgDropCnt = 0;

  mcSideLink = null;
  mcUpLink   = null;

  mStartupDelayCnt = 100;  // [ms]

  mu8DownLinkCnt = 0;
  for (u8 lu8t = 0; lu8t < enCnstSlaveCnt; lu8t++)
  {
    mcDownLinks[lu8t] = null;
    mbHandleHardware[lu8t] = False;

    vLinkDynSyncReset(lu8t);
  }
  mbHandleHardware[0] = False;

  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet() {}


uint32 cBotNet::SizeOf()
{
  uint32 lui32Size;

  lui32Size = sizeof(cBotNet);
  lui32Size -= sizeof(cBotNetStreamSystem);
  lui32Size -= 2 * sizeof(cBotNetMsg_BaseDyn);

  for (int t = 0; t < enCnstSlaveCnt; t++)
  {
    if (mcDownLinks[t])
    {
      lui32Size += mcDownLinks[t]->SizeOf();
    }
  }

  lui32Size += mcMsgRxDyn.SizeOf();
  lui32Size += mcMsgTxDyn.SizeOf();

  lui32Size += mcStreamSys.SizeOf();
  if (mcUpLink)
  {
    lui32Size += mcUpLink->SizeOf();
  }

  return lui32Size;
}

void cBotNet::vSetHandleHardware(u8 lu8IdxDownLink, bool lbState)
{
  lu8IdxDownLink--;
  if (lu8IdxDownLink < cBotNet_DownLinkCnt)
  {
    mbHandleHardware[lu8IdxDownLink] = lbState;
  }
}

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
      case cBotNet_LinkBase::enDownLink:
        for (u8 lu8LinkIdx = 0; lu8LinkIdx < cBotNet_DownLinkCnt; lu8LinkIdx++)
        {
          if (mcDownLinks[lu8LinkIdx] == null)
          {
            mcDownLinks[lu8LinkIdx] = lcLink;
            mu8DownLinkCnt = lu8LinkIdx + 1;
            return lcLink->bAddedToBn(mcAdr.GetSlaveAdr(lu8LinkIdx + 1));
          }
        }
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
    else
      // Slave von mir ?
      if (mcAdr.isMasterOf(lcAdr))
      {
        // Direkter Slave ?
        if (mcAdr.isDirectMasterOf(lcAdr))
        {
          u8 lu8LinkIdx = lcAdr.GetIdx();
          if (lu8LinkIdx > mu8DownLinkCnt)
          {
            mu8DownLinkCnt = lu8LinkIdx;
          }
          mcDownLinks[lu8LinkIdx - 1] = lcLink;
          return lcLink->bAddedToBn(mcAdr.GetSlaveAdr(lu8LinkIdx - 1));
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
  mcDownLinkDynSyncCnt[lu16Idx]       = 0;
  mcDownLinkDynSyncReload[lu16Idx]    = 1;
  mcDownLinkDynSyncReloadCnt[lu16Idx] = 1;
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
  bool lbSyncAll;

  // Es werden nur alle 1000 Versuche (=1000ms) alle Slaves ge'sync't
  // Sonst nur die aktiven
  if (!mu16DownLinkSyncCnt_ms)
  {
    lbSyncAll = True;
    mu16DownLinkSyncCnt_ms = mu16DownLinkSyncCntReload_ms;
  }
  else
  {
    lbSyncAll = False;
    mu16DownLinkSyncCnt_ms--;
  }

  for (u8 lu8t = 0; lu8t < mu8DownLinkCnt; lu8t++)
  {
    bool lbSync = lbSyncAll;
    if (mcDownLinkDynSyncCnt[lu8t] > 0)
    {
      mcDownLinkDynSyncCnt[lu8t]--;
    }

    if (mcDownLinkDynSyncCnt[lu8t] == 0)
    {
      mcDownLinkDynSyncCnt[lu8t] = mcDownLinkDynSyncReload[lu8t];
      lbSync = True;
    }

    if (lbSync) vLinkSync(mcDownLinks[lu8t], lbSyncAll);
  }

  vLinkSync(mcUpLink, lbSyncAll);
  vLinkSync(mcSideLink, lbSyncAll);


  // Wird weitergeleite an 1ms Tick der HW-Schnittstelle...
  // ...und bStartNext()
  for (u8 lu8t = 0; lu8t < enCnstSlaveCnt; lu8t++)
  {
    if ((mcDownLinks[lu8t] != null) && (mbHandleHardware[lu8t]))
    {
      mcDownLinks[lu8t]->vTick1ms();
    }
  }
  if (mcUpLink   != null) mcUpLink->vTick1ms();
  if (mcSideLink != null) mcSideLink->vTick1ms();
}

void cBotNet::vTickHp1ms()
{
  vLinkSyncAllHp1ms();
}


void cBotNet::vLinkMisc10ms()
{
  u16 lu16t;

  /* Startup Delay in ms */
  /* Nachrichten verarbeitung fängt erst dann an, aber links sind schon online */
  if (mStartupDelayCnt > 10) mStartupDelayCnt -= 10;
                        else mStartupDelayCnt  = 0;


  //Dynamische Lastanpassung.
  for (lu16t = 0; lu16t < mu8DownLinkCnt; lu16t++)
  {
    if (mcDownLinks[lu16t] != null) mcDownLinks[lu16t]->vTick10ms();

    if (mcDownLinkDynSyncReloadCnt[lu16t] > 0)
    {
      mcDownLinkDynSyncReloadCnt[lu16t]--;
    }
    else
    {
      if (mcDownLinkDynSyncReload[lu16t] < 64)
      {
        mcDownLinkDynSyncReload[lu16t] *= 2;
      }
    }
  }

  if (mcUpLink   != null) mcUpLink->vTick10ms();
  if (mcSideLink != null) mcSideLink->vTick10ms();


  for (lu16t = 0; lu16t < mcStreamSys.mu8PortCnt; lu16t++)
  {
    mcStreamSys.mcPorts[lu16t]->vTick10ms();
  }
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
      if (mcStreamSys.mcRxComBuf.isFitting(lcMsg))
      {
        lbMsgConsumed = mcStreamSys.bPut(lcMsg);
      }
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
          u8 lu8SlaveIdx = mcAdr.u8GetNextLevelSlaveIdx(mcMsgRx.cGetDAdr().Get());
          if (lu8SlaveIdx > 0)
          {
            vLinkDynSyncReset(lu8SlaveIdx - 1);
            return bForwardMsgToLink(mcDownLinks[lu8SlaveIdx - 1], &mcMsgRx);
          }
          // Nachricht mit schlechter Adresse ->konsumieren
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
        u8 lu8SlaveIdx = mcAdr.u8GetNextLevelSlaveIdx(lcMsg->cGetDAdr().Get());
        if (lu8SlaveIdx > 0)
        {
          vLinkDynSyncReset(lu8SlaveIdx - 1);
          return bForwardMsgToLink(mcDownLinks[lu8SlaveIdx - 1], lcMsg);
        }
        // Nachricht mit schlechter Adresse ->konsumieren
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
        if (mbMsgRxOk) mstStatus.u32MsgCnt++;
      }
      else
      {
        break;
      }
    }
    while (mbMsgRxOk);
  }

  // DownLink
  // Alle Sklaven überprüfen
  for (u8 lu8t = 0; lu8t < mu8DownLinkCnt; lu8t++)
  {
    mcDownLinkActive = mcDownLinks[lu8t];
    if (mcDownLinkActive->IsOnline())
    {
      do
      {
        if (mbMsgRxOk)
        {
          mcDownLinkActive->u16Get(&mcMsgRx);
          mcMsgRx.vClearAge();
        }

        if (mcMsgRx.Len() > 0)
        {
          vLinkDynSyncReset(lu8t);
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

  while (mu32Time_us > 1000)
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


