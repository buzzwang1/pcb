#ifndef _RES8B_BPP1_1G_SPRITEENGINE_BASE_H__
#define _RES8B_BPP1_1G_SPRITEENGINE_BASE_H__

#include "bitmap.h"
#include "screen.h"
#include "spriteengine_base.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cSpriteEngine_Res8b_Bpp1_1G_Base
{
public:

  cSpriteEngine_Res8b_Bpp1_1G_Base() {}

  virtual FRet i8Set(GfxInt lixDestX, GfxInt lixDestY, const uint8* lpui8Resource) = 0;
};


#ifdef __cplusplus
}
#endif


#endif // _RES8B_BPP1_1G_SPRITEENGINE_BASE_H__

