#include "cBnStreamSys.h"
#include "cBotnet.h"

cBotNetStreamSystem::cBotNetStreamSystem(cBotNet* lcBn)
  : mcRxComBuf(cBotNet_StreamSysComBufSize), mcTxComBuf(cBotNet_StreamSysComBufSize),
    mcMsgRx(cBotNet_MsgSize), mcMsgStreamRx(&mcMsgRx),
    mcMsgTx(cBotNet_MsgSize), mcMsgStreamTx(&mcMsgTx),
    mcCmdEcho(),
    mcMemPort(RomConst_Partition_Count, RomConst_stDevice_Info->stPartitions)
{
  mcBotNet = lcBn;
  mcAdr.Set(lcBn->mcAdr.Get());

  mcPorts[0] = &mcCmdEcho;
  mcCmdEcho.vAddedToBn(lcBn);  
	
  mcPorts[1] = &mcMemPort;
  mcMemPort.vAddedToBn(lcBn);
}




