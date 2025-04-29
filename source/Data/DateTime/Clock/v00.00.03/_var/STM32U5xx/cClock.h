#ifndef _CCLOCK_H
#define _CCLOCK_H

#include "typedef.h"
#include "CStrT.h"
#include "ClockInfo.h"
#include "cWufHandler.h"
#include "cBuRam.h"

#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_system.h"
#include "stm32u5xx_ll_rtc.h"
#include "stm32u5xx_ll_pwr.h"


class cClock
{
  public:

  typedef struct
  {
    uint8  ui8Hour;
    uint8  ui8Minute;
    uint8  ui8Second;
    uint8  ui8SubSecond;
  } tstTime;


  typedef union
  {
    uint32  ui32Time;
    tstTime stTime;
  }tunTime;

  typedef struct
  {
    uint16 ui16Year;
    uint8  ui8Month;
    uint8  ui8Day;
  } tstDate;

  typedef union
  {
    uint32  ui32Date;
    tstDate stDate;
  }tunDate;

  typedef struct
  {
    tunDate  unDate;
    tunTime  unTime;
  } tstTimeDate;

  tunTime mTime;
  tunDate mDate;
  tstTimeDate mLastSync;

  u8      mValid;

  u8      mu8ClockSource; // 0 = no; 1 = LSE, 2 = LSI

  static const uint8 nMonthTable[12];

  cClock()
  {
    mu8ClockSource = 0;

    cBuRam::vEnable();

    bool lu8Reset = False;
    if ((cWufHandler::munWakeupSources->stWakeupSources.isWuPinRst) ||
        (cWufHandler::munWakeupSources->stWakeupSources.isWuSftRst)) lu8Reset = True;

    // Clock valid ?
    if ((cBuRam::mBuRam->u32RtcSyncCnt != 0) && (!lu8Reset))
    {
      vReadFromRtc();
      mValid = 1;
    }
    else
    {
      vRtcInit(lu8Reset);

      mTime.ui32Time = 0;
      mDate.ui32Date = 0;
      mValid = 0;
    }


    if ((RCC->BDCR & RCC_BDCR_LSEON) && (LL_RCC_LSE_IsReady()) && (LL_RCC_GetRTCClockSource()== LL_RCC_RTC_CLKSOURCE_LSE))
    {
      mu8ClockSource = 1;
    }
    else
    {
      if ((RCC->BDCR & RCC_BDCR_LSION) && (LL_RCC_LSI_IsReady()) && (LL_RCC_GetRTCClockSource() == LL_RCC_RTC_CLKSOURCE_LSI))
      {
        mu8ClockSource = 2;
      }
    }

    mLastSync.unDate.ui32Date = cBuRam::mBuRam->u32RtcSyncDate;
    mLastSync.unTime.ui32Time = cBuRam::mBuRam->u32RtcSyncTime;
  }

  void vClear()
  {
    mValid = 0;
  }


