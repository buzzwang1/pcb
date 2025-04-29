#ifndef __BITMAP_BASE_H__
#define __BITMAP_BASE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "GfxDefs.h"
#include "MemTools.h"


#define BM_INVALID            GFX_INVALID
#define BM_FAIL               GFX_FAIL
#define BM_OK                 GFX_OK
#define BM_INLINE             GFX_INLINE

//Types
#define BM_BPP32_8R8G8B 0
#define BM_BPP8_8G      1
#define BM_BPP1_1G      2
#define BM_BPP16_5R6G5B 3

#define BPP32_8R8G8B_CHECK(lcBm)              BM_BPP32_8R8G8B_CHECK(lcBm)
#define BPP8_8G_CHECK(lcBm)                   BM_BPP8_8G_CHECK(lcBm)
#define BPP1_1G_CHECK(lcBm)                   BM_BPP1_1G_CHECK(lcBm)
#define BPP16_5R6G5B_CHECK(lcBm)              BM_BPP16_5R6G5B_CHECK(lcBm)

/* --- General  --- */
#define BM_CHK_INIT(lcBm)                     if (lcBm.mpui8Data == BM_INVALID) return 2
#define BM_CHK_SIZE(lcBmSource, lcBmDest)     if ((lcBmSource.miGfxWidth != lcBmDest.miGfxWidth) || (lcBmSource.miGfxHeight != lcBmDest.miGfxHeight)) return 2
#define BM_POSX(lui32Index, lcBm)             ((lui32Index) % lcBm.miGfxWidth)
#define BM_POSY(lui32Index, lcBm)             ((lui32Index) / lcBm.miGfxWidth)
#define BM_IDX(liGfxX, liGfxY, lcBm)          ((uint32)((liGfxY) * lcBm.miGfxWidth + (liGfxX)))
#define BM_SIZE(lcBm)                         (lcBm.miGfxWidth * lcBm.miGfxHeight)
#define BM_DATA(lcBm)                         (lcBm.mpui8Data)
#define BM_CLONE(lcBmSource, lcBmDest)        lcBmSource.i8Clone(lcBmDest)
#define BM_CLIP_COL(Col)                      if (Col > 255) Col = 255;  if (Col < 0) Col = 0;


typedef struct
{
  u8 b; // Blue
  u8 g; // Green
  u8 r; // Red
  u8 a; // Alpha
}tstBmARGB;

typedef union  // Deklaration eines neuen Datentyps
{
  u32       u32Col;
  tstBmARGB stARGB;
}unBmBaseCol;



class cBitmap
{
  public:
    // Memeory ganz vorne in der Klasse, damit
    // man die Klasse auf 32Bit align'en kann => wegen DMA
    u8*    mpui8Data;        // Pixel memory
    GfxInt miGfxWidth;       // Width
    GfxInt miGfxHeight;      // Height
    u8     mui8Type;         // Type of bitmap e.g. Color or grey
                             //   < 128 static memory; >= 128 dyn memory

    cBitmap()
    {
      mpui8Data   = BM_INVALID;
      miGfxWidth  = 0;
      miGfxHeight = 0;
      mui8Type    = 0;
    }

    cBitmap(GfxInt liGfxWidth, GfxInt liGfxHeight, u8 lui8Type, u8* lpui8Data)
    {
      mpui8Data   = lpui8Data;
      miGfxWidth  = liGfxWidth;
      miGfxHeight = liGfxHeight;
      mui8Type    = lui8Type;
    }

    cBitmap(GfxInt liGfxWidth, GfxInt liGfxHeight, u8 lui8Type)
      :cBitmap()
    {
      i8Create(liGfxWidth, liGfxHeight, lui8Type);
    }

    u8 u8Bpp(u8 lui8Type)
    {
      u8    lui8BPP;

      switch (lui8Type)
      {
        case BM_BPP1_1G:
          lui8BPP = 1;
        break;
        case BM_BPP8_8G:
          lui8BPP = 8;
        break;
        case BM_BPP16_5R6G5B:
          lui8BPP = 16;
        break;
        case BM_BPP32_8R8G8B:
          lui8BPP = 32;
        break;
        default:
          lui8BPP = 32;
      }

      return lui8BPP;
    }

    u8 u8Bpp()
    {
      return u8Bpp(mui8Type);
    }

