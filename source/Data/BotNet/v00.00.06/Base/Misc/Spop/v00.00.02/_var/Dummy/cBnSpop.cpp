#include "cBnSpop.h"


void cBnSpop_vStartWdog()
{
}


void cBnSpop_vResetWdog()
{
}


uint16 cBnSpop_u16GetPartition(uint8* lpui8Adress)
{
  UNUSED(lpui8Adress);
  return RomConst_Error;
}


uint16 cBnSpop_u16GetMemorySortByAdr(uint8* lpui8Adress)
{
  UNUSED(lpui8Adress);
  return RomConst_Error;
}

uint16 cBnSpop_u16GetMemorySortByMemIdx(u8 lu8MemIdx)
{
  UNUSED(lu8MemIdx);
  return RomConst_Error;
}


uint32 cBnSpop_ui32Flash_Size2Pages(uint8* lpui8Adress, uint32 lui32Size)
{
  UNUSED(lpui8Adress);
  UNUSED(lui32Size);

  return 0;
}


void cBnSpop_vFlash_Unlock()
{
}

void cBnSpop_vFlash_Lock()
{
}

void cBnSpop_vFlash_ErasePage_Start(uint8* lpui8PageStartAdress)
{
  UNUSED(lpui8PageStartAdress);
}

void cBnSpop_vFlash_ErasePage_Finish()
{
}

u32 cBnSpop_vFlash_Program(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  UNUSED(lpui8Adress);
  UNUSED(lpui8Src);
  UNUSED(lui32Size);

  return 0;
}

u32 cBnSpop_vFlash_Program_Start(uint8* lpui8Adress, uint8* lpui8Src, uint32 lui32Size)
{
  UNUSED(lpui8Adress);
  UNUSED(lpui8Src);
  UNUSED(lui32Size);

  return 0;
}

void cBnSpop_vFlash_Program_Finish()
{
}

bool cBnSpop_isFlash_Busy()
{
  return False;
}

bool cBnSpop_isFlash_Error()
{
  return False;
}


void cBnSpop_vSystemReset()
{
}


void cBnSpop_vJumpToApp(uint8* lpui8Adr)
{
  UNUSED(lpui8Adr);
}

void cBnSpop_vCall(uint8* lpui8Adr, uint32* luip32Param)
{
  UNUSED(lpui8Adr);
  UNUSED(luip32Param);
}

void cBnSpop_vWriteBackupData(uint16 lu16Idx, u32 lu32Value)
{
  UNUSED(lu16Idx);
  UNUSED(lu32Value);
}

u32 cBnSpop_u32ReadBackupData(uint16 lu16Idx)
{
  UNUSED(lu16Idx);

  return 0;
}