#include "cBotNet.h"

cBotNet::cBotNet(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess *lcpMsgProcess)
 :mcAdr(lcCfg->mu16BnAdr),
  mcDeviceID(lcCfg->mu16BnDeviceID),
  mcMsgRxDyn(cBotNet_MsgSize), mcMsgRx(&mcMsgRxDyn),
  mcMsgTxDyn(cBotNet_MsgSize), mcMsgTx(&mcMsgTxDyn)
{
  mcCfg = lcCfg;
  mcpMsgProcess   = lcpMsgProcess;

  //mcMsgTx.mstHeader.cDAdr.Set(lu16Adr);
  //mcMsgRx.mstHeader.cDAdr.Set(lu16Adr);

  mcUpLink   = null;
  mcDownLink = null;
}

cBotNet::~cBotNet()
{}

uint32 cBotNet::SizeOf()
{
  uint32 lui32Size;

  lui32Size = sizeof(cBotNet);
  lui32Size -= 2 * sizeof(cBotNetMsg_BaseDyn);

  lui32Size += mcDownLink->SizeOf();

  lui32Size += mcMsgRxDyn.SizeOf();
  lui32Size += mcMsgTxDyn.SizeOf();

  if (mcUpLink)
  {
    lui32Size += mcUpLink->SizeOf();
  }


  return lui32Size;
}


bool cBotNet::bSetUpLink(cBotNet_LinkBase* lcLink, u16 lu16Adr)
{
  if (lcLink != null)
  {
    mcUpLink = lcLink;
    return lcLink->bAddedToBn(lu16Adr);
  }
  return False;
}

bool cBotNet::bSetDownLink(cBotNet_LinkBase* lcLink, u16 lu16Adr)
{
  if (lcLink != null)
  {
    mcDownLink = lcLink;
    return lcLink->bAddedToBn(lu16Adr);
  }
  return False;
}


// Muss alle 1ms aufgerufen werden
void cBotNet::vLinkSyncAll1ms()
{
  if (mcDownLink != null)
  {
    if (mcDownLink->IsInitAndOnline())
    {
      mcDownLink->vSync();
    }
  }

  if (mcUpLink != null)
  {
    if (mcUpLink->IsInitAndOnline())
    {
      mcUpLink->vSync();
    }
  }

  
  // Wird weitergeleite an 1ms Tick der HW-Schnittstelle...
  // ...und bStartNext()
  // Weil alle DownLinks die gleiche HW-Schnittstelle nutzen, 
  // wird es nur einmal für DownLink aufgerufen
  if (mcDownLink != null) mcDownLink->vTick1ms();
  
  if (mcUpLink   != null) mcUpLink->vTick1ms();
}

void cBotNet::vLinkMisc10ms()
{
  if (mcDownLink != null) mcDownLink->vTick10ms();
  if (mcUpLink != null) mcUpLink->vTick10ms();
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


bool cBotNet::bDecodeMsgType(cBotNetMsg_Base *lcMsg)
{
  bool lbMsgConsumed = False;

  switch (lcMsg->enGetMsgType())
  {
    case cBotNetMsgFrame::tenType::enMessage:
    //case cBotNetMsgFrame::tenType::enRMessage:
    {
      cBotNetMsg_MsgProt lcMsgProt(lcMsg);
      lcMsgProt.vDecode(mcAdr.Get(), mcAdr.Get());
      
      mcpMsgProcess->bMsg(lcMsgProt, this);
      
      lbMsgConsumed = True;
    }
    break;

    case cBotNetMsgFrame::tenType::enStream:
      lbMsgConsumed = True;
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

  if (mcMsgRx.enGetMsgType() == cBotNetMsgFrame::tenType::enMessage)
  {
    cBotNetMsg_MsgProt lcMsg(&mcMsgRx);
    lcMsg.vDecode(mcAdr.Get(), mcAdr.Get());
    mcpMsgProcess->bMsg(lcMsg, (cBotNet*)this);
  }

  mcDownLink->bPut(&mcMsgRx);
  return True;
}


void cBotNet::vProcess10ms()
{
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

  // DownLink
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


