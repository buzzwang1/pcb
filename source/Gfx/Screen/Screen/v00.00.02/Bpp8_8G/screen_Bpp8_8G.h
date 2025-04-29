#ifndef __SCREEN_BPP8_8G_H__
#define __SCREEN_BPP8_8G_H__

/* Includes ------------------------------------------------------------------*/
#include "screen_base.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if SCREEN_BPP16_5R6G5B_DIRECT
  #define SCREEN_BPP8_8G_PP(liGfxX, liGfxY, lcBm, lu16Col) SCREEN_BPP8_8G_PXL((liGfxX), (liGfxY), (*lcBm)) = lu16Col
  #define SCREEN_BPP8_8G_GP(liGfxX, liGfxY, lcBm)          SCREEN_BPP8_8G_PXL((liGfxX), (liGfxY), (*lcBm))
#else
  #define SCREEN_BPP8_8G_PP(liGfxX, liGfxY, lcBm, lu16Col)   ((cBitmap_Bpp8_8G*)lcBm)->vPp(liGfxX, liGfxY, lu16Col)
  #define SCREEN_BPP8_8G_GP(liGfxX, liGfxY, lcBm)            ((cBitmap_Bpp8_8G*)lcBm)->u8Gp(liGfxX, liGfxY)
#endif

