#ifndef __BOTNET_MESSAGE_SYS_SPOP_H__
#define __BOTNET_MESSAGE_SYS_SPOP_H__

#include "Typedef.h"
#include "RomConst.h"
#include "cJobHdl.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cStrT.h"
#include "cBnCfg.h"
#include "cBnMsgSys.h"
#include "cBnMsgSysBtr.h"
#include "cBnSpop.h"
//#include "Eep.h"

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

    u8    mu8MsgTx_Buf[cBotNet_MsgSize];

    cBotNetMsgPortBtr*  mcBtr;

    cBarryPtrT<u16> mcTxMsgTx;
    cBotNetAdress   mcTxDAdr;

    cBotNetMsgPortSpop(cBotNet* lcBotNet, cBotNetMsgPortBtr* lcBtr);

    //cI2cEep* mcEep;

    u8*   mu8DataPointer;
    u32   mu32DataCnt;
    u32   mu32DataChecksum;

    u8   mu8FlashWriteBuf[48] __attribute__((aligned(8)));
    u32   mu32Exitcounter_ms;

    u8    mu8PackageCounter;
    u8    mu8MemmoryIdx;

    u8    mu8DefaultMemorySort; // Falls Adresse in RomConst nicht aufgelöst werden kann
                                // Werden Adressen hierhin geschrieben/aufgelöst

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


    u32 ui32ChkSum(uint8* lpui8Src, u32 lui32Len)
    {
      u8 lui32Checksum;

      lui32Checksum = 0;
      while (lui32Len > 0)
      {
        lui32Checksum += *((u8*)lpui8Src);
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

    bool isEnable()
    {
      return (mu32Exitcounter_ms > 0);
    }

    //bool isEraseActive()
    //{
    //  return (((u8)mcJob == (u8)cJobs::nJobEraseIntFlash) && 
    //          ((mcState == cJobHandler::cJobStates::stJobProcess2) ||
    //           (mcState == cJobHandler::cJobStates::stJobProcess3)));
    //}
    void vFlashErase();
    bool bMsg(cBotNetMsg_MsgProt& lcMsg);
    void vProcess(u16 lu16TimeDiff_ms);
    void vTick10ms() override;
};


#endif // __BOTNET_MESSAGE_SYS_SPOP_H__
