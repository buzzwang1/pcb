#ifndef __BITMAP_BPP16_5R6G5B_H__
#define __BITMAP_BPP16_5R6G5B_H__

/* Includes ------------------------------------------------------------------*/
#include "bitmap_base.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
  uint16 b:5; // Blue
  uint16 g:6; // Green
  uint16 r:5; // Red
}tstRGB16;

typedef union  // Deklaration eines neuen Datentyps
{
  uint16   ui16RGB;
  tstRGB16 stRGB;
}tunBPP16_5R6G5B_RGB;

/* --- define BM_BPP16_5R6G5B --- */
#define BM_BPP16_5R6G5B_IDX(liGfxX, liGfxY, lcBm) (u32)((liGfxY) * lcBm.miGfxWidth + (liGfxX))
#define BM_BPP16_5R6G5B_BM_SIZE(lcBm)             (u32)((lcBm.miGfxWidth * lcBm.miGfxHeight) * 2)

#define BM_BPP16_5R6G5B_CHECK(lcBm)               if ((lcBm.mui8Type & 127) != BM_BPP16_5R6G5B) return 2;
#define BM_BPP16_5R6G5B_MEM(li32Index,      lcBm) (((uint16*)((lcBm).mpui8Data))[(li32Index)])
#define BM_BPP16_5R6G5B_PXL(liGfxX, liGfxY, lcBm) BM_BPP16_5R6G5B_MEM(BM_IDX((liGfxX), (liGfxY), (lcBm)), (lcBm))


// RGB
// red is most left
// rrrr.rggg gggb.bbbb
#define BM_BPP16_5R6G5B_RGB(li8r, li8g, li8b)                (uint16)(((((u16)(li8r)) << 6) + ((u16)(li8g))) << 5) + ((u16)(li8b))
#define BM_BPP16_5R6G5B_GET_RGB(lui16Col, li8r, li8g, li8b)  li8r = (lui16Col >> 11); li8g = ((lui16Col >> 5) & 63); li8b = (lui16Col & 31);
#define BM_BPP16_5R6G5B_GET_R(lui16Col)                      (lui16Col >> 11)
#define BM_BPP16_5R6G5B_GET_G(lui16Col)                      ((lui16Col >> 5) & 63)
#define BM_BPP16_5R6G5B_GET_B(lui16Col)                      (lui16Col & 31)


/*
// RGB inverse
// red is most right
// bbbb.bggg gggr.rrrr
#define BM_BPP16_5R6G5B_RGB(li8r, li8g, li8b)                (uint16)(((((u16)(li8b)) << 6) + ((u16)(li8g))) << 5) + ((u16)(li8r))
#define BM_BPP16_5R6G5B_GET_RGB(lui16Col, li8r, li8g, li8b)  li8r = (lui16Col & 31); li8g = ((lui16Col >> 5) & 63); li8b = (lui16Col >> 11);
#define BM_BPP16_5R6G5B_GET_R(lui16Col)                      (lui16Col & 31)
#define BM_BPP16_5R6G5B_GET_G(lui16Col)                      ((lui16Col >> 5) & 63)
#define BM_BPP16_5R6G5B_GET_B(lui16Col)                      (lui16Col >> 11)
*/

#define BM_BPP16_5R6G5B_BLACK     BM_BPP16_5R6G5B_RGB(  0,  0,  0)
#define BM_BPP16_5R6G5B_WHITE     BM_BPP16_5R6G5B_RGB( 31, 63, 31)
#define BM_BPP16_5R6G5B_RED       BM_BPP16_5R6G5B_RGB( 31,  0,  0)
#define BM_BPP16_5R6G5B_BLUE      BM_BPP16_5R6G5B_RGB(  0,  0, 31)
#define BM_BPP16_5R6G5B_GREEN     BM_BPP16_5R6G5B_RGB(  0, 63,  0)

#define BM_BPP16_5R6G5B_PXLIDX2BYTEPOS(lui32Index)           (lui32Index  * 2)

class cBitmap_Bpp16_5R6G5B : public cBitmap
{
  public:
    cBitmap_Bpp16_5R6G5B(GfxInt liGfxWidth, GfxInt liGfxHeight, u8* lpui8Data) : cBitmap( liGfxWidth, liGfxHeight,BM_BPP16_5R6G5B, lpui8Data)  {};
    cBitmap_Bpp16_5R6G5B(GfxInt liGfxWidth, GfxInt liGfxHeight)                : cBitmap( liGfxWidth, liGfxHeight,BM_BPP16_5R6G5B) {};
    ~cBitmap_Bpp16_5R6G5B() {};

    BM_INLINE u32  u32PxlIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  * 2);};
    BM_INLINE u32  u32ByteIdx2PxlIdx(u32 lui32ByteIdx) {return (lui32ByteIdx / 2);};

    BM_INLINE void vFill(u16 lui16Col)
    {
      // Size gibt in Byte zurück
      // vMemSet16 braucht es in 16Bit, daher /2
      cMemTools::vMemSet16((u16*)mpui8Data, lui16Col, u32Size() / 2); 
    }

    BM_INLINE void vPp(GfxInt liGfxX, GfxInt liGfxY, u16 lui16Col)
    {
      u16* lpui16Data = (u16*)mpui8Data;
      lpui16Data[u32PxlIdx(liGfxX, liGfxY)] = lui16Col;
    }

    BM_INLINE u16 u16Gp(GfxInt liGfxX, GfxInt liGfxY)
    {
      u16* lpui16Data = (u16*)mpui8Data;
      return lpui16Data[u32PxlIdx(liGfxX, liGfxY)];
    }
};


#ifdef __cplusplus
}
#endif

#endif  //BITMAP_BPP16_5R6G5B
