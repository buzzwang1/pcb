#include "cBnLinkBase.h"
#include "cBotnet.h"


cBotNet_LinkBase::cBotNet_LinkBase(uint16 lui16Adr, u32 lu32RxComBufSize, u32 lu32TxComBufSize)
  : mcRxComBuf(lu32RxComBufSize), mcTxComBuf(lu32TxComBufSize)
{
  mcAdr.Set(lui16Adr);

  mu8MsgCntTx      = 0;
  mu8MsgCntRx      = 0;
  mu8MsgCntRx_Last = 0xFF;

  mcBotnet = null;
}

void cBotNet_LinkBase::vAddedToBn(cBotNet* lcBotNet, cBotNet_LinkBase::tenType lenType)
{
  mcBotnet = lcBotNet;

  switch(lenType)
  {
    case enUpLink:
    case enSideLink:
      // Master/Slave Uplink
      // d.h. ich bin Slave und Dest. Der Master is Source
      mcDAdr = mcBotnet->mcAdr;
      mcSAdr = mcAdr;
    break;
    case enDownLink:
      // Master/Slave Downlink
      // d.h. ich bin Slave und Dest. Der Master is Source
      mcDAdr = mcAdr;
      mcSAdr = mcBotnet->mcAdr;
    break;
    case enOpenLink:
    default:
    break;
  }
}
