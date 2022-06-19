#ifndef _MEMTOOLSHW_H
#define _MEMTOOLSHW_H

#include "TypeDef.h"
#include "stdio.h"
#include "string.h" // for memcpy


class cMemToolsHw
{
  public:

  static void vMemCpy(uint8* lpui8Dest, const uint8* lpui8Source, size_t luixSize)
  {
    memcpy(lpui8Dest, lpui8Source, luixSize);
  }

  static void vMemSet(uint8* lpui8Dest, const uint8  lui8Value, size_t luixSize)
  {
    memset(lpui8Dest, lui8Value, luixSize);
  }

  static void vMemSet16(uint16* lpui16Dest, const uint16 lui16Value, size_t luixSize)
  {
    while (luixSize)
    {
      *lpui16Dest++ = lui16Value;
      luixSize--;
    }
  }

  static void vMemSet32(uint32* lpui32Dest, const uint32 lui32Value, size_t luixSize)
  {
    while (luixSize)
    {
      *lpui32Dest++ = lui32Value;
      luixSize--;
    }
  }
};

#endif /* _MEMTOOLSHW_H */
