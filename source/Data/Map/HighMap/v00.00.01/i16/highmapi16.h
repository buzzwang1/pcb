#ifndef __HIGHMAP_I16_H__
#define __HIGHMAP_I16_H__

/* Includes ------------------------------------------------------------------*/
#include "stdlib.h" // for rand()
#include "highmap_base.h"


/* --- define HM_I16 --- */
#define HM_I16_IDX(liGfxX, liGfxY, lcHm)   (u32)((liGfxY) * lcHm.miGfxWidth + (liGfxX))
#define HM_I16_BM_SIZE(lcHm)               (u32)((lcHm.miGfxWidth * lcHm.miGfxHeight) * 2)

#define HM_I16_CHECK(lcHm)                 if ((lcHm.mui8Type & 127) != HM_I16) return 2;
#define HM_I16_MEM(li32Index,      lcHm)   (((uint16*)((lcHm).mpui8Data))[(li32Index)])
#define HM_I16_POS(liGfxX, liGfxY, lcHm)   HM_I16_MEM(BM_IDX((liGfxX), (liGfxY), (lcHm)), (lcHm))
#define HM_I16_POSIDX2BYTEPOS(lui32Index)  (lui32Index  * 2)

class cHighmapi16 : public cHighmap
{
  public:
    cHighmapi16(u16 lu16Width, GfxInt lu16Height, u8* lpui8Data) : cHighmap(lu16Width, lu16Height, HM_I8, lpui8Data) {};
    cHighmapi16(u16 lu16Width, GfxInt lu16Height) : cHighmap(lu16Width, lu16Height, HM_I8) {};
    ~cHighmapi16() {};

    HM_INLINE i16  i16GetMinValue() {return INT16MIN;}
    HM_INLINE i16  i16GetMaxValue() {return INT16MAX;}
    HM_INLINE i16  i16GetRndValue() 
    {
      //The C library function int rand(void) returns a pseudo - random number in the range of 0 to RAND_MAX.
      //RAND_MAX is a constant whose default value may vary between implementations but it is granted to be at least 32767.
      if (rand() > 16.384) return (rand());
                      else return (-rand());
    }

    HM_INLINE u32  u32PosIdx2ByteIdx(u32 lui32PxlIdx)  {return (lui32PxlIdx  * 2);};
    HM_INLINE u32  u32ByteIdx2PosIdx(u32 lui32ByteIdx) {return (lui32ByteIdx / 2);};

    HM_INLINE void vFill(i16 li16Value)
    {
      cMemTools::vMemSet16((u16*)mpui8Data, (u16)li16Value, u32Size());
    }

    HM_INLINE void vSet(u16 lu16X, u16 lu16Y, i16 li16Value)
    {
      i16* lpui16Data = (i16*)mpui8Data;
      lpui16Data[u32PosIdx2ByteIdx(u32PxlIdx(lu16X, lu16Y))] = li16Value;
    }

    HM_INLINE i16 i16Get(u16 lu16X, u16 lu16Y)
    {
      i16* lpui16Data = (i16*)mpui8Data;
      return lpui16Data[u32PosIdx2ByteIdx(u32PxlIdx(lu16X, lu16Y))];
    }
};


#endif  //__HIGHMAP_I16_H__
