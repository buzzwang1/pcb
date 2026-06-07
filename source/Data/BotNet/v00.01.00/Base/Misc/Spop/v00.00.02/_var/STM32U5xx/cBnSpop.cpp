#include "cBnSpop.h"

//FLASH_CacheTypeDef cBnSpop_menFlash_CacheToReactivate;


void cBnSpop_vStartWdog()
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


void cBnSpop_vResetWdog()
{
  IWDG->KR = IWDG_KEY_RELOAD;
}

void cBnSpop_vDeactivateIWgdDuringSleep()
{
  FLASH_OBProgramInitTypeDef OB;
  HAL_FLASHEx_OBGetConfig(&OB);

  // check if
  if ((OB.USERConfig & FLASH_OPTR_IWDG_STDBY) ||
      (OB.USERConfig & FLASH_OPTR_IWDG_STOP))
  {
    HAL_FLASH_Unlock();
    HAL_FLASH_OB_Unlock();

    OB.OptionType = OPTIONBYTE_USER;
    OB.USERType   = OB_USER_IWDG_STOP | OB_USER_IWDG_STDBY;
    OB.USERConfig = OB_IWDG_STOP_FREEZE | OB_IWDG_STDBY_FREEZE;

    if (HAL_FLASHEx_OBProgram(&OB) != HAL_OK)
    {
      HAL_FLASH_OB_Lock();
      HAL_FLASH_Lock();
      return;
    }
    HAL_FLASH_OB_Launch();
  }
}


uint16 cBnSpop_u16GetPartition(uint8* lpui8Adress)
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

  return RomConst_Error;
}


uint16 cBnSpop_u16GetMemorySortByAdr(uint8* lpui8Adress)
{
  u16 lu16t;
  const tstMemPart_Partition* lpPartition;

  lu16t = cBnSpop_u16GetPartition(lpui8Adress);
  RomConst_Platform_vLoadPartition(lu16t, lpPartition);

  if (lu16t < RomConst_stDevice_Info->u8PartitionCnt)
  {
    return lpPartition->ui16Sort;
  }

  return RomConst_Error;
}

uint16 cBnSpop_u16GetMemorySortByMemIdx(u8 lu8MemIdx)
{
  const tstMemPart_Partition* lpPartition;

  if (lu8MemIdx < RomConst_stDevice_Info->u8PartitionCnt)
  {
    RomConst_Platform_vLoadPartition(lu8MemIdx, lpPartition);
    return lpPartition->ui16Sort;
  }
  return RomConst_Error;
}


uint32 cBnSpop_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size)
{
  uint32 lui32Pages;
  u16 lu16t;
  const tstMemPart_Partition* lpPartition;

  lu16t = cBnSpop_u16GetPartition(lpui8Adress);
  RomConst_Platform_vLoadPartition(lu16t, lpPartition);
  lui32Pages = lui32Size / lpPartition->ui32Block_Size;

  if (lui32Pages * lpPartition->ui32Block_Size < lui32Size)
  {
    lui32Pages++;
  }

  return lui32Pages;
}


//void cBnSpop_vFlash_FlushCaches(void)
//{
//  FLASH_CacheTypeDef cache = cBnSpop_menFlash_CacheToReactivate;
//
//  /* Flush instruction cache  */
//  if ((cache == FLASH_CACHE_ICACHE_ENABLED) ||
//      (cache == FLASH_CACHE_ICACHE_DCACHE_ENABLED))
//  {
//    /* Reset instruction cache */
//    __HAL_FLASH_INSTRUCTION_CACHE_RESET();
//    /* Enable instruction cache */
//    __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();
//  }
//
//  /* Flush data cache */
//  if ((cache == FLASH_CACHE_DCACHE_ENABLED) ||
//      (cache == FLASH_CACHE_ICACHE_DCACHE_ENABLED))
//  {
//    /* Reset data cache */
//    __HAL_FLASH_DATA_CACHE_RESET();
//    /* Enable data cache */
//    __HAL_FLASH_DATA_CACHE_ENABLE();
//  }
//
//  /* Reset internal variable */
//  cBnSpop_menFlash_CacheToReactivate = FLASH_CACHE_DISABLED;
//}


void cBnSpop_vFlash_Unlock()
{
  /* Unlock the Flash to enable the flash control register access *************/
  HAL_FLASH_Unlock();
}

void cBnSpop_vFlash_Lock()
{
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  HAL_FLASH_Lock();
}




