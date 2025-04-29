#ifndef __TEXTURE_32_BASE_H__
#define __TEXTURE_32_BASE_H__

/* Includes ------------------------------------------------------------------*/
#include "Bitmap.h"
#include "MemTools.h"

#ifdef __cplusplus
 extern "C" {
#endif


#define TX32_INVALID            GFX_INVALID
#define TX32_FAIL               GFX_FAIL
#define TX32_OK                 GFX_OK
#define TX32_INLINE             GFX_INLINE


class cTexture32: public cBitmap
{
  public:

    cTexture32()
    :cBitmap()
    {}

    cTexture32(u8 lui8Type, u8* lpui8Data)
    :cBitmap(32, 32, lui8Type, lpui8Data)
    {}

    cTexture32(u8 lui8Type)
      :cBitmap(32, 32, lui8Type)
    {
    }

    ~cTexture32()
    {
      cBitmap::vDelete();
    }
};


#ifdef __cplusplus
}
#endif

#endif  //TEXTURE_32_BASE
