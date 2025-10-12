#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcMsgRx(mu8MsgRxBuf, sizeof(mu8MsgRxBuf)),
  mcMsgTx(mu8MsgTxBuf, sizeof(mu8MsgTxBuf)),
  mszName(mszBufName, 0, 16)
{

  mcCfg = lcCfg;

  mcpMsgProcess   = null;

  mstStatus.u32MsgCnt     = 0;
  mstStatus.u32MsgDropCnt = 0;

  mcUpLink   = null;

  mStartupDelayCnt = 100;  // [ms]

  mcDownLink = null;

  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet() {}


uint32 cBotNet::SizeOf()
{
  return 0;
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
  vLinkSync(mcUpLink, False);
  vLinkSync(mcDownLink, False);

  if (mcUpLink   != null) mcUpLink->vTick1ms();
  if (mcDownLink != null) mcDownLink->vTick1ms();
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

  if (mcUpLink   != null) mcUpLink->vTick10ms();
  if (mcDownLink != null) mcDownLink->vTick10ms();
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


bool cBotNet::bDecodeAndDispatch()
{
  mcMsgRx.vDecode(mcAdr.Get(), mcAdr.Get());

  if (mcMsgRx.enGetMsgType() == cBotNetMsgFrame::tenType::enMessage)
  {
    cBotNetMsg_MsgProt lcMsg(&mcMsgRx);
    lcMsg.vDecode(mcAdr.Get(), mcAdr.Get());
    bCallMsgHandler(lcMsg);
  }

  mcDownLink->bPut(&mcMsgRx);
  return True;
}

bool cBotNet::bSendMsg(cBotNetMsg_Base* lcMsg)
{
  UNUSED(lcMsg);
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
    while (1)
    {
      if (mcUpLink->u16Get(&mcMsgRx))
      {
        bDecodeAndDispatch();
      }
      else
      {
        break;
      }
    }
  }

  // mcDownLink
  if (mcDownLink)
  {
    while (1)
    {
      if (mcDownLink->u16Get(&mcMsgRx))
      {
        mcUpLink->bPut(&mcMsgRx);
      }
      else
      {
        break;
      }
    }
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
    }
  }
}


