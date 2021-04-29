/* MemHdl32.c - a simple handle based memory manager  -- based on code from David Betz */

#include "MEM16Idx.h"
#include "_MemTools.h"

#define MEMIDX16_MINBLK       (sizeof(Mem16Idx_tstMem16IdxFreeListH) + 1)
#define MEMIDX16_BLKMOD       (sizeof(Mem16Idx_tstMem16IdxFreeListH) - 1)
#define MEMIDX16_RAM          (lpstMem16Idx->pui8Heap)
#define Mem16Idx_ui16LEN(x)   (*((uint16*) &(MEMIDX16_RAM[x])))
#define Mem16Idx_ui16NEXT(x)  (Mem16Idx_ui16LEN(x+sizeof(uint16)))


#if (MEMIDX16_ANALYSE == 1)
  uint16 gui16CallsOfCreate;
  uint16 gui16CallsOfFree;
  uint16 gui16MemAlloced;
  uint16 gui16MaxMemAlloced;
#endif

// FreeListe => Linked List Helper Functions
INLINE void Mem16Idx_vFreeList_AddFront(uint16 lui16IdxBlockToAdd, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{  
  Mem16Idx_ui16NEXT(lui16IdxBlockToAdd)  = lpstMem16Idx->ui16IdxFreeList;
  lpstMem16Idx->ui16IdxFreeList          = lui16IdxBlockToAdd;
}

INLINE void Mem16Idx_vFreeList_AddLast(uint16 lui16IdxPrevBlock, uint16 lui16IdxBlockToAdd, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  Mem16Idx_ui16NEXT(lui16IdxBlockToAdd) = Mem16Idx_ui16NEXT(lui16IdxPrevBlock);
  Mem16Idx_ui16NEXT(lui16IdxPrevBlock)  = lui16IdxBlockToAdd;
}

INLINE void Mem16Idx_vFreeList_Add(uint16 lui16IdxPrevBlock, uint16 lui16IdxBlockToAdd, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  if (lui16IdxPrevBlock == UINT16MAX)
  {
    Mem16Idx_vFreeList_AddFront(lui16IdxBlockToAdd, lpstMem16Idx);
  }
  else
  {
    Mem16Idx_ui16NEXT(lui16IdxBlockToAdd) = Mem16Idx_ui16NEXT(lui16IdxPrevBlock);
    Mem16Idx_ui16NEXT(lui16IdxPrevBlock)  = lui16IdxBlockToAdd; 
  }
}


INLINE void Mem16Idx_vFreeList_RemoveFront(Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  lpstMem16Idx->ui16IdxFreeList = Mem16Idx_ui16NEXT(lpstMem16Idx->ui16IdxFreeList);
}

INLINE void Mem16Idx_vFreeList_RemoveLast(uint16 lui16IdxPrevBlock, uint16 lui16IdxBlockToRemove, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
    Mem16Idx_ui16NEXT(lui16IdxPrevBlock)  = Mem16Idx_ui16NEXT(lui16IdxBlockToRemove);
}

INLINE void Mem16Idx_vFreeList_Remove(uint16 lui16IdxPrevBlock, uint16 lui16IdxBlockToRemove, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  if (lui16IdxPrevBlock == UINT16MAX)
  {
    Mem16Idx_vFreeList_RemoveFront(lpstMem16Idx);
  }
  else
  {
    Mem16Idx_ui16NEXT(lui16IdxPrevBlock) = Mem16Idx_ui16NEXT(lui16IdxBlockToRemove);
  }   
}

INLINE void Mem16Idx_vFreeList_Merge(uint16 lui16IdxPrevBlockFront, uint16 lui16IdxBlockToMergeFront, 
                                     uint16 lui16IdxPrevBlockBack,  uint16 lui16IdxBlockToMergeBack, 
                                     Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  //Hinteren Block aus der Liste entfernen
  Mem16Idx_vFreeList_Remove(lui16IdxPrevBlockBack,  lui16IdxBlockToMergeBack, lpstMem16Idx);

  //Zusammenbauen...
  Mem16Idx_ui16LEN(lui16IdxBlockToMergeFront) += (Mem16Idx_ui16LEN(lui16IdxBlockToMergeBack) + sizeof(Mem16Idx_tstMem16IdxUsedH));
}


void Mem16Idx_vInit(uint8* lpui8Heap, Mem16Idx_uixHeapSize luixHeapSize, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  lpstMem16Idx->pui8Heap = lpui8Heap;
  lpstMem16Idx->ui16Size = luixHeapSize;

  MEMTOOLS_vMemSet(lpui8Heap, 0xAA, luixHeapSize);
  lpstMem16Idx->ui16IdxFreeList                    = 0;
  Mem16Idx_ui16LEN(lpstMem16Idx->ui16IdxFreeList)  = luixHeapSize - sizeof(Mem16Idx_tstMem16IdxUsedH);
  Mem16Idx_ui16NEXT(lpstMem16Idx->ui16IdxFreeList) = UINT16MAX; 
  
  #if (MEMIDX16_ANALYSE == 1)
    gui16CallsOfCreate = 0;
    gui16CallsOfFree   = 0;
    gui16MemAlloced    = 0;
    gui16MaxMemAlloced = 0;
  #endif
}

void Mem16Idx_vDeInit(Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
}

/* Speicher reservieren */
uint8* Mem16Idx_pui8Alloc(Mem16Idx_uint luixSize, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  uint16 lui16IdxFreeBlock; // p; 
  uint16 lui16IdxPrevFreeBlock; //r
  uint16 lui16IdxFoundBlock; //q
  uint16 lui16IdxPrevBlock;  //s
  uint16 lui16PrevFoundBlockSize;

  #if (MEMIDX16_ANALYSE == 1)
    gui16CallsOfCreate++;
  #endif


  // 4, 8, 12, ... auf nächste 4ter aufrunden
  if (luixSize < 2) luixSize  = 0;
               else luixSize -= 2;
  luixSize  = (luixSize + MEMIDX16_BLKMOD) & (~MEMIDX16_BLKMOD);

  #if (MEMIDX16_ANALYSE == 1)
    gui16MemAlloced   += (luixSize + sizeof(RamIdx16FreeListH));
    if (gui16MemAlloced > gui16MaxMemAlloced) gui16MaxMemAlloced = gui16MemAlloced;
  #endif


  // Die freien Blöcke sind in der Freelist miteinander verknüpft
  // Nächste freie Stelle suchen mit genügend freien Platz

  
  lui16IdxFreeBlock       = lpstMem16Idx->ui16IdxFreeList;
  lui16IdxFoundBlock      = UINT16MAX;
  lui16IdxPrevBlock       = UINT16MAX;
  lui16IdxPrevFreeBlock   = UINT16MAX;
  lui16PrevFoundBlockSize = UINT16MAX;

  // kleinster Block suchen in der der geforderte Speicher passt
  while (lui16IdxFreeBlock != UINT16MAX) 
  {
    // Ist der Block groß genug
    // bzw. es wurde schon ein Block gefunden, dieser war jedoch größer
    if ((Mem16Idx_ui16LEN(lui16IdxFreeBlock) >= luixSize) && 
        ((lui16IdxFoundBlock == UINT16MAX) || (lui16PrevFoundBlockSize > Mem16Idx_ui16LEN(lui16IdxFreeBlock)))) 
    {
      lui16PrevFoundBlockSize = Mem16Idx_ui16LEN(lui16IdxFreeBlock);
      lui16IdxFoundBlock      = lui16IdxFreeBlock;
      lui16IdxPrevFreeBlock   = lui16IdxPrevBlock;
    }

    lui16IdxPrevBlock = lui16IdxFreeBlock;
    lui16IdxFreeBlock = Mem16Idx_ui16NEXT(lui16IdxFreeBlock);
  }

  
  //wenn freie Stelle gefunden wurde
  if (lui16IdxFoundBlock != UINT16MAX) 
  {
    // Überprüfen ober der gefundene Block groß genug zum Splitten ist
    if ((Mem16Idx_ui16LEN(lui16IdxFoundBlock) - luixSize) >= MEMIDX16_MINBLK) 
    { 
      // den gesplitteten Speicher in die Kette aufnehmen
      lui16IdxFreeBlock                    = lui16IdxFoundBlock + sizeof(Mem16Idx_tstMem16IdxFreeListH) + luixSize;
      Mem16Idx_ui16NEXT(lui16IdxFreeBlock) = Mem16Idx_ui16NEXT(lui16IdxFoundBlock);
      Mem16Idx_ui16LEN(lui16IdxFreeBlock)  = Mem16Idx_ui16LEN(lui16IdxFoundBlock) - luixSize - sizeof(Mem16Idx_tstMem16IdxFreeListH);

      if (lui16IdxPrevFreeBlock == UINT16MAX)
        lpstMem16Idx->ui16IdxFreeList = lui16IdxFreeBlock;
      else
        Mem16Idx_ui16NEXT(lui16IdxPrevFreeBlock) = lui16IdxFreeBlock;

      //Neuer Bereich festlegen
      Mem16Idx_ui16LEN(lui16IdxFoundBlock) = luixSize  + sizeof(Mem16Idx_tstMem16IdxUsedH);
    }
    else
    {
      // Sonst ganzen Block beanspruchen        
      if (lui16IdxPrevFreeBlock == UINT16MAX)
        lpstMem16Idx->ui16IdxFreeList = Mem16Idx_ui16NEXT(lui16IdxFoundBlock);
      else
        Mem16Idx_ui16NEXT(lui16IdxPrevFreeBlock) = Mem16Idx_ui16NEXT(lui16IdxFoundBlock);
    }
  }
  else // keine groß genuge freie Stelle gefunden
  {
    return 0;
  }

  return (uint8*)&MEMIDX16_RAM[lui16IdxFoundBlock + sizeof(Mem16Idx_tstMem16IdxUsedH)];
}

/* Gibt die Größe eines reservierten Speicherbereichs zurück */
Mem16Idx_uixHeapSize Mem16Idx_uixSizeof(uint8 *lpui8Mem, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  uint16 lui16IdxBlock;

  lui16IdxBlock = (uint16)(lpui8Mem - lpstMem16Idx->pui8Heap) - sizeof(Mem16Idx_tstMem16IdxUsedH);

  return Mem16Idx_ui16LEN(lui16IdxBlock); 
}


/* Gibt die Größe des Speichers zurück */
Mem16Idx_uixHeapSize Mem16Idx_uixSize(Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  return lpstMem16Idx->ui16Size;
}


/* Gibt die Größe des zur Verfügung stehenden Speichers zurück */
Mem16Idx_uixHeapSize Mem16Idx_uixMemAvailable(Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  uint16 lui16FreeSpace;
  uint16 lui16IdxBlock;

  lui16FreeSpace = 0;
  lui16IdxBlock  = lpstMem16Idx->ui16IdxFreeList; 

  while (lui16IdxBlock != UINT16MAX)
  {
    lui16FreeSpace += Mem16Idx_ui16LEN(lui16IdxBlock);
    lui16IdxBlock   = Mem16Idx_ui16NEXT(lui16IdxBlock); 
  }

  return lui16FreeSpace;
}

/* Gibt einen reservierten Speicherbereich weider frei */
void Mem16Idx_vFree(uint8 *lpui8Mem, Mem16Idx_tstMem16Idx *lpstMem16Idx)
{
  uint16 lui16IdxBlockToFree;
  uint16 lui16IdxSearchBlock;
  uint16 lui16IdxPrevSearchedBlock;

  uint16 lui16IdxBlockToCompareAhead;
  uint16 lui16IdxBlockAhead;
  uint16 lui16IdxBlockPrevAhead;

  uint16 lui16IdxBlockToCompareBehind;
  uint16 lui16IdxBlockBehind;
  uint16 lui16IdxBlockPrevBehind;

  uint8  lui8Fertig;


  #if (MEMIDX16_ANALYSE == 1)
    gui16CallsOfFree++;
  #endif


  if (!lpui8Mem) return;

  lui8Fertig = 0;

  //Pointer auf diesen Eintrag
  lui16IdxBlockToFree = (uint16) ((uint16)(lpui8Mem - lpstMem16Idx->pui8Heap) - sizeof(Mem16Idx_tstMem16IdxUsedH)); 

  #if (MEMIDX16_ANALYSE == 1)
    gui16MemAlloced   -= (Mem16Idx_ui16LEN(lui16IdxBlockToFree) + sizeof(RamIdx16UsedH));
  #endif

  //Den Block zur FreeList hinzufügen
  Mem16Idx_vFreeList_AddFront(lui16IdxBlockToFree, lpstMem16Idx);

  //Überprüfen ob es freie Blöck in der Nachbarschaft gibt
  lui16IdxPrevSearchedBlock    = UINT16MAX;
  lui16IdxSearchBlock          = lpstMem16Idx->ui16IdxFreeList;
  lui16IdxBlockToCompareBehind = lui16IdxBlockToFree + Mem16Idx_ui16LEN(lui16IdxBlockToFree) + sizeof(Mem16Idx_tstMem16IdxUsedH);

  //Following 4 lines only to avoid "may be used uninitialized" warnings
  lui16IdxBlockBehind          = 0;
  lui16IdxBlockPrevBehind      = 0;
  lui16IdxBlockAhead           = 0;
  lui16IdxBlockPrevAhead       = 0;


  while (lui16IdxSearchBlock != UINT16MAX)
  {
    if (!(lui8Fertig & 1))
    {

      lui16IdxBlockToCompareAhead = lui16IdxSearchBlock + Mem16Idx_ui16LEN(lui16IdxSearchBlock) + sizeof(Mem16Idx_tstMem16IdxUsedH);

      //Überprüfen ob es einen vorangehenden freie Block gibt
      //wenn ja zusammenführen 
      if (lui16IdxBlockToCompareAhead == lui16IdxBlockToFree) 
      {
        lui16IdxBlockPrevAhead = lui16IdxPrevSearchedBlock;
        lui16IdxBlockAhead     = lui16IdxSearchBlock;
      
        lui8Fertig |= 1;
        if (lui8Fertig == 3) break;
      }
    }

    //Überprüfen ob es einen nachfolgender freien Block gibt
    //wenn ja zusammenführen 
    if (lui16IdxBlockToCompareBehind == lui16IdxSearchBlock) 
    {
      lui16IdxBlockPrevBehind = lui16IdxPrevSearchedBlock;
      lui16IdxBlockBehind     = lui16IdxSearchBlock;        

      lui8Fertig |= 2;
      if (lui8Fertig == 3) break;
    }


    lui16IdxPrevSearchedBlock = lui16IdxSearchBlock;
    lui16IdxSearchBlock       = Mem16Idx_ui16NEXT(lui16IdxSearchBlock);     
  }

  //Überprüfen ob es einen vorangehenden freien Blöcke gibt
  //wenn ja zusammenführen 
  if (lui8Fertig & 2)
    Mem16Idx_vFreeList_Merge(UINT16MAX,               lui16IdxBlockToFree, 
                             lui16IdxBlockPrevBehind, lui16IdxBlockBehind, 
                             lpstMem16Idx);

  if (lui8Fertig & 1)
    Mem16Idx_vFreeList_Merge(lui16IdxBlockPrevAhead,  lui16IdxBlockAhead,
                             UINT16MAX,               lui16IdxBlockToFree, 
                             lpstMem16Idx);

}



/* Reservierter Speicher in der Größe ändern. Inhalt bleibt bestehen */
/*
uint8* pui8MEMIDX16_Realloc(uint8 *lpui8Mem, uint16 lui16Size)
{
  RamIdx16FreeListH *p;
  char *q, *f1;
  size_t s1;

  if (!lpui8Mem) return mmalloc(lui16Size);

  p = (RamIdx16FreeListH *) ((char *)lpui8Mem - sizeof(RamIdx16FreeListH));
  s1 = Mem16Idx_ui16LEN(p);
  mfree(lpui8Mem);

  if (s1 > lui16Size)
  {
    s1 = lui16Size + MEMIDX16_BLKMOD & ~MEMIDX16_BLKMOD;
  }

  if (!Mem16Idx_ui16LEN(p)) // ???
  {
    f1 = (char *)(Mem16Idx_ui16NEXT((p) + 1);
    memcpy(f1, lpui8Mem, s1);
    lpui8Mem = f1;
  }

  q = mmalloc(lui16Size);

  if (q && q != lpui8Mem)
  {
    memcpy(q, lpui8Mem, s1);
  }

  return q;
}*/

