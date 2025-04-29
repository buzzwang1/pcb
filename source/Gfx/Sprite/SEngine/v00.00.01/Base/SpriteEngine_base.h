#ifndef __SPRITE_ENGINE_BASE_H__
#define __SPRITE_ENGINE_BASE_H__

/* Includes ------------------------------------------------------------------*/
#include "Bitmap.h"
#include "Screen.h"
#include "Sprite.h"
#include "SpriteDef.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define SPRITEENGINE_INVALID            SPRITE_INVALID
#define SPRITEENGINE_FAIL               SPRITE_FAIL
#define SPRITEENGINE_OK                 SPRITE_OK
#define SPRITEENGINE_INLINE             SPRITE_INLINE


class cSpriteEngine;
typedef FRet(cSpriteEngine::*tmp_i8SpriteEngineBitx)(GfxInt, GfxInt);

class cSpriteEngine
{
  public:
    static const tmp_i8SpriteEngineBitx i8Bitx[Sprite_nModeLastEntry * 2];

    Sprite_tenMode menMode; //Cpy, And, Or, ...

    // Helpers General
    cSprite   *mpcSprite;
    GfxInt     miGfxSpriteX1;
    GfxInt     miGfxSpriteY1;
    GfxInt     miGfxSpriteX2;
    GfxInt     miGfxSpriteY2;
    uint8*     mpui8Sprite;

    cScreen   *mpcScreenDest;
    GfxInt     miGfxDestX1;
    GfxInt     miGfxDestY1;
    GfxInt     miGfxDestX2;
    GfxInt     miGfxDestY2;
    uint8*     mpui8Dest;

    // Helper Raw Copy
    GfxInt  muixYCnt;
    GfxInt  muixXCnt;

    uint32  mui32Col;


    cSpriteEngine() 
    {
      mui32Col = 0xFFFFFF;
    }

    cSpriteEngine(Sprite_tenMode lenMode)
    {
      menMode  = lenMode;
      mui32Col = 0xFFFFFF;
    }

    void vSetParam(Sprite_tenMode lenMode, cScreen* lpcScreenDest)
    {
      menMode       = lenMode;
      mpcScreenDest = lpcScreenDest;
    }

    void vSetParam(Sprite_tenMode lenMode, cSprite* lpcSprite, cScreen* lpcScreenDest)
    {
      menMode       = lenMode;
      mpcSprite     = lpcSprite;
      mpcScreenDest = lpcScreenDest;
    }



    bool bClip(GfxInt liGfxDestX1, GfxInt liGfxDestY1)
    {
      miGfxDestX1 = liGfxDestX1;
      miGfxDestY1 = liGfxDestY1;
      miGfxDestX2 = miGfxDestX1 + mpcSprite->miGfxWidth - 1;
      miGfxDestY2 = miGfxDestY1 + mpcSprite->miGfxHeight - 1;

      if (mpcScreenDest->isInAbs(miGfxDestX1, miGfxDestY1, miGfxDestX2, miGfxDestY2))
      {
        miGfxSpriteX1 = 0;
        miGfxSpriteY1 = 0;
        miGfxSpriteX2 = mpcSprite->miGfxWidth - 1;
        miGfxSpriteY2 = mpcSprite->miGfxHeight - 1;

        if (miGfxDestX1 < mpcScreenDest->miGfxClipPosx1) { miGfxSpriteX1 = mpcScreenDest->miGfxClipPosx1 - miGfxDestX1; miGfxDestX1 = mpcScreenDest->miGfxClipPosx1; }
        if (miGfxDestY1 < mpcScreenDest->miGfxClipPosy1) { miGfxSpriteY1 = mpcScreenDest->miGfxClipPosy1 - miGfxDestY1; miGfxDestY1 = mpcScreenDest->miGfxClipPosy1; }

        if (miGfxDestX2 > mpcScreenDest->miGfxClipPosx2) miGfxSpriteX2 = miGfxSpriteX1 + mpcScreenDest->miGfxClipPosx2 - miGfxDestX1;
        if (miGfxDestY2 > mpcScreenDest->miGfxClipPosy2) miGfxSpriteY2 = miGfxSpriteY1 + mpcScreenDest->miGfxClipPosy2 - miGfxDestY1;

        return True;
      }
      return False;
    }

    void vSetCol(u32 lu32Col)
    {
      mui32Col = lu32Col;
    }

	  virtual FRet i8BitCpyRaw(GfxInt lixDestX, GfxInt lixDestY) = 0;
	  virtual FRet i8BitAndRaw(GfxInt lixDestX, GfxInt lixDestY) = 0;
	  virtual FRet i8BitOrRaw(GfxInt lixDestX, GfxInt lixDestY) = 0;
	  virtual FRet i8BitXorRaw(GfxInt lixDestX, GfxInt lixDestY) = 0;

	  virtual FRet i8BitCpyRle(GfxInt lixDestX, GfxInt lixDestY) = 0;
	  virtual FRet i8BitAndRle(GfxInt lixDestX, GfxInt lixDestY) = 0;
	  virtual FRet i8BitOrRle(GfxInt lixDestX, GfxInt lixDestY) = 0;
	  virtual FRet i8BitXorRle(GfxInt lixDestX, GfxInt lixDestY) = 0;

	  virtual FRet i8Set(GfxInt lixDestX, GfxInt lixDestY) = 0;
	  virtual FRet i8Set(GfxInt lixDestX, GfxInt lixDestY, cSprite *lpcSprite) = 0;

    ~cSpriteEngine()
    {
    }
};


#ifdef __cplusplus
}
#endif

#endif  //SPRITE_ENGINE_BASE
