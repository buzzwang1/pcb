#include "MemWrap.h"

//MEM_tstMem      MEM_stMem;

void MemWrap_vInit(uint8* lpui8Heap, MemWrap_uiHeapSize luixHeapSize)
{
  UNUSED(lpui8Heap);
  UNUSED(luixHeapSize);
  //MEMx_Init(lpui8Heap, luixHeapSize, &MEM_stMem);
}

void MemWrap_vDeInit(void)
{
  //MEMx_DeInit(&MEM_stMem);
}


uint8* MemWrap_pui8Alloc(MemWrap_uint luixSize)
{
  //return MEMx_Alloc(luixSize, &MEM_stMem);
  return MEMx_Alloc(luixSize);
}

void MemWrap_vFree(uint8 *pui8Data)
{
  //MEMx_Free(pui8Data, &MEM_stMem);
  MEMx_Free(pui8Data);
}

MemWrap_uiHeapSize  MemWrap_uixMemAvailable(void)
{
  //return MEMx_MemAvailable(&MEM_stMem);
  return 0;
}

MemWrap_uiHeapSize MemWrap_uixMemSize(void)
{
  //return MEMx_MemSize(&MEM_stMem);
  return 0;
}

MemWrap_uiHeapSize MemWrap_uixSizeof(uint8 *lpui8Mem)
{
  UNUSED(lpui8Mem);
  //return MEMx_MemSizeof(lpui8Mem, &MEM_stMem);
  return 0;
}










