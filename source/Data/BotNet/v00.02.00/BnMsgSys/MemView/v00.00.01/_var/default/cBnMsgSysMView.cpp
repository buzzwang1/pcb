#include "cBnMsgSysMView.h"
#include "cBotnet.h"


bool cBotNetMsgPortMView::bMsg(cBotNetMsg_MsgProt& lcMsg)
{
  cBotNetAdress lcTxDAdr;

  u8* lpu8PayloadRx = lcMsg.GetPayload().mpu8Data;
  lcTxDAdr = lcMsg.cGetSAdr();

  switch (lcMsg.u16GetIdx())
  {
    case 5: // Request
      {
        if (isOpen())
        {
          // Read Entry Request
          if ((lpu8PayloadRx[0] == 2) && (lpu8PayloadRx[1] < cBotNetMsgPortMViewEntries) && (lpu8PayloadRx[2] == 0))
          {
            mcTxMsgTx.Set(lpu8PayloadRx, 3);

            u32 lu32Adr = pau32EntryAdr[lpu8PayloadRx[1]];
            u8  lu8Size = pau8EntrySize[lpu8PayloadRx[1]];

            if (lu8Size > 40) lu8Size = 40;
            
            if ((lu32Adr != 0) && (lu8Size != 0))
            {
              mcTxMsgTx.Add((u8*)lu32Adr, lu8Size);
            }
            else
            {
              mcTxMsgTx.Add(0x00);
            }
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
        }
        return True; // Consumed
      }
      break;
    case 7: // Set Msg
      {
        if (isOpen())
        {
          // Set Entry
          if ((lpu8PayloadRx[0] == 1) && (lpu8PayloadRx[1] < cBotNetMsgPortMViewEntries) && (lpu8PayloadRx[2] == 0))
          {
            mcTxMsgTx.Set(lpu8PayloadRx, 3);

            u32 lu32Adr = cMemTools::u32U8toU32(lpu8PayloadRx);
            u8  lu8Size = lpu8PayloadRx[10];

            if (lu8Size > 40) lu8Size = 40;
            
            if (cBnSpop_u16GetMemorySortByAdr((u8*)lu32Adr) == RomConst_Sort_Ram)
            {
              mcTxMsgTx.Add(0x01);
              pau32EntryAdr[lpu8PayloadRx[1]] = lu32Adr;
              pau8EntrySize[lpu8PayloadRx[1]] = lu8Size;
            }
            else
            {
              mcTxMsgTx.Add(0x00);
            }
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
          else
          // Write Entry Data
          if ((lpu8PayloadRx[0] == 3) && (lpu8PayloadRx[1] < cBotNetMsgPortMViewEntries) && (lpu8PayloadRx[2] == 0))
          {
            mcTxMsgTx.Set(lpu8PayloadRx, 3);

            u32 lu32Adr = pau32EntryAdr[lpu8PayloadRx[1]];
            u8  lu8Size = pau8EntrySize[lpu8PayloadRx[1]];
            
            if ((lu32Adr != 0) && (lu8Size != 0))
            {
              mcTxMsgTx.Add(0x01);
              cMemTools::vMemCpy((u8*)lu32Adr, (u8*)(lpu8PayloadRx + 3), lu8Size);
            }
            else
            {
              mcTxMsgTx.Add(0x00);
            }
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
          else
          // MemRead
          if ((lpu8PayloadRx[0] == 4) && (lpu8PayloadRx[1] == 0) && (lpu8PayloadRx[2] == 0))
          {
            mcTxMsgTx.Set(lpu8PayloadRx, 3);

            u32 lu32Adr = cMemTools::u32U8toU32(lpu8PayloadRx);
            u8  lu8Size = lpu8PayloadRx[10];

            if (lu8Size > 40) lu8Size = 40;
            
            if ((cBnSpop_u16GetMemorySortByAdr((u8*)lu32Adr) == RomConst_Sort_Ram) && 
                (lu8Size != 0))
            {
              mcTxMsgTx.Add((u8*)lu32Adr, lu8Size);
            }
            else
            {
              mcTxMsgTx.Add(0x00);
            }
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
          else
          // MemWrite
          if ((lpu8PayloadRx[0] == 5) && (lpu8PayloadRx[1] == 0) && (lpu8PayloadRx[2] == 0))
          {
            mcTxMsgTx.Set(lpu8PayloadRx, 3);

            u32 lu32Adr = cMemTools::u32U8toU32(lpu8PayloadRx);
            u8  lu8Size = lcMsg.Len() - 7;
            
            if ((cBnSpop_u16GetMemorySortByAdr((u8*)lu32Adr) == RomConst_Sort_Ram) && 
                (lu8Size != 0))
            {
              mcTxMsgTx.Add(0x01);
              cMemTools::vMemCpy((u8*)lu32Adr, (u8*)(lpu8PayloadRx + 7), lu8Size);
            }
            else
            {
              mcTxMsgTx.Add(0x00);
            }
            u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
          }
        }

        // Open
        if ((lpu8PayloadRx[0] == 0) && (lpu8PayloadRx[1] == 0) && (lpu8PayloadRx[2] == 0))
        { 
          u8 lu8Temp[4];
          mcTxMsgTx.Set(lpu8PayloadRx, 3);
          if (cMemTools::u32U8toU32(lpu8PayloadRx + 3) == 0x01020304)
          {
            mbOpen = True;

            mcTxMsgTx.Add(0x01);
            mcTxMsgTx.Add(RomConst_stDevice_Info->u8PartitionCnt);

            for (u8 lu8MemIdx = 0; lu8MemIdx < RomConst_stDevice_Info->u8PartitionCnt; lu8MemIdx++)
            {
              const tstMemPart_Partition* lpPartition;
              RomConst_Platform_vLoadPartition(lu8MemIdx, lpPartition);

              if (lpPartition->ui16Sort == RomConst_Sort_Ram)
              {
                mcTxMsgTx.Add(cMemTools::pu8U32toU8(lpPartition->ui32Start, lu8Temp), 4);
                mcTxMsgTx.Add(cMemTools::pu8U32toU8(lpPartition->ui32Size, lu8Temp), 4);
              }
            }
          }
          else
          {
            mcTxMsgTx.Add(0x00);
          }
          u8PutInt(mcBn->mcAdr, lcTxDAdr, 6, mcTxMsgTx.mpu8Data, mcTxMsgTx.Len());
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
