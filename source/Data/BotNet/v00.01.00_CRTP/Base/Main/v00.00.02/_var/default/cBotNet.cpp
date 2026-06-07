#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcStreamSys(this),
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

    vLinkDynSyncReset(lu8t);
  }

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
void cBotNet::vLinkSyncAllTick1ms()
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

  mcStreamSys.vTick10ms();
}


void cBotNet::vDecodeMsgType(u8 &lu8PoolIdx)
{
  cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, lu8PoolIdx);

  switch (lcMsg.enGetMsgType())
  {
    case cBotNetMsg_Base::tenType::enMessage:
    {
      cBotNetMsg_MsgProt lcMsgProt(lcMsg);

      cBotNet_MsgSysProcess* mcpMsgHandlerWork = mcpMsgProcess;
      while (mcpMsgHandlerWork != null)
      {
        if (mcpMsgHandlerWork->bMsg(lcMsgProt)) break;
        mcpMsgHandlerWork = mcpMsgHandlerWork->mcMsgSysNext;
      }

      cBnMsgPool::vDeleteMsg(lu8PoolIdx);
    }
    break;

    case cBotNetMsg_Base::tenType::enStream:
      if (mcStreamSys.bPut(lu8PoolIdx))
      {
        cBnMsgPool::vReleaseMsg(lu8PoolIdx);
        lu8PoolIdx = 0;
      }
      break;

    case cBotNetMsg_Base::tenType::enPacket: // Noch nicht implementiert
      vDropMsg(lu8PoolIdx);
      break;

    case cBotNetMsg_Base::tenType::enMultiMessage: // Noch nicht implementiert
      vDropMsg(lu8PoolIdx);
      break;
  }
}


void cBotNet::vForwardMsgToLink(cBotNet_LinkBase* lpcLink, u8 &lu8PoolIdx)
{
  if (lpcLink)
  {
    lpcLink->bPut(lu8PoolIdx);
    cBnMsgPool::vReleaseMsg(lu8PoolIdx);
    lu8PoolIdx = 0;
  }
  else
  {
    //Wenn Link nicht da ist, dann konsumieren
    vDropMsg(lu8PoolIdx);
  }
}

void cBotNet::vSendMsg(u8& lu8PoolIdx)
{
  cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, lu8PoolIdx);

  if (mcAdr.isMe(lcMsg.cGetDAdr()))
  {
    // Nachricht für mich
    vDecodeMsgType(lu8PoolIdx);
  }
  else
  {
    // Ist die Destination der SideLink ?
    if ((mcSideLink->mcAdr.isMe(lcMsg.cGetDAdr())) ||
        (mcSideLink->mcAdr.isMasterOf(lcMsg.cGetDAdr())))
    {
      // Weiterleiten an SideLink
      vForwardMsgToLink(mcSideLink, lu8PoolIdx);
    }
    else
    {
      // Ist die Destination ein Slave von mir ?
      if (mcAdr.isMasterOf(lcMsg.cGetDAdr()))
      {
        // Weiterleiten an Slave
        u8 lu8SlaveIdx = mcAdr.u8GetNextLevelSlaveIdx(lcMsg.cGetDAdr().Get());
        if (lu8SlaveIdx > 0)
        {
          vLinkDynSyncReset(lu8SlaveIdx - 1);
          vForwardMsgToLink(mcDownLinks[lu8SlaveIdx - 1], lu8PoolIdx);
        }
        // Nachricht mit schlechter Adresse ->konsumieren
      }
      else
      {
        // Weiterleiten an Master
        vForwardMsgToLink(mcUpLink, lu8PoolIdx);
      }
    }
  }
}


void cBotNet::vSendMsg(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u16 liIdx, u8* lpuData, u8 luSize)
{
  u8 lu8PoolIdx;
  cBotNetMsg_MsgProt lcMsgTx;  cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, luSize + 10);
  if (lu8PoolIdx == 0) return;

  lcMsgTx.cPrepare(liIdx).Set(lpuData, luSize);
  lcMsgTx.vEncode(lcSAdr, lcDAdr);

  cBnMsgPool::vSetLen(lu8PoolIdx, lcMsgTx.muLen);
  vSendMsg(lu8PoolIdx);
  cBnMsgPool::vReleaseMsg(lu8PoolIdx);
}


