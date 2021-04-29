#ifndef _MEMTOOLSHW_H
#define _MEMTOOLSHW_H

#include "TypeDef.h"

#if (nDefCoreType == nDefCoreType8)
  typedef uint16 MemTool_uiSize;
#endif
#if (nDefCoreType == nDefCoreType16)
  typedef uint32 MemTool_uiSize;
#endif
#if (nDefCoreType == nDefCoreType32)
  typedef uint32 MemTool_uiSize;
#endif


class cMemToolsHw
{
  public:

  static void vMemCpy(uint8* lpui8Dest, uint8* lpui8Source, MemTool_uiSize luixSize)
  {
    #if (nDefCoreType == nDefCoreType32)
      MemTool_uiSize luixCount;

      if (((((uint32)lpui8Source) & 3) == 0) && // auf ganzen 32 Bit Adresses ?
          ((((uint32)lpui8Dest) & 3) == 0))
      {
        /* Alles auf 32Bit Adresses und Count ist auch 32Bit
           bzw. alles Werte sind durch 4 teilbar
           Dann mit 32Bit breite Kopieren
        */

        uint32* lpui32Source = (uint32*)lpui8Source;
        uint32* lpui32Dest   = (uint32*)lpui8Dest;

        if (luixSize > 15)
        {
          luixCount  = luixSize >> 4; // 16Byte Blöcke
          luixSize  &= 15;

          while (luixCount > 0) // 4 * 4 Byte = 16Byte
          {
            *lpui32Dest++ = *lpui32Source++;
            *lpui32Dest++ = *lpui32Source++;
            *lpui32Dest++ = *lpui32Source++;
            *lpui32Dest++ = *lpui32Source++;
            luixCount--;
          }
        }

        if (luixSize > 3)
        {
          luixCount  = luixSize >> 2; // 4Byte Blöcke
          luixSize  &= 3;

          while (luixCount > 0) // 1 * 4 Byte = 16Byte
          {
            *lpui32Dest++ = *lpui32Source++;
            luixCount--;
          }
        }

        lpui8Dest   = (uint8*)lpui32Dest;
        lpui8Source = (uint8*)lpui32Source;
        // Den Rest byteweise
        while (luixSize > 0)
        {
          *lpui8Dest++ = *lpui8Source++;
          luixSize--;
        }
      }
      else
      {
        while (luixSize > 0)
        {
          *lpui8Dest++ = *lpui8Source++;
          luixSize--;
        }
      }
    #else
      while (luixSize > 0)
      {
        *lpui8Dest++ = *lpui8Source++;
        luixSize--;
      }
    #endif
  }

  static void vMemSet(uint8* lpui8Dest, uint8  lui8Value, MemTool_uiSize luixSize)
  {
    #if (nDefCoreType == nDefCoreType32)
      if ((((uint32)lpui8Dest) & 3) == 0)// auf ganzen 32 Bit Adresses ?
      {
        uint32* lpui32Dest   = (uint32*)lpui8Dest;
        uint8   lui32Value[4];
        MemTool_uiSize luixCount;

        lui32Value[0] = lui8Value;
        lui32Value[1] = lui8Value;
        lui32Value[2] = lui8Value;
        lui32Value[3] = lui8Value;

        if (luixSize > 15)
        {
          luixCount  = luixSize >> 4; // 16Byte Blöcke
          luixSize  &= 15;

          while (luixCount > 0) // 4 * 4 Byte = 16Byte
          {
            *lpui32Dest++ = *(uint32*)lui32Value;
            *lpui32Dest++ = *(uint32*)lui32Value;
            *lpui32Dest++ = *(uint32*)lui32Value;
            *lpui32Dest++ = *(uint32*)lui32Value;
            luixCount--;
          }
        }

        if (luixSize > 3)
        {
          luixCount  = luixSize >> 2; // 4Byte Blöcke
          luixSize  &= 3;

          while (luixCount > 0) // 1 * 4 Byte = 16Byte
          {
            *lpui32Dest++ = *(uint32*)lui32Value;
            luixCount--;
          }
        }

        lpui8Dest = (uint8*)lpui32Dest;

        // Den Rest byteweise
        while (luixSize > 0)
        {
          *lpui8Dest++ = lui8Value;
          luixSize--;
        }
      }
      else
      {
        while (luixSize > 0)
        {
          *lpui8Dest++ = lui8Value;
          luixSize--;
        }
      }
    #else
      while (luixSize > 0)
      {
        *lpui8Dest++ = lui8Value;
        luixSize--;
      }
    #endif
  }

  #if (nDefCoreType == nDefCoreType16)
    static void vMemSet16(uint16* lpui16Dest, uint16 lui16Value, MemTool_uiSize luixSize)
    {
      if (luixSize > 8)
      {
        MemTool_uiSize luixCount;

        luixCount = luixSize >> 3; // 2x8 = 16Byte Blöcke
        luixSize &= 7;

        while (luixCount > 0) // 2 * 8 Byte = 16Byte
        {
          *lpui16Dest++ = lui16Value;
          *lpui16Dest++ = lui16Value;
          *lpui16Dest++ = lui16Value;
          *lpui16Dest++ = lui16Value;
          *lpui16Dest++ = lui16Value;
          *lpui16Dest++ = lui16Value;
          *lpui16Dest++ = lui16Value;
          *lpui16Dest++ = lui16Value;
          luixCount--;
        }
      }

      // Den Rest byteweise
      while (luixSize > 0)
      {
        *lpui16Dest++ = lui16Value;
        luixSize--;
      }
    }
    #endif

  #if (nDefCoreType == nDefCoreType32)
    static void vMemSet16(uint16* lpui16Dest, uint16 lui16Value, MemTool_uiSize luixSize)
    {
      if ((((uint32)lpui16Dest) & 3) == 0)// auf ganzen 32 Bit Adresses ?
      {
        uint32* lpui32Dest = (uint32*)lpui16Dest;
        uint32  lui32Value;
        MemTool_uiSize luixCount;

        lui32Value = (lui16Value << 16) + lui16Value;

        if (luixSize > 7)
        {
          luixCount = luixSize >> 3; // 8Byte Blöcke
          luixSize &= 7;

          while (luixCount > 0) // 4 * 4 Byte = 16Byte
          {
            *lpui32Dest++ = lui32Value;
            *lpui32Dest++ = lui32Value;
            *lpui32Dest++ = lui32Value;
            *lpui32Dest++ = lui32Value;
            luixCount--;
          }
          lpui16Dest = (uint16*)lpui32Dest;
        }

        // Den Rest byteweise
        while (luixSize > 0)
        {
          *lpui16Dest++ = lui16Value;
          luixSize--;
        }
      }
      else
      {
        while (luixSize > 0)
        {
          *lpui16Dest++ = lui16Value;
          luixSize--;
        }
      }
    }

    static void vMemSet32(uint32* lpui32Dest, uint32 lui32Value, MemTool_uiSize luixSize)
    {
      if (luixSize > 4)
      {
        MemTool_uiSize luixCount;

        luixCount  = luixSize >> 2; // 16Byte Blöcke
        luixSize  &= 3;

        while (luixCount > 0) // 4 * 4 Byte = 16Byte
        {
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          luixCount--;
        }
      }

      // Den Rest byteweise
      while (luixSize > 0)
      {
        *lpui32Dest++ = lui32Value;
        luixSize--;
      }
    }
  #endif
};

#endif /* _MEMTOOLSHW_H */
