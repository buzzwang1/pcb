#ifndef __SCREEN_BPP16_5R6G5B_H__
#define __SCREEN_BPP16_5R6G5B_H__

/* Includes ------------------------------------------------------------------*/
#include "screen_base.h"

#ifdef __cplusplus
 extern "C" {
#endif

#if SCREEN_BPP16_5R6G5B_DIRECT
  #define SCREEN_BPP16_5R6G5B_PP(liGfxX, liGfxY, lcBm, lu16Col) SCREEN_BPP16_5R6G5B_PXL((liGfxX), (liGfxY), (*lcBm)) = lu16Col
  #define SCREEN_BPP16_5R6G5B_GP(liGfxX, liGfxY, lcBm)          SCREEN_BPP16_5R6G5B_PXL((liGfxX), (liGfxY), (*lcBm))
#else
  #define SCREEN_BPP16_5R6G5B_PP(liGfxX, liGfxY, lcBm, lu16Col)   ((cBitmap_Bpp16_5R6G5B*)lcBm)->vPp(liGfxX, liGfxY, lu16Col)
  #define SCREEN_BPP16_5R6G5B_GP(liGfxX, liGfxY, lcBm)            ((cBitmap_Bpp16_5R6G5B*)lcBm)->u16Gp(liGfxX, liGfxY)
#endif

class cScreen_Bpp16_5R6G5B : public cScreen
{
  public:
    cScreen_Bpp16_5R6G5B(cBitmap* lcBm)
      : cScreen(lcBm) {};
    cScreen_Bpp16_5R6G5B(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cBitmap* lcBm)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcBm) {};
    cScreen_Bpp16_5R6G5B(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cScreen* lcScreen)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcScreen) {};
    ~cScreen_Bpp16_5R6G5B() {};

    SCREEN_INLINE u32 u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  * 2);};
    SCREEN_INLINE u32 u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx / 2);};

    SCREEN_INLINE void vFill(u32 lui32Col)
    {
      GfxInt liGfxY;
      GfxInt liGfxWidth;

      u16* lpui16Data = (u16*)&mpcBm->mpui8Data[u32PxlIdx2ByteIdx(mpcBm->u32PxlIdx(miGfxClipPosx1, miGfxClipPosy1))];

      liGfxWidth  = miGfxClipPosx2 - miGfxClipPosx1 + 1;

      for (liGfxY = miGfxClipPosy1; liGfxY <= miGfxClipPosy2; liGfxY++)
      {
        MEMTOOLS_vMemSet16(lpui16Data, lui32Col, liGfxWidth);
        lpui16Data += mpcBm->miGfxWidth;
      }
    }

    SCREEN_INLINE void vPpAbsRaw(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col)
    {
      SCREEN_BPP16_5R6G5B_PP(liGfxX, liGfxY, mpcBm, lui32Col);
    }

    SCREEN_INLINE u32 u32GpAbsRaw(GfxInt liGfxX, GfxInt liGfxY)
    {
      return SCREEN_BPP16_5R6G5B_GP(liGfxX, liGfxY, mpcBm);
    }

    SCREEN_INLINE u32 u32GetCol(u32 lui32Col)
    {           
      u8 r, g, b;

      r = BM_BPP32_8R8G8B_GET_R(lui32Col) >> 3;
      g = BM_BPP32_8R8G8B_GET_G(lui32Col) >> 2;
      b = BM_BPP32_8R8G8B_GET_B(lui32Col) >> 3;

      lui32Col = BM_BPP16_5R6G5B_RGB(r, g, b);

      return lui32Col;
    }



    // Zeichnet eine horizontale Linie
    SCREEN_INLINE void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        u16* lpui16Data;
        u32  lui32Idx;
        u32  lui32BytePos;

        lui32Idx = mpcBm->u32PxlIdx(liGfxX1, liGfxY1);
        lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);

        lpui16Data = (uint16*)(mpcBm->mpui8Data + lui32BytePos);
        MEMTOOLS_vMemSet16(lpui16Data, lui32Col, liGfxX2 - liGfxX1 + 1);
      }
    }

    // Zeichnet eine vertikale Linie
    void vLineVAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        while (liGfxY1 <= liGfxY2)
        {
          SCREEN_BPP16_5R6G5B_PP(liGfxX1, liGfxY1, mpcBm, lui32Col);
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

        SCREEN_BPP16_5R6G5B_PP(liGfxx, liGfxy, mpcBm, lui32Col);

        for (liGfxy = liGfxY1 + 1; liGfxy <= liGfxY2; liGfxy++)
        {
          if (liGfxXd >= 0)
          {
            liGfxx += liGfxXIncr;
            liGfxXd += liGfxAIncr;
          }
          else liGfxXd += liGfxBIncr;

          SCREEN_BPP16_5R6G5B_PP(liGfxx, liGfxy, mpcBm, lui32Col);
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

        SCREEN_BPP16_5R6G5B_PP(liGfxx, liGfxy, mpcBm, lui32Col);

        for (liGfxx = liGfxX1 + 1; liGfxx <= liGfxX2; liGfxx++)
        {
          if (liGfxXd >= 0)
          {
            liGfxy += liGfxYIncr;
            liGfxXd += liGfxAIncr;
          }
          else liGfxXd += liGfxBIncr;

          SCREEN_BPP16_5R6G5B_PP(liGfxx, liGfxy, mpcBm, lui32Col);
        }
      }
    }
};


#ifdef __cplusplus
}
#endif

#endif  //SCREEN_BPP16_5R6G5B
