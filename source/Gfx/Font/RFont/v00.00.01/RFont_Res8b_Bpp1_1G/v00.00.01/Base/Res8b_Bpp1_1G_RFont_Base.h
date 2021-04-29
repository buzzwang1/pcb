#ifndef _RES8B_BPP1_1G_RFONT_BASE_H__
#define _RES8B_BPP1_1G_RFONT_BASE_H__

#include "RFont_Base.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cRFont_Res8b_Bpp1_1G : public cRFont
{
  public:
    typedef struct
    {
      uint8               u8XOffset;
      uint8               u8YOffset;
      uint8               u8Width;
      uint8               u8Height;
    } tstCharRes8B;

    cSprite_Res8b_Bpp1_1G     mcSprite;
    cSpriteEngine            *mpcSpriteEng;

    tmp_i8SpriteEngineBitx   mi8Bitx;

    cRFont_Res8b_Bpp1_1G(cSpriteEngine *lpcSpriteEng)
      :cRFont()
    {
      mpcSpriteEng = lpcSpriteEng;
    }

    cRFont_Res8b_Bpp1_1G(const uint8 *lpui8RFontRes8B, const uint8 *lpaui8CharData, cSpriteEngine *lpcSpriteEng)
      :cRFont()
    {
      mpcSpriteEng = lpcSpriteEng;
      i8InitFromRes8B(lpui8RFontRes8B, lpaui8CharData);
    }

    FRet i8InitFromRes8B(const uint8 *lpui8RFontRes8B, const uint8 *lpaui8CharData)
    {
      if ((lpui8RFontRes8B[0] == Sprite_nType_ArrayFont) &&
          (lpui8RFontRes8B[1] == Sprite_nBPP_BPP1_1G))
      {
        menFormat  = (Sprite_tenFormat)lpui8RFontRes8B[2]; // RAW, RLE, ...

        mui8IdxMin = lpui8RFontRes8B[3];
        mui8IdxMax = lpui8RFontRes8B[4];

        miGfxSpacing = lpui8RFontRes8B[5];
        miGfxHeight  = lpui8RFontRes8B[6];
        miGfxYOffset = lpui8RFontRes8B[7];
        
        maui16CharLut = (uint16*)(lpui8RFontRes8B + 8);
        maui8CharData = (uint8*)lpaui8CharData;

        switch (menFormat)
        {
        case Sprite_nFormatRaw:
          mi8Bitx = (cSpriteEngine_Bpp1_1G::i8Bitx[(u8)mpcSpriteEng->menMode]);
          break;
        case Sprite_nFormatRle:
          mi8Bitx = ((cSpriteEngine_Bpp1_1G::i8Bitx[(u8)(mpcSpriteEng->menMode + Sprite_nModeLastEntry)]));
          break;
        default:
          mi8Bitx = (cSpriteEngine_Bpp1_1G::i8Bitx[(u8)mpcSpriteEng->menMode]);
          break;
        }

        return RFONT_OK;
      }
      return RFONT_FAIL;
    }

    tstGfxPos2D i8PutString(cScreen *lpcScr, bool lbSizeOnly = False)
    {
      uint32 lui32SignIdx;
      uint16 lui16LetterIdx;

      uint8  lui8Letter;

      tstGfxPos2D lstPos  = mstStartPos;
      tstGfxPos2D lstSize;
      GfxInt      liGfxSizeLineX;

      tstCharRes8B  *lpstCharData;
      uint8         *lpui8CharData;

      lstSize.x = 0;
      lstSize.y = miGfxHeight;
      liGfxSizeLineX = 0;

      mpcSpriteEng->vSetParam(mpcSpriteEng->menMode, &mcSprite, lpcScr);
      mpcSpriteEng->vSetCol(mui32Col);

      for (lui32SignIdx = 0; lui32SignIdx < mcText.Len(); lui32SignIdx++)
      {
        lui8Letter = mcText.mpu8Data[lui32SignIdx];

        if ((lui8Letter >= mui8IdxMin) &&
            (lui8Letter <= mui8IdxMax))
        {
          lui16LetterIdx = maui16CharLut[lui8Letter - mui8IdxMin];

          if (lui16LetterIdx != 0xFFFF)
          {
            lpstCharData = (tstCharRes8B*)(maui8CharData + lui16LetterIdx);
            lpui8CharData = (uint8*)lpstCharData + sizeof(tstCharRes8B);

            if (!lbSizeOnly)
            {
              mcSprite.vSet(lpstCharData->u8Width, lpstCharData->u8Height, menFormat, Sprite_nBPP_BPP1_1G, lpui8CharData);            
              (mpcSpriteEng->*mi8Bitx)(lstPos.x + (int8)lpstCharData->u8XOffset,
                                       lstPos.y - (lpstCharData->u8Height - 1) - (int8)lpstCharData->u8YOffset);
            }
            
            lstPos.x += (int8)lpstCharData->u8XOffset + lpstCharData->u8Width + miGfxSpacing;
            liGfxSizeLineX += (int8)lpstCharData->u8XOffset + lpstCharData->u8Width + miGfxSpacing;
            if (liGfxSizeLineX > lstSize.x) lstSize.x = liGfxSizeLineX;
          }
        }
        else
        {
          // Einige Sonderzeichen behandeln: \0, \r, \n, \b
          switch (lui8Letter)
          {
          case '\r':
            break;

          case '\n': // Zeilenumbruch
            if ((lui32SignIdx + 1) < mcText.Len())
            {
              lstPos.x   = mstStartPos.x;
              lstPos.y  += miGfxHeight;
              lstSize.y += miGfxHeight;
              liGfxSizeLineX = 0;
            }
            break;

            //case '\b': // Ein Zeichen zurück
            //  BPP1_1G_HTrtlMinFP_AddPos2(-VF_SPACE_X(1), 0, lhTurtle);
            //break;

          default:
            lstPos.x += miGfxSpacing;
            liGfxSizeLineX += miGfxSpacing;
            break;
          }
        }
      }

      return lstSize;
    }


    tstGfxPos2D i8PutStringXY(GfxInt lixDestX, GfxInt lixDestY, char8 const *lszStr, cScreen *lpcScr, bool lbSizeOnly = False)
    {
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From((char*) lszStr);

      return i8PutString(lpcScr, lbSizeOnly);
    };

    tstGfxPos2D i8PutStringXY(GfxInt lixDestX, GfxInt lixDestY, cStr* lcStr, cScreen* lpcScr, bool lbSizeOnly = False)
    {
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From(lcStr);

      return i8PutString(lpcScr, lbSizeOnly);
    };

    tstGfxPos2D i8StringLen(tstGfxPos2D* lpstStrLen)
    {
      uint32 lui32SignIdx;
      uint16 lui16LetterIdx;

      tstCharRes8B  *lpstCharData;

      uint32 lui32StrWidthMax;

      uint8  lui8Letter;

      lpstStrLen->x = 0;
      lpstStrLen->y = miGfxHeight;
      lui32StrWidthMax = 0;

      for (lui32SignIdx = 0; lui32SignIdx < mcText.Len(); lui32SignIdx++)
      {
        lui8Letter = mcText.mpu8Data[lui32SignIdx];

        if ((lui8Letter >= mui8IdxMin) &&
          (lui8Letter <= mui8IdxMax))
        {
          lui16LetterIdx = maui16CharLut[lui8Letter - mui8IdxMin];

          if (lui16LetterIdx != 0xFFFF)
          {
            lpstCharData = (tstCharRes8B*)(maui8CharData + lui16LetterIdx);
            {
              lpstStrLen->x += (int8)lpstCharData->u8XOffset + lpstCharData->u8Width + miGfxSpacing;

              if (lui32StrWidthMax < (u32)lpstStrLen->x)
              {
                lui32StrWidthMax = (u32)lpstStrLen->x;
              }
            }
          }
        }
        else
        {
          // Einige Sonderzeichen behandeln: \0, \r, \n, \b, SPACE
          switch (lui8Letter)
          {
          case '\r':
            break;

          case '\n': // Zeilenumbruch
            lpstStrLen->y += miGfxHeight;
            lpstStrLen->x  = 0;
            break;

            //case '\b': // Ein Zeichen zurück
            //  BPP1_1G_HTrtlMinFP_AddPos2(-VF_SPACE_X(1), 0, lhTurtle);
            //break;

          default:
            //lstPos.x += mpstLut->stLutHeader.iGfxSpacing;
            break;
          }
        }
      }

      lpstStrLen->x = lui32StrWidthMax;

      if (lpstStrLen->x)
      {
        lpstStrLen->x -= miGfxSpacing;
      }
      return *lpstStrLen;
    }

    u32 u32StringWidth()
    {
      uint32 lui32SignIdx;
      uint16 lui16LetterIdx;
      uint32 lui32StrWidth;
      uint32 lui32StrWidthMax;

      tstCharRes8B  *lpstCharData;
      uint8  lui8Letter;

      lui32StrWidth = 0;
      lui32StrWidthMax = 0;

      for (lui32SignIdx = 0; lui32SignIdx < mcText.Len(); lui32SignIdx++)
      {
        lui8Letter = mcText.mpu8Data[lui32SignIdx];

        if ((lui8Letter >= mui8IdxMin) &&
          (lui8Letter <= mui8IdxMax))
        {
          lui16LetterIdx = maui16CharLut[lui8Letter - mui8IdxMin];

          if (lui16LetterIdx != 0xFFFF)
          {
            lpstCharData = (tstCharRes8B*)(maui8CharData + lui16LetterIdx);
            {
              lui32StrWidth += (int8)lpstCharData->u8XOffset + lpstCharData->u8Width + miGfxSpacing;

              if (lui32StrWidthMax < lui32StrWidth)
              {
                lui32StrWidthMax = lui32StrWidth;
              }
            }
          }
        }
        else
        {
          // Einige Sonderzeichen behandeln: \0, \r, \n, \b, SPACE
          switch (lui8Letter)
          {
          case '\r':
            break;

            //case ' ': // Leerzeichen
            //  BPP1_1G_HTrtlMinFP_AddPos2(VF_SPACE_X(1), 0, lhTurtle);
            //break;

          case '\n': // Zeilenumbruch
            lui32StrWidth = 0;
            break;

            //case '\b': // Ein Zeichen zurück
            //  BPP1_1G_HTrtlMinFP_AddPos2(-VF_SPACE_X(1), 0, lhTurtle);
            //break;

          default:
            //lstPos.x += mpstLut->stLutHeader.iGfxSpacing;
            break;
          }
        }
      }

      if (lui32StrWidthMax)
      {
        lui32StrWidthMax -= miGfxSpacing;
      }

      return lui32StrWidthMax;
    }

    u32 u32CharWidth(char8 lChr)
    {
      uint16 lui16LetterIdx;
      tstCharRes8B* lpstCharData;

      if ((lChr >= mui8IdxMin) &&
          (lChr <= mui8IdxMax))
      {
        lui16LetterIdx = maui16CharLut[lChr - mui8IdxMin];

        if (lui16LetterIdx != 0xFFFF)
        {
          lpstCharData = (tstCharRes8B*)(maui8CharData + lui16LetterIdx);
          {
            return (int8)lpstCharData->u8XOffset + lpstCharData->u8Width + miGfxSpacing;
          }
        }
      }
      return 0;
    }
};

#ifdef __cplusplus
}
#endif

#endif // _RES8B_BPP1_1G_RFONT_BASE_H__

