#ifndef _FIXPTI2210_H
#define _FIXPTI2210_H


#include "typedef.h"

struct cFixPti2210
{
  // https://sunshine2k.de/articles/coding/fp/sunfp.html
  enum
  {
    nNumIntBits = 22,
    nNumFracBits = 10,
    nMaskFracBits = (1 << 10) - 1
  };

  typedef struct
  {
    u32 Frac : 10;
    i32 Int : 22;
  }tstFp;

  typedef union
  {
    i32   i32Fp;
    tstFp stFp;
  }tunFp;

  tunFp mFp;

  cFixPti2210()
  {
    vSet((int32)0);
  }

  cFixPti2210(const cFixPti2210& lcNum)
  {
    *this = lcNum;
  }

  cFixPti2210(const i32 li32Num)
  {
    vSet(li32Num);
  }

  cFixPti2210(const float lfNum)
  {
    vSet(lfNum);
  }

  void vSet(const float lfNum)
  {
    mFp.i32Fp = ((i32)(lfNum * (float)(1 << nNumFracBits)));
  }

  void vSet(const i32 li32Num)
  {
    mFp.i32Fp = li32Num << nNumFracBits;
  }

  void vSetFrac(const i32 li32Num)
  {
    mFp.i32Fp = li32Num;
  }

  void vSet(const cFixPti2210& lcFp)
  {
    mFp.i32Fp = lcFp.mFp.i32Fp;
  }

  i32 i32Get()
  {
    //Runden
    if ((mFp.stFp.Frac > 0) && (mFp.stFp.Frac > (nNumFracBits / 2))) return (mFp.stFp.Int + 1);
    if ((mFp.stFp.Frac < 0) && (mFp.stFp.Frac > (nNumFracBits / 2))) return (mFp.stFp.Int - 1);
    return mFp.stFp.Int;
  }

  float fGet()
  {
    return ((float)(mFp.i32Fp / (float)(1 << nNumFracBits)));
  }

  void vAdd(cFixPti2210 lcFp) // __attribute__((optimize("-O0")))
  {
    mFp.i32Fp += lcFp.mFp.i32Fp;
  }

  void vSub(cFixPti2210 lcFp)
  {
    mFp.i32Fp -= lcFp.mFp.i32Fp;
  }

  bool isIn(cFixPti2210 lcMinMax)
  {
    if ((*this >= (-lcMinMax)) && (*this <= lcMinMax)) return True;
    return false;
  }

  void vMul(cFixPti2210 lcFp) // __attribute__((optimize("-O0")))
  {
    i32 intPart1 = mFp.i32Fp >> nNumFracBits;
    i32 intPart2 = lcFp.mFp.i32Fp >> nNumFracBits;

    i32 fracPart1 = mFp.i32Fp & nMaskFracBits;
    i32 fracPart2 = lcFp.mFp.i32Fp & nMaskFracBits;


    mFp.i32Fp = (intPart1 * intPart2) << nNumFracBits;
    mFp.i32Fp += (intPart1 * fracPart2);
    mFp.i32Fp += (fracPart1 * intPart2);
    mFp.i32Fp += ((fracPart1 * fracPart2) >> nNumFracBits) & nMaskFracBits;
  }

  i8 i8Sign()
  {
    i32 intPart = mFp.i32Fp >> nNumFracBits;

    if (intPart > 0) return 1;
    return -1;
  }

  // https://en.cppreference.com/w/cpp/language/operators
  // copy assignment
  cFixPti2210& operator=(const cFixPti2210& lFpOther) { vSet(lFpOther); return *this; }
  cFixPti2210& operator=(const i32& li32Other) { vSet(li32Other); return *this; }
  cFixPti2210& operator=(const float& lfOther) { vSet(lfOther); return *this; }

  cFixPti2210& operator += (const cFixPti2210& num) { vAdd(num); return *this; }
  cFixPti2210& operator -= (const cFixPti2210& num) { vSub(num); return *this; }
  cFixPti2210& operator *= (const cFixPti2210& num) { vMul(num); return *this; }


  cFixPti2210 operator-() { cFixPti2210 lZwerg(*this); lZwerg.mFp.i32Fp = -lZwerg.mFp.i32Fp; return lZwerg; }

  bool operator==(const cFixPti2210& lhs) { return  (mFp.i32Fp == lhs.mFp.i32Fp); }
  bool operator!=(const cFixPti2210& lhs) { return !(*this == lhs); }

  //bool operator > (const cFixPti2210& lhs)  { return (lhs.mFp.i32Fp  > mFp.i32Fp); }
  //bool operator < (const cFixPti2210& lhs)  { return (lhs.mFp.i32Fp  < mFp.i32Fp); }
  //bool operator >=(const cFixPti2210& lhs) { return (lhs.mFp.i32Fp >= mFp.i32Fp); }
  //bool operator <=(const cFixPti2210& lhs) { return (lhs.mFp.i32Fp <= mFp.i32Fp); }

  friend bool operator > (const cFixPti2210& lhs, const cFixPti2210& rhs) { return (lhs.mFp.i32Fp > rhs.mFp.i32Fp); }
  friend bool operator < (const cFixPti2210& lhs, const cFixPti2210& rhs) { return (lhs.mFp.i32Fp < rhs.mFp.i32Fp); }
  friend bool operator >=(const cFixPti2210& lhs, const cFixPti2210& rhs) { return (lhs.mFp.i32Fp >= rhs.mFp.i32Fp); }
  friend bool operator <=(const cFixPti2210& lhs, const cFixPti2210& rhs) { return (lhs.mFp.i32Fp <= rhs.mFp.i32Fp); }

  operator float() { return (float)fGet(); }
  operator i32() { return (i32)i32Get(); }

  u8* u8Encode(u8* lu8Data)
  {
    lu8Data[0] = (u8)(mFp.i32Fp >> 24);
    lu8Data[1] = (u8)(mFp.i32Fp >> 16);
    lu8Data[2] = (u8)(mFp.i32Fp >> 8);
    lu8Data[3] = (u8)(mFp.i32Fp);
    return lu8Data + 4;
  }
  
  u8* u8Decode(u8* lu8Data)
  {
     mFp.i32Fp = (lu8Data[0] << 24) + (lu8Data[1] << 16) + (lu8Data[2] << 8) + lu8Data[3];
     return lu8Data + 4;
  }
};


cFixPti2210 operator+ (const cFixPti2210& x, const cFixPti2210 y) { cFixPti2210 lZwerg = x; lZwerg.vAdd(y); return lZwerg; }
cFixPti2210 operator- (const cFixPti2210& x, const cFixPti2210 y) { cFixPti2210 lZwerg = x; lZwerg.vSub(y); return lZwerg; }
cFixPti2210 operator* (const cFixPti2210& x, const cFixPti2210 y) { cFixPti2210 lZwerg = x; lZwerg.vMul(y); return lZwerg; }


#endif /* _FIXPTI2210_H */
