#include "main.h"

const uint8 mu8RomConstInit[] __attribute__((aligned(4))) = { 0x12, 0x34, 0x56, 0x78, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x4d, 0x84, 0x00, 0x02, 0xa4, 0x0d, 0x00, 0x00, 0x00, 0xe9, 0x00, 0x00, 0x04, 0x00, 0x08, 0x03, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x8b, 0x08, 0x08, 0xb8, 0x67, 0x1b, 0x67, 0x02, 0x0b, 0x52, 0x6f, 0x6d, 0x43, 0x6f, 0x6e, 0x73, 0x74, 0x5f, 0x30, 0x58, 0x30, 0x38, 0x30, 0x33, 0x46, 0x30, 0x30, 0x30, 0x5f, 0x30, 0x58, 0x31, 0x30, 0x30, 0x30, 0x2e, 0x62, 0x69, 0x6e, 0x00, 0xf3, 0x75, 0x2a, 0x33, 0x34, 0x48, 0x32, 0xf2, 0xcc, 0xcb, 0x64, 0x00, 0x83, 0xe0, 0x10, 0x5f, 0x63, 0x23, 0x1f, 0x13, 0x63, 0x63, 0x67, 0xe7, 0x10, 0x33, 0x20, 0xdf, 0x39, 0xbf, 0xa8, 0x24, 0xb5, 0x42, 0xd7, 0xd7, 0x44, 0x27, 0x28, 0x3f, 0x57, 0xc1, 0xc8, 0xd4, 0xcc, 0xdb, 0x49, 0x27, 0x28, 0x31, 0x57, 0xc1, 0xcc, 0xc4, 0xdb, 0x09, 0xa4, 0x9e, 0x89, 0x81, 0x41, 0x00, 0x08, 0x19, 0x18, 0x99, 0x38, 0xfe, 0x73, 0x01, 0x79, 0x36, 0x0c, 0x5b, 0x18, 0x1a, 0x02, 0x99, 0x19, 0xf8, 0x19, 0x8c, 0x18, 0xfe, 0x03, 0x01, 0x23, 0x1a, 0x00, 0xea, 0x65, 0xc0, 0x02, 0x14, 0x80, 0x26, 0x00, 0x21, 0x03, 0x03, 0xd0, 0x16, 0xe7, 0xfc, 0xbc, 0xe2, 0x12, 0x98, 0x84, 0x00, 0x88, 0xf8, 0xc0, 0xcc, 0x01, 0x66, 0x70, 0x30, 0x30, 0xb8, 0xe4, 0x56, 0x32, 0xe0, 0x03, 0x83, 0x5d, 0xfe, 0x3f, 0x14, 0x64, 0x6b, 0x31, 0x30, 0xb8, 0xf9, 0x78, 0x3b, 0xfe, 0x1f, 0x05, 0xa3, 0x60, 0x00, 0xc0, 0xb2, 0x85, 0x4c, 0xe0, 0xf4, 0x07, 0x00, 0x0d, 0xa4, 0x02, 0x00, 0x00, 0x04, 0x00, 0x00 };


void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}

void PendSV_Handler(void)
{
  while (1)
  {
  }
}


void SysTick_Handler(void)
{
}


typedef struct
{
  uint8  uID1;
  uint8  uID2;
  uint8  uCompressionMethod; // 8 == deflate
  uint8  uFlags;
  uint8  uTimeStampD1;
  uint8  uTimeStampD2;
  uint8  uTimeStampD3;
  uint8  uTimeStampD4;
  uint8  uExtraFlags;
  /*
    00 - default compression
    02 - compressor used maximum compression, slowest algorithm
    04 - compressor used fastest algorithm
  */
  uint8  uOs;
  /*
    00 - FAT filesystem (MS-DOS, OS/2, NT/Win32)
    01 - Amiga
    02 - VMS (or OpenVMS)
    03 - Unix
    04 - VM/CMS
    05 - Atari TOS
    06 - HPFS filesystem (OS/2, NT)
    07 - Macintosh
    08 - Z-System
    09 - CP/M 0A - TOPS-20
    0B - NTFS filesystem (NT)
    0C - QDOS
    0D - Acorn RISCOS
    FF - unknown
  */
}gzip_tstHeader;



