#ifndef __PAINT_TRIANGLE_Z_TElfXTURE_lfBRIGHTNESS_GRADIENT_BASE_H__
#define __PAINT_TRIANGLE_Z_TElfXTURE_lfBRIGHTNESS_GRADIENT_BASE_H__

#include "screen.h"
#include "math.h"


#ifdef __cplusplus
 extern "C" {
#endif

class cPaintTriZTxBrigthgrad;
typedef void(cPaintTriZTxBrigthgrad::*cPaintTriZTxBrigthgrad_vLineHAbs)(u16);

class cPaintTriZTxBrigthgrad
{
  public:
    cPaintTriZTxBrigthgrad_vLineHAbs vLineHAbs;

    typedef struct
    {
      float fx;
      float fiz;

      float fiTexX;
      float fiTexY;

      u8    u8B;
    }tstScanline;


    typedef struct
    {
      typedef struct{
        float fx, fy, fiz;

        typedef struct 
        {
          float fx;
          float fy;
        }tstTex;
        tstTex stTex;

        u8     u8B;
      }tstData;

      tstData Dat[3];
      
      cBitmap *cBm;
    }tstTri;
    
    tstScanline*  mstLeft;
    tstScanline*  mstRight;

    tstTri  *mstTri;
    
    cScreen* mcScr;
    float*   mfzlfBuffer;

    cPaintTriZTxBrigthgrad(float* lzlfBuffer)
    {
      mcScr     = null;
      mstLeft   = null;
      mstRight  = null;
      vLineHAbs = null;
      mfzlfBuffer = lzlfBuffer;
    }

    ~cPaintTriZTxBrigthgrad()
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
            vLineHAbs =  &cPaintTriZTxBrigthgrad::BM_BPP16_5R6G5B_vLineHAbs;
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
        mstLeft[liGfxPosY1].fiz = mstTri->Dat[lu8t1].fiz;
        mstLeft[liGfxPosY1].fiTexX = mstTri->Dat[lu8t1].stTex.fx * mstTri->Dat[lu8t1].fiz;
        mstLeft[liGfxPosY1].fiTexY = mstTri->Dat[lu8t1].stTex.fy * mstTri->Dat[lu8t1].fiz;

        mstRight[liGfxPosY1].u8B = mstTri->Dat[lu8t2].u8B;
        mstRight[liGfxPosY1].fx = mstTri->Dat[lu8t2].fx;
        mstRight[liGfxPosY1].fiz = mstTri->Dat[lu8t2].fiz;
        mstRight[liGfxPosY1].fiTexX = mstTri->Dat[lu8t2].stTex.fx * mstTri->Dat[lu8t1].fiz;
        mstRight[liGfxPosY1].fiTexY = mstTri->Dat[lu8t2].stTex.fy * mstTri->Dat[lu8t1].fiz;
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
          float lfiZ_inc = (mstTri->Dat[lu8t2].fiz - mstTri->Dat[lu8t1].fiz)    * lfdy;
          float lfiTexX_inc = (mstTri->Dat[lu8t2].stTex.fx * mstTri->Dat[lu8t2].fiz - mstTri->Dat[lu8t1].stTex.fx * mstTri->Dat[lu8t1].fiz) * lfdy;
          float lfiTexY_inc = (mstTri->Dat[lu8t2].stTex.fy * mstTri->Dat[lu8t2].fiz - mstTri->Dat[lu8t1].stTex.fy * mstTri->Dat[lu8t1].fiz) * lfdy;

          float lfB  = mstTri->Dat[lu8t1].u8B;
          float lfiZ = mstTri->Dat[lu8t1].fiz;
          float lfX  = mstTri->Dat[lu8t1].fx;
          float lfiTexX = mstTri->Dat[lu8t1].stTex.fx * lfiZ;
          float lfiTexY = mstTri->Dat[lu8t1].stTex.fy * lfiZ;

          lfB     += (liGfxPosY1 - liGfxPosY1_Save) * lfb_inc;
          lfX     += (liGfxPosY1 - liGfxPosY1_Save) * lfX_inc;
          lfiZ    += (liGfxPosY1 - liGfxPosY1_Save) * lfiZ_inc;
          lfiTexX += (liGfxPosY1 - liGfxPosY1_Save) * lfiTexX_inc;
          lfiTexY += (liGfxPosY1 - liGfxPosY1_Save) * lfiTexY_inc;

          /*if (((lstMem == mstRight) && (lfX_inc > 0)) ||
              ((lstMem == mstLeft)  && (lfX_inc < 0)))
          {
            lfB  += lfb_inc;
            lfX  += lfX_inc;
            lfiZ += lfiZ_inc;
            lfiTexX += lfiTexX_inc;
            lfiTexY += lfiTexY_inc;
          }*/

          while (liGfxPosY1 <= liGfxPosY2)
          {
            lstMem[liGfxPosY1].u8B  = (u8)lfB;
            lstMem[liGfxPosY1].fx  = lfX;
            lstMem[liGfxPosY1].fiz = lfiZ;
            lstMem[liGfxPosY1].fiTexX = lfiTexX;
            lstMem[liGfxPosY1].fiTexY = lfiTexY;

            lfB  += lfb_inc;
            lfX  += lfX_inc;
            lfiZ += lfiZ_inc;
            lfiTexX += lfiTexX_inc;
            lfiTexY += lfiTexY_inc;

            liGfxPosY1++;
          }
        }
      }
    }

    void BM_BPP16_5R6G5B_vLineHAbs(u16 lu16IdxY)
    {
      u16 lu16r, lu16g, lu16b;

      GfxInt liGfxTexX;
      GfxInt liGfxTexY;

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
        float lfiZ_inc = (mstRight[lu16IdxY].fiz - mstLeft[lu16IdxY].fiz)    * dx;
        float lfiZ = mstLeft[lu16IdxY].fiz + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfiZ_inc;

        //Überprüfen ob, überhaupt sichtbar ist
        u8  lu8Paint = 0;
        
        if ((liGfxPoslfX2 - liGfxPoslfX1) > 4)
        {
          float lfzTemp;
          lfzTemp = fGetZ(liGfxPoslfX1, lu16IdxY);
          if ((1.0f / lfiZ) >= (lfzTemp - 1.0f))
          {
            lu8Paint++;
          }

          lfzTemp = fGetZ(liGfxPoslfX2, lu16IdxY);
          if ((1.0f / (lfiZ + lfiZ_inc * (liGfxPoslfX2 - liGfxPoslfX1))) >= (lfzTemp - 1.0f))
          {
            lu8Paint++;
          }
        }
        else
        {
          lu8Paint = 1;
        }

        if (lu8Paint > 0)
        {
          float lfb_inc = (mstRight[lu16IdxY].u8B - mstLeft[lu16IdxY].u8B)    * dx;
          float lfiTexX_inc = (mstRight[lu16IdxY].fiTexX - mstLeft[lu16IdxY].fiTexX) * dx;
          float lfiTexY_inc = (mstRight[lu16IdxY].fiTexY - mstLeft[lu16IdxY].fiTexY) * dx;

          float lfz;
          float lfB = mstLeft[lu16IdxY].u8B + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfb_inc;
          float lfiTexX = mstLeft[lu16IdxY].fiTexX + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfiTexX_inc;
          float lfiTexY = mstLeft[lu16IdxY].fiTexY + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfiTexY_inc;

          float* lpu16StartZ = &mfzlfBuffer[lu16IdxY * mcScr->mpcBm->miGfxWidth + liGfxPoslfX1];
          u16    lu16B;

          u16*   lpu16Start = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX1, lu16IdxY, *mcScr->mpcBm);
          u16*   lpu16End = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX2, lu16IdxY, *mcScr->mpcBm);

          while (lpu16Start <= lpu16End)
          {
            lfz = 1.0f / lfiZ;

            liGfxTexX = (GfxInt)(lfz * lfiTexX);
            liGfxTexY = (GfxInt)(lfz * lfiTexY);

            if (lfz > *lpu16StartZ)
            {
              /*
              if (liGfxTexX < 0)
                liGfxTexX = 0;
              if (liGfxTexX >= mstTri->lfBm->miGfxWidth)
                liGfxTexX = mstTri->lfBm->miGfxWidth - 1;
              if (liGfxTexY < 0)
                liGfxTexY = 0;
              if (liGfxTexY >= mstTri->lfBm->miGfxHeight)
                liGfxTexY = mstTri->lfBm->miGfxHeight - 1;*/

              BM_BPP16_5R6G5B_GET_RGB(BM_BPP16_5R6G5B_PXL(liGfxTexX, liGfxTexY, *mstTri->cBm), lu16r, lu16g, lu16b);

              lu16B = (u16)lfB;
              lu16r *= lu16B;
              lu16g *= lu16B;
              lu16b *= lu16B;

              lu16r >>= 8;
              lu16g >>= 8;
              lu16b >>= 8;

              *lpu16StartZ = lfz;
              *lpu16Start = BM_BPP16_5R6G5B_RGB(lu16r, lu16g, lu16b);
            }

            lfB += lfb_inc;
            lfiZ += lfiZ_inc;
            lfiTexX += lfiTexX_inc;
            lfiTexY += lfiTexY_inc;

            lpu16Start++;
            lpu16StartZ++;
          }
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

    inline float fGetZ(GfxInt liGfxlfX, GfxInt liGfxY)
    {
      return mfzlfBuffer[liGfxY * mcScr->mpcBm->miGfxWidth + liGfxlfX];
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


#endif // __PAINT_TRIANGLE_Z_TElfXTURE_lfBRIGHTNESS_GRADIENT_BASE_H__

