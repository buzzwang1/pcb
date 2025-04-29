

#ifndef _FIXPTI168_H
#define _FIXPTI168_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "typedef.h"

// The order must be changed in respect of the MSB
/*
typedef struct
{
  uint8 u8Dummy;
  int16 i16Num;
  uint8 u8Frac;
}FPi168_tstFPi168; */

typedef struct
{
  int32  u8Frac:8;
  int32  i16Num:16;
  int32  u8Dummy:8;
}FPi168_tstFPi168int;

typedef struct
{
  uint32  u8Frac:8;
  uint32  ui16Num:16;
  uint32  u8Dummy:8;
}FPi168_tstFPi168uint;

typedef union
{
  uint32                u32FPi168;
  FPi168_tstFPi168int   stFPi168i;
  FPi168_tstFPi168uint  stFPi168u;
}FPi168_tunFPi168;

#define FIXPTI168_SHIFT     8
#define FIXPTI168_SQRT2   362  //1.4142 * 256
#define FIXPTI168_SQRT0_5 181  //0.7071 * 256

// i16.u8
// 1    => 256      , 00 01 00
// 2    => 512      , 00 02 00
// -1   => 167776960, FF FF 00
// 0,5  => 128,       00 00 80
//-0,5  =>-128,       FF FF 80
// 1,5  => 384,       00 01 80
//-1,5  =>-384,       FF FE 80
// 1,25 => 320,       00 01 40
//-1,25 =>-320,       FF FE C0

#define FPi168_vSetMin(unFPi168)                               unFPi168.u32FPi168    = (INT16MIN << FIXPTI168_SHIFT) + UINT8MAX
#define FPi168_vSetMax(unFPi168)                               unFPi168.u32FPi168    = (INT16MAX << FIXPTI168_SHIFT) + UINT8MAX
#define FPi168_vSetZero(unFPi168)                              unFPi168.u32FPi168    = 0

//#define FPi168_vSet(unFPi168Dest, li16Val,      lu8Frac)       unFPi168Dest.stFPi168.u8Dummy = 0; unFPi168Dest.stFPi168.i16Num = li16Val; unFPi168Dest.stFPi168.u8Frac = lu8Frac
#define FPi168_vSet(unFPi168Dest, li16Val,      lu8Frac)       unFPi168Dest.u32FPi168 = (((uint32)li16Val) << FIXPTI168_SHIFT) + lu8Frac


#define FPi168_vCpy(   unFPi168Src,  unFPi168Dest)                unFPi168Dest.u32FPi168  = unFPi168Src.u32FPi168
#define FPi168_vAdd(   unFPi168Src1, unFPi168Src2, unFPi168Dest)  unFPi168Dest.u32FPi168  = unFPi168Src1.u32FPi168 + unFPi168Src2.u32FPi168
#define FPi168_vAddNum(li16Val,      unFPi168Dest)                unFPi168Dest.u32FPi168 += (((uint32)li16Val) << FIXPTI168_SHIFT)
#define FPi168_vSub(   unFPi168Src1, unFPi168Src2, unFPi168Dest)  unFPi168Dest.u32FPi168  = unFPi168Src1.u32FPi168 - unFPi168Src2.u32FPi168
#define FPi168_vMulNum(li16Val,      unFPi168Dest)                unFPi168Dest.u32FPi168 *= (li16Val)
#define FPi168_vMul(   unFPi168Src1, unFPi168Src2, unFPi168Dest)  unFPi168Dest.u32FPi168  = (unFPi168Src1.u32FPi168 * unFPi168Src2.u32FPi168) >> FIXPTI168_SHIFT
#define FPi168_vDiv(   unFPi168Src1, unFPi168Src2, unFPi168Dest)  unFPi168Dest.u32FPi168  = (unFPi168Src1.u32FPi168 << FIXPTI168_SHIFT) / unFPi168Src2.u32FPi168

void FPi168_vDiv_Save(FPi168_tunFPi168 unFPi168Src1, FPi168_tunFPi168 unFPi168Src2, FPi168_tunFPi168* unFPi168Dest);


#ifdef __cplusplus
}
#endif

#endif /* _FIXPTI168_H */
