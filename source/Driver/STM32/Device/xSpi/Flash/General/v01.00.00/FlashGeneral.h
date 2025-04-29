#ifndef __SPI_FLASH_GENERAL_H__
#define __SPI_FLASH_GENERAL_H__

#include "Typedef.h"

class cSpiFlashGeneral
{
  public:

  typedef enum
  {
    INIT_ID = 0x000000,  // 1M bit
    W25X10_FLASH_ID = 0x1130EF,  // 1M bit
    W25X20_FLASH_ID = 0x1230EF,  // 2M bit
    W25X40_FLASH_ID = 0x1330EF,  // 4M bit
    W25X80_FLASH_ID = 0x1440EF,  // 8M bit
    W25Q16_FLASH_ID1 = 0x1530EF,  // 16M bit
    W25Q16_FLASH_ID2 = 0x1540EF,  // 16M bit
    W25Q32_FLASH_ID1 = 0x1640EF,  // 32M bit
    W25Q32_FLASH_ID2 = 0x1660BA,  // 32M bit
    W25Q64_FLASH_ID1 = 0x1740EF,  // 64M bit
    W25Q64_FLASH_ID2 = 0x1760EF,  // 64M bit
    W25Q128_FLASH_ID1 = 0x1840EF,  // 128M bit
    W25Q128_FLASH_ID2 = 0x1860EF,  // 128M bit
    W25Q256_FLASH_ID1 = 0x1940EF,  // 256M bit
    W25Q256_FLASH_ID2 = 0x1960EF,  // 256M bit

  }tenFlashIDs;


  typedef enum
  {
    StInitMissing,
    
    StIdle,
    StError,

    StInit1,
    StInit2,
    StInit3,
    StInit4,
    StInit5,
    StInit6,
    StInit7,

    StRead1,
    StRead2,
    StRead3,
    StRead4,


    StWrite1,
    StWrite2,
    StWrite3,
    StWrite4,

    StEraseSector1,
    StEraseSector2,
    StEraseSector3,
    StEraseSector4,

    StEraseChip1,
    StEraseChip2,
    StEraseChip3,
    StEraseChip4,
  }tenState;

  u32 mu32FlashID;
  u32 mu32FlashSize;
  u32 mu32FlashSectorCount;
  u32 mu32FlashSectorSize;
  u32 mu32FlashBaseAdr;

  u8* mu32WorkAdrRam;
  u32 mu32WorkAdrFlash;
  u32 mu32WorkSize;
  
  u32 mu32Delay_us;
  tenState menState;
  u8  mu8FlashStatus[3];
  u8  mu8Error;
  bool mbInit;

  cSpiFlashGeneral(u32 lu32SectorSize, u32 lu32SectorCount, u32 lu32FlashBaseAdr)
  {
    menState = StInitMissing;
    mbInit   = False;
    mu8Error = 0;
    mu32Delay_us = 0;
    mu8FlashStatus[0] = mu8FlashStatus[1] = mu8FlashStatus[2] = 0;

    mu32FlashID = INIT_ID;
    mu32FlashSize = lu32SectorCount * lu32SectorSize;
    mu32FlashSectorCount = lu32SectorCount;
    mu32FlashSectorSize  = lu32SectorSize;
    mu32FlashBaseAdr     = lu32FlashBaseAdr;
  }

  void vSetError()
  {
    mu8Error = 1;
    __asm("NOP");
  }

  bool isKnownId(u32 lu32Id)
  {
    switch (lu32Id)
    {
      case W25X10_FLASH_ID:    mu32FlashID = lu32Id; break;  // 1M bit
      case W25X20_FLASH_ID:    mu32FlashID = lu32Id; break;  // 2M bit
      case W25X40_FLASH_ID:    mu32FlashID = lu32Id; break;  // 4M bit
      case W25X80_FLASH_ID:    mu32FlashID = lu32Id; break;  // 8M bit
      case W25Q16_FLASH_ID1:   mu32FlashID = lu32Id; break;  // 16M bit
      case W25Q16_FLASH_ID2:   mu32FlashID = lu32Id; break;  // 16M bit
      case W25Q32_FLASH_ID1:   mu32FlashID = lu32Id; break;  // 32M bit
      case W25Q32_FLASH_ID2:   mu32FlashID = lu32Id; break;  // 32M bit
      case W25Q64_FLASH_ID1:   mu32FlashID = lu32Id; break;  // 64M bit
      case W25Q64_FLASH_ID2:   mu32FlashID = lu32Id; break;  // 64M bit
      case W25Q128_FLASH_ID1:  mu32FlashID = lu32Id; break;  // 128M bit
      case W25Q128_FLASH_ID2:  mu32FlashID = lu32Id; break;  // 128M bit
      case W25Q256_FLASH_ID1:  mu32FlashID = lu32Id; break;  // 256M bit
      case W25Q256_FLASH_ID2:  mu32FlashID = lu32Id; break;  // 256M bit
      default: return False;
    }
    return True;
  }
};



#endif // __SPI_FLASH_GENERAL_H__
