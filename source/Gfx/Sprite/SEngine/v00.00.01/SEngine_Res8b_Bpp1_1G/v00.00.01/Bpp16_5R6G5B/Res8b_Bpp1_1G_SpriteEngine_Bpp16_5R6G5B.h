#ifndef _RES8B_BPP1_1G_SPRITEENGINE_BPP16_5R6G5B
#define _RES8B_BPP1_1G_SPRITEENGINE_BPP16_5R6G5B

#include "bitmap.h"
#include "screen.h"
#include "Res8b_Bpp1_1G_SpriteEngine_Base.h"
#include "SpriteEngine.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B : public cSpriteEngine_Res8b_Bpp1_1G_Base, public cSpriteEngine
{
  public:
    // Helpers General
    GfxImgSize muixBytePosSprite;
    GfxImgSize muixBitPosSprite;

    // Helper Rle Copy
    uint8      mui8SpriteRleCnt;
    uint8      mui8SpriteRleValue;

    /* 1Byte = 8Pixel
       0x80 = Most Left  Pixel
       0x01 = Most Right Pixel

       0b10000000 => X_______
       0b00000001 => _______X
    */

    cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B()
      :cSpriteEngine_Res8b_Bpp1_1G_Base(), cSpriteEngine()
    {
    }

    cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B(Sprite_tenMode lenMode)
      :cSpriteEngine_Res8b_Bpp1_1G_Base(), cSpriteEngine(lenMode)
    {
    }

  void vPrepareRaw()
  {
    GfxImgSize luixBytePosDest;

    muixBytePosSprite = (GfxImgSize)(miGfxSpriteY1 * mpcSprite->miGfxWidth + miGfxSpriteX1);
    mpui8Sprite = mpcSprite->mpui8Data + BM_BPP1_1G_PXLIDX2BYTEPOS(muixBytePosSprite);
    muixBitPosSprite = BM_BPP1_1G_PXLIDX2BITPOS(muixBytePosSprite);

    luixBytePosDest = (GfxImgSize)(miGfxDestY1 * mpcScreenDest->mpcBm->miGfxWidth + miGfxDestX1);
    mpui8Dest = mpcScreenDest->mpcBm->mpui8Data + BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(luixBytePosDest);

    muixYCnt = (miGfxSpriteY2 - miGfxSpriteY1) + 1;
    muixXCnt = (miGfxSpriteX2 - miGfxSpriteX1) + 1;
  }

  SPRITE_INLINE void vLoopUpdateRaw()
  {
    muixBitPosSprite += mpcSprite->miGfxWidth;
    mpui8Sprite += BM_BPP1_1G_PXLIDX2BYTEPOS(muixBitPosSprite);
    muixBitPosSprite = BM_BPP1_1G_PXLIDX2BITPOS(muixBitPosSprite);

    mpui8Dest += BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(mpcScreenDest->mpcBm->miGfxWidth);
  }

  FRet i8BitCpyRaw(GfxInt lixDestX, GfxInt lixDestY)
  {
    mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

    if (bClip(lixDestX, lixDestY))
    {
      vPrepareRaw();

      while (muixYCnt > 0)
      {
        cMemTools::vMemBit2u16Cpy(mpui8Sprite, muixBitPosSprite, (uint16*)mpui8Dest, mui32Col, muixXCnt);
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
        cMemTools::vMemBit2u16And(mpui8Sprite, muixBitPosSprite, (uint16*)mpui8Dest, mui32Col, muixXCnt);
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
        cMemTools::vMemBit2u16Or(mpui8Sprite, muixBitPosSprite, (uint16*)mpui8Dest, mui32Col, muixXCnt);
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
        cMemTools::vMemBit2u16Xor(mpui8Sprite, muixBitPosSprite, (uint16*)mpui8Dest, mui32Col, muixXCnt);
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
        muixPxlCnt = 0;
      }
      else
      {
        muixPxlCnt -= mui8SpriteRleCnt;
        mpui8Sprite++;
        mui8SpriteRleValue = ((*mpui8Sprite) & 128);
        mui8SpriteRleCnt = ((*mpui8Sprite) & 127);
      }
    }
  }

  void vPrepareRle()
  {
    GfxImgSize luixBytePosDest;
    // Zum Anfang im RLE gehen
    mui8SpriteRleValue = ((*mpui8Sprite) & 128);
    mui8SpriteRleCnt = ((*mpui8Sprite) & 127);

    vGoPxlRle(miGfxSpriteX1 + (miGfxSpriteY1 * mpcSprite->miGfxWidth));

    luixBytePosDest = (GfxImgSize)(miGfxDestY1 * mpcScreenDest->mpcBm->miGfxWidth + miGfxDestX1);
    mpui8Dest = mpcScreenDest->mpcBm->mpui8Data + BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(luixBytePosDest);

    muixYCnt = (miGfxSpriteY2 - miGfxSpriteY1) + 1;
    muixXCnt = (miGfxSpriteX2 - miGfxSpriteX1) + 1;
  }


  FRet i8BitCpyRle(GfxInt lixDestX, GfxInt lixDestY)
  {
    mpcScreenDest->vRel2Abs(lixDestX, lixDestY);

    if (bClip(lixDestX, lixDestY))
    {
      GfxInt luixXCnt;
      uint16* lpui16Dest;

      vPrepareRle();

      while (muixYCnt > 0)
      {
        luixXCnt   = muixXCnt;
        lpui16Dest = (uint16*)mpui8Dest;

        while (luixXCnt > 0)
        {
          if (mui8SpriteRleValue)
            *lpui16Dest = mui32Col;
          else
            *lpui16Dest = 0;


          lpui16Dest++;

          mui8SpriteRleCnt--;
          if (mui8SpriteRleCnt == 0)
          {
            mpui8Sprite++;
            mui8SpriteRleValue = ((*mpui8Sprite) & 128);
            mui8SpriteRleCnt = ((*mpui8Sprite) & 127);
          }

          luixXCnt--;
        }

        // Den Anfang der nächsten Zeile im RLE gehen
        vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

        mpui8Dest += BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(mpcScreenDest->mpcBm->miGfxWidth);
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
      uint16* lpui16Dest;

      vPrepareRle();

      while (muixYCnt > 0)
      {
        luixXCnt = muixXCnt;
        lpui16Dest = (uint16*)mpui8Dest;

        while (luixXCnt > 0)
        {
          if (mui8SpriteRleValue)
            *lpui16Dest &= mui32Col;
          else
            *lpui16Dest = 0;


          lpui16Dest++;

          mui8SpriteRleCnt--;
          if (mui8SpriteRleCnt == 0)
          {
            mpui8Sprite++;
            mui8SpriteRleValue = ((*mpui8Sprite) & 128);
            mui8SpriteRleCnt = ((*mpui8Sprite) & 127);
          }

          luixXCnt--;
        }

        // Den Anfang der nächsten Zeile im RLE gehen
        vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

        mpui8Dest += BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(mpcScreenDest->mpcBm->miGfxWidth);
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
      uint16* lpui16Dest;

      vPrepareRle();

      while (muixYCnt > 0)
      {
        luixXCnt = muixXCnt;
        lpui16Dest = (uint16*)mpui8Dest;

        while (luixXCnt > 0)
        {
          if (mui8SpriteRleValue)
            *lpui16Dest |= mui32Col;
          else
            *lpui16Dest = 0;


          lpui16Dest++;

          mui8SpriteRleCnt--;
          if (mui8SpriteRleCnt == 0)
          {
            mpui8Sprite++;
            mui8SpriteRleValue = ((*mpui8Sprite) & 128);
            mui8SpriteRleCnt = ((*mpui8Sprite) & 127);
          }

          luixXCnt--;
        }

        // Den Anfang der nächsten Zeile im RLE gehen
        vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

        mpui8Dest += BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(mpcScreenDest->mpcBm->miGfxWidth);
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
      uint16* lpui16Dest;

      vPrepareRle();

      while (muixYCnt > 0)
      {
        luixXCnt = muixXCnt;
        lpui16Dest = (uint16*)mpui8Dest;

        while (luixXCnt > 0)
        {
          if (mui8SpriteRleValue)
            *lpui16Dest ^= mui32Col;
          else
            *lpui16Dest = 0;


          lpui16Dest++;

          mui8SpriteRleCnt--;
          if (mui8SpriteRleCnt == 0)
          {
            mpui8Sprite++;
            mui8SpriteRleValue = ((*mpui8Sprite) & 128);
            mui8SpriteRleCnt = ((*mpui8Sprite) & 127);
          }

          luixXCnt--;
        }

        // Den Anfang der nächsten Zeile im RLE gehen
        vGoPxlRle(miGfxSpriteX1 + (mpcSprite->miGfxWidth - (miGfxSpriteX2 + 1)));

        mpui8Dest += BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(mpcScreenDest->mpcBm->miGfxWidth);
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

  FRet i8Set(GfxInt lixDestX, GfxInt lixDestY, const uint8* lpui8Resource)
  {
    if (((cSprite_Res8b_Bpp1_1G*)mpcSprite)->vSetRes8B(lpui8Resource) == SPRITE_OK)
    {
      i8Set(lixDestX, lixDestY);
      return SPRITEENGINE_OK;
    }
    return SPRITEENGINE_FAIL;
  }
};


#ifdef __cplusplus
}
#endif


#endif // _RES8B_BPP1_1G_SPRITEENGINE_BPP16_5R6G5B

