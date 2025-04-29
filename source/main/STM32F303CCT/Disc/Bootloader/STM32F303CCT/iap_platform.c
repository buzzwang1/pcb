#include "iap_platform.h"

const tstInAppProg_Platform_Partition stInAppProg_Platform_Partitions[IAP_PARTITION_COUNT] =
{
  {
   //1   2   3   4   5   6     7    8   9   10    11    12    13   14   15   16   17   18   19   20   21   22   23   24
   //'F','l','a','s','h', ' ', 'B', 'L', 0, 0x55, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    IAP_MM_FLASH,
    0,
    0x08000000,
    16*1024,
    0x0800, // 2kb
  },
  {
   //1   2   3   4   5   6     7    8    9  10   11    12    13   14   15   16   17   18   19   20   21   22   23   24
   //'F','l','a','s','h', ' ', 'A', 'P', 'P', 0, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    IAP_MM_FLASH,
    0,
    0x08000000 + 16*1024,
    (256-16)*1024,
    0x0800,
  },
  {
   //1  2   3    4   5      6     7     8     9     10    11    12    13   14   15   16   17   18   19   20   21   22   23   24
   //'R','a','m', 0, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    IAP_MM_RAM,
    0,
    0x20000000,
    40*1024,
    1,
  },
  {
   //1  2   3    4   5      6     7     8     9     10    11    12    13   14   15   16   17   18   19   20   21   22   23   24
   //'C','C','M', 0, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    IAP_MM_RAM,
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

uint16 InAppProg_Platform_u16GetMM(uint8* lpui8Adress)
{
  u16 lu16Ret;
  u16 lu16t;

  lu16Ret = IAP_MM_ERROR;

  for (lu16t = 0; lu16t < IAP_PARTITION_COUNT; lu16t++)
  {
    if (((uint32)lpui8Adress >= stInAppProg_Platform_Partitions[lu16t].ui32Start) &&
        ((uint32)lpui8Adress < (stInAppProg_Platform_Partitions[lu16t].ui32Start + stInAppProg_Platform_Partitions[lu16t].ui32Size))
        )
    {
      return stInAppProg_Platform_Partitions[lu16t].ui16Sort;
    }    
  }

  return lu16Ret;
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


