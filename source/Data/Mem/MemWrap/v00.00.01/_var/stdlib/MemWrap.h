#ifndef _MEMWRAP_H
#define _MEMWRAP_H

#ifdef __cplusplus
  extern "C" {
#endif

#include "typedef.h"
#include <stdlib.h>
//#include "_Mem16Idx.h"

#define MEM_nINVALID        null

//typedef Mem16Idx_tstMem16Idx  MEM_tstMem;
//typedef Mem16Idx_uint         MemWrap_uint;
//typedef Mem16Idx_uixHeapSize  MemWrap_uiHeapSize;
typedef uint32          MemWrap_uint;
typedef uint32          MemWrap_uiHeapSize;


#define MEMx_Init           //Mem16Idx_vInit
#define MEMx_DeInit         //Mem16Idx_vDeInit
#define MEMx_Alloc          malloc//Mem16Idx_pui8Alloc
#define MEMx_Free           free//Mem16Idx_vFree
#define MEMx_MemAvailable   //Mem16Idx_uixMemAvailable
#define MEMx_MemSize        //Mem16Idx_uixSize
#define MEMx_MemSizeof      //Mem16Idx_uixSizeof

//#define MEMx_nCLUSTERSIZE   MEM16IDX_nCLUSTERSIZE

//extern MEM_tstMem      MEM_stMem;

void            MemWrap_vInit(uint8* lpui8Heap, MemWrap_uiHeapSize luixHeapSize);
void            MemWrap_vDeInit(void);
uint8*          MemWrap_pui8Alloc(MemWrap_uint luixSize);
void            MemWrap_vFree(uint8 *pui8Data);
MemWrap_uiHeapSize MemWrap_uixSizeof(uint8 *lpui8Mem);
MemWrap_uiHeapSize MemWrap_uixMemAvailable(void);
MemWrap_uiHeapSize MemWrap_uixMemSize(void);

#ifdef __cplusplus
}
#endif

#endif /* _MEMWRAP_H */
