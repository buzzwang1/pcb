#ifndef __SCREEN_BPP32_8R8G8B_H__
#define __SCREEN_BPP32_8R8G8B_H__

/* Includes ------------------------------------------------------------------*/
#include "screen_base.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if SCREEN_BPP16_5R6G5B_DIRECT
  #define SCREEN_BPP32_8R8G8_PP(liGfxX, liGfxY, lcBm, lu16Col) SCREEN_BPP32_8R8G8_PXL((liGfxX), (liGfxY), (*lcBm)) = lu16Col
  #define SCREEN_BPP32_8R8G8_GP(liGfxX, liGfxY, lcBm)          SCREEN_BPP32_8R8G8_PXL((liGfxX), (liGfxY), (*lcBm))
#else
  #define SCREEN_BPP32_8R8G8_PP(liGfxX, liGfxY, lcBm, lu16Col)   ((cBitmap_Bpp32_8R8G8B*)lcBm)->vPp(liGfxX, liGfxY, lu16Col)
  #define SCREEN_BPP32_8R8G8_GP(liGfxX, liGfxY, lcBm)            ((cBitmap_Bpp32_8R8G8B*)lcBm)->u32Gp(liGfxX, liGfxY)
#endif
class cScreen_Bpp32_8R8G8B : public cScreen
{
  public:
    cScreen_Bpp32_8R8G8B(cBitmap* lcBm)
      : cScreen(lcBm) {};
    cScreen_Bpp32_8R8G8B(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cBitmap* lcBm)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcBm) {};
    cScreen_Bpp32_8R8G8B(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cScreen* lcScreen)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcScreen) {};
    ~cScreen_Bpp32_8R8G8B() {};

    SCREEN_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  * 4);};
    SCREEN_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx / 4);};

    SCREEN_INLINE void vFill(u32 lui32Col)
    {
      GfxInt liGfxY;
      GfxInt liGfxWidth;

      u32* lpui32Data = (u32*)&mpcBm->mpui8Data[u32PxlIdx2ByteIdx(mpcBm->u32PxlIdx(miGfxClipPosx1, miGfxClipPosy1))];

      liGfxWidth  = miGfxClipPosx2 - miGfxClipPosx1 + 1;

      for (liGfxY = miGfxClipPosy1; liGfxY <= miGfxClipPosy2; liGfxY++)
      {
        cMemTools::vMemSet32(lpui32Data, lui32Col, liGfxWidth);
        lpui32Data += mpcBm->miGfxWidth;
      }
    }

    SCREEN_INLINE void vPpAbsRaw(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col)
    {
      SCREEN_BPP32_8R8G8_PP(liGfxX, liGfxY, mpcBm, lui32Col);
    }

    SCREEN_INLINE u32 u32GpAbsRaw(GfxInt liGfxX, GfxInt liGfxY)
    {
      return SCREEN_BPP32_8R8G8_GP(liGfxX, liGfxY, mpcBm);
    }

    SCREEN_INLINE u32 u32GetCol(u32 lui32Col)
    {
      return lui32Col;
    }

    // Zeichnet eine horizontale Linie
    SCREEN_INLINE void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        u32* lpui32Data;
        u32  lui32Idx;
        u32  lui32BytePos;

        vSort(liGfxX1, liGfxX2);

        lui32Idx = mpcBm->u32PxlIdx(liGfxX1, liGfxY1);
        lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);

        lpui32Data = (uint32*)(mpcBm->mpui8Data + lui32BytePos);
        cMemTools::vMemSet32(lpui32Data, lui32Col, liGfxX2 - liGfxX1 + 1);
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
          u8 r1, g1, b1;
          u8 r2, g2, b2;

          i32 r_inc, g_inc, b_inc, temp;

          u8 r, g, b;
          GfxInt liGfxPos;

          vSort(liGfxX1, liGfxX2);

          r1 = BM_BPP32_8R8G8B_GET_R(lui32Col1);
          g1 = BM_BPP32_8R8G8B_GET_G(lui32Col1);
          b1 = BM_BPP32_8R8G8B_GET_B(lui32Col1);

          r2 = BM_BPP32_8R8G8B_GET_R(lui32Col2);
          g2 = BM_BPP32_8R8G8B_GET_G(lui32Col2);
          b2 = BM_BPP32_8R8G8B_GET_B(lui32Col2);

          r_inc = ((r2 - r1) * 65536) / ((liGfxX2 - liGfxX1) + 1);
          g_inc = ((g2 - g1) * 65536) / ((liGfxX2 - liGfxX1) + 1);
          b_inc = ((b2 - b1) * 65536) / ((liGfxX2 - liGfxX1) + 1);

          liGfxPos = 0;

          while (liGfxX1 <= liGfxX2)
          {
            temp = liGfxPos * r_inc;
            r = r1 + (temp >> 16);
            if (temp & 0x8000) r++;

            temp = liGfxPos * g_inc;
            g = g1 + (temp >> 16);
            if (temp & 0x8000) g++;

            temp = liGfxPos * b_inc;
            b = b1 + (temp >> 16);
            if (temp & 0x8000) b++;

            SCREEN_BPP32_8R8G8_PP(liGfxX1, liGfxY1, mpcBm, BM_BPP32_8R8G8B_RGB(r, g, b));

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
        while (liGfxY1 <= liGfxY2)
        {
          SCREEN_BPP32_8R8G8_PP(liGfxX1, liGfxY1, mpcBm, lui32Col);
          liGfxY1++;
        }
      }
    }

    // Zeichnet eine Linie 
    void vLineAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1,
                     GfxInt liGfxX2, GfxInt liGfxY2,
                     uint32 lui32Col)
    {
      GfxInt liGfxXd, liGfxDx, liGfxDy;
      GfxInt liGfxAIncr, liGfxBIncr;
      GfxInt liGfxXIncr, liGfxYIncr;
      GfxInt liGfxx, liGfxy, liGfxDummy;

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

        SCREEN_BPP32_8R8G8_PP(liGfxx, liGfxy, mpcBm, lui32Col);

        for (liGfxy = liGfxY1 + 1; liGfxy <= liGfxY2; liGfxy++)
        {
          if (liGfxXd >= 0)
          {
            liGfxx += liGfxXIncr;
            liGfxXd += liGfxAIncr;
          }
          else liGfxXd += liGfxBIncr;

          SCREEN_BPP32_8R8G8_PP(liGfxx, liGfxy, mpcBm, lui32Col);
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

        SCREEN_BPP32_8R8G8_PP(liGfxx, liGfxy, mpcBm, lui32Col);

        for (liGfxx = liGfxX1 + 1; liGfxx <= liGfxX2; liGfxx++)
        {
          if (liGfxXd >= 0)
          {
            liGfxy += liGfxYIncr;
            liGfxXd += liGfxAIncr;
          }
          else liGfxXd += liGfxBIncr;

          SCREEN_BPP32_8R8G8_PP(liGfxx, liGfxy, mpcBm, lui32Col);
        }
      }
    }
};


#ifdef __cplusplus
}
#endif

#endif  //SCREEN_BPP32_8R8G8B
