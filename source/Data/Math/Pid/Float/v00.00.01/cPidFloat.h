

#ifndef _PIDFLOAT_H
#define _PIDFLOAT_H


#include "typedef.h"

class cFloatLowPass
{
public:
  float mValue;
  float mFactor;

  cFloatLowPass(float lfFactor)
  {
    mFactor = lfFactor;
    mValue  = 0.0;
  }

  float DoProcess(float lfNewValue) // __attribute__((optimize("-O0")))
  {
    mValue = mFactor * lfNewValue + (1.0f - mFactor) * mValue;
    return mValue;
  }
};


class cFloatPid
{
public:
  float mfErrOld;

  float mfErrIntegral;
  float mfDeltaError;

  cFloatLowPass mLpOutput;

  float mfKp;
  float mfKi;
  float mfKd;

  float mfOutputLimit;

  cFloatPid(float lfLpInitValue)
    : mLpOutput(lfLpInitValue)
  {
    mfErrOld = 0;
    mfErrIntegral = 0;
    mfDeltaError = 0;
  }

  float fDoProcess(float lFpErr) // __attribute__((optimize("-O0")))
  {
    mfDeltaError = lFpErr - mfErrOld;
    mfErrOld = lFpErr;

    // PID
    float v = mfKp * lFpErr +
              mfErrIntegral +
              mfKd * mfDeltaError;

    bool saturating = False;

    if      (v >=  mfOutputLimit)  { v =  mfOutputLimit; saturating = True; }
    else if (v <= -mfOutputLimit)  { v = -mfOutputLimit; saturating = True; }

    // error and output same sign
    bool sign = (lFpErr > 0) && (v > 0);
    // zero
    bool clamp = saturating && sign;
    if (!clamp)
    {
      mfErrIntegral += (mfKi * lFpErr);
    }

    // output
    return mLpOutput.DoProcess(v);
  }
};

#endif /* _PIDFLOAT_H */
