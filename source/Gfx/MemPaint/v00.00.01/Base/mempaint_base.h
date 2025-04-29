#ifndef _MEMPAINT_BASE_H__
#define _MEMPAINT_BASE_H__

#include "screen.h"


#ifdef __cplusplus
 extern "C" {
#endif

class cMemPaint
{
  public:
    
    GfxInt*  mi16Left;
    GfxInt*  mi16Right;

    cScreen* mcScr;

    cMemPaint()
    {
      mcScr     = null;
      mi16Left  = null;
      mi16Right = null;
   }

    ~cMemPaint()
    {
      vFreeMem();
    }

    void vSetScreen(cScreen* lcScr)
    {
      if (mcScr != null)
      {
        if (mcScr->mpcBm != lcScr->mpcBm)
        {
          vFreeMem();
          mcScr = lcScr;

          mi16Left = (GfxInt*) new GfxInt[lcScr->mpcBm->miGfxHeight];
          mi16Right = (GfxInt*) new GfxInt[lcScr->mpcBm->miGfxHeight];
        }
      }
      else
      {
        mcScr = lcScr;

        mi16Left = (GfxInt*) new GfxInt[lcScr->mpcBm->miGfxHeight];
        mi16Right = (GfxInt*) new GfxInt[lcScr->mpcBm->miGfxHeight];
      }
    }

    void vFreeMem()
    {
      if (mi16Left != null)  delete[]mi16Left;
      if (mi16Right != null) delete[]mi16Right;
    }

    // Zeichnet eine horizontale Linie
    SCREEN_INLINE void vLineHAbsRaw(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, GfxInt* mi16Mem)
    {
      UNUSED(liGfxX2);
      mi16Mem[liGfxY1] = liGfxX1;
    }


    // Zeichnet eine vertikale Linie
    void vLineVAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, GfxInt* mi16Mem)
    {
      //if (bChkInit())
      {
        while (liGfxY1 <= liGfxY2)
        {
          mi16Mem[liGfxY1] = liGfxX1;
          liGfxY1++;
        }
      }
    }

    /* http://members.chello.at/~easyfilter/bresenham.html */
    void vLineAbsRaw(GfxInt liGfxX1, GfxInt liGfxY1,
                     GfxInt liGfxX2, GfxInt liGfxY2,
                     GfxInt* mi16Mem)
    {
      if (mi16Mem == mi16Right)
      {
        if (liGfxX1 > liGfxX2)
        {
          GfxInt liGfxTemp;

          liGfxTemp = liGfxX1;
          liGfxX1   = liGfxX2;
          liGfxX2   = liGfxTemp;

          liGfxTemp = liGfxY1;
          liGfxY1   = liGfxY2;
          liGfxY2   = liGfxTemp;
        }

        // x2>x1

        GfxInt liGfxdx =  liGfxX2-liGfxX1;

        GfxInt liGfxdy = -mcScr->mpcBm->iGfxAbs(liGfxY2-liGfxY1);
        GfxInt liGfxsy = (liGfxY1 < liGfxY2) ? 1 : -1; 

        GfxInt liGfxXerr = liGfxdx + liGfxdy;
        GfxInt liGfxXe2; /* error value e_xy */

        for(;;)
        {  /* loop */
          mi16Mem[liGfxY1] = liGfxX1;

          if ((liGfxX1 == liGfxX2) && (liGfxY1 == liGfxY2)) break;

          liGfxXe2 = 2 * liGfxXerr;

          /* e_xy+e_x > 0 */
          if (liGfxXe2 >= liGfxdy) 
          { 
            liGfxXerr += liGfxdy;
            liGfxX1++; 
          } 

          /* e_xy+e_y < 0 */
          if (liGfxXe2 <= liGfxdx) 
          { 
            liGfxXerr += liGfxdx; 
            liGfxY1   += liGfxsy;
          }
        }
      }
      else
      {
        if (liGfxX1 < liGfxX2)
        {
          GfxInt liGfxTemp;

          liGfxTemp = liGfxX1;
          liGfxX1   = liGfxX2;
          liGfxX2   = liGfxTemp;

          liGfxTemp = liGfxY1;
          liGfxY1   = liGfxY2;
          liGfxY2   = liGfxTemp;
        }

        // x1>x2

        GfxInt liGfxdx =  liGfxX1-liGfxX2;

        GfxInt liGfxdy = -mcScr->mpcBm->iGfxAbs(liGfxY2-liGfxY1);
        GfxInt liGfxsy = (liGfxY1 < liGfxY2) ? 1 : -1; 

        GfxInt liGfxXerr = liGfxdx + liGfxdy;
        GfxInt liGfxXe2; /* error value e_xy */

        for(;;)
        {  /* loop */
          mi16Mem[liGfxY1] = liGfxX1;

          if ((liGfxX1 == liGfxX2) && (liGfxY1 == liGfxY2)) break;

          liGfxXe2 = 2 * liGfxXerr;

          /* e_xy+e_x > 0 */
          if (liGfxXe2 >= liGfxdy) 
          { 
            liGfxXerr += liGfxdy;
            liGfxX1--; 
          } 

          /* e_xy+e_y < 0 */
          if (liGfxXe2 <= liGfxdx) 
          { 
            liGfxXerr += liGfxdx; 
            liGfxY1   += liGfxsy;
          }
        }
      }
   }

  
    // Zeichnet eine horizontale Linie
    SCREEN_INLINE void vLineHAbs(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, GfxInt* mi16Mem)
    {
      //if (bChkInit())
      {
        if (mcScr->bHLineClipAbs(liGfxX1, liGfxX2, liGfxY1))
        {
          vLineHAbsRaw(liGfxX1, liGfxX2, liGfxY1, mi16Mem);
        }
      }
    }

    SCREEN_INLINE void vLineH(GfxInt liGfxX1, GfxInt liGfxX2, GfxInt liGfxY1, GfxInt* mi16Mem)
    {
      mcScr->vRel2AbsX(liGfxX1);
      mcScr->vRel2AbsX(liGfxX2);
      mcScr->vRel2AbsY(liGfxY1);

      vLineHAbs(liGfxX1, liGfxX2, liGfxY1, mi16Mem);
    }


    // Zeichnet eine vertikale Linie
    SCREEN_INLINE void vLineVAbs(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, GfxInt* mi16Mem)
    {
      //if (bChkInit())
      {
        mcScr->vSort(liGfxY1, liGfxY2);

        if (!mcScr->isInY1Abs(liGfxY2)) return;
        if (!mcScr->isInY2Abs(liGfxY1)) return;

        mcScr->vClipY1Abs(liGfxY1);
        mcScr->vClipY2Abs(liGfxY2);

        vLineVAbsRaw(liGfxX1, liGfxY1, liGfxY2, mi16Mem);
      }
    }

    SCREEN_INLINE void vLineV(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxY2, GfxInt* mi16Mem)
    {
      mcScr->vRel2AbsX(liGfxX1);
      mcScr->vRel2AbsY(liGfxY1);
      mcScr->vRel2AbsY(liGfxY2);

      vLineVAbs(liGfxX1, liGfxY1, liGfxY2, mi16Mem);
    }


    // Zeichnet eine Linie auf 
    SCREEN_INLINE void vLineAbs(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxX2, GfxInt liGfxY2, GfxInt* mi16Mem)
    {
      //if (bChkInit())
      {
        if (liGfxX1 == liGfxX2)
        {
          vLineVAbs(liGfxX1, liGfxY1, liGfxY2, mi16Mem);
        }
        else if (liGfxY1 == liGfxY2)
        {
          vLineHAbs(liGfxX1, liGfxX2, liGfxY1, mi16Mem);
        }
        else
        {
          if (mcScr->bLineClipYAbs(liGfxX1, liGfxY1, liGfxX2, liGfxY2))
          {
            vLineAbsRaw(liGfxX1, liGfxY1, liGfxX2, liGfxY2, mi16Mem);
          }
        }
      }
    }


    SCREEN_INLINE void vLine(GfxInt liGfxX1, GfxInt liGfxY1, GfxInt liGfxX2, GfxInt liGfxY2, GfxInt* mi16Mem)
    {
      if (liGfxX1 == liGfxX2)
      {
        vLineV(liGfxX1, liGfxY1, liGfxY2, mi16Mem);
      }
      else if (liGfxY1 == liGfxY2)
      {
        vLineH(liGfxX1, liGfxX2, liGfxY1, mi16Mem);
      }
      else
      {
        mcScr->vRel2Abs(liGfxX1, liGfxY1, liGfxX2, liGfxY2);
        if (mcScr->bLineClipYAbs(liGfxX1, liGfxY1, liGfxX2, liGfxY2))
        {
          vLineAbsRaw(liGfxX1, liGfxY1, liGfxX2, liGfxY2, mi16Mem);
        }
      }
    }

    SCREEN_INLINE void vSortY(GfxInt &liGfxx1, GfxInt &liGfxy1,
                              GfxInt &liGfxx2, GfxInt &liGfxy2,
                              GfxInt &liGfxx3, GfxInt &liGfxy3)
    {
      GfxInt liGfxTemp; 

      if (liGfxy3 < liGfxy2)
      {
        liGfxTemp = liGfxx2;
        liGfxx2   = liGfxx3;
        liGfxx3   = liGfxTemp;

        liGfxTemp = liGfxy2;
        liGfxy2   = liGfxy3;
        liGfxy3   = liGfxTemp;
      }

      if (liGfxy2 < liGfxy1)
      {
        liGfxTemp = liGfxx1;
        liGfxx1   = liGfxx2;
        liGfxx2   = liGfxTemp;

        liGfxTemp = liGfxy1;
        liGfxy1   = liGfxy2;
        liGfxy2   = liGfxTemp;
      }

      if (liGfxy3 < liGfxy2)
      {
        liGfxTemp = liGfxx2;
        liGfxx2 = liGfxx3;
        liGfxx3 = liGfxTemp;

        liGfxTemp = liGfxy2;
        liGfxy2 = liGfxy3;
        liGfxy3 = liGfxTemp;
      }
    }

    SCREEN_INLINE void vSortX(GfxInt& liGfxx1, GfxInt& liGfxy1,
                              GfxInt& liGfxx2, GfxInt& liGfxy2,
                              GfxInt& liGfxx3, GfxInt& liGfxy3)
    {
      GfxInt liGfxTemp;

      if (liGfxx3 < liGfxx2)
      {
        liGfxTemp = liGfxx2;
        liGfxx2 = liGfxx3;
        liGfxx3 = liGfxTemp;

        liGfxTemp = liGfxy2;
        liGfxy2 = liGfxy3;
        liGfxy3 = liGfxTemp;
      }

      if (liGfxx2 < liGfxx1)
      {
        liGfxTemp = liGfxx1;
        liGfxx1 = liGfxx2;
        liGfxx2 = liGfxTemp;

        liGfxTemp = liGfxy1;
        liGfxy1 = liGfxy2;
        liGfxy2 = liGfxTemp;
      }

      if (liGfxx3 < liGfxx2)
      {
        liGfxTemp = liGfxx2;
        liGfxx2 = liGfxx3;
        liGfxx3 = liGfxTemp;

        liGfxTemp = liGfxy2;
        liGfxy2 = liGfxy3;
        liGfxy3 = liGfxTemp;
      }
    }

    // Zeichnet ein ausgefülltes Dreieck auf Schirm
    void vTriangle(GfxInt liGfxx1, GfxInt liGfxy1,
                   GfxInt liGfxx2, GfxInt liGfxy2,
                   GfxInt liGfxx3, GfxInt liGfxy3,
                   uint32 lui32Col)
    {
      GfxInt liGfxMinY;
      GfxInt liGfxMaxY;
      GfxInt liGfxi;

      liGfxMinY = -1;
      liGfxMaxY = -1;

      vSortY(liGfxx1, liGfxy1,
             liGfxx2, liGfxy2,
             liGfxx3, liGfxy3);

      mcScr->vRel2Abs(liGfxx1, liGfxy1);
      mcScr->vRel2Abs(liGfxx2, liGfxy2);
      mcScr->vRel2Abs(liGfxx3, liGfxy3);

      if (liGfxy1 == liGfxy3)
      {
        mcScr->vLineAbs(liGfxx1, liGfxy1, liGfxx3, liGfxy3, lui32Col);
      }
      else
      {
        liGfxMinY = liGfxy1;
        if (liGfxMinY < mcScr->miGfxClipPosy1) liGfxMinY = mcScr->miGfxClipPosy1;

        liGfxMaxY = liGfxy3;
        if (liGfxMaxY > mcScr->miGfxClipPosy2) liGfxMaxY = mcScr->miGfxClipPosy2;

        for (liGfxi = liGfxMinY; liGfxi <= liGfxMaxY; liGfxi++)
        {
          mi16Left[liGfxi] =  mcScr->miGfxClipPosx1;
          mi16Right[liGfxi] = mcScr->miGfxClipPosx2;
        }

        /*
        MEMTOOLS_vMemSet(mi16Left, lui32Idx, lui32Col, liGfxX2 - liGfxX1 + 1);
        MEMTOOLS_vMemSet(mi16Right, lui32Idx, lui32Col, liGfxX2 - liGfxX1 + 1);
        */

        if (liGfxy1 == liGfxy2)
        {
          if (liGfxx1 < liGfxx2)
          {
            vLineAbs(liGfxx1, liGfxy1, liGfxx3, liGfxy3, mi16Left);
            vLineAbs(liGfxx2, liGfxy2, liGfxx3, liGfxy3, mi16Right);

            if ((liGfxx1 <= mcScr->miGfxClipPosx2) &&
                (liGfxx2 >= mcScr->miGfxClipPosx1))
            {
              mi16Left[liGfxy1] = liGfxx1;
              if  (mi16Left[liGfxy1] < mcScr->miGfxClipPosx1) mi16Left[liGfxy1] = mcScr->miGfxClipPosx1;
              mi16Right[liGfxy1] = liGfxx2;
              if (mi16Right[liGfxy1] > mcScr->miGfxClipPosx2) mi16Right[liGfxy1] = mcScr->miGfxClipPosx2;
            }
          }
          else
          {
            vLineAbs(liGfxx1, liGfxy1, liGfxx3, liGfxy3, mi16Right);
            vLineAbs(liGfxx2, liGfxy2, liGfxx3, liGfxy3, mi16Left);

            if ((liGfxx1 <= mcScr->miGfxClipPosx2) &&
              (liGfxx2 >= mcScr->miGfxClipPosx1))
            {
              mi16Left[liGfxy1] = liGfxx2;
              if (mi16Left[liGfxy1] < mcScr->miGfxClipPosx1) mi16Left[liGfxy1] = mcScr->miGfxClipPosx1;
              mi16Right[liGfxy1] = liGfxx1;
              if (mi16Right[liGfxy1] > mcScr->miGfxClipPosx2) mi16Right[liGfxy1] = mcScr->miGfxClipPosx2;
            }
          }
        }
        else
        if (liGfxy2 == liGfxy3)
        {
          if (liGfxx2 < liGfxx3)
          {
            vLineAbs(liGfxx1, liGfxy1, liGfxx2, liGfxy2, mi16Left);
            vLineAbs(liGfxx1, liGfxy1, liGfxx3, liGfxy3, mi16Right);

            if ((liGfxx2 <= mcScr->miGfxClipPosx2) &&
              (liGfxx3 >= mcScr->miGfxClipPosx1))
            {
              mi16Left[liGfxy2] = liGfxx2;
              if (mi16Left[liGfxy2] < mcScr->miGfxClipPosx1) mi16Left[liGfxy2] = mcScr->miGfxClipPosx1;
              mi16Right[liGfxy2] = liGfxx3;
              if (mi16Right[liGfxy2] > mcScr->miGfxClipPosx2) mi16Right[liGfxy2] = mcScr->miGfxClipPosx2;
            }
          }
          else
          {
            vLineAbs(liGfxx1, liGfxy1, liGfxx2, liGfxy2, mi16Right);
            vLineAbs(liGfxx1, liGfxy1, liGfxx3, liGfxy3, mi16Left);

            if ((liGfxx2 <= mcScr->miGfxClipPosx2) &&
              (liGfxx3 >= mcScr->miGfxClipPosx1))
            {
              mi16Left[liGfxy2] = liGfxx3;
              if (mi16Left[liGfxy2] < mcScr->miGfxClipPosx1) mi16Left[liGfxy2] = mcScr->miGfxClipPosx1;
              mi16Right[liGfxy2] = liGfxx2;
              if (mi16Right[liGfxy2] > mcScr->miGfxClipPosx2) mi16Right[liGfxy2] = mcScr->miGfxClipPosx2;
            }
          }
        }
        else
        {

          int32 lim12 = ((liGfxx2 - liGfxx1) * 65536) / (liGfxy2 - liGfxy1);
          int32 lim13 = ((liGfxx3 - liGfxx1) * 65536) / (liGfxy3 - liGfxy1);

          if (lim12 < lim13)
          {
            vLineAbs(liGfxx1, liGfxy1, liGfxx2, liGfxy2, mi16Left);
            vLineAbs(liGfxx2, liGfxy2, liGfxx3, liGfxy3, mi16Left);
            vLineAbs(liGfxx1, liGfxy1, liGfxx3, liGfxy3, mi16Right);

            if ((liGfxy2 >= liGfxMinY) && (liGfxy2 <= liGfxMaxY))
            {
              if (mi16Left[liGfxy2] > mi16Right[liGfxy2]) mi16Right[liGfxy2] = mi16Left[liGfxy2];
            }
          }
          else
          {
            vLineAbs(liGfxx1, liGfxy1, liGfxx2, liGfxy2, mi16Right);
            vLineAbs(liGfxx2, liGfxy2, liGfxx3, liGfxy3, mi16Right);
            vLineAbs(liGfxx1, liGfxy1, liGfxx3, liGfxy3, mi16Left);

            if ((liGfxy2 >= liGfxMinY) && (liGfxy2 <= liGfxMaxY))
            {
              if (mi16Left[liGfxy2] > mi16Right[liGfxy2]) mi16Right[liGfxy2] = mi16Left[liGfxy2];
            }
          }
        }

        for (liGfxi = liGfxMinY; liGfxi <= liGfxMaxY; liGfxi++)
        {
          mcScr->vLineHAbs(mi16Left[liGfxi], mi16Right[liGfxi], liGfxi, lui32Col);
        }

      }
    }

};


#ifdef __cplusplus
}
#endif


#endif // _MEMPAINT_BASE_H__

