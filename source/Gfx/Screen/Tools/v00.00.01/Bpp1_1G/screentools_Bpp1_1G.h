#ifndef __SCREENTOOLS_BPP1_1G_H__
#define __SCREENTOOLS_BPP1_1G_H__

/* Includes ------------------------------------------------------------------*/
#include "screen_bpp1_1g.h"

#ifdef __cplusplus
 extern "C" {
#endif


class cScreentools_Bpp1_1G
{
  public:
    static int8 i8Copy(cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      SCREEN_CHK_INIT(lpcScreenSrc);
      SCREEN_BPP1_1G_CHECK(lpcScreenSrc);
      SCREEN_CHK_INIT(lpcScreenDst);
      SCREEN_BPP1_1G_CHECK(lpcScreenDst);

      if ((lpcScreenSrc.mbVisible) && (lpcScreenDst.mbVisible))
      {
        GfxInt liGfxW, liGfxH;

        if (lpcScreenDst.iGfxWidth() < lpcScreenSrc.iGfxWidth()) liGfxW = lpcScreenDst.iGfxWidth();
                                                            else liGfxW = lpcScreenSrc.iGfxWidth();
        if (lpcScreenDst.iGfxHeight() < lpcScreenSrc.iGfxHeight()) liGfxH = lpcScreenDst.iGfxHeight();
                                                              else liGfxH = lpcScreenSrc.iGfxHeight();

        u32 lui32IdxSrc;
        u8* lpui8DataSrc;
        u32 lui32IdxDst;
        u8* lpui8DataDst;

        BM_BPP1_1G_PXL2MEM_BITPOS(lpcScreenSrc.miGfxClipPosx1, lpcScreenSrc.miGfxClipPosy1, lpui8DataSrc, lui32IdxSrc, (*lpcScreenSrc.mpcBm));
        BM_BPP1_1G_PXL2MEM_BITPOS(lpcScreenDst.miGfxClipPosx1, lpcScreenDst.miGfxClipPosy1, lpui8DataDst, lui32IdxDst, (*lpcScreenSrc.mpcBm));

        for (GfxInt liGfxY = lpcScreenDst.miGfxClipPosy1; liGfxY <= (lpcScreenDst.miGfxClipPosy1 + liGfxH - 1); liGfxY++)
        {
          cMemTools::vMemBitCpy(lpui8DataSrc, lui32IdxSrc, lpui8DataDst, lui32IdxDst, liGfxW);

          lpui8DataSrc += lpcScreenSrc.u32PxlIdx2ByteIdx(lpcScreenSrc.mpcBm->miGfxWidth);
          lpui8DataDst += lpcScreenDst.u32PxlIdx2ByteIdx(lpcScreenDst.mpcBm->miGfxWidth);
        }
      }
      return SCREEN_OK;
    }

/*
    int8 BPP1_1G_Tools_i8CopyScreenBlockWH(uGfxInt lui16x, uGfxInt lui16y, uGfxInt ui16Width, uGfxInt ui16Height, cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      uint8 lui8Col;
      GfxInt liGfxsx1,  liGfxsy1;
      GfxInt liGfxWidth,liGfxHeight;

      GfxInt liGfxsWidth   = lpcScreenSrc.miGfxWidth;
      GfxInt liGfxsHeight  = lpcScreenSrc.miGfxHeight;
      //GfxInt liGfxdWidth;

      int32  liGfxi = 0,liGfxt = 0;


      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);
      SCREEN_CHK_INIT(lpcScreenDst);
      BPP1_1G_CHECK(lpcScreenDst);


      if (lui16x > lpcScreenSrc.miGfxWidth) return 1;
      if (lui16y > lpcScreenSrc.miGfxHeight) return 1;
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


      ScreenCreate(BPP1_1G, (uGfxInt)liGfxWidth, (uGfxInt)liGfxHeight, lpcScreenDst);


      liGfxsy1    = liGfxsy1 * liGfxsWidth + liGfxsx1;
      //liGfxdWidth = liGfxWidth;

      SCREEN_UpdatePxlPtr(lpcScreenSrc);
      SCREEN_UpdatePxlPtr(lpcScreenDst);
      
      for (liGfxt=0; liGfxt < liGfxHeight; liGfxt++)
      {
        for (liGfxi=0; liGfxi < liGfxWidth; liGfxi++)
        {

          lui8Col = BPP1_1G_GetPixel_Direct(liGfxsx1 + liGfxi, liGfxsy1 + liGfxt, lpcScreenSrc);
          BPP1_1G_SetPixel_Direct(liGfxi, liGfxt, lui8Col, lpcScreenDst);
        }
      }    

      return 0;
    }


    int8 BPP1_1G_Tools_i8ScrollstScreen(GfxInt liGfxXadd, GfxInt liGfxYadd,  cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);
      if (SCREEN_CLONE(lpcScreenSrc, lpcScreenDst) == 0)
      {
        if (liGfxXadd < 0) liGfxXadd = (liGfxXadd) % lpcScreenSrc.miGfxHeight;
        if (liGfxYadd < 0) liGfxYadd = (liGfxYadd) % lpcScreenSrc.miGfxHeight;
        
        SCREEN_UpdatePxlPtr(lpcScreenSrc);
        SCREEN_UpdatePxlPtr(lpcScreenDst);

        for (liGfxx = 0; liGfxx < lpcScreenSrc.miGfxWidth; liGfxx++)
        {
          for (liGfxy = 0; liGfxy < lpcScreenSrc.miGfxHeight; liGfxy++)
          {
            lui8col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);      
            BPP1_1G_SetPixel_Direct((liGfxx + liGfxXadd) % lpcScreenSrc.miGfxWidth, ((liGfxy + liGfxYadd) % lpcScreenSrc.miGfxHeight), lui8col, lpcScreenSrc);
          }
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }


    int8 BPP1_1G_Tools_i8XORValue(uint8 lui8Value, cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);

      if (SCREEN_CLONE(lpcScreenSrc, lpcScreenDst) == 0)
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
        
        for (liGfxx = 0; liGfxx < lpcScreenSrc.miGfxWidth; liGfxx++)
        {
          for (liGfxy = 0; liGfxy < lpcScreenSrc.miGfxHeight; liGfxy++)
          {
            lui8col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc) ^ lui8Value;      
            BPP1_1G_SetPixel_Direct(liGfxx, liGfxy, lui8col, lpcScreenSrc);
          }
        }
      }
      else
      {
        return 2;
      }

      return 0;
    }


    int8 BPP1_1G_Tools_i8NOTScreen( cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);

      if (SCREEN_CLONE(lpcScreenSrc, lpcScreenDst) == 0)
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
        
        for (liGfxx = 0; liGfxx < lpcScreenSrc.miGfxWidth; liGfxx++)
        {
          for (liGfxy = 0; liGfxy < lpcScreenSrc.miGfxHeight; liGfxy++)
          {
            lui8col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);      
            lui8col = ~lui8col;
            BPP1_1G_SetPixel_Direct(liGfxx, liGfxy, lui8col, lpcScreenSrc);
          }
        }
      }
      else
      {
        return 2;
      }

      return 0;
    }


    int8 BPP1_1G_Tools_i8MAXCol( cScreen_Bpp1_1G &lpcScreenSrc, uint8 *lui8Col)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;


      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);

      *lui8Col = 0;
      
      SCREEN_UpdatePxlPtr(lpcScreenSrc);

      for (liGfxx = 0; liGfxx < lpcScreenSrc.miGfxWidth; liGfxx++)
      {
        for (liGfxy = 0; liGfxy < lpcScreenSrc.miGfxHeight; liGfxy++)
        {
          lui8col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);      
          if (lui8col > 0) 
          {
            *lui8Col = 1;
            return 0;
          }
        }
      }

      return 0;
    }


    int8 BPP1_1G_Tools_i8MINCol( cScreen_Bpp1_1G &lpcScreenSrc, uint8 *lui8Col)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;


      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);

      *lui8Col = 0;
      SCREEN_UpdatePxlPtr(lpcScreenSrc);

      for (liGfxx = 0; liGfxx < lpcScreenSrc.miGfxWidth; liGfxx++)
      {
        for (liGfxy = 0; liGfxy < lpcScreenSrc.miGfxHeight; liGfxy++)
        {
          lui8col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);
          if (lui8col < 1) 
          {
            *lui8Col = 0;
            return 0;
          }
        }
      }

      return 0;
    }

    int8 BPP1_1G_Tools_i8SUMCol( cScreen_Bpp1_1G &lpcScreenSrc, GfxInt *liGfxCol)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8col;

      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);

      *liGfxCol = 0;
      SCREEN_UpdatePxlPtr(lpcScreenSrc);

      for (liGfxx = 0; liGfxx < lpcScreenSrc.miGfxWidth; liGfxx++)
      {
        for (liGfxy = 0; liGfxy < lpcScreenSrc.miGfxHeight; liGfxy++)
        {
          lui8col   = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);      
          liGfxCol += lui8col;
        }
      }

      return 0;
    }



    int8 BPP1_1G_Tools_i8ANDScreen( cScreen_Bpp1_1G &lpcScreenSrc1, 
                           cScreen_Bpp1_1G &lpcScreenSrc2,
                           cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxt;

      SCREEN_CHK_INIT(lpcScreenSrc1);
      BPP1_1G_CHECK(lpcScreenSrc1);
      SCREEN_CHK_INIT(lpcScreenSrc2);
      BPP1_1G_CHECK(lpcScreenSrc2);
      SCREEN_CHK_SIZE(lpcScreenSrc1, lpcScreenSrc2);

      if (SCREEN_CLONE(lpcScreenSrc1, lpcScreenDst) == 0)
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc1);
        SCREEN_UpdatePxlPtr(lpcScreenSrc2);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
            
        for (liGfxt = 0; liGfxt < (int32)BPP1_1G_SCREEN_SIZE(lpcScreenDst); liGfxt++)
        {
          BPP1_1G_MEM(liGfxt, lpcScreenDst)  = BPP1_1G_MEM(liGfxt, lpcScreenSrc1) & BPP1_1G_MEM(liGfxt, lpcScreenSrc2);
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }

    int8 BPP1_1G_Tools_i8ORScreen( cScreen_Bpp1_1G &lpcScreenSrc1, 
                          cScreen_Bpp1_1G &lpcScreenSrc2,
                          cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxt;

      SCREEN_CHK_INIT(lpcScreenSrc1);
      BPP1_1G_CHECK(lpcScreenSrc1);
      SCREEN_CHK_INIT(lpcScreenSrc2);
      BPP1_1G_CHECK(lpcScreenSrc2);
      SCREEN_CHK_SIZE(lpcScreenSrc1, lpcScreenSrc2);

      if (SCREEN_CLONE(lpcScreenSrc1, lpcScreenDst) == 0)
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc1);
        SCREEN_UpdatePxlPtr(lpcScreenSrc2);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
        
        for (liGfxt = 0; liGfxt < (int32)BPP1_1G_SCREEN_SIZE(lpcScreenDst); liGfxt++)
        {
          BPP1_1G_MEM(liGfxt, lpcScreenDst)  = BPP1_1G_MEM(liGfxt, lpcScreenSrc1) | BPP1_1G_MEM(liGfxt, lpcScreenSrc2);
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }

    int8 BPP1_1G_Tools_i8XORScreen( cScreen_Bpp1_1G &lpcScreenSrc1, 
                                cScreen_Bpp1_1G &lpcScreenSrc2,
                                cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxt;

      SCREEN_CHK_INIT(lpcScreenSrc1);
      BPP1_1G_CHECK(lpcScreenSrc1);
      SCREEN_CHK_INIT(lpcScreenSrc2);
      BPP1_1G_CHECK(lpcScreenSrc2);
      SCREEN_CHK_SIZE(lpcScreenSrc1, lpcScreenSrc2);

      if (SCREEN_CLONE(lpcScreenSrc1, lpcScreenDst) == 0)
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc1);
        SCREEN_UpdatePxlPtr(lpcScreenSrc2);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
        
        for (liGfxt = 0; liGfxt < (int32)BPP1_1G_SCREEN_SIZE(lpcScreenDst); liGfxt++)
        {
          BPP1_1G_MEM(liGfxt, lpcScreenDst)  = BPP1_1G_MEM(liGfxt, lpcScreenSrc1) ^ BPP1_1G_MEM(liGfxt, lpcScreenSrc2);
        }
      }
      else
      {
        return 2;
      }
      return 0;
    }



    int8 BPP1_1G_Tools_i8ScallstScreen( cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8Col;

      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);
      SCREEN_CHK_INIT(lpcScreenDst);
      BPP1_1G_CHECK(lpcScreenDst);

      if ((lpcScreenSrc.miGfxWidth  == lpcScreenDst.miGfxWidth) &&
          (lpcScreenSrc.miGfxHeight == lpcScreenDst.miGfxHeight))
      {
        return BPP1_1G_CopyScreenRaw(lpcScreenSrc, lpcScreenDst);
      }
      else
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
        
        for (liGfxy = 0 ; liGfxy < lpcScreenDst.miGfxHeight; liGfxy++)
        {
          for (liGfxx = 0; liGfxx < lpcScreenDst.miGfxWidth; liGfxx++)
          {
            lui8Col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);
            BPP1_1G_SetPixel_Direct((liGfxx * lpcScreenSrc.miGfxWidth)  / lpcScreenDst.miGfxWidth, 
                                          (liGfxy * lpcScreenSrc.miGfxHeight) / lpcScreenDst.miGfxHeight, 
                                           lui8Col, lpcScreenDst);
          }
        }
      }
      return 0;
    }



    int8 BPP1_1G_Tools_i8ScreenFlipX( cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8Col;

      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);

      if (SCREEN_CLONE(lpcScreenSrc, lpcScreenDst) == 0)
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
        
        for (liGfxy = 0 ; liGfxy < lpcScreenDst.miGfxHeight; liGfxy++)
        {
          for (liGfxx = 0; liGfxx < lpcScreenDst.miGfxWidth; liGfxx++)
          {
            lui8Col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);
            BPP1_1G_SetPixel_Direct((lpcScreenDst.miGfxWidth - 1) - liGfxx, 
                                    liGfxy, 
                                    lui8Col, lpcScreenDst);
          }
        }
      }
      else
      {
        return 0;
      }

      return 0;
    }

    int8 BPP1_1G_Tools_i8ScreenFlipY( cScreen_Bpp1_1G &lpcScreenSrc, cScreen_Bpp1_1G &lpcScreenDst)
    {
      GfxInt liGfxx, liGfxy;
      uint8  lui8Col;

      SCREEN_CHK_INIT(lpcScreenSrc);
      BPP1_1G_CHECK(lpcScreenSrc);

      if (SCREEN_CLONE(lpcScreenSrc, lpcScreenDst) == 0)
      {
        SCREEN_UpdatePxlPtr(lpcScreenSrc);
        SCREEN_UpdatePxlPtr(lpcScreenDst);
        
        for (liGfxy = 0 ; liGfxy < lpcScreenDst.miGfxHeight; liGfxy++)
        {
          for (liGfxx = 0; liGfxx < lpcScreenDst.miGfxWidth; liGfxx++)
          {
            lui8Col = BPP1_1G_GetPixel_Direct(liGfxx, liGfxy, lpcScreenSrc);
            BPP1_1G_SetPixel_Direct(liGfxx, 
                                         (lpcScreenDst.miGfxHeight - 1) - liGfxy, 
                                          lui8Col, lpcScreenDst);
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

#endif  //SCREENTOOLS_BPP1_1G
