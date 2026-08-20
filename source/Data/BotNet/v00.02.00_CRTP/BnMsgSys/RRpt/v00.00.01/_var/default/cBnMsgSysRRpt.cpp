#include "cBnMsgSysRRpt.h"
#include "cBotnet.h"


bool cBotNetMsgPortRRpt::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  bool lbConsumed = False;

  if ((lcMsg.u16GetIdx() == 4) && (lcMsg.GetPayload().Len() == 8))
  {
    switch (lcMsg.GetPayload()[0])
    {
      case 1: // Set 1 Kommando
      {
        // Disable
        if (lcMsg.GetPayload()[7] == 0) // CT = Cycle Time
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
              cMemTools::vMemCpy(mcSlot[lu8i].mu8Payload, lcMsg.GetPayload().mpu8Data + 1, (u8)cBotNetMsgPortRRptSlot::cByte::nLast);
              mcSlot[lu8i].mu8Cnt = lcMsg.GetPayload()[7]; // CT = Cycle Time
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
           
        // Sende Nachricht an mich selbst
        u8PutInt(cBotNetAdress((mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nDH] << 8) +
                                mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nDL]),
                 mcBn->mcAdr,
                 mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nMI],
                 &mcSlot[lu8i].mu8Payload[(u8)cBotNetMsgPortRRptSlot::cByte::nRI], 
                 3);
      }
    }
  }
}