    bool bDyn()
    {
      return (mui8Type > 127);
    }

    u8 u8Type()
    {
      return (mui8Type & 0x7F);
    }

    i8 i8Create(GfxInt liGfxWidth, GfxInt liGfxHeight, u8 lui8Type)
    {
      u32   lui32SizeInByte;
      u8    lui8BPP;

      lui32SizeInByte = 0;

      if (mpui8Data != BM_INVALID)
      {
        if ((u8Type()    == lui8Type) &&
            (miGfxWidth  == liGfxWidth) &&
            (miGfxHeight == liGfxHeight))
        {
          return BM_OK;
        }
        else
        {
          if (!bDyn())
          {
            return BM_FAIL;
          }
          else
          {
            delete [] mpui8Data;
          }
        }
      }

      lui8BPP = u8Bpp(lui8Type);

      if (lui8BPP == 1)
      {
        // Auf volle Bytes runden
        lui32SizeInByte = ((((liGfxWidth + 8) & 7) + liGfxWidth) * liGfxHeight * lui8BPP) / 8;
      }
      else
      {
        lui32SizeInByte = liGfxWidth * liGfxHeight * lui8BPP / 8;
      }

      mui8Type = lui8Type;
      if (!bDyn()) 
      {
        mui8Type += 128;
      }

      mpui8Data = new u8[lui32SizeInByte];

      if (mpui8Data != BM_INVALID)
      {
        miGfxWidth    = liGfxWidth;
        miGfxHeight   = liGfxHeight;
      }
      else
      {
        mui8Type      = BM_BPP8_8G;
        miGfxWidth    = 0;
        miGfxHeight   = 0;
        return BM_FAIL;
      }

      return BM_OK;
    }

    BM_INLINE bool   bChkInit()                                      {return (mpui8Data != BM_INVALID);};
    BM_INLINE bool   bChkSize(GfxInt liGfxWidth, GfxInt liGfxHeight) {return ((miGfxWidth == liGfxWidth) && (miGfxHeight == liGfxHeight));};
    BM_INLINE bool   bChkSize(cBitmap lcBm)                          {return ((miGfxWidth == lcBm.miGfxWidth) && (miGfxHeight == lcBm.miGfxHeight));};
    BM_INLINE u32    u32PosX(u32 lui32PxlIdx)                        {return ((lui32PxlIdx) % miGfxWidth);};
    BM_INLINE u32    u32PosY(u32 lui32PxlIdx)                        {return ((lui32PxlIdx) / miGfxWidth);};
    BM_INLINE u32    u32PxlIdx(GfxInt liGfxX, GfxInt liGfxY)         {return ((uint32)((liGfxY) * miGfxWidth + (liGfxX)));};
    BM_INLINE u8*    pu8Data()                                       {return (mpui8Data);};
    BM_INLINE i8     i8Clone(cBitmap lcBm)                           {return lcBm.i8Create(miGfxWidth, miGfxHeight, mui8Type);};
    BM_INLINE GfxInt iGfxAbs(GfxInt  liGfxV)                         {if (liGfxV < 0) return -liGfxV; else return liGfxV;};

    BM_INLINE u32    u32Size()
    {
      u32 lu32Size = miGfxWidth * miGfxHeight;

      switch (mui8Type)
      {
        case BM_BPP1_1G:
          lu32Size /= 8;
        break;
        case BM_BPP16_5R6G5B:
          lu32Size *= 2;
        break;
        case BM_BPP32_8R8G8B:
          lu32Size *= 4;
        break;
      }
      return lu32Size;
    }


    BM_INLINE void vCopy(cBitmap &lcBm)
    {
      if ((lcBm.miGfxWidth == miGfxWidth) && (lcBm.miGfxHeight == miGfxHeight))
      {
        cMemTools::vMemCpy(lcBm.mpui8Data, mpui8Data, u32Size());
      }
    }
    
    void vDelete()
    {
      if (bDyn())
      {
        if (mpui8Data != BM_INVALID)
        {
          delete [] mpui8Data;
        }
      }
      mpui8Data = BM_INVALID;
    }

    ~cBitmap()
    {
      vDelete();
    }
};


#ifdef __cplusplus
}
#endif

#endif  //BITMAP_BASE
