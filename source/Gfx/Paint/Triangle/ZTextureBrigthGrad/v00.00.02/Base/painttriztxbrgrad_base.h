#ifndef __PAINT_TRIANGLE_Z_TElfXTURE_lfBRIGHTNESS_GRADIENT_BASE_H__
#define __PAINT_TRIANGLE_Z_TElfXTURE_lfBRIGHTNESS_GRADIENT_BASE_H__

#include "screen.h"
#include "math.h"

#ifdef __cplusplus
 extern "C" {
#endif

class cPaintTriZTxBrigthgrad;
typedef void(cPaintTriZTxBrigthgrad::*cPaintTriZTxBrigthgrad_vLineHAbs)(u8,u8);

class cPaintTriZTxBrigthgrad
{
  public:
    cPaintTriZTxBrigthgrad_vLineHAbs vLineHAbs;

    typedef struct
    {
      float  fx;
      float  fyPt1;
      float  fyPt2;

      float  fiz;

      float fiTexX;
      float fiTexY;
      u8    u8B;

      float lfX_inc;
      float lfb_inc;
      float lfiZ_inc;
      float lfiTexX_inc;
      float lfiTexY_inc;

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

    tstScanline mstScanLine[3]; // 3 Linien zu je 2 Punkten(Start und Ende)
    
    tstTri  *mstTri;
    
    cScreen* mcScr;
    float*   mfzlfBuffer;

    cPaintTriZTxBrigthgrad(float* lzlfBuffer)
    {
      mcScr     = null;
      vLineHAbs = null;
      mfzlfBuffer = lzlfBuffer;
    }

    ~cPaintTriZTxBrigthgrad()
    {
    }

    void vSetScreen(cScreen* lcScr)
    {
      bool lbGetMem;

      lbGetMem = False;

      if (mcScr != null)
      {
        if (mcScr->mpcBm != lcScr->mpcBm)
        {
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
      }
    }


    inline GfxInt iGfxRound(float lf)
    {
      if (lf > 0) return (GfxInt)(lf + 0.5f);
      return (GfxInt)(lf - 0.5f);
    }

    void vSetAndClipScanline(uint8 lu8IdxPt1, uint8 lu8IdxPt2, uint8 lu8IdxLine)
    {
      GfxInt liGfxPosY1   = iGfxRound(mstTri->Dat[lu8IdxPt1].fy);
      GfxInt liGfxPosY2   = iGfxRound(mstTri->Dat[lu8IdxPt2].fy);

      mstScanLine[lu8IdxLine].fyPt1  = -1.0f;
      mstScanLine[lu8IdxLine].fyPt2  = -1.0f;

      if (liGfxPosY1 == liGfxPosY2)
      {
        if (!mcScr->isInY1Abs(liGfxPosY1)) return;
        if (!mcScr->isInY2Abs(liGfxPosY1)) return;
        if (!mcScr->isInX1Abs((GfxInt)mstTri->Dat[lu8IdxPt1].fx)) return;
        if (!mcScr->isInX2Abs((GfxInt)mstTri->Dat[lu8IdxPt2].fx)) return;

        mstScanLine[lu8IdxLine].fx     = mstTri->Dat[lu8IdxPt1].fx;
        mstScanLine[lu8IdxLine].fyPt1  = mstTri->Dat[lu8IdxPt1].fy;
        mstScanLine[lu8IdxLine].fyPt2  = mstTri->Dat[lu8IdxPt2].fy;

        mstScanLine[lu8IdxLine].u8B    = mstTri->Dat[lu8IdxPt1].u8B;
        mstScanLine[lu8IdxLine].fiz    = mstTri->Dat[lu8IdxPt1].fiz;
        mstScanLine[lu8IdxLine].fiTexX = mstTri->Dat[lu8IdxPt1].stTex.fx * mstTri->Dat[lu8IdxPt1].fiz;
        mstScanLine[lu8IdxLine].fiTexY = mstTri->Dat[lu8IdxPt1].stTex.fy * mstTri->Dat[lu8IdxPt1].fiz;

        mstScanLine[lu8IdxLine].lfX_inc     = 0.0f;
        mstScanLine[lu8IdxLine].lfb_inc     = 0.0f;
        mstScanLine[lu8IdxLine].lfiZ_inc    = 0.0f;
        mstScanLine[lu8IdxLine].lfiTexX_inc = 0.0f;
        mstScanLine[lu8IdxLine].lfiTexY_inc = 0.0f;
      }
      else
      {
        GfxInt liGfxPoslfX1 = iGfxRound(mstTri->Dat[lu8IdxPt1].fx);
        GfxInt liGfxPoslfX2 = iGfxRound(mstTri->Dat[lu8IdxPt2].fx);

        float lfdy = (float)(liGfxPosY2 - liGfxPosY1);
        lfdy = 1.0f / lfdy;

        GfxInt liGfxPosY1_Save = liGfxPosY1;

        float lfX_inc = (liGfxPoslfX2 - liGfxPoslfX1)                   * lfdy;

        // Hier clipping nur an Y. Clipping an X wird später in vLineHAbs gemacht
        if (mcScr->bLineClipYAbs(liGfxPoslfX1, liGfxPosY1, liGfxPoslfX2, liGfxPosY2))
        {
          float lfb_inc     = (mstTri->Dat[lu8IdxPt2].u8B - mstTri->Dat[lu8IdxPt1].u8B)    * lfdy;
          float lfiZ_inc    = (mstTri->Dat[lu8IdxPt2].fiz - mstTri->Dat[lu8IdxPt1].fiz)    * lfdy;
          float lfiTexX_inc = (mstTri->Dat[lu8IdxPt2].stTex.fx * mstTri->Dat[lu8IdxPt2].fiz - mstTri->Dat[lu8IdxPt1].stTex.fx * mstTri->Dat[lu8IdxPt1].fiz) * lfdy;
          float lfiTexY_inc = (mstTri->Dat[lu8IdxPt2].stTex.fy * mstTri->Dat[lu8IdxPt2].fiz - mstTri->Dat[lu8IdxPt1].stTex.fy * mstTri->Dat[lu8IdxPt1].fiz) * lfdy;

          float lfB  = mstTri->Dat[lu8IdxPt1].u8B;
          float lfiZ = mstTri->Dat[lu8IdxPt1].fiz;
          float lfX  = mstTri->Dat[lu8IdxPt1].fx;
          float lfiTexX = mstTri->Dat[lu8IdxPt1].stTex.fx * lfiZ;
          float lfiTexY = mstTri->Dat[lu8IdxPt1].stTex.fy * lfiZ;

          lfB     += (liGfxPosY1 - liGfxPosY1_Save) * lfb_inc;
          lfX     += (liGfxPosY1 - liGfxPosY1_Save) * lfX_inc;
          lfiZ    += (liGfxPosY1 - liGfxPosY1_Save) * lfiZ_inc;
          lfiTexX += (liGfxPosY1 - liGfxPosY1_Save) * lfiTexX_inc;
          lfiTexY += (liGfxPosY1 - liGfxPosY1_Save) * lfiTexY_inc;

          mstScanLine[lu8IdxLine].fyPt1  = liGfxPosY1;
          mstScanLine[lu8IdxLine].fyPt2  = liGfxPosY2;
          mstScanLine[lu8IdxLine].fx     = lfX;

          mstScanLine[lu8IdxLine].u8B    = (u8)lfB;
          mstScanLine[lu8IdxLine].fiz    = lfiZ;
          mstScanLine[lu8IdxLine].fiTexX = lfiTexX;
          mstScanLine[lu8IdxLine].fiTexY = lfiTexY;

          mstScanLine[lu8IdxLine].lfX_inc      = lfX_inc;
          mstScanLine[lu8IdxLine].lfb_inc      = lfb_inc;
          mstScanLine[lu8IdxLine].lfiZ_inc     = lfiZ_inc;
          mstScanLine[lu8IdxLine].lfiTexX_inc  = lfiTexX_inc;
          mstScanLine[lu8IdxLine].lfiTexY_inc  = lfiTexY_inc;
        }
      }
    }

    void BM_BPP16_5R6G5B_vLineHAbs(uint8 lu8IdxLeftLine, uint8 lu8IdxRightLine)
    {
      if ((GfxInt)mstScanLine[lu8IdxLeftLine].fyPt1  < 0) return;
      if ((GfxInt)mstScanLine[lu8IdxRightLine].fyPt1 < 0) return;
      
      GfxInt liGfxPosY1   = (GfxInt)mstScanLine[lu8IdxLeftLine].fyPt1;
      GfxInt liGfxPosY2   = (GfxInt)mstScanLine[lu8IdxLeftLine].fyPt2;

      if (liGfxPosY1 < (GfxInt)mstScanLine[lu8IdxRightLine].fyPt1) liGfxPosY1 = (GfxInt)mstScanLine[lu8IdxRightLine].fyPt1;
      if (liGfxPosY2 > (GfxInt)mstScanLine[lu8IdxRightLine].fyPt2) liGfxPosY2 = (GfxInt)mstScanLine[lu8IdxRightLine].fyPt2;

      float lfX_incL     = mstScanLine[lu8IdxLeftLine].lfX_inc;
      float lfb_incL     = mstScanLine[lu8IdxLeftLine].lfb_inc;
      float lfiZ_incL    = mstScanLine[lu8IdxLeftLine].lfiZ_inc;
      float lfiTexX_incL = mstScanLine[lu8IdxLeftLine].lfiTexX_inc;
      float lfiTexY_incL = mstScanLine[lu8IdxLeftLine].lfiTexY_inc;

      float lfXL     = mstScanLine[lu8IdxLeftLine].fx;
      float lfBL     = mstScanLine[lu8IdxLeftLine].u8B;
      float lfiZL    = mstScanLine[lu8IdxLeftLine].fiz;
      float lfiTexXL = mstScanLine[lu8IdxLeftLine].fiTexX;
      float lfiTexYL = mstScanLine[lu8IdxLeftLine].fiTexY;

      // Auf die Startposition gehen
      lfBL     += (liGfxPosY1 - mstScanLine[lu8IdxLeftLine].fyPt1) * lfb_incL;
      lfXL     += (liGfxPosY1 - mstScanLine[lu8IdxLeftLine].fyPt1) * lfX_incL;
      lfiZL    += (liGfxPosY1 - mstScanLine[lu8IdxLeftLine].fyPt1) * lfiZ_incL;
      lfiTexXL += (liGfxPosY1 - mstScanLine[lu8IdxLeftLine].fyPt1) * lfiTexX_incL;
      lfiTexYL += (liGfxPosY1 - mstScanLine[lu8IdxLeftLine].fyPt1) * lfiTexY_incL;

      //------------------------------

      float lfX_incR     = mstScanLine[lu8IdxRightLine].lfX_inc;
      float lfb_incR     = mstScanLine[lu8IdxRightLine].lfb_inc;
      float lfiZ_incR    = mstScanLine[lu8IdxRightLine].lfiZ_inc;
      float lfiTexX_incR = mstScanLine[lu8IdxRightLine].lfiTexX_inc;
      float lfiTexY_incR = mstScanLine[lu8IdxRightLine].lfiTexY_inc;

      float lfXR     = mstScanLine[lu8IdxRightLine].fx;
      float lfBR     = mstScanLine[lu8IdxRightLine].u8B;
      float lfiZR    = mstScanLine[lu8IdxRightLine].fiz;
      float lfiTexXR = mstScanLine[lu8IdxRightLine].fiTexX;
      float lfiTexYR = mstScanLine[lu8IdxRightLine].fiTexY;

      // Auf die Startposition gehen
      lfBR     += (liGfxPosY1 - mstScanLine[lu8IdxRightLine].fyPt1) * lfb_incR;
      lfXR     += (liGfxPosY1 - mstScanLine[lu8IdxRightLine].fyPt1) * lfX_incR;
      lfiZR    += (liGfxPosY1 - mstScanLine[lu8IdxRightLine].fyPt1) * lfiZ_incR;
      lfiTexXR += (liGfxPosY1 - mstScanLine[lu8IdxRightLine].fyPt1) * lfiTexX_incR;
      lfiTexYR += (liGfxPosY1 - mstScanLine[lu8IdxRightLine].fyPt1) * lfiTexY_incR;

      //-------------------------------------------------

      while (liGfxPosY1 <= liGfxPosY2)
      {
        {
          u16 lu16r, lu16g, lu16b;

          GfxInt liGfxTexX;
          GfxInt liGfxTexY;

          GfxInt liGfxPoslfX1 = iGfxRound(lfXL);
          GfxInt liGfxPoslfX2 = iGfxRound(lfXR);

          float dx = (float)(liGfxPoslfX2 - liGfxPoslfX1);
          if (liGfxPoslfX2 != liGfxPoslfX1)
            dx = 1.0f / dx;
          else
            dx = 0.0f;

          GfxInt liGfxPoslfX1_Save = liGfxPoslfX1;

          if (mcScr->bHLineClipAbs(liGfxPoslfX1, liGfxPoslfX2, liGfxPosY1))
          {
            float lfiZ_inc = (lfiZR - lfiZL)    * dx;
            float lfiZ     =  lfiZL + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfiZ_inc;

            if ((liGfxPoslfX2 > liGfxPoslfX1))
            {
              float lfb_inc =     (lfBR     - lfBL)     * dx;
              float lfiTexX_inc = (lfiTexXR - lfiTexXL) * dx;
              float lfiTexY_inc = (lfiTexYR - lfiTexYL) * dx;

              // Falls geclippt wurde, auf die Start setzen
              float lfz;
              float lfB     = lfBL     + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfb_inc;
              float lfiTexX = lfiTexXL + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfiTexX_inc;
              float lfiTexY = lfiTexYL + (liGfxPoslfX1 - liGfxPoslfX1_Save) * lfiTexY_inc;

              float* lpu16StartZ = &mfzlfBuffer[liGfxPosY1 * mcScr->mpcBm->miGfxWidth + liGfxPoslfX1];
              u16    lu16B;

              u16*   lpu16Start = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX1, liGfxPosY1, *mcScr->mpcBm);
              u16*   lpu16End   = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX2, liGfxPosY1, *mcScr->mpcBm);

              while (lpu16Start <= lpu16End)
              {
                lfz = 1.0f / lfiZ;

                liGfxTexX = (GfxInt)(lfz * lfiTexX);
                liGfxTexY = (GfxInt)(lfz * lfiTexY);

                // Soll zBuffer-Fighting verringern.
                // Aktuellen z-Wert lesen und schauen, ob der neue z-Wert 0.1 näher ist
                if ((lfz - 0.1f) > *lpu16StartZ)
                {
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

                lpu16Start++;  // Position Bildschirm
                lpu16StartZ++; // Position zBuffer
              }
            }
          }
        }

        lfBL  += lfb_incL;
        lfXL  += lfX_incL;
        lfiZL += lfiZ_incL;
        lfiTexXL += lfiTexX_incL;
        lfiTexYL += lfiTexY_incL;

        lfBR  += lfb_incR;
        lfXR  += lfX_incR;
        lfiZR += lfiZ_incR;
        lfiTexXR += lfiTexX_incR;
        lfiTexYR += lfiTexY_incR;

        liGfxPosY1++;
      }
    }

    void vSortX(uint8 lu8t1, uint8 lu8t2)
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

      liGfxMinY = -1;
      liGfxMaxY = -1;

      mstTri = lcTri;

      for (t = 0; t < 3; t++)
      {
        mstTri->Dat[t].fx = iGfxRound(mstTri->Dat[t].fx + mcScr->miGfxRefPosx1);
        mstTri->Dat[t].fy = iGfxRound(mstTri->Dat[t].fy + mcScr->miGfxRefPosy1);
      }

      vSortY(1,2);
      vSortY(0,1);
      vSortY(1,2);

      if (mstTri->Dat[0].fy == mstTri->Dat[2].fy)
      {
        /*  0 ----- 1 ----- 2
               (0)     (1)
        */
        vSortX(1,2);
        vSortX(0,1);
        vSortX(1,2);

        vSetAndClipScanline(0, 2, 0);
        vSetAndClipScanline(2, 0, 2);

        (this->*vLineHAbs)(0, 2);
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
          vSortX(0, 1);


          /*     (0)
              0-------1
               \     /
            (1) \   / (2)
                 \ /
                  2   */

          vSetAndClipScanline(0, 2, 1);
          vSetAndClipScanline(1, 2, 2);

          (this->*vLineHAbs)(1, 2);
        }
        else
        if (mstTri->Dat[1].fy == mstTri->Dat[2].fy)
        {
          /*     0          0   
                / \   =>   / \  
               /   \  =>  /   \ 
              2-----1    1-----2 */
          vSortX(1, 2);

          /*      0   
                 / \  
            (1) /   \ (2)
               /     \ 
              1-------2  
                  (0)
                 */

          vSetAndClipScanline(0, 1, 1);
          vSetAndClipScanline(0, 2, 2);

          (this->*vLineHAbs)(1, 2);
        }
        else
        {
          if (mstTri->Dat[1].fx < mstTri->Dat[0].fx)
          {
             /*      0 
                   / |
              (0) /  |
                 /   |
                1    | (2)
                 \   |
              (1) \  |
                   \ |
                     2  */

            vSetAndClipScanline(0, 1, 0);
            vSetAndClipScanline(1, 2, 1);
            vSetAndClipScanline(0, 2, 2);

            (this->*vLineHAbs)(0, 2);
            (this->*vLineHAbs)(1, 2);
          }
          else
          {
            /*  0 
                | \
                |  \ (0)
                |   \
            (2) |    1
                |   /
                |  / (1)
                | /
                2     */

            vSetAndClipScanline(0, 1, 0);
            vSetAndClipScanline(1, 2, 1);
            vSetAndClipScanline(0, 2, 2);

            (this->*vLineHAbs)(2, 0);
            (this->*vLineHAbs)(2, 1);
          }
        }
      }
    }
};


#ifdef __cplusplus
}
#endif


#endif // __PAINT_TRIANGLE_Z_TElfXTURE_lfBRIGHTNESS_GRADIENT_BASE_H__

