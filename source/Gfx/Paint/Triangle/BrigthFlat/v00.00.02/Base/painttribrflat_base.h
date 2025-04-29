#ifndef __PAINT_TRIANGLE_BRIGHTNESS_FLAT_BASE_H__
#define __PAINT_TRIANGLE_BRIGHTNESS_FLAT_BASE_H__

#include "screen.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cPaintTriBrigthFlat;
typedef void(cPaintTriBrigthFlat::*cPaintTriBrigthFlat_vLineHAbs)(u8,u8);

class cPaintTriBrigthFlat
{
  public:
    cPaintTriBrigthFlat_vLineHAbs vLineHAbs;

    typedef struct
    {
      float  fx;
      float  fyPt1;
      float  fyPt2;
      float lfX_inc;
    }tstScanline;


    typedef struct
    {
      typedef struct{
        float fx, fy;
      }tstData;

      u8      u8B;
      tstData Dat[3];
      unBmBaseCol unCol;
    }tstTri;

    tstScanline mstScanLine[3]; // 3 Linien zu je 2 Punkten(Start und Ende)
    
    tstTri  *mstTri;
    
    cScreen* mcScr;

    cPaintTriBrigthFlat()
    {
      mcScr     = null;
      vLineHAbs = null;
    }

    ~cPaintTriBrigthFlat()
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
            vLineHAbs =  &cPaintTriBrigthFlat::BM_BPP16_5R6G5B_vLineHAbs;
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
        mstScanLine[lu8IdxLine].lfX_inc     = 0.0f;
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
          float lfX  = mstTri->Dat[lu8IdxPt1].fx;
          lfX     += (liGfxPosY1 - liGfxPosY1_Save) * lfX_inc;
          mstScanLine[lu8IdxLine].fyPt1  = liGfxPosY1;
          mstScanLine[lu8IdxLine].fyPt2  = liGfxPosY2;
          mstScanLine[lu8IdxLine].fx     = lfX;
          mstScanLine[lu8IdxLine].lfX_inc      = lfX_inc;
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
      float lfXL     = mstScanLine[lu8IdxLeftLine].fx;
      // Auf die Startposition gehen
      lfXL     += (liGfxPosY1 - mstScanLine[lu8IdxLeftLine].fyPt1) * lfX_incL;
      //------------------------------

      float lfX_incR     = mstScanLine[lu8IdxRightLine].lfX_inc;
      float lfXR     = mstScanLine[lu8IdxRightLine].fx;

      // Auf die Startposition gehen
      lfXR     += (liGfxPosY1 - mstScanLine[lu8IdxRightLine].fyPt1) * lfX_incR;
      //-------------------------------------------------

      while (liGfxPosY1 <= liGfxPosY2)
      {
        u16 lu16r, lu16g, lu16b;

        GfxInt liGfxPoslfX1 = iGfxRound(lfXL);
        GfxInt liGfxPoslfX2 = iGfxRound(lfXR);

        GfxInt liGfxPoslfX1_Save = liGfxPoslfX1;

        if (mcScr->bHLineClipAbs(liGfxPoslfX1, liGfxPoslfX2, liGfxPosY1))
        {
          if ((liGfxPoslfX2 > liGfxPoslfX1))
          {
            u16    lu16B;
            u16*   lpu16Start = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX1, liGfxPosY1, *mcScr->mpcBm);
            u16*   lpu16End   = &BM_BPP16_5R6G5B_PXL(liGfxPoslfX2, liGfxPosY1, *mcScr->mpcBm);

            if (mstTri->unCol.stARGB.r == 0xFF) lu16r = 31;
                                            else lu16r = mstTri->unCol.stARGB.r >> 3;
            if (mstTri->unCol.stARGB.g == 0xFF) lu16g = 63;
                                            else lu16g = mstTri->unCol.stARGB.g >> 2;
            if (mstTri->unCol.stARGB.b == 0xFF) lu16b = 31;
                                            else lu16b = mstTri->unCol.stARGB.b >> 3;

            lu16B = (u16)mstTri->u8B;
            lu16r = (lu16r * lu16B) >> 8;
            lu16g = (lu16g * lu16B) >> 8;
            lu16b = (lu16b * lu16B) >> 8;

            lu16B = BM_BPP16_5R6G5B_RGB(lu16r, lu16g, lu16b);

            while (lpu16Start <= lpu16End)
            {
              *lpu16Start = lu16B;
              lpu16Start++;
            }
          }
        }
        lfXL  += lfX_incL;
        lfXR  += lfX_incR;
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


#endif // __PAINT_TRIANGLE_BRIGHTNESS_GRADIENT_BASE_H__

