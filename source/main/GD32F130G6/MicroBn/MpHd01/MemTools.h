#ifndef _MEMTOOLS_H
#define _MEMTOOLS_H

#include "TypeDef.h"
#include "stdio.h"
#include "string.h" // for memcpy

class cMemTools
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


};

#endif /* _MEMTOOLS_H */
