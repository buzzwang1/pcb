#include "cBotNet.h"
#include "cBotNetBlu.h"

cBotNetBlu::cBotNetBlu(cBotNetCfg* lcCfg)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcStreamSys((cBotNet*)this),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcMsgTxDyn(cBotNet_MsgSize), mcMsgTx(&mcMsgTxDyn)
{
  mcCfg = lcCfg;
  mu16DownLinkSyncCnt = 0;

  mstStatus.u32MsgCnt     = 0;
  mstStatus.u32MsgDropCnt = 0;

  mcSideLink = null;

  mStartupDelayCnt = 100;  // [ms]
}

cBotNetBlu::~cBotNetBlu()
{}


uint32 cBotNetBlu::SizeOf()
{
  return 0;
}

bool cBotNetBlu::bAddLink(cBotNet_LinkBase* lcLink)
{
  if (lcLink != null)
  {
    lcLink->vAddedToBn((cBotNet*)this, cBotNet_LinkBase::enSideLink);
    mcSideLink = lcLink;
  }
  return False;
}


u16 cBotNetBlu::u16StreamPortAdd(cBotNetStreamPort* lcPort)
{
  return mcStreamSys.AddPort(lcPort);
}

void cBotNetBlu::vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx)
{
  mcStreamSys.ConnectPort(lu8SourcePortIdx, lu8DestPortAdr, lu8DestPortIdx);
}


void cBotNetBlu::vLinkSync(cBotNet_LinkBase* lpcLink, bool lbForceSync)
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
void cBotNetBlu::vLinkSyncAll1ms()
{
  bool lbSyncAll;

  // Es werden nur alle 100 Versuche alle Slaves ge'sync't
  // Sonst nur die aktiven
  if (!mu16DownLinkSyncCnt)
  {
    lbSyncAll = True;
    mu16DownLinkSyncCnt = cBotNet_DownLinkSyncAll;
  }
  else
  {
    lbSyncAll = False;
    mu16DownLinkSyncCnt--;
  }

  vLinkSync(mcSideLink, lbSyncAll);
}


void cBotNetBlu::vLinkMisc10ms()
{
  u16 lu16t;

  /* Startup Delay in ms */
  /* Nachrichten verarbeitung fängt erst dann an, aber links sind schon online */
  if (mStartupDelayCnt > 10) mStartupDelayCnt -= 10;
                        else mStartupDelayCnt  = 0;


  if (mcSideLink != null) mcSideLink->vTick10ms();

  for (lu16t = 0; lu16t < mcStreamSys.mu8PortCnt; lu16t++)
  {
    mcStreamSys.mcPorts[lu16t]->vTick10ms();
  }
}


void cBotNet::vTickHp1ms()
{
  vLinkSyncAll1ms();
}

void cBotNet::vTickLp10ms()
{
  vProcess10ms();
  vLinkMisc10ms();
}


bool cBotNetBlu::bDecodeMsgType(cBotNetMsg_Base *lcMsg)
{
  bool lbMsgConsumed = False;

  switch (lcMsg->enGetMsgType())
  {
    case cBotNetMsgFrame::tenType::enMessage:
      lbMsgConsumed = True;
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


bool cBotNetBlu::bForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg)
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

bool cBotNetBlu::bDecodeAndDispatch()
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
    if (mcSideLink->mcDAdr.isMe(mcMsgRx.cGetDAdr()))
    {
      // Weiterleiten an SideLink
      return bForwardMsgToLink(mcSideLink, &mcMsgRx);
    }
  }

  // Nachricht konnte zugeteil werden
  // => Nachricht konsumieren => return True
  return True;
}


bool cBotNetBlu::bForwardTxMsg()
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
  }

  // Nachricht konnte zugeteil werden
  // => Nachricht konsumieren => return True
  return True;
}

void cBotNetBlu::vResetCom(bool lbAlways)
{
  if (mStartupDelayCnt) return;

  // SideLink
  if (mcSideLink)
  {
    mcSideLink->vResetCom(lbAlways);
  }
}


void cBotNetBlu::vProcess10ms()
{
  static bool lbMsgTxOk = True;
  static bool lbMsgRxOk = True;

  if (mStartupDelayCnt) return;


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
        if (lbMsgRxOk) mstStatus.u32MsgCnt++;
      }
      else
      {
        break;
      }
    }
    while (lbMsgRxOk);
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


