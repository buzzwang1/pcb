#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include <tuple>

#include "TypeDef.h"


struct cFixPti1814
{
  // https://sunshine2k.de/articles/coding/fp/sunfp.html
  enum
  {
    nNumIntBits = 18,
    nNumFracBits = 14,
    nMaskFracBits = (1 << 14) - 1
  };

  typedef struct
  {
    u32 Frac : 14;
    i32 Int : 18;
  }tstFp;

  typedef union
  {
    i32   i32Fp;
    tstFp stFp;
  }tunFp;

  tunFp mFp;

  cFixPti1814()
  {
    vSet((int32)0);
  }

  cFixPti1814(const cFixPti1814& lcNum)
  {
    *this = lcNum;
  }

  cFixPti1814(const i32 li32Num)
  {
    vSet(li32Num);
  }

  cFixPti1814(const float lfNum)
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

  void vSet(const cFixPti1814& lcFp)
  {
    mFp.i32Fp = lcFp.mFp.i32Fp;
  }

  i32 i32Get()
  {
    //Runden
    if ((mFp.stFp.Int >= 0) && (mFp.stFp.Frac >= ((1 << nNumFracBits) / 2))) return (mFp.stFp.Int + 1);
    if ((mFp.stFp.Int < 0)  && (mFp.stFp.Frac >  ((1 << nNumFracBits) / 2))) return (mFp.stFp.Int + 1);
    return mFp.stFp.Int;
  }

  float fGet()
  {
    return ((float)(mFp.i32Fp / (float)(1 << nNumFracBits)));
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
    i32 intPart = mFp.i32Fp >> nNumFracBits;

    if (intPart > 0) return 1;
    return -1;
  }

  // https://en.cppreference.com/w/cpp/language/operators
  // copy assignment
  cFixPti1814& operator=(const cFixPti1814& lFpOther) { vSet(lFpOther); return *this; }
  cFixPti1814& operator=(const i32& li32Other) { vSet(li32Other); return *this; }
  cFixPti1814& operator=(const float& lfOther) { vSet(lfOther); return *this; }

  cFixPti1814& operator += (const cFixPti1814& num) { vAdd(num); return *this; }
  cFixPti1814& operator -= (const cFixPti1814& num) { vSub(num); return *this; }
  cFixPti1814& operator *= (const cFixPti1814& num) { vMul(num); return *this; }


  cFixPti1814 operator-() { cFixPti1814 lZwerg(*this); lZwerg.mFp.i32Fp = -lZwerg.mFp.i32Fp; return lZwerg; }

  bool operator==(const cFixPti1814& lhs) { return  (mFp.i32Fp == lhs.mFp.i32Fp); }
  bool operator!=(const cFixPti1814& lhs) { return !(*this == lhs); }

  //bool operator > (const cFixPti1814& lhs)  { return (lhs.mFp.i32Fp  > mFp.i32Fp); }
  //bool operator < (const cFixPti1814& lhs)  { return (lhs.mFp.i32Fp  < mFp.i32Fp); }
  //bool operator >=(const cFixPti1814& lhs) { return (lhs.mFp.i32Fp >= mFp.i32Fp); }
  //bool operator <=(const cFixPti1814& lhs) { return (lhs.mFp.i32Fp <= mFp.i32Fp); }

  friend bool operator > (const cFixPti1814& lhs, const cFixPti1814& rhs) { return (lhs.mFp.i32Fp > rhs.mFp.i32Fp); }
  friend bool operator < (const cFixPti1814& lhs, const cFixPti1814& rhs) { return (lhs.mFp.i32Fp < rhs.mFp.i32Fp); }
  friend bool operator >=(const cFixPti1814& lhs, const cFixPti1814& rhs) { return (lhs.mFp.i32Fp >= rhs.mFp.i32Fp); }
  friend bool operator <=(const cFixPti1814& lhs, const cFixPti1814& rhs) { return (lhs.mFp.i32Fp <= rhs.mFp.i32Fp); }

  operator float() { return (float)fGet(); }
  operator i32() { return (i32)i32Get(); }
};


cFixPti1814 operator+ (const cFixPti1814& x, const cFixPti1814 y) { cFixPti1814 lZwerg = x; lZwerg.vAdd(y); return lZwerg; }
cFixPti1814 operator- (const cFixPti1814& x, const cFixPti1814 y) { cFixPti1814 lZwerg = x; lZwerg.vSub(y); return lZwerg; }
cFixPti1814 operator* (const cFixPti1814& x, const cFixPti1814 y) { cFixPti1814 lZwerg = x; lZwerg.vMul(y); return lZwerg; }


template <typename T> struct cProcess
{
  virtual T operator()(const T lVal) = 0;
};


template <typename T> struct cLowPassT : cProcess<T>
{
  T mValue;
  T mFactor;

  cLowPassT(const T lFactor)
  {
    mFactor = lFactor;
    mValue = T((const i32)0);
  }

  T DoProcess(const T lNewValue)
  {
    mValue = mFactor * lNewValue + (T((const i32)1) - mFactor) * mValue;
    return mValue;
  }

  T operator()(const T lVal)
  {
    return DoProcess(lVal);
  }

  operator float()
  {
    return (float)mValue;
  }

  operator i32()
  {
    return (i32)mValue;
  }
};