  void vRtcInit(bool boBuReset)
  {
    u8 lu8t = 10;

    cBuRam::mBuRam->u32RtcSyncCnt  = 0; // set invalid 
    cBuRam::mBuRam->u32RtcSyncDate = 0; // set invalid 
    cBuRam::mBuRam->u32RtcSyncTime = 0; // set invalid 

    // --------------- First try LSE ---------------

    /*##-2- Configure LSE/LSI as RTC clock source ###############################*/
    // RTC_CLOCK_SOURCE_LSE
    if (boBuReset)
    {
      // Löscht BuRam und RTC-Status, z.B. Wakeup
      LL_RCC_ForceBackupDomainReset();
      LL_RCC_ReleaseBackupDomainReset();
    }
    LL_RCC_LSE_Enable();

    while ((LL_RCC_LSE_IsReady() != 1) && (lu8t > 0))
    {
      cClockInfo::Delay_us(10);
      lu8t--;
    }

    if (LL_RCC_LSE_IsReady())
    {
      mu8ClockSource = 1;
      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);

      /*##-3- Enable RTC peripheral Clocks #######################################*/
      /* Enable RTC Clock */
      LL_RCC_EnableRTC();

      LL_RTC_InitTypeDef lstRtc;
      LL_RTC_StructInit(&lstRtc);

      lstRtc.HourFormat = LL_RTC_HOURFORMAT_24HOUR;

      // 32768 = 256*128
      // ck_apre = LSEFreq / (ASYNC prediv + 1) = 256Hz
      // ck_apre = 32768Hz / (127 + 1)          = 256Hz
      lstRtc.AsynchPrescaler = ((uint32_t)0x7F);

      // ck_spre = ck_apre / (SYNC prediv + 1) = 1 Hz
      // ck_spre = 256Hz   / (255 + 1)         = 1 Hz
      lstRtc.SynchPrescaler = ((uint32_t)0x00FF);
      LL_RTC_Init(RTC, &lstRtc);
    }
    else
    {
      mu8ClockSource = 2;
      LL_RCC_LSE_Disable();

      LL_RCC_LSI_Enable();
      while (LL_RCC_LSI_IsReady() != 1)
      {
      }
      LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);

      /*##-3- Enable RTC peripheral Clocks #######################################*/
      /* Enable RTC Clock */
      LL_RCC_EnableRTC();

      LL_RTC_InitTypeDef lstRtc;
      LL_RTC_StructInit(&lstRtc);

      lstRtc.HourFormat = LL_RTC_HOURFORMAT_24HOUR;


      // 32768 = 256*125
      // ck_apre = LSEFreq / (ASYNC prediv + 1) = 256Hz
      // ck_apre = 32768Hz / (124 + 1)          = 256Hz
      lstRtc.AsynchPrescaler = ((uint32_t)0x7C);

