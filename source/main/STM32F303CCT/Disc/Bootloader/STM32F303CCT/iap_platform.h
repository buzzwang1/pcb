#ifndef __IAP_PLATFORM_H__
#define __IAP_PLATFORM_H__

#ifdef __cplusplus
  extern "C" {
#endif

#include "Typedef.h"
#include "stm32f30x.h"
#include "stm32f30x_flash.h"
#include "stm32f30x_iwdg.h"

#define IAP_FLASH_START     ((uint8*)0x08000000)
#define IAP_FLASH_SIZE      (256 * 1024)

#define IAP_NVM_AREA_SIZE  (4 * 1024)
#define IAP_NVM_AREA_START (&IAP_FLASH_START[IAP_FLASH_SIZE - IAP_NVM_AREA_SIZE])

#define IAP_IAP_AREA_SIZE  (76 * 1024)
#define IAP_IAP_AREA_START (&IAP_FLASH_START[IAP_FLASH_SIZE - (IAP_NVM_AREA_SIZE + IAP_IAP_AREA_SIZE)])

#define IAP_BL_AREA_SIZE   (16 * 1024)
#define IAP_BL_AREA_START  (&IAP_FLASH_START[0])
#define IAP_APP_AREA_START (&IAP_FLASH_START[IAP_BL_AREA_SIZE])

#define IAP_PARTITION_COUNT  4

#define IAP_MM_RAM         0
#define IAP_MM_FLASH       16
#define IAP_MM_EEPROM      32
#define IAP_MM_ERROR       0xFFFF


typedef struct
{
  //char8  szName[24];
  uint16 ui16Sort;
  uint16 ui16Type;
  uint32 ui32Start;
  uint32 ui32Size;
  uint32 ui32Block_Size;
}tstInAppProg_Platform_Partition;





typedef void cInAppProg_Platform_vJump(void);

void   InAppProg_Platform_vStartWdog();
void   InAppProg_Platform_vResetWdog();

void   InAppProg_Platform_vFlash_Unlock();
void   InAppProg_Platform_vFlash_Lock();
void   InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress);
void   InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size);

void   InAppProg_Platform_vJumpToApp(uint8* lpui8Adr);
void   InAppProg_Platform_vCall(uint8* lpui8Adr, uint32* luip32Param);

uint16 InAppProg_Platform_u16GetMM(uint8* lpui8Adress);

//void   InAppProg_PlatformvSystemReset();


#ifdef __cplusplus
}
#endif

#endif // __IAP_PLATFORM_H__

