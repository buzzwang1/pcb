
#ifndef __IGRAPHY_BASE_H__
#define __IGRAPHY_BASE_H__

#include "TypeDef.h"

#include "cStringTools.h"
#include "screen.h"
#include "rfont.h"
#include "screen.h"
#include "paint.h"

#include "iGraphY_Chn.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define GRAPHY_MAX_CHANNELS         64

#define GRAPHY_MODE_DOTS      0
#define GRAPHY_MODE_LINES     1

#define IGRAPHY_INVALID        BM_INVALID
#define IGRAPHY_FAIL           BM_FAIL
#define IGRAPHY_OK             BM_OK
#define IGRAPHY_INLINE         BM_INLINE


class ciGraphY
{
public:

  // Abmessungen des Screens
  GfxInt miGfxWidth;
  GfxInt miGfxHeight;

  // Abmessungen des Zeichenbereichs
  GfxInt miGfxGraphWidth;
  GfxInt miGfxGraphHeight;

  // Rand für Beschriftung
  GfxInt miGfxSpaceX;
  GfxInt miGfxSpaceY;

  //Skalierung
  int16   mi16SkalXCnt;
  int16   mi16SkalXMin;
  int16   mi16SkalXMax;

  //Gitterlinien
  int8    mi8Grid;

  //Farben
  uint32  mi32ColBg;
  uint32  mi32ColLines;
  uint32  mi32ColGrid;

  //Beschriftung
  char8    mszLabelX[16];
  int16    mi16LabelXDiv;

  cRFont  *mpstRFont;
  cScreen *mpstDest;  // Ziel des Graphes

  ciGraphY(cRFont *lpstRFont, GfxInt liGfxWidth, GfxInt liGfxHeight, cScreen *lpstDest)
  : ciGraphY(lpstRFont, liGfxWidth, liGfxHeight)
  {
    mpstDest = lpstDest;
  }

  ciGraphY(cRFont *lpstRFont, GfxInt liGfxWidth, GfxInt liGfxHeight)
  {
    tstGfxPos2D stStrLen;
    char8 lszTemp[] = "0";

    mpstRFont = lpstRFont;

    mi16SkalXCnt = 10;

    mi16SkalXMin  = 0;
    mi16SkalXMax  = 100;
    mi16LabelXDiv = 1;

    mi8Grid = 1;

    cStrTools::szStrCpy(mszLabelX, "X-Values");

    mi32ColBg    = 0x202020;
    mi32ColLines = 0xFFFFFF;
    mi32ColGrid  = 0x808080;

    mpstRFont->mcText.From(lszTemp);
    mpstRFont->i8StringLen(&stStrLen);

    miGfxSpaceX = stStrLen.x + 3;
    miGfxSpaceY = stStrLen.y + 4;

    vSetSize(liGfxWidth, liGfxHeight);

    miGfxGraphWidth  = miGfxWidth  - 2 * miGfxSpaceX;
    miGfxGraphHeight = miGfxHeight - 2 * miGfxSpaceY;
  }

  void vSetSize(GfxInt liGfxWidth, GfxInt liGfxHeight)
  {
    miGfxWidth  = liGfxWidth;
    miGfxHeight = liGfxHeight;

    miGfxGraphWidth  = miGfxWidth  - 2 * miGfxSpaceX;
    miGfxGraphHeight = miGfxHeight - 2 * miGfxSpaceY;
  }

  void vSetSpace(GfxInt liGfxX, GfxInt liGfxY)
  {
    miGfxSpaceX = liGfxX;
    miGfxSpaceY = liGfxY;

    miGfxGraphWidth  = miGfxWidth  - 2 * miGfxSpaceX;
    miGfxGraphHeight = miGfxHeight - 2 * miGfxSpaceY;
  }


  void vSetScreen(cScreen *lpcScreen)
  {
    mpstDest = lpcScreen;
  }

  BM_INLINE i32 i32XValuetoPixelRelGraph(i16 li16Value)
  {
    return (int32)(((int32)miGfxGraphWidth * (li16Value - mi16SkalXMin)) / (mi16SkalXMax - mi16SkalXMin));
  }

  BM_INLINE i32 i32YValuetoPixelRelGraph(i16 li16Value, ciGraphY_Channel* lpcChn)
  {
    return (miGfxGraphHeight - (int32)(((int32)miGfxGraphHeight * (li16Value - lpcChn->mi16SkalYMin)) / (lpcChn->mi16SkalYMax - lpcChn->mi16SkalYMin)));
  }

