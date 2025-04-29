#ifndef __H__
#define __H__

#include "Typedef.h"
#include "tGPPin.h"
#include "cClock.h"



class cDcf77
{
  public:

  enum
  {
    nSmIdle = 0,
    nSmNotSynched,
    nSmSynched
  }tenSmStates;


  typedef enum
  {
    nEvReset = 0,
    nEvTick
  }tenEvent;


  typedef enum MAIN_enConsts
  {
    //Werte für Data
    nData0 =  0,
    nData1,
    nDataSynch0,
    nDataSynch1,
    nDataError,
  
    // Werte für die Signalerkennung
    //e.g.
    // 100ms = 10*10ms => '0'
    // 200ms = 20*10ms => '1'  
    nData0_Min =  7,
    nData0_Max = 15,
  
    nData1_Min = 17,
    nData1_Max = 25,  
  
    nDataSynch_Min   = 140,
    nDataInvalid_Max = 50,
  
    //Error Codes
    nDataError_Bit0   = 1,
    nDataError_Bit20,
    nDataError_ParMin,
    nDataError_ParHour,
    nDataError_ParDate
  }tenConsts;

  typedef struct
  {
    cClock   mcClock;
    uint8    ui8MEZ;
  }tstClock;



  typedef struct
  {
    tstClock mstDCF77Clock; 

    uint8 ui8SmDCF77;        // Statemaschine siehe DCF77_tenSmStates

    uint8 ui8DCF77_Data;     // Wert von DCF77_tenConsts
    uint8 ui8DCF77_DataPar;  // Paritäts-Check
    uint8 ui8DCF77_DataValid;// Paritäts-Check usw.
    uint8 ui8DCF77_DataToRead;// Paritäts-Check usw.

    uint8 ui8DCF77_DataCnt;  // 0..59 für jede Sekunde

    uint8 ui8DCF77_Pin;      // Zustand DCF77 Eingangs-PIN
    uint8 ui8DCF77_Pin_Last; // Letzer zustand DCF77 Eingangs-PIN um Flanken zu erkennen
  
    uint8 ui8DCF77_Filter;   // Filter um Störungen zu Unterdrücken

    uint8 ui8DCF77_Pin_OnTicks;  // Anzeit  in cDcf77::nEvTick des DCF77 Eingangs-PIN
    uint8 ui8DCF77_Pin_OffTicks; // Auszeit in cDcf77::nEvTick des DCF77 Eingangs-PIN 
  }DCF77_tstRuntime;

  cGpPin          *mPin;
  cClock           mcClock;
  DCF77_tstRuntime mstRtData;

  cDcf77(cGpPin *lcScl)
  {
    mPin = lcScl;
    vInit();
  }


  void  vInit()
  {
    vSm(cDcf77::nEvReset);
  }

