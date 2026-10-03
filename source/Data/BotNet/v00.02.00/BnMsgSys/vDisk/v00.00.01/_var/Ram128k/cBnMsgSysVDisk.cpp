#include "cBnMsgSysVDisk.h"
#include "cBotnet.h"


bool cBotNetMsgPortVDisk::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  if (lcMsg.u16GetIdx() != 6)
    return false;

  u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;
  cBotNetAdress lcTxDAdr = lcMsg.cGetSAdr();

  if (isOpen())
  {
    switch (lcMsg.u32GetId())
    {
      case lcMsg.cId24Req(0x01, 0x00, 0x00): // Device Info
      {
        // Device Info        TX 01 | 00 | 00 | RC.SC.SC.SC.SC.SS.SS:          RC: Response code: 0x00 = Fail; 0x1B = ok
        //                                                                     SC.SC.SC.SC: 32Bit: Sector Count
        //                                                                     SS.SS: 16Bit: Sector Size
        u8 lau8Resp[10];
        cBotNetMsg_MsgProt::vRespId(1, 0, 0, lau8Resp);
        lau8Resp[3] = 0x1B;

        cMemTools::pu8U32toU8(sizeof(mau8VDisk), lau8Resp + 4);
        cMemTools::pu8U16toU8(512, lau8Resp + 8);

        u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, lau8Resp, sizeof(lau8Resp));
      }
      break;

      case lcMsg.cId24(0x02, 0x00, 0x00): // MemRead
      {
        u32 lu32Adr = cMemTools::u32U8toU32((u8*)(lpu8PayloadRx + 3));
        u8  lu8Size = lpu8PayloadRx[7];

        u8 lau8Resp[43];
        cBotNetMsg_MsgProt::vRespId(2, 0, 0, lau8Resp);

        if (lu8Size > 40) lu8Size = 40;
        cMemTools::vMemCpy((u8*)(lau8Resp + 3), (u8*)(&mau8VDisk[lu32Adr]), lu8Size);
        u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, lau8Resp, 3 + lu8Size);
      }
      break;

      case lcMsg.cId24(0x03, 0x00, 0x00): // MemWrite
      {
        u32 lu32Adr = cMemTools::u32U8toU32((u8*)(lpu8PayloadRx + 3));
        u8  lu8Size = lcMsg.Len() - 7;

        if (lu8Size != 0) // Posetiv response
        {
          const u8 lau8Resp[] = {cBotNetMsg_MsgProt::u8RespId(0x03), 0x00, 0x00, 0x1B };
          cMemTools::vMemCpy((u8*)(&mau8VDisk[lu32Adr]), (u8*)(lpu8PayloadRx + 7), lu8Size);
          u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
        }
        else // Negativ response
        {
          const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x03), 0x00, 0x00, 0x00 };
          u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
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
      const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x00), 0x00, 0x00, 0x1B };
      mbOpen = True;
      u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
    }
    else
    {
      const u8 lau8Resp[] = { cBotNetMsg_MsgProt::u8RespId(0x00), 0x00, 0x00, 0x00 };
      u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, (u8*)lau8Resp, sizeof(lau8Resp));
    }
  }

  return True; // Consumed
}

void cBotNetMsgPortVDisk::vTick10ms()
{
}

u8 cBotNetMsgPortVDisk::mau8VDisk[1024 * 128] __attribute__((section(".vdisk_section")));
