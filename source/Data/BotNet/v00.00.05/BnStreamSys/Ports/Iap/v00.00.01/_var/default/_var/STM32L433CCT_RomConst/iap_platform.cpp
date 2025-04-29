#include "iap_platform.h"


void InAppProg_Platform_vStartWdog()
{
  /* 32khz / 256 = 8.0ms
     8.0ms * 150 = 1200ms
     IWDG timeout equal to 960 ms (the timeout may varies due to LSI frequency
     dispersion) */

  IWDG_HandleTypeDef lhiwdg;

  lhiwdg.Instance = IWDG;

  lhiwdg.Init.Prescaler = IWDG_PRESCALER_256;
  lhiwdg.Init.Reload    = 150;
  lhiwdg.Init.Window    = IWDG_WINDOW_DISABLE;

  HAL_IWDG_Init(&lhiwdg);
}


void InAppProg_Platform_vResetWdog()
{
  IWDG->KR = IWDG_KEY_RELOAD;
}


uint16 InAppProg_Platform_u16GetPartition(uint8* lpui8Adress)
{
  u16 lu16t;
  const tstMemPart_Partition* lpPartition;

  for (lu16t = 0; lu16t < RomConst_stDevice_Info->u8PartitionCnt; lu16t++)
  {
    RomConst_Platform_vLoadPartition(lu16t, lpPartition);
    if (((uint32)lpui8Adress >= lpPartition->ui32Start) &&
        ((uint32)lpui8Adress < (lpPartition->ui32Start + lpPartition->ui32Size))
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
  const tstMemPart_Partition* lpPartition;

  lu16t = InAppProg_Platform_u16GetPartition(lpui8Adress);
  RomConst_Platform_vLoadPartition(lu16t, lpPartition);

  if (lu16t < RomConst_stDevice_Info->u8PartitionCnt)
  {
      return lpPartition->ui16Sort;
  }

  return InAppProg_Error;
}


uint32 InAppProg_Platform_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size)
{
  uint32 lui32Pages;
  u16 lu16t;
  const tstMemPart_Partition* lpPartition;

  lu16t = InAppProg_Platform_u16GetPartition(lpui8Adress);
  RomConst_Platform_vLoadPartition(lu16t, lpPartition);
  lui32Pages = lui32Size / lpPartition->ui32Block_Size;

  if (lui32Pages * lpPartition->ui32Block_Size < lui32Size)
  {
    lui32Pages++;
  }

  return lui32Pages;
}


void InAppProg_Platform_vFlash_Unlock()
{
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
}

void InAppProg_Platform_vFlash_Lock()
{
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
}



void InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress)
{
  FLASH_EraseInitTypeDef EraseInitStruct;
  u32                    Dummy;


  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES; 
  EraseInitStruct.Page    = (u32)lpui8PageStartAdress;
  EraseInitStruct.Page   -= 0x08000000;
  EraseInitStruct.Page  >>= 11; // /= 2048;
  EraseInitStruct.Banks   =  FLASH_BANK_1;
  EraseInitStruct.NbPages = 1;

  if (HAL_FLASHEx_Erase(&EraseInitStruct, &Dummy) != HAL_OK)
  {
     /* Error occurred while sector erase.
         User can add here some code to deal with this error  */
    //mui8Error = 1;
  }
 
}

// Nur 64Bit (8 Byte) Worte an 8Byte aligned Adressen erlaubt
u32 InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  uint64 lu64Data;
  uint32 lui32CheckSum;
  uint8  lu8t;

  lui32Size >>= 1;
  lui32Size <<= 1;
  lui32CheckSum = 0;

  while (lui32Size > 0)
  {
    lu64Data       = *((u64*)lpui8Src);
    for (lu8t = 0; lu8t < 8; lu8t++)
    {
      lui32CheckSum += lpui8Src[lu8t];
    }
    
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32)lpui8Adress, lu64Data) == HAL_OK)
    {
      lpui8Adress += 8;
      lpui8Src    += 8;
      lui32Size   -= 8;
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      //mui8Error = 2;
      break;
    }
  }

  return lui32CheckSum;
}


void InAppProg_Platform_vSystemReset()
{
  __asm volatile ("dsb 0xF":::"memory");                            /* Ensure all outstanding memory accesses included
                                                                       buffered write are completed before reset */
  SCB->AIRCR  = (uint32_t)((0x5FAUL << SCB_AIRCR_VECTKEY_Pos)    |
                           (SCB->AIRCR & SCB_AIRCR_PRIGROUP_Msk) |
                            SCB_AIRCR_SYSRESETREQ_Msk    );         /* Keep priority group unchanged */
  __asm volatile ("dsb 0xF":::"memory");                            /* Ensure completion of memory access */

  for(;;)                                                           /* wait until reset */
  {
    __asm volatile ("nop");
  }
}


void InAppProg_Platform_vJumpToApp(uint8* lpui8Adr)
{
  cInAppProg_Platform_vJump* lpvFunc;
  uint32      lui32JumpAddress;

  if ((uint32)lpui8Adr == 0)
  {
    InAppProg_Platform_vSystemReset();
  }
  else
  {
    /* Jump to user application */
    lui32JumpAddress = *(__IO uint32*) (lpui8Adr + 4);
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32*) lpui8Adr);

    lpvFunc = (cInAppProg_Platform_vJump*)(lui32JumpAddress);
    lpvFunc();
  }
}

void InAppProg_Platform_vCall(uint8* lpui8Adr, uint32* luip32Param)
{
  cInAppProg_Platform_vCall* lpvFunc;

  if ((uint32)lpui8Adr == 0)
  {
    InAppProg_Platform_vSystemReset();
  }
  else
  {
    /* Add 1 !!! https://community.nxp.com/thread/113048
     *
     * The trick to the BX command is, It uses the address of the destination
     * to determine what instruction mode it jumps into. An even instruction
     * address changes the instruction mode to ARM 7. An odd instruction address
     * keeps the processor in Thumb mode. The ARM core can only run instructions
     * on even addresses. The core masks off the least significant bit of an
     * address and uses it to detremine the instruction set it is running in.
     * So to jump to a bootloader address and stay in thumb mode, you must add 1
     * to the destination address.
     * */
    lpvFunc = (cInAppProg_Platform_vCall*)(lpui8Adr + 1);
    lpvFunc(luip32Param[0], luip32Param[1], luip32Param[2], luip32Param[4]);
  }
}

