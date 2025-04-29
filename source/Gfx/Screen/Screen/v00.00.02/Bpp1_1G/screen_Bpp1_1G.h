#ifndef __SCREEN_BPP1_1G_H__
#define __SCREEN_BPP1_1G_H__

/* Includes ------------------------------------------------------------------*/
#include "screen_base.h"

#ifdef __cplusplus
 extern "C" {
#endif

#define SCREEN_BPP1_1G_CHECK(lpcScr)   BPP1_1G_CHECK((*lpcScr.mpcBm));

#define SCREEN_BPP1_1G_nPXL0 0x80
#define SCREEN_BPP1_1G_nPXL1 0x40
#define SCREEN_BPP1_1G_nPXL2 0x20
#define SCREEN_BPP1_1G_nPXL3 0x10
#define SCREEN_BPP1_1G_nPXL4 0x08
#define SCREEN_BPP1_1G_nPXL5 0x04
#define SCREEN_BPP1_1G_nPXL6 0x02
#define SCREEN_BPP1_1G_nPXL7 0x01

#define SCREEN_BPP1_1G_nPXLMsk0 (0xFF - SCREEN_BPP1_1G_nPXL0)
#define SCREEN_BPP1_1G_nPXLMsk1 (0xFF - SCREEN_BPP1_1G_nPXL1)
#define SCREEN_BPP1_1G_nPXLMsk2 (0xFF - SCREEN_BPP1_1G_nPXL2)
#define SCREEN_BPP1_1G_nPXLMsk3 (0xFF - SCREEN_BPP1_1G_nPXL3)
#define SCREEN_BPP1_1G_nPXLMsk4 (0xFF - SCREEN_BPP1_1G_nPXL4)
#define SCREEN_BPP1_1G_nPXLMsk5 (0xFF - SCREEN_BPP1_1G_nPXL5)
#define SCREEN_BPP1_1G_nPXLMsk6 (0xFF - SCREEN_BPP1_1G_nPXL6)
#define SCREEN_BPP1_1G_nPXLMsk7 (0xFF - SCREEN_BPP1_1G_nPXL7)

class cScreen_Bpp1_1G : public cScreen
{
  public:
    const uint8 nPxl[8] = {SCREEN_BPP1_1G_nPXL0, SCREEN_BPP1_1G_nPXL1, SCREEN_BPP1_1G_nPXL2, SCREEN_BPP1_1G_nPXL3,
                           SCREEN_BPP1_1G_nPXL4, SCREEN_BPP1_1G_nPXL5, SCREEN_BPP1_1G_nPXL6, SCREEN_BPP1_1G_nPXL7};

    const uint8 nPxlMsk[8] = {SCREEN_BPP1_1G_nPXLMsk0, SCREEN_BPP1_1G_nPXLMsk1, SCREEN_BPP1_1G_nPXLMsk2, SCREEN_BPP1_1G_nPXLMsk3,
                              SCREEN_BPP1_1G_nPXLMsk4, SCREEN_BPP1_1G_nPXLMsk5, SCREEN_BPP1_1G_nPXLMsk6, SCREEN_BPP1_1G_nPXLMsk7};

    cScreen_Bpp1_1G(cBitmap* lcBm)
      : cScreen(lcBm) {};
    cScreen_Bpp1_1G(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cBitmap* lcBm)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcBm) {};
    cScreen_Bpp1_1G(GfxInt liGfxAOIx1, GfxInt liGfxAOIy1, GfxInt liGfxAOIx2, GfxInt liGfxAOIy2, cScreen* lcScreen)
      : cScreen(liGfxAOIx1, liGfxAOIy1, liGfxAOIx2, liGfxAOIy2, lcScreen) {};
    ~cScreen_Bpp1_1G() {};

    SCREEN_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  >> 3);};
    SCREEN_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx << 3);};


    SCREEN_INLINE void vFill(u32 lui32Col)
    {
      GfxInt liGfxY;
      GfxInt liGfxWidth;
      u32 lui32Idx;
      u32 lui32BytePos;

      lui32Idx     = mpcBm->u32PxlIdx(miGfxClipPosx1, miGfxClipPosy1);
      lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
      lui32Idx     = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);

      u8* lpui8Data = (u8*)&mpcBm->mpui8Data[lui32BytePos];

      liGfxWidth  = miGfxClipPosx2 - miGfxClipPosx1 + 1;

      lui32Col = u32GetCol(lui32Col);

      for (liGfxY = miGfxClipPosy1; liGfxY <= miGfxClipPosy2; liGfxY++)
      {
        cMemTools::vMemBitSet(lpui8Data, lui32Idx, lui32Col, liGfxWidth);
        lpui8Data += u32PxlIdx2ByteIdx(mpcBm->miGfxWidth);
      }
    }

    SCREEN_INLINE void vPpAbsRaw(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col)
    {
      u32 lui32Idx;
      u32 lui32BytePos;

      lui32Idx     = mpcBm->u32PxlIdx(liGfxX, liGfxY);
      lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
      lui32Idx     = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);

      if (lui32Col)
      {
        mpcBm->mpui8Data[lui32BytePos] = BM_BPP1_1G_SET_PXL1(mpcBm->mpui8Data[lui32BytePos], lui32Idx);
      }
      else
      {
        mpcBm->mpui8Data[lui32BytePos] = BM_BPP1_1G_SET_PXL0(mpcBm->mpui8Data[lui32BytePos], lui32Idx);
      }
    }


    SCREEN_INLINE u32 u32GpAbsRaw(GfxInt liGfxX, GfxInt liGfxY)
    {
      u32 lui32Idx;
      u32 lui32BytePos;

      lui32Idx     = mpcBm->u32PxlIdx(liGfxX, liGfxY);
      lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
      lui32Idx     = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);

      if (BM_BPP1_1G_GET_PXL(mpcBm->mpui8Data[lui32BytePos], lui32Idx))
        return BM_BPP32_8R8G8B_RGB(0xFF, 0xFF, 0xFF);
      else
        return 0;
    }

    SCREEN_INLINE u32 u32GetCol(u32 lui32Col)
    {
      if (lui32Col) return 1;
      return 0;
    }

    // Zeichnet eine horizontale Linie
    SCREEN_INLINE void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        u8* lpui8Data;
        u32 lui32Idx;
        u32 lui32BytePos;

        vSort(liGfxX1, liGfxX2);

        lui32Idx = mpcBm->u32PxlIdx(liGfxX1, liGfxY1);
        lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
        lui32Idx = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);

        lui32Col = u32GetCol(lui32Col);

        lpui8Data = mpcBm->mpui8Data + lui32BytePos;
        cMemTools::vMemBitSet(lpui8Data, lui32Idx, lui32Col, liGfxX2 - liGfxX1 + 1);
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
          u8* lpui8Data;
          u32 lui32Idx;
          u32 lui32BytePos;
          GfxInt liGfxX_Dist;

          vSort(liGfxX1, liGfxX2);

          lui32Col1 = u32GetCol(lui32Col1);
          lui32Col2 = u32GetCol(lui32Col2);

          liGfxX_Dist = (liGfxX2 - liGfxX1 + 1);

          lui32Idx = mpcBm->u32PxlIdx(liGfxX1, liGfxY1);
          lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
          lui32Idx = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);
          lpui8Data = mpcBm->mpui8Data + lui32BytePos;
          cMemTools::vMemBitSet(lpui8Data, lui32Idx, lui32Col1, liGfxX_Dist / 2);
          
          lui32Idx = mpcBm->u32PxlIdx(liGfxX1 + liGfxX_Dist / 2 + 1, liGfxY1);
          lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
          lui32Idx = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);
          lpui8Data = mpcBm->mpui8Data + lui32BytePos;
          cMemTools::vMemBitSet(lpui8Data, lui32Idx, lui32Col2, liGfxX_Dist - liGfxX_Dist / 2);
        }
      }
    }


    // Zeichnet eine vertikale Linie
    void vLineVAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, uint32 lui32Col)
    {
      //if (bChkInit())
      {
        u8* lpui8Data;
        u32 lui32Idx;
        u32 lui32BytePos;

        uint8  lui8Mask;
        GfxInt liGfxWidth;

        lui32Idx = mpcBm->u32PxlIdx(liGfxX1, liGfxY1);
        lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
        lui32Idx = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);

        lpui8Data = mpcBm->mpui8Data + lui32BytePos;
        liGfxWidth = mpcBm->miGfxWidth / 8;

        if (lui32Col)
        {
          lui8Mask = nPxl[lui32Idx];
          while (liGfxY1 <= liGfxY2)
          {
            *lpui8Data |= lui8Mask;
            liGfxY1++;
            lpui8Data += liGfxWidth;
          }
        }
        else
        {
          lui8Mask = nPxlMsk[lui32Idx];
          while (liGfxY1 <= liGfxY2)
          {
            *lpui8Data &= lui8Mask;
            liGfxY1++;
            lpui8Data += liGfxWidth;
          }
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

      //if (bChkInit())
      {
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

          vPpAbsRaw(liGfxx, liGfxy, lui32Col);

          for (liGfxy = liGfxY1 + 1; liGfxy <= liGfxY2; liGfxy++)
          {
            if (liGfxXd >= 0)
            {
              liGfxx += liGfxXIncr;
              liGfxXd += liGfxAIncr;
            }
            else liGfxXd += liGfxBIncr;

            vPpAbsRaw(liGfxx, liGfxy, lui32Col);
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

          vPpAbsRaw(liGfxx, liGfxy, lui32Col);

          for (liGfxx = liGfxX1 + 1; liGfxx <= liGfxX2; liGfxx++)
          {
            if (liGfxXd >= 0)
            {
              liGfxy += liGfxYIncr;
              liGfxXd += liGfxAIncr;
            }
            else liGfxXd += liGfxBIncr;

            vPpAbsRaw(liGfxx, liGfxy, lui32Col);
          }
        }
      }
    }
};


#ifdef __cplusplus
}
#endif

#endif  //SCREEN_BPP1_1G