template <typename T> struct cClipT : cProcess<T>
{
  T mClip;

  cClipT(const T lClip)
  {
    mClip = lClip;
  }

  T operator()(const T lVal)
  {
    if      (lVal >  (mClip)) { return  (mClip);}
    else if (lVal < (-mClip)) { return (-mClip);}
    return lVal;
  }
};

template <typename T> struct cConvertT : cProcess<T>
{
  T mOffset;
  T mFactor;
  cClipT<T> mClip;

  cConvertT(const T lFactor, const T lOffset, const T lClip)
    : mClip(lClip)
  {
    mOffset = lOffset;
    mFactor = lFactor;
  }

  T DoProcess(const T lVal)
  {
    return mClip((mFactor * lVal - mOffset));
  }

  T operator()(const T lVal)
  {
    return DoProcess(lVal);
  }
};


template <typename T> struct cPidT : cProcess<T>
{
  T mKp;
  T mKi;
  T mKd;

  T mOutputLimit;

  T mErrOld;

  T mErrIntegral;
  T mDeltaError;

  cPidT(const T lKp, const T lKi, const T lKd, const T lClamp)
  {
    mKp = lKp;
    mKi = lKi;
    mKd = lKd;
    mOutputLimit = lClamp;

    mErrOld      = T((i32)0);
    mErrIntegral = T((i32)0);
    mDeltaError  = T((i32)0);
  }


  T DoProcess(const T lErr) // __attribute__((optimize("-O0")))
  {
    mDeltaError = lErr - mErrOld;
    mErrOld = lErr;

    // PID
    T v = mKp * lErr + mErrIntegral + mKd * mDeltaError;

    bool saturating = False;

    if (v >=  mOutputLimit) 
    { 
      v =  mOutputLimit; 
    }
    else if (v <= -mOutputLimit) 
    { 
      v = -mOutputLimit; 
    }

    if (mErrIntegral >= mOutputLimit)
    {
      mErrIntegral = mOutputLimit;
      saturating = True;
    }
    else if (mErrIntegral <= -mOutputLimit)
    {
      mErrIntegral = -mOutputLimit;
      saturating = True;
    }

    // error and output same sign
    bool clamp = False;
    if (saturating)
    {
      if (((lErr > T((i32)0)) && (v > T((i32)0)))) clamp = True;
      if (((lErr < T((i32)0)) && (v < T((i32)0)))) clamp = True;
    }

    if (!clamp)
    {
      mErrIntegral += (mKi * lErr);
    }

    // output
    return v;
  }

  T operator()(const T lVal)
  {
    return DoProcess(lVal);
  }
};


class PBArry
{
public:
  u8* mData;
  u16 mLen;
  u16 mSize;

  PBArry()
  {
    mLen = 0;
    mSize = 0;
    mData = 0;
  }

  PBArry(u8* lData, u16 lLen, u16 lSize)
  {
    mLen  = lLen;
    mSize = lSize;
    mData = lData;
  }

  PBArry& From(u8* lData, u16 lLen, u16 lSize)
  {
    mLen = lLen;
    mSize = lSize;
    mData = lData;
    return *this;
  }
};

template <u16 tu16Size>
class SBArry
{
public:
  u16 mLen;
  u8 mData[tu16Size];

  SBArry()
  {
    mLen = 0;
  }

  u16 Size()
  {
    return tu16Size;
  }
};


template <typename T>
class BArry : public T
{
public:

  BArry() : T() {}

  BArry& Set(const u8* lpuData, u16 const luLen)
  {
    if (luLen > Size()) mLen = Size();
                   else mLen = luLen; 
    memcpy(mData, lpuData, mLen);
    return *this;
  };

  BArry& Set(u8 lu8Byte)
  {
    mData[0] = lu8Byte;
    mLen = 1;
    return *this;
  };

  BArry& Set(PBArry const& lcData)
  {
    return Set(lcData.mData, lcData.mLen);
  };

  //operator PBArry() const
  //{
  //  return PBArry((u8*)mData, mLen, Size());
  //}

  PBArry& To(PBArry& lcPBArry)
  {
    return lcPBArry.From((u8*)mData, mLen, Size());
  }

  BArry& operator=(u8 lu8Byte) { return Set(lu8Byte); };
  BArry& operator=(PBArry const& lcData) { return Set(lcData); };
};

class Test
{
public: 
  PBArry* mMsg;


  Test(PBArry* lMsg)
  {
    mMsg = lMsg;
  }
};