  BM_INLINE void vValueToPixelRelGraph(stPxli16* lpstValue, stPxli16* lpstPixel, ciGraphY_Channel* lpcChn)
  {
    lpstPixel->x = i32XValuetoPixelRelGraph(lpstValue->x);
    lpstPixel->y = i32YValuetoPixelRelGraph(lpstValue->y, lpcChn);
  }

  BM_INLINE void vValueToPixelRelScreen(stPxli16* lpstValue, stPxli16* lpstPixel, ciGraphY_Channel* lpcChn)
  {
    vValueToPixelRelGraph(lpstValue, lpstPixel, lpcChn);

    lpstPixel->x = miGfxSpaceX + lpstPixel->x;
    lpstPixel->y = miGfxSpaceY + lpstPixel->y;
  }

  BM_INLINE void vValueToPixelAbs(stPxli16* lpstValue, stPxli16* lpstPixel, ciGraphY_Channel* lpcChn)
  {
    vValueToPixelRelScreen(lpstValue, lpstPixel, lpcChn);

    lpstPixel->x = mpstDest->miGfxRefPosx1 + lpstPixel->x;
    lpstPixel->y = mpstDest->miGfxRefPosy1 + lpstPixel->y;
  }

  FRet i8PixelToi16Value(stPxli16* lpstValue, stPxli16* lpstPixel)
  {
    UNUSED(lpstValue);
    UNUSED(lpstPixel);
    /*
    int16 li16StartX;
    int16 li16StartY;
    int16 li16Breite;
    int16 li16Hoehe;

    li16StartX = mi16PosX1 + miGfxSpaceX;
    li16StartY = mi16PosY1 + miGfxSpaceY;
    li16Breite = mi16PosX2 - mi16PosX1 - 2 * miGfxSpaceX;
    li16Hoehe  = mi16PosY2 - mi16PosY1 - 2 * miGfxSpaceY;

    lpstValue->x = (((lpstPixel->x)  - (li16StartX))             * mi16SkalX_Value) / li16Breite;
    lpstValue->y = (((-lpstPixel->y) + (li16StartY + li16Hoehe)) * mi16SkalY_Value) / li16Hoehe;
    */
    return 0;
  }

  void vSet_X_Label(const char8* lszText)
  {
    cStrTools::szStrnCpy(mszLabelX, lszText, sizeof(mszLabelX));
  }


  void vPaint_X_Label(stPxli16* lpstOffset)
  {
    int16 li16StartX1;
    int16 li16StartY1;
    int16 li16StartX2;
    int16 li16StartY2;

    //SCREEN_CHK_INIT((*mpstDest));
    //CHECK((*mpstDest));

    li16StartX1 = mpstDest->miGfxRefPosx1;
    li16StartY1 = mpstDest->miGfxRefPosy1;
    li16StartX2 = li16StartX1 + this->miGfxWidth - 1;
    li16StartY2 = li16StartY1 + this->miGfxHeight - 1;

    li16StartX1 += miGfxSpaceX + lpstOffset->x;
    li16StartY1 += miGfxSpaceY + lpstOffset->y;
    li16StartX2 += -miGfxSpaceX + lpstOffset->x;
    li16StartY2 += -miGfxSpaceY + lpstOffset->y;

    //Beschriftung
    {
      tstGfxPos2D stStrLen;

      mpstRFont->mui32Col = mpstDest->u32GetCol(mi32ColLines);
      mpstRFont->mcText.From(mszLabelX);
      mpstRFont->i8StringLen(&stStrLen);

      mpstRFont->mstStartPos.x = li16StartX2 - (stStrLen.x + 2) - li16StartX1 + miGfxSpaceX;
      mpstRFont->mstStartPos.y = li16StartY2 - 2                - li16StartY1 + miGfxSpaceY;

      mpstRFont->i8PutString(mpstDest);
    }
  }

  void vPaint_X_Raster(stPxli16* lpstOffset)
  {
    if (mi8Grid) // Gitter zeichnen ?    
    {
      int16 li16x;
      int16 li16StartX1;
      int16 li16StartY1;
      //int16 li16StartX2;
      int16 li16StartY2;

      //SCREEN_CHK_INIT((*mpstDest));
      //CHECK((*mpstDest));

      li16StartX1 = this->miGfxSpaceX + lpstOffset->x;
      li16StartY1 = this->miGfxSpaceY + lpstOffset->y;
      //li16StartX2 = li16StartX1 + this->miGfxGraphWidth - 1;
      li16StartY2 = li16StartY1 + this->miGfxGraphHeight - 1;

      // Gitterlinien und Beschriftung
      {
        int16 li16t;
        int32 li32PosX;

        li32PosX = mi16SkalXMin;

        for (li16t = 0; li16t <= mi16SkalXCnt; li16t++)
        {
          // Striche zeichnen
          li16x = li16StartX1 + i32XValuetoPixelRelGraph(li32PosX);

          mpstDest->vLineV(li16x, li16StartY1, li16StartY2, mi32ColGrid);

          li32PosX += (mi16SkalXMax - mi16SkalXMin) / mi16SkalXCnt;
        }
      }
    }
  }


