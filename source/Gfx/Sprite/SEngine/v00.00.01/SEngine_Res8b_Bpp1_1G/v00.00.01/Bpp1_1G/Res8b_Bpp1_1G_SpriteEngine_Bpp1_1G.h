#ifndef _RES8B_BPP1_1G_SPRITEENGINE_BPP1_1G
#define _RES8B_BPP1_1G_SPRITEENGINE_BPP1_1G

#include "bitmap.h"
#include "screen.h"
#include "Res8b_Bpp1_1G_SpriteEngine_Base.h"
#include "SpriteEngine.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G : public cSpriteEngine_Res8b_Bpp1_1G_Base, public cSpriteEngine_Bpp1_1G
{
  public:
    cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G() :cSpriteEngine_Res8b_Bpp1_1G_Base(), cSpriteEngine_Bpp1_1G() {}

   cRes8b_Bpp1_1G_SpriteEngine_Bpp1_1G(Sprite_tenMode lenMode)
      :cSpriteEngine_Res8b_Bpp1_1G_Base(), cSpriteEngine_Bpp1_1G(lenMode)
    {}
   
    using  cSpriteEngine_Bpp1_1G::i8Set;

    FRet i8Set(GfxInt lixDestX, GfxInt lixDestY, const uint8* lpui8Resource)
    {
      if (((cSprite_Res8b_Bpp1_1G*)mpcSprite)->vSetRes8B(lpui8Resource) == SPRITE_OK)
      {
        cSpriteEngine_Bpp1_1G::i8Set(lixDestX, lixDestY);
        return SPRITEENGINE_OK;
      }
      return SPRITEENGINE_FAIL;
    }
};


#ifdef __cplusplus
}
#endif


#endif // _RES8B_BPP1_1G_SPRITEENGINE_BPP1_1G

