#include "RomConst.h"

const tstRomConst_Platform_Device_Info* RomConst_stDevice_Info = (tstRomConst_Platform_Device_Info *)ROMCONST_PARTITION_START_ADRESS_EEP;


u8 RomConst_IsHeaderPlausible()
{
  // Just check the first 4 bytes of the romconst
  // If the Flash is ereared, it content is 0xFF
  static const u8 lu8EmtpyFlash[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
  if (*((u32*)RomConst_stDevice_Info->szDevice_Name) != *((u32*)(lu8EmtpyFlash)))
  {
    // Unplausible
    return 0;
  }
  return 1;
}


u8 RomConst_IsMagicPatternOk()
{
  static const u8 lu8MagicPattern[4] = { 'F', 'L', 'K', 'A' };
  if (RomConst_stDevice_Info->u32MagicPattern != *((u32*)(lu8MagicPattern)))
  {
    // Magic Pattern Error
    return 0;
  }
  return 1;
}


u8 RomConst_IsValid()
{
  static const u8 lu8MagicPattern[4] = { 'F', 'L', 'K', 'A' };
  if (RomConst_stDevice_Info->u32MagicPattern != *((u32*)(lu8MagicPattern)))
  {
    // Magic Pattern Error
    return 0;
  }

  // Check Checksum
  u32 lu32Checksum = 1;
  u8* lu8pData    = (u8*)ROMCONST_PARTITION_START_ADRESS_EEP;
  u8* lu8pDataEnd = (u8*)ROMCONST_PARTITION_START_ADRESS_EEP +  sizeof(tstRomConst_Platform_Device_Info) - 8;
  while (lu8pData < lu8pDataEnd)
  {
    lu32Checksum += *lu8pData;
    lu8pData++;
  }

  if (RomConst_stDevice_Info->u32Checksum != lu32Checksum)
  {
    // Checksum Error
    return 0;
  }

  return 1;
}
