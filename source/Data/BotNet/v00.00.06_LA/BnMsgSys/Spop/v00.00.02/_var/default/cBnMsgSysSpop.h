#ifndef __BOTNET_MESSAGE_SYS_SPOP_H__
#define __BOTNET_MESSAGE_SYS_SPOP_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cJobHdl.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"
#include "cBnSpop.h"
#include "Eep.h"

//            Sector   Sector     Flash        Flash      Flash      Flash      Flash  
//             Size    Erase      Prog       Prog 256B  Prog 512B  Prog 1024B  Prog 4096B
// STM32L433    2kb     25ms   64Bit 90us      2880us     5760us    11520us     46080us 

class cBotNetMsgPortSpop: public cJobHandler, public cBotNet_MsgSysProcess
{
  public:

    enum class cJobs : u8
    {
      nJobExit = (u8)cJobHandler::cJobs::nLast,
      nJobEnter,
      nJobConfig,
      nJobErase,
      nJobEraseIntRam,
      nJobEraseExtRam,
      nJobEraseIntEeprom,
      nJobEraseExtEeprom,
      nJobEraseIntFlash,
      nJobEraseExtFlash,


      nJobRead,
      nJobReadIntRam,
      nJobReadExtRam,
      nJobReadIntEeprom,
      nJobReadExtEeprom,
      nJobReadIntFlash,
      nJobReadExtFlash,

      nJobWrite,
      nJobWriteIntRam,
      nJobWriteExtRam,
      nJobWriteIntEeprom,
      nJobWriteExtEeprom,
      nJobWriteIntFlash,
      nJobWriteExtFlash,

      nJobCheckSum,
      nJobCheckSumIntRam,
      nJobCheckSumExtRam,
      nJobCheckSumIntEeprom,
      nJobCheckSumExtEeprom,
      nJobCheckSumIntFlash,
      nJobCheckSumExtFlash,

      nJobCall,
      nJobJump,
      nJobReset
    };

    u8*   mu8DataPointer;
    u32   mu32DataCnt;
    u32   mu32DataChecksum;

    u8    mu8PackageCounter;
    u8    mu8MemmoryIdx;

    u8    mu8DefaultMemorySort; // Falls Adresse in RomConst nicht aufgelöst werden kann
                                // Werden Adressen hierhin geschrieben/aufgelöst

    u32   mu32Exitcounter_us;
    u32   mu32Exitcounter_ms;

    u8    mu8MsgTx_Buf[cBotNet_MsgSize];
    cbArrayExtT<uint16> mcTxMsgTx_BAry;
    cBotNetMsg_MsgProt mcTxMsgTx;

    cBotNetMsgPortSpop(cBotNet* lcBotNet);

    cI2cEep  *mcEep;

    u16 u16GetMemorySortByAdr(u8 *lpui8Adress)
    {
      u16 lu16MemSort = RomConst_Error;

      if (RomConst_IsHeaderPlausible())
      {
        lu16MemSort = cBnSpop_u16GetMemorySortByAdr(lpui8Adress);
      }

      if (lu16MemSort == RomConst_Error) return mu8DefaultMemorySort;
      return lu16MemSort;
    }

    u16 u16GetMemorySortByMemIdx(u8 lu8MemoryIndex)
    {
      u16 lu16MemSort = mu8DefaultMemorySort;

      if (RomConst_IsHeaderPlausible())
      {
        lu16MemSort = cBnSpop_u16GetMemorySortByMemIdx(lu8MemoryIndex);

        if (lu16MemSort == RomConst_Error) lu16MemSort = mu8DefaultMemorySort;
      }
      return lu16MemSort;
    }


    uint32 ui32ChkSum(uint8* lpui8Src, uint32 lui32Len)
    {
      uint32 lui32Checksum;

      lui32Checksum = 0;
      while (lui32Len > 0)
      {
        lui32Checksum += *((uint8*)lpui8Src);
        (uint8*)lpui8Src++;
        lui32Len--;
      }
      return lui32Checksum;
    };

    u32 vMemCopy(const uint8* lpvSrc, uint8* lpvDst, uint32 lui32Len)
    {
      u32 lu32Checksum;

      lu32Checksum = 0;
      while (lui32Len > 0)
      {
        *((uint8*)lpvDst) = *((uint8*)lpvSrc);
        lu32Checksum += *((uint8*)lpvSrc);
        (uint8*)lpvDst++;
        (uint8*)lpvSrc++;
        lui32Len--;
      }
      return lu32Checksum;
    };

    inline u8* u8U32toU8(u32 lu32Val, u8* lu8Array)
    {
      lu8Array[3] = (lu32Val & 0xFF); lu32Val >>= 8;
      lu8Array[2] = (lu32Val & 0xFF); lu32Val >>= 8;
      lu8Array[1] = (lu32Val & 0xFF); lu32Val >>= 8;
      lu8Array[0] = (lu32Val & 0xFF);
      return lu8Array;
    }

    inline u32 u32U8toU32(u8* lu8Array)
    {
      return (lu8Array[0] << 24) + (lu8Array[1] << 16) + (lu8Array[2] << 8) + (lu8Array[3]);
    }

    bool isEnable()
    {
      return (mu32Exitcounter_ms > 0);
    }

    bool bMsg(cBotNetMsg_MsgProt& lcMsg);
    void vProcess(u32 lu32TimeDiff_us);
    void vProcess10ms();
};


#endif // __BOTNET_MESSAGE_SYS_SPOP_H__
