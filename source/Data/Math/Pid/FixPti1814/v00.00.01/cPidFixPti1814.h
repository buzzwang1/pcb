

#ifndef _PIDFIXPTI1814_H
#define _PIDFIXPTI1814_H


#include "typedef.h"
#include "cFixPti1814.h"


class cFp1814LowPass
{
  public:
    cFixPti1814 mValue;
    cFixPti1814 mFactor;

    cFp1814LowPass(i32 li32Factor)
    {
      mFactor.vSetFrac(li32Factor);
      mValue.vSet(0);
    }

    cFixPti1814 DoProcess(cFixPti1814 lFpNewValue) // __attribute__((optimize("-O0")))
    {
      mValue = mFactor * lFpNewValue + (cFixPti1814(1) - mFactor) * mValue;
      return mValue;
    }
};


class cFp1814Pid
{
  public:
  cFixPti1814 mFpErrOld;

  cFixPti1814 mFpErrIntegral;
  cFixPti1814 mFpDeltaError;

  cFp1814LowPass mLpOutput;

  cFixPti1814 mFpKp;
  cFixPti1814 mFpKi;
  cFixPti1814 mFpKd;

  i16         mi16OutputLimit;

  cFp1814Pid(u32 lu32LpInitValue)
    : mLpOutput(lu32LpInitValue)
  {
  }

  i16 i16DoProcess(cFixPti1814 lFpErr) // __attribute__((optimize("-O0")))
  {
    mFpDeltaError = lFpErr - mFpErrOld;
    mFpErrOld = lFpErr;

    // PID
    cFixPti1814 v = mFpKp * lFpErr +
                    mFpErrIntegral +
                    mFpKd * mFpDeltaError;

    bool saturating = False;

    if (v.i32Get() >= mi16OutputLimit)       {v = cFixPti1814( mi16OutputLimit); saturating = True;}
    else if (v.i32Get() <= -mi16OutputLimit) {v = cFixPti1814(-mi16OutputLimit); saturating = True;}

    // error and output same sign
    bool sign = (lFpErr.i8Sign() * v.i8Sign() > 0);
    // zero
    bool clamp = saturating && sign;
    if(!clamp)
    {
      mFpErrIntegral += (mFpKi * lFpErr);
    }

    // output
    return mLpOutput.DoProcess(v).i32Get();
  }
};


#endif /* _PIDFIXPTI1814_H */
