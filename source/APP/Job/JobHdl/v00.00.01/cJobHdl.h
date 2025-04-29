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
  u32        mu32ExeTime_us;

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
      mu32ExeTime_us = 0;
    }
  }

  void vFinished()
  {
    mcJob   = cJobs::nJobNop;
    mcState = cJobStates::stJobNop;
    mu32ExeTime_us = 0;
  }

  virtual void vProcess(u32 lu32TimeDiff_us)
  {
    if ((U32MAX - mu32ExeTime_us) > lu32TimeDiff_us)
    {
      mu32ExeTime_us += lu32TimeDiff_us;
    }
    else
    {
      mu32ExeTime_us = U32MAX;
    }
  }

  u32 u32GetTime_us() {return mu32ExeTime_us;}

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
