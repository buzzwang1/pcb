#include "cBnStreamSys.h"
#include "cBotnet.h"

cBotNetStreamSystem::cBotNetStreamSystem(cBotNet* lcBn)
  : mcRxComBuf(mcRxComBufBuf, sizeof(mcRxComBufBuf)), mcTxComBuf(mcTxComBufBuf, sizeof(mcTxComBufBuf)),
    mcCmdPort(lcBn)
{
  mcBotNet = lcBn;
  mcAdr.Set(lcBn->mcAdr.Get());

  mcPorts[0] = &mcCmdPort;
  mcCmdPort.vAddedToBn(lcBn);
}




