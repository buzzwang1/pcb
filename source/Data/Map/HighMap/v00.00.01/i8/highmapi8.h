#ifndef __HIGHMAP_I8_H__
#define __HIGHMAP_I8_H__

/* Includes ------------------------------------------------------------------*/
#include "stdlib.h" // for rand()
#include "math.h"   // for round()
#include "highmap_base.h"

/* --- define HM_I8 --- */
#define HM_I8_IDX(liGfxX, liGfxY, lcHm)   (u32)((liGfxY) * lcHm.miGfxWidth + (liGfxX))
#define HM_I8_BM_SIZE(lcHm)               (u32)((lcHm.miGfxWidth * lcHm.miGfxHeight) * 2)

#define HM_I8_CHECK(lcHm)                 if ((lcHm.mui8Type & 127) != HM_I8) return 2;
#define HM_I8_MEM(li32Index,      lcHm)   (((uint16*)((lcHm).mpui8Data))[(li32Index)])
#define HM_I8_POS(liGfxX, liGfxY, lcHm)   HM_I8_MEM(BM_IDX((liGfxX), (liGfxY), (lcHm)), (lcHm))
#define HM_I8_POSIDX2BYTEPOS(lui32Index)  (lui32Index  * 2)

class cHighmapi8 : public cHighmap
{
  public:
    class cMidStep
    {
      public:
      cHighmapi8* mcHM;

      i16 x, y;
      i16 hx = 0, hy = 0;
      i16 Dim, d1, d2;
      i16 delta;
      i16 delta5;


      cMidStep(cHighmapi8* lcHM)
      {
        mcHM = lcHM;
      }

      i16 i16NormRand(i16 factor)
      {
        if (factor > 0)
        {
          factor++;
          return ((2 * (rand() % factor) + (rand() % factor) + (rand() % factor)) - (3 * (rand() % (factor - 1))) / 32);
        }
        else
        {
          return 0;
        }
      }

      i16 Arithm3(i16 a, i16 b, i16 c)
      {
        i16 e;

        e = (a + b + c) / 3 + i16NormRand(delta);

        if (e > 127)
        {
          e = 127;
        }
        else
        {
          if (e < -128)
          {
            e = 128;
          }
        }
        return e;
      }

      i16 Arithm4(i16 a, i16 b, i16 c, i16 d)
      {
        i16 e;

        e = (a + b + c + d) / 4 + i16NormRand(delta);

        if (e > 127)
        {
          e = 127;
        }
        else
          if (e < -128)
          {
            e = 128;
          }
        return e;
      }


