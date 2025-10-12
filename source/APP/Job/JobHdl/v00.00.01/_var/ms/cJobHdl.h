#ifndef __JOB_HANDLER_H__
#define __JOB_HANDLER_H__

#include "Typedef.h"


class cJobHandler
{
  public:
  enum class cJobs: u8
  {
    nJobNop,
    nLast
  };

  enum class cJobStates : u8
  {
    stJobNop,
    stJobProcessStart,
    stJobProcess2,
    stJobProcess3,
    stJobProcess4,
    stJobProcess5,
    stJobProcess6,
    stJobProcess7,
    stJobProcess8,
    stJobProcess9,
    stJobProcess10,
    stJobProcess11
  };

  cJobs      mcJob;
  cJobStates mcState;
  u16        mu16ExeTime_ms;

  cJobHandler()
  {
    mcJob   = cJobs::nJobNop;
    mcState = cJobStates::stJobNop;
  }

  void vStart(cJobs lcJob, bool lbForceStart = false)
  {
    if (isReady() || (lbForceStart))
    {
      mcJob   = lcJob;
      mcState = cJobStates::stJobProcessStart;
      mu16ExeTime_ms = 0;
    }
  }

  void vFinished()
  {
    mcJob   = cJobs::nJobNop;
    mcState = cJobStates::stJobNop;
    mu16ExeTime_ms = 0;
  }

  virtual void vProcess(u16 lu16TimeDiff_ms)
  {
    if ((U16MAX - mu16ExeTime_ms) > lu16TimeDiff_ms)
    {
      mu16ExeTime_ms += lu16TimeDiff_ms;
    }
    else
    {
      mu16ExeTime_ms = U16MAX;
    }
  }

  u16 u16GetTime_ms() {return mu16ExeTime_ms;}

  bool isReady()
  {
    return (mcJob == cJobs::nJobNop);
  }

  bool isBusy()
  {
    return (!isReady());
  }
};


#endif // __JOB_HANDLER_H__
