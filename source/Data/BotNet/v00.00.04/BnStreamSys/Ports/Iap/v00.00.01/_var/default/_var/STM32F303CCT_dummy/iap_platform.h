#ifndef __IAP_PLATFORM_H__
#define __IAP_PLATFORM_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "stm32f30x.h"
#include "stm32f30x_flash.h"
#include "stm32f30x_iwdg.h"


const char8  InAppProg_Platform_Name[] = "STM32F303CCT6";
const char8  InAppProg_Platform_Info[] = "Cortex-M4,Rom 256KB,Ram 48KB";

#define  InAppProg_Platform_Ram_Size   (1024*8)

typedef struct
{
  char8  szName[24];
  uint16 ui16Sort;
  uint16 ui16Type;
  uint32 ui32Start;
  uint32 ui32Size;
  uint32 ui32Block_Size;
}tstInAppProg_Platform_Partition;


const tstInAppProg_Platform_Partition stInAppProg_Platform_Partitions[3] =
{
  {
   //1  2   3   4    5   6  7     8     9     10    11    12    13   14   15   16   17   18   19   20   21   22   23   24
    'F','l','a','s','h', 0, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    16,
    0,
    0x08000000,
    256*1024,
    0x0800,
  },
  {
   //1  2   3    4   5      6     7     8     9     10    11    12    13   14   15   16   17   18   19   20   21   22   23   24
    'R','a','m', 0, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0,
    0,
    0x20000000,
    40*1024,
    1,
  },
  {
   //1  2   3    4   5      6     7     8     9     10    11    12    13   14   15   16   17   18   19   20   21   22   23   24
    'C','C','M', 0, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0,
    0,
    0x10000000,
    8*1024,
    1,
  }
};


typedef void cInAppProg_Platform_vCall(uint32 lui32P1, uint32 lui32P2, uint32 lui32P3, uint32 lui32P4);
typedef void cInAppProg_Platform_vJump(void);

void   InAppProg_Platform_vStartWdog();
void   InAppProg_Platform_vResetWdog();


bool   InAppProg_Platform_IsFlash(uint8* lpui8Adress);
void   InAppProg_Platform_vFlash_Unlock();
void   InAppProg_Platform_vFlash_Lock();
uint32 InAppProg_Platform_ui32Flash_Size2Pages(uint32 lui32Size);
void   InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress);
uint32 InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size);

void   InAppProg_Platform_vJumpToApp(uint8* lpui8Adr);
void   InAppProg_Platform_vCall(uint8* lpui8Adr, uint32* luip32Param);

void   InAppProg_PlatformvSystemReset();


#ifdef __cplusplus
}
#endif

#endif // __IAP_PLATFORM_H__

