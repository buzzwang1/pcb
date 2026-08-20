#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcStreamSys(this),
  mszName(mszBufName, 0, 16)
{
  mcCfg = lcCfg;

  mcpMsgProcess   = null;

  mstStatus.u32MsgCnt     = 0;
  mstStatus.u32MsgDropCnt = 0;

  mcUpLink   = null;

  mStartupDelayCnt = 100;  // [ms]

  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet() {}






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
  // Nicht interrupts getrieben Uplinks, wie USB, brauchen einen Sync
  if (lpcLink != null)
  {
    if ((lpcLink->IsInitAndOnline()) || (lbForceSync))
    {
      lpcLink->vSync();
    }
  }
}


void cBotNet::vLinkMisc10ms()
{
  /* Startup Delay in ms */
  /* Nachrichten verarbeitung fängt erst dann an, aber links sind schon online */
  if (mStartupDelayCnt > 10) mStartupDelayCnt -= 10;
                        else mStartupDelayCnt  = 0;

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
    // Weiterleiten an Master
    vForwardMsgToLink(mcUpLink, lu8PoolIdx);
  }
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


void cBotNet::vProcess(u32 lu32DiffTime_us)
{
  mu32Time_us += lu32DiffTime_us;

  while (mu32Time_us >= 1000)
  {
    mu32Time_us -= 1000;
    mu32Time_ms++;

    vLinkSync(mcUpLink, True);
    vProcessData();

    if (mu32Time_ms >= 10)
    {
      mu32Time_ms = 0;
      vLinkMisc10ms();

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


