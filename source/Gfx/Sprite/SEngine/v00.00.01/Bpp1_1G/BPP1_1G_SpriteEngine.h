#ifndef _BPP1_1G_SPRITE_ENGINE
#define _BPP1_1G_SPRITE_ENGINE

#include "spriteengine_base.h"
#include "screen_Bpp1_1G.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cSpriteEngine_Bpp1_1G : public cSpriteEngine
{
  public:

    cSpriteEngine_Bpp1_1G():cSpriteEngine() {}
    cSpriteEngine_Bpp1_1G(Sprite_tenMode lenMode):cSpriteEngine(lenMode) {}

    // Helpers General
    GfxImgSize muixBytePosSprite;
	  GfxImgSize muixBitPosSprite;

    GfxImgSize muixBytePosDest;
	  GfxImgSize muixBitPosDest;

    // Helper Rle Copy
    uint8      mui8SpriteRleCnt;
    uint8      mui8SpriteRleValue;

    /* 1Byte = 8Pixel
       0x80 = Most Left  Pixel
       0x01 = Most Right Pixel

       0b10000000 => X_______
       0b00000001 => _______X
    */

    void vPrepareRaw()
    {
      muixBytePosSprite = (GfxImgSize)(miGfxSpriteY1 * mpcSprite->miGfxWidth + miGfxSpriteX1);
      mpui8Sprite       = mpcSprite->mpui8Data + BM_BPP1_1G_PXLIDX2BYTEPOS(muixBytePosSprite);
      muixBitPosSprite  = BM_BPP1_1G_PXLIDX2BITPOS(muixBytePosSprite);

      muixBytePosDest = (GfxImgSize)(miGfxDestY1 * mpcScreenDest->mpcBm->miGfxWidth + miGfxDestX1);
      mpui8Dest       = mpcScreenDest->mpcBm->mpui8Data + BM_BPP1_1G_PXLIDX2BYTEPOS(muixBytePosDest);
      muixBitPosDest  = BM_BPP1_1G_PXLIDX2BITPOS(muixBytePosDest);

      muixYCnt = (miGfxSpriteY2 - miGfxSpriteY1) + 1;
      muixXCnt = (miGfxSpriteX2 - miGfxSpriteX1) + 1;
    }

    SPRITE_INLINE void vLoopUpdateRaw()
    {
      muixBitPosSprite  += mpcSprite->miGfxWidth;
      mpui8Sprite       += BM_BPP1_1G_PXLIDX2BYTEPOS(muixBitPosSprite);
      muixBitPosSprite   = BM_BPP1_1G_PXLIDX2BITPOS(muixBitPosSprite);

      muixBitPosDest    += mpcScreenDest->mpcBm->miGfxWidth;
      mpui8Dest         += BM_BPP1_1G_PXLIDX2BYTEPOS(muixBitPosDest);
      muixBitPosDest     = BM_BPP1_1G_PXLIDX2BITPOS(muixBitPosDest);
    }
    
    FRet i8BitCpyRaw(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        vPrepareRaw();

        while (muixYCnt > 0)
        {
          cMemTools::vMemBitCpy(mpui8Sprite, muixBitPosSprite, mpui8Dest, muixBitPosDest, muixXCnt);
          vLoopUpdateRaw();
          muixYCnt--;
        }
      }
      return FRET_OK;
    }

    FRet i8BitAndRaw(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        vPrepareRaw();

        while (muixYCnt > 0)
        {
          cMemTools::vMemBitAnd(mpui8Sprite, muixBitPosSprite, mpui8Dest, muixBitPosDest, muixXCnt);
          vLoopUpdateRaw();
          muixYCnt--;
        }
      }
      return FRET_OK;
    }

    FRet i8BitOrRaw(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        vPrepareRaw();

        while (muixYCnt > 0)
        {
          cMemTools::vMemBitOr(mpui8Sprite, muixBitPosSprite, mpui8Dest, muixBitPosDest, muixXCnt);
          vLoopUpdateRaw();
          muixYCnt--;
        }
      }
      return FRET_OK;
    }


    FRet i8BitXorRaw(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        vPrepareRaw();

        while (muixYCnt > 0)
        {
          cMemTools::vMemBitXor(mpui8Sprite, muixBitPosSprite, mpui8Dest, muixBitPosDest, muixXCnt);
          vLoopUpdateRaw();
          muixYCnt--;
        }
      }
      return FRET_OK;
    }


    /* 1Byte = RLE
       0bBCCCCCCC
         B => Bitvalue 1 or 0
         C => Count    0 to 127
    */


    void vGoPxlRle(GfxImgSize muixPxlCnt)
    {
      while (muixPxlCnt > 0)
      {
        if (mui8SpriteRleCnt > muixPxlCnt)
        {
          mui8SpriteRleCnt -= muixPxlCnt;
          muixPxlCnt        = 0;
        }
        else
        {
          muixPxlCnt -= mui8SpriteRleCnt;
          mpui8Sprite++;
          mui8SpriteRleValue = ((*mpui8Sprite) & 128);
          mui8SpriteRleCnt   = ((*mpui8Sprite) & 127);
        }
      }
    }

    void vPrepareRle()
    {
      // Zum Anfang im RLE gehen
      mui8SpriteRleValue = ((*mpui8Sprite) & 128);
      mui8SpriteRleCnt   = ((*mpui8Sprite) & 127);

      vGoPxlRle(miGfxSpriteX1 + (miGfxSpriteY1 * mpcSprite->miGfxWidth));

      muixBytePosDest = (GfxImgSize)(miGfxDestY1 * mpcScreenDest->mpcBm->miGfxWidth + miGfxDestX1);
      mpui8Dest       = mpcScreenDest->mpcBm->mpui8Data + BM_BPP1_1G_PXLIDX2BYTEPOS(muixBytePosDest);
      muixBitPosDest  = BM_BPP1_1G_PXLIDX2BITPOS(muixBytePosDest);

      muixYCnt = (miGfxSpriteY2 - miGfxSpriteY1) + 1;
      muixXCnt = (miGfxSpriteX2 - miGfxSpriteX1) + 1;
    }


    FRet i8BitCpyRle(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        GfxInt luixXCnt;
        uint8  lui8BitMskDest;

        vPrepareRle();

        lui8BitMskDest  = 128 >> muixBitPosDest;

        while (muixYCnt > 0)
        {
          luixXCnt  = muixXCnt;

          while (luixXCnt > 0)
          {
            if (mui8SpriteRleValue) {*mpui8Dest |= (lui8BitMskDest);} else {*mpui8Dest &= ~(lui8BitMskDest);}

            cMemTools::vBitIncPos(mpui8Dest, lui8BitMskDest);

            mui8SpriteRleCnt--;
            if (mui8SpriteRleCnt == 0)
            {
              mpui8Sprite++;
              mui8SpriteRleValue = ((*mpui8Sprite) & 128);
              mui8SpriteRleCnt   = ((*mpui8Sprite) & 127);
              
            }
            
            luixXCnt--;
          }

          // Den Anfang der nächsten Zeile im RLE gehen
          vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

          mpui8Dest     += (mpcScreenDest->mpcBm->miGfxWidth >> 3);
          lui8BitMskDest = 128 >> muixBitPosDest;

          muixYCnt--;
        }
      }
      return FRET_OK;
    }

    FRet i8BitAndRle(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        GfxInt luixXCnt;
        uint8  lui8BitMskDest;

        vPrepareRle();

        lui8BitMskDest  = 128 >> muixBitPosDest;

        while (muixYCnt > 0)
        {
          luixXCnt  = muixXCnt;

          while (luixXCnt > 0)
          {
            if (mui8SpriteRleValue) {*mpui8Dest &= (lui8BitMskDest);}

            cMemTools::vBitIncPos(mpui8Dest, lui8BitMskDest);

            mui8SpriteRleCnt--;
            if (mui8SpriteRleCnt == 0)
            {
              mpui8Sprite++;
              mui8SpriteRleValue = ((*mpui8Sprite) & 128);
              mui8SpriteRleCnt   = ((*mpui8Sprite) & 127);
              
            }
            
            luixXCnt--;
          }

          // Den Anfang der nächsten Zeile im RLE gehen
          vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

          mpui8Dest     += (mpcScreenDest->mpcBm->miGfxWidth >> 3);
          lui8BitMskDest = 128 >> muixBitPosDest;

          muixYCnt--;
        }
      }
      return FRET_OK;
    }


    FRet i8BitOrRle(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        GfxInt luixXCnt;
        uint8  lui8BitMskDest;

        vPrepareRle();

        lui8BitMskDest  = 128 >> muixBitPosDest;

        while (muixYCnt > 0)
        {
          luixXCnt  = muixXCnt;

          while (luixXCnt > 0)
          {
            if (mui8SpriteRleValue) {*mpui8Dest |= (lui8BitMskDest);}

            cMemTools::vBitIncPos(mpui8Dest, lui8BitMskDest);

            mui8SpriteRleCnt--;
            if (mui8SpriteRleCnt == 0)
            {
              mpui8Sprite++;
              mui8SpriteRleValue = ((*mpui8Sprite) & 128);
              mui8SpriteRleCnt   = ((*mpui8Sprite) & 127);
              
            }
            
            luixXCnt--;
          }

          // Den Anfang der nächsten Zeile im RLE gehen
          vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

          mpui8Dest     += (mpcScreenDest->mpcBm->miGfxWidth >> 3);
          lui8BitMskDest = 128 >> muixBitPosDest;

          muixYCnt--;
        }
      }
      return FRET_OK;
    }

    FRet i8BitXorRle(GfxInt lixDestX, GfxInt lixDestY)
    {
      mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

      if (bClip(lixDestX, lixDestY))
      {
        GfxInt luixXCnt;
        uint8  lui8BitMskDest;

        vPrepareRle();

        lui8BitMskDest  = 128 >> muixBitPosDest;

        while (muixYCnt > 0)
        {
          luixXCnt  = muixXCnt;

          while (luixXCnt > 0)
          {
            if (mui8SpriteRleValue) {*mpui8Dest ^= (lui8BitMskDest);}

            cMemTools::vBitIncPos(mpui8Dest, lui8BitMskDest);

            mui8SpriteRleCnt--;
            if (mui8SpriteRleCnt == 0)
            {
              mpui8Sprite++;
              mui8SpriteRleValue = ((*mpui8Sprite) & 128);
              mui8SpriteRleCnt   = ((*mpui8Sprite) & 127);
              
            }
            
            luixXCnt--;
          }

          // Den Anfang der nächsten Zeile im RLE gehen
          vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

          mpui8Dest     += (mpcScreenDest->mpcBm->miGfxWidth >> 3);
          lui8BitMskDest = 128 >> muixBitPosDest;

          muixYCnt--;
        }
      }
      return FRET_OK;
    }


    FRet i8Set(GfxInt lixDestX, GfxInt lixDestY)
    {
      Sprite_tenFormat lenFormat;

      lenFormat = mpcSprite->menFormat;
      switch (lenFormat)
      {
        case Sprite_nFormatRaw:
          return ((this->*i8Bitx[(u8)menMode])(lixDestX, lixDestY));
        break;
        case Sprite_nFormatRle:
          return ((this->*i8Bitx[(u8)(menMode + Sprite_nModeLastEntry)])(lixDestX, lixDestY));
        break;
      }
      return FRET_ERR_GENERAL;
    }

    FRet i8Set(GfxInt lixDestX, GfxInt lixDestY, cSprite *lpcSprite)
    {
      mpcSprite = lpcSprite;
      return i8Set(lixDestX, lixDestY);
    }
};


#ifdef __cplusplus
}
#endif


#endif // _BPP1_1G_SPRITE_ENGINE

