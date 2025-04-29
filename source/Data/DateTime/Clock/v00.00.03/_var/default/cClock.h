#ifndef _CCLOCK_H
#define _CCLOCK_H

#include "typedef.h"
#include "CStrT.h"

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
    tstDate  stDate;
    tstTime  stTime;
  } tstTimeDate;

  tunTime mTime;
  tunDate mDate;

  u8      mValid;

  static const uint8 nMonthTable[12];

  cClock()
  {
    vClear();
  }

  inline void vClear()
  {
    mTime.ui32Time = 0;
    mDate.ui32Date = 0;
    mValid = 0;
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

    mValid = 1;
  }

  bool IsValid()
  {
    return (mValid == 1);
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
  }

  void vAdd1s()
  {
    mTime.stTime.ui8Second++;
    if (mTime.stTime.ui8Second >= 60)
    {
      mTime.stTime.ui8Second = 0;

      mTime.stTime.ui8Minute++;
      if (mTime.stTime.ui8Minute >= 60)
      {
        mTime.stTime.ui8Minute = 0;
        
        mTime.stTime.ui8Hour++;
        if (mTime.stTime.ui8Hour >= 24)
        {
          mTime.stTime.ui8Hour = 0;
          
          mDate.stDate.ui8Day++;
          if (mDate.stDate.ui8Day >= u8GetDayPerMonth(mDate.stDate.ui8Day, mDate.stDate.ui8Month))
          {
            mDate.stDate.ui8Day = 0;

            mDate.stDate.ui8Month++;
            if (mDate.stDate.ui8Month >= 12)
            {
              mDate.stDate.ui8Month = 0;
              mDate.stDate.ui16Year++;
            }
          }
        }
      }
    }
  }

  void vAdd(cClock &lcClock)
  {
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
  }

  uint32 u32Encode32Bit()
  {
    uint32 lui32i;
    uint32 lui32Counts;

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
    return lpu8Data;
  }


  cStr& toStringTime(cStr& lcStr)
  {
    lcStr.Setf((const char8*)"%02d:%02d:%02d", mTime.stTime.ui8Hour, mTime.stTime.ui8Minute, mTime.stTime.ui8Second);
    return lcStr;
  }

  cStr& toStringDate(cStr& lcStr)
  {
    lcStr.Setf((const char8*)"%04d-%02d-%02d", mDate.stDate.ui16Year, mDate.stDate.ui8Month, mDate.stDate.ui8Day);
    return lcStr;
  }

  cStr& toString(cStr& lcStr)
  {
    // 2020-10-10 10:10:19

    lcStr.Setf((const char8*)"%04d-%02d-%02d %02d:%02d:%02d", mDate.stDate.ui16Year, mDate.stDate.ui8Month,  mDate.stDate.ui8Day,
                                                              mTime.stTime.ui8Hour,  mTime.stTime.ui8Minute, mTime.stTime.ui8Second);
    return lcStr;
  }
};



#endif /* _CCLOCK_H */
