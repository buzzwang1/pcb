#ifndef _MEMTOOLSHW_H
#define _MEMTOOLSHW_H

#include "TypeDef.h"

// Speziel für ARM CM§ und CM4, d.h. immer 32 Bit

typedef u32 MemTool_uiSize;

class cMemToolsHw
{
  public:


  static void vMemCpy(uint8* lpui8Dest, uint8* lpui8Source, MemTool_uiSize luixSize) // __attribute__((optimize("-Ofast")))
  {
    MemTool_uiSize luixCount;


    // Note: Compiler on -O3 will compile this using STRD (store two consecutive registers, 64-bit doubleword) or LDRD (loads two words from memory)
    //       Therefore the address must be a multiple of eight for doubleword transfers, an alignment exception if alignment checking is enabled


    // Check if Source and Dest can be aligned to 64Bit and size is >31 Byte
    if (((((uint32)lpui8Source) & 7) == (((uint32)lpui8Dest) & 7)) &&
         (luixSize > 31))
    {
      // Try to copy with 64 Bit

      // Align to 64Bit adress
      while ((((u32)lpui8Dest) & 0x07) && (luixSize > 0))
      {
        *lpui8Dest++ = *lpui8Source++;
        luixSize--;
      }

      u32* lpui32Source = (u32*)lpui8Source;
      u32* lpui32Dest   = (u32*)lpui8Dest;

      if (luixSize > 31)
      {
        luixCount = luixSize >> 5; // vMemCpy: 32Byte Blöcke
        luixSize &= 31;

        while (luixCount > 0) // vMemCpy: 8 * 4 Byte = 32Byte
        {
          *lpui32Dest++ = *lpui32Source++;
          *lpui32Dest++ = *lpui32Source++;
          *lpui32Dest++ = *lpui32Source++;
          *lpui32Dest++ = *lpui32Source++;
          *lpui32Dest++ = *lpui32Source++;
          *lpui32Dest++ = *lpui32Source++;
          *lpui32Dest++ = *lpui32Source++;
          *lpui32Dest++ = *lpui32Source++;
          luixCount--;
        }
      }
      // Copy with 32 Bit, taking advantage of STM ability to read/write unaligned data
      if (luixSize > 3)
      {
        luixCount = luixSize >> 2; // vMemCpy: 4Byte Blöcke
        luixSize &= 3;

        while (luixCount > 0) // vMemCpy: 1 * 4 Byte = 4Byte
        {
          *lpui32Dest++ = *lpui32Source++;
          luixCount--;
        }
      }

      // Copy with 8 Bit, taking advantage of STM ability to read/write unaligned data
      lpui8Dest = (uint8*)lpui32Dest;
      lpui8Source = (uint8*)lpui32Source;
      while (luixSize > 0)
      {
        *lpui8Dest++ = *lpui8Source++;
        luixSize--;
      }
    }
    else
    {
      u32* lpui32Source = (u32*)lpui8Source;
      u32* lpui32Dest = (u32*)lpui8Dest;

      // Copy with 32 Bit, taking advantage of STM ability to read/write unaligned data
      if (luixSize > 3)
      {
        luixCount = luixSize >> 2; // vMemCpy: 4Byte Blöcke
        luixSize &= 3;

        while (luixCount > 0) // vMemCpy: 1 * 4 Byte = 4Byte
        {
          *lpui32Dest++ = *lpui32Source++;
          luixCount--;
        }
      }

      // Copy with 8 Bit, taking advantage of STM ability to read/write unaligned data
      lpui8Dest = (uint8*)lpui32Dest;
      lpui8Source = (uint8*)lpui32Source;
      while (luixSize > 0)
      {
        *lpui8Dest++ = *lpui8Source++;
        luixSize--;
      }
    }
  }


  // Copy longwords, taking advantage of STM ability to read/write unaligned data
  // and sum up the copied data
  static u32 vMemCpyAndSum(uint8* lpui8Dest, uint8* lpui8Source, MemTool_uiSize luixSize)
  {
    MemTool_uiSize luixCount;
    u32            u32Sum = 0;
    //if (((((uint32)lpui8Source) & 3) == 0) && // auf ganzen 32 Bit Adresses ?
    //    ((((uint32)lpui8Dest) & 3) == 0))
    {
      /* Alles auf 32Bit Adresses und Count ist auch 32Bit
         bzw. alles Werte sind durch 4 teilbar
         Dann mit 32Bit breite Kopieren
      */

      uint32* lpui32Source = (uint32*)lpui8Source;
      uint32* lpui32Dest = (uint32*)lpui8Dest;

      if (luixSize > 15)
      {
        luixCount = luixSize >> 4; // 16Byte Blöcke
        luixSize &= 15;

        while (luixCount > 0) // 4 * 4 Byte = 16Byte
        {
          u32Sum += *lpui32Source; *lpui32Dest++ = *lpui32Source++;
          u32Sum += *lpui32Source; *lpui32Dest++ = *lpui32Source++;
          u32Sum += *lpui32Source; *lpui32Dest++ = *lpui32Source++;
          u32Sum += *lpui32Source; *lpui32Dest++ = *lpui32Source++;
          luixCount--;
        }
      }

      if (luixSize > 3)
      {
        luixCount = luixSize >> 2; // 4Byte Blöcke
        luixSize &= 3;

        while (luixCount > 0) // 1 * 4 Byte = 16Byte
        {
          u32Sum += *lpui32Source; *lpui32Dest++ = *lpui32Source++;
          luixCount--;
        }
      }

      lpui8Dest = (uint8*)lpui32Dest;
      lpui8Source = (uint8*)lpui32Source;
      // Den Rest byteweise
      while (luixSize > 0)
      {
        u32Sum += *lpui8Source; *lpui8Dest++ = *lpui8Source++;
        luixSize--;
      }
    }
    return u32Sum;
  }

//#pragma GCC push_options
//#pragma GCC optimize ("Og")

