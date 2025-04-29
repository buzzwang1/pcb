#ifndef _MEMTOOLS_H
#define _MEMTOOLS_H

#include "MemToolsHw.h"

#define MEMTOOLS_nBitRight6 0x7F  //_XXX XXXX
#define MEMTOOLS_nBitRight5 0x3F  //__XX XXXX
#define MEMTOOLS_nBitRight4 0x1F  //___X XXXX
#define MEMTOOLS_nBitRight3 0x0F  //____ XXXX
#define MEMTOOLS_nBitRight2 0x07  //____ _XXX
#define MEMTOOLS_nBitRight1 0x03  //____ __XX
#define MEMTOOLS_nBitRight0 0x01  //____ ___X

#define MEMTOOLS_nBitLeft6 (0xFE)  //XXXX XXX_
#define MEMTOOLS_nBitLeft5 (0xFC)  //XXXX XX__
#define MEMTOOLS_nBitLeft4 (0xF8)  //XXXX X___
#define MEMTOOLS_nBitLeft3 (0xF0)  //XXXX ____
#define MEMTOOLS_nBitLeft2 (0xE0)  //XXX_ ____
#define MEMTOOLS_nBitLeft1 (0xC0)  //XX__ ____
#define MEMTOOLS_nBitLeft0 (0x80)  //X___ ____


class cMemTools : public cMemToolsHw
{
  public:

  static const uint8 nBitRight[7];
  static const uint8 nBitLeft[7];

  static inline void  vBitIncPos(u8* &pui8Ptr, u8 &ui8Mask)                                 {if ((ui8Mask) > 1) {ui8Mask >>= 1;} else {ui8Mask = 128; pui8Ptr++;}}
  static inline void  vBitOr(u8 ui8SrcByte,   u8 &ui8DstByte, u8 ui8SrcMask, u8 ui8DstMask) {if ((ui8SrcMask) & (ui8SrcByte)) {ui8DstByte |= (ui8DstMask);}}
  static inline void  vBitAnd(u8 ui8SrcByte,  u8 &ui8DstByte, u8 ui8SrcMask, u8 ui8DstMask) {if ((ui8SrcMask) & (ui8SrcByte)) {                           } else {ui8DstByte &= ~(ui8DstMask);}}
  static inline void  vBitCpy(u8 ui8SrcByte,  u8 &ui8DstByte, u8 ui8SrcMask, u8 ui8DstMask) {if ((ui8SrcMask) & (ui8SrcByte)) {ui8DstByte |= (ui8DstMask);} else {ui8DstByte &= ~(ui8DstMask);}}
  static inline void  vBitXor(u8 ui8SrcByte,  u8 &ui8DstByte, u8 ui8SrcMask, u8 ui8DstMask) {if ((ui8SrcMask) & (ui8SrcByte)) {if ((ui8DstMask) & (ui8DstByte)) {ui8DstByte &= ~(ui8DstMask);}else{ui8DstByte ^= (ui8DstMask);}}}


  static u32 u32Checksum(uint8* lpui8Dest, uint32 luixSize)
  {
    u32 lu32Checksum;

    lu32Checksum = 1;
    while (luixSize > 4)
    {
      lu32Checksum+=*lpui8Dest++;
      lu32Checksum+=*lpui8Dest++;
      lu32Checksum+=*lpui8Dest++;
      lu32Checksum+=*lpui8Dest++;
      luixSize-=4;
    }
    while (luixSize > 0)
    {
      lu32Checksum+=*lpui8Dest++;
      luixSize--;
    }
    return lu32Checksum;
  }

