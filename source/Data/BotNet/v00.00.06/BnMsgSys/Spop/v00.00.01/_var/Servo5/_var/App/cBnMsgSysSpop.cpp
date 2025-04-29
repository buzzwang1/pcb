#include "cBnMsgSysSpop.h"
#include "cBotnet.h"



void cBotNetMsgPortSpop::vEnter(uint16 lu16SAdr, uint16 lu16DAdr)
{
  cBnSpop_vWriteBackupData(0, (u32)(('S' << 24) + ('P' << 16) + ('O' << 8) + 'P'));
  cBnSpop_vWriteBackupData(1, (u32)(((u32)lu16SAdr << 16) + (u32)lu16DAdr));
  cBnSpop_vJumpToApp(0);
}

bool cBotNetMsgPortSpop::bMsg(cBotNetMsg_MsgProt& lcMsg, cBotNet* lpcBn)
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
          mcTxMsgTx.vPrepare(lcMsg.mcFrame.mcDAdr.Get(), lcMsg.mcFrame.mcSAdr.Get(), 3);
          mcTxMsgTx.mcPayload.Set(0x81);
          mcTxMsgTx.mcPayload.Add(0xFE);
          mcTxMsgTx.vEncode();
          lpcBn->bSendMsg(&mcTxMsgTx);
        }
      }
      break;
    default: break;
  }
  return lbConsumed;
}

 









