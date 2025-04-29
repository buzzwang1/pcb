// MemHdl.h - Definitions for a simple handle based memory manager based on code from David Betz 
// http://collaboration.cmc.ec.gc.ca/science/rpn/biblio/ddj/Website/articles/DDJ/1991/9112/9112e/9112e.htm
// Note: The MemHdl not using aligned adresses. Check if you memory supports this.

#ifndef _MEM16IDX_H
#define _MEM16IDX_H

#ifdef __cplusplus
  extern "C" {
#endif

#include "typedef.h"

#define MEM16IDX_nDEBUG 0

typedef int16         Mem16Idx_int;
typedef uint16        Mem16Idx_uint;
typedef uint16        Mem16Idx_uixHeapSize;

typedef Mem16Idx_uint MEM16IDX_IDX;

#define MEM16IDX_nUNUSED_IDX       ((1 << (sizeof(MEM16IDX_IDX)*8)) - 1)
#define MEM16IDX_nINVALID_IDX      MEM16IDX_nUNUSED_IDX


#define MEM16IDX_Ptr2Idx(lpstHeapHdr, lpPtr)   ((lpPtr - (uint8*)lpstHeapHdr))
#define MEM16IDX_Idx2Ptr(lpstHeapHdr, lui8Idx) ((lui8Idx) + (uint8*)lpstHeapHdr)

#define MEM16IDX_MAX_HEAP_ALLOWED (MEM16IDX_nUNUSED_IDX-1)

  typedef struct
  {  
    uint16 ui16Len;
    uint16 ui16IdxNext;
  } Mem16Idx_tstMem16IdxFreeListH;

  typedef struct
  {  
    uint16 ui16Len;   
  } Mem16Idx_tstMem16IdxUsedH;

  typedef struct
  {
    uint8*               pui8Heap;
    Mem16Idx_uixHeapSize ui16Size;
    
    // Alle freien Blöcke sind miteinander verkettet
    uint16               ui16IdxFreeList; // Index auf den ersten freien Block      
  } Mem16Idx_tstMem16Idx;


  /* Gibt die Größe eines Speicherblocks zurück */
  extern Mem16Idx_uixHeapSize Mem16Idx_uixSizeof(uint8 *lpui8Mem, Mem16Idx_tstMem16Idx *lpstMem16Idx);
  /* Gibt die Größe des freien Speicherplatzs zurück */
  extern Mem16Idx_uixHeapSize Mem16Idx_uixMemAvailable(Mem16Idx_tstMem16Idx *lpstMem16Idx);
  /* Gibt die Gesamt-Größe des Speichers zurück */
  extern Mem16Idx_uixHeapSize Mem16Idx_uixSize(Mem16Idx_tstMem16Idx *lpstMem16Idx);

  /* Init */
  extern void     Mem16Idx_vInit(uint8* lpui8Heap, Mem16Idx_uixHeapSize luixHeapSize, Mem16Idx_tstMem16Idx *lpstMem16Idx);
  extern void     Mem16Idx_vDeInit(Mem16Idx_tstMem16Idx *lpstMem16Idx);

  /* Reserviert Speicher */
  extern uint8   *Mem16Idx_pui8Alloc(Mem16Idx_uint luixSize, Mem16Idx_tstMem16Idx *lpstMem16Idx);
  
  /* Reservierter Speicher in der Größe ändern. Inhalt bleibt bestehen */
  /* Ungetestet */
  /*extern uint8* pui8RamIdx16_Realloc(uint8 *lpui8Mem, uint16 lui16Size);*/
  /* Reservierter Speicher freigeben */
  extern void     Mem16Idx_vFree(uint8 *pui8Data, Mem16Idx_tstMem16Idx *lpstMem16Idx);

#ifdef __cplusplus
  }
#endif

#endif /* _MEM16IDX_H */

