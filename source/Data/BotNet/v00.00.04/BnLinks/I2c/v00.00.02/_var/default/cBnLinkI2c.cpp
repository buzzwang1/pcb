#include "cBnLinkI2c.h"
#include "cBotNet.h"


void cBotNet_UpLinkI2c::vAddedToBn(cBotNet* lcBotNet, cBotNet_LinkBase::tenType lenType)
{
  UNUSED(lenType);
  cBotNet_ComLinkI2c::vAddedToBn(lcBotNet, cBotNet_LinkBase::enUpLink);

  mcSAdr.Set(lcBotNet->mcAdr.Get());
  mcDAdr.Set(mcSAdr.GetMasterAdr());

  mAdrSync = mcSAdr.GetIdx() << 1;
  mAdrData = mAdrSync + (0x20 << 1);
  mAdrAck  = mAdrSync + (0x40 << 1);

  mcUpLink->vSetNode((cComNode*)this);
}