  uint8 ui8Decode(uint8 luiBitIndex, uint8 luiData)
  {
    uint8 lui8Error;
  
    lui8Error = 0;
  
    mstRtData.ui8DCF77_DataPar += luiData;
  
    switch (luiBitIndex)
    {
      case 0: // Start einer neuen Minute (ist immer '0')
        mstRtData.ui8DCF77_DataValid  = 1;
        mstRtData.ui8DCF77_DataToRead = 0;
        mstRtData.ui8DCF77_DataPar    = 0;
        if (luiData != 0) 
        {
          mstRtData.ui8DCF77_DataValid = 0;
          lui8Error = cDcf77::nDataError_Bit0;
        }
      
      break;
    
      case 1: // WetterInfo
      case 2: // WetterInfo
      case 3: // WetterInfo
      case 4: // WetterInfo
      case 5: // WetterInfo
      case 6: // WetterInfo
      case 7: // WetterInfo
      case 8: // WetterInfo
      case 9: // WetterInfo
      case 10: // WetterInfo
      case 11: // WetterInfo
      case 12: // WetterInfo
      case 13: // WetterInfo
      case 14: // WetterInfo
      break;
    
      case 15: // Rufbit
      break;
      case 16: // Zeitumstellung
      break;
      case 17: // Sommer/Winterzeit: '0': MEZ, '1': MESZ
        mstRtData.mstDCF77Clock.ui8MEZ = luiData;
      break;
      case 18: // Sommer/Winterzeit: '0': MESZ, '1': MEZ
      break;
      case 19: // Schaltsekunde
      break;
    
      case 20: // Beginn der Zeitinformation (ist immer '1')
        mstRtData.ui8DCF77_DataPar   = 0;
      
        if (luiData != 1)
        {
          mstRtData.ui8DCF77_DataValid = 0;
          lui8Error = cDcf77::nDataError_Bit20;
        }
   
    
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Second    = 0;
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8SubSecond = 0;
      break;
    
      case 21: // Minute Einer Bit 1
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute  = 0;
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute |= (luiData << 0);
      break;
      case 22: // Minute Einer Bit 2
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute |= (luiData << 1);
      break;
      case 23: // Minute Einer Bit 4
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute |= (luiData << 2);
      break;
      case 24: // Minute Einer Bit 8
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute |= (luiData << 3);
      break;
      case 25: // Minute Zehner Bit 10
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute += (luiData << 0) * 10;
      break;
      case 26: // Minute Zehner Bit 20
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute += (luiData << 1) * 10;
      break;
      case 27: // Minute Zehner Bit 40
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Minute += (luiData << 2) * 10;
      break;
      case 28: // Parität Minute
        // Wenn ungerade dann Fehler
        if (mstRtData.ui8DCF77_DataPar & 1)
        {
          mstRtData.ui8DCF77_DataValid = 0;
          lui8Error = cDcf77::nDataError_ParMin;
        }      
        mstRtData.ui8DCF77_DataPar   = 0;
      break;
    
      case 29: // Stunde Einer Bit 1
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Hour  = 0;
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Hour |= (luiData << 0);
      break;
      case 30: // Stunde Einer Bit 2
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Hour |= (luiData << 1);
      break;
      case 31: // Stunde Einer Bit 4
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Hour |= (luiData << 2);
      break;
      case 32: // Stunde Einer Bit 8
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Hour |= (luiData << 3);
      break;
      case 33: // Stunde Zehner Bit 10
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Hour += (luiData << 0) * 10;
      break;
      case 34: // Stunde Zehner Bit 20
        mstRtData.mstDCF77Clock.mcClock.mTime.stTime.ui8Hour += (luiData << 1) * 10;
      break;
      case 35: // Parität Stunde
        // Wenn ungerade dann Fehler
        if (mstRtData.ui8DCF77_DataPar & 1)
        {
          mstRtData.ui8DCF77_DataValid = 0;
          lui8Error = cDcf77::nDataError_ParHour;
        }
        mstRtData.ui8DCF77_DataPar   = 0;    
      break;

      case 36: // Kalendertag Einer Bit 1
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Day  = 0;
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Day |= (luiData << 0);
      break;
      case 37: // Kalendertag Einer Bit 2
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Day |= (luiData << 1);
      break;
      case 38: // Kalendertag Einer Bit 4
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Day |= (luiData << 2);
      break;
      case 39: // Kalendertag Einer Bit 8
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Day |= (luiData << 3);
      break;
      case 40: // Kalendertag Zehner Bit 10
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Day += (luiData << 0) * 10;
      break;
      case 41: // Kalendertag Zehner Bit 20
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Day += (luiData << 1) * 10;
      break;
      case 42: // Wochentag Einer Bit 1
      break;
      case 43: // Wochentag Einer Bit 2
      break;
      case 44: // Wochentag Einer Bit 4
      break;
      case 45: // Monatsnummer Einer Bit 1
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Month  = 0;
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Month |= (luiData << 0);
      break;
      case 46: // Monatsnummer Einer Bit 2
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Month |= (luiData << 1);
      break;
      case 47: // Monatsnummer Einer Bit 4
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Month |= (luiData << 2);
      break;
      case 48: // Monatsnummer Einer Bit 8
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Month |= (luiData << 3);
      break;
      case 49: // Monatsnummer Zehner Bit 10
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui8Month += (luiData << 0) * 10;
      break;
    
      case 50: // Jahr Einer Bit 1
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year  = 0;
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year |= (luiData << 0);
      break;
      case 51: // Jahr Einer Bit 2
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year |= (luiData << 1);
      break;
      case 52: // Jahr Einer Bit 4
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year |= (luiData << 2);
      break;
      case 53: // Jahr Einer Bit 8
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year |= (luiData << 3);
      break;
      case 54: // Jahr Zehner Bit 10
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year += (luiData << 0) * 10;
      break;
      case 55: // Jahr Zehner Bit 20
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year += (luiData << 1) * 10;
      break;
      case 56: // Jahr Zehner Bit 40
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year += (luiData << 2) * 10;
      break;
      case 57: // Jahr Zehner Bit 80
        mstRtData.mstDCF77Clock.mcClock.mDate.stDate.ui16Year += (luiData << 3) * 10;
      break;
      case 58: // Parität Datum
        // Wenn ungerade dann Fehler
        if (mstRtData.ui8DCF77_DataPar & 1)
        {
          mstRtData.ui8DCF77_DataValid = 0;
          lui8Error = cDcf77::nDataError_ParDate;
        }
        else
        {
          mstRtData.ui8DCF77_DataToRead = 1;
          mcClock = mstRtData.mstDCF77Clock.mcClock;
        }
      break;
      case 59: // Parität keine Sekundenmarke
      break;

      default:
      break;
    }
  
    return lui8Error;
  }

