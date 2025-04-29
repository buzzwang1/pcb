#ifndef __PAINT_TRIANGLE_BRIGHTNESS_GRADIENT_BASE_H__
#define __PAINT_TRIANGLE_BRIGHTNESS_GRADIENT_BASE_H__

#include "screen.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cPaintTriBrigthgrad;
typedef void(cPaintTriBrigthgrad::*cPaintTriBrigthgrad_vLineHAbs)(u16);

class cPaintTriBrigthgrad
{
  public:
    cPaintTriBrigthgrad_vLineHAbs vLineHAbs;

    typedef struct
    {
      float fx;
      u8    u8B;
    }tstScanline;


    typedef struct
    {
      typedef struct{
        float fx, fy;
        u8     u8B;
      }tstData;

      tstData Dat[3];
      unBmBaseCol unCol;
    }tstTri;
    
    tstScanline*  mstLeft;
    tstScanline*  mstRight;

    tstTri  *mstTri;
    
    cScreen* mcScr;

    cPaintTriBrigthgrad()
    {
      mcScr     = null;
      mstLeft   = null;
      mstRight  = null;
      vLineHAbs = null;
    }

    ~cPaintTriBrigthgrad()
    {
      vFreeMem();
    }

    void vSetScreen(cScreen* lcScr)
    {
      bool lbGetMem;

      lbGetMem = False;

      if (mcScr != null)
      {
        if (mcScr->mpcBm != lcScr->mpcBm)
        {
          vFreeMem();
          lbGetMem = True;
        }
      }
      else
      {
        lbGetMem = True;
      }

      if (lbGetMem)
      {
        mcScr = lcScr;

        switch (lcScr->mpcBm->u8Type())
        {
          case BM_BPP1_1G:
            break;
          case BM_BPP8_8G:
            break;
          case BM_BPP16_5R6G5B:
            vLineHAbs =  &cPaintTriBrigthgrad::BM_BPP16_5R6G5B_vLineHAbs;
            break;
          case BM_BPP32_8R8G8B:
            break;
          default:
            vLineHAbs = null;
            break;
        }

        mstLeft  = (tstScanline*) new tstScanline[lcScr->mpcBm->miGfxHeight];
        mstRight = (tstScanline*) new tstScanline[lcScr->mpcBm->miGfxHeight];
      }
    }

    void vFreeMem()
    {
      if (mstLeft !=    null) delete[]mstLeft;
      if (mstRight !=   null) delete[]mstRight;
    }

    inline GfxInt iGfxRound(float lf)
    {
      if (lf > 0) return (GfxInt)(lf + 0.5f);
      return (GfxInt)(lf - 0.5f);
    }

    void vScanLineAbs(uint8 lu8t1, uint8 lu8t2, tstScanline* lstMem)
    {
      GfxInt liGfxPosY1   = iGfxRound(mstTri->Dat[lu8t1].fy);
      GfxInt liGfxPosY2   = iGfxRound(mstTri->Dat[lu8t2].fy);

      if (liGfxPosY1 == liGfxPosY2)
      {
        if (!mcScr->isInY1Abs(liGfxPosY1)) return;
        if (!mcScr->isInY2Abs(liGfxPosY1)) return;
        if (!mcScr->isInX1Abs((GfxInt)mstTri->Dat[lu8t1].fx)) return;
        if (!mcScr->isInX2Abs((GfxInt)mstTri->Dat[lu8t2].fx)) return;

        mstLeft[liGfxPosY1].u8B = mstTri->Dat[lu8t1].u8B;
        mstLeft[liGfxPosY1].fx = mstTri->Dat[lu8t1].fx;

        mstRight[liGfxPosY1].u8B = mstTri->Dat[lu8t2].u8B;
        mstRight[liGfxPosY1].fx = mstTri->Dat[lu8t2].fx;
      }
      else
      {
        GfxInt liGfxPoslfX1 = iGfxRound(mstTri->Dat[lu8t1].fx);
        GfxInt liGfxPoslfX2 = iGfxRound(mstTri->Dat[lu8t2].fx);

        float lfdy = (float)(liGfxPosY2 - liGfxPosY1);
        lfdy = 1.0f / lfdy;

        GfxInt liGfxPosY1_Save = liGfxPosY1;

        float lfX_inc = (liGfxPoslfX2 - liGfxPoslfX1)                   * lfdy;

        if (mcScr->bLineClipYAbs(liGfxPoslfX1, liGfxPosY1, liGfxPoslfX2, liGfxPosY2))
        {
          float lfb_inc = (mstTri->Dat[lu8t2].u8B - mstTri->Dat[lu8t1].u8B)       * lfdy;

          float lfB  = mstTri->Dat[lu8t1].u8B;
          float lfX  = mstTri->Dat[lu8t1].fx;

          lfB     += (liGfxPosY1 - liGfxPosY1_Save) * lfb_inc;
          lfX     += (liGfxPosY1 - liGfxPosY1_Save) * lfX_inc;

          while (liGfxPosY1 <= liGfxPosY2)
          {
            lstMem[liGfxPosY1].u8B  = (u8)lfB;
            lstMem[liGfxPosY1].fx  = lfX;

            lfB  += lfb_inc;
            lfX  += lfX_inc;

            liGfxPosY1++;
          }
        }
      }
    }