  void vPaint_X_Axis(stPxli16* lpstOffset, ciGraphY_Channel* lpcChn)
  {    
    int16 li16x;
    int16 li16StartX1;
    int16 li16StartY1;
    //int16 li16StartX2;
    int16 li16StartY2;

    //SCREEN_CHK_INIT((*mpstDest));
    //CHECK((*mpstDest));

    li16StartX1 = this->miGfxSpaceX + lpstOffset->x;
    li16StartY1 = this->miGfxSpaceY + lpstOffset->y;
    //li16StartX2 = li16StartX1 + this->miGfxGraphWidth - 1;
    li16StartY2 = li16StartY1 + this->miGfxGraphHeight - 1;

    // Gitterlinien und Beschriftung
    {
      int16 li16t;
      char8 lszTemp[10];

      int32 li32PosX;
      int32 li32PosY;

      tstGfxPos2D stStrLen;

      if ((lpcChn->mi16SkalYMin > 0) || (lpcChn->mi16SkalYMax < 0))
      {
        li32PosY = li16StartY2 + 1;
      }
      else
      {
        //li32PosY = li16StartY1 + i32YValuetoPixelRelGraph(0, lpcChn);
        li32PosY = li16StartY2 + 1;
      }
      li32PosX = mi16SkalXMin;

      for (li16t = 0; li16t <= mi16SkalXCnt; li16t++)
      {
        // Striche zeichnen
        li16x = li16StartX1 + i32XValuetoPixelRelGraph(li32PosX);

        cStrTools::uixItoa(li32PosX / mi16LabelXDiv, lszTemp, 10);

        mpstDest->vLineV(li16x, li32PosY - 2, li32PosY + 2, mi32ColLines);

        mpstRFont->mui32Col = mpstDest->u32GetCol(mi32ColLines);
        mpstRFont->mcText.From(lszTemp);
        mpstRFont->i8StringLen(&stStrLen);

        mpstRFont->mstStartPos.x = li16x - (stStrLen.x / 2);
        mpstRFont->mstStartPos.y = li32PosY + (stStrLen.y + 1);

        mpstRFont->i8PutString(mpstDest);

        li32PosX += (mi16SkalXMax - mi16SkalXMin) / mi16SkalXCnt;
      }

      // X-Achse 
      // Koordinatenkreuz zeichnen;
      mpstDest->vLineV(li16StartX1 + i32XValuetoPixelRelGraph(0), li16StartY1, li16StartY2, mi32ColLines);
      //mpstDest->vLineH(li16StartX1, li16StartX2, li16StartY1, mi32ColLines);
      //mpstDest->vLineH(li16StartX1, li16StartX2, li16StartY2, mi32ColLines);
    }
  }


  void vPaint_Y_Label(stPxli16* lpstOffset, ciGraphY_Channel* lpcChn)
  {
    int16 li16StartX1;
    int16 li16StartY1;
    int16 li16StartX2;
    int16 li16StartY2;

    //SCREEN_CHK_INIT((*mpstDest));
    //CHECK((*mpstDest));

    li16StartX1 = mpstDest->miGfxRefPosx1;
    li16StartY1 = mpstDest->miGfxRefPosy1;
    li16StartX2 = li16StartX1 + this->miGfxWidth - 1;
    li16StartY2 = li16StartY1 + this->miGfxHeight - 1;

    li16StartX1 += miGfxSpaceX  + lpstOffset->x;
    li16StartY1 += miGfxSpaceY  + lpstOffset->y;
    li16StartX2 += -miGfxSpaceX + lpstOffset->x;
    li16StartY2 += -miGfxSpaceY + lpstOffset->y;

    //Beschriftung
    {
      tstGfxPos2D stStrLen;

      mpstRFont->mui32Col = mpstDest->u32GetCol(lpcChn->mui32Col);
      mpstRFont->mcText.From((char8*)lpcChn->mszLabelY);
      mpstRFont->i8StringLen(&stStrLen);

      mpstRFont->mstStartPos.x = li16StartX1 - mpstDest->miGfxRefPosx1;
      mpstRFont->mstStartPos.y = li16StartY1 - 2 - mpstDest->miGfxRefPosy1;

      mpstRFont->i8PutString(mpstDest);
    }
  }
  
