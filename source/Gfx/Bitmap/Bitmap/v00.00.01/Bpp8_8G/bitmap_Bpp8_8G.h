#ifndef __BITMAP_BPP8_8G_H__
#define __BITMAP_BPP8_8G_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap_base.h"

#ifdef __cplusplus
 extern "C" {
#endif


/* --- define BPP8_8G --- */
#define BM_BPP8_8G_IDX(liGfxX, liGfxY, lcBm) (u32)((liGfxY) * lcBm.miGfxWidth + (liGfxX))
#define BM_BPP8_8G_BM_SIZE(lcBm)         (u32)((lcBm.miGfxWidth * lcBm.miGfxHeight))

#define BM_BPP8_8G_CHECK(lcBm)               if ((lcBm.mui8Type & 127) != BM_BPP8_8G) return 2;
#define BM_BPP8_8G_MEM(li32Index,      lcBm) (uint8) ((uint8* )lcBm.mpui8Data)[(li32Index)]
#define BM_BPP8_8G_PXL(liGfxX, liGfxY, lcBm) BM_BPP8_8G_MEM(BM_IDX((liGfxX), (liGfxY), (lcBm)), (lcBm))

class cBitmap_Bpp8_8G : public cBitmap
{
  public:
    cBitmap_Bpp8_8G(GfxInt liGfxWidth, GfxInt liGfxHeight, u8* lpui8Data) : cBitmap( liGfxWidth, liGfxHeight,BM_BPP8_8G, lpui8Data)  {};
    cBitmap_Bpp8_8G(GfxInt liGfxWidth, GfxInt liGfxHeight)                : cBitmap( liGfxWidth, liGfxHeight,BM_BPP8_8G) {};
    ~cBitmap_Bpp8_8G() {};

    BM_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx);};
    BM_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx);};

    BM_INLINE void vFill(u8 lui8Col)
    {
      cMemTools::vMemSet(mpui8Data, lui8Col, u32Size());
    }

    BM_INLINE void vPp(GfxInt liGfxX, GfxInt liGfxY, u8 lui8Col)
    {
      mpui8Data[u32PxlIdx(liGfxX, liGfxY)] = lui8Col;
    }

    BM_INLINE u8 u8Gp(GfxInt liGfxX, GfxInt liGfxY)
    {
      return mpui8Data[u32PxlIdx(liGfxX, liGfxY)];
    }
};


#ifdef __cplusplus
}
#endif

#endif  //BITMAP_BPP8_8G