    void BM_BPP16_5R6G5B_vLineHAbs(u16 lu16IdxY)
    {
      u16 lu16r, lu16g, lu16b;
      u16 lu16r2, lu16g2, lu16b2;

      GfxInt liGfxPoslfX1 = iGfxRound(mstLeft[lu16IdxY].fx);
      GfxInt liGfxPoslfX2 = iGfxRound(mstRight[lu16IdxY].fx);

      float dx = (float)(liGfxPoslfX2 - liGfxPoslfX1);
      if (liGfxPoslfX2 != liGfxPoslfX1)
        dx = 1.0f / dx;
      else
        dx = 0.0f;

      GfxInt liGfxPoslfX1_Save = liGfxPoslfX1;

      if (mcScr->bHLineClipAbs(liGfxPoslfX1, liGfxPoslfX2, lu16IdxY))
      {
        float lfb_inc = (mstRight[lu16IdxY].u8B - mstLeft[lu16IdxY].u8B)    * dx;
        float lfB = mstLeft[lu16IdxY].u8B + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfb_inc;

        u16    lu16B;

        u16*   lpu16Start = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX1, lu16IdxY, *mcScr->mpcBm);
        u16*   lpu16End = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX2, lu16IdxY, *mcScr->mpcBm);

        if (mstTri->unCol.stARGB.r == 0xFF) lu16r = 31;
                                        else lu16r = mstTri->unCol.stARGB.r >> 3;
        if (mstTri->unCol.stARGB.g == 0xFF) lu16g = 63;
                                        else lu16g = mstTri->unCol.stARGB.g >> 2;
        if (mstTri->unCol.stARGB.b == 0xFF) lu16b = 31;
                                        else lu16b = mstTri->unCol.stARGB.b >> 3;