  void vPaint_Y_Raster(stPxli16* lpstOffset, ciGraphY_Channel* lpcChn)
  {
    if (mi8Grid) // Gitter zeichnen ?    
    {
      int16 li16y;
      int16 li16StartX1;
      int16 li16StartY1;
      int16 li16StartX2;
      //int16 li16StartY2;

      //SCREEN_CHK_INIT((*mpstDest));
      //CHECK((*mpstDest));

      li16StartX1 = this->miGfxSpaceX + lpstOffset->x;
      li16StartY1 = this->miGfxSpaceY + lpstOffset->y;
      li16StartX2 = li16StartX1 + this->miGfxGraphWidth - 1;
      //li16StartY2 = li16StartY1 + this->miGfxGraphHeight - 1;

      // Gitterlinien und Beschriftung
      {
        int16 li16t;

        int32 li32PosY;

        li32PosY = lpcChn->mi16SkalYMin;

        for (li16t = 0; li16t <= lpcChn->mi16SkalYCnt; li16t++)
        {
          // Striche zeichnen
          li16y = li16StartY1 + i32YValuetoPixelRelGraph(li32PosY, lpcChn);
          mpstDest->vLineH(li16StartX1, li16StartX2, li16y, mi32ColGrid);
          li32PosY += (lpcChn->mi16SkalYMax - lpcChn->mi16SkalYMin) / lpcChn->mi16SkalYCnt;
        }
      }
    }
  }


  void vPaint_Y_Axis(stPxli16* lpstOffset, ciGraphY_Channel* lpcChn)
  {
    int16 li16y;
    int16 li16StartX1;
    int16 li16StartY1;
    int16 li16StartX2;
    //int16 li16StartY2;

    //SCREEN_CHK_INIT((*mpstDest));
    //CHECK((*mpstDest));

    li16StartX1 = this->miGfxSpaceX + lpstOffset->x;
    li16StartY1 = this->miGfxSpaceY + lpstOffset->y;
    li16StartX2 = li16StartX1 + this->miGfxGraphWidth  - 1;
    //li16StartY2 = li16StartY1 + this->miGfxGraphHeight - 1;

    // Gitterlinien und Beschriftung
    {
      int16 li16t;
      char8 lszTemp[10];

      int32 li32PosY;
      int32 li32PosX;

      tstGfxPos2D stStrLen;

      if ((mi16SkalXMin > 0) || (mi16SkalXMax < 0))
      {
        li32PosX = li16StartX1;
      }
      else
      {
        //li32PosX = li16StartX1 + i32XValuetoPixelRelGraph(0);
        li32PosX = li16StartX1;
      }
      li32PosY = lpcChn->mi16SkalYMin;


      for (li16t = 0; li16t <= lpcChn->mi16SkalYCnt; li16t++)
      {
        // Striche zeichnen
        li16y = li16StartY1 + i32YValuetoPixelRelGraph(li32PosY, lpcChn);

        cStrTools::uixItoa(li32PosY / lpcChn->mi16LabelYDiv, lszTemp, 10);

        mpstDest->vLineH(li32PosX - 2, li32PosX + 2, li16y, mi32ColLines);

        mpstRFont->mcText.From(lszTemp);
        mpstRFont->i8StringLen(&stStrLen);

        mpstRFont->mstStartPos.x = -3 - stStrLen.x + li32PosX;
        mpstRFont->mstStartPos.y = li16y + (stStrLen.y) - mpstRFont->miGfxHeight / 2 - mpstRFont->miGfxYOffset;

        mpstRFont->i8PutString(mpstDest);

        li32PosY += (lpcChn->mi16SkalYMax - lpcChn->mi16SkalYMin) / lpcChn->mi16SkalYCnt;
      }

      // Y-Achse 
      // Koordinatenkreuz zeichnen;
      mpstDest->vLineH(li16StartX1, li16StartX2, li16StartY1 + i32YValuetoPixelRelGraph(0, lpcChn), mi32ColLines);
      //mpstDest->vLineV(li16StartX2, li16StartY1, li16StartY2, mi32ColLines);
    }
  }

