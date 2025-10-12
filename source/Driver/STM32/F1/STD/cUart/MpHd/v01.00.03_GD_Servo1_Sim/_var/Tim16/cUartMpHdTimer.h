#ifndef __cUSART_MPHD_TIM_H__
#define __cUSART_MPHD_TIM_H__

#include "Typedef.h"


class cUsartMpHd_Slave_Timer
{
  public:

  cUsartMpHd_Slave_Timer()
  {
    vInit();
  }

  void vStart(u16 lui16Time_us)
  {
    UNUSED(lui16Time_us);
  }

  void vStop()
  {
  }

  void vInit(void)
  {
  }
};


#endif // __cUSART_MPHD_TIM_H__
