#include "cBnMsgSysMView.h"
#include "cBotnet.h"


bool cBotNetMsgPortMView::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  if (lcMsg.u16GetIdx() != 5)
    return false;

  u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;
  cBotNetAdress lcTxDAdr = lcMsg.cGetSAdr();

  if (isOpen())
  {
    switch (lcMsg.u32GetId())
    {
      case lcMsg.cId24(0x04, 0x00, 0x00): // MemRead
      {
        u32 lu32Adr = cMemTools::u32U8toU32((u8*)(lpu8PayloadRx + 3));
        u8  lu8Size = lpu8PayloadRx[10];

        u8 lau8Resp[43];
        cBotNetMsg_MsgProt::vRespId(4, 0, 0, lau8Resp);

        if (lu8Size > 40) lu8Size = 40;
        cMemTools::vMemCpy((u8*)(lau8Resp + 3), (u8*)(lu32Adr), lu8Size);
        u8PutInt(mcBn->mcAdr, lcTxDAdr, 5, lau8Resp, 3 + lu8Size);
      }
      break;

      case lcMsg.cId24(0x05, 0x00, 0x00): // MemWrite
      {
        u32 lu32Adr = cMemTools::u32U8toU32((u8*)(lpu8PayloadRx + 3));
        u8  lu8Size = lcMsg.Len() - 7;

        if (lu8Size != 0) // Posetiv response
        {
          const u8 lau8Resp[] = {cBotNetMsg_MsgProt::u8RespId(0x05), 0x00, 0x00, 0x01 };
          cMemTools::vMemCpy((u8*)lu32Adr, (u8*)(lpu8PayloadRx + 7), lu8Size);
          u8PutInt(mcBn->mcAdr, lcTxDAdr, 5, (u8*)lau8Resp, sizeof(lau8Resp));
        }
        else // Negativ response
        {
          const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x05), 0x00, 0x00, 0x00 };
          u8PutInt(mcBn->mcAdr, lcTxDAdr, 5, (u8*)lau8Resp, sizeof(lau8Resp));
        }
      }
      break;
    }
  }

  // Open
  if (lcMsg.isSetId(0x00, 0x00, 0x00))
  {
    if (cMemTools::u32U8toU32(lpu8PayloadRx + 3) == 0x01020304)
    {
      const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x00), 0x00, 0x00, 0x01, 0x01, 0x20, 0x00, 0x00 , 0x00, 0x00, 0x10, 0x00, 0x00 };
      mbOpen = True;
      u8PutInt(mcBn->mcAdr, lcTxDAdr, 5, (u8*)lau8Resp, sizeof(lau8Resp));
    }
    else
    {
      const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x00), 0x00, 0x00, 0x00 };
      u8PutInt(mcBn->mcAdr, lcTxDAdr, 5, (u8*)lau8Resp, sizeof(lau8Resp));
    }
  }

  return True; // Consumed
}

void cBotNetMsgPortMView::vTick10ms()
{
}