  void vPaint_i16Channel(stPxli16* lpstOffset, ciGraphY_Channel* lpcChn)
  {
    stPxli16 lstSrcPxl1;
    stPxli16 lstDestPxl1;
    stPxli16 lstDestPxl2;
    uint16   lui16Idx;
    uint16   lui16Pos;

    UNUSED(lpstOffset);

    //SCREEN_CHK_INIT((*mpstDest));
    //CHECK((*mpstDest));

    //AOI sichern und Festlegen
    cScreenArea cAreaSave(mpstDest);
    mpstDest->vSetArea(miGfxSpaceX, miGfxSpaceY, this->miGfxGraphWidth + miGfxSpaceX, this->miGfxGraphHeight + miGfxSpaceY, mpstDest);

    lui16Pos = lpcChn->mu16Outpos;

    lstDestPxl2.x = 0;
    lstDestPxl2.y = 0;

    for (lui16Idx = 0; lui16Idx < lpcChn->mu16Count; lui16Idx++)
    {
      lstSrcPxl1.x = lui16Idx;
      lstSrcPxl1.y = lpcChn->mi16P[lui16Pos];

      vValueToPixelRelGraph(&lstSrcPxl1, &lstDestPxl1, lpcChn);

      if (lui16Idx == 0)
      {
        lstDestPxl2.x = lstDestPxl1.x;
        lstDestPxl2.y = lstDestPxl1.y;
      }

      switch (lpcChn->mui8DrawMode)
      {
        case GRAPHY_MODE_DOTS:
          mpstDest->vPp(lstDestPxl1.x,
                        lstDestPxl1.y,
                        lpcChn->mui32Col);
          break;

        case GRAPHY_MODE_LINES:
          mpstDest->vLine(lstDestPxl1.x, lstDestPxl1.y,
                          lstDestPxl2.x, lstDestPxl2.y,
                          lpcChn->mui32Col);
          break;
      }

      lstDestPxl2.x = lstDestPxl1.x;
      lstDestPxl2.y = lstDestPxl1.y;

      lui16Pos++;
      if (lui16Pos >= lpcChn->mu16Size)
      {
        lui16Pos = 0;
      }
    }
    mpstDest->vSetArea(&cAreaSave);
  }



  void vPaint(ciGraphY_Channel* lpcChn)
  {
    stPxli16 lstOffset;
    stPxli16 lstOffset2;
    ciGraphY_Channel* lpcChnTemp;

    GfxInt   liGfxStringWidth;

    liGfxStringWidth = 0;
    lpcChnTemp       = lpcChn;
    while (lpcChnTemp != null)
    {
      if (lpcChnTemp->miGfxSkalY_MaxStrWidth > liGfxStringWidth)
      {
        liGfxStringWidth = lpcChnTemp->miGfxSkalY_MaxStrWidth;
      }
      lpcChnTemp = lpcChnTemp->mcNext;
    }

    vSetSpace(liGfxStringWidth + 4, miGfxSpaceY);

    if ((miGfxGraphWidth > 0) &&
        (miGfxGraphWidth > 0))
    {
      lstOffset.x = 0;
      lstOffset.y = 0;
      lstOffset2.x = 0;
      lstOffset2.y = 0;

      vPaint_X_Raster(&lstOffset);
      lpcChnTemp = lpcChn;
      //while (lpcChnTemp != null)
      //{
        vPaint_Y_Raster(&lstOffset, lpcChnTemp);
        lstOffset2.x += lpcChnTemp->miGfxLabelY_MaxStrWidth + 4;
        lpcChnTemp = lpcChnTemp->mcNext;
      //}

      lpcChnTemp = lpcChn;

      lstOffset.x = 0;
      lstOffset.y = 0;
      lstOffset2.x = 0;
      lstOffset2.y = 0;

      vPaint_X_Axis(&lstOffset, lpcChnTemp);
      vPaint_X_Label(&lstOffset);

      lpcChnTemp = lpcChn;
      while (lpcChnTemp != null)
      {
        vPaint_Y_Axis(&lstOffset, lpcChnTemp);
        vPaint_Y_Label(&lstOffset2, lpcChnTemp);
        lstOffset2.x += lpcChnTemp->miGfxLabelY_MaxStrWidth + 4;
        lpcChnTemp = lpcChnTemp->mcNext;
      }

      lpcChnTemp = lpcChn;
      while (lpcChnTemp != null)
      {
        vPaint_i16Channel(&lstOffset, lpcChnTemp);
        lpcChnTemp = lpcChnTemp->mcNext;
      }
    }
  }

  void vClear()
  {
    mpstDest->vFill(mi32ColBg);
  }
};


#ifdef __cplusplus
 }
#endif


#endif // __IGRAPHY_BASE_H__



