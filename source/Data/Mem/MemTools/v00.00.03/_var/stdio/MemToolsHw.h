#ifndef _MEMTOOLSHW_H
#define _MEMTOOLSHW_H

#include "TypeDef.h"
#include <string.h>

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
    memcpy(lpui8Dest, lpui8Source, luixSize);
  }

  static void vMemSet(uint8* lpui8Dest, uint8  lui8Value, MemTool_uiSize luixSize)
  {
    memset(lpui8Dest, lui8Value, luixSize);
  }


  static void vMemSet16(uint16* lpui16Dest, uint16 lui16Value, MemTool_uiSize luixSize)
  {
    memset((u8*)lpui16Dest, lui16Value, luixSize * 2);
  }


  static void vMemSet32(uint32* lpui32Dest, uint32 lui32Value, MemTool_uiSize luixSize)
  {
    memset((u8*)lpui32Dest, lui32Value, luixSize * 4);
  }
};

#endif /* _MEMTOOLSHW_H */
