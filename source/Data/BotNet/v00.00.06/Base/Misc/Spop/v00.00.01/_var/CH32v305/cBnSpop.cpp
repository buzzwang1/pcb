#include "cBnSpop.h"


void cBnSpop_vStartWdog()
{
  /* 40khz / 256 = 6.4ms
     6.4ms * 150 = 960ms */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_SetPrescaler(IWDG_Prescaler_256);
  IWDG_SetReload(150);
  IWDG_ReloadCounter();
  IWDG_Enable();
}


void cBnSpop_vResetWdog()
{
  IWDG_ReloadCounter();
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


uint16 cBnSpop_u16GetMemorySort(uint8* lpui8Adress)
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


void cBnSpop_vFlash_Unlock()
{
  /* Unlock the Flash to enable the flash control register access *************/
  FLASH_Unlock();
}

void cBnSpop_vFlash_Lock()
{
  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock();
}


// CH32V307DS0.PDF
// Symbol       Parameter                          Conditions        Min Typ Max Unit
// Fprog        Programming frequency)             TA = -40°C~85°C           60  MHz
// tprog_page   Page(256 bytes) programming time   TA = -40°C~85°C        2 2.5  ms
// terase_page  Page(256 bytes) erase time         TA = -40°C~85°C       16  20  ms
// terase_sec   Sector(4K bytes) erase time        TA = -40°C~85°C       16  20  ms
void cBnSpop_vFlash_EreasePage(uint8* lpui8PageStartAdress)
{
  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Clear pending flags (if any) */
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);

  if (FLASH_ErasePage((uint32)lpui8PageStartAdress) != FLASH_COMPLETE)
  {
    // Error occurred while sector erase.
    //  User can add here some code to deal with this error
   //__asm("nop");
  }
}

// GD32F130xx_Datasheet_Rev3.1.pdf
// Symbol       Parameter                          Conditions        Min Typ Max Unit
// Fprog        Programming frequency)             TA = -40°C~85°C           60  MHz
// tprog_page   Page(256 bytes) programming time   TA = -40°C~85°C        2 2.5  ms
// terase_page  Page(256 bytes) erase time         TA = -40°C~85°C       16  20  ms
// terase_sec   Sector(4K bytes) erase time        TA = -40°C~85°C       16  20  ms
u32 cBnSpop_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  uint32 lui16Data;
  uint32 lui32CheckSum;

  lui32Size >>= 1;
  lui32Size <<= 1;
  lui32CheckSum = 0;

  while (lui32Size > 0)
  {
    lui16Data = *((uint16*)lpui8Src);
    lui32CheckSum += *lpui8Src;
    lui32CheckSum += *(lpui8Src + 1);

    if (FLASH_ProgramHalfWord((uint32)lpui8Adress, lui16Data) == FLASH_COMPLETE)
    {
      lpui8Adress += 2;
      lpui8Src += 2;
      lui32Size -= 2;
    }
    else
    {
      // Error occurred while writing data in Flash memory.
      //   User can add here some code to deal with this error
      //mui8Error = 2;
      break;
    }
  }

  return lui32CheckSum;
}


void cBnSpop_vSystemReset()
{
  NVIC_SystemReset();
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
    ////__set_MSP(*(__IO uint32*) lpui8Adr);

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

void cBnSpop_vWriteBackupData(uint16 lu16Idx, u32 lu32Value)
{
  /* enable PMU clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* PMU backup domain write enable */
  PWR_BackupAccessCmd(ENABLE);

  // Backupregister haben nur 16 Bit
  // Zwei 16Bit Backupregister zu einen 32Bit zusammenfügen
  volatile u32* lpu32Bkp16Bit;
  if (lu16Idx < 5)
  {
    lpu32Bkp16Bit = (volatile u32*)(u32)(&BKP->DATAR1);
  }
  else
  {
    lpu32Bkp16Bit = (volatile u32*)(u32)(&BKP->DATAR11);
  }
  
  lu16Idx *= 2;
  lpu32Bkp16Bit[lu16Idx]     = lu32Value;
  lpu32Bkp16Bit[lu16Idx + 1] = lu32Value >> 16;
}

u32 cBnSpop_u32ReadBackupData(uint16 lu16Idx)
{
  /* enable PMU clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
  /* PMU backup domain write enable */
  PWR_BackupAccessCmd(ENABLE);

  // Backupregister haben nur 16 Bit
  // Zwei 16Bit Backupregister zu einen 32Bit zusammenfügen
  volatile u32* lpu32Bkp16Bit;
  if (lu16Idx < 5)
  {
    lpu32Bkp16Bit = (volatile u32*)(u32)(&BKP->DATAR1);
  }
  else
  {
    lpu32Bkp16Bit = (volatile u32*)(u32)(&BKP->DATAR11);
  }
  lu16Idx *= 2;
  return lpu32Bkp16Bit[lu16Idx] + (lpu32Bkp16Bit[lu16Idx + 1] << 16);
}
