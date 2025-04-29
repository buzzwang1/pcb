#include "iap_platform.h"


const tstInAppProg_Platform_Partition stInAppProg_Platform_Partitions[6] =
{
  {
   //1   2   3   4   5   6     7    8   9   10    11    12    13   14   15   16   17   18   19   20   21   22   23   24
    'F','l','a','s','h', ' ', 'B', 'L', 0, 0x55, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    16,
    0,
    0x08000000,
    16*1024,
    0x0800,
  },
  {
   //1   2   3   4   5   6     7    8    9  10   11    12    13   14   15   16   17   18   19   20   21   22   23   24
    'F','l','a','s','h', ' ', 'A', 'P', 'P', 0, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    16,
    0,
    0x08000000 + 16*1024,
    (160)*1024,
    0x0800,
  },
  {
   //1   2   3   4   5   6     7    8    9  10   11    12    13   14   15   16   17   18   19   20   21   22   23   24
    'F','l','a','s','h', ' ', 'I', 'A', 'P', 0, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    16,
    0,
    0x08000000 + (16+160)*1024,
    (76)*1024,
    0x0800,
  },
  {
   //1   2   3   4   5   6     7    8    9  10   11    12    13   14   15   16   17   18   19   20   21   22   23   24
    'F','l','a','s','h', ' ', 'E', 'E', 'P', 0, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    16,
    0,
    0x08000000 + (16+160+76)*1024,
    (4)*1024,
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



void InAppProg_Platform_vStartWdog()
{
  /* 40khz / 256 = 6.4ms
     6.4ms * 150 = 960ms
     IWDG timeout equal to 960 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_256);

  /* Set counter reload value to obtain 250ms IWDG TimeOut.*/
  IWDG_SetReload(150);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}


void InAppProg_Platform_vResetWdog()
{
  IWDG_ReloadCounter();
}

bool InAppProg_Platform_IsFlash(uint8* lpui8Adress)
{
  if (((uint32)lpui8Adress >= stInAppProg_Platform_Partitions[0].ui32Start) &&
      ((uint32)lpui8Adress < (stInAppProg_Platform_Partitions[0].ui32Start + stInAppProg_Platform_Partitions[0].ui32Size))
      )
  {
    return True;
  }
  return False;
}

void InAppProg_Platform_vFlash_Unlock()
{
  /* Unlock the Flash to enable the flash control register access *************/
  FLASH_Unlock();
}

void InAppProg_Platform_vFlash_Lock()
{
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock();
}


uint32 InAppProg_Platform_ui32Flash_Size2Pages(uint32 lui32Size)
{
  uint32 lui32Pages;

  lui32Pages = lui32Size / stInAppProg_Platform_Partitions[0].ui32Block_Size;

  if (lui32Pages * stInAppProg_Platform_Partitions[0].ui32Block_Size < lui32Size)
  {
    lui32Pages++;
  }

  return lui32Pages;
}

void InAppProg_Platform_vFlash_EreasePage(uint8* lpui8PageStartAdress)
{
  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Clear pending flags (if any) */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);

  if (FLASH_ErasePage((uint32)lpui8PageStartAdress)!= FLASH_COMPLETE)
  {
     /* Error occurred while sector erase.
         User can add here some code to deal with this error  */
    //mui8Error = 1;
  }
}

void InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  uint32 lui32Data;

  lui32Size &= 0xFFFFFFFC;

  while (lui32Size > 3)
  {
    lui32Data = *((uint32*)lpui8Src);

    if (FLASH_ProgramWord((uint32)lpui8Adress, lui32Data) == FLASH_COMPLETE)
    {
      lpui8Adress += 4;
      lpui8Src    += 4;
      lui32Size   -= 4;
    }
    else
    {
      /* Error occurred while writing data in Flash memory.
         User can add here some code to deal with this error */
      //mui8Error = 2;
      break;
    }
  }
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

