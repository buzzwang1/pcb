#ifndef __TEXTURE_32_BPP16_5R6G5B_H__
#define __TEXTURE_32_BPP16_5R6G5B_H__

/* Includes ------------------------------------------------------------------*/
#include "texture32_base.h"

#ifdef __cplusplus
 extern "C" {
#endif


/* --- define BM_BPP16_5R6G5B --- */
#define TX32_BPP16_5R6G5B_IDX(liGfxX, liGfxY, lcBm)   (u32)((liGfxY) * 32 + (liGfxX))
#define TX32_BPP16_5R6G5B_BM_SIZE(lcBm)               (u32)((32 * 32) * 2)

#define TX32_BPP16_5R6G5B_CHECK(lcBm)                 if ((lcBm.mui8Type & 127) != BM_BPP16_5R6G5B) return 2;
#define TX32_BPP16_5R6G5B_MEM(li32Index,      lcBm)   (uint16)((uint16*)lcBm.mpui8Data)[(li32Index)]
#define TX32_BPP16_5R6G5B_PXL(liGfxX, liGfxY, lcBm)   TX32_BPP16_5R6G5B_MEM(BM_IDX((liGfxX), (liGfxY), (lcBm)), (lcBm))
#define TX32_BPP16_5R6G5B_PXLIDX2BYTEPOS(lui32Index)  (lui32Index  * 2)

class cTexture32_Bpp16_5R6G5B : public cTexture32
{
  public:
    cTexture32_Bpp16_5R6G5B(u8* lpui8Data) : cTexture32(BM_BPP16_5R6G5B, lpui8Data)  {};
    cTexture32_Bpp16_5R6G5B()              : cTexture32(BM_BPP16_5R6G5B) {};
    ~cTexture32_Bpp16_5R6G5B() {};

    TX32_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  * 2);};
    TX32_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx / 2);};

    TX32_INLINE void vFill(u16 lui16Col)
    {
      cMemTools::vMemSet16((u16*)mpui8Data, lui16Col, u32Size());
    }

    TX32_INLINE void vPp(GfxInt liGfxX, GfxInt liGfxY, u16 lui16Col)
    {
      u16* lpui16Data = (u16*)mpui8Data;
      lpui16Data[u32PxlIdx(liGfxX, liGfxY)] = lui16Col;
    }

    TX32_INLINE u16 u16Gp(GfxInt liGfxX, GfxInt liGfxY)
    {
      u16* lpui16Data = (u16*)mpui8Data;
      return lpui16Data[u32PxlIdx(liGfxX, liGfxY)];
    }

    void Test_Pattern_Flat(u16 lu16Col)
    {
      vFill(lu16Col);
    }

    void Test_Pattern_2x2(u16 lu16Col1, u16 lu16Col2, u16 lu16ColBg)
    {
      vFill(lu16ColBg);
      for (int x = 0; x < 16; x++)
      {
        for (int y = 0; y < 16; y++)
        {
          vPp(x,    y,    lu16Col1);
          vPp(x+16, y+16, lu16Col2);
        }
      }
    }

    void Test_Pattern_4x4(u16 lu16Col1, u16 lu16Col2, u16 lu16ColBg)
    {
      vFill(lu16ColBg);
      for (int x = 0; x < 8; x++)
      {
        for (int y = 0; y < 8; y++)
        {
          vPp(x,    y,    lu16Col1);
          vPp(x+8,  y+8,  lu16Col2);
          vPp(x+16, y+16, lu16Col1);
          vPp(x+24, y+24, lu16Col2);
        }
      }
    }
};


#ifdef __cplusplus
}
#endif

#endif  //TEXTURE_32_BPP16_5R6G5B
