#include "cBotNetSwitch.h"
#include "cBotNet.h"

cBotNetSwitch::cBotNetSwitch(cBotNetCfg* lcCfg, cBotNet_MsgSysProcess *lcpMsgProcess)
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


uint32 cBotNetSwitch::SizeOf()
{
  uint32 lui32Size;

  lui32Size = sizeof(cBotNetSwitch);
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

bool cBotNetSwitch::bSetUpLink(cBotNet_LinkBase* lcLink)
{
  if (lcLink != null)
  {
    mcUpLink = lcLink;
    lcLink->vAddedToBn((cBotNet*)this, cBotNet_LinkBase::enUpLink);
    return True;
  }
  return False;

}

bool cBotNetSwitch::bSetDownLink(cBotNet_LinkBase* lcLink)
{
  if (lcLink != null)
  {
    mcDownLink = lcLink;
    lcLink->vAddedToBn((cBotNet*)this, cBotNet_LinkBase::enDownLink);
    return True;
  }
  return False;
}


void cBotNetSwitch::vSync()
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
}

void cBotNetSwitch::vTick10ms()
{
  if (mcDownLink != null) mcDownLink->vTick10ms();
  if (mcUpLink != null) mcUpLink->vTick10ms();
}



void cBotNetSwitch::vDecode()
{
  mcMsgRx.vDecode(mcAdr.Get(), mcAdr.Get());

  if (mcMsgRx.enGetMsgType() == cBotNetMsgFrame::tenType::enMessage)
  {
    cBotNetMsg_MsgProt lcMsg(&mcMsgRx);
    lcMsg.vDecode(mcAdr.Get(), mcAdr.Get());
    mcpMsgProcess->vMsg(lcMsg, (cBotNet*)this);
  }

  mcDownLink->bPut(&mcMsgRx);
}


void cBotNetSwitch::vProcess()
{
  // UpLink
  if (mcUpLink)
  {
    while (1)
    {
      if (mcUpLink->u16Get(&mcMsgRx))
      {
        vDecode();
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

