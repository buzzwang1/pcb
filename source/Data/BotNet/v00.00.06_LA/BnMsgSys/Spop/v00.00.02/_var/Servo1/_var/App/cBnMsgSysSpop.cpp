#include "cBnMsgSysSpop.h"
#include "cBotnet.h"
cBotNetMsgPortSpop::cBotNetMsgPortSpop(cBotNet* lcBotNet)
  : cBotNet_MsgSysProcess(lcBotNet),
    mcTxMsgTx_BAry(mu8MsgTx_Buf, cBotNet_MsgSize),
    mcTxMsgTx(&mcTxMsgTx_BAry)
{
}



void cBotNetMsgPortSpop::vEnter(uint16 lu16SAdr, uint16 lu16DAdr)
{
  cBnSpop_vWriteBackupData(0, (u32)(('S' << 24) + ('P' << 16) + ('O' << 8) + 'P'));
  cBnSpop_vWriteBackupData(1, (u32)(((u32)lu16SAdr << 16) + (u32)lu16DAdr));
  cBnSpop_vJumpToApp(0);
}

bool cBotNetMsgPortSpop::bMsg(cBotNetMsg_MsgProt& lcMsg) 
{
  bool lbConsumed = False;

  u16 lu16SubIdx = lcMsg.mcPayload[0];
  switch (lu16SubIdx)
  {
    case 0x01: // Enter
      lbConsumed = True;
      {
        if ((lcMsg.mcPayload.mpu8Data[1] == 0x12) &&
            (lcMsg.mcPayload.mpu8Data[2] == 0x34))
        {
          vEnter(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get());
        }
        else
        {
          if (!IsSpace()) return False;

          u8 lu8PoolIdx = cBnMsgPool::u8ReqMsg(48);
          if (lu8PoolIdx == 0) return False;

          cBotNetMsg_Base* mcMsgBase = null;
          cBotNetMsg_MsgProt2  lcMsgTx(mcMsgBase);

          cBnMsgPool::vGetMsg(lu8PoolIdx, &mcMsgBase);

          lcMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          lcMsgTx.mcPayload.Set(0x81);
          lcMsgTx.mcPayload.Add(0xFE);
          lcMsgTx.vEncode();
          bPutInt(lu8PoolIdx);
          cBnMsgPool::vReleaseMsg(lu8PoolIdx);
        }
      }
      break;
    default: break;
  }
  return lbConsumed;
}

 









