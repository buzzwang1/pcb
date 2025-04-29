#ifndef _BPP1_1G_RES8B_SPRITE_BASE_H__
#define _BPP1_1G_RES8B_SPRITE_BASE_H__

#include "bitmap.h"
#include "screen.h"
#include "sprite.h"


#ifdef __cplusplus
 extern "C" {
#endif

class cResSprite
{
  public:
    uint8 enMode;
    uint8 enFormat;
    uint8 ui8Width;
    uint8 ui8Height;
    //uint8 aui8Data[0];
};

class cSprite_Res8b_Bpp1_1G : public cSprite
{
public:

  cSprite_Res8b_Bpp1_1G() : cSprite() {}

  FRet vSetRes8B(const uint8* lpui8Resource)
  {
    if ((lpui8Resource[0] == Sprite_nType_Sprite) && (lpui8Resource[1] == Sprite_nBPP_BPP1_1G))
    {
      cSprite::vSet(lpui8Resource[3], lpui8Resource[4], (Sprite_tenFormat)lpui8Resource[2], Sprite_nBPP_BPP1_1G, lpui8Resource + 5);

      return FRET_OK;
    }
    return FRET_ERR_GENERAL;
  }

};


#ifdef __cplusplus
}
#endif


#endif // _BPP1_1G_RES8B_SPRITE_BASE_H__

