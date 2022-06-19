#include "cBnLinkUsartMpHd.h"
#include "cBotNet.h"

  void cBotNet_UpLinkUsartMpHd::vAddedToBn(cBotNet* lcBotNet)
  {
    mcSAdr.Set(lcBotNet->mcAdr.Get());
    mcDAdr.Set(mcSAdr.GetMasterAdr());
    mAdr  = mcSAdr.GetIdx();

    menSm = enStSyncWait;

    mcUpLink->vSetNode((cComNode*)this);
  }



