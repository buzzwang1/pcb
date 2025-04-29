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
              mcSlot[lu8i].mcMsg.CopyFrom(lcMsg);
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

void cBotNetMsgPortRRpt::vProcess10ms()
{
  if (mu8SlotCnt)
  {
    for (u8 lu8i = 0; lu8i < cBotNetMsgPortRRptSlots; lu8i++)
    {
      if (mcSlot[lu8i].bProcess10ms())
      {
        // Sende Nachricht an mich selbst
        u8 lu8Data[3];
        mcMsgTx.vPrepare((mcSlot[lu8i].mcMsg.mcPayload[5] << 8) + mcSlot[lu8i].mcMsg.mcPayload[6], // DH.DL = Destination Botnet Adr
                          mcBn->mcAdr.Get(),
                          mcSlot[lu8i].mcMsg.mcPayload[1]);                                        // MI = Message Index

        lu8Data[0] = mcSlot[lu8i].mcMsg.mcPayload[2]; // R1
        lu8Data[1] = mcSlot[lu8i].mcMsg.mcPayload[3]; // S1
        lu8Data[2] = mcSlot[lu8i].mcMsg.mcPayload[4]; // S2

        mcMsgTx.mcPayload.Set(lu8Data, sizeof(lu8Data));
        mcMsgTx.vEncode();
        mcBn->bCallMsgHandler(mcMsgTx);
      }
    }
  }
}