  static void vMemAnd(uint8* lpui8Dest, uint8* lpui8Source, uint32 luixSize)
  {
    #if (nDefCoreType == nDefCoreType32)
      if (((((uint32)lpui8Source) & 3) == 0) && // auf ganzen 32 Bit Adresses ?
          ((((uint32)lpui8Dest) & 3) == 0))
      {
        uint32* lpui32Source = (uint32*)lpui8Source;
        uint32* lpui32Dest   = (uint32*)lpui8Dest;
        uint32 luixCount;

        if (luixSize > 15)
        {
          luixCount  = luixSize >> 4; // 16Byte Blöcke
          luixSize  &= 15;

          while (luixCount > 0) // 4 * 4 Byte = 16Byte
          {
            *lpui32Dest++ &= *lpui32Source++;
            *lpui32Dest++ &= *lpui32Source++;
            *lpui32Dest++ &= *lpui32Source++;
            *lpui32Dest++ &= *lpui32Source++;
            luixCount--;
          }
        }

        if (luixSize > 3)
        {
          luixCount  = luixSize >> 2; // 4Byte Blöcke
          luixSize  &= 3;

          while (luixCount > 0) // 1 * 4 Byte = 16Byte
          {
            *lpui32Dest++ &= *lpui32Source++;
            luixCount--;
          }
        }

        lpui8Dest   = (uint8*)lpui32Dest;
        lpui8Source = (uint8*)lpui32Source;
        // Den Rest byteweise
        while (luixSize > 0)
        {
          *lpui8Dest++ &= *lpui8Source++;
          luixSize--;
        }
      }
      else
      {
        while (luixSize > 0)
        {
          *lpui8Dest++ &= *lpui8Source++;
          luixSize--;
        }
      }
    #else
      while (luixSize > 0)
      {
        *lpui8Dest++ &= *lpui8Source++;
        luixSize--;
      }
    #endif
  }

  static void vMemOr(uint8* lpui8Dest, uint8* lpui8Source, uint32 luixSize)
  {
    #if (nDefCoreType == nDefCoreType32)
      if (((((uint32)lpui8Source) & 3) == 0) && // auf ganzen 32 Bit Adresses ?
          ((((uint32)lpui8Dest) & 3) == 0))
      {
        uint32* lpui32Source = (uint32*)lpui8Source;
        uint32* lpui32Dest   = (uint32*)lpui8Dest;
        uint32 luixCount;

        if (luixSize > 15)
        {
          luixCount  = luixSize >> 4; // 16Byte Blöcke
          luixSize  &= 15;

          while (luixCount > 0) // 4 * 4 Byte = 16Byte
          {
            *lpui32Dest++ |= *lpui32Source++;
            *lpui32Dest++ |= *lpui32Source++;
            *lpui32Dest++ |= *lpui32Source++;
            *lpui32Dest++ |= *lpui32Source++;
            luixCount--;
          }
        }

        if (luixSize > 3)
        {
          luixCount  = luixSize >> 2; // 4Byte Blöcke
          luixSize  &= 3;

          while (luixCount > 0) // 1 * 4 Byte = 16Byte
          {
            *lpui32Dest++ |= *lpui32Source++;
            luixCount--;
          }
        }

        lpui8Source = (uint8*)lpui32Source;
        lpui8Dest   = (uint8*)lpui32Dest;

        // Den Rest byteweise
        while (luixSize > 0)
        {
          *lpui8Dest++ |= *lpui8Source++;
          luixSize--;
        }
      }
      else
      {
        while (luixSize > 0)
        {
          *lpui8Dest++ |= *lpui8Source++;
          luixSize--;
        }
      }
    #else
      while (luixSize > 0)
      {
        *lpui8Dest++ |= *lpui8Source++;
        luixSize--;
      }
    #endif
  }


  static void vMemXor(uint8* lpui8Dest, uint8* lpui8Source, uint32 luixSize)
  {
    #if (nDefCoreType == nDefCoreType32)
      if (((((uint32)lpui8Source) & 3) == 0) && // auf ganzen 32 Bit Adresses ?
          ((((uint32)lpui8Dest) & 3) == 0))
      {
        uint32* lpui32Source = (uint32*)lpui8Source;
        uint32* lpui32Dest   = (uint32*)lpui8Dest;
        uint32 luixCount;

        if (luixSize > 15)
        {
          luixCount  = luixSize >> 4; // 16Byte Blöcke
          luixSize  &= 15;

          while (luixCount > 0) // 4 * 4 Byte = 16Byte
          {
            *lpui32Dest++ ^= *lpui32Source++;
            *lpui32Dest++ ^= *lpui32Source++;
            *lpui32Dest++ ^= *lpui32Source++;
            *lpui32Dest++ ^= *lpui32Source++;
            luixCount--;
          }
        }

        if (luixSize > 3)
        {
          luixCount  = luixSize >> 2; // 4Byte Blöcke
          luixSize  &= 3;

          while (luixCount > 0) // 1 * 4 Byte = 16Byte
          {
            *lpui32Dest++ ^= *lpui32Source++;
            luixCount--;
          }
        }

        lpui8Source = (uint8*)lpui32Source;
        lpui8Dest   = (uint8*)lpui32Dest;

        // Den Rest byteweise
        while (luixSize > 0)
        {
          *lpui8Dest++ ^= *lpui8Source++;
          luixSize--;
        }
      }
      else
      {
        while (luixSize > 0)
        {
          *lpui8Dest++ ^= *lpui8Source++;
          luixSize--;
        }
      }
    #else
      while (luixSize > 0)
      {
        *lpui8Dest++ ^= *lpui8Source++;
        luixSize--;
      }
    #endif
  }