      // ck_spre = ck_apre / (SYNC prediv + 1) = 1 Hz
      // ck_spre = 256Hz   / (255 + 1)         = 1 Hz
      lstRtc.SynchPrescaler = ((uint32_t)0x00FF);
      LL_RTC_Init(RTC, &lstRtc);
    }
  }

  void vWriteToRtc()
  {
    LL_RTC_DateTypeDef lstDate;
    LL_RTC_DATE_StructInit(&lstDate);

    //lstDate.WeekDay = 6;
    lstDate.Day   = mDate.stDate.ui8Day;
    lstDate.Month = mDate.stDate.ui8Month;
    if (mDate.stDate.ui16Year > 2000)
    {
      lstDate.Year  = mDate.stDate.ui16Year - 2000;
    }
    else
    {
      lstDate.Year  = mDate.stDate.ui16Year;
    }


    LL_RTC_DATE_Init(RTC, LL_RTC_FORMAT_BIN, &lstDate);

    LL_RTC_TimeTypeDef lstTime;
    LL_RTC_TIME_StructInit(&lstTime);

    lstTime.TimeFormat = LL_RTC_TIME_FORMAT_AM_OR_24;
    lstTime.Hours   = mTime.stTime.ui8Hour;
    lstTime.Minutes = mTime.stTime.ui8Minute;
    lstTime.Seconds = mTime.stTime.ui8Second;
    LL_RTC_TIME_Init(RTC, LL_RTC_FORMAT_BIN, &lstTime);

    if (cBuRam::mBuRam->u32RtcSyncCnt < UINT32_MAX)
    {
      cBuRam::mBuRam->u32RtcSyncCnt++; // set valid
    }

    mLastSync.unDate.ui32Date = mDate.ui32Date;
    mLastSync.unTime.ui32Time = mTime.ui32Time;

    cBuRam::mBuRam->u32RtcSyncDate = mLastSync.unDate.ui32Date;
    cBuRam::mBuRam->u32RtcSyncTime = mLastSync.unTime.ui32Time;

    mValid = 1;
  }

  void vReadFromRtc()
  {
    mTime.stTime.ui8SubSecond = 0;
    mTime.stTime.ui8Second = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetSecond(RTC));
    mTime.stTime.ui8Minute = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetMinute(RTC));
    mTime.stTime.ui8Hour   = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_TIME_GetHour(RTC));

    mDate.stDate.ui8Day   = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetDay(RTC));
    mDate.stDate.ui8Month = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetMonth(RTC));
    mDate.stDate.ui16Year = __LL_RTC_CONVERT_BCD2BIN(LL_RTC_DATE_GetYear(RTC)) + 2000;
  }

  void vSet(u16 lu16Year, u8 lu8Month, u8 lu8Day, u8 lu8Hour, u8 lu8Min, u8 lu8Sec)
  {
    mTime.stTime.ui8SubSecond = 0;
    mTime.stTime.ui8Second    = lu8Sec;
    mTime.stTime.ui8Minute    = lu8Min;
    mTime.stTime.ui8Hour      = lu8Hour;

    mDate.stDate.ui8Day   = lu8Day;
    mDate.stDate.ui8Month = lu8Month;
    mDate.stDate.ui16Year = lu16Year;

    vWriteToRtc();
  }

  bool IsValid()
  {
    return (mValid == 1);
  }


  i32 i32DaysDiff(tstTimeDate lstFrom)
  {
    // https://stackoverflow.com/questions/9987562/determining-the-difference-between-dates
    static i8 increment[12] = { 1, -2, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 };

    i32 daysInc = 0;
    if (((i16)mDate.stDate.ui8Day - (i16)lstFrom.unDate.stDate.ui8Day) < 0)
    {
      i32 month = (i16)mDate.stDate.ui8Month - 2; // -1 from zero, -1 previous month.
      if (month < 0)
      {
        month = 11; // Previous month is December.
      }
      daysInc = increment[month];

      if ((month == 1) && ((mDate.stDate.ui16Year % 4) == 0))
      {
        daysInc++; // Increment days for leap year.
      }
    }

    i32 total1 = lstFrom.unDate.stDate.ui16Year * 360 + lstFrom.unDate.stDate.ui8Month * 30 + lstFrom.unDate.stDate.ui8Day;
    i32 total2 = mDate.stDate.ui16Year * 360   + mDate.stDate.ui8Month * 30   + mDate.stDate.ui8Day;
    i32 diff   = total2 - total1;
    i32 years  = diff / 360;
    i32 months = (diff - years * 360) / 30;
    i32 days   = diff - years * 360 - months * 30 + daysInc;

    // Extra calculation when we can pass one month instead of 30 days.
    if ((lstFrom.unDate.stDate.ui8Day == 1) && (mDate.stDate.ui8Day == 31)) 
    {
      months--;
      days = 30;
    }
    return days;
  }

  bool bResync(i32 li32Days)
  {
    i32 li32DiffDays;

    li32DiffDays = i32DaysDiff(mLastSync);

    if (li32DiffDays < 0) li32DiffDays = -li32DiffDays;
    if (li32DiffDays >= li32Days) return 1;
    return 0;
  }

  void vDecode32Bit(uint32 lui32TimeDate)
  {
    uint32 lui32Counts;
    uint32 lui32Counts1;

    mDate.stDate.ui16Year  = 1970 - 1;
    lui32Counts     = 0;
    lui32Counts1    = lui32Counts;

    // Remaining Year, Month, Day, Hour, Minute, Second
    while(lui32Counts < lui32TimeDate)
    {
      lui32Counts1 = lui32Counts;
      if(IsLeapYear(mDate.stDate.ui16Year))
      {
        lui32Counts += 31622400;
      }
      else
      {
        lui32Counts += 31536000;
      }
      mDate.stDate.ui16Year++;
    }

    lui32TimeDate -= lui32Counts1;
    lui32Counts    = 0;

    // Remaining Month, Day, Hour, Minute, Second
    mDate.stDate.ui8Month = 0;
    while(lui32Counts < lui32TimeDate)
    {
      lui32Counts1 = lui32Counts;
      lui32Counts += (((uint32)nMonthTable[mDate.stDate.ui8Month]) * 86400);

      if (mDate.stDate.ui8Month == 1) //February ?
      {
        if(IsLeapYear(mDate.stDate.ui16Year) == 1)
        {
          lui32Counts += 86400;
        }
      }

      mDate.stDate.ui8Month++;
    }

    lui32TimeDate -= lui32Counts1;

    // Remaining Day, Hour, Minute, Second
    mDate.stDate.ui8Day     = ((lui32TimeDate / 86400) + 1);
    lui32TimeDate -= (mDate.stDate.ui8Day - 1) * 86400;

    // Remaining Hour, Minute, Second
    mTime.stTime.ui8Hour    =  (lui32TimeDate / 3600);
    lui32TimeDate -= mTime.stTime.ui8Hour * 3600;

    // Remaining Minute, Second
    mTime.stTime.ui8Minute  =  (lui32TimeDate / 60);
    mTime.stTime.ui8Second  =  (lui32TimeDate % 60);

    vWriteToRtc();
  }

  void vAdd1s()
  {
    if (mValid) vReadFromRtc();
  }

  void vAdd(cClock &lcClock)
  {
    vReadFromRtc();
    mTime.stTime.ui8Second += lcClock.mTime.stTime.ui8Second;
    if (mTime.stTime.ui8Second >= 60)
    {
      mTime.stTime.ui8Second %= 60;
      mTime.stTime.ui8Minute++;
    }

    mTime.stTime.ui8Minute += lcClock.mTime.stTime.ui8Minute;
    if (mTime.stTime.ui8Minute >= 60)
    {
      mTime.stTime.ui8Minute %= 60;
      mTime.stTime.ui8Hour++;
    }

    mTime.stTime.ui8Hour += lcClock.mTime.stTime.ui8Hour;
    if (mTime.stTime.ui8Hour >= 24)
    {
      mTime.stTime.ui8Hour %= 24;
      mDate.stDate.ui8Day++;
    }

    mDate.stDate.ui8Day += lcClock.mDate.stDate.ui8Day;
    u8 lu8DayPerMonth = u8GetDayPerMonth(mDate.stDate.ui8Day, mDate.stDate.ui8Month);
    if (mDate.stDate.ui8Day >= lu8DayPerMonth)
    {
      mDate.stDate.ui8Day %= lu8DayPerMonth;
      mDate.stDate.ui8Month++;
    }

    mDate.stDate.ui8Month += lcClock.mDate.stDate.ui8Month;
    if (mDate.stDate.ui8Month >= 12)
    {
      mDate.stDate.ui8Month %= 12;
      mDate.stDate.ui16Year++;
    }

    mDate.stDate.ui16Year += lcClock.mDate.stDate.ui16Year;

    vWriteToRtc();
  }

  uint32 u32Encode32Bit()
  {
    uint32 lui32i;
    uint32 lui32Counts;

    vReadFromRtc();

    if(mDate.stDate.ui16Year > 2099)
    {
      mDate.stDate.ui16Year = 2099;
    }

    if(mDate.stDate.ui16Year < 1970)
    {
      mDate.stDate.ui16Year = 1970;
    }

    lui32Counts = 0;

    for(lui32i = 1970; lui32i < mDate.stDate.ui16Year; lui32i++)
    {
      if(IsLeapYear(lui32i))
      {
        lui32Counts += 31622400;
      }
      else
      {
        lui32Counts += 31536000;
      }
    }

    for(lui32i = 0; lui32i < (u32)(mDate.stDate.ui8Month - 1); lui32i++)
    {
      lui32Counts += (((uint32)nMonthTable[lui32i]) * 86400);
    }

    if (mDate.stDate.ui8Month > 2)
    {
      if(IsLeapYear(mDate.stDate.ui16Year) == 1)
      {
        lui32Counts += 86400;
      }
    }

    lui32Counts += ((uint32)(mDate.stDate.ui8Day - 1) * 86400);
    lui32Counts += ((uint32)mTime.stTime.ui8Hour      *  3600);
    lui32Counts += ((uint32)mTime.stTime.ui8Minute    *    60);
    lui32Counts += mTime.stTime.ui8Second;

    return lui32Counts;
  }


  u8 u8GetDayPerMonth(u8 lu8Month, u16 lu16Year)
  {
    //Februar ?
    if (lu8Month == 2)
    {
      if (IsLeapYear(lu16Year))
      {
        return 29;
      }
    }
    return nMonthTable[lu8Month - 1];
  }


  bool IsLeapYear(uint16 lui16Year)
  {
    if(lui16Year % 4 == 0)
    {
      if(lui16Year % 100 == 0)
      {
        if(lui16Year % 400 == 0)
        {
          return True;
        }
        else
        {
          return False;
        }
      }
      else
      {
        return True;
      }
    }
    return False;
  }

  u8* vSerialize(u8* lpu8Data)
  {
    vReadFromRtc();
    *lpu8Data++ = mDate.stDate.ui16Year >> 8;
    *lpu8Data++ = mDate.stDate.ui16Year & 0xFF;
    *lpu8Data++ = mDate.stDate.ui8Month;
    *lpu8Data++ = mDate.stDate.ui8Day;
    *lpu8Data++ = mTime.stTime.ui8Hour;
    *lpu8Data++ = mTime.stTime.ui8Minute;
    *lpu8Data++ = mTime.stTime.ui8Second;
    return lpu8Data;
  }

  u8* vDeserialize(u8* lpu8Data)
  {
    mDate.stDate.ui16Year     = *lpu8Data++;
    mDate.stDate.ui16Year     = (mDate.stDate.ui16Year << 8) + *lpu8Data++;
    mDate.stDate.ui8Month     = *lpu8Data++;
    mDate.stDate.ui8Day       = *lpu8Data++;
    mTime.stTime.ui8Hour      = *lpu8Data++;
    mTime.stTime.ui8Minute    = *lpu8Data++;
    mTime.stTime.ui8Second    = *lpu8Data++;
    mTime.stTime.ui8SubSecond = 0;
    mValid = 1;

    vWriteToRtc();

    return lpu8Data;
  }


  cStr& toStringTime(cStr& lcStr)
  {
    vReadFromRtc();
    lcStr.Setf((const char8*)"%02d:%02d:%02d", mTime.stTime.ui8Hour, mTime.stTime.ui8Minute, mTime.stTime.ui8Second);
    return lcStr;
  }

  cStr& toStringDate(cStr& lcStr)
  {
    vReadFromRtc();
    lcStr.Setf((const char8*)"%04d-%02d-%02d", mDate.stDate.ui16Year, mDate.stDate.ui8Month, mDate.stDate.ui8Day);
    return lcStr;
  }

  cStr& toString(cStr& lcStr)
  {
    // 123456789012345678901
    // 2020-10-10 10:10:19 -/I/E
    vReadFromRtc();
    lcStr.Setf((const char8*)"%04d-%02d-%02d %02d:%02d:%02d ", mDate.stDate.ui16Year, mDate.stDate.ui8Month,  mDate.stDate.ui8Day,
                                                               mTime.stTime.ui8Hour,  mTime.stTime.ui8Minute, mTime.stTime.ui8Second);

    switch (mu8ClockSource)
    {
      case 1:  lcStr += 'E'; break;
      case 2:  lcStr += 'I'; break;
      default: lcStr += '-'; break;
    }

    return lcStr;
  }
};



#endif /* _CCLOCK_H */
