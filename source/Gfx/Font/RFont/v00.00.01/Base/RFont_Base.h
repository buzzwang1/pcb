#ifndef _RFONT_BASE_H__
#define _RFONT_BASE_H__

#include "bitmap.h"
#include "screen.h"
#include "sprite.h"
#include "spriteengine.h"
#include "cStrT.h"

#ifdef __cplusplus
 extern "C" {
#endif


#define RFONT_INVALID GFX_INVALID
#define RFONT_FAIL    GFX_FAIL
#define RFONT_OK      GFX_OK
#define RFONT_INLINE  GFX_INLINE

class cRFont
{
  public:
    typedef struct
    {
      GfxInt              iGfxXOffset;
      GfxInt              iGfxYOffset;
      GfxInt              iGfxWidth;
      GfxInt              iGfxHeight;
    } tstChar;

    Sprite_tenFormat menFormat; // RAW, RLE, ...

    u8         mui8IdxMin;
    u8         mui8IdxMax;

    GfxInt     miGfxSpacing;
    GfxInt     miGfxHeight;
    GfxInt     miGfxYOffset;

    uint8     *maui8CharData;
    uint16    *maui16CharLut;

    uint32     mui32Col;

    cStr       mcText;

    tstGfxPos2D     mstStartPos;

    cRFont()
      : mcText()
    {
      mstStartPos.x = 0;
      mstStartPos.y = 0;
      mui32Col      = (uint32)-1;
    }

    virtual tstGfxPos2D i8PutString(cScreen *lpcScr, bool lbSizeOnly = False) = 0;
    virtual tstGfxPos2D i8PutStringXY(GfxInt lixDestX, GfxInt lixDestY, char8 const *lszStr, cScreen *lpcScr, bool lbSizeOnly = False) = 0;
    virtual tstGfxPos2D i8PutStringXY(GfxInt lixDestX, GfxInt lixDestY, cStr  *lszStr, cScreen *lpcScr, bool lbSizeOnly = False) = 0;
    virtual tstGfxPos2D i8StringLen(tstGfxPos2D* lpstStrLen) = 0;
    virtual u32 u32StringWidth() = 0;
    virtual u32 u32CharWidth(char8 lChr) = 0;


    tstGfxPos2D vFitSmartText(GfxInt lixDestX, GfxInt lixDestY, char8 const *lszText, cScreen *lpcScr, bool lbSizeOnly = False)
    {
      GfxInt lixWidth = lpcScr->iGfxWidth();
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From((char*) lszText);
      return vFitSmartText(lpcScr, lixWidth, lbSizeOnly);
    };

    tstGfxPos2D vFitSmartText(GfxInt lixDestX, GfxInt lixDestY, GfxInt lixWidth, char8 const *lszText, cScreen *lpcScr, bool lbSizeOnly = False)
    {
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From((char*) lszText);
      return vFitSmartText(lpcScr, lixWidth, lbSizeOnly);
    };

    tstGfxPos2D vFitSmartText(GfxInt lixDestX, GfxInt lixDestY, cStr *lszText, cScreen *lpcScr, bool lbSizeOnly = False)
    {
      GfxInt lixWidth = lpcScr->iGfxWidth();
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From(lszText);
      return vFitSmartText(lpcScr, lixWidth, lbSizeOnly);
    };


    tstGfxPos2D vFitSmartText(GfxInt lixDestX, GfxInt lixDestY, GfxInt lixWidth, cStr *lszText, cScreen *lpcScr, bool lbSizeOnly = False)
    {
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From(lszText);
      return vFitSmartText(lpcScr, lixWidth, lbSizeOnly);
    };

    tstGfxPos2D vFitSmartText(cScreen *lpcScreen, GfxInt liClipWidth, bool lbSizeOnly = False)
    {
      u32 liTextLen = mcText.Len();
      u32 liLineStart = 0;
      u32 liLineEnd = 0;
      u32 liLineWidth = 3;
      u8  lu8StartOfLine = 1;

      u8  lu8PrintLn = 0;

      u32 liWordStart = 0;
      u32 liWordEnd = 0;
      u32 liWordWidth;

      tstGfxPos2D lstSize;
      tstGfxPos2D lstTemp;

      lstSize.x = 0;
      lstSize.y = 0;

      u32 lit;
      tstGfxPos2D lstPos = mstStartPos;
      cStr   lcText   = cStr(&mcText);

      for (lit = 0; lit < liTextLen;)
      {
        if (lcText.mpu8Data[lit] == '\n')
        {
          liLineEnd = lit;
          lu8PrintLn = 1;
          lit++;
        }
        else
        if (cChr8Tools::IsSpace(lcText.mpu8Data[lit]) || (lcText.mpu8Data[lit] == '-'))
        {
          lu8StartOfLine = 0;
          while ((cChr8Tools::IsSpace(lcText.mpu8Data[lit]) || (lcText.mpu8Data[lit] == '-')) &&
                 (lcText.mpu8Data[lit] != '\n') &&
                 (lit < liTextLen))
          {
            liLineEnd++;
            liLineWidth += u32CharWidth(lcText.mpu8Data[lit]);
            if (liLineWidth > (u32)liClipWidth)
            {
              lu8PrintLn = 1;
            }
            lit++;
          }
        }
        else
        {
          liWordStart = lit;
          liWordEnd = lcText.EndOfText(liWordStart);

          mcText.muiLen = mcText.muiSize = (liWordEnd - liWordStart) + 1;
          mcText.mpu8Data = (u8*)&lcText.mpu8Data[liWordStart];
          liWordWidth = u32StringWidth();

          if ((liLineWidth + liWordWidth) < (u32)liClipWidth)
          {
            liLineWidth += liWordWidth;
            liLineEnd = liWordEnd;
            lu8StartOfLine = 0;
            lit = liLineEnd + 1;
          }
          else
          {
            lu8PrintLn = 1;
            if (lu8StartOfLine)
            {
              liLineEnd = liWordEnd;
              lit = liLineEnd + 1;
            }
          }
        }


        if (lu8PrintLn)
        {
          lu8PrintLn = 0;
          mcText.muiLen = mcText.muiSize = (liLineEnd - liLineStart) + 1;
          mcText.mpu8Data = (u8*)&lcText.mpu8Data[liLineStart];
          lstTemp = i8PutString(lpcScreen, lbSizeOnly);

          if (lstSize.x < lstTemp.x) lstSize.x = lstTemp.x;
          lstSize.y += lstTemp.y;

          mstStartPos.x  = lstPos.x;
          mstStartPos.y += miGfxHeight;

          lu8StartOfLine = 1;
          liLineWidth = 3;
          liLineStart = liLineEnd + 1;
        }
      }

      if (liLineStart <= liTextLen)
      {
        mcText.muiLen = mcText.muiSize = liTextLen - liLineStart;
        mcText.mpu8Data = (u8*)&lcText.mpu8Data[liLineStart];
        lstTemp = i8PutString(lpcScreen, lbSizeOnly);
        if (lstSize.x < lstTemp.x) lstSize.x = lstTemp.x;
        lstSize.y += lstTemp.y;
      }

      return lstSize;
    }

    tstGfxPos2D vFitText(GfxInt lixDestX, GfxInt lixDestY, char8 const* lszText, cScreen* lpcScr, bool lbSizeOnly = False)
    {
      GfxInt lixWidth = lpcScr->iGfxWidth();
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From((char*)lszText);
      return vFitText(lpcScr, lixWidth, lbSizeOnly);
    };

    tstGfxPos2D vFitText(GfxInt lixDestX, GfxInt lixDestY, GfxInt lixWidth, char8 const *lszText, cScreen *lpcScr, bool lbSizeOnly = False)
    {
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From((char*) lszText);
      return vFitText(lpcScr, lixWidth, lbSizeOnly);
    };



    tstGfxPos2D vFitText(GfxInt lixDestX, GfxInt lixDestY, cStr* lszText, cScreen* lpcScr, bool lbSizeOnly = False)
    {
      GfxInt lixWidth = lpcScr->iGfxWidth();
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From(lszText);
      return vFitText(lpcScr, lixWidth, lbSizeOnly);
    };

    tstGfxPos2D vFitText(GfxInt lixDestX, GfxInt lixDestY, GfxInt lixWidth, cStr *lszText, cScreen *lpcScr, bool lbSizeOnly = False)
    {
      mstStartPos.x = lixDestX;
      mstStartPos.y = lixDestY;
      mcText.From(lszText);
      return vFitText(lpcScr, lixWidth, lbSizeOnly);
    };

    tstGfxPos2D vFitText(cScreen *lpcScreen, GfxInt liClipWidth, bool lbSizeOnly = False)
    {
      u32 liTextLen   = mcText.Len();
      u32 liLineStart = 0;
      u32 liLineEnd   = 0;
      u32 liLineWidth = 3;
      u32 liCharWidth = 0;
      u8  lu8PrintLn  = 0;
      u32 lit;
      tstGfxPos2D lstPos   = mstStartPos;
      char8* lszText  = (char8*)mcText.mpu8Data;

      tstGfxPos2D lstSize;
      tstGfxPos2D lstTemp;

      lstSize.x = 0;
      lstSize.y = 0;

      liLineWidth += u32CharWidth(lszText[0]);

      for (lit = 0; lit < liTextLen;)
      {
        if (lszText[lit] == '\n')
        {
          liLineEnd = lit;

          if (lit > 0)
          {
            liCharWidth  = u32CharWidth(lszText[lit - 1]);
          }

          lu8PrintLn = 1;
          lit++;
        }
        else
        {
          if (lit < liTextLen)
          {
            liCharWidth  = u32CharWidth(lszText[lit + 1]);
            liLineWidth += liCharWidth;
          }

          if (liLineWidth > (u32)liClipWidth)
          {
            lu8PrintLn = 1;
          }
          liLineEnd = lit++;
        }

        if (lu8PrintLn)
        {
          lu8PrintLn = 0;
          mcText.muiLen = mcText.muiSize = (liLineEnd - liLineStart) + 1;
          mcText.mpu8Data = (u8*)&lszText[liLineStart];
          lstTemp = i8PutString(lpcScreen, lbSizeOnly);

          if (lstSize.x < lstTemp.x) lstSize.x = lstTemp.x;
          lstSize.y += lstTemp.y;

          mstStartPos.x  = lstPos.x;
          mstStartPos.y += miGfxHeight;

          liLineWidth = 3 + liCharWidth;
          liLineStart = liLineEnd + 1;
        }
      }

      if (liLineStart <= liTextLen)
      {
        mcText.muiLen = mcText.muiSize = liTextLen - liLineStart;
        mcText.mpu8Data = (u8*)&lszText[liLineStart];
        lstTemp = i8PutString(lpcScreen, lbSizeOnly);

        if (lstSize.x < lstTemp.x) lstSize.x = lstTemp.x;
        lstSize.y += lstTemp.y;
      }

      return lstSize;
    }
};


#ifdef __cplusplus
}
#endif

#endif // _RFONT_BASE_H__

