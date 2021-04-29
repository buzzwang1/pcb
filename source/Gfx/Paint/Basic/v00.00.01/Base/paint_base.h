#ifndef _PAINT_BASE_H__
#define _PAINT_BASE_H__

#include "screen.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cPaint
{
  public:

  // Zeichnet ein Viereck auf Schirm
  static void vRect(GfxInt liGfxx, GfxInt liGfxy, GfxInt liGfxWidth, GfxInt liGfxHeigth, uint32 lui32Col, cScreen* lcScr)
  {
    GfxInt liGfxx2;
    GfxInt liGfxy2;

    liGfxx2 = liGfxx + liGfxWidth  - 1;
    liGfxy2 = liGfxy + liGfxHeigth - 1;

    lcScr->vRel2Abs(liGfxx, liGfxy, liGfxx2, liGfxy2);

    if (lcScr->bBoxClipAbs(liGfxx, liGfxy, liGfxx2, liGfxy2))
    {
      lcScr->vLineHAbs(liGfxx,  liGfxx2, liGfxy,  lui32Col);
      lcScr->vLineHAbs(liGfxx,  liGfxx2, liGfxy2, lui32Col);
      lcScr->vLineVAbs(liGfxx,  liGfxy,  liGfxy2, lui32Col);
      lcScr->vLineVAbs(liGfxx2, liGfxy,  liGfxy2, lui32Col);
    }
  }

  // Zeichnet ein gestricheltes Viereck auf Schirm
  static void vRectDashed(GfxInt liGfxx, GfxInt liGfxy, GfxInt liGfxWidth, GfxInt liGfxHeigth, uint32 lui32Col, GfxInt liGfxDistance, cScreen* lcScr)
  {
    GfxInt liGfxx2;
    GfxInt liGfxy2;

    liGfxx2 = liGfxx + liGfxWidth  - 1;
    liGfxy2 = liGfxy + liGfxHeigth - 1;

    lcScr->vRel2Abs(liGfxx, liGfxy, liGfxx2, liGfxy2);

    if (lcScr->bBoxClipAbs(liGfxx, liGfxy, liGfxx2, liGfxy2))
    {
      lcScr->vLineHAbsDashed(liGfxx,  liGfxx2, liGfxy,  lui32Col, liGfxDistance);
      lcScr->vLineHAbsDashed(liGfxx,  liGfxx2, liGfxy2, lui32Col, liGfxDistance);
      lcScr->vLineVAbsDashed(liGfxx,  liGfxy,  liGfxy2, lui32Col, liGfxDistance);
      lcScr->vLineVAbsDashed(liGfxx2, liGfxy,  liGfxy2, lui32Col, liGfxDistance);
    }
  }

  // Zeichnet ein ausgefülltet Viereck auf Schirm
  static void vRectFull(GfxInt liGfxx, GfxInt liGfxy, GfxInt liGfxWidth, GfxInt liGfxHeigth, uint32 lui32Col, cScreen* lcScr)
  {
    GfxInt liGfxi;
    GfxInt liGfxx2;
    GfxInt liGfxy2;

    liGfxx2 = liGfxx + liGfxWidth  - 1;
    liGfxy2 = liGfxy + liGfxHeigth - 1;

    lcScr->vRel2Abs(liGfxx, liGfxy, liGfxx2, liGfxy2);

    if (lcScr->bBoxClipAbs(liGfxx, liGfxy, liGfxx2, liGfxy2))
    {
      for (liGfxi = liGfxy; liGfxi <= liGfxy2; liGfxi++)
      {
        lcScr->vLineHAbsRaw(liGfxx, liGfxx2, liGfxi, lui32Col);
      }
    }
  }
};


#ifdef __cplusplus
}
#endif


#endif // _PAINT_BASE_H__

