#include "iap_platform.h"



void InAppProg_Platform_vStartWdog()
{
}


void InAppProg_Platform_vResetWdog()
{
}


uint16 InAppProg_Platform_u16GetPartition(uint8* lpui8Adress)
{
  u16 lu16t;

  for (lu16t = 0; lu16t < InAppProg_Partition_Count; lu16t++)
  {
    if (((uint32)lpui8Adress >= stInAppProg_Platform_Partitions[lu16t].ui32Start) &&
        ((uint32)lpui8Adress < (stInAppProg_Platform_Partitions[lu16t].ui32Start + stInAppProg_Platform_Partitions[lu16t].ui32Size))
        )
    {
      return lu16t;
    }    
  }

  return InAppProg_Error;
}


uint16 InAppProg_Platform_u16GetMemorySort(uint8* lpui8Adress)
{
  u16 lu16t;

  lu16t = InAppProg_Platform_u16GetPartition(lpui8Adress);

  if (lu16t < InAppProg_Partition_Count)
  {
      return stInAppProg_Platform_Partitions[lu16t].ui16Sort;
  }

  return InAppProg_Error;
}


uint32 InAppProg_Platform_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size)
{
  uint32 lui32Pages;
  u16 lu16t;

  lu16t = InAppProg_Platform_u16GetPartition(lpui8Adress);
  lui32Pages = lui32Size / stInAppProg_Platform_Partitions[lu16t].ui32Block_Size;

  if (lui32Pages * stInAppProg_Platform_Partitions[lu16t].ui32Block_Size < lui32Size)
  {
    lui32Pages++;
  }

  return lui32Pages;
}


void InAppProg_Platform_vFlash_Unlock()
{
  /* Unlock the Flash to enable the flash control register access *************/
  //HAL_FLASH_Unlock();
}

void InAppProg_Platform_vFlash_Lock()
{
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  //HAL_FLASH_Lock();
}


void InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress)
{
  (void)lpui8PageStartAdress; /* avoid warning */
}

u32 InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  (void)lpui8Adress; /* avoid warning */
  (void)lpui8Src; /* avoid warning */
  (void)lui32Size; /* avoid warning */
  return 0;
}


void InAppProg_Platform_vSystemReset()
{
}


void InAppProg_Platform_vJumpToApp(uint8* lpui8Adr)
{
  (void)lpui8Adr; /* avoid warning */
}

void InAppProg_Platform_vCall(uint8* lpui8Adr, uint32* luip32Param)
{
  (void)lpui8Adr; /* avoid warning */
  (void)luip32Param; /* avoid warning */
}

