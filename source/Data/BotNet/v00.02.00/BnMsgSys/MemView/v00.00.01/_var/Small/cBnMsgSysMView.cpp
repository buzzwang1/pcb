#include "cBnMsgSysMView.h"
#include "cBotnet.h"


bool cBotNetMsgPortMView::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  cBotNetAdress lcTxDAdr;

  u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;
  lcTxDAdr = lcMsg.cGetSAdr();

  switch (lcMsg.u16GetIdx())
  {
    case 7: // Set Msg
      {
        if (isOpen())
        {
          // MemRead
          if ((cMemTools::u32U8toU32(lpu8PayloadRx) >> 8) == 0x040000)
          {
            u32 lu32Adr = cMemTools::u32U8toU32((u8*)(lpu8PayloadRx + 3));
            u8  lu8Size = lpu8PayloadRx[10];

            u8 lau8Resp[43];
            lau8Resp[0] = 4;
            lau8Resp[1] = 0;
            lau8Resp[2] = 0;

            if (lu8Size > 40) lu8Size = 40;
            cMemTools::vMemCpy((u8*)(lau8Resp + 3), (u8*)(lu32Adr), lu8Size);
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, lau8Resp, 3 + lu8Size);
          }
          else
          // MemWrite
          //if ((lpu8PayloadRx[0] == 5) && (lpu8PayloadRx[1] == 0) && (lpu8PayloadRx[2] == 0))
          if ((cMemTools::u32U8toU32(lpu8PayloadRx) >> 8) == 0x050000)
          {
            u32 lu32Adr = cMemTools::u32U8toU32((u8*)(lpu8PayloadRx + 3));
            u8  lu8Size = lcMsg.Len() - 7;

            if (lu8Size != 0)
            {
              const u8 lau8Resp[] = { 0x05, 0x00, 0x00, 0x01 };
              cMemTools::vMemCpy((u8*)lu32Adr, (u8*)(lpu8PayloadRx + 7), lu8Size);
              u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
            }
            else
            {
              const u8 lau8Resp[] = { 0x05, 0x00, 0x00, 0x00 };
              u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
            }
          }
        }

        // Open
        if ((cMemTools::u32U8toU32(lpu8PayloadRx) >> 8) == 0x000000)
        {
          if (cMemTools::u32U8toU32(lpu8PayloadRx + 3) == 0x01020304)
          {
            const u8 lau8Resp[] = { 0x00, 0x00, 0x00, 0x01, 0x01, 0x20, 0x00, 0x00 , 0x00, 0x00, 0x10, 0x00, 0x00 };
            mbOpen = True;
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
          }
          else
          {
            const u8 lau8Resp[] = { 0x00, 0x00, 0x00, 0x00 };
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
          }
        }
        return True; // Consumed
      }
      break;
  }

  return False; // Not Consumed
}

void cBotNetMsgPortMView::vTick10ms()
{
}