void cBotNet::vDropMsg(u8& lu8PoolIdx)
{
  cBnMsgPool::vDeleteMsg(lu8PoolIdx);
  mstStatus.u32MsgDropCnt++;
}

void cBotNet::vCheckMsg(u8 &lu8PoolIdx)
{
  cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, lu8PoolIdx);

  // Echo-Unterdrückung
  if (mcAdr.isMe(lcMsg.cGetSAdr()))
  {
    vDropMsg(lu8PoolIdx);
  }
}


void cBotNet::vProcessData()
{
  if (mStartupDelayCnt) return;

  // UpLink
  if (mcUpLink)
  {
    if (mu8PoolIdx == 0)
    {
      //Neue Nachricht holen
      mu8PoolIdx = mcUpLink->u8Get();
      if (mu8PoolIdx > 0)
      {
        vCheckMsg(mu8PoolIdx);
      }
    }
  }

  // SideLink
  if (mcSideLink)
  {
    if (mu8PoolIdx == 0)
    {
      //Neue Nachricht holen
      mu8PoolIdx = mcSideLink->u8Get();
      if (mu8PoolIdx > 0)
      {
        vCheckMsg(mu8PoolIdx);
      }
    }
  }


  // DownLink
  // Alle Sklaven überprüfen
  if (mu8PoolIdx == 0)
  {
    for (u8 lu8t = 0; lu8t < mu8DownLinkCnt; lu8t++)
    {
      mcDownLinkActive = mcDownLinks[lu8t];
      if (mcDownLinkActive->IsOnline())
      {
        //Neue Nachricht holen
        mu8PoolIdx = mcDownLinkActive->u8Get();
        if (mu8PoolIdx > 0)
        {
          vCheckMsg(mu8PoolIdx);
        }
        if (mu8PoolIdx > 0) break;
      }
    }
  }

  // MsgSys
  if (mu8PoolIdx == 0)
  {
    cBotNet_MsgSysProcess* mcpMsgHandlerWork = mcpMsgProcess;
    while (mcpMsgHandlerWork != null)
    {
      mu8PoolIdx = mcpMsgHandlerWork->u8Get();
      if (mu8PoolIdx > 0) break;
      mcpMsgHandlerWork = mcpMsgHandlerWork->mcMsgSysNext;
    }
  }

  // StreamSys
  if (mu8PoolIdx == 0)
  {
    mu8PoolIdx = mcStreamSys.u8Get();
  }

  if (mu8PoolIdx > 0)
  {
    vSendMsg(mu8PoolIdx);
    if (mu8PoolIdx == 0) mstStatus.u32MsgCnt++;
  }
}

void cBotNet::vMessageAgeing10ms()
{
  // Timeout-Handling für Nachrichten
  // Falls Nachrichten nicht zugestellt werden können, dann konsumieren.
  //if (mcMsgTx.bDoAgeing())
  //{
  //  mbMsgTxOk = True;
  //  mstStatus.u32MsgDropCnt++;
  //}
  //
  //if (mcMsgRx.bDoAgeing())
  //{
  //  mbMsgRxOk = True;
  //  mstStatus.u32MsgDropCnt++;
  //}
}

void cBotNet::vProcess(u32 lu32DiffTime_us)
{
  mu32Time_us += lu32DiffTime_us;

  while (mu32Time_us >= 1000)
  {
    mu32Time_us -= 1000;
    mu32Time_ms++;

    vProcessData();
    vLinkSyncAllTick1ms();

    if (mu32Time_ms >= 10)
    {
      mu32Time_ms = 0;
      vLinkMisc10ms();
      vMessageAgeing10ms();

      cBotNet_MsgSysProcess* mcpMsgHandlerWork = mcpMsgProcess;
      while (mcpMsgHandlerWork != null)
      {
        mcpMsgHandlerWork->vTick10ms();
        mcpMsgHandlerWork = mcpMsgHandlerWork->mcMsgSysNext;
      }

      mcStreamSys.vProcess();
    }
  }
}