  bool IsDataReady()
  {
    return ((mstRtData.ui8DCF77_DataToRead) && (mstRtData.ui8DCF77_DataValid));
  }


  // Erkennung des DCF77 Signals via polling
  // Muss alle 10ms aufgerufen werden
  void vSm(cDcf77::tenEvent lenEvent)
  {  
    if (lenEvent == cDcf77::nEvReset)
    {
      mstRtData.ui8SmDCF77        = cDcf77::nSmNotSynched;
    
      mstRtData.ui8DCF77_Pin      = mPin->ui8Get();
      mstRtData.ui8DCF77_Pin_Last = mstRtData.ui8DCF77_Pin;
    
      mstRtData.ui8DCF77_DataCnt = 0;
    
      mstRtData.ui8DCF77_Pin_OnTicks  = 0;
      mstRtData.ui8DCF77_Pin_OffTicks = 0;
    
      mcClock.vClear();
    }
    else
    {
      //alle 10ms einlesen
      mstRtData.ui8DCF77_Pin_Last = mstRtData.ui8DCF77_Pin;

      // Einlesen mit einfachen Entprellung um Spikes zu unterdrücken
      mstRtData.ui8DCF77_Filter   = (mstRtData.ui8DCF77_Filter << 1) + mPin->ui8Get();

      if ((mstRtData.ui8DCF77_Filter & 3) == 3)
      {
        mstRtData.ui8DCF77_Pin = 1;
      }
      else if ((mstRtData.ui8DCF77_Filter & 3) == 0)
      {
        mstRtData.ui8DCF77_Pin = 0;
      }
    }
  

    if (mstRtData.ui8DCF77_Pin)
    {
      if (mstRtData.ui8DCF77_Pin_OnTicks < 255) mstRtData.ui8DCF77_Pin_OnTicks++;
    }
    else
    {
      if (mstRtData.ui8DCF77_Pin_OffTicks < 255) mstRtData.ui8DCF77_Pin_OffTicks++;
    }
  
    if ((!mstRtData.ui8DCF77_Pin_Last) && (mstRtData.ui8DCF77_Pin)) // 0->1 Übergang
    {
      if ((mstRtData.ui8DCF77_Pin_OnTicks >= cDcf77::nData0_Min) && (mstRtData.ui8DCF77_Pin_OnTicks <= cDcf77::nData0_Max))
      {
        mstRtData.ui8DCF77_Data = cDcf77::nData0; // '0'
      }
      else
      if ((mstRtData.ui8DCF77_Pin_OnTicks >= cDcf77::nData1_Min) && (mstRtData.ui8DCF77_Pin_OnTicks <= cDcf77::nData1_Max))
      {
        mstRtData.ui8DCF77_Data = cDcf77::nData1; //  '1'
      }
      else
      {
        mstRtData.ui8DCF77_Data = cDcf77::nDataError; //Fehler 
      }
    
      if (mstRtData.ui8DCF77_Pin_OffTicks >= cDcf77::nDataSynch_Min)
      {
        if (mstRtData.ui8DCF77_Data == cDcf77::nData0) mstRtData.ui8DCF77_Data = cDcf77::nDataSynch0;  // Synch-Bit
        if (mstRtData.ui8DCF77_Data == cDcf77::nData1) mstRtData.ui8DCF77_Data = cDcf77::nDataSynch1;  // Synch-Bit
      }
      else
      if (mstRtData.ui8DCF77_Pin_OffTicks <= cDcf77::nDataInvalid_Max)
      {
        mstRtData.ui8DCF77_Data = cDcf77::nDataError; //Fehler 
      }
    
      mstRtData.ui8DCF77_Pin_OnTicks  = 0;
      mstRtData.ui8DCF77_Pin_OffTicks = 0;


      switch (mstRtData.ui8SmDCF77)
      {
        case cDcf77::nSmNotSynched:
          if ((mstRtData.ui8DCF77_Data == cDcf77::nDataSynch0) || (mstRtData.ui8DCF77_Data == cDcf77::nDataSynch1))
          {
            mstRtData.ui8SmDCF77 = cDcf77::nSmSynched;
            mstRtData.ui8DCF77_DataCnt = 0;
          }
        break;
    
        case cDcf77::nSmSynched:
          if (mstRtData.ui8DCF77_Data == cDcf77::nDataError)
          {
            mstRtData.ui8SmDCF77 = cDcf77::nSmNotSynched;
          }
          else
          {
         
            switch (mstRtData.ui8DCF77_Data)
            {
              case cDcf77::nData0:
                ui8Decode(mstRtData.ui8DCF77_DataCnt, 0);
                mstRtData.ui8DCF77_DataCnt++;
                if (mstRtData.ui8DCF77_DataCnt > 60) mstRtData.ui8SmDCF77 = cDcf77::nSmNotSynched;
              break;
          
              case cDcf77::nData1:
                ui8Decode(mstRtData.ui8DCF77_DataCnt, 1);
                mstRtData.ui8DCF77_DataCnt++;
                if (mstRtData.ui8DCF77_DataCnt > 60) mstRtData.ui8SmDCF77 = cDcf77::nSmNotSynched;
              break;
          
              case cDcf77::nDataSynch0:
                if (mstRtData.ui8DCF77_DataCnt == 58)
                {
                  ui8Decode(mstRtData.ui8DCF77_DataCnt, 0);
                }
                mstRtData.ui8DCF77_DataCnt = 0;
              break;

              case cDcf77::nDataSynch1:
                if (mstRtData.ui8DCF77_DataCnt == 58)
                {
                  ui8Decode(mstRtData.ui8DCF77_DataCnt, 1);
                }
                mstRtData.ui8DCF77_DataCnt = 0;
              break;


              default:
              break;
            }
          }
        break;
    
        default:
        break;
      }
    }  
  }
};

#endif // __H__
