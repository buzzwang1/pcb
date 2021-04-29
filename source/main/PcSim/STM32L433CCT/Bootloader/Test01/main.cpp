#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include "stm-hdr.h"
//#include "flashblk.h"
//#include "mxconstants.h"
#include "uzlib.h"
#include "main.h"

//#define printf(...)

typedef unsigned char uint8;
typedef unsigned int  uint32;

uint8 mui8FlashMemInput[256 * 1024];
#define IAP_BL_AREA_SIZE   (16  * 1024)
#define IAP_APP_AREA_SIZE  (160 * 1024)
#define IAP_IAP_AREA_SIZE  (76  * 1024)
#define IAP_EEP_AREA_SIZE  (4   * 1024)


#define IAP_BL_AREA_START   (&mui8FlashMemInput[0])
#define IAP_APP_AREA_START  (IAP_BL_AREA_START  + IAP_BL_AREA_SIZE)
#define IAP_IAP_AREA_START  (IAP_APP_AREA_START + IAP_APP_AREA_SIZE)
#define IAP_EEP_AREA_START  (IAP_IAP_AREA_START + IAP_IAP_AREA_SIZE)



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

typedef struct
{
  // - 0x12345678 : New Block, neededs to be flashed
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
  //unsigned rst;
  //unsigned stk;  
  //unsigned vtor;
  //rst = *(unsigned *)(p + 4);
  //stk = *(unsigned *)p;
  //vtor = (unsigned)p & 0x1FFFFF80;
  //vtor = (vtor & ~0xffe00000) | (rst & 0xffe0000);
  //SCB->VTOR = vtor;
  //__set_MSP(stk);
  //__asm("bx %0" : : "r" (rst));
}


u32 GetCheckums(uint8* lpu8StartAdr, u32 lu32Size)
{
  return uzlib_crc32(lpu8StartAdr, lu32Size, 1);
}

int run_upd_block(Bl_tstBlockHeader *lstHeader, uint8* lpu8HeaderAdr)
{
  uint8*     lpui8DestMem;
  uint16     lu16MemoryType;

  lpui8DestMem = IAP_BL_AREA_START + lstHeader->DestinationAdress.u32Data;
  lu16MemoryType = InAppProg_Platform_u16GetMM(lpui8DestMem);

  if ((lu16MemoryType == IAP_MM_RAM) || (lu16MemoryType == IAP_MM_FLASH))
  {
    uzlib_init();

    memset(&gz_stream.tinf, 0, sizeof(gz_stream.tinf)); // sizeof(gz_stream.tinf) = 1292 Byte

    gz_stream.tinf.source = lpu8HeaderAdr + 32;
    gz_stream.tinf.source_limit = IAP_IAP_AREA_START + IAP_IAP_AREA_SIZE;

    int st = uzlib_gzip_parse_header(&gz_stream.tinf);
    if (st != TINF_OK) {
      return -1;
    }

    uzlib_uncompress_init(&gz_stream.tinf, lpui8DestMem, buf, DICT_SIZE);

    if (lu16MemoryType == IAP_MM_FLASH)
    {
      InAppProg_Platform_vFlash_Unlock();
    }

    st = 0;
    while (!st)
    {
      gz_stream.tinf.dest = buf;
      gz_stream.tinf.dest_limit = buf + BUFLEN;
      st = uzlib_uncompress_chksum(&gz_stream.tinf);

      if (lu16MemoryType == IAP_MM_FLASH)
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
      if (lu16MemoryType == IAP_MM_FLASH)
      {
        memcpy(buf, lpu8HeaderAdr, 2048);
        buf[0] = 0x78;
        buf[1] = 0x56;
        buf[2] = 0x34;
        buf[3] = 0x12;
        InAppProg_Platform_vFlash_EreasePage(lpu8HeaderAdr);
        InAppProg_Platform_vFlash_Program(lpu8HeaderAdr, buf, 2048);
        InAppProg_Platform_vFlash_Lock();
      }
      return 1;
    }

    if (lu16MemoryType == IAP_MM_FLASH)
    {
      InAppProg_Platform_vFlash_Lock();
    }
  }
  return 0;
}



void run_upd()
{
  uint8*             lpu8Mem;
  Bl_tstBlockHeader  lstHeader;

  lpu8Mem = IAP_IAP_AREA_START;
  vGetBlockHeader(lpu8Mem, &lstHeader);
  lstHeader.DestinationAdress.u32Data -= 0x08000000;

  while (1)
  {
    //Check MagicByte
    if (lstHeader.MagicPatter.u32Data == 0x12345678)
    {
      if (run_upd_block(&lstHeader, lpu8Mem))
      {
        if (lstHeader.ExecutionAdress.u32Data)
        {
          run_main((void*)lstHeader.ExecutionAdress.u32Data);
        }
      }
      lpu8Mem += lstHeader.SizeImage.u32Data;
      vGetBlockHeader(lpu8Mem, &lstHeader);
    }
    else if (lstHeader.MagicPatter.u32Data == 0x78563412)
    {
      lpu8Mem += lstHeader.SizeImage.u32Data;
      vGetBlockHeader(lpu8Mem, &lstHeader);
    }
    else
    {
      return;
    }
  }
}

void main(void)
//void bl1_main(void)
{
  FILE *f;
  long fsize;

  memset(mui8FlashMemInput, 0x33, 256 * 1024);

  f = fopen("C:\\E2\\Projekte\\SW\\Code\\Cmake_PC_Gzip\\source\\main\\PC\\Bn\\Test02\\App_0x0802C000.bin.gz.bin", "rb");
  fseek(f, 0, SEEK_END);
  fsize = ftell(f);
  fseek(f, 0, SEEK_SET);  /* same as rewind(f); */
  
  fread(IAP_IAP_AREA_START, 1, fsize, f);
  fclose(f);

  printf("Start BL1\n");
  
  run_upd();


  f = fopen("C:\\E2\\Projekte\\SW\\Code\\Cmake_PC_Gzip\\source\\main\\PC\\Bn\\Test02\\out.bin", "wb");
  fwrite(IAP_APP_AREA_START, 1, IAP_APP_AREA_SIZE, f);
  fclose(f);


  printf("Start main code"); // @%p\n",strt);
  //run_main(IAP_APP_AREA_START);
}