        while (lpu16Start <= lpu16End)
        {
          lu16B = (u16)lfB;
          lu16r2 = (lu16r * lu16B) >> 8;
          lu16g2 = (lu16g * lu16B) >> 8;
          lu16b2 = (lu16b * lu16B) >> 8;

          *lpu16Start = BM_BPP16_5R6G5B_RGB(lu16r2, lu16g2, lu16b2);

          lfB += lfb_inc;

          lpu16Start++;
        }

      }
    }

    void vSortlfX(uint8 lu8t1, uint8 lu8t2)
    {
      tstTri::tstData lstTemp;

      if (mstTri->Dat[lu8t2].fx < mstTri->Dat[lu8t1].fx)
      {
        lstTemp = mstTri->Dat[lu8t1];
        mstTri->Dat[lu8t1] = mstTri->Dat[lu8t2];
        mstTri->Dat[lu8t2] = lstTemp;
      }
    }

    void vSortY(uint8 lu8t1, uint8 lu8t2)
    {
      tstTri::tstData lstTemp;

      if (mstTri->Dat[lu8t2].fy < mstTri->Dat[lu8t1].fy)
      {
        lstTemp            = mstTri->Dat[lu8t1];
        mstTri->Dat[lu8t1] = mstTri->Dat[lu8t2];
        mstTri->Dat[lu8t2] = lstTemp;
      }
    }


     // Zeichnet ein ausgefülltes Dreieck auf Schirm
    void vTriangle(tstTri  *lcTri)
    {
      int t;
      GfxInt liGfxMinY;
      GfxInt liGfxMaxY;
      GfxInt liGfxi;

      liGfxMinY = -1;
      liGfxMaxY = -1;

      mstTri = lcTri;

      vSortY(1,2);
      vSortY(0,1);
      vSortY(1,2);

      for (t = 0; t < 3; t++)
      {
        mstTri->Dat[t].fx += mcScr->miGfxRefPosx1;
        mstTri->Dat[t].fy += mcScr->miGfxRefPosy1;
      }

      if (mstTri->Dat[0].fy == mstTri->Dat[2].fy)
      {
        vSortlfX(0, 1);
        vSortlfX(1, 2);
        liGfxMinY = liGfxMaxY = iGfxRound(mstTri->Dat[0].fy);
        vScanLineAbs(0, 2, mstRight);
      }
      else
      {
        liGfxMinY = iGfxRound(mstTri->Dat[0].fy);
        if (liGfxMinY < iGfxRound(mcScr->miGfxClipPosy1)) liGfxMinY = iGfxRound(mcScr->miGfxClipPosy1);

        liGfxMaxY = iGfxRound(mstTri->Dat[2].fy);
        if (liGfxMaxY > iGfxRound(mcScr->miGfxClipPosy2)) liGfxMaxY = iGfxRound(mcScr->miGfxClipPosy2);

        if (mstTri->Dat[0].fy == mstTri->Dat[1].fy)
        {
          /*  1-----0    0-----1
               \   /  =>  \   / 
                \ /   =>   \ /  
                 2          2    */
          vSortlfX(0, 1);

          /*  0-----1
               \   /
                \ /
                 2   */
          vScanLineAbs(0, 2, mstLeft);
          vScanLineAbs(1, 2, mstRight);
        }
        else
        if (mstTri->Dat[1].fy == mstTri->Dat[2].fy)
        {
          /*     0          0   
                / \   =>   / \  
               /   \  =>  /   \ 
              2-----1    1-----2 */
          vSortlfX(1, 2);

          /*     0   
                / \  
               /   \ 
              1-----2  */
          vScanLineAbs(0, 1, mstLeft);
          vScanLineAbs(0, 2, mstRight);
        }
        else
        {
          float lim12 = (mstTri->Dat[1].fx - mstTri->Dat[0].fx) / (mstTri->Dat[1].fy - mstTri->Dat[0].fy);
          float lim13 = (mstTri->Dat[2].fx - mstTri->Dat[0].fx) / (mstTri->Dat[2].fy - mstTri->Dat[0].fy);

          if (lim12 < lim13)
          {
             /*     0 
                  / |
                 /  |
                1   |
                 \  |
                  \ |
                    2  */

            vScanLineAbs(0, 1, mstLeft);
            vScanLineAbs(1, 2, mstLeft);
            vScanLineAbs(0, 2, mstRight);
          }
          else
          {
            /*  0 
                | \
                |  \
                |   1
                |  /
                | /
                2     */
            vScanLineAbs(0, 1, mstRight);
            vScanLineAbs(1, 2, mstRight);
            vScanLineAbs(0, 2, mstLeft);
          }
        }

        for (liGfxi = liGfxMinY; liGfxi <= liGfxMaxY; liGfxi++)
        {
          ((this->*vLineHAbs)(liGfxi));
        }
      }
    }
};


#ifdef __cplusplus
}
#endif


#endif // __PAINT_TRIANGLE_BRIGHTNESS_GRADIENT_BASE_H__

