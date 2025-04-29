#ifndef __BITMAP_BPP1_1G_H__
#define __BITMAP_BPP1_1G_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap_base.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* --- define BPP1_1G --- */

/* 1Byte = 8Pixel
   0x80 = Most Left  Pixel
   0x01 = Most Right Pixel

   0b10000000 => X_______
   0b00000001 => _______X
*/

#define BM_BPP1_1G_IDX(liGfxX, liGfxY, lcBm)        (u32)((liGfxY) * lcBm.miGfxWidth + (liGfxX))
#define BM_BPP1_1G_BM_SIZE(lcBm)                    (u32)((lcBm.miGfxWidth * lcBm.miGfxHeight) / 8)

#define BM_BPP1_1G_CHECK(lcBm)                      if ((lcBm.mui8Type & 127) != BM_BPP1_1G) return 2;
#define BM_BPP1_1G_PXLIDX2BYTEPOS(lui32PxlIdx)      (lui32PxlIdx >> 3)
#define BM_BPP1_1G_PXLIDX2BITPOS(lui32PxlIdx)       (lui32PxlIdx &  7)

#define BM_BPP1_1G_PXL2MEM_BITPOS(liGfxX, liGfxY, lpui8Data, lui32BitPos, lcBm)   lui32BitPos  = BM_BPP1_1G_IDX(liGfxX, liGfxY, lcBm); \
                                                                                  lpui8Data    = (u8*)&BM_BPP1_1G_MEM_GRP(lui32BitPos, lcBm); \
                                                                                  lui32BitPos  = BM_BPP1_1G_PXLIDX2BITPOS(lui32BitPos);


#define BM_BPP1_1G_MEM(lixIndex,           lcBm)    lcBm.mpui8Data[lixIndex]
#define BM_BPP1_1G_MEM_GRP(li32Index,      lcBm)    lcBm.mpui8Data[BM_BPP1_1G_PXLIDX2BYTEPOS(li32Index)]
#define BM_BPP1_1G_PXL_GRP(liGfxX, liGfxY, lcBm)    BM_BPP1_1G_MEM(BM_BPP1_1G_PXLIDX2BYTEPOS(BM_IDX((liGfxX), (liGfxY), (lcBm)), (lcBm))
#define BM_BPP1_1G_SET_PXL0(lui8PxlGrp, lui8Pxl)    (lui8PxlGrp & (~(128>>lui8Pxl)))
#define BM_BPP1_1G_SET_PXL1(lui8PxlGrp, lui8Pxl)    (lui8PxlGrp | (128>>lui8Pxl))
#define BM_BPP1_1G_GET_PXL(lui8PxlGrp, lui8Pxl)     (lui8PxlGrp & (128>>lui8Pxl))


class cBitmap_Bpp1_1G : public cBitmap
{
  public:

    cBitmap_Bpp1_1G(GfxInt liGfxWidth, GfxInt liGfxHeight, u8* lpui8Data) : cBitmap( liGfxWidth, liGfxHeight,BM_BPP1_1G, lpui8Data)  {};
    cBitmap_Bpp1_1G(GfxInt liGfxWidth, GfxInt liGfxHeight)                : cBitmap( liGfxWidth, liGfxHeight,BM_BPP1_1G) {};
    ~cBitmap_Bpp1_1G() {};

    BM_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  >> 3);};
    BM_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx << 3);};

    BM_INLINE void vFill(u8 lui8Col)
    {
      cMemTools::vMemSet(mpui8Data, lui8Col, u32Size() / 8);
    }


    BM_INLINE void vPp(GfxInt liGfxX, GfxInt liGfxY, u8 lui8Col)
    {
      u32 lui32Idx;
      u32 lui32BytePos;

      lui32Idx     = u32PxlIdx((GfxInt)liGfxX, (GfxInt)liGfxY);
      lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
      lui32Idx     = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);

      if (lui8Col)
      {
        mpui8Data[lui32BytePos] = BM_BPP1_1G_SET_PXL1(mpui8Data[lui32BytePos], lui32Idx);
      }
      else
      {
        mpui8Data[lui32BytePos] = BM_BPP1_1G_SET_PXL0(mpui8Data[lui32BytePos], lui32Idx);
      }
    }

    BM_INLINE u8 u8Gp(GfxInt liGfxX, GfxInt liGfxY)
    {
      u32 lui32Idx;
      u32 lui32BytePos;

      lui32Idx     = u32PxlIdx((GfxInt)liGfxX, (GfxInt)liGfxY);
      lui32BytePos = u32PxlIdx2ByteIdx(lui32Idx);
      lui32Idx     = BM_BPP1_1G_PXLIDX2BITPOS(lui32Idx);

      return BM_BPP1_1G_GET_PXL(mpui8Data[lui32BytePos], lui32Idx);
    }
};


#ifdef __cplusplus
}
#endif

#endif  //BITMAP_BPP1_1G
