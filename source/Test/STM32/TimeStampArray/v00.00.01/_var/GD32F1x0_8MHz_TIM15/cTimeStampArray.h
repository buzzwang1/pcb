
#ifndef __TIMESTAMPARRAY_H__
#define __TIMESTAMPARRAY_H__


#include "Typedef.h"
#include "gd32f1x0.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_timer.h"

class cTimeStampArray
{
  public:

  typedef enum
  {
    enPoints = 32
  }tenConsts;

  static u16 mTimeStamps[enPoints];

  // typedef struct
  // {
  //   uint32_t CTRL;
  //   uint32_t CYCCNT;
  //   uint32_t CPICNT;
  //   uint32_t EXCCNT;
  //   uint32_t SLEEPCNT;
  //   uint32_t LSUCNT;
  //   uint32_t FOLDCNT;
  //   uint32_t PCSR;
  // } tstDWT;
  // 
  // static tstDWT mDWT;

  cTimeStampArray()
  {
    vInit();
  }

  static void vInit()
  {
    rcu_periph_clock_enable(RCU_TIMER15);

    TIMER_PSC(TIMER15)   = 7;
    TIMER_CAR(TIMER15)   = -1;
    TIMER_CTL0(TIMER15)  =  1;
  };

  static void vReset(void)
  {
    TIMER_CTL0(TIMER15) = 0;
    TIMER_CNT(TIMER15)  = 0;
    TIMER_CTL0(TIMER15) = 1;
  }


  static void vSetTimeStamp(u16 lu16Idx)
  {
    mTimeStamps[lu16Idx] = uGet();
  }


  static inline u16 uGet(void)
  {
    return TIMER_CNT(TIMER15);
  }


  static void vDtwStart()
  {
    DWT->CTRL &= 0xFF000000UL;
    DWT->CYCCNT = 0;
    DWT->CPICNT = 0;
    DWT->EXCCNT = 0;
    DWT->CTRL |= 0x07F0001UL;    
  }


  static void vDtwStop()
  {
    // https://developer.arm.com/documentation/ddi0337/e/ch11s05s01
    // https://developer.arm.com/documentation/ddi0337/e/System-Debug/DWT/Summary-and-description-of-the-DWT-registers?lang=en#BABFDDBA
    DWT->CTRL &= 0xFF000000UL;

    //  Add following line tp debugger
    //  *((DWT_Type*)DWT_BASE)

    // mDWT.CTRL     = DWT->CTRL;     // Control Register                   : 
    // mDWT.CYCCNT   = DWT->CYCCNT;   // Cycle Count Register               : count the number of core cycles. This count can measure elapsed execution time.
    // mDWT.CPICNT   = DWT->CPICNT;   // CPI Count Register                 : count the total number of instruction cycles beyond the first cycle.
    // mDWT.EXCCNT   = DWT->EXCCNT;   // Exception Overhead Count Register  : count the total cycles spent in interrupt processing.
    // mDWT.SLEEPCNT = DWT->SLEEPCNT; // Sleep Count Register               : count the total number of cycles during which the processor is sleeping.
    // mDWT.LSUCNT   = DWT->LSUCNT;   // LSU Count Register                 : 
    // mDWT.FOLDCNT  = DWT->FOLDCNT;  // Folded-instruction Count Register  : 
    // mDWT.PCSR     = DWT->PCSR;     // Program Counter Sample Register    : 
  }

};

#endif // __TIMESTAMPARRAY_H__
