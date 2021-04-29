#ifndef __BITMAPCONVERT_BPP32_8R8G8B_H__
#define __BITMAPCONVERT_BPP32_8R8G8B_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap.h"


#ifdef __cplusplus
 extern "C" {
#endif

class cBitmapConvert_Bpp32_8R8G8B
{
  public:
    static FRet i8FromBpp1_1G(cBitmap_Bpp1_1G &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt lui32x, lui32y;
      uint32 lui32PCnt;
      uint8  li8Scol;

      BM_CHK_INIT(lpcBmSrc);
      BPP1_1G_CHECK(lpcBmSrc);
      BM_CHK_INIT(lpcBmDst);
      BPP32_8R8G8B_CHECK(lpcBmDst);

      if ((lpcBmSrc.miGfxWidth  != lpcBmDst.miGfxWidth) ||
          (lpcBmSrc.miGfxHeight != lpcBmDst.miGfxHeight))
      {
        if (BM_CLONE(lpcBmSrc, lpcBmDst) == BM_OK)
          lpcBmDst.i8Create(lpcBmSrc.miGfxWidth,
                            lpcBmSrc.miGfxHeight,
                            BM_BPP32_8R8G8B);
      };

      lui32PCnt = 0;
      for (lui32y = 0; lui32y < lpcBmSrc.miGfxHeight; lui32y++)
      {
        for (lui32x = 0; lui32x < lpcBmSrc.miGfxWidth; lui32x++)
        {
          li8Scol = lpcBmSrc.u8Gp(lui32x, lui32y);
          if (li8Scol)
          {
            BM_BPP32_8R8G8B_MEM(lui32PCnt, lpcBmDst) = (uint32)-1;
          }
          else
          {
            BM_BPP32_8R8G8B_MEM(lui32PCnt, lpcBmDst) = 0;
          }
          lui32PCnt++;
        }
      }

      return BM_OK;
    }

    static FRet i8FromBpp16_5R6G5B(cBitmap_Bpp16_5R6G5B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt lui32x, lui32y;
      uint32 lui32PCnt;
      uint16 lui16Scol;
      uint8  lui8R;
      uint8  lui8G;
      uint8  lui8B;

      BM_CHK_INIT(lpcBmSrc);
      BPP16_5R6G5B_CHECK(lpcBmSrc);
      BM_CHK_INIT(lpcBmDst);
      BPP32_8R8G8B_CHECK(lpcBmDst);

      if ((lpcBmSrc.miGfxWidth != lpcBmDst.miGfxWidth) ||
          (lpcBmSrc.miGfxHeight != lpcBmDst.miGfxHeight))
      {
        if (BM_CLONE(lpcBmSrc, lpcBmDst) == BM_OK)
          lpcBmDst.i8Create(lpcBmSrc.miGfxWidth,
                            lpcBmSrc.miGfxHeight,
                            BM_BPP32_8R8G8B);
      };

      lui32PCnt = 0;
      for (lui32y = 0; lui32y < lpcBmSrc.miGfxHeight; lui32y++)
      {
        for (lui32x = 0; lui32x < lpcBmSrc.miGfxWidth; lui32x++)
        {
          lui16Scol = BM_BPP16_5R6G5B_MEM(lui32PCnt, lpcBmSrc);

          BM_BPP16_5R6G5B_GET_RGB(lui16Scol, lui8R, lui8G, lui8B);

          if (lui8R == 0x1F) lui8R = 0xFF;
                       else  lui8R <<= 3;
          if (lui8G == 0x3F) lui8G = 0xFF;
                       else  lui8G <<= 2;
          if (lui8B == 0x1F) lui8B = 0xFF;
                       else  lui8B <<= 3;

          BM_BPP32_8R8G8B_MEM(lui32PCnt, lpcBmDst) = BM_BPP32_8R8G8B_RGB(lui8R, lui8G, lui8B);

          lui32PCnt++;
        }
      }

      return BM_OK;
    }

/*
    int8 BPP32_8R8G8B_Tools_i8CopyBitmapBlockWH(uGfxInt lui16x, uGfxInt lui16y, uGfxInt ui16Width, uGfxInt ui16Height, cBitmap_Bpp32_8R8G8B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      uint8 lui8Col;
      GfxInt liGfxsx1,  liGfxsy1;
      GfxInt liGfxWidth,liGfxHeight;

      GfxInt liGfxsWidth   = lpcBmSrc.miGfxWidth;
      GfxInt liGfxsHeight  = lpcBmSrc.miGfxHeight;
      //GfxInt liGfxdWidth;

      int32  liGfxi = 0,liGfxt = 0;


      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);
      BM_CHK_INIT(lpcBmDst);
      BPP32_8R8G8B_CHECK(lpcBmDst);


      if (lui16x > lpcBmSrc.miGfxWidth) return 1;
      if (lui16y > lpcBmSrc.miGfxHeight) return 1;
      if (lui16x + ui16Width  < 0) return 1;
      if (lui16y + ui16Height < 0) return 1;

      liGfxsx1 = lui16x;
      liGfxsy1 = lui16y;

      liGfxWidth  = ui16Width;
      liGfxHeight = ui16Height;

      if (lui16x < 0) 
      {
        liGfxsx1   = 0;
        liGfxWidth-= lui16x;
      }

      if (lui16y < 0) 
      {
        liGfxsy1    = 0;
        liGfxHeight-= lui16y;
      }


      if (liGfxsx1 + liGfxWidth  > liGfxsWidth)  liGfxWidth  = liGfxsWidth  - liGfxsx1;
      if (liGfxsy1 + liGfxHeight > liGfxsHeight) liGfxHeight = liGfxsHeight - liGfxsy1;

      if (liGfxWidth  < 1) return 1;
      if (liGfxHeight < 1) return 1;


      BitmapCreate(BPP32_8R8G8B, (uGfxInt)liGfxWidth, (uGfxInt)liGfxHeight, lpcBmDst);


      liGfxsy1    = liGfxsy1 * liGfxsWidth + liGfxsx1;
      //liGfxdWidth = liGfxWidth;

      BM_UpdatePxlPtr(lpcBmSrc);
      BM_UpdatePxlPtr(lpcBmDst);
      
      for (liGfxt=0; liGfxt < liGfxHeight; liGfxt++)
      {
        for (liGfxi=0; liGfxi < liGfxWidth; liGfxi++)
        {

          lui8Col = BPP32_8R8G8B_GetPixel_Direct(liGfxsx1 + liGfxi, liGfxsy1 + liGfxt, lpcBmSrc);
          BPP32_8R8G8B_SetPixel_Direct(liGfxi, liGfxt, lui8Col, lpcBmDst);
        }
      }    

      return 0;
    }


    int8 BPP32_8R8G8B_Tools_i8ScrollstBitmap(GfxInt liGfxXadd, GfxInt liGfxYadd,  cBitmap_Bpp32_8R8G8B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);
      if (BM_CLONE(lpcBmSrc, lpcBmDst) == 0)
      {
        if (liGfxXadd < 0) liGfxXadd = (liGfxXadd) % lpcBmSrc.miGfxHeight;
        if (liGfxYadd < 0) liGfxYadd = (liGfxYadd) % lpcBmSrc.miGfxHeight;
        
        BM_UpdatePxlPtr(lpcBmSrc);
        BM_UpdatePxlPtr(lpcBmDst);

        for (liGfxx = 0; liGfxx < lpcBmSrc.miGfxWidth; liGfxx++)
        {
          for (liGfxy = 0; liGfxy < lpcBmSrc.miGfxHeight; liGfxy++)
          {
            lui8col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);      
            BPP32_8R8G8B_SetPixel_Direct((liGfxx + liGfxXadd) % lpcBmSrc.miGfxWidth, ((liGfxy + liGfxYadd) % lpcBmSrc.miGfxHeight), lui8col, lpcBmSrc);
          }
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }


    int8 BPP32_8R8G8B_Tools_i8XORValue(uint8 lui8Value, cBitmap_Bpp32_8R8G8B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);

      if (BM_CLONE(lpcBmSrc, lpcBmDst) == 0)
      {
        BM_UpdatePxlPtr(lpcBmSrc);
        BM_UpdatePxlPtr(lpcBmDst);
        
        for (liGfxx = 0; liGfxx < lpcBmSrc.miGfxWidth; liGfxx++)
        {
          for (liGfxy = 0; liGfxy < lpcBmSrc.miGfxHeight; liGfxy++)
          {
            lui8col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc) ^ lui8Value;      
            BPP32_8R8G8B_SetPixel_Direct(liGfxx, liGfxy, lui8col, lpcBmSrc);
          }
        }
      }
      else
      {
        return 2;
      }

      return 0;
    }


    int8 BPP32_8R8G8B_Tools_i8NOTBitmap( cBitmap_Bpp32_8R8G8B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);

      if (BM_CLONE(lpcBmSrc, lpcBmDst) == 0)
      {
        BM_UpdatePxlPtr(lpcBmSrc);
        BM_UpdatePxlPtr(lpcBmDst);
        
        for (liGfxx = 0; liGfxx < lpcBmSrc.miGfxWidth; liGfxx++)
        {
          for (liGfxy = 0; liGfxy < lpcBmSrc.miGfxHeight; liGfxy++)
          {
            lui8col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);      
            lui8col = ~lui8col;
            BPP32_8R8G8B_SetPixel_Direct(liGfxx, liGfxy, lui8col, lpcBmSrc);
          }
        }
      }
      else
      {
        return 2;
      }

      return 0;
    }


    int8 BPP32_8R8G8B_Tools_i8MAXCol( cBitmap_Bpp32_8R8G8B &lpcBmSrc, uint8 *lui8Col)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;


      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);

      *lui8Col = 0;
      
      BM_UpdatePxlPtr(lpcBmSrc);

      for (liGfxx = 0; liGfxx < lpcBmSrc.miGfxWidth; liGfxx++)
      {
        for (liGfxy = 0; liGfxy < lpcBmSrc.miGfxHeight; liGfxy++)
        {
          lui8col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);      
          if (lui8col > 0) 
          {
            *lui8Col = 1;
            return 0;
          }
        }
      }

      return 0;
    }


    int8 BPP32_8R8G8B_Tools_i8MINCol( cBitmap_Bpp32_8R8G8B &lpcBmSrc, uint8 *lui8Col)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;


      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);

      *lui8Col = 0;
      BM_UpdatePxlPtr(lpcBmSrc);

      for (liGfxx = 0; liGfxx < lpcBmSrc.miGfxWidth; liGfxx++)
      {
        for (liGfxy = 0; liGfxy < lpcBmSrc.miGfxHeight; liGfxy++)
        {
          lui8col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);
          if (lui8col < 1) 
          {
            *lui8Col = 0;
            return 0;
          }
        }
      }

      return 0;
    }

    int8 BPP32_8R8G8B_Tools_i8SUMCol( cBitmap_Bpp32_8R8G8B &lpcBmSrc, GfxInt *liGfxCol)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);

      *liGfxCol = 0;
      BM_UpdatePxlPtr(lpcBmSrc);

      for (liGfxx = 0; liGfxx < lpcBmSrc.miGfxWidth; liGfxx++)
      {
        for (liGfxy = 0; liGfxy < lpcBmSrc.miGfxHeight; liGfxy++)
        {
          lui8col   = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);      
          liGfxCol += lui8col;
        }
      }

      return 0;
    }



    int8 BPP32_8R8G8B_Tools_i8ANDBitmap( cBitmap_Bpp32_8R8G8B &lpcBmSrc1, 
                           cBitmap_Bpp32_8R8G8B &lpcBmSrc2,
                           cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxt;

      BM_CHK_INIT(lpcBmSrc1);
      BPP32_8R8G8B_CHECK(lpcBmSrc1);
      BM_CHK_INIT(lpcBmSrc2);
      BPP32_8R8G8B_CHECK(lpcBmSrc2);
      BM_CHK_SIZE(lpcBmSrc1, lpcBmSrc2);

      if (BM_CLONE(lpcBmSrc1, lpcBmDst) == 0)
      {
        BM_UpdatePxlPtr(lpcBmSrc1);
        BM_UpdatePxlPtr(lpcBmSrc2);
        BM_UpdatePxlPtr(lpcBmDst);
            
        for (liGfxt = 0; liGfxt < (int32)BPP32_8R8G8B_BM_SIZE(lpcBmDst); liGfxt++)
        {
          BPP32_8R8G8B_MEM(liGfxt, lpcBmDst)  = BPP32_8R8G8B_MEM(liGfxt, lpcBmSrc1) & BPP32_8R8G8B_MEM(liGfxt, lpcBmSrc2);
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }

    int8 BPP32_8R8G8B_Tools_i8ORBitmap( cBitmap_Bpp32_8R8G8B &lpcBmSrc1, 
                          cBitmap_Bpp32_8R8G8B &lpcBmSrc2,
                          cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxt;

      BM_CHK_INIT(lpcBmSrc1);
      BPP32_8R8G8B_CHECK(lpcBmSrc1);
      BM_CHK_INIT(lpcBmSrc2);
      BPP32_8R8G8B_CHECK(lpcBmSrc2);
      BM_CHK_SIZE(lpcBmSrc1, lpcBmSrc2);

      if (BM_CLONE(lpcBmSrc1, lpcBmDst) == 0)
      {
        BM_UpdatePxlPtr(lpcBmSrc1);
        BM_UpdatePxlPtr(lpcBmSrc2);
        BM_UpdatePxlPtr(lpcBmDst);
        
        for (liGfxt = 0; liGfxt < (int32)BPP32_8R8G8B_BM_SIZE(lpcBmDst); liGfxt++)
        {
          BPP32_8R8G8B_MEM(liGfxt, lpcBmDst)  = BPP32_8R8G8B_MEM(liGfxt, lpcBmSrc1) | BPP32_8R8G8B_MEM(liGfxt, lpcBmSrc2);
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }

    int8 BPP32_8R8G8B_Tools_i8XORBitmap( cBitmap_Bpp32_8R8G8B &lpcBmSrc1, 
                                cBitmap_Bpp32_8R8G8B &lpcBmSrc2,
                                cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxt;

      BM_CHK_INIT(lpcBmSrc1);
      BPP32_8R8G8B_CHECK(lpcBmSrc1);
      BM_CHK_INIT(lpcBmSrc2);
      BPP32_8R8G8B_CHECK(lpcBmSrc2);
      BM_CHK_SIZE(lpcBmSrc1, lpcBmSrc2);

      if (BM_CLONE(lpcBmSrc1, lpcBmDst) == 0)
      {
        BM_UpdatePxlPtr(lpcBmSrc1);
        BM_UpdatePxlPtr(lpcBmSrc2);
        BM_UpdatePxlPtr(lpcBmDst);
        
        for (liGfxt = 0; liGfxt < (int32)BPP32_8R8G8B_BM_SIZE(lpcBmDst); liGfxt++)
        {
          BPP32_8R8G8B_MEM(liGfxt, lpcBmDst)  = BPP32_8R8G8B_MEM(liGfxt, lpcBmSrc1) ^ BPP32_8R8G8B_MEM(liGfxt, lpcBmSrc2);
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }



    int8 BPP32_8R8G8B_Tools_i8ScallstBitmap( cBitmap_Bpp32_8R8G8B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8Col;

      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);
      BM_CHK_INIT(lpcBmDst);
      BPP32_8R8G8B_CHECK(lpcBmDst);

      if ((lpcBmSrc.miGfxWidth  == lpcBmDst.miGfxWidth) &&
          (lpcBmSrc.miGfxHeight == lpcBmDst.miGfxHeight))
      {
        return BPP32_8R8G8B_CopyBitmapRaw(lpcBmSrc, lpcBmDst);
      }
      else
      {
        BM_UpdatePxlPtr(lpcBmSrc);
        BM_UpdatePxlPtr(lpcBmDst);
        
        for (liGfxy = 0 ; liGfxy < lpcBmDst.miGfxHeight; liGfxy++)
        {
          for (liGfxx = 0; liGfxx < lpcBmDst.miGfxWidth; liGfxx++)
          {
            lui8Col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);
            BPP32_8R8G8B_SetPixel_Direct((liGfxx * lpcBmSrc.miGfxWidth)  / lpcBmDst.miGfxWidth, 
                                          (liGfxy * lpcBmSrc.miGfxHeight) / lpcBmDst.miGfxHeight, 
                                           lui8Col, lpcBmDst);
          }
        }
      }
      return 0;
    }



    int8 BPP32_8R8G8B_Tools_i8BitmapFlipX( cBitmap_Bpp32_8R8G8B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8Col;

      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);

      if (BM_CLONE(lpcBmSrc, lpcBmDst) == 0)
      {
        BM_UpdatePxlPtr(lpcBmSrc);
        BM_UpdatePxlPtr(lpcBmDst);
        
        for (liGfxy = 0 ; liGfxy < lpcBmDst.miGfxHeight; liGfxy++)
        {
          for (liGfxx = 0; liGfxx < lpcBmDst.miGfxWidth; liGfxx++)
          {
            lui8Col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);
            BPP32_8R8G8B_SetPixel_Direct((lpcBmDst.miGfxWidth - 1) - liGfxx, 
                                    liGfxy, 
                                    lui8Col, lpcBmDst);
          }
        }
      }
      else
      {
        return 0;
      }

      return 0;
    }

    int8 BPP32_8R8G8B_Tools_i8BitmapFlipY( cBitmap_Bpp32_8R8G8B &lpcBmSrc, cBitmap_Bpp32_8R8G8B &lpcBmDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8Col;

      BM_CHK_INIT(lpcBmSrc);
      BPP32_8R8G8B_CHECK(lpcBmSrc);

      if (BM_CLONE(lpcBmSrc, lpcBmDst) == 0)
      {
        BM_UpdatePxlPtr(lpcBmSrc);
        BM_UpdatePxlPtr(lpcBmDst);
        
        for (liGfxy = 0 ; liGfxy < lpcBmDst.miGfxHeight; liGfxy++)
        {
          for (liGfxx = 0; liGfxx < lpcBmDst.miGfxWidth; liGfxx++)
          {
            lui8Col = BPP32_8R8G8B_GetPixel_Direct(liGfxx, liGfxy, lpcBmSrc);
            BPP32_8R8G8B_SetPixel_Direct(liGfxx, 
                                         (lpcBmDst.miGfxHeight - 1) - liGfxy, 
                                          lui8Col, lpcBmDst);
          }
        }
      }
      else
      {
        return 0;
      }

      return 0;
    }*/
};


#ifdef __cplusplus
}
#endif

#endif  //BITMAPCONVERT_BPP32_8R8G8B
