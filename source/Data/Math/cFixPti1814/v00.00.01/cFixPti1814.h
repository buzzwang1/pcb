

#ifndef _FIXPTI1814_H
#define _FIXPTI1814_H


#include "typedef.h"

class cFixPti1814
{
  public:
  // https://sunshine2k.de/articles/coding/fp/sunfp.html
  enum
  {
    nNumIntBits   = 18,
    nNumFracBits  = 14,
    nMaskFracBits = (1<<14)-1
  };

  typedef struct
  {
    i32 Frac:14;
    i32 Int:18;
  }tstFp;

  typedef union
  {
    i32   i32Fp;
    tstFp stFp;
  }tunFp;

  tunFp mFp;

  cFixPti1814()
  {
    vSet(0);
  }

  cFixPti1814(i32 li32Num)
  {
    vSet(li32Num);
  }

  void vSet(i32 li32Num)
  {
    mFp.i32Fp = li32Num << nNumFracBits;
  }

  void vSetFrac(i32 li32Num)
  {
    mFp.i32Fp = li32Num;
  }

  void vSet(cFixPti1814 lcFp)
  {
    mFp.i32Fp = lcFp.mFp.i32Fp;
  }

  i32 i32Get()
  {
    i32 intPart  = mFp.i32Fp >> nNumFracBits;
    i32 fracPart = mFp.i32Fp & nMaskFracBits;

    //Runden
    if (fracPart > (nNumFracBits / 2)) intPart++;

    return intPart;
  }

  void vAdd(cFixPti1814 lcFp) // __attribute__((optimize("-O0")))
  {
    mFp.i32Fp += lcFp.mFp.i32Fp;
  }

  void vSub(cFixPti1814 lcFp)
  {
    mFp.i32Fp -= lcFp.mFp.i32Fp;
  }

  void vMul(cFixPti1814 lcFp) // __attribute__((optimize("-O0")))
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
    i32 intPart  = mFp.i32Fp >> nNumFracBits;

    if (intPart > 0) return 1;
    return -1;
  }

  // https://en.cppreference.com/w/cpp/language/operators
  // copy assignment
  cFixPti1814& operator=(const cFixPti1814& lFpOther)
  {
    vSet(lFpOther);
    return *this; // return the result by reference
  }


  cFixPti1814& operator=(const int32& li32Other)
  {
    vSet(li32Other);
    return *this; // return the result by reference
  }

  cFixPti1814& operator += (const cFixPti1814& num) { vMul(num); return *this; }
  friend cFixPti1814 operator + (const cFixPti1814& num1, const cFixPti1814& num2) { cFixPti1814 tmp(num1); tmp.vAdd(num2); return tmp; }

  cFixPti1814& operator -= (const cFixPti1814& num) { vMul(num); return *this; }
  friend cFixPti1814 operator - (const cFixPti1814& num1, const cFixPti1814& num2) { cFixPti1814 tmp(num1); tmp.vSub(num2); return tmp; }


  cFixPti1814& operator *= (const cFixPti1814& num) { vMul(num); return *this; }
  friend cFixPti1814 operator * (const cFixPti1814& num1, const cFixPti1814& num2) { cFixPti1814 tmp(num1); tmp.vMul(num2); return tmp; }
};

#endif /* _FIXPTI1814_H */
