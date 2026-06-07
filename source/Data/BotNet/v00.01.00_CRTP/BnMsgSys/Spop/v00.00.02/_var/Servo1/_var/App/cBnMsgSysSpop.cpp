#include "cBnMsgSysSpop.h"
#include "cBotnet.h"
cBotNetMsgPortSpop::cBotNetMsgPortSpop(cBotNet* lcBotNet)
  : cBotNet_MsgSysProcess(lcBotNet)
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

  if (lcMsg.u16GetIdx() == 3)
  {
    u16 lu16SubIdx = lcMsg.GetPayload()[0];
    switch (lu16SubIdx)
    {
      case 0x01: // Enter
        lbConsumed = True;
        {
          if ((lcMsg.GetPayload()[1] == 0x12) &&
              (lcMsg.GetPayload()[2] == 0x34))
          {
            vEnter(lcMsg.cGetDAdr().Get(), lcMsg.cGetDAdr().Get());
          }
          else
          {
            if (!IsSpace()) return False;

            u8 lu8PoolIdx;
            cBotNetMsg_MsgProt lcMsgTx; cBnMsgPool::vReqMsg(lcMsgTx, lu8PoolIdx, 48);
            if (lu8PoolIdx == 0) return False;

            lcMsgTx.cPrepare(3).Set(0x81).Add(0xFE);

            lcMsgTx.vEncode(lcMsg.cGetDAdr(), lcMsg.cGetSAdr());

            u8PutInt(lu8PoolIdx);
            cBnMsgPool::vReleaseMsg(lu8PoolIdx);
          }
        }
        break;
      default: break;
    }
  }
  return lbConsumed;
}

 