  static void vMemNeg(uint8* lpui8Dest, uint32 luixSize)
  {
    #if (nDefCoreType == nDefCoreType32)
      if ((((uint32)lpui8Dest) & 3) == 0) // auf ganzen 32 Bit Adresses ?
      {
        uint32* lpui32Dest   = (uint32*)lpui8Dest;
        uint32 luixCount;

        if (luixSize > 15)
        {
          luixCount  = luixSize >> 4; // 16Byte Blöcke
          luixSize  &= 15;

          while (luixCount > 0) // 4 * 4 Byte = 16Byte
          {
            *lpui32Dest = ~(*lpui32Dest);
            lpui32Dest++;
            *lpui32Dest = ~(*lpui32Dest);
            lpui32Dest++;
            *lpui32Dest = ~(*lpui32Dest);
            lpui32Dest++;
            *lpui32Dest = ~(*lpui32Dest);
            lpui32Dest++;
            luixCount--;
          }
        }

        if (luixSize > 3)
        {
          luixCount  = luixSize >> 2; // 4Byte Blöcke
          luixSize  &= 3;

          while (luixCount > 0) // 1 * 4 Byte = 16Byte
          {
            *lpui32Dest = ~(*lpui32Dest);
            lpui32Dest++;
            luixCount--;
          }
        }

        // Den Rest byteweise
        while (luixSize > 0)
        {
          *lpui8Dest = ~(*lpui8Dest);
          lpui8Dest++;
          luixSize--;
        }
      }
      else
      {
        while (luixSize > 0)
        {
          *lpui8Dest = ~(*lpui8Dest);
          lpui8Dest++;
          luixSize--;
        }
      }
    #else
      while (luixSize > 0)
      {
        *lpui8Dest = ~(*lpui8Dest);
        lpui8Dest++;
        luixSize--;
      }
    #endif
  }



  static int8 iMemCmp(uint8* lpui8Mem1, uint8* lpui8Mem2, uint32 luixSize)
  {
    while ((luixSize--) > 0)
    {
      if (*lpui8Mem1++ != *lpui8Mem2++)
      {
	      return lpui8Mem1[-1] < lpui8Mem2[-1] ? -1 : 1;
      }
    }
    return 0;
  }