  static void vMemSet(uint8* lpui8Dest, uint8  lui8Value, MemTool_uiSize luixSize)
  {
    if (luixSize <= 4)
    {
      while (luixSize)
      {
        *lpui8Dest++ = lui8Value;
        luixSize--;
      }
    }
    else
    { // Mehr als 4 (>4Byte)
      // Versuchen mit 32Bit zu kopieren.
      // wenn es nicht 32Bit aligned ist, dann auf 32Bit align'en
      while ((((u32)lpui8Dest) & 0x3) && (luixSize > 0))
      {
        *lpui8Dest++ = lui8Value;
        luixSize--;
      }

      vMemSet32((uint32*)lpui8Dest, ((u32)lui8Value << 24) + ((u32)lui8Value << 16) + ((u32)lui8Value << 8) + lui8Value, luixSize / 4);

      lpui8Dest += luixSize & (~0x3);
      luixSize &= 3;

      //Restlichen Daten kopieren
      while (luixSize)
      {
        *lpui8Dest++ = lui8Value;
        luixSize--;
      }
    }
  }

  // luixSize in 16Bit Werten, z.B. 2 => 4Byte
  // lpui16Dest muss 16Bit aligned sein
  static void vMemSet16(uint16* lpui16Dest, uint16 lui16Value, MemTool_uiSize luixSize)
  {
    // Zu wenig Daten oder Daten auf keiner geraden 16Bit Adresse
    if ((luixSize <= 8) || (((u32)lpui16Dest) & 0x01))
    {
      while (luixSize)
      {
        *lpui16Dest++ = lui16Value;
        luixSize--;
      }
    }
    else
    {
      // Wenn wir hier ankommen dann ist:
      //  - Mehr als 8 (>16Byte)
      //  - Destpointer ist eine ganze 16Bit Adresse

      // Versuchen mit 32Bit zu kopieren.
      // wenn es nicht 32Bit aligned ist, dann auf 32Bit align'en
      if (((u32)lpui16Dest) & 0x1)
      {
        *lpui16Dest++ = lui16Value;
        luixSize--;
      }

      vMemSet32((uint32*)lpui16Dest, ((u32)lui16Value << 16) + lui16Value, luixSize / 2);

      //Evtl. letztes Wort kopieren
      if (luixSize & 0x1)
      {
        lpui16Dest += luixSize & (~0x1);
        *lpui16Dest++ = lui16Value;
      }
    }
  }

  // luixSize in 32Bit Werten, z.B. 2 => 8Byte
  // lpui32Dest muss 32Bit aligned sein
  static void vMemSet32(uint32* lpui32Dest, uint32 lui32Value, MemTool_uiSize luixSize)  // __attribute__((optimize("-O0")))
  {
    // Zu wenig Daten oder Daten auf keiner ganzen 32Bit Adresse
    if ((luixSize <= 8) || (((u32)lpui32Dest) & 0x03))
    {
      while (luixSize)
      {
        *lpui32Dest++ = lui32Value;
        luixSize--;
      }
    }
    else
    {
      // Wenn wir hier ankommen dann ist:
      //  - Mehr als 8 (>32Byte)
      //  - Destpointer ist eine ganze 32Bit Adresse
      // Versuchen mit 64Bit zu kopieren.
      // wenn es nicht 64Bit aligned ist, dann auf 64Bit align'en
      if (((u32)lpui32Dest) & 0x07)
      {
        *lpui32Dest++ = lui32Value;
        luixSize--;
      }

      //Versuchen 32 Byte Blöcke zu kopieren
      if (luixSize > 32 / 4)
      {
        MemTool_uiSize luixCount;

        luixCount = luixSize / 8;  //  x * 8 = x * 4 / 32;  '* 4' u32 auf Byte; ' / 32' 32 Byte Blöcke
        luixSize -= luixCount * 8;

        // Note: Compiler on -O3 will compile this to 4x STRD (store two consecutive registers, 64-bit doubleword)
        //       Therefore the address must be a multiple of eight for doubleword transfers, an alignment exception if alignment checking is enabled
        while (luixCount > 0) // 8 * 4 Byte = 32Byte
        {
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          *lpui32Dest++ = lui32Value;
          luixCount--;
        }
      }

      //Restlichen Daten kopieren
      while (luixSize)
      {
        *lpui32Dest++ = lui32Value;
        luixSize--;
      }
    }
  }

  //#pragma GCC pop_options
};



#endif