void cBnSpop_vFlash_ErasePage_Start(uint8* lpui8PageStartAdress)
{
  cBnSpop_isFlash_Error();
  if (!cBnSpop_isFlash_Busy())
  {
    // Deactivate the cache if they are activated to avoid data misbehavior 
    //if(READ_BIT(FLASH->ACR, FLASH_ACR_ICEN) != 0U)
    //{
    //  /* Disable instruction cache  */
    //  __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
    //
    //  if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U)
    //  {
    //    /* Disable data cache  */
    //    __HAL_FLASH_DATA_CACHE_DISABLE();
    //    cBnSpop_menFlash_CacheToReactivate = FLASH_CACHE_ICACHE_DCACHE_ENABLED;
    //  }
    //  else
    //  {
    //    cBnSpop_menFlash_CacheToReactivate = FLASH_CACHE_ICACHE_ENABLED;
    //  }
    //}
    //else if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U)
    //{
    //  /* Disable data cache  */
    //  __HAL_FLASH_DATA_CACHE_DISABLE();
    //  cBnSpop_menFlash_CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
    //}
    //else
    //{
    //  cBnSpop_menFlash_CacheToReactivate = FLASH_CACHE_DISABLED;
    //}

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

void cBnSpop_vFlash_ErasePage_Finish()
{
  /* If the erase operation is completed, disable the PER Bit */
  CLEAR_BIT(FLASH->NSCR, (FLASH_NSCR_PER | FLASH_NSCR_PNB));
  //cBnSpop_vFlash_FlushCaches();
}

// Nur 128Bit (16 Byte) Worte an 16Byte aligned Adressen erlaubt
u32 cBnSpop_vFlash_Program_Start(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  uint32 lui32CheckSum;
  uint8  lu8t;

   // Auf 16 Byte trimmen
  lui32Size >>= 4;
  lui32Size <<= 4;
  lui32CheckSum = 0;

  cBnSpop_isFlash_Error();
  if (!cBnSpop_isFlash_Busy())
  {
    //if(READ_BIT(FLASH->ACR, FLASH_ACR_DCEN) != 0U)
    //{
    //  /* Disable data cache  */
    //  __HAL_FLASH_DATA_CACHE_DISABLE();
    //  cBnSpop_menFlash_CacheToReactivate = FLASH_CACHE_DCACHE_ENABLED;
    //}
    //else
    //{
    //  cBnSpop_menFlash_CacheToReactivate = FLASH_CACHE_DISABLED;
    //}

    while (lui32Size > 0)
    {
      SET_BIT(FLASH->NSCR, FLASH_NSCR_PG);

      // Checksumme der nächsten 4 Byte
      for (lu8t = 0; lu8t < 4; lu8t++)
      {
        lui32CheckSum += lpui8Src[lu8t];
      }

      // Enter critical section: Disable interrupts to avoid any interruption during the loop
      _dai()

      // Program the quad-word
      *(u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;
      *(u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;
      *(u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;
      *(u32*)lpui8Adress = *((u32*)lpui8Src);
      lpui8Adress += 4; lpui8Src += 4; lui32Size -= 4;


      // Exit critical section: restore previous priority mask
      _eai();

      while (cBnSpop_isFlash_Busy()) {};

      CLEAR_BIT(FLASH->NSCR, FLASH_NSCR_PG);
    }
  }

  return lui32CheckSum;
}

void cBnSpop_vFlash_Program_Finish()
{
  CLEAR_BIT(FLASH->NSCR, FLASH_NSCR_PG);
  //cBnSpop_vFlash_FlushCaches();
}

bool cBnSpop_isFlash_Busy()
{
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_BSY))
  {
    return True;
  }

  // FLASH_FLAG_BSY : FLASH write / erase operations in progress flag
  // FLASH_FLAG_WDW : FLASH Wait Data to Write flag
  if (__HAL_FLASH_GET_FLAG(FLASH_FLAG_EOP) || __HAL_FLASH_GET_FLAG(FLASH_FLAG_WDW))
  {
    /* Clear FLASH End of Operation pending bit */
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP);
  }
  return False;
}

bool cBnSpop_isFlash_Error()
{
  u32 lu32Error;

  lu32Error = (FLASH->NSSR & FLASH_FLAG_SR_ERRORS);

  if (lu32Error != 0u)
  {
    __HAL_FLASH_CLEAR_FLAG(lu32Error);
    return True;
  }
  return False;
}


void cBnSpop_vSystemReset()
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


void cBnSpop_vJumpToApp(uint8* lpui8Adr)
{
  cBnSpop_tvJump* lpvFunc;
  uint32      lui32JumpAddress;

  if ((uint32)lpui8Adr == 0)
  {
    cBnSpop_vSystemReset();
  }
  else
  {
    /* Jump to user application */
    lui32JumpAddress = *(__IO uint32*) (lpui8Adr + 4);
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32*) lpui8Adr);

    lpvFunc = (cBnSpop_tvJump*)(lui32JumpAddress);
    lpvFunc();
  }
}

void cBnSpop_vCall(uint8* lpui8Adr, uint32* luip32Param)
{
  cBnSpop_tvCall* lpvFunc;

  if ((uint32)lpui8Adr == 0)
  {
    cBnSpop_vSystemReset();
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
    lpvFunc = (cBnSpop_tvCall*)(lpui8Adr + 1);
    lpvFunc(luip32Param[0], luip32Param[1], luip32Param[2], luip32Param[4]);
  }
}
