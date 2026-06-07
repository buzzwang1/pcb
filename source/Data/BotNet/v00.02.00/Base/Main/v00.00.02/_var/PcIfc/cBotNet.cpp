#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mszName(mszBufName, 0, 16)
{

  mcCfg = lcCfg;

  mcpMsgProcess   = null;

  mstStatus.u32MsgCnt     = 0;
  mstStatus.u32MsgDropCnt = 0;

  mcUpLink   = null;

  mStartupDelayCnt = 100;  // [ms]

  mcDownLink = null;
  vLinkDynSyncReset(0);

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
        mcDownLink = lcLink;
        return lcLink->bAddedToBn(mcAdr.GetSlaveAdr(0));
        break;
      default:
        break;
    }
  }
  return False;
}


void cBotNet::vSetName(const char8* lszName)
{
  mszName = lszName;
}

void cBotNet::vLinkDynSyncReset(u16 lu16Idx)
{
  UNUSED(lu16Idx);
  mcDownLinkDynSyncCnt       = 0;
  mcDownLinkDynSyncReload    = 1;
  mcDownLinkDynSyncReloadCnt = 1;
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
  if (mcDownLinkDynSyncCnt > 0)
  {
    mcDownLinkDynSyncCnt--;
  }

  if (mcDownLinkDynSyncCnt == 0)
  {
    mcDownLinkDynSyncCnt = mcDownLinkDynSyncReload;
    vLinkSync(mcDownLink, False);
  }

  vLinkSync(mcUpLink, False);
}



void cBotNet::vLinkMisc10ms()
{
  /* Startup Delay in ms */
  /* Nachrichten verarbeitung fängt erst dann an, aber links sind schon online */
  if (mStartupDelayCnt > 10) mStartupDelayCnt -= 10;
                        else mStartupDelayCnt  = 0;

  //Dynamische Lastanpassung.
  if (mcDownLinkDynSyncReloadCnt > 0)
  {
    mcDownLinkDynSyncReloadCnt--;
  }
  else
  {
    if (mcDownLinkDynSyncReload < 64)
    {
      mcDownLinkDynSyncReload *= 2;
    }
  }
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
        if (mcpMsgHandlerWork->bMsg(lcMsgProt))
        {
          cBnMsgPool::vDeleteMsg(lu8PoolIdx);
          break;
        }
        mcpMsgHandlerWork = mcpMsgHandlerWork->mcMsgSysNext;
      }
    }
    break;
    default:
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

  vDecodeMsgType(lu8PoolIdx);

  if (lu8PoolIdx)
  {
    vForwardMsgToLink(mcDownLink, lu8PoolIdx);
	vLinkDynSyncReset(0);
  }

  if (lu8PoolIdx)
  {
    cBnMsgPool::vDeleteMsg(lu8PoolIdx);
  }
}


void cBotNet::vSendMsg(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u16 liIdx, u8* lpuData, u8 luSize)
{
  u8 lu8PoolIdx;
  cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, luSize + 10);
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
        vSendMsg(mu8PoolIdx);
      }
    }
  }

  // DownLink
  if (mcDownLink)
  {
    if (mu8PoolIdx == 0)
    {
      //Neue Nachricht holen
      mu8PoolIdx = mcDownLink->u8Get();
      if (mu8PoolIdx > 0)
      {
        vForwardMsgToLink(mcUpLink, mu8PoolIdx);
      	vLinkDynSyncReset(0);
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
      if (mu8PoolIdx > 0)
      {
        vForwardMsgToLink(mcUpLink, mu8PoolIdx);
        break;
      }
      mcpMsgHandlerWork = mcpMsgHandlerWork->mcMsgSysNext;
    }
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

      //BotNet_MsgSysProcess* mcpMsgHandlerWork = mcpMsgProcess;
      //hile (mcpMsgHandlerWork != null)
      //
      // mcpMsgHandlerWork->vTick10ms();
      // mcpMsgHandlerWork = mcpMsgHandlerWork->mcMsgSysNext;
      //
    }
  }
}