typedef union
{
  struct
  {
    u8 B[4];
  };
  u32 u32Data;
} tunBlu32;


/*void SwapBlu32(tunBlu32 *lunBlu32)
{
  u8 u8Temp;

  u8Temp = lunBlu32->B[0];
  lunBlu32->B[0] = lunBlu32->B[3];
  lunBlu32->B[3] = u8Temp;

  u8Temp = lunBlu32->B[1];
  lunBlu32->B[1] = lunBlu32->B[2];
  lunBlu32->B[2] = u8Temp;
}*/

typedef struct
{
  uint8 uCrc32D1;
  uint8 uCrc32D2;
  uint8 uCrc32D3;
  uint8 uCrc32D4;

  uint8 uSizeD1;
  uint8 uSizeD2;
  uint8 uSizeD3;
  uint8 uSizeD4;
}gzip_tstTail;

/*
8x4Byte = 32 Byte Header

Img Adr.

 0. Magic Pattern:
      - 0x12345678: New Block, neededs to be flashed
      - 0x87654321: Block, already flashed
      - 0x12344321 : To Ram (not implemented)
 4. 0xFFFFFFFF: Reserved
 8. Checksum Image: Starting from Img. Adress 12
12. Checksum Data
16. Size Image von Adress 0: Note: Auf die nächste Flashgrenze aufrunden
20. Size Data
24. Destination Adress, to be programmed
28. Execution Adress, Null = no execution
32. Start of Image
*/


typedef struct
{
  // - 0x12345678 : New Block, needs to be flashed
  // - 0x87654321 : Block, already flashed
  // - 0x12344321 : To Ram
  tunBlu32 MagicPatter;
  tunBlu32 Reserved;
  tunBlu32 ChecksumImage; // Starting from Img.Adress 12
  tunBlu32 ChecksumData;
  tunBlu32 SizeImage; // von Adress 0
  tunBlu32 SizeData;
  tunBlu32 DestinationAdress; // to be programmed
  tunBlu32 ExecutionAdress; // Null = no execution
}Bl_tstBlockHeader;





#define BUFLEN 2048
unsigned char buf[BUFLEN];

#define DICT_SIZE (1 << 15)

typedef struct _gz_stream_t {
  TINF_DATA tinf;
} gz_stream_t;

static gz_stream_t gz_stream;

void vGetBlockHeader(uint8* lpu8Mem, Bl_tstBlockHeader *lstHeader)
{
  int t;
  uint8* lpu8DestMem = (uint8*)lstHeader;
  for (t = 0; t < 32/4; t++)
  {
    lpu8Mem += 3;
    *lpu8DestMem++ = *lpu8Mem--;
    *lpu8DestMem++ = *lpu8Mem--;
    *lpu8DestMem++ = *lpu8Mem--;
    *lpu8DestMem++ = *lpu8Mem;
    lpu8Mem += 4;
  }
}

void run_main(void *p)
{
  unsigned rst;
  unsigned stk;
  unsigned vtor;
  rst = *(unsigned *)(p + 4);
  stk = *(unsigned *)p;
  vtor = (unsigned)p & 0xff000000;
  SCB->VTOR = vtor;

  __set_MSP(stk);
  __asm("bx %0" : : "r" (rst));
}


u32 GetCheckums(uint8* lpu8StartAdr, u32 lu32Size)
{
  return uzlib_crc32(lpu8StartAdr, lu32Size, 1);
}


// return :
//   1: success
//   0: no success: wrong memory
//  -1: no success: decompress error

