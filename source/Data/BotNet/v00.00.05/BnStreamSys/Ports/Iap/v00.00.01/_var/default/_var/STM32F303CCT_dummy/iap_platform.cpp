#include "iap.h"



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
  /* Always true, um nicht ausversehen den RAM zu überschreiben */
  return True;
}

void InAppProg_Platform_vFlash_Unlock()
{
}

void InAppProg_Platform_vFlash_Lock()
{
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
}

uint32 InAppProg_Platform_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  return 0;
}


void InAppProg_Platform_vSystemReset()
{
}


void InAppProg_Platform_vJumpToApp(uint8* lpui8Adr)
{
}

void InAppProg_Platform_vCall(uint8* lpui8Adr, uint32* luip32Param)
{
}

