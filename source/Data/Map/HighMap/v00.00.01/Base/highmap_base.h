#ifndef __HIGHMAP_BASE_H__
#define __HIGHMAP_BASE_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "GfxDefs.h"
#include "MemTools.h"


#define HM_INVALID            GFX_INVALID
#define HM_FAIL               GFX_FAIL
#define HM_OK                 GFX_OK
#define HM_INLINE             GFX_INLINE

//Types
#define HM_I8       0
#define HM_I16      1
#define HM_I32      2
#define HM_FLOAT    3


class cHighmap
{
  public:
    u8*    mpui8Data;
    u16    mu16Width;       // Width
    u16    mu16Height;      // Height
    u8     mui8Type;                      

    cHighmap()
    {
      mpui8Data   = HM_INVALID;
      mu16Width   = 0;
      mu16Height  = 0;
      mui8Type    = 0;
    }

    cHighmap(u16 lu16Width, u16 lu16Height, u8 lui8Type, u8* lpui8Data)
    {
      mpui8Data   = lpui8Data;
      mu16Width   = lu16Width;
      mu16Height  = lu16Height;
      mui8Type    = lui8Type;
    }

    cHighmap(u16 lu16Width, u16 lu16Height, u8 lui8Type)
      :cHighmap()
    {
      i8Create(lu16Width, lu16Height, lui8Type);
    }

    u8 u8Bpp(u8 lui8Type)
    {
      u8    lui8BPP;

      switch (lui8Type)
      {
        case HM_I8:
          lui8BPP = 8;
        break;
        case HM_I16:
          lui8BPP = 16;
        break;
        case HM_I32:
          lui8BPP = 32;
        break;
        case HM_FLOAT:
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

    i8 i8Create(u16 lu16Width, u16 lu16Height, u8 lui8Type)
    {
      u32   lui32SizeInByte;
      u8    lui8BPP;

      lui32SizeInByte = 0;

      if (mpui8Data != HM_INVALID)
      {
        if ((u8Type()   == lui8Type) &&
            (mu16Width  == lu16Width) &&
            (mu16Height == lu16Height))
        {
          return HM_OK;
        }
        else
        {
          if (!bDyn())
          {
            return HM_FAIL;
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
        lui32SizeInByte = ((((lu16Width + 8) & 7) + lu16Width) * lu16Height * lui8BPP) / 8;
      }
      else
      {
        lui32SizeInByte = lu16Width * lu16Height * lui8BPP / 8;
      }

      mui8Type = lui8Type;
      if (!bDyn()) 
      {
        mui8Type += 128;
      }

      mpui8Data = new u8[lui32SizeInByte];

      if (mpui8Data != HM_INVALID)
      {
        mu16Width    = lu16Width;
        mu16Height   = lu16Height;
      }
      else
      {
        mui8Type      = HM_I8;
        mu16Width    = 0;
        mu16Height   = 0;
        return HM_FAIL;
      }

      return HM_OK;
    }

    HM_INLINE bool   bChkInit()                                      {return (mpui8Data != HM_INVALID);};
    HM_INLINE bool   bChkSize(u16 lu16Width, u16 lu16Height)         {return ((mu16Width == lu16Width) && (mu16Height == lu16Height));};
    HM_INLINE bool   bChkSize(cHighmap lcHm)                         {return ((mu16Width == lcHm.mu16Width) && (mu16Height == lcHm.mu16Height));};
    HM_INLINE u32    u32PosX(u32 lui32PxlIdx)                        {return ((lui32PxlIdx) % mu16Width);};
    HM_INLINE u32    u32PosY(u32 lui32PxlIdx)                        {return ((lui32PxlIdx) / mu16Width);};
    HM_INLINE u32    u32PxlIdx(GfxInt liGfxX, GfxInt liGfxY)         {return ((uint32)((liGfxY) * mu16Width + (liGfxX)));};
    HM_INLINE u32    u32Size()                                       {return ((uint32)(mu16Width * mu16Height));};
    HM_INLINE u8*    pu8Data()                                       {return (mpui8Data);};
    HM_INLINE i8     i8Clone(cHighmap lcHm)                          {return lcHm.i8Create(mu16Width, mu16Height, mui8Type);};

    HM_INLINE void vCopy(cHighmap &lcHm)
    {
      if ((lcHm.mu16Width == mu16Width) && (lcHm.mu16Height == mu16Height))
      {
        u32 lu32Size = mu16Width * mu16Height;

        switch (mui8Type)
        {
          case HM_I16:
            lu32Size *= 2;
            break;
          case HM_I32:
            lu32Size *= 4;
            break;
          case HM_FLOAT:
            lu32Size *= 4;
            break;
        }

        cMemTools::vMemCpy(lcHm.mpui8Data, mpui8Data, lu32Size);
      }
    }
    
    void vDelete()
    {
      if (bDyn())
      {
        if (mpui8Data != HM_INVALID)
        {
          delete [] mpui8Data;
        }
      }
      mpui8Data = HM_INVALID;
    }

    ~cHighmap()
    {
      vDelete();
    }
};


#ifdef __cplusplus
}
#endif

#endif  //__HIGHMAP_BASE_H__
