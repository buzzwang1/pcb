#ifndef __SPRITE_BASE_H__
#define __SPRITE_BASE_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap.h"
#include "SpriteDef.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define SPRITE_INVALID            BM_INVALID
#define SPRITE_FAIL               BM_FAIL
#define SPRITE_OK                 BM_OK
#define SPRITE_INLINE             BM_INLINE

class cSprite: public cBitmap
{
  public:
    Sprite_tenFormat menFormat; // RAW, RLE, ...

    cSprite()
    {
    }

    cSprite(GfxInt liGfxWidth, GfxInt liGfxHeight, Sprite_tenFormat lenFormat, uint8 lui8Type, u8* lpui8Data)
      : cBitmap(liGfxWidth, liGfxHeight, lui8Type, lpui8Data)
    {
      menFormat = lenFormat;
    }

    cSprite(GfxInt liGfxWidth, GfxInt liGfxHeight, Sprite_tenFormat lenFormat, uint8 lui8Type)
      : cBitmap(liGfxWidth, liGfxHeight, lui8Type)
    {
      menFormat = lenFormat;
    }

    SPRITE_INLINE void vSet(GfxInt liGfxWidth, GfxInt liGfxHeight, Sprite_tenFormat lenFormat, uint8 lui8Type, const u8* lpui8Data)
    {
      mpui8Data   = (uint8*)lpui8Data;
      miGfxWidth  = liGfxWidth;
      miGfxHeight = liGfxHeight;
      mui8Type    = lui8Type;
      menFormat   = lenFormat;
    }

    ~cSprite()
    {
      vDelete();
    }
};


#ifdef __cplusplus
}
#endif

#endif  //SPRITE_BASE