void main(void)
{
  cCpp11::Rvalue_references();

  cCpp11_Lambda::Example1();
  cCpp11_Lambda::Example2();

  {
    cFixPti1814 T1;
    cFixPti1814 T2;

    T1 = 1.5f; T2 = -1.5f;
    printf("Test: %i ", (i32)T1);
    printf("Test: %i\n", (i32)T2);
  }

  cPidT<float> mcInPidPos(10.0f, 1.0f, 0.0f, 100.0f);
  cPidT<float> mcInPidCur( 5.0f, 1.0f, 0.0f, 100.0f);
  
  float mcOutPos_Pwm;
  float mcOutPos_Pwm2;
  float mcOutPosSoll =  0.0f;
  float mcOutPosIst  = -5.0f;

  //mcOutPosSoll = 0.0f;
  //mcOutPosIst = +5.0f;
  //for (i16 t = 0; t < 20; t++)
  //{
  //  mcOutPos_Pwm = mcInPidPos(mcOutPosSoll - mcOutPosIst);
  //  printf("%i: Test: %f - %f\n", t, mcOutPos_Pwm, mcInPidPos.mErrIntegral);
  //}

  float mcOutCur_Pwm;
  mcOutPosSoll = 0.0f;
  mcOutPosIst = -5.0f;
  for (i16 t = 0; t < 20; t++)
  {
    mcOutPos_Pwm = mcInPidPos(mcOutPosSoll - mcOutPosIst);
    printf("%i: Test: %f - %f\n", t, mcOutPos_Pwm, mcInPidPos.mErrIntegral);
  }

  float mcOutCurLimit = 100.0f;
  float mcOutCurIst = 50.0f;

  mcOutCurLimit = 100.0f;
  mcOutCurIst   = 105.0f;
  for (i16 t = 0; t < 20; t++)
  {
    mcOutCur_Pwm = -mcInPidCur(mcOutCurLimit - mcOutCurIst);

    mcOutPos_Pwm2 = mcOutPos_Pwm;
    if (mcOutCur_Pwm > 0)
    {
      if ((mcOutPos_Pwm2 > 0) && ( mcOutPos_Pwm >= mcOutCur_Pwm))  mcOutPos_Pwm2 -= mcOutCur_Pwm;
      if ((mcOutPos_Pwm2 < 0) && (-mcOutPos_Pwm >= mcOutCur_Pwm))  mcOutPos_Pwm2 += mcOutCur_Pwm;
    }

    printf("%i: Test: %f : %f  PWM:%f\n", t, mcOutCur_Pwm, mcInPidCur.mErrIntegral, mcOutPos_Pwm2);
  }

  mcOutCurLimit = 100.0f;
  mcOutCurIst = 90.0f;
  for (i16 t = 0; t < 20; t++)
  {
    mcOutCur_Pwm = -mcInPidCur(mcOutCurLimit - mcOutCurIst);

    mcOutPos_Pwm2 = mcOutPos_Pwm;
    if (mcOutCur_Pwm > 0)
    {
      if ((mcOutPos_Pwm2 > 0) && (mcOutPos_Pwm >= mcOutCur_Pwm))  mcOutPos_Pwm2 -= mcOutCur_Pwm;
      if ((mcOutPos_Pwm2 < 0) && (-mcOutPos_Pwm >= mcOutCur_Pwm))  mcOutPos_Pwm2 += mcOutCur_Pwm;
    }

    printf("%i: Test: %f : %f  PWM:%f\n", t, mcOutCur_Pwm, mcInPidCur.mErrIntegral, mcOutPos_Pwm2);
  }

  mcOutCurLimit = 100.0f;
  mcOutCurIst = 105.0f;
  for (i16 t = 0; t < 20; t++)
  {
    mcOutCur_Pwm = -mcInPidCur(mcOutCurLimit - mcOutCurIst);

    mcOutPos_Pwm2 = mcOutPos_Pwm;
    if (mcOutCur_Pwm > 0)
    {
      if ((mcOutPos_Pwm2 > 0) && (mcOutPos_Pwm >= mcOutCur_Pwm))  mcOutPos_Pwm2 -= mcOutCur_Pwm;
      if ((mcOutPos_Pwm2 < 0) && (-mcOutPos_Pwm >= mcOutCur_Pwm))  mcOutPos_Pwm2 += mcOutCur_Pwm;
    }

    printf("%i: Test: %f : %f  PWM:%f\n", t, mcOutCur_Pwm, mcInPidCur.mErrIntegral, mcOutPos_Pwm2);
  }


  
  BArry<SBArry<2>> lcBarry1;
  BArry<SBArry<4>> lcBarry2;

  lcBarry1 = 2;
  lcBarry2 = 4;

  PBArry C;
  lcBarry1 = lcBarry2.To(C);
  Test D(&C);

  //lcBarry1 = (PBArry)lcBarry2;


  //
  //SBArry<8> *lcBarry1P = &lcBarry1;
  //SBArry<2> *lcBarry2P = (SBArry<2>*)&lcBarry1;
  //
  //
  //lcBarry2.vSet(lcBarry1.Get());
  //lcBarry2 = lcBarry1.Get();
  //lcBarry2 = (SBArry<2>*) &lcBarry1;

  printf("\nEnde\n");

  //float lPos = T1(2048.0f);
  //
  //cMotT<float> lfMot(0.1f, 1.0f, 1.0f, 1.0f, 100.0f, 0.1f);
  //
  //lfMot(30.0f);
  
  //printf("Test: %i ", mcOutPos_Pwm);
}