class cScreen_Bpp8_8G : public cScreen
{
  public:
    cScreen_Bpp8_8G(cBitmap* lcBm)
      : cScreen(lcBm) {};
    cScreen_Bpp8_8G(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cBitmap* lcBm)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcBm) {};
    cScreen_Bpp8_8G(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cScreen* lcScreen)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcScreen) {};
    ~cScreen_Bpp8_8G() {};

    SCREEN_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx);};
    SCREEN_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx);};

    SCREEN_INLINE void vFill(u32 lui32Col)
    {
      GfxInt liGfxY;
      GfxInt liGfxWidth;

      lui32Col = u32GetCol(lui32Col);

      u8* lpui8Data = (u8*)&mpcBm->mpui8Data[u32PxlIdx2ByteIdx(mpcBm->u32PxlIdx(miGfxClipPosx1, miGfxClipPosy1))];

      liGfxWidth  = miGfxClipPosx2 - miGfxClipPosx1 + 1;

      for (liGfxY = miGfxClipPosy1; liGfxY <= miGfxClipPosy2; liGfxY++)
      {
        cMemTools::vMemSet(lpui8Data, lui32Col, liGfxWidth);
        lpui8Data += mpcBm->miGfxWidth;
      }
    }

    SCREEN_INLINE void vPpAbsRaw(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col)
    {
      lui32Col = u32GetCol(lui32Col);
      SCREEN_BPP8_8G_PP(liGfxX, liGfxY, mpcBm, lui32Col);
    }

    SCREEN_INLINE u32 u32GpAbsRaw(GfxInt liGfxX, GfxInt liGfxY)
    {
      u8 lu8Col;

      lu8Col = SCREEN_BPP8_8G_GP(liGfxX, liGfxY, mpcBm);

      if (!lu8Col)
      {
        if (lu8Col < 0xFF)
        {
          return BM_BPP32_8R8G8B_RGB(lu8Col, lu8Col, lu8Col);
        }
        else
        {
          return BM_BPP32_8R8G8B_RGB(0xFF, 0xFF, 0xFF);
        }
      }

      return 0;
    }

    SCREEN_INLINE u32 u32GetCol(u32 lui32Col)
    {
      if (lui32Col)
      {
        if (lui32Col < BM_BPP32_8R8G8B_RGB(0xFF, 0xFF, 0xFF))
        {
          u8 r, g, b;
          r = BM_BPP32_8R8G8B_GET_R(lui32Col);
          g = BM_BPP32_8R8G8B_GET_G(lui32Col);
          b = BM_BPP32_8R8G8B_GET_B(lui32Col);
          lui32Col = (r + g + b) / 3;
        }
        else
        {
          return 0xFF;
        }
      }
      return lui32Col;
    }


    // Zeichnet eine horizontale Linie
    SCREEN_INLINE void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        u8*  lpui8Data;
        u32  lui32Idx;

        vSort(liGfxX1, liGfxX2);

        lui32Col = u32GetCol(lui32Col);

        lui32Idx = mpcBm->u32PxlIdx(liGfxX1, liGfxY1);

        lpui8Data = mpcBm->mpui8Data + lui32Idx;
        cMemTools::vMemSet(lpui8Data, lui32Col, liGfxX2 - liGfxX1 + 1);
      }
    }

    // Zeichnet eine horizontale Linie
    SCREEN_INLINE void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col1, uint32 lui32Col2)
    {
      //if (bChkInit())
      {
        if (lui32Col1 == lui32Col2)
        {
          vLineHAbsRaw(liGfxX1, liGfxX2, liGfxY1, lui32Col1, lui32Col1);
        }
        else
        {
          u8 g1;
          u8 g2;

          i32 g_inc, temp;

          u8 g;
          GfxInt liGfxPos;

          vSort(liGfxX1, liGfxX2);

          g1 = u32GetCol(lui32Col1);
          g2 = u32GetCol(lui32Col1);

          g_inc = ((g2 - g1) * 65536) / ((liGfxX2 - liGfxX1) + 1);

          liGfxPos = 0;

          while (liGfxX1 <= liGfxX2)
          {
            temp = liGfxPos * g_inc;
            g = g1 + (temp >> 16);
            if (temp & 0x8000) g++;

            SCREEN_BPP8_8G_PP(liGfxX1, liGfxY1, mpcBm, g);

            liGfxX1++;
            liGfxPos++;
          }
        }
      }
    }

    // Zeichnet eine vertikale Linie
    void vLineVAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        lui32Col = u32GetCol(lui32Col);
        while (liGfxY1 <= liGfxY2)
        {
          SCREEN_BPP8_8G_PP(liGfxX1, liGfxY1, mpcBm, lui32Col);
          liGfxY1++;
        }
      }
    }

    // Zeichnet eine Linie auf 
    void vLineAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1,
                     GfxInt liGfxX2, GfxInt liGfxY2,
                     uint32 lui32Col)
    {
      GfxInt liGfxXd, liGfxDx, liGfxDy;
      GfxInt liGfxAIncr, liGfxBIncr;
      GfxInt liGfxXIncr, liGfxYIncr;
      GfxInt liGfxx, liGfxy, liGfxDummy;

      lui32Col = u32GetCol(lui32Col);

      if ((mpcBm->iGfxAbs(liGfxX2 - liGfxX1) < mpcBm->iGfxAbs(liGfxY2 - liGfxY1)))
      {
        if (liGfxY1 > liGfxY2)
        {
          liGfxDummy = liGfxX1;
          liGfxX1 = liGfxX2;
          liGfxX2 = liGfxDummy;
          liGfxDummy = liGfxY1;
          liGfxY1 = liGfxY2;
          liGfxY2 = liGfxDummy;
        }

        if (liGfxX2 > liGfxX1) liGfxXIncr = 1;
        else liGfxXIncr = -1;

        liGfxDy = liGfxY2 - liGfxY1;
        liGfxDx = mpcBm->iGfxAbs(liGfxX2 - liGfxX1);
        liGfxXd = 2 * liGfxDx - liGfxDy;
        liGfxAIncr = 2 * (liGfxDx - liGfxDy);
        liGfxBIncr = 2 * liGfxDx;
        liGfxx = liGfxX1;
        liGfxy = liGfxY1;

        SCREEN_BPP8_8G_PP(liGfxx, liGfxy, mpcBm, lui32Col);

        for (liGfxy = liGfxY1 + 1; liGfxy <= liGfxY2; liGfxy++)
        {
          if (liGfxXd >= 0)
          {
            liGfxx += liGfxXIncr;
            liGfxXd += liGfxAIncr;
          }
          else liGfxXd += liGfxBIncr;

          SCREEN_BPP8_8G_PP(liGfxx, liGfxy, mpcBm, lui32Col);
        }
      }
      else
      {
        if (liGfxX1 > liGfxX2)
        {
          liGfxDummy = liGfxX1;
          liGfxX1 = liGfxX2;
          liGfxX2 = liGfxDummy;
          liGfxDummy = liGfxY1;
          liGfxY1 = liGfxY2;
          liGfxY2 = liGfxDummy;
        }
        if (liGfxY2 > liGfxY1) liGfxYIncr = 1;
        else liGfxYIncr = -1;

        liGfxDx = liGfxX2 - liGfxX1;
        liGfxDy = mpcBm->iGfxAbs(liGfxY2 - liGfxY1);
        liGfxXd = 2 * liGfxDy - liGfxDx;
        liGfxAIncr = 2 * (liGfxDy - liGfxDx);
        liGfxBIncr = 2 * liGfxDy;
        liGfxx = liGfxX1;
        liGfxy = liGfxY1;

        SCREEN_BPP8_8G_PP(liGfxx, liGfxy, mpcBm, lui32Col);

        for (liGfxx = liGfxX1 + 1; liGfxx <= liGfxX2; liGfxx++)
        {
          if (liGfxXd >= 0)
          {
            liGfxy += liGfxYIncr;
            liGfxXd += liGfxAIncr;
          }
          else liGfxXd += liGfxBIncr;

          SCREEN_BPP8_8G_PP(liGfxx, liGfxy, mpcBm, lui32Col);
        }
      }
    }
};


#ifdef __cplusplus
}
#endif

#endif  //SCREEN_BPP8_8G