int run_upd_block(Bl_tstBlockHeader *lstHeader, uint8* lpu8HeaderAdr, uint8 lu8ForceRam, uint8 lu8ForceFlash)
{
  uint8*     lpui8DestMem;
  uint16     lu16MemorySort;

  lpui8DestMem = (uint8*)lstHeader->DestinationAdress.u32Data;

  // InAppProg_Platform_u16GetMemorySort kann nur ausgeführt werden, wenn RomConst vorhanden ist.
  // Ansonsten BLU wird in den RAM geladen.
  if (lu8ForceRam)   lu16MemorySort = IAP_MM_RAM;
  else
  if (lu8ForceFlash) lu16MemorySort = IAP_MM_FLASH;
  else
    lu16MemorySort = InAppProg_Platform_u16GetMemorySort(lpui8DestMem);

  if ((lu16MemorySort == IAP_MM_RAM) || (lu16MemorySort == IAP_MM_FLASH))
  {
    uzlib_init();

    memset(&gz_stream.tinf, 0, sizeof(gz_stream.tinf)); // sizeof(gz_stream.tinf) = 1292 Byte

    gz_stream.tinf.source = lpu8HeaderAdr + 32;
    gz_stream.tinf.source_limit = IAP_PARTITION_START_ADRESS_IMG + IAP_PARTITION_SIZE_IMG;

    int st = uzlib_gzip_parse_header(&gz_stream.tinf);
    if (st != TINF_OK)
    {
      return -1;
    }

    uzlib_uncompress_init(&gz_stream.tinf, lpui8DestMem, buf, DICT_SIZE);

    if (lu16MemorySort == IAP_MM_FLASH)
    {
      InAppProg_Platform_vFlash_Unlock();
    }

    st = 0;
    while (!st)
    {
      InAppProg_Platform_vResetWdog();
      gz_stream.tinf.dest = buf;
      gz_stream.tinf.dest_limit = buf + BUFLEN;
      st = uzlib_uncompress_chksum(&gz_stream.tinf);

      if (lu16MemorySort == IAP_MM_FLASH)
      {
        InAppProg_Platform_vFlash_EreasePage(lpui8DestMem);
        //InAppProg_Platform_vFlash_EreasePage(lpui8DestMem+2048);
        InAppProg_Platform_vFlash_Program(lpui8DestMem, buf, BUFLEN);
      }
      else
      {
        memcpy(lpui8DestMem, buf, BUFLEN);
      }

      lpui8DestMem += BUFLEN;
      gz_stream.tinf.dict_idx_thres += BUFLEN;

      if  ((gz_stream.tinf.dict_idx_thres & (DICT_SIZE-1)) == 0)
      {
        gz_stream.tinf.dict_idx_thres = 0;
        gz_stream.tinf.dict_ring_flash += gz_stream.tinf.dict_size;
      }
    }

    
    
    if (st == TINF_DONE)
    {
      InAppProg_Platform_vResetWdog();

      // Falls das Image aus dem Flash  geladen wurde
      // dann noch als bearbeitet markieren.
      // Außer es war ein internen Image (Dummy Romconst oder Blu)
      if ((lu16MemorySort == IAP_MM_FLASH) &&
          (!((lu8ForceRam) || (lu8ForceFlash))))
      {
        // Alignment auf Sectorgrenzen beachten
        // z.B. falls Image nicht am Start einen Sektors beginnt
        memcpy(buf, (u8*)((u32)lpu8HeaderAdr & ((u32)(~2047))), 2048);
        buf[((u32)lpu8HeaderAdr & 2047) + 0] = 0x78;
        buf[((u32)lpu8HeaderAdr & 2047) + 1] = 0x56;
        buf[((u32)lpu8HeaderAdr & 2047) + 2] = 0x34;
        buf[((u32)lpu8HeaderAdr & 2047) + 3] = 0x12;
        InAppProg_Platform_vFlash_EreasePage((u8*)((u32)lpu8HeaderAdr & ((u32)(~2047))));
        InAppProg_Platform_vFlash_Program((u8*)((u32)lpu8HeaderAdr & ((u32)(~2047))), buf, 2048);
        InAppProg_Platform_vFlash_Lock();
      }
      return 1;
    }

    if (lu16MemorySort == IAP_MM_FLASH)
    {
      InAppProg_Platform_vFlash_Lock();
    }
  }
  return 0;
}



