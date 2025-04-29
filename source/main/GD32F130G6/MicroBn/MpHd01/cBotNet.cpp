#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess *lcpMsgProcess)
 :mcAdr(lcCfg->mu16BnAdr),
  mcUpLink(lcCfg->mu16BnAdr, 0, &mcUartMpHdU0),
  mcStreamSys(this),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcMsgTxDyn(cBotNet_MsgSize), mcMsgTx(&mcMsgTxDyn),
  mszName(mszBufName, 16)
{
  mcCfg = lcCfg;
  mcpMsgProcess   = lcpMsgProcess;

  mszName = (const char8*)lcCfg->mszName;
}

cBotNet::~cBotNet() {}


void cBotNet::vStreamPortConnect(u8 lu8SourcePortIdx, u16 lu8DestPortAdr, u8 lu8DestPortIdx)
{
  mcStreamSys.ConnectPort(lu8SourcePortIdx, lu8DestPortAdr, lu8DestPortIdx);
}

void cBotNet::vSetName(const char8* lszName)
{
  mszName = lszName;
}

// Muss alle 1ms aufgerufen werden
void cBotNet::vLinkSyncAll1ms()
{
  mcUpLink.vSync();
  mcUpLink.vTick1ms();
}

void cBotNet::vLinkMisc10ms()
{
  u16 lu16t;

  mcUpLink.vTick10ms();
  for (lu16t = 0; lu16t < cBotNet_StreamSysPortsCnt; lu16t++)
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


void cBotNet::vDecodeMsgType(cBotNetMsg_Base *lcMsg)
{
  switch (lcMsg->enGetMsgType())
  {
    case cBotNetMsgFrame::tenType::enMessage:
    {
      cBotNetMsg_MsgProt lcMsgProt(lcMsg);
      lcMsgProt.vDecode(mcAdr.Get(), mcAdr.Get());
      if (lcMsgProt.mu16Idx > 63)
      {
        mcpMsgProcess->vMsg(lcMsgProt, this);
      }
    }
    break;

    case cBotNetMsgFrame::tenType::enStream:
      if (mcStreamSys.mcRxComBuf.isFitting())
      {
        mcStreamSys.put(lcMsg);
      }
      break;

    case cBotNetMsgFrame::tenType::enPacket: // Noch nicht implementiert
      break;

    case cBotNetMsgFrame::tenType::enMultiMessage: // Noch nicht implementiert
      break;
  }
}


void cBotNet::vForwardMsgToLink(cBotNet_LinkBase* lpcLink, cBotNetMsg_Base* lpcMsg)
{
  if (lpcLink)
  {
    lpcLink->put(lpcMsg);
  }
}

void cBotNet::vDecodeAndDispatch(cBotNetMsg_Base* lcMsg)
{
  lcMsg->vDecode(mcAdr.Get(), mcAdr.Get());

  if (mcAdr.isMe(lcMsg->cGetDAdr()))
  {
    // Nachricht für mich
    vDecodeMsgType(lcMsg);
  }
  else
  {
    // Weiterleiten an Master
    if (!mcAdr.isMasterOf(lcMsg->cGetDAdr()))
    {
      mcUpLink.put(lcMsg);
    }
  }
}

void cBotNet::vSendMsg(cBotNetMsg_Base* lcMsg)
{
  vForwardMsgToLink(&mcUpLink, lcMsg);
}


void cBotNet::vForwardTxMsg()
{
  vForwardMsgToLink(&mcUpLink, &mcMsgTx);
}

void cBotNet::vResetCom(bool lbAlways)
{
  mcUpLink.vResetCom(lbAlways);
}


void cBotNet::vProcess10ms()
{
  // UpLink
  do
  {
    //Neue Nachricht holen
    mcUpLink.u16Get(&mcMsgRx);

    if (mcMsgRx.Len() > 0)
    {
      //Wird ok, wenn Nachricht zugeteilt wurde
      vDecodeAndDispatch(&mcMsgRx);
    }
    else
    {
      break;
    }
  }
  while(1);


  mcStreamSys.vProcess();
  do
  {
    mcStreamSys.get(&mcMsgTx);

    if (mcMsgTx.Len() > 0)
    {
      vDecodeAndDispatch(&mcMsgTx);
    }
    else
    {
      break;
    }
  }
  while (1);
}