  static void vMemBitSet(uint8* lpui8Dest, uint8 lui8StartBitPos, uint8  lui8Value, uint32 luixSize)
  {
    uint8  lui8SizeStart;
    uint32 luixSizeMid;
    uint8  lui8SizeEnd;

    if (lui8StartBitPos > 0)
    {
      lui8SizeStart = 8 - lui8StartBitPos;
    }
    else
    {
      lui8SizeStart = 0;
    }

    if (luixSize >= lui8SizeStart)
    {
      luixSize     -= lui8SizeStart;
    }
    else
    {
      lui8SizeStart = (uint8)luixSize;
      luixSize = 0;
    }

    lui8SizeEnd  = ((uint8)luixSize & 7);

    if (luixSize >= lui8SizeEnd)
    {
      luixSizeMid  = (luixSize - lui8SizeEnd) >> 3;
    }
    else
    {
      luixSizeMid = 0;
    }

    if (lui8Value)
    {
      if ((luixSizeMid == 0) && (lui8SizeEnd == 0))
      {
        uint8 lui8Mask;

        lui8Mask = cMemTools::nBitLeft[lui8SizeStart-1];
        lui8Mask >>= lui8StartBitPos;
        *lpui8Dest |= lui8Mask;
      }
      else
      {
        if (lui8SizeStart > 0)
        {
          *lpui8Dest |= cMemTools::nBitRight[lui8SizeStart-1];
          lpui8Dest++;
        }
        if (luixSizeMid > 0)
        {
          vMemSet(lpui8Dest, 0xFF, luixSizeMid);
          lpui8Dest += luixSizeMid;
        }
        if (lui8SizeEnd > 0)
        {
          *lpui8Dest |= cMemTools::nBitLeft[lui8SizeEnd-1];
        }
      }
    }
    else
    {
      if ((luixSizeMid == 0) && (lui8SizeEnd == 0))
      {
        uint8 lui8Mask;

        lui8Mask = cMemTools::nBitLeft[lui8SizeStart-1];
        lui8Mask >>= lui8StartBitPos;
        *lpui8Dest &= ~lui8Mask;
      }
      else
      {
        if (lui8SizeStart > 0)
        {
          *lpui8Dest &= ~cMemTools::nBitRight[lui8SizeStart-1];
          lpui8Dest++;
        }
        if (luixSizeMid > 0)
        {
          vMemSet(lpui8Dest, 0x00, luixSizeMid);
          lpui8Dest += luixSizeMid;
        }
        if (lui8SizeEnd > 0)
        {
          *lpui8Dest &= ~cMemTools::nBitLeft[lui8SizeEnd-1];
        }
      }
    }
  }


  static void vMemBitNeg(uint8* lpui8Dest, uint8 lui8StartBitPos, uint32 luixSize)
  {
    uint8  lui8SizeStart;
    uint32 luixSizeMid;
    uint8  lui8SizeEnd;

    if (lui8StartBitPos > 0)
    {
      lui8SizeStart = 8 - lui8StartBitPos;
    }
    else
    {
      lui8SizeStart = 0;
    }

    if (luixSize >= lui8SizeStart)
    {
      luixSize     -= lui8SizeStart;
    }
    else
    {
      lui8SizeStart = (uint8)luixSize;
      luixSize = 0;
    }

    lui8SizeEnd  = ((uint8)luixSize & 7);

    if (luixSize >= lui8SizeEnd)
    {
      luixSizeMid  = (luixSize - lui8SizeEnd) >> 3;
    }
    else
    {
      luixSizeMid = 0;
    }

    if ((luixSizeMid == 0) && (lui8SizeEnd == 0))
    {
      uint8 lui8Mask;

      lui8Mask = cMemTools::nBitLeft[lui8SizeStart-1];
      lui8Mask >>= lui8StartBitPos;
      *lpui8Dest |= (lui8Mask & ~(*lpui8Dest));
    }
    else
    {
      if (lui8SizeStart > 0)
      {
        *lpui8Dest = ~(*lpui8Dest);
        lpui8Dest++;
      }
      if (luixSizeMid > 0)
      {
        vMemNeg(lpui8Dest, luixSizeMid);
        lpui8Dest += luixSizeMid;
      }
      if (lui8SizeEnd > 0)
      {
        *lpui8Dest |= (cMemTools::nBitLeft[lui8SizeEnd-1] & ~(*lpui8Dest));
      }
    }
  }



