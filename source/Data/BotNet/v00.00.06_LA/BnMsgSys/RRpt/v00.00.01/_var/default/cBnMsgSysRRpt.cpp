#include "cBnMsgSysRRpt.h"
#include "cBotnet.h"


bool cBotNetMsgPortRRpt::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  bool lbConsumed = False;

  if ((lcMsg.mu16Idx == 4) && (lcMsg.mcPayload.Len() == 8))
  {
    switch (lcMsg.mcPayload[0])
    {
      case 1: // Set 1 Kommando
      {
        // Disable
        if (lcMsg.mcPayload[7] == 0) // CT = Cycle Time
        {
          for (u8 lu8i = 0; lu8i < cBotNetMsgPortRRptSlots; lu8i++)
          {
            if (mcSlot[lu8i].isSame(lcMsg))
            {
              mcSlot[lu8i].mu8Enabled = 0;
              mu8SlotCnt--;
            }
          }
        }
        else
        {
          // Enable
          for (u8 lu8i = 0; lu8i < cBotNetMsgPortRRptSlots; lu8i++)
          {
            if (!mcSlot[lu8i].mu8Enabled)
            {
              //                                                                   + 1, weil mcPayload[0] das Kommando ist
              cMemTools::vMemCpy(mcSlot[lu8i].mu8Payload, lcMsg.mcPayload.mpu8Data + 1, (u8)cBotNetMsgPortRRptSlot::cByte::nLast);
              mcSlot[lu8i].mu8Cnt = lcMsg.mcPayload[7]; // CT = Cycle Time
              mcSlot[lu8i].mu8Enabled = 1;
              mu8SlotCnt++;
              break;
            }
          }
        }
        lbConsumed = True;
      }
      break;
    }
  }

  return lbConsumed;
}

void cBotNetMsgPortRRpt::vTick10ms()
{
  if (mu8SlotCnt)
  {
    for (u8 lu8i = 0; lu8i < cBotNetMsgPortRRptSlots; lu8i++)
    {
      if (mcSlot[lu8i].bProcess10ms())
      {
        if (!IsSpace()) return;

        u8 lu8PoolIdx = cBnMsgPool::u8ReqMsg(16);
        if (lu8PoolIdx == 0) return;
        
        cBotNetMsg_Base*     mcMsgBase = null;
        cBotNetMsg_MsgProt2  lcMsgTx(mcMsgBase);

        cBnMsgPool::vGetMsg(lu8PoolIdx, &mcMsgBase);

        lcMsgTx.vPrepare((mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nDH] << 8) +
                          mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nDL],         // DH.DL = Destination Botnet Adr
                          mcBn->mcAdr.Get(),
                          mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nMI]);        // MI = Message Index

        lcMsgTx.mcPayload.Set((u8*)&mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nRI], 3);
        lcMsgTx.vEncode();
        bPutInt(lu8PoolIdx);
        cBnMsgPool::vReleaseMsg(lu8PoolIdx);
      }
    }
  }
}




