#ifndef __SCREEN_BASE_H__
#define __SCREEN_BASE_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap.h"


#ifdef __cplusplus
 extern "C" {
#endif

#define SCREEN_INVALID        BM_INVALID
#define SCREEN_FAIL           BM_FAIL
#define SCREEN_OK             BM_OK
#define SCREEN_INLINE         BM_INLINE

/* --- General  --- */
#define SCREEN_CHK_INIT(lpcScr)                   BM_CHK_INIT((*lpcScr.mpcBm))
#define SCREEN_CHK_SIZE(lcBmSource, lcBmDest)     BM_CHK_SIZE(lcBmSource, lcBmDest)
#define SCREEN_POSX(lui32Index, lcBm)             BM_POSX(lui32Index, lcBm)
#define SCREEN_POSY(lui32Index, lcBm)             BM_POSY(lui32Index, lcBm)
#define SCREEN_IDX(liGfxX, liGfxY, lcBm)          BM_IDX(liGfxX, liGfxY, lcBm)
#define SCREEN_SIZE(lcBm)                         BM_SIZE(lcBm)
#define SCREEN_DATA(lcBm)                         BM_DATA(lcBm)
#define SCREEN_CLONE(lcBmSource, lcBmDest)        BM_CLONE(lcBmSource, lcBmDest)
#define SCREEN_CLIP_COL(Col)                      BM_CLIP_COL(Col)

// Screen Clipping
#define SCREEN_CHK_AOI_X(liGfxX, lcScreen)              if ((liGfxX < lcScreen.miGfxClipPosx1) || (liGfxX > lcScreen.miGfxClipPosx2)) return 2
#define SCREEN_CHK_AOI_Y(liGfxY, lcScreen)              if ((liGfxY < lcScreen.miGfxClipPosy1) || (liGfxY > lcScreen.miGfxClipPosy2)) return 2
#define SCREEN_CHK_AOI(liGfxX, liGfxY, lcScreen)        SCREEN_CHK_AOI_X(liGfxX, lcScreen);SCREEN_CHK_AOI_Y(liGfxY, lcScreen)

#define SCREEN_CHK_AOI_X1(liGfxX, lcScreen)             if (liGfxX < lcScreen.miGfxClipPosx1) return 2
#define SCREEN_CHK_AOI_X2(liGfxX, lcScreen)             if (liGfxX > lcScreen.miGfxClipPosx2) return 2
#define SCREEN_CHK_AOI_Y1(liGfxY, lcScreen)             if (liGfxY < lcScreen.miGfxClipPosy1) return 2
#define SCREEN_CHK_AOI_Y2(liGfxY, lcScreen)             if (liGfxY > lcScreen.miGfxClipPosy2) return 2

#define SCREEN_CLIP_AOI_X1(liGfxX, lcScreen)            if (liGfxX < lcScreen.miGfxClipPosx1) liGfxX = lcScreen.miGfxClipPosx1
#define SCREEN_CLIP_AOI_X2(liGfxX, lcScreen)            if (liGfxX > lcScreen.miGfxClipPosx2) liGfxX = lcScreen.miGfxClipPosx2
#define SCREEN_CLIP_AOI_Y1(liGfxY, lcScreen)            if (liGfxY < lcScreen.miGfxClipPosy1) liGfxY = lcScreen.miGfxClipPosy1
#define SCREEN_CLIP_AOI_Y2(liGfxY, lcScreen)            if (liGfxY > lcScreen.miGfxClipPosy2) liGfxY = lcScreen.miGfxClipPosy2

/* --------------------------------------------------------------------------------- */

class cScreenArea
{
  public:
  // Referenzpunkt Absolute Position im Bitmap
  GfxInt miGfxRefPosx1;
  GfxInt miGfxRefPosy1;
  
  // Clipping in absoluter Position im Bitmap
  GfxInt miGfxClipPosx1;
  GfxInt miGfxClipPosy1;     //  (x1/y1)------+
  GfxInt miGfxClipPosx2;     //  |            |
  GfxInt miGfxClipPosy2;     //  +------(x2/y2)

  bool   mbVisible;

  cScreenArea()
  {
    miGfxRefPosx1 = 0;
    miGfxRefPosy1 = 0;

    miGfxClipPosx1 = 0;
    miGfxClipPosy1 = 0;
    miGfxClipPosx2 = 0;
    miGfxClipPosy2 = 0;

    mbVisible = True;
  }

  cScreenArea(cScreenArea* lpsScreenArea)
  {
    vSetArea(lpsScreenArea);
  }

  void vClone(cScreenArea* lpcArea)
  {
    lpcArea->miGfxRefPosx1 = miGfxRefPosx1;
    lpcArea->miGfxRefPosy1 = miGfxRefPosy1;
    //lpcArea->miGfxRefPosx2 = miGfxRefPosx2;
    //lpcArea->miGfxRefPosy2 = miGfxRefPosy2;

    lpcArea->miGfxClipPosx1 = miGfxClipPosx1;
    lpcArea->miGfxClipPosy1 = miGfxClipPosy1;
    lpcArea->miGfxClipPosx2 = miGfxClipPosx2;
    lpcArea->miGfxClipPosy2 = miGfxClipPosy2;

    lpcArea->mbVisible = mbVisible;
  }

  void vSetArea(cScreenArea* lpsScreenArea)
  {
    miGfxRefPosx1 = lpsScreenArea->miGfxRefPosx1;
    miGfxRefPosy1 = lpsScreenArea->miGfxRefPosy1;
    //miGfxRefPosx2 = lpsScreenArea->miGfxRefPosx2;
    //miGfxRefPosy2 = lpsScreenArea->miGfxRefPosy2;

    miGfxClipPosx1 = lpsScreenArea->miGfxClipPosx1;
    miGfxClipPosy1 = lpsScreenArea->miGfxClipPosy1;
    miGfxClipPosx2 = lpsScreenArea->miGfxClipPosx2;
    miGfxClipPosy2 = lpsScreenArea->miGfxClipPosy2;

    mbVisible = lpsScreenArea->mbVisible;
  }

  void vSetArea(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cScreenArea* lpsScreenArea)
  {
    vRel2Abs(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2);

    miGfxRefPosx1 = liGfxPosx1;
    miGfxRefPosy1 = liGfxPosy1;
    //miGfxRefPosx2 = liGfxPosx2;
    //miGfxRefPosy2 = liGfxPosy2;

    if (liGfxPosx1 < lpsScreenArea->miGfxClipPosx1) liGfxPosx1 = lpsScreenArea->miGfxClipPosx1;
    if (liGfxPosy1 < lpsScreenArea->miGfxClipPosy1) liGfxPosy1 = lpsScreenArea->miGfxClipPosy1;
    if (liGfxPosx2 > lpsScreenArea->miGfxClipPosx2) liGfxPosx2 = lpsScreenArea->miGfxClipPosx2;
    if (liGfxPosy2 > lpsScreenArea->miGfxClipPosy2) liGfxPosy2 = lpsScreenArea->miGfxClipPosy2;

    miGfxClipPosx1 = liGfxPosx1;
    miGfxClipPosy1 = liGfxPosy1;
    miGfxClipPosx2 = liGfxPosx2;
    miGfxClipPosy2 = liGfxPosy2;

    mbVisible = lpsScreenArea->mbVisible;
  }


  SCREEN_INLINE void vSort(GfxInt &liGfxV1, GfxInt &liGfxV2)
  {
    GfxInt liGfxV;
    if (liGfxV1 > liGfxV2)
    {
      liGfxV = liGfxV2;
      liGfxV2 = liGfxV1;
      liGfxV1 = liGfxV;
    }
  }

  SCREEN_INLINE void vSort(GfxInt &liGfxV1, uint32 &lu32Col1, GfxInt &liGfxV2, uint32 &lu32Col2)
  {
    GfxInt liGfxV;
    uint32 lu32Col;
    if (liGfxV1 > liGfxV2)
    {
      liGfxV = liGfxV2;
      liGfxV2 = liGfxV1;
      liGfxV1 = liGfxV;

      lu32Col  = lu32Col2;
      lu32Col2 = lu32Col1;
      lu32Col1 = lu32Col;
    }
  }

  SCREEN_INLINE void vSort(GfxInt &liGfxV1, uint8 &lu8Br1, GfxInt &liGfxV2, uint8 &lu8Br2)
  {
    GfxInt liGfxV;
    uint8  lu8Br;
    if (liGfxV1 > liGfxV2)
    {
      liGfxV = liGfxV2;
      liGfxV2 = liGfxV1;
      liGfxV1 = liGfxV;

      lu8Br  = lu8Br2;
      lu8Br2 = lu8Br1;
      lu8Br1 = lu8Br;
    }
  }


  SCREEN_INLINE void vRel2AbsX(GfxInt &liGfxX) { liGfxX += miGfxRefPosx1; }
  SCREEN_INLINE void vRel2AbsY(GfxInt &liGfxY) { liGfxY += miGfxRefPosy1; }
  SCREEN_INLINE void vRel2Abs(GfxInt &liGfxX, GfxInt &liGfxY) { vRel2AbsX(liGfxX); vRel2AbsY(liGfxY); }
  SCREEN_INLINE void vRel2Abs(GfxInt &liGfxX1, GfxInt &liGfxY1,
                              GfxInt &liGfxX2, GfxInt &liGfxY2) {
    vRel2Abs(liGfxX1, liGfxY1); vRel2Abs(liGfxX2, liGfxY2);
  }

  SCREEN_INLINE bool isInX1Abs(GfxInt liGfxX) { return ((mbVisible) && (liGfxX >= miGfxClipPosx1)); }
  SCREEN_INLINE bool isInX2Abs(GfxInt liGfxX) { return ((mbVisible) && (liGfxX <= miGfxClipPosx2)); }
  SCREEN_INLINE bool isInY1Abs(GfxInt liGfxY) { return ((mbVisible) && (liGfxY >= miGfxClipPosy1)); }
  SCREEN_INLINE bool isInY2Abs(GfxInt liGfxY) { return ((mbVisible) && (liGfxY <= miGfxClipPosy2)); }

  SCREEN_INLINE bool isInXAbs(GfxInt liGfxX) { return (isInX1Abs(liGfxX) && isInX2Abs(liGfxX)); }
  SCREEN_INLINE bool isInYAbs(GfxInt liGfxY) { return (isInY1Abs(liGfxY) && isInY2Abs(liGfxY)); }
  SCREEN_INLINE bool isInAbs(GfxInt liGfxX, GfxInt liGfxY) { return (isInXAbs(liGfxX) && isInYAbs(liGfxY)); }
  SCREEN_INLINE bool isInAbs(GfxInt liGfxX1, GfxInt liGfxY1,
                             GfxInt liGfxX2, GfxInt liGfxY2) { return ((mbVisible) &&
                                                                       (liGfxX1 <= miGfxClipPosx2) && (liGfxY1 <= miGfxClipPosy2) &&
                                                                       (liGfxX2 >= miGfxClipPosx1) && (liGfxY2 >= miGfxClipPosy1));}
  SCREEN_INLINE bool isInBoxXYAbs(GfxInt liGfxX1, GfxInt liGfxY1,
                                  GfxInt liGfxX2, GfxInt liGfxY2) {return (isInAbs(liGfxX1, liGfxY1, liGfxX2, liGfxY2));}
  SCREEN_INLINE bool isInBoxWHAbs(GfxInt liGfxX, GfxInt liGfxY,
                                  GfxInt liGfxW, GfxInt liGfxH) {return (isInAbs(liGfxX, liGfxY, liGfxX + liGfxW - 1, liGfxY + liGfxH - 1));}

  SCREEN_INLINE void vClipX1Abs(GfxInt &liGfxX) { if (liGfxX < miGfxClipPosx1) liGfxX = miGfxClipPosx1; }
  SCREEN_INLINE void vClipX2Abs(GfxInt &liGfxX) { if (liGfxX > miGfxClipPosx2) liGfxX = miGfxClipPosx2; }
  SCREEN_INLINE void vClipY1Abs(GfxInt &liGfxY) { if (liGfxY < miGfxClipPosy1) liGfxY = miGfxClipPosy1; }
  SCREEN_INLINE void vClipY2Abs(GfxInt &liGfxY) { if (liGfxY > miGfxClipPosy2) liGfxY = miGfxClipPosy2; }




  SCREEN_INLINE bool isInX1Rel(GfxInt liGfxX) { return ((mbVisible) && ((liGfxX + miGfxClipPosx1) >= miGfxClipPosx1)); }
  SCREEN_INLINE bool isInX2Rel(GfxInt liGfxX) { return ((mbVisible) && ((liGfxX + miGfxClipPosx1) <= miGfxClipPosx2)); }
  SCREEN_INLINE bool isInY1Rel(GfxInt liGfxY) { return ((mbVisible) && ((liGfxY + miGfxClipPosy1) >= miGfxClipPosy1)); }
  SCREEN_INLINE bool isInY2Rel(GfxInt liGfxY) { return ((mbVisible) && ((liGfxY + miGfxClipPosy1) <= miGfxClipPosy2)); }

  SCREEN_INLINE bool isInXRel(GfxInt liGfxX) { return (isInX1Abs(liGfxX) && isInX2Abs(liGfxX)); }
  SCREEN_INLINE bool isInYRel(GfxInt liGfxY) { return (isInY1Abs(liGfxY) && isInY2Abs(liGfxY)); }
  SCREEN_INLINE bool isInRel(GfxInt liGfxX, GfxInt liGfxY) { return (isInXAbs(liGfxX) && isInYAbs(liGfxY)); }
  SCREEN_INLINE bool isInRel(GfxInt liGfxX1, GfxInt liGfxY1,
                             GfxInt liGfxX2, GfxInt liGfxY2) { return ((mbVisible) &&
                                                                       ((liGfxX1 + miGfxClipPosx1) <= miGfxClipPosx2) && ((liGfxY1 + miGfxClipPosy1) <= miGfxClipPosy2) &&
                                                                       ((liGfxX2 + miGfxClipPosx1) >= miGfxClipPosx1) && ((liGfxY2 + miGfxClipPosy1) >= miGfxClipPosy1));}
  SCREEN_INLINE bool isInBoxXYRel(GfxInt liGfxX1, GfxInt liGfxY1,
                                  GfxInt liGfxX2, GfxInt liGfxY2) {return (isInRel(liGfxX1, liGfxY1, liGfxX2, liGfxY2));}
  SCREEN_INLINE bool isInBoxWHRel(GfxInt liGfxX, GfxInt liGfxY,
                                  GfxInt liGfxW, GfxInt liGfxH) {return (isInRel(liGfxX, liGfxY, liGfxX + liGfxW - 1, liGfxY + liGfxH - 1));}

  SCREEN_INLINE void vClipX1Rel(GfxInt &liGfxX) { if ((liGfxX + miGfxClipPosx1) < miGfxClipPosx1) liGfxX = 0; }
  SCREEN_INLINE void vClipX2Rel(GfxInt &liGfxX) { if ((liGfxX + miGfxClipPosx1) > miGfxClipPosx2) liGfxX = iGfxWidth(); }
  SCREEN_INLINE void vClipY1Rel(GfxInt &liGfxY) { if ((liGfxY + miGfxClipPosy1) < miGfxClipPosy1) liGfxY = 0; }
  SCREEN_INLINE void vClipY2Rel(GfxInt &liGfxY) { if ((liGfxY + miGfxClipPosy1) > miGfxClipPosy2) liGfxY = iGfxHeight(); }


  SCREEN_INLINE GfxInt iGfxWidth()  { return (miGfxClipPosx2 - miGfxClipPosx1 + 1); }
  SCREEN_INLINE GfxInt iGfxHeight() { return (miGfxClipPosy2 - miGfxClipPosy1 + 1); }

  SCREEN_INLINE bool bHLineClipAbs(GfxInt &liGfxX1, GfxInt &liGfxX2, GfxInt liGfxY)
  {
    if (!isInYAbs(liGfxY)) return False;

    vSort(liGfxX1, liGfxX2);

    if (!isInX1Abs(liGfxX2)) return False;
    if (!isInX2Abs(liGfxX1)) return False;

    vClipX1Abs(liGfxX1);
    vClipX2Abs(liGfxX2);

    return True;
  }


  SCREEN_INLINE bool bVLineClipAbs(GfxInt &liGfxY1, GfxInt &liGfxY2, GfxInt liGfxX)
  {
    if (!isInXAbs(liGfxX)) return False;

    vSort(liGfxY1, liGfxY2);

    if (!isInY1Abs(liGfxY2)) return False;
    if (!isInY2Abs(liGfxY1)) return False;

    vClipY1Abs(liGfxY1);
    vClipY2Abs(liGfxY2);

    return True;
  }


  //{Linienclipping mit Cohen - Sutherland} 
  bool bLineClipYAbs(GfxInt &liGfxLCX1, GfxInt &liGfxLCY1,
                     GfxInt &liGfxLCX2, GfxInt &liGfxLCY2)
  {
    bool   lbDone;
    bool   lbLineClip;
    uint8  li8PCode,
      li8PCode1,
      li8PCode2;
    GfxInt liGfxLCX = 0,
      liGfxLCY = 0;
    GfxInt liGfxLCD;

    lbLineClip = False;
    lbDone = False;
    li8PCode1 = 0;
    li8PCode2 = 0;

    //{BereichCodes}

    if (liGfxLCY1 < miGfxClipPosy1) li8PCode1 = li8PCode1 | 8;
    else if (liGfxLCY1 > miGfxClipPosy2) li8PCode1 = li8PCode1 | 4;
    if (liGfxLCY2 < miGfxClipPosy1) li8PCode2 = li8PCode2 | 8;
    else if (liGfxLCY2 > miGfxClipPosy2) li8PCode2 = li8PCode2 | 4;

    /*
    1001 | 1000 | 1010
    -----+------+-----
    0001 | 0000 | 0010
    -----+------+-----
    0101 | 0100 | 0110
    */

    do
    {
      if ((li8PCode1 == 0) && (li8PCode2 == 0))
      {
        lbDone = True;
        lbLineClip = True;
      }
      else if ((li8PCode1 & li8PCode2) != 0)
      {
        lbLineClip = False;
        lbDone = True;
      }
      else
      {
        if (li8PCode1 != 0) li8PCode = li8PCode1;
        else li8PCode = li8PCode2;

        if ((li8PCode & 8) == 8)
        {
          // {ObenClip}
          liGfxLCD = liGfxLCX2 - liGfxLCX1;
          liGfxLCD = liGfxLCD * (miGfxClipPosy1 - liGfxLCY1);
          liGfxLCD = liGfxLCD / (liGfxLCY2 - liGfxLCY1);
          liGfxLCY = miGfxClipPosy1;
          liGfxLCX = liGfxLCX1 + liGfxLCD;
        }
        else if ((li8PCode & 4) == 4)
        {
          //{UntenClip}
          liGfxLCD = (liGfxLCX2 - liGfxLCX1);
          liGfxLCD = liGfxLCD * (miGfxClipPosy2 - liGfxLCY1);
          liGfxLCD = liGfxLCD / (liGfxLCY2 - liGfxLCY1);
          liGfxLCY = miGfxClipPosy2;
          liGfxLCX = liGfxLCX1 + liGfxLCD;
        }

        if (li8PCode == li8PCode1)
        {
          li8PCode1 = 0;
          liGfxLCX1 = liGfxLCX;
          liGfxLCY1 = liGfxLCY;
          if (liGfxLCY1 < miGfxClipPosy1) li8PCode1 = li8PCode1 | 8;
          else if (liGfxLCY1 > miGfxClipPosy2) li8PCode1 = li8PCode1 | 4;
        }
        else
        {
          li8PCode2 = 0;
          liGfxLCX2 = liGfxLCX;
          liGfxLCY2 = liGfxLCY;
          if (liGfxLCY2 < miGfxClipPosy1) li8PCode2 = li8PCode2 | 8;
          else if (liGfxLCY2 > miGfxClipPosy2) li8PCode2 = li8PCode2 | 4;
        }
      }
    } while (!lbDone);

    return lbLineClip;
  }

  //{Linienclipping mit Cohen - Sutherland} 
  bool bLineClipAbs(GfxInt &liGfxLCX1, GfxInt &liGfxLCY1,
                    GfxInt &liGfxLCX2, GfxInt &liGfxLCY2)
  {
    bool   lbDone;
    bool   lbLineClip;
    uint8  li8PCode,
           li8PCode1,
           li8PCode2;
    GfxInt liGfxLCX = 0,
           liGfxLCY = 0;
    GfxInt liGfxLCD;

    lbLineClip = False;
    lbDone     = False;
    li8PCode1  = 0;
    li8PCode2  = 0;

    //{BereichCodes}
    if (liGfxLCX1 < miGfxClipPosx1)      li8PCode1 = li8PCode1 | 1;
    else if (liGfxLCX1 > miGfxClipPosx2) li8PCode1 = li8PCode1 | 2;
    if (liGfxLCY1 < miGfxClipPosy1)      li8PCode1 = li8PCode1 | 8;
    else if (liGfxLCY1 > miGfxClipPosy2) li8PCode1 = li8PCode1 | 4;

    if (liGfxLCX2 < miGfxClipPosx1)      li8PCode2 = li8PCode2 | 1;
    else if (liGfxLCX2 > miGfxClipPosx2) li8PCode2 = li8PCode2 | 2;
    if (liGfxLCY2 < miGfxClipPosy1)      li8PCode2 = li8PCode2 | 8;
    else if (liGfxLCY2 > miGfxClipPosy2) li8PCode2 = li8PCode2 | 4;

    /*
    1001 | 1000 | 1010
    -----+------+-----
    0001 | 0000 | 0010
    -----+------+-----
    0101 | 0100 | 0110
    */

    do
    {
      if ((li8PCode1 == 0) && (li8PCode2 == 0))
      {
        lbDone = True;
        lbLineClip = True;
      }
      else if ((li8PCode1 & li8PCode2) != 0)
      {
        lbLineClip = False;
        lbDone = True;
      }
      else
      {
        if (li8PCode1 != 0) li8PCode = li8PCode1;
        else li8PCode = li8PCode2;

        if ((li8PCode & 8) == 8)
        {
          // {ObenClip}
          liGfxLCD = liGfxLCX2 - liGfxLCX1;
          liGfxLCD = liGfxLCD * (miGfxClipPosy1 - liGfxLCY1);
          liGfxLCD = liGfxLCD / (liGfxLCY2 - liGfxLCY1);
          liGfxLCY = miGfxClipPosy1;
          liGfxLCX = liGfxLCX1 + liGfxLCD;
        }
        else if ((li8PCode & 4) == 4)
        {
          //{UntenClip}
          liGfxLCD = (liGfxLCX2 - liGfxLCX1);
          liGfxLCD = liGfxLCD * (miGfxClipPosy2 - liGfxLCY1);
          liGfxLCD = liGfxLCD / (liGfxLCY2 - liGfxLCY1);
          liGfxLCY = miGfxClipPosy2;
          liGfxLCX = liGfxLCX1 + liGfxLCD;
        }
        else if ((li8PCode & 2) == 2)
        {
          //{RechtsClip}
          liGfxLCD = (miGfxClipPosx2 - liGfxLCX1);
          liGfxLCD = liGfxLCD * (liGfxLCY2 - liGfxLCY1);
          liGfxLCD = liGfxLCD / (liGfxLCX2 - liGfxLCX1);
          liGfxLCX = miGfxClipPosx2;
          liGfxLCY = liGfxLCY1 + liGfxLCD;
        }
        else if ((li8PCode & 1) == 1)
        {
          //{LinksClip}
          liGfxLCD = miGfxClipPosx1 - liGfxLCX1;
          liGfxLCD = liGfxLCD * (liGfxLCY2 - liGfxLCY1);
          liGfxLCD = liGfxLCD / (liGfxLCX2 - liGfxLCX1);
          liGfxLCX = miGfxClipPosx1;
          liGfxLCY = liGfxLCY1 + liGfxLCD;
        }
        if (li8PCode == li8PCode1)
        {
          li8PCode1 = 0;
          liGfxLCX1 = liGfxLCX;
          liGfxLCY1 = liGfxLCY;
          if (liGfxLCX1 < miGfxClipPosx1) li8PCode1 = li8PCode1 | 1;
          else if (liGfxLCX1 > miGfxClipPosx2) li8PCode1 = li8PCode1 | 2;
          if (liGfxLCY1 < miGfxClipPosy1) li8PCode1 = li8PCode1 | 8;
          else if (liGfxLCY1 > miGfxClipPosy2) li8PCode1 = li8PCode1 | 4;
        }
        else
        {
          li8PCode2 = 0;
          liGfxLCX2 = liGfxLCX;
          liGfxLCY2 = liGfxLCY;
          if (liGfxLCY2 < miGfxClipPosy1) li8PCode2 = li8PCode2 | 8;
          else if (liGfxLCY2 > miGfxClipPosy2) li8PCode2 = li8PCode2 | 4;
          if (liGfxLCX2 < miGfxClipPosx1) li8PCode2 = li8PCode2 | 1;
          else if (liGfxLCX2 > miGfxClipPosx2) li8PCode2 = li8PCode2 | 2;
        }
      }
    } while (!lbDone);

    return lbLineClip;
  }


  SCREEN_INLINE bool bBoxClipAbs(GfxInt &liGfxLCX1, GfxInt &liGfxLCY1,
                                 GfxInt &liGfxLCX2, GfxInt &liGfxLCY2)
  {
    if (!mbVisible)
    {
      return False;
    }

    vSort(liGfxLCX1, liGfxLCX2);
    vSort(liGfxLCY1, liGfxLCY2);

    if (isInAbs(liGfxLCX1, liGfxLCY1, liGfxLCX2, liGfxLCY2))
    {
      vClipX1Abs(liGfxLCX1);
      vClipX2Abs(liGfxLCX2);
      vClipY1Abs(liGfxLCY1);
      vClipY2Abs(liGfxLCY2);
      return True;
    }

    return False;
  }
};

/* --------------------------------------------------------------------------------- */

class cScreen : public cScreenArea
{
  public:
    cBitmap *mpcBm;

    cScreen(cBitmap* lcBm)
    {
      mpcBm = lcBm;
      vSet(0, 0, mpcBm->miGfxWidth - 1, mpcBm->miGfxHeight - 1);
    }

    cScreen(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cBitmap* lcBm)
    {
      mpcBm = lcBm;
      vSet(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2);
    }

    cScreen(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cScreen* lcScreen)
    {
      vSet(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2, lcScreen);
    }

    ~cScreen()
    {
    }

    void vMoveArea(GfxInt liGfxPosx, GfxInt liGfxPosy)
    {
      miGfxRefPosx1 += liGfxPosx;
      miGfxRefPosy1 += liGfxPosy;

      miGfxClipPosx1 += liGfxPosx;
      miGfxClipPosy1 += liGfxPosy;
      miGfxClipPosx2 += liGfxPosx;
      miGfxClipPosy2 += liGfxPosy;

      mbVisible = True;


      if ((miGfxClipPosx2 < 0) ||
          (miGfxClipPosy2 < 0) ||
          (miGfxClipPosx1 >= mpcBm->miGfxWidth) ||
          (miGfxClipPosy1 >= mpcBm->miGfxHeight))
      {
        mbVisible = False;
      }

      if (miGfxClipPosx1 < 0) miGfxClipPosx1 = 0;
      if (miGfxClipPosy1 < 0) miGfxClipPosy1 = 0;
      if (miGfxClipPosx2 >= mpcBm->miGfxWidth)  miGfxClipPosx2 = mpcBm->miGfxWidth - 1;
      if (miGfxClipPosy2 >= mpcBm->miGfxHeight) miGfxClipPosy2 = mpcBm->miGfxHeight - 1;
    }

    void vSet(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2, cScreen* lcScreen)
    {
      mpcBm = lcScreen->mpcBm;
      vRel2Abs(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2);

      miGfxRefPosx1 = liGfxPosx1;
      miGfxRefPosy1 = liGfxPosy1;

      if (liGfxPosx1 < lcScreen->miGfxClipPosx1) liGfxPosx1 = lcScreen->miGfxClipPosx1;
      if (liGfxPosy1 < lcScreen->miGfxClipPosy1) liGfxPosy1 = lcScreen->miGfxClipPosy1;
      if (liGfxPosx2 > lcScreen->miGfxClipPosx2) liGfxPosx2 = lcScreen->miGfxClipPosx2;
      if (liGfxPosy2 > lcScreen->miGfxClipPosy2) liGfxPosy2 = lcScreen->miGfxClipPosy2;

      vSet(liGfxPosx1, liGfxPosy1, liGfxPosx2, liGfxPosy2);
    }

    void vSet(GfxInt liGfxPosx1, GfxInt liGfxPosy1, GfxInt liGfxPosx2, GfxInt liGfxPosy2)
    {
      mbVisible = True;

      if ((liGfxPosx2 <  0) || 
          (liGfxPosy2 <  0) || 
          (liGfxPosx1 >= mpcBm->miGfxWidth) ||
          (liGfxPosy1 >= mpcBm->miGfxHeight))
      {
        mbVisible = False;
      }

      if (liGfxPosx1 <  0) liGfxPosx1 = 0;
      if (liGfxPosy1 <  0) liGfxPosy1 = 0;
      if (liGfxPosx2 >= mpcBm->miGfxWidth)  liGfxPosx2 = mpcBm->miGfxWidth  - 1;
      if (liGfxPosy2 >= mpcBm->miGfxHeight) liGfxPosy2 = mpcBm->miGfxHeight - 1;

      miGfxClipPosx1 = liGfxPosx1;
      miGfxClipPosy1 = liGfxPosy1;
      miGfxClipPosx2 = liGfxPosx2;
      miGfxClipPosy2 = liGfxPosy2;
    }


    virtual /*SCREEN_INLINE*/ u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  = 0;
    virtual /*SCREEN_INLINE*/ u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) = 0;
    virtual /*SCREEN_INLINE*/ void vFill(u32 lui32Col) = 0;

    virtual /*SCREEN_INLINE*/ void vPpAbsRaw(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col) = 0;
    SCREEN_INLINE void vPpAbs(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col)
    {
      if (isInAbs(liGfxX, liGfxY))
      {
        vPpAbsRaw(liGfxX, liGfxY, lui32Col);
      }
    }
    SCREEN_INLINE void vPp(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col)
    {
      vRel2Abs(liGfxX, liGfxY);
      vPpAbs(liGfxX, liGfxY, lui32Col);
    }

    virtual /*SCREEN_INLINE*/ u32 u32GpAbsRaw(GfxInt liGfxX, GfxInt liGfxY) = 0;
    SCREEN_INLINE u32 u32GpAbs(GfxInt liGfxX, GfxInt liGfxY)
    {
      if (isInAbs(liGfxX, liGfxY))
      {
        return u32GpAbsRaw(liGfxX, liGfxY);
      }
      return 0;
    }
    SCREEN_INLINE u32 u32Gp(GfxInt liGfxX, GfxInt liGfxY)
    {
      vRel2Abs(liGfxX, liGfxY);
      return u32GpAbs(liGfxX, liGfxY);
    }

    virtual /*SCREEN_INLINE*/ u32  u32GetCol(u32 lui32Col) = 0;

    // Zeichnet eine horizontale Linie
    virtual void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col) = 0;
    // Zeichnet eine horizontale Linie mit Farbgradient
    virtual void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col, uint32 lui32Col2) = 0;

    SCREEN_INLINE void vLineHAbs(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        if (bHLineClipAbs(liGfxX1, liGfxX2, liGfxY1))
        {
          vLineHAbsRaw(liGfxX1, liGfxX2, liGfxY1, lui32Col);
        }
      }
    }

    void vLineHAbs(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col1, uint32 lui32Col2)
    {
      //if (bChkInit())
      {
        vSort(liGfxX1, lui32Col1, liGfxX2, lui32Col2);

        GfxInt liGfxX1_Save = liGfxX1;
        GfxInt liGfxX2_Save = liGfxX2;

        if (bHLineClipAbs(liGfxX1, liGfxX2, liGfxY1))
        {
          if ((liGfxX1_Save != liGfxX1) || (liGfxX2_Save != liGfxX2))
          {
            u8 r1, g1, b1;
            u8 r2, g2, b2;

            i32 r_inc, g_inc, b_inc, temp;

            BM_BPP32_8R8G8B_GET_RGB(lui32Col1, r1, g1, b1);
            BM_BPP32_8R8G8B_GET_RGB(lui32Col2, r2, g2, b2);

            r_inc = ((r2 - r1) * 65536) / ((liGfxX2_Save - liGfxX1_Save) + 1);
            g_inc = ((g2 - g1) * 65536) / ((liGfxX2_Save - liGfxX1_Save) + 1);
            b_inc = ((b2 - b1) * 65536) / ((liGfxX2_Save - liGfxX1_Save) + 1);

            if (liGfxX1_Save != liGfxX1)
            {
              GfxInt liGfxPos = liGfxX1 - liGfxX1_Save;

              temp = liGfxPos * r_inc;
              r1 = r1 + (temp >> 16);
              if (temp & 0x8000) r1++;

              temp = liGfxPos * g_inc;
              g1 = g1 + (temp >> 16);
              if (temp & 0x8000) g1++;

              temp = liGfxPos * b_inc;
              b1 = b1 + (temp >> 16);
              if (temp & 0x8000) b1++;

              lui32Col1 = BM_BPP32_8R8G8B_RGB(r1, g1, b1);
            }

            if (liGfxX2_Save != liGfxX2)
            {
              GfxInt liGfxPos = liGfxX2 - liGfxX1_Save;

              temp = liGfxPos * r_inc;
              r2 = r1 + (temp >> 16);
              if (temp & 0x8000) r2++;

              temp = liGfxPos * g_inc;
              g2 = g1 + (temp >> 16);
              if (temp & 0x8000) g2++;

              temp = liGfxPos * b_inc;
              b2 = b1 + (temp >> 16);
              if (temp & 0x8000) b2++;

              lui32Col2 = BM_BPP32_8R8G8B_RGB(r2, g2, b2);
            }
          }

          vLineHAbsRaw(liGfxX1, liGfxX2, liGfxY1, lui32Col1, lui32Col2);
        }
      }
    }

    SCREEN_INLINE void vLineHAbsDashed(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col, GfxInt liGfxDistance)
    {
      //if (bChkInit())
      {
        if (bHLineClipAbs(liGfxX1, liGfxX2, liGfxY1))
        {
          while (liGfxX1 < liGfxX2)
          {
            if ((liGfxX1 + liGfxDistance) <= liGfxX2)
            {
              vLineHAbsRaw(liGfxX1, liGfxX1 + liGfxDistance, liGfxY1, lui32Col);
            }
            else
            {
              vLineHAbsRaw(liGfxX1, liGfxX2, liGfxY1, lui32Col);
            }

            liGfxX1 += (2 * liGfxDistance) + 1;
          }
        }
      }
    }

    SCREEN_INLINE void vLineH(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col)
    {
      vRel2AbsX(liGfxX1);
      vRel2AbsX(liGfxX2);
      vRel2AbsY(liGfxY1);

      vLineHAbs(liGfxX1, liGfxX2, liGfxY1, lui32Col);
    }

    SCREEN_INLINE void vLineH(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col1, uint32 lui32Col2)
    {
      vRel2AbsX(liGfxX1);
      vRel2AbsX(liGfxX2);
      vRel2AbsY(liGfxY1);

      vLineHAbs(liGfxX1, liGfxX2, liGfxY1, lui32Col1, lui32Col2);
    }

    SCREEN_INLINE void vLineHDashed(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col, GfxInt liGfxDistance)
    {
      vRel2AbsX(liGfxX1);
      vRel2AbsX(liGfxX2);
      vRel2AbsY(liGfxY1);

      vLineHAbsDashed(liGfxX1, liGfxX2, liGfxY1, lui32Col, liGfxDistance);
    }

    // Zeichnet eine vertikale Linie
    virtual void vLineVAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col) = 0;
    SCREEN_INLINE void vLineVAbs(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        if (bVLineClipAbs(liGfxY1, liGfxY2, liGfxX1))
        {
          vLineVAbsRaw(liGfxX1, liGfxY1, liGfxY2, lui32Col);
        }
      }
    }

    SCREEN_INLINE void vLineVAbsDashed(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col, GfxInt liGfxDistance)
    {
      //if (bChkInit())
      {
        if (bVLineClipAbs(liGfxY1, liGfxY2, liGfxX1))
        {
          while (liGfxY1 < liGfxY2)
          {
            if ((liGfxY1 + liGfxDistance) <= liGfxY2)
            {
              vLineVAbsRaw(liGfxX1, liGfxY1, liGfxY1 + liGfxDistance, lui32Col);
            }
            else
            {
              vLineVAbsRaw(liGfxX1, liGfxY1, liGfxY2, lui32Col);
            }

            liGfxY1 += (2 * liGfxDistance) + 1;
          }
        }
      }
    }

    SCREEN_INLINE void vLineV(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col)
    {
      vRel2AbsX(liGfxX1);
      vRel2AbsY(liGfxY1);
      vRel2AbsY(liGfxY2);

      vLineVAbs(liGfxX1, liGfxY1, liGfxY2, lui32Col);
    }

    SCREEN_INLINE void vLineVDashed(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col, GfxInt liGfxDistance)
    {
      vRel2AbsX(liGfxX1);
      vRel2AbsY(liGfxY1);
      vRel2AbsY(liGfxY2);

      vLineVAbsDashed(liGfxX1, liGfxY1, liGfxY2, lui32Col, liGfxDistance);
    }

    // Zeichnet eine Linie auf 
    virtual void vLineAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxX2, GfxInt liGfxY2, uint32 lui32Col) = 0; 
    SCREEN_INLINE void vLineAbs(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxX2, GfxInt liGfxY2, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        if (liGfxX1 == liGfxX2)
        {
          vLineVAbs(liGfxX1, liGfxY1, liGfxY2, lui32Col);
        }
        else if (liGfxY1 == liGfxY2)
        {
          vLineHAbs(liGfxX1, liGfxX2, liGfxY1, lui32Col);
        }
        else
        {
          if (bLineClipAbs(liGfxX1, liGfxY1, liGfxX2, liGfxY2))
          {
            vLineAbsRaw(liGfxX1, liGfxY1, liGfxX2, liGfxY2, lui32Col);
          }
        }
      }
    }

    SCREEN_INLINE void vLine(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxX2, GfxInt liGfxY2, uint32 lui32Col)
    {
      if (liGfxX1 == liGfxX2)
      {
        vLineV(liGfxX1, liGfxY1, liGfxY2, lui32Col);
      }
      else if (liGfxY1 == liGfxY2)
      {
        vLineH(liGfxX1, liGfxX2, liGfxY1, lui32Col);
      }
      else
      {
        vRel2Abs(liGfxX1, liGfxY1, liGfxX2, liGfxY2);
        if (bLineClipAbs(liGfxX1, liGfxY1, liGfxX2, liGfxY2))
        {
          vLineAbsRaw(liGfxX1, liGfxY1, liGfxX2, liGfxY2, lui32Col);
        }
      }
    }
};

#ifdef __cplusplus
}
#endif

#endif  //SCREEN_BASE