  static void vMemBitCpy(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint8* lpui8Dest, uint8 lui8DestStartBitPos, uint32 luixSize)
  {
    uint8  lui8Mask;

    if (lui8SrcStartBitPos == lui8DestStartBitPos)
    {
      uint8  lui8SizeStart;
      uint32 luixSizeMid;
      uint8  lui8SizeEnd;

      if (lui8SrcStartBitPos > 0)
      {
        lui8SizeStart = 8 - lui8SrcStartBitPos;
      }
      else
      {
        lui8SizeStart = 0;
      }

      if (luixSize >= lui8SizeStart)
      {
        luixSize     -= lui8SizeStart;
      }
      else
      {
        lui8SizeStart = (uint8)luixSize;
        luixSize = 0;
      }

      lui8SizeEnd  = ((uint8)luixSize & 7);

      if (luixSize >= lui8SizeEnd)
      {
        luixSizeMid  = (luixSize - lui8SizeEnd) >> 3;
      }
      else
      {
        luixSizeMid = 0;
      }

      if ((luixSizeMid == 0) && (lui8SizeEnd == 0))
      {
        lui8Mask = cMemTools::nBitLeft[lui8SizeStart-1];
        lui8Mask >>= lui8SrcStartBitPos;
        *lpui8Dest &=  (~lui8Mask);
        *lpui8Dest |= (lui8Mask & (*lpui8Source));
      }
      else
      {
        if (lui8SizeStart > 0)
        {
          lui8Mask = cMemTools::nBitRight[lui8SizeStart-1];
          *lpui8Dest &=  (~lui8Mask);
          *lpui8Dest |= (lui8Mask & (*lpui8Source));
          lpui8Dest++;
          lpui8Source++;
        }
        if (luixSizeMid > 0)
        {
          vMemCpy(lpui8Dest, lpui8Source, luixSizeMid);
          lpui8Dest   += luixSizeMid;
          lpui8Source += luixSizeMid;
        }
        if (lui8SizeEnd > 0)
        {
          lui8Mask = cMemTools::nBitLeft[lui8SizeEnd-1];
          *lpui8Dest &= (~lui8Mask);
          *lpui8Dest |= (lui8Mask & (*lpui8Source));
        }
      }
    }
    else
    {
      uint8 lui8BitMskSource;
      uint8 lui8BitMskDest;

      lui8BitMskSource = 128 >> lui8SrcStartBitPos;
      lui8BitMskDest   = 128 >> lui8DestStartBitPos;

      while (luixSize > 0)
      {
        vBitCpy(*lpui8Source, *lpui8Dest, lui8BitMskSource, lui8BitMskDest);

        vBitIncPos(lpui8Source, lui8BitMskSource);
        vBitIncPos(lpui8Dest,   lui8BitMskDest);

        luixSize--;
      }
    }
  }


  static void vMemBitAnd(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint8* lpui8Dest, uint8 lui8DestStartBitPos, uint32 luixSize)
  {
    uint8  lui8Mask;

    if (lui8SrcStartBitPos == lui8DestStartBitPos)
    {
      uint8  lui8SizeStart;
      uint32 luixSizeMid;
      uint8  lui8SizeEnd;

      if (lui8SrcStartBitPos > 0)
      {
        lui8SizeStart = 8 - lui8SrcStartBitPos;
      }
      else
      {
        lui8SizeStart = 0;
      }

      if (luixSize >= lui8SizeStart)
      {
        luixSize     -= lui8SizeStart;
      }
      else
      {
        lui8SizeStart = (uint8)luixSize;
        luixSize = 0;
      }

      lui8SizeEnd  = ((uint8)luixSize & 7);

      if (luixSize >= lui8SizeEnd)
      {
        luixSizeMid  = (luixSize - lui8SizeEnd) >> 3;
      }
      else
      {
        luixSizeMid = 0;
      }

      if ((luixSizeMid == 0) && (lui8SizeEnd == 0))
      {
        lui8Mask = cMemTools::nBitLeft[lui8SizeStart-1];
        lui8Mask >>= lui8SrcStartBitPos;
        *lpui8Dest &= (lui8Mask & (*lpui8Source));
      }
      else
      {
        if (lui8SizeStart > 0)
        {
          lui8Mask = cMemTools::nBitRight[lui8SizeStart-1];
          *lpui8Dest &= (~lui8Mask | (*lpui8Source));
          lpui8Dest++;
          lpui8Source++;
        }
        if (luixSizeMid > 0)
        {
          vMemAnd(lpui8Dest, lpui8Source, luixSizeMid);
          lpui8Dest   += luixSizeMid;
          lpui8Source += luixSizeMid;
        }
        if (lui8SizeEnd > 0)
        {
          lui8Mask = cMemTools::nBitLeft[lui8SizeEnd-1];
          *lpui8Dest &= (~lui8Mask | (*lpui8Source));
        }
      }
    }
    else
    {
      uint8 lui8BitMskSource;
      uint8 lui8BitMskDest;

      lui8BitMskSource = 128 >> lui8SrcStartBitPos;
      lui8BitMskDest   = 128 >> lui8DestStartBitPos;

      while (luixSize > 0)
      {
        vBitAnd(*lpui8Source, *lpui8Dest, lui8BitMskSource, lui8BitMskDest);

        vBitIncPos(lpui8Source, lui8BitMskSource);
        vBitIncPos(lpui8Dest,   lui8BitMskDest);

        luixSize--;
      }
    }
  }