      void vDiagMid(i16 a, i16 b)
      {
        x = a;
        hx = x + d2;
        while ((x <= (Dim - d2)) && (hx <= Dim))
        {
          y = b;
          hy = y + d2;
          while (y <= (Dim - d2))
          {
            mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, y))] =
              (i8)Arithm4(mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, hy))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, y - d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(hx, y))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x - d2, y))]);
            y += d1;
            hy = y + d2;
          }
          x += d1;
          hx = x + d2;
        }
      }

      void vMidPointStep(i8 ldelta, i8 step, float frakdim, bool newborder)
      {
        delta = ldelta;
        Dim = 128;
        d1 = Dim >> (step - 1);
        d2 = d1 / 2;

        frakdim = (3 - frakdim) / 2;
        delta = (i8)roundf(delta * expf(-0.6931f * frakdim * (2 * (step - 1))));
        delta5 = delta << 5;

        x = d2;

        do
        {
          y = d2;
          do
          {
            mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, y))] =
              (i8)Arithm4(mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x + d2, y + d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x + d2, y - d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x - d2, y + d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x - d2, y - d2))]);
            y += d1;
          } while (y <= (Dim - d2));
          x += d1;
        } while (x <= (Dim - d2));

        delta = (i8)roundf(delta * expf(-0.6931f) * frakdim);
        delta5 = delta << 5;

        if (newborder)
        {
          x = d2;
          do
          {
            y = x;

            mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(0, x))] =
              (i8)Arithm3(mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(0, x + d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(0, x - d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(d2, x))]);

            mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(Dim, x))] =
              (i8)Arithm3(mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(Dim, x + d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(Dim, x - d2))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(Dim - d2, x))]);

            y = 0;
            mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, 0))] =
              (i8)Arithm3(mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x + d2, 0))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x - d2, 0))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, x))]);

            mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, Dim))] =
              (i8)Arithm3(mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x + d2, Dim))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x - d2, Dim))],
                mcHM->mpui8Data[mcHM->u32PosIdx2ByteIdx(mcHM->u32PxlIdx(x, Dim - d2))]);
            x += d1;
          } while (x <= (Dim - d2));
        }
        vDiagMid(d2, d1);
        vDiagMid(d1, d2);
      }

      void vMidPoint(i8 delta, float frakdim)
      {
        i16 step;

        for (step = 1; step <= 7; step++)
        {
          vMidPointStep(delta, (i8)step, frakdim, True);
        }
      }

    };

    class cCrater
    {
      public:
      cHighmapi8* mcHM;

      u16 mu16SqrTap[129];
      i16 mi16CraHei[129];

      cCrater(cHighmapi8* lcHM)
      {
        mcHM = lcHM;

        float r;

        for (u16 w = 0; w <= 128; w++)
        {
          mu16SqrTap[w] = w * w;
          r = w;

          if (w < 80)
          {
            // Innerer Teil: Mulde
            mi16CraHei[w] = (i16)round(2000 * exp(0.0625 * r - 3)) - 4800;
          }
          else
          {
            // Äußerer Teil: Wall
            mi16CraHei[w] = (i16)round(5000 * cos((r - 80) / 48 * 3.1416)) + 5000;
          }
        }
      }


      u16 u16GetRad(i16 xp, i16 yp, i16 xr, i16 yr)
      {
        u16 sqr1, sqr2, x;
        u16 bot, top, mid;

        //sq1 = (xp-xr)^2 + (yp-yr)^2
        sqr1 = xp; sqr1 -= xr; sqr1 = sqr1 * sqr1;
        sqr2 = yp; sqr2 -= yr; sqr2 = sqr2 * sqr2;
        sqr1 += sqr2;

        bot = 0;
        top = 128;
        do
        {
          mid = (bot + top) / 2;
          x = mu16SqrTap[mid];
          if (x < sqr1)
            bot = mid + 1;
          else
            top = mid - 1;
        } while (top > bot);
        return bot;
      }


      void vSetCrater(i16 x, i16 y, i16 rad)
      {
        i16 x1, y1, x2, y2;
        u16 xr, yr, r;
        i16 heimid, crh, scndrad, fak, fak2, dx, hei;


        if (rad < 7) rad = 7;

        x1 = x; x1 -= rad;
        if (x1 < 0) x1 = 0;
        x2 = x; x2 += rad;
        if (x2 > 128) x2 = 128;

        y1 = y; y1 -= rad;
        if (y1 < 0) y1 = 0;
        y2 = y; y2 += rad;
        if (y2 > 128) y2 = 128;

        heimid = (mcHM->i16Get(x, y) +
          mcHM->i16Get(x1, y) +
          mcHM->i16Get(x, y1) +
          mcHM->i16Get(x2, y) +
          mcHM->i16Get(x, y2)) / 5;

        if (heimid > 90) heimid = 90;
        if (heimid < -90) heimid = -90;

        scndrad = 10000 / rad;


        for (yr = y1; yr <= y2; yr++)
        {
          for (xr = x1; xr <= x2; xr++)
          {
            r = u16GetRad(x, y, xr, yr);

            if (r <= rad)
            {
              fak = (r * 128) / rad;
              fak2 = (128 - fak);
              crh = mi16CraHei[fak] / scndrad;
              dx = mcHM->i16Get(xr, yr) - heimid;

              hei = ((fak2 * crh + fak * dx) / 128) + heimid;

              if (hei > 127) hei = 127;
              if (hei < -128) hei = -128;

              mcHM->vSet(xr, yr, (i8)hei);
            }
          }
        }
      }
    };


    cMidStep mcMidStep;
    cCrater mcCrater;

    cHighmapi8(u16 lu16Width, GfxInt lu16Height, i8* lpui8Data) 
      : cHighmap(lu16Width, lu16Height, HM_I8, (u8*)lpui8Data),
        mcMidStep(this),
        mcCrater(this)
    {};

    cHighmapi8(u16 lu16Width, GfxInt lu16Height)                
      : cHighmap(lu16Width, lu16Height, HM_I8),
        mcMidStep(this),
        mcCrater(this)
    {};

    ~cHighmapi8() {};

    HM_INLINE i8  i8GetMinValue() {return INT8MIN;}
    HM_INLINE i8  i8GetMaxValue() {return INT8MAX;}
    HM_INLINE i8  i8GetRndValue() 
    {
      //The C library function int rand(void) returns a pseudo - random number in the range of 0 to RAND_MAX.
      //RAND_MAX is a constant whose default value may vary between implementations but it is granted to be at least 32767.    
      return (i8)(-rand() % 255);
    }

    HM_INLINE u32  u32PosIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx);};
    HM_INLINE u32  u32ByteIdx2PosIdx(u32 lui32ByteIdx) {return (lui32ByteIdx);};

    HM_INLINE void vFill(i8 li8Value)
    {
      cMemTools::vMemSet(mpui8Data, (u8)li8Value, u32Size());
    }

    HM_INLINE void vSet(u16 lu16X, u16 lu16Y, i8 li8Value)
    {
      i8* lpui8Data = (i8*)mpui8Data;
      lpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(lu16X, lu16Y))] = li8Value;
    }

    HM_INLINE i8 i16Get(u16 lu16X, u16 lu16Y)
    {
      i8* lpui8Data = (i8*)mpui8Data;
      return lpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(lu16X, lu16Y))];
    }

    void vCutSteps(u16 steps)
    {
      int16 s, i, add, rn, x1, y1, x2, y2, s1, s2, e, intchange, x, y, dx, dy, xpos, ypos;

      s = 1;

      do
      {
        do
        {
          add = rand() % 2;
          if (add == 0) add = -1;

          rn = rand() % 6;

          switch (rn)
          {
            case 0:
              x1 = rand() % 127 + 1; y1 = 0;
              x2 = 0;                y2 = rand() % 127 + 1;

              if (y2 < x1)
              {
                for (x = x1 + 1; x < 129; x++)
                  for (y = 0; y < 129; y++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] -= add;
              }
              else
              {
                for (y = y2 + 1; y < 129; y++)
                  for (x = 0; x < 129; x++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] -= add;
              }
              break;
            case 1:
              x1 = rand() % 127 + 1; y1 = 0;
              x2 = rand() % 127 + 1; y2 = 128;
              break;
            case 2:
              x1 = rand() % 127 + 1; y1 = 0;
              x2 = 128;              y2 = rand() % 127 + 1;

              if (y2 > 128 - x1)
              {
                for (y = y2 + 1; y < 129; y++)
                  for (x = 0; x < 129; x++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] += add;
              }
              else
              {
                for (x = 0; x <= x1 - 1; x++)
                  for (y = 0; y < 129; y++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] += add;
              }
              break;
            case 3:
              x1 =   0; y1 = rand() % 127 + 1;
              x2 = 128; y2 = rand() % 127 + 1;
              break;
            case 4:
              x1 = 0;                y1 = rand() % 127 + 1;
              x2 = rand() % 127 + 1; y2 = 128;

              if (128 - y1 > x2)
              {
                for (y = 0; y <= y1 - 1; y++)
                  for (x = 0; x < 129; x++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] -= add;
              }
              else
              {
                for (x = x2 + 1; x < 129; x++)
                  for (y = 0; y < 129; y++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] -= add;
              }
              break;
            case 5:
              x1 = rand() % 127 + 1; y1 = 128;
              x2 = 128;              y2 = rand() % 127 + 1;

              if (128 - x1 > 128 - y2)
              {
                for (x = 0; x <= x1 - 1; x++)
                  for (y = 0; y < 129; y++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] += add;
              }
              else
              {
                for (y = 0; y <= y2 - 1; y++)
                  for (x = 0; x < 129; x++)
                    mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, y))] += add;
              }
              break;
          }

          x = x1; y = y1;
          dx = x2 - x1; dy = y2 - y1;

          if (dx < 0)
          {
            s1 = -1;
            dx = abs(dx);
          }
          else
          {
            if (dx == 0) s1 = 0;
                    else s1 = 1;
          }

          if (dy < 0)
          {
            s2 = -1;
            dy = abs(dy);
          }
          else
          {
            if (dy == 0)
            {
              s2 = 0;
            }
            else
            {
              s2 = 1;
            }
          }

          if (dy > dx)
          {
            e = dx; 
            dx = dy; 
            dy = e; 
            intchange = 1;
          }
          else
          {
            intchange = 0;
          }

          e = (2 * dy) - dx;

          for (i = 0; i <= dx; i++)
          {
            if (intchange == 0)
            {
              ypos = 0;
              while (ypos < y)
              {
                mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, ypos))] += add;
                ypos++;
              }
              ypos++;
              while (ypos < 129)
              {
                mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(x, ypos))] -= add;
                ypos++;
              }
            }
            else
            {
              xpos = 0;
              while (xpos < x)
              {
                mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(xpos, y))] += add;
                xpos++;
              }
              xpos++;
              while (xpos < 129)
              {
                mpui8Data[u32PosIdx2ByteIdx(u32PxlIdx(xpos, y))] -= add;
                xpos++;
              }
            }

            while (e >= 0)
            {
              if (intchange == 1) x += s1;
                             else y += s2;
              e -= 2 * dx;
            }

            if (intchange == 1) y += s2;
                           else x += s1;
            e += 2 * dy;
          }
          
          s++;

        } while (s < steps);
      } while (s < steps);
    }


    class cWell
    {
      cHighmapi8* mcHM;

      struct tstAttributes
      {
        u8 _shad : 1;
        u8 _snow : 1;
        u8 _ocea : 1;
        u8 _well : 1;
        u8 _riv : 1;
        u8 _lite : 1;
        u8 _dark : 1;
      };



      tstAttributes mcAttr[129][129];

      cWell(cHighmapi8* lcHM)
      {
        mcHM = lcHM;
      }

      void vFindRiver(i16 &x, i16 &y)
      {
        i8 hight, min, ctrl;
        i16 xret, yret;

        xret = x;
        yret = y;

        hight = mcHM->i16Get(x, y);

        min = hight;

        // Jetzt prüfen, welcher der Nachbarpunkte am niedrigsten liegt
        ctrl = mcHM->i16Get(x - 1, y); if (ctrl < min) { min = ctrl; xret = x - 1; yret = y; }
        ctrl = mcHM->i16Get(x, y - 1); if (ctrl < min) { min = ctrl; xret = x; yret = y - 1; }
        ctrl = mcHM->i16Get(x + 1, y); if (ctrl < min) { min = ctrl; xret = x + 1; yret = y; }
        ctrl = mcHM->i16Get(x, y + 1); if (ctrl < min) { min = ctrl; xret = x; yret = y + 1; }

        x = xret;
        y = yret;
      }



      void vSetWell(i16 x, i16 y)
      {
        i16 xrem, yrem;

        // wenn kein Meer, dann
        if (mcAttr[x][y]._ocea == 0)
        {
          // eine Quelle setzen
          mcAttr[x][y]._well = 1;
        }

        // Solange kein Meer oder kein Rand erreicht ist
        while ((mcAttr[x][y]._ocea == 0) && (x > 0) && (x < 128) && (y > 0) && (y < 128))
        {
          xrem = x; yrem = y; // Position merken
          vFindRiver(x, y);   // Niedrigsten Nachbarpunkt finden

          if ((xrem == x) && (yrem == y)) // Talsole => See
          {
            // Tal auffüllen
            mcHM->vSet(x, y, mcHM->i16Get(x, y) + 1); 

            if ((x < 0) || (x > 128) || (y < 0) || (y > 128))
            {
              x = x;
            }
            else
            {
              // Flussbit setzen
              mcAttr[x][y]._riv = 1;
            }
          }          
        }
      }
    };
};


#endif  //__Highmap_i8_H__
