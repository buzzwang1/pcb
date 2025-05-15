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

/*void RomConst_Platform_vLoadPartition(u8 lu8PartitionIdx, tstRomConst_Platform_Device_Info* lpPartition)
{
  u8* lpu8PartitionRom;
  u8* lpu8PartitionRam;

  lpu8PartitionRam = (u8*)lpPartition;
  lpu8PartitionRom = (u8*)InAppProg_Partition(lu8PartitionIdx);

  //32 Byte vom ROM in RAM kopieren
  for (u8 lu8t = 0; lu8t < 16; lu8t++)
  {
    *lpu8PartitionRam++ = *lpu8PartitionRom++;
  }

  lpPartition->ui16Sort  = IAP_u16Getu16(lpu8PartitionRom);lpu8PartitionRom+=2;
  lpPartition->ui16Type  = IAP_u16Getu16(lpu8PartitionRom);lpu8PartitionRom+=2;
  lpPartition->ui32Start = IAP_u32Getu32(lpu8PartitionRom);lpu8PartitionRom+=4;
  lpPartition->ui32Size  = IAP_u32Getu32(lpu8PartitionRom);lpu8PartitionRom+=4;
  lpPartition->ui32Block_Size =  IAP_u32Getu32(lpu8PartitionRom);
}*/



uint16 InAppProg_Platform_u16GetPartition(uint8* lpui8Adress)
{
  u16 lu16t;
  const tstRomConst_Platform_Partition* lpPartition;

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
  const tstRomConst_Platform_Partition* lpPartition;

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
  const tstRomConst_Platform_Partition* lpPartition;

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

u8 InAppProg_Platform_isFlash_Busy()
{
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
  {
    return 1;
  }

  // FLASH_FLAG_BSY : FLASH write / erase operations in progress flag
  // FLASH_FLAG_WDW : FLASH Wait Data to Write flag
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) || __HAL_FLASH_GET_FLAG(FLASH_FLAG_WDW))
  {
    /* Clear FLASH End of Operation pending bit */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
  }
  return 0;
}

u8 InAppProg_Platform_isFlash_Error()
{
  u32 lu32Error;

  lu32Error = (FLASH->NSSR & FLASH_FLAG_SR_ERRORS);

  if (lu32Error != 0u)
  {
    __HAL_FLASH_CLEAR_FLAG(lu32Error);
    return 1;
  }
  return 0;
}

void InAppProg_Platform_vFlash_ErasePage_Start(uint8* lpui8PageStartAdress)
{
  InAppProg_Platform_isFlash_Error();
  if (!InAppProg_Platform_isFlash_Busy())
  {
    u32 lu32Page;

    if ((u32)lpui8PageStartAdress < 0x08100000)
    {
      // Bank1
      CLEAR_BIT(FLASH->NSCR, FLASH_NSCR_BKER);
      lu32Page = (u32)lpui8PageStartAdress - 0x08000000;
    }
    else
    {
      // Bank2
      SET_BIT(FLASH->NSCR, FLASH_NSCR_BKER);
      lu32Page = (u32)lpui8PageStartAdress - 0x08100000;
    }

    lu32Page >>= 13; // /= 8192;

    MODIFY_REG(FLASH->NSCR, (FLASH_NSCR_PNB | FLASH_NSCR_PER | FLASH_NSCR_STRT), \
      ((lu32Page << FLASH_NSCR_PNB_Pos) | FLASH_NSCR_PER | FLASH_NSCR_STRT));
  }
}

void InAppProg_Platform_vErasePage_Finish()
{
  /* If the erase operation is completed, disable the PER Bit */
  CLEAR_BIT(FLASH->NSCR, (FLASH_NSCR_PER | FLASH_NSCR_PNB));
}

void InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress)
{
  InAppProg_Platform_vFlash_ErasePage_Start(lpui8PageStartAdress);
  while (InAppProg_Platform_isFlash_Busy());
  InAppProg_Platform_vErasePage_Finish();
}


// Nur 128Bit (16 Byte) Worte an 16Byte aligned Adressen erlaubt
u32 InAppProg_Platform_Program_Start(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  uint32 lui32CheckSum;
  uint8  lu8t;

  // Auf 16 Byte trimmen
  lui32Size >>= 4;
  lui32Size <<= 4;
  lui32CheckSum = 0;

  InAppProg_Platform_isFlash_Error();
  if (!InAppProg_Platform_isFlash_Busy())
  {
    while (lui32Size > 0)
    {
      SET_BIT(FLASH->NSCR, FLASH_NSCR_PG);

      // Checksumme der nächsten 4 Byte
      for (lu8t = 0; lu8t < 4; lu8t++)
      {
        lui32CheckSum += lpui8Src[lu8t];
      }

      // Enter critical section: Disable interrupts to avoid any interruption during the loop
      //_dai()

        // Program the quad-word
        * (u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;
      *(u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;
      *(u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;
      *(u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;


      // Exit critical section: restore previous priority mask
      //_eai();

      while (InAppProg_Platform_isFlash_Busy()) {};

      CLEAR_BIT(FLASH->NSCR, FLASH_NSCR_PG);
    }
  }

  return lui32CheckSum;
}

void InAppProg_Platform_vFlash_Program_Finish()
{
  CLEAR_BIT(FLASH->NSCR, FLASH_NSCR_PG);
}


// Nur 64Bit (8 Byte) Worte an 8Byte aligned Adressen erlaubt
u32 InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  uint32 lui32CheckSum;

  lui32CheckSum = InAppProg_Platform_Program_Start(lpui8Adress, lpui8Src, lui32Size);
  while (InAppProg_Platform_isFlash_Busy());
  InAppProg_Platform_vFlash_Program_Finish();

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

#if 0
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
#endif