  static void vMemBitOr(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint8* lpui8Dest, uint8 lui8DestStartBitPos, uint32 luixSize)
  {
    uint8  lui8Mask;

    if (lui8SrcStartBitPos == lui8DestStartBitPos)
    {
      uint8  lui8SizeStart;
      uint32 luixSizeMid;
      uint8  lui8SizeEnd;

      if (lui8SrcStartBitPos > 0)
      {
        lui8SizeStart = 8 - lui8SrcStartBitPos;
      }
      else
      {
        lui8SizeStart = 0;
      }

      if (luixSize >= lui8SizeStart)
      {
        luixSize     -= lui8SizeStart;
      }
      else
      {
        lui8SizeStart = (uint8)luixSize;
        luixSize = 0;
      }

      lui8SizeEnd  = ((uint8)luixSize & 7);

      if (luixSize >= lui8SizeEnd)
      {
        luixSizeMid  = (luixSize - lui8SizeEnd) >> 3;
      }
      else
      {
        luixSizeMid = 0;
      }

      if ((luixSizeMid == 0) && (lui8SizeEnd == 0))
      {
        lui8Mask = cMemTools::nBitLeft[lui8SizeStart-1];
        lui8Mask >>= lui8SrcStartBitPos;
        *lpui8Dest |= (lui8Mask & (*lpui8Source));
      }
      else
      {
        if (lui8SizeStart > 0)
        {
          lui8Mask = cMemTools::nBitRight[lui8SizeStart-1];
          *lpui8Dest |= (lui8Mask & (*lpui8Source));
          lpui8Dest++;
          lpui8Source++;
        }
        if (luixSizeMid > 0)
        {
          vMemOr(lpui8Dest, lpui8Source, luixSizeMid);
          lpui8Dest   += luixSizeMid;
          lpui8Source += luixSizeMid;
        }
        if (lui8SizeEnd > 0)
        {
          lui8Mask = cMemTools::nBitLeft[lui8SizeEnd-1];
          *lpui8Dest |= (lui8Mask & (*lpui8Source));
        }
      }
    }
    else
    {
      uint8 lui8BitMskSource;
      uint8 lui8BitMskDest;

      lui8BitMskSource = 128 >> lui8SrcStartBitPos;
      lui8BitMskDest   = 128 >> lui8DestStartBitPos;

      while (luixSize > 0)
      {
        vBitOr(*lpui8Source, *lpui8Dest, lui8BitMskSource, lui8BitMskDest);

        vBitIncPos(lpui8Source, lui8BitMskSource);
        vBitIncPos(lpui8Dest,   lui8BitMskDest);

        luixSize--;
      }
    }
  }


