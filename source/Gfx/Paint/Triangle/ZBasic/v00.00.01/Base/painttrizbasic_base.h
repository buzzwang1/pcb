#ifndef _PAINT_TRIANGLE_ZBASIC_BASE_H__
#define _PAINT_TRIANGLE_ZBASIC_BASE_H__

#include "screen.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cPaintTriZBasic;
typedef void(cPaintTriZBasic::*cPaintTriZBasic_vLineHAbs)(u16);

class cPaintTriZBasic
{
  public:
    cPaintTriZBasic_vLineHAbs vLineHAbs;

    typedef struct
    {
      float fx;
      float fiz;
    }tstScanline;

    typedef struct
    {
      typedef struct{
        float fx, fy, fiz;
      }tstData;

      tstData Dat[3];
      unBmBaseCol unCol;
    }tstTri;
    
    tstScanline*  mstLeft;
    tstScanline*  mstRight;

    tstTri  *mstTri;
    
    cScreen* mcScr;
    float*   mfzlfBuffer;

    cPaintTriZBasic(float* lzlfBuffer)
    {
      mcScr     = null;
      mstLeft   = null;
      mstRight  = null;
      vLineHAbs = null;
      mfzlfBuffer = lzlfBuffer;
    }

    ~cPaintTriZBasic()
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
            vLineHAbs =  &cPaintTriZBasic::BM_BPP16_5R6G5B_vLineHAbs;
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

        mstLeft[liGfxPosY1].fx = mstTri->Dat[lu8t1].fx;
        mstLeft[liGfxPosY1].fiz = mstTri->Dat[lu8t1].fiz;

        mstRight[liGfxPosY1].fx = mstTri->Dat[lu8t2].fx;
        mstRight[liGfxPosY1].fiz = mstTri->Dat[lu8t2].fiz;
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
          float lfiZ_inc = (mstTri->Dat[lu8t2].fiz - mstTri->Dat[lu8t1].fiz)    * lfdy;

          float lfiZ = mstTri->Dat[lu8t1].fiz;
          float lfX  = mstTri->Dat[lu8t1].fx;

          lfX     += (liGfxPosY1 - liGfxPosY1_Save) * lfX_inc;
          lfiZ    += (liGfxPosY1 - liGfxPosY1_Save) * lfiZ_inc;

          while (liGfxPosY1 <= liGfxPosY2)
          {
            lstMem[liGfxPosY1].fx  = lfX;
            lstMem[liGfxPosY1].fiz = lfiZ;

            lfX  += lfX_inc;
            lfiZ += lfiZ_inc;

            liGfxPosY1++;
          }
        }
      }
    }

    void BM_BPP16_5R6G5B_vLineHAbs(u16 lu16IdxY)
    {
      u16 lu16r, lu16g, lu16b;

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
          float lfz;

          float* lpu16StartZ = &mfzlfBuffer[lu16IdxY * mcScr->mpcBm->miGfxWidth + liGfxPoslfX1];

          u16*   lpu16Start = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX1, lu16IdxY, *mcScr->mpcBm);
          u16*   lpu16End = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX2, lu16IdxY, *mcScr->mpcBm);

          if (mstTri->unCol.stARGB.r == 0xFF) lu16r = 31;
                                         else lu16r = mstTri->unCol.stARGB.r >> 3;
          if (mstTri->unCol.stARGB.g == 0xFF) lu16g = 63;
                                         else lu16g = mstTri->unCol.stARGB.g >> 2;
          if (mstTri->unCol.stARGB.b == 0xFF) lu16b = 31;
                                         else lu16b = mstTri->unCol.stARGB.b >> 3;
										 
		      u16 lu16Col = BM_BPP16_5R6G5B_RGB(lu16r, lu16g, lu16b);
                  
          while (lpu16Start <= lpu16End)
          {
            lfz = 1.0f / lfiZ;

            if (lfz > *lpu16StartZ)
            {
              *lpu16StartZ = lfz;
              *lpu16Start = lu16Col;
            }

            lfiZ += lfiZ_inc;

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


#endif // _PAINT_TRIANGLE_ZBASIC_BASE_H__

