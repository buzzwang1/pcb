#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess *lcpMsgProcess)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcStreamSys(this),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcMsgTxDyn(cBotNet_MsgSize), mcMsgTx(&mcMsgTxDyn),
  mszName(mszBufName, 16)
{

  mcCfg = lcCfg;

  mu8DownLinkSyncCnt = 0;
  mcpMsgProcess   = lcpMsgProcess;

  mu8DownLinkCnt = 0;

  mstStatus.u32MsgCnt     = 0;
  mstStatus.u32MsgDropCnt = 0;

  mcSideLink = null;
  mcUpLink   = null;

  mStartupDelayCnt = 100;  // [ms]

  mu8DownLinkCnt = 0;
  for (int t = 0; t < enCnstSlaveCnt; t++)
  {
    mcDownLinks[t] = null;
  }

  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet()
{}


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

bool cBotNet::bAddLink(cBotNet_LinkBase* lcLink)
{
  if (lcLink != null)
  {
    // Master von mir ?
    if (lcLink->mcAdr.isMasterOf(mcAdr))
    {
      // Direkter Master ?
      if (lcLink->mcAdr.isDirectMasterOf(mcAdr))
      {
        lcLink->vAddedToBn(this, cBotNet_LinkBase::enUpLink);
        mcUpLink = lcLink;
        return True;
      }
    }
    else
    // Slave von mir ?
    if (mcAdr.isMasterOf(lcLink->mcAdr))
    {
      // Direkter Slave ?
      if (mcAdr.isDirectMasterOf(lcLink->mcAdr))
      {
        u8 lu8LinkIdx = lcLink->mcAdr.GetIdx();
        if (lu8LinkIdx > mu8DownLinkCnt)
        {
          mu8DownLinkCnt = lu8LinkIdx;
        }
        lcLink->vAddedToBn(this, cBotNet_LinkBase::enDownLink);
        mcDownLinks[lu8LinkIdx - 1] = lcLink;
        return True;
      }
    }
    // kein Master und kein Slave ? Also SideLink
    else
    {
      lcLink->vAddedToBn(this, cBotNet_LinkBase::enSideLink);
      mcSideLink = lcLink;
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


void cBotNet::vSyncLink(cBotNet_LinkBase* lpcLink, bool lbForceSync)
{
  if (lpcLink != null)
  {
    if ((lpcLink->IsInitAndOnline()) || (lbForceSync))
    {
      lpcLink->vSync();
    }
  }
}

void cBotNet::vSync()
{
  bool lbSyncAll;

  // Es werden nur alle 100 Versuche alle Slaves ge'sync't
  // Sonst nur die aktiven
  if (!mu8DownLinkSyncCnt)
  {
    lbSyncAll = True;
    mu8DownLinkSyncCnt = 100;
  }
  else
  {
    lbSyncAll = False;
    mu8DownLinkSyncCnt--;
  }

  for (int t = 0; t < mu8DownLinkCnt; t++)
  {
    vSyncLink(mcDownLinks[t], lbSyncAll);
  }

  vSyncLink(mcUpLink, lbSyncAll);
  vSyncLink(mcSideLink, lbSyncAll);
}


void cBotNet::vTick10ms()
{
  u16 lu16t;

  if (mStartupDelayCnt > 10) mStartupDelayCnt -= 10;
                        else mStartupDelayCnt  = 0;

  for (lu16t = 0; lu16t < mu8DownLinkCnt; lu16t++)
  {
    if (mcDownLinks[lu16t] != null) mcDownLinks[lu16t]->vTick10ms();
  }

  if (mcUpLink   != null) mcUpLink->vTick10ms();
  if (mcSideLink != null) mcSideLink->vTick10ms();


  for (lu16t = 0; lu16t < mcStreamSys.mu8PortCnt; lu16t++)
  {
    mcStreamSys.mcPorts[lu16t]->vTick10ms();
  }
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
      if (lcMsgProt.mu16Idx > 63)
      {
        mcpMsgProcess->vMsg(lcMsgProt, this);
        lbMsgConsumed = True;
      }
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

bool cBotNet::bDecode()
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
    if (mcSideLink->mcAdr.isMe(mcMsgRx.cGetDAdr()))
    {
      // Weiterleiten an SideLink
      return bForwardMsgToLink(mcSideLink, &mcMsgRx);
    }
    else
    {
      // Ist die Destination ein Slave von mir ?
      if (mcAdr.isMasterOf(mcMsgRx.cGetDAdr()))
      {
        u8 lu8SlaveIdx = mcMsgRx.cGetDAdr().GetIdx();
        return bForwardMsgToLink(mcDownLinks[lu8SlaveIdx - 1], &mcMsgRx);
      }
      else
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
    if (mcSideLink->mcAdr.isMe(lcMsg->cGetDAdr()))
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
        return bForwardMsgToLink(mcDownLinks[lu8SlaveIdx - 1], lcMsg);
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

  if (mcAdr.isMe(mcMsgTx.cGetDAdr()))
  {
    // Nachricht für mich
    return bDecodeMsgType(&mcMsgTx);
  }
  else
  {
    // Ist die Destination der SideLink ?
    if (mcSideLink->mcAdr.isMe(mcMsgTx.cGetDAdr()))
    {
      // Weiterleiten an SideLink
      return bForwardMsgToLink(mcSideLink, &mcMsgTx);
    }
    else
    {
      // Ist die Destination ein Slave von mir ?
      if (mcAdr.isMasterOf(mcMsgTx.cGetDAdr()))
      {
        // Weiterleiten an Slave
        u8 lu8SlaveIdx = mcAdr.u8GetNextLevelSlaveIdx(mcMsgTx.cGetDAdr().Get());
        return bForwardMsgToLink(mcDownLinks[lu8SlaveIdx - 1], &mcMsgTx);
      }
      else
      {
        // Weiterleiten an Master
        return bForwardMsgToLink(mcUpLink, &mcMsgTx);
      }
    }
  }
  return False;
}

void cBotNet::vResetCom(bool lbAlways)
{
  if (mStartupDelayCnt) return;

  // UpLink
  if (mcUpLink)
  {
    mcUpLink->vResetCom(lbAlways);
  }

  // SideLink
  if (mcSideLink)
  {
    mcSideLink->vResetCom(lbAlways);
  }

  // DownLink
  for (int t = 0; t < mu8DownLinkCnt; t++)
  {
    mcDownLinks[t]->vResetCom(lbAlways);
  }
}


void cBotNet::vProcess()
{
  static bool lbMsgTxOk = True;
  static bool lbMsgRxOk = True;

  if (mStartupDelayCnt) return;

  // UpLink
  if (mcUpLink)
  {
    do
    {
      if (lbMsgRxOk)
      {
        mcUpLink->u16Get(&mcMsgRx);
        mcMsgRx.vClearAge();
      }

      if (mcMsgRx.Len() > 0)
      {
        lbMsgRxOk = bDecode();
        if (lbMsgRxOk) mstStatus.u32MsgCnt++;
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
        lbMsgRxOk = bDecode();
        if (lbMsgRxOk) mstStatus.u32MsgCnt++;
      }
      else
      {
        break;
      }
    }
    while (lbMsgRxOk);
  }

  // DownLink
  // Alle Sklaven überprüfen
  for (int t = 0; t < mu8DownLinkCnt; t++)
  {
    mcDownLinkActive = mcDownLinks[t];
    if (mcDownLinkActive->IsOnline())
    {
      do
      {
        if (lbMsgRxOk)
        {
          mcDownLinkActive->u16Get(&mcMsgRx);
          mcMsgRx.vClearAge();
        }

        if (mcMsgRx.Len() > 0)
        {
          lbMsgRxOk = bDecode();
          if (lbMsgRxOk) mstStatus.u32MsgCnt++;
        }
        else
        {
          break;
        }
      }
      while (lbMsgRxOk);
    }
  }

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
      lbMsgTxOk = bForwardTxMsg();
      if (lbMsgTxOk) mstStatus.u32MsgCnt++;
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
    mstStatus.u32MsgDropCnt++;
  }

  if (mcMsgRx.bDoAgeing())
  {
    lbMsgRxOk = True;
    mstStatus.u32MsgDropCnt++;
  }
}