bool run_upd(u8 *lu8pAdress, uint8 lu8ForceRam, uint8 lu8ForceFlash)
{
  uint8*             lpu8Mem;
  Bl_tstBlockHeader  lstHeader;
  bool               lbNewSoftware;

  lpu8Mem = lu8pAdress;
  lbNewSoftware = False;
  vGetBlockHeader(lpu8Mem, &lstHeader);

  // prozess all images
  while (1)
  {
    //Check MagicByte
    // - 0x12345678 : New Block, needs to be flashed
    // - 0x87654321 : Block, already flashed
    InAppProg_Platform_vResetWdog();
    if (lstHeader.MagicPatter.u32Data == 0x12345678)
    {
      if (run_upd_block(&lstHeader, lpu8Mem, lu8ForceRam, lu8ForceFlash) > 0)
      {
        lbNewSoftware = True;
        if (lstHeader.ExecutionAdress.u32Data)
        {
          run_main((void*)lstHeader.ExecutionAdress.u32Data);
        }
      }
      lpu8Mem += lstHeader.SizeImage.u32Data;
      vGetBlockHeader(lpu8Mem, &lstHeader);
    }
    else if (lstHeader.MagicPatter.u32Data == 0x78563412) // already programmed, skip that image
    {
      lpu8Mem += lstHeader.SizeImage.u32Data;
      vGetBlockHeader(lpu8Mem, &lstHeader);
    }
    else
    {
      return lbNewSoftware;
    }
  }
}

int main(void)
{
  tunWakeupSources lunWuSources;
  tstBuRamDef*     lstBuRamDef = (tstBuRamDef*)((RTC_BASE + 0x50));
  u8 lbStartBlu = 0;

  SCB->VTOR = 0x08000000;

  // BuRam-Zugriff freischalten
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  LL_PWR_EnableBkUpAccess();
  WufDef_vCheckWuReason(&lunWuSources);
  lstBuRamDef->u32WuReason = lunWuSources.u32WakeupSources;
  lstBuRamDef->u32BluReason = 0;
  lstBuRamDef->u32Bl++;
  if (lstBuRamDef->u32Bl == 0) lstBuRamDef->u32Bl = 1;

  #ifndef PCB_PROJECTCFG_Test
    InAppProg_Platform_vStartWdog();
  #endif

  // Falls kein RomConst da ist, Init Romconst laden
  if (!RomConst_IsValid())
  {
    run_upd((u8*)mu8RomConstInit, 0, 1);
  }

  //Noch Init RomConst ?
  if (RomConst_stDevice_Info->u8PartitionCnt < 3)
  {
    lbStartBlu = 1;
    lstBuRamDef->u32BluReason = 1;
  }

  if (lunWuSources.stWakeupSources.isWuIWdg)
  {
    // Bei Watchdog Reset-BLU starten
    lbStartBlu = 1;
    lstBuRamDef->u32BluReason = 2;


    // Bit 18 IWDG_STDBY: Independent watchdog counter freeze in Standby mode
    //   0 : Independent watchdog counter is frozen in Standby mode
    //   1 : Independent watchdog counter is running in Standby mode
    if ((FLASH->OPTR & FLASH_OPTR_IWDG_STDBY) || (FLASH->OPTR & FLASH_OPTR_IWDG_STOP))
    {
      lstBuRamDef->u32BluReason = 3;
    }
  }

  // Wenn SW-Reset von der APP kam, dann BLU starten

  if (lunWuSources.stWakeupSources.isWuSftRst)
  {
    u32 lu32Zwerg = ('S' << 24) + ('P' << 16) + ('O' << 8) + 'P';
    if (lstBuRamDef->u32SpopMp == lu32Zwerg)
    {
      lbStartBlu = 1;
      lstBuRamDef->u32BluReason = 4;
    }
  }

  InAppProg_Platform_vResetWdog();

  // try to decompress and run the blu img(boot loader updater) to the ram
  if (lbStartBlu)
  {
    lstBuRamDef->u32SpopMp = 0;
    run_upd(IAP_PARTITION_START_ADRESS_BLU, 1, 0);
    //This point is only reached, if BLU img is missing.
  }

  // Check, if there something in the image area
  // if yes decompress it and execute it
  run_upd(IAP_PARTITION_START_ADRESS_IMG, 0, 0);

  run_main(IAP_PARTITION_START_ADRESS_APP);
}