  static void vMemBitXor(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint8* lpui8Dest, uint8 lui8DestStartBitPos, uint32 luixSize)
  {
    uint8  lui8Mask;

    if (lui8SrcStartBitPos == lui8DestStartBitPos)
    {
      uint8  lui8SizeStart;
      uint32 luixSizeMid;
      uint8  lui8SizeEnd;

      if (lui8SrcStartBitPos > 0)
      {
        lui8SizeStart = 8 - lui8SrcStartBitPos;
      }
      else
      {
        lui8SizeStart = 0;
      }

      if (luixSize >= lui8SizeStart)
      {
        luixSize     -= lui8SizeStart;
      }
      else
      {
        lui8SizeStart = (uint8)luixSize;
        luixSize = 0;
      }

      lui8SizeEnd  = ((uint8)luixSize & 7);

      if (luixSize >= lui8SizeEnd)
      {
        luixSizeMid  = (luixSize - lui8SizeEnd) >> 3;
      }
      else
      {
        luixSizeMid = 0;
      }

      if ((luixSizeMid == 0) && (lui8SizeEnd == 0))
      {
        lui8Mask = cMemTools::nBitLeft[lui8SizeStart-1];
        lui8Mask >>= lui8SrcStartBitPos;
        *lpui8Dest ^= (lui8Mask & (*lpui8Source));
      }
      else
      {
        if (lui8SizeStart > 0)
        {
          lui8Mask = cMemTools::nBitRight[lui8SizeStart-1];
          *lpui8Dest ^= (lui8Mask & (*lpui8Source));
          lpui8Dest++;
          lpui8Source++;
        }
        if (luixSizeMid > 0)
        {
          vMemXor(lpui8Dest, lpui8Source, luixSizeMid);
          lpui8Dest   += luixSizeMid;
          lpui8Source += luixSizeMid;
        }
        if (lui8SizeEnd > 0)
        {
          lui8Mask = cMemTools::nBitLeft[lui8SizeEnd-1];
          *lpui8Dest ^= (lui8Mask & (*lpui8Source));
        }
      }
    }
    else
    {
      uint8 lui8BitMskSource;
      uint8 lui8BitMskDest;

      lui8BitMskSource = 128 >> lui8SrcStartBitPos;
      lui8BitMskDest   = 128 >> lui8DestStartBitPos;

      while (luixSize > 0)
      {
        vBitXor(*lpui8Source, *lpui8Dest, lui8BitMskSource, lui8BitMskDest);

        vBitIncPos(lpui8Source, lui8BitMskSource);
        vBitIncPos(lpui8Dest,   lui8BitMskDest);

        luixSize--;
      }
    }
  }


  static void vMemBit2u16Cpy(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint16* lpui16Dest, uint16 lui16Value, uint32 luixSize)
  {
    uint8 lui8BitMskSource;

    lui8BitMskSource = 128 >> lui8SrcStartBitPos;

    while (luixSize > 0)
    {
      if (*lpui8Source & lui8BitMskSource)
      {
        *lpui16Dest = lui16Value;
      }
      else
      {
        *lpui16Dest = 0;
      }

      vBitIncPos(lpui8Source, lui8BitMskSource);
      lpui16Dest++;

      luixSize--;
    }
  }


  static void vMemBit2u16And(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint16* lpui16Dest, uint16 lui16Value, uint32 luixSize)
  {
    uint8 lui8BitMskSource;

    lui8BitMskSource = 128 >> lui8SrcStartBitPos;

    while (luixSize > 0)
    {
      if (*lpui8Source & lui8BitMskSource)
      {
        *lpui16Dest &= lui16Value;
      }
      else
      {
        *lpui16Dest = 0;
      }

      vBitIncPos(lpui8Source, lui8BitMskSource);
      lpui16Dest++;

      luixSize--;
    }
  }


  static void vMemBit2u16Or(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint16* lpui16Dest, uint16 lui16Value, uint32 luixSize)
  {
    uint8 lui8BitMskSource;

    lui8BitMskSource = 128 >> lui8SrcStartBitPos;

    while (luixSize > 0)
    {
      if (*lpui8Source & lui8BitMskSource)
      {
        *lpui16Dest |= lui16Value;
      }

      vBitIncPos(lpui8Source, lui8BitMskSource);
      lpui16Dest++;

      luixSize--;
    }
  }


  static void vMemBit2u16Xor(uint8* lpui8Source, uint8  lui8SrcStartBitPos, uint16* lpui16Dest, uint16 lui16Value, uint32 luixSize)
  {
    uint8 lui8BitMskSource;

    lui8BitMskSource = 128 >> lui8SrcStartBitPos;

    while (luixSize > 0)
    {
      if (*lpui8Source & lui8BitMskSource)
      {
        *lpui16Dest ^= lui16Value;
      }
      else
      {
        *lpui16Dest = 0;
      }

      vBitIncPos(lpui8Source, lui8BitMskSource);
      lpui16Dest++;

      luixSize--;
    }
  }
};

#endif /* _MEMTOOLS_H */
