#ifndef __BITMAP_BPP32_8R8G8B_H__
#define __BITMAP_BPP32_8R8G8B_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap_base.h"

#ifdef __cplusplus
 extern "C" {
#endif


typedef tstBmARGB   tstRGB32;
typedef unBmBaseCol tunBPP32_8R8G8B_RGB;

/* --- BPP32_8R8G8B  --- */
#define BM_BPP32_8R8G8B_IDX(liGfxX, liGfxY, lcBm) (u32)((liGfxY) * lcBm.miGfxWidth + (liGfxX))
#define BM_BPP32_8R8G8B_BM_SIZE(lcBm)             (u32)((lcBm.miGfxWidth * lcBm.miGfxHeight) * 4)

#define BM_BPP32_8R8G8B_CHECK(lcBm)               if ((lcBm.mui8Type & 127) != BM_BPP32_8R8G8B) return 2;
#define BM_BPP32_8R8G8B_MEM(li32Index,      lcBm) ((uint32*)lcBm.mpui8Data)[(li32Index)]
#define BM_BPP32_8R8G8B_PXL(liGfxX, liGfxY, lcBm) BM_BPP32_8R8G8B_MEM(BM_IDX((liGfxX), (liGfxY), (lcBm)), (lcBm))

//Works always but slower
#define BM_BPP32_8R8G8B_RGB(li32r, li32g, li32b)  ((((li32r) << 8) + (li32g)) << 8) + (li32b)
//faster, but be careful with little and big endian
#define BM_BPP32_8R8G8B_GET_RGB(lui32Col, li32r, li32g, li32b)  li32r = ((uint8*)(&lui32Col))[2]; li32g = ((uint8*)(&lui32Col))[1]; li32b = ((uint8*)(&lui32Col))[0];
#define BM_BPP32_8R8G8B_GET_R(lui32Col)  ((uint8*)(&lui32Col))[2]
#define BM_BPP32_8R8G8B_GET_G(lui32Col)  ((uint8*)(&lui32Col))[1]
#define BM_BPP32_8R8G8B_GET_B(lui32Col)  ((uint8*)(&lui32Col))[0]

#define BM_BPP32_8R8G8B_BLACK     BM_BPP32_8R8G8B_RGB(   0,    0,    0)
#define BM_BPP32_8R8G8B_GREY1     BM_BPP32_8R8G8B_RGB(0x3F, 0x3F, 0x3F)
#define BM_BPP32_8R8G8B_GREY2     BM_BPP32_8R8G8B_RGB(0x7F, 0x7F, 0x7F)
#define BM_BPP32_8R8G8B_GREY3     BM_BPP32_8R8G8B_RGB(0xBF, 0xBF, 0xBF)
#define BM_BPP32_8R8G8B_WHITE     BM_BPP32_8R8G8B_RGB(0xFF, 0xFF, 0xFF)
#define BM_BPP32_8R8G8B_RED       BM_BPP32_8R8G8B_RGB(0xFF,    0,    0)
#define BM_BPP32_8R8G8B_BLUE      BM_BPP32_8R8G8B_RGB(   0,    0, 0xFF)
#define BM_BPP32_8R8G8B_GREEN     BM_BPP32_8R8G8B_RGB(   0, 0xFF,    0)

class cBitmap_Bpp32_8R8G8B : public cBitmap
{
  public:
    cBitmap_Bpp32_8R8G8B(GfxInt liGfxWidth, GfxInt liGfxHeight, u8* lpui8Data) : cBitmap( liGfxWidth, liGfxHeight,BM_BPP32_8R8G8B, lpui8Data) {};
    cBitmap_Bpp32_8R8G8B(GfxInt liGfxWidth, GfxInt liGfxHeight)                : cBitmap( liGfxWidth, liGfxHeight,BM_BPP32_8R8G8B) {};
    ~cBitmap_Bpp32_8R8G8B() {};

    BM_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  * 4);};
    BM_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx / 4);};

    BM_INLINE void vFill(u32 lui32Col)
    {
      // Size gibt in Byte zurück
      // vMemSet16 braucht es in 32Bit, daher /4
      cMemTools::vMemSet32((u32*)mpui8Data, lui32Col, u32Size() / 4);
    }

    BM_INLINE void vPp(GfxInt liGfxX, GfxInt liGfxY, u32 lui32Col)
    {
      u32* lpui32Data = (u32*)mpui8Data;
      lpui32Data[u32PxlIdx(liGfxX, liGfxY)] = lui32Col;
    }

    BM_INLINE u32 u32Gp(GfxInt liGfxX, GfxInt liGfxY)
    {
      u32* lpui32Data = (u32*)mpui8Data;
      return lpui32Data[u32PxlIdx(liGfxX, liGfxY)];
    }
};


#ifdef __cplusplus
}
#endif

#endif  //BITMAP_BPP32_8R8G8B
