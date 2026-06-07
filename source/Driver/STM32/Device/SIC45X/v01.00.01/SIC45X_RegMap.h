#ifndef __SIC45X_REGMAP_H__
#define __SIC45X_REGMAP_H__

/**
 * Created by Gemini 3 Flash, 2026-04-02
 * Resource: TI SIC45X Datasheet (SLUSEN4)
 */

#include "TypeDef.h"
#include "cI2C.h"
#include <math.h>


class cBitField32
{
  public:
  u32 mu32Data;
  cBitField32() : mu32Data(0) {}

  // Setzt ein spezifisches Bit (0-31)
  void vSet(u8 lu8Bit) {mu32Data |= (1UL << lu8Bit);}

  // Löscht ein spezifisches Bit (0-31)
  void vClear(u8 lu8Bit) {mu32Data &= ~(1UL << lu8Bit);}

  // Fragt ab, ob ein spezifisches Bit gesetzt ist
  bool isSet(u8 lu8Bit) const {return (mu32Data & (1UL << lu8Bit)) != 0;}

  // Löscht alle Bits (setzt auf 0)
  void vClearAll() {mu32Data = 0;}

  // Gibt den Index des ersten gesetzten Bits zurück (LSB zuerst)
  // Gibt -1 zurück, wenn kein Bit gesetzt ist.
  i8 i8GetFirst() const
  {
    // Nutzt Compiler-Builtin für Effizienz (GCC/Clang)
    // __builtin_ctz zählt die Nullen von rechts (Trailing Zeros)
    return static_cast<i8>(__builtin_ctz(mu32Data));
  }

  // Operator für den Schreibzugriff auf den Rohwert (Zuweisung)
  cBitField32& operator=(u32 u32Value)
  {
    mu32Data = u32Value;
    return *this;
  }

  // Operator für den Lesezugriff auf den Rohwert (Implizite Konvertierung)
  operator u32() const { return mu32Data; }

  // Operator für den Lesezugriff auf den Rohwert (Implizite Konvertierung)
  operator bool() const { return (mu32Data != 0); }
};

class cComponent
{
  public:
    typedef enum
    {
      nSEQ00Reset = 0,
      nSEQ01Init1,  // - Read All
      nSEQ02Init2,  // - Set Init values
      nSEQ04Enable,
      nSEQ05Disable,
      nSEQ06DeInit,
    }tenDefaultSequences;

    cBitField32 mReq;
    cBitField32 mProcess;
    cBitField32 mDone;
    cBitField32 mError;

    void vClearAllRequest() { mReq.vClearAll(); }
    void vClearAllErrors() { mError.vClearAll(); }

    i8 i8GetRequest() { return mReq.i8GetFirst(); }

    void vRequestClear(u8 lu8Bit) { mReq.vClear(lu8Bit); }
    void vRequestSet(u8 lu8Bit) { mReq.vSet(lu8Bit); }
    bool isRequest() { return (mReq != 0); };

    void vProcessClear(u8 lu8Bit) { mProcess.vClear(lu8Bit); }
    void vProcessSet(u8 lu8Bit) { mProcess.vSet(lu8Bit); }

    void vDoneClear(u8 lu8Bit) { mDone.vClear(lu8Bit); }
    void vDoneSet(u8 lu8Bit) { mDone.vSet(lu8Bit); }
    bool isDone(u8 lu8Bit) { return mDone.isSet(lu8Bit); }

    void vErrorClear(u8 lu8Bit) { mError.vClear(lu8Bit); }
    void vErrorSet(u8 lu8Bit) { mError.vSet(lu8Bit); }
    bool isError() { return mError; };
    bool isError(u8 lu8Bit) { return mError.isSet(lu8Bit); }
};


class cSequence
{
  public:
  class cEntry
  {
    public:
    typedef union
    {
      u8 u8Value;
      struct
      {
        u8 Write : 1; // 0 =  read: 1 = write
        u8 Idx : 7;
      } Bits;
    } tunEntry;

    tunEntry munEntry;

    cEntry(const u8 lu8Idx, const bool bWrite = False)
    {
      munEntry.Bits.Write = bWrite;
      munEntry.Bits.Idx   = lu8Idx;
    }

    cEntry(const cEntry &lcCopy)
    {
      munEntry.u8Value = lcCopy.munEntry.u8Value;
    }

    u8 u8GetReg()
    {
      return munEntry.Bits.Idx;
    }

    bool isReg(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx < lu8RegCnt);
    }

    bool isRegAll(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx == lu8RegCnt);
    }

    bool isRead()
    {
      return (!munEntry.Bits.Write);
    }

    bool isWrite()
    {
      return (munEntry.Bits.Write);
    }


    bool isDelay(const u8 lu8RegCnt)
    {
      return (munEntry.Bits.Idx == lu8RegCnt + 1);
    }

    cEntry& operator=(const cEntry &lcCopy)
    {
      munEntry.u8Value = lcCopy.munEntry.u8Value;
      return *this;
    }
  };

  virtual cEntry* pacGet() { return null; }
  virtual u8      u8GetLen() { return 0; }
};


// --- Hauptklasse cSIC45X ---

struct cSIC45X_RegisterMap: public cComponent
{

  class cLinear
  {
    public:
      typedef union
      {
        u16 u16Value;
        struct
        {
          u8 HB;
          u8 LB;
        } Bytes;
      } tunValue;

      tunValue munValue;


      cLinear()
      {
        munValue.u16Value = 0;
      }

      void vDecode(u8* lpu8Bytes)
      {
        munValue.Bytes.LB = lpu8Bytes[0];
        munValue.Bytes.HB = lpu8Bytes[1];
      }

      void pu8Encode(u8* lpu8Bytes)
      {
        lpu8Bytes[0] = munValue.Bytes.LB;
        lpu8Bytes[1] = munValue.Bytes.HB;
      }
  };


  // ==============================================================================
  // LINEAR11: 5-Bit Exponent (Signed), 11-Bit Mantissa (Signed)
  // ==============================================================================
  class cLinear11 : public cLinear
  {
    public:
    cLinear11() : cLinear() {}

    cLinear11(const float fVal)
    {
      *this = fVal;
    }

    operator float() const
    {
      u16 u16Val = munValue.u16Value;

      i8 i8Exp   = (i8)((u16Val >> 11) & 0x1F);
      if (i8Exp > 0x0F) i8Exp |= 0xE0; // Sign Extension

      i16 i16Mant = (i16)(u16Val & 0x07FF);
      if (i16Mant > 0x03FF) i16Mant |= 0xF800; // Sign Extension

      return (float)i16Mant * powf(2.0f, i8Exp);
    }

    // Encode: instance = 12.5f;
    void operator=(float fVal)
    {
      i8 i8Exp = -16;
      i16 i16Mant;
      while (fVal * powf(2.0f, -i8Exp) > 1023.0f && i8Exp < 15) i8Exp++;
      while (fVal * powf(2.0f, -i8Exp) < -1024.0f && i8Exp < 15) i8Exp++;
      i16Mant = (i16)(fVal * powf(2.0f, -i8Exp));
      munValue.u16Value = (u16)(((i8Exp & 0x1F) << 11) | (i16Mant & 0x07FF));
    }
  };


  // ==============================================================================
  // ULINEAR11: 5-Bit Exponent (Signed), 11-Bit Mantissa (Unsigned)
  // ==============================================================================
  class cULinear11 : public cLinear
  {
  public:
    cULinear11() : cLinear() {}

    cULinear11(const float fVal)
    {
      *this = fVal;
    }

    operator float() const
    {
      u16 u16Val = munValue.u16Value;

      i8 i8Exp = (i8)((u16Val >> 11) & 0x1F);
      if (i8Exp > 0x0F) i8Exp |= 0xE0;

      u16 u16Mant = (u16)(u16Val & 0x07FF);

      return (float)u16Mant * powf(2.0f, i8Exp);
    }

    void operator=(float fVal)
    {
      i8 i8Exp = -16;
      if (fVal < 0.0f) fVal = 0.0f;
      while (fVal * powf(2.0f, -i8Exp) > 2047.0f && i8Exp < 15) i8Exp++;
      u16 u16Mant = (u16)(fVal * powf(2.0f, -i8Exp));
      munValue.u16Value = (u16)(((i8Exp & 0x1F) << 11) | (u16Mant & 0x07FF));
    }
  };

  // ==============================================================================
  // ULINEAR16: 16-Bit Mantissa (Unsigned), Fester Exponent
  // ==============================================================================
  class cSLinear16 : public cLinear
  {
  public:
    const float mfFactor;
    cSLinear16(const float lfFactor)
      : cLinear(),
        mfFactor(lfFactor)
    {
    }

    cSLinear16(const float lfFactor, const float fVal)
      : mfFactor(lfFactor)
    {
      *this = fVal;
    }

    operator float() const
    {
      i16 i16Val = (i16)munValue.u16Value;
      return (float)i16Val * mfFactor;
    }

    void operator=(float fVal)
    {
      munValue.u16Value = (fVal / mfFactor);
    }
  };

  // ==============================================================================
  // ULINEAR16: 16-Bit Mantissa (Unsigned), Fester Exponent
  // ==============================================================================
  class cUlinear16 : public cLinear
  {
  public:
    const float mfFactor;
    cUlinear16(const float lfFactor)
      : cLinear(),
        mfFactor(lfFactor)
    {
    }

    cUlinear16(const float lfFactor, const float fVal)
      : mfFactor(lfFactor)
    {
      *this = fVal;
    }


    operator float() const
    {
      u16 u16Val = munValue.u16Value;
      return (float)u16Val * mfFactor;
    }

    void operator=(float fVal)
    {
      munValue.u16Value = (fVal / mfFactor);
    }
  };

  typedef enum
  {
    nREG01_OPERATION              = 0,
    nREG02_ON_OFF_CONFIG          , // = 1,
    nREG03_CLEAR_FAULTS           , // = 2,
    nREG10_WRITE_PROTECT          , // = 3,
    nREG15_STORE_USER_ALL         , // = 4,
    nREG16_RESTORE_USER_ALL       , // = 5,
    nREG19_CAPABILITY             , // = 6,
    nREG20_VOUT_MODE              , // = 7,
    nREG21_VOUT_COMMAND           , // = 8,
    nREG22_VOUT_TRIM              , // = 9,
    nREG24_VOUT_MAX               , // = 10,
    nREG25_VOUT_MARGIN_HIGH       , // = 11,
    nREG26_VOUT_MARGIN_LOW        , // = 12,
    nREG27_VOUT_TRANSITION_RATE   , // = 13,
    nREG29_VOUT_SCALE_LOOP        , // = 14,
    nREG33_FREQUENCY_SWITCH       , // = 15,
    nREG35_VIN_ON                 , // = 16,
    nREG36_VIN_OFF                , // = 17,
    nREG37_INTERLEAVE             , // = 18,
    nREG40_VOUT_OV_FAULT_LIMIT    , // = 19,
    nREG41_VOUT_OV_FAULT_RESPONSE , // = 20,
    nREG42_VOUT_OV_WARN_LIMIT     , // = 21,
    nREG43_VOUT_UV_WARN_LIMIT     , // = 22,
    nREG44_VOUT_UV_FAULT_LIMIT    , // = 23,
    nREG45_VOUT_UV_FAULT_RESPONSE , // = 24,
    nREG46_IOUT_OC_FAULT_LIMIT    , // = 25,
    nREG47_IOUT_OC_FAULT_RESPONSE , // = 26,
    nREG4A_IOUT_OC_WARN_LIMIT     , // = 27,
    nREG4F_OT_FAULT_LIMIT         , // = 28,
    nREG50_OT_FAULT_RESPONSE      , // = 29,
    nREG51_OT_WARN_LIMIT          , // = 30,
    nREG55_VIN_OV_FAULT_LIMIT     , // = 31,
    nREG56_VIN_OV_FAULT_RESPONSE  , // = 32,
    nREG58_VIN_UV_WARN_LIMIT      , // = 33,
    nREG5D_IIN_OC_WARN_LIMIT      , // = 34,
    nREG5E_POWER_GOOD_ON          , // = 35,
    nREG5F_POWER_GOOD_OFF         , // = 36,
    nREG60_TON_DELAY              , // = 37,
    nREG61_TON_RISE               , // = 38,
    nREG62_TON_MAX_FAULT_LIMIT    , // = 39,
    nREG63_TON_MAX_FAULT_RESPONSE , // = 40,
    nREG64_TOFF_DELAY             , // = 41,
    nREG65_TOFF_FALL              , // = 42,
    nREG66_TOFF_MAX_WARN_LIMIT    , // = 43,
    nREG78_STATUS_BYTE            , // = 44,
    nREG79_STATUS_WORD            , // = 45,
    nREG7A_STATUS_VOUT            , // = 46,
    nREG7B_STATUS_IOUT            , // = 47,
    nREG7C_STATUS_INPUT           , // = 48,
    nREG7D_STATUS_TEMPERATURE     , // = 49,
    nREG7E_STATUS_CML             , // = 50,
    nREG80_STATUS_MFR_SPECIFIC    , // = 51,
    nREG88_READ_VIN               , // = 52,
    nREG89_READ_IIN               , // = 53,
    nREG8B_READ_VOUT              , // = 54,
    nREG8C_READ_IOUT              , // = 55,
    nREG8D_READ_TEMPERATURE       , // = 56,
    nREG94_READ_DUTY_CYCLE        , // = 57,
    nREG96_READ_POUT              , // = 58,
    nREG97_READ_PIN               , // = 59,
    nREG98_PMBUS_REVISION         , // = 60,
    nREG9E_MFR_SERIAL             , // = 61,
    nREGAD_IC_DEVICE_ID           , // = 62,
    nREGD7_MFR_BASE_ADDRESS       , // = 63,
    nREGDB_EEPROM_PASSWORD        , // = 64,
    nREGE2_MFR_BASE_ADDRESS_2     , // = 65,

    nREG_MAX_COUNT,               // Hilfswert für Array-Größen (66)

    nREGXX_ALL = nREG_MAX_COUNT,
    nDELAY     = nREG_MAX_COUNT + 1
  } tenRegIndex;

  u8 u8GetRegCnt()
  {
    return nREG_MAX_COUNT;
  }


  typedef struct
  {
    u8 u8Address; // Die tatsächliche PMBus/I2C Register-Adresse
    u8 u8Length;  // Länge der Daten in Bytes (0 für Send-Byte Kommandos)
  } tenRegLookupEntry;

  // Lookup-Tabelle: Indexiert durch eSiC45xRegIndex
  const tenRegLookupEntry gastRegLookupTable[nREG_MAX_COUNT] =
  {
    // Adresse | Länge | Name / Zuordnung
    { 0x01, 1 }, // nREG01_OPERATION
    { 0x02, 1 }, // nREG02_ON_OFF_CONFIG
    { 0x03, 0 }, // nREG03_CLEAR_FAULTS
    { 0x10, 1 }, // nREG10_WRITE_PROTECT
    { 0x15, 0 }, // nREG15_STORE_USER_ALL
    { 0x16, 0 }, // nREG16_RESTORE_USER_ALL
    { 0x19, 1 }, // nREG19_CAPABILITY
    { 0x20, 1 }, // nREG20_VOUT_MODE
    { 0x21, 2 }, // nREG21_VOUT_COMMAND
    { 0x22, 2 }, // nREG22_VOUT_TRIM
    { 0x24, 2 }, // nREG24_VOUT_MAX
    { 0x25, 2 }, // nREG25_VOUT_MARGIN_HIGH
    { 0x26, 2 }, // nREG26_VOUT_MARGIN_LOW
    { 0x27, 2 }, // nREG27_VOUT_TRANSITION_RATE
    { 0x29, 2 }, // nREG29_VOUT_SCALE_LOOP
    { 0x33, 2 }, // nREG33_FREQUENCY_SWITCH
    { 0x35, 2 }, // nREG35_VIN_ON
    { 0x36, 2 }, // nREG36_VIN_OFF
    { 0x37, 2 }, // nREG37_INTERLEAVE
    { 0x40, 2 }, // nREG40_VOUT_OV_FAULT_LIMIT
    { 0x41, 1 }, // nREG41_VOUT_OV_FAULT_RESPONSE
    { 0x42, 2 }, // nREG42_VOUT_OV_WARN_LIMIT
    { 0x43, 2 }, // nREG43_VOUT_UV_WARN_LIMIT
    { 0x44, 2 }, // nREG44_VOUT_UV_FAULT_LIMIT
    { 0x45, 1 }, // nREG45_VOUT_UV_FAULT_RESPONSE
    { 0x46, 2 }, // nREG46_IOUT_OC_FAULT_LIMIT
    { 0x47, 1 }, // nREG47_IOUT_OC_FAULT_RESPONSE
    { 0x4A, 2 }, // nREG4A_IOUT_OC_WARN_LIMIT
    { 0x4F, 2 }, // nREG4F_OT_FAULT_LIMIT
    { 0x50, 1 }, // nREG50_OT_FAULT_RESPONSE
    { 0x51, 2 }, // nREG51_OT_WARN_LIMIT
    { 0x55, 2 }, // nREG55_VIN_OV_FAULT_LIMIT
    { 0x56, 1 }, // nREG56_VIN_OV_FAULT_RESPONSE
    { 0x58, 2 }, // nREG58_VIN_UV_WARN_LIMIT
    { 0x5D, 2 }, // nREG5D_IIN_OC_WARN_LIMIT
    { 0x5E, 2 }, // nREG5E_POWER_GOOD_ON
    { 0x5F, 2 }, // nREG5F_POWER_GOOD_OFF
    { 0x60, 2 }, // nREG60_TON_DELAY
    { 0x61, 2 }, // nREG61_TON_RISE
    { 0x62, 2 }, // nREG62_TON_MAX_FAULT_LIMIT
    { 0x63, 1 }, // nREG63_TON_MAX_FAULT_RESPONSE
    { 0x64, 2 }, // nREG64_TOFF_DELAY
    { 0x65, 2 }, // nREG65_TOFF_FALL
    { 0x66, 2 }, // nREG66_TOFF_MAX_WARN_LIMIT
    { 0x78, 1 }, // nREG78_STATUS_BYTE
    { 0x79, 2 }, // nREG79_STATUS_WORD
    { 0x7A, 1 }, // nREG7A_STATUS_VOUT
    { 0x7B, 1 }, // nREG7B_STATUS_IOUT
    { 0x7C, 1 }, // nREG7C_STATUS_INPUT
    { 0x7D, 1 }, // nREG7D_STATUS_TEMPERATURE
    { 0x7E, 1 }, // nREG7E_STATUS_CML
    { 0x80, 1 }, // nREG80_STATUS_MFR_SPECIFIC
    { 0x88, 2 }, // nREG88_READ_VIN
    { 0x89, 2 }, // nREG89_READ_IIN
    { 0x8B, 2 }, // nREG8B_READ_VOUT
    { 0x8C, 2 }, // nREG8C_READ_IOUT
    { 0x8D, 2 }, // nREG8D_READ_TEMPERATURE
    { 0x94, 2 }, // nREG94_READ_DUTY_CYCLE
    { 0x96, 2 }, // nREG96_READ_POUT
    { 0x97, 2 }, // nREG97_READ_PIN
    { 0x98, 1 }, // nREG98_PMBUS_REVISION
    { 0x9E, 4 }, // nREG9E_MFR_SERIAL
    { 0xAD, 2 }, // nREGAD_IC_DEVICE_ID
    { 0xD7, 1 }, // nREGD7_MFR_BASE_ADDRESS
    { 0xDB, 2 }, // nREGDB_EEPROM_PASSWORD
    { 0xE2, 1 }  // nREGE2_MFR_BASE_ADDRESS_2
  };

  u8 u8GetRegAdr(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Address;
  }

  u16 u8GetRegLen(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Length;
  }


  // Enum für die Register-Startadressen (Offsets)
  typedef enum
  {
      // --- Steuerung & Konfiguration ---
      nReg_OPERATION                   = 0x01, // R/W Byte
      nReg_ON_OFF_CONFIGURATION        = 0x02, // R/W Byte
      nReg_CLEAR_FAULTS                = 0x03, // Send Byte
      nReg_WRITE_PROTECT               = 0x10, // R/W Byte
      nReg_STORE_USER_ALL              = 0x15, // Send Byte
      nReg_RESTORE_USER_ALL            = 0x16, // Send Byte
      nReg_CAPABILITY                  = 0x19, // Read Byte
      nReg_SMBALERT_MASK               = 0x1B, // Block Read/Write

      // --- Ausgangsspannung ---
      nReg_VOUT_MODE                   = 0x20, // Read Byte
      nReg_VOUT_COMMAND                = 0x21, // R/W Word (L16)
      nReg_VOUT_TRIM                   = 0x22, // R/W Word (L16)
      nReg_VOUT_MAX                    = 0x24, // R/W Word (L16)
      nReg_VOUT_MARGIN_HIGH            = 0x25, // R/W Word (L16)
      nReg_VOUT_MARGIN_LOW             = 0x26, // R/W Word (L16)
      nReg_VOUT_TRANSITION_RATE        = 0x27, // R/W Word (L11)
      nReg_VOUT_SCALE_LOOP             = 0x29, // R/W Word (L11)

      // --- Frequenz & Eingang ---
      nReg_FREQUENCY_SWITCH            = 0x33, // R/W Word (L11)
      nReg_VIN_ON                      = 0x35, // R/W Word (L11)
      nReg_VIN_OFF                     = 0x36, // R/W Word (L11)
      nReg_INTERLEAVE                  = 0x37, // R/W Word

      // --- Fehler- & Warnlimits ---
      nReg_VOUT_OV_FAULT_LIMIT         = 0x40, // R/W Word (L16)
      nReg_VOUT_OV_FAULT_RESPONSE      = 0x41, // R/W Byte
      nReg_VOUT_OV_WARN_LIMIT          = 0x42, // R/W Word (L16)
      nReg_VOUT_UV_WARN_LIMIT          = 0x43, // R/W Word (L16)
      nReg_VOUT_UV_FAULT_LIMIT         = 0x44, // R/W Word (L16)
      nReg_VOUT_UV_FAULT_RESPONSE      = 0x45, // R/W Byte
      nReg_IOUT_OC_FAULT_LIMIT         = 0x46, // R/W Word (L11)
      nReg_IOUT_OC_FAULT_RESPONSE      = 0x47, // R/W Byte
      nReg_IOUT_OC_WARN_LIMIT          = 0x4A, // R/W Word (L11)
      nReg_OT_FAULT_LIMIT              = 0x4F, // R/W Word (L11)
      nReg_OT_FAULT_RESPONSE           = 0x50, // R/W Byte
      nReg_OT_WARN_LIMIT               = 0x51, // R/W Word (L11)
      nReg_VIN_OV_FAULT_LIMIT          = 0x55, // R/W Word (L11)
      nReg_VIN_OV_FAULT_RESPONSE       = 0x56, // R/W Byte
      nReg_VIN_UV_WARN_LIMIT           = 0x58, // R/W Word (L11)
      nReg_IIN_OC_WARN_LIMIT           = 0x5D, // R/W Word (L11)

      // --- Power Good & Timing ---
      nReg_POWER_GOOD_ON               = 0x5E, // R/W Word (L16)
      nReg_POWER_GOOD_OFF              = 0x5F, // R/W Word (L16)
      nReg_TON_DELAY                   = 0x60, // R/W Word (L11)
      nReg_TON_RISE                    = 0x61, // R/W Word (L11)
      nReg_TON_MAX_FAULT_LIMIT         = 0x62, // R/W Word (L11)
      nReg_TON_MAX_FAULT_RESPONSE      = 0x63, // R/W Byte
      nReg_TOFF_DELAY                  = 0x64, // R/W Word (L11)
      nReg_TOFF_FALL                   = 0x65, // R/W Word (L11)
      nReg_TOFF_MAX_WARN_LIMIT         = 0x66, // R/W Word (L11)

      // --- Status Register ---
      nReg_STATUS_BYTE                 = 0x78, // Read Byte
      nReg_STATUS_WORD                 = 0x79, // Read Word
      nReg_STATUS_VOUT                 = 0x7A, // Read Byte
      nReg_STATUS_IOUT                 = 0x7B, // Read Byte
      nReg_STATUS_INPUT                = 0x7C, // Read Byte
      nReg_STATUS_TEMPERATURE          = 0x7D, // Read Byte
      nReg_STATUS_CML                  = 0x7E, // Read Byte
      nReg_STATUS_MFR_SPECIFIC         = 0x80, // Read Byte

      // --- Telemetrie (Read Only) ---
      nReg_READ_VIN                    = 0x88, // Read Word (L11)
      nReg_READ_IIN                    = 0x89, // Read Word (L11)
      nReg_READ_VOUT                   = 0x8B, // Read Word (L16)
      nReg_READ_IOUT                   = 0x8C, // Read Word (L11)
      nReg_READ_TEMPERATURE            = 0x8D, // Read Word (L11)
      nReg_READ_DUTY_CYCLE             = 0x94, // Read Word (L11)
      nReg_READ_POUT                   = 0x96, // Read Word (L11)
      nReg_READ_PIN                    = 0x97, // Read Word (L11)

      // --- Identifikation ---
      nReg_PMBUS_REVISION              = 0x98, // Read Byte
      nReg_MFR_ID                      = 0x99, // Read Block
      nReg_MFR_MODEL                   = 0x9A, // Read Block
      nReg_MFR_REVISION                = 0x9B, // Read Block
      nReg_MFR_LOCATION                = 0x9C, // Read Block
      nReg_MFR_DATE                    = 0x9D, // Read Block
      nReg_MFR_SERIAL                  = 0x9E, // Read Block
      nReg_IC_DEVICE_ID                = 0xAD, // Read Block

      // --- Herstellerspezifisch ---
      nReg_MFR_BASE_ADDRESS            = 0xD7, // Read Byte
      nReg_MFR_BASE_ADDRESS_2          = 0xE2  // Read Byte
  } tenRegisterList;



  // ==============================================================================
  // Register 0x01: OPERATION
  // ==============================================================================
  // Zweck:
  // Schaltet das Gerät ein oder aus und steuert die Margining-Funktionen.
  //
  // Datenformat: Write Byte
  // Default: 0x00
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Reserved0      : 2; // [1:0]: Reserviert
      u8 u8MarginLow      : 1; // [2]: 1 = Margin Low (Aktiv wenn Bit [5:4]=01)
      u8 u8MarginHigh     : 1; // [3]: 1 = Margin High (Aktiv wenn Bit [5:4]=10)
      u8 u8MarginEn       : 2; // [5:4]: 00=Off, 01=Margin Low, 10=Margin High
      u8 u8On             : 1; // [6]: 1 = Gerät Ein, 0 = Gerät Aus (Soft-Off)
      u8 u8UnitEn         : 1; // [7]: 1 = Normaler Betrieb, 0 = Alles Aus
    } Bits;
  } tunRegOperation;

  // ==============================================================================
  // Register 0x02: ON_OFF_CONFIG
  // ==============================================================================
  // Zweck:
  // Konfiguriert die Reaktion auf den CONTROL-Pin und das OPERATION-Register.
  //
  // Datenformat: Write Byte
  // Default: 0x1F (Abhängig von NVM/Pinstrap)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8WriteProtect   : 1; // [0]: 1 = Nur OPERATION Bit [7] änderbar
      u8 u8Polarity       : 1; // [1]: 1 = Active High, 0 = Active Low (Control Pin)
      u8 u8ControlPinEn   : 1; // [2]: 1 = CONTROL Pin erforderlich zum Einschalten
      u8 u8OperationEn    : 1; // [3]: 1 = OPERATION Register erforderlich zum Einschalten
      u8 u8OffBehavior    : 1; // [4]: 1 = Sofort Aus, 0 = Soft-Off (nach VOUT_OFF_DELAY)
      u8 u8Reserved       : 3; // [7:5]: Reserviert
    } Bits;
  } tunRegOnOffConfig;

  // ==============================================================================
  // Register 0x10: WRITE_PROTECT
  // ==============================================================================
  // Zweck:
  // Steuert die Schreibrechte auf die Geräteregister, um versehentliche
  // Änderungen während des Betriebs zu verhindern.
  //
  // Datenformat: Write Byte
  // Default-Verhalten: 0x00 (Alle Schreibzugriffe erlaubt)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Reserved0      : 5; // [4:0]: Reserviert (immer 0)
      u8 u8VoutProt       : 1; // [5]: 1 = Erlaubt nur WP, OPERATION, ON_OFF, VOUT_CMD
      u8 u8OpProt         : 1; // [6]: 1 = Erlaubt nur WP, OPERATION, ON_OFF
      u8 u8AllProt        : 1; // [7]: 1 = Sperrt alle Schreibvorgänge außer auf WP
    } Bits;
  } tunRegWriteProtect;

  // ==============================================================================
  // Register 0x19: CAPABILITY
  // ==============================================================================
  // Zweck:
  // Bietet Informationen über die Schlüsselfunktionen des Geräts (PEC, Speed, Alert).
  //
  // Datenformat: Read Byte
  // Default-Verhalten: Gerätespezifisch (SiC45x: 0xB0)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Reserved0      : 4; // [3:0]: Reserviert
      u8 u8Smbalert       : 1; // [4]: SMBALERT# Support (0 = nicht unterstützt, 1 = unterstützt)
      u8 u8MaxSpeed       : 2; // [6:5]: Maximale Bus-Geschwindigkeit (00 = 100kHz, 01 = 400kHz)
      u8 u8PecSupport     : 1; // [7]: Packet Error Checking Support (0 = nicht unterstützt, 1 = unterstützt)
    } Bits;
  } tunRegCapability;

  // ==============================================================================
  // Register 0x20: VOUT_MODE
  // ==============================================================================
  // Zweck: Bestimmt das Datenformat für alle auf die Ausgangsspannung bezogenen Befehle.
  // Datenformat: Read Byte
  // Default: 0x17 (Linear Mode, Exponent -9)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Parameter      : 5; // [4:0]: Exponent (Zweierkomplement) für Linear Mode
      u8 u8Mode           : 3; // [7:5]: 000 = Linear Mode
    } Bits;
  } tunRegVoutMode;

  // ==============================================================================
  // Register 0x21: VOUT_COMMAND
  // ==============================================================================
  // Zweck: Setzt den Zielwert der Ausgangsspannung.
  // Datenformat: Write Word (Linear16)
  // Bit-Semantik: [15:0]: Unsigned mantissa (Value = Mantissa * 2^VOUT_MODE_Exp)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutCommand;

  // ==============================================================================
  // Register 0x22: VOUT_TRIM
  // ==============================================================================
  // Zweck: Korrekturwert zur Feinjustierung der Ausgangsspannung.
  // Datenformat: Write Word (Linear16, Vorzeichenbehaftet)
  // Bit-Semantik: [15:0]: Signed mantissa (Zweierkomplement)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutTrim;

  // ==============================================================================
  // Register 0x24: VOUT_MAX
  // ==============================================================================
  // Zweck: Oberste Sicherheitsgrenze für die Ausgangsspannung.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutMax;

  // ==============================================================================
  // Register 0x25: VOUT_MARGIN_HIGH
  // ==============================================================================
  // Zweck: Zielspannung wenn OPERATION auf Margin High gesetzt wird.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutMarginHigh;

  // ==============================================================================
  // Register 0x26: VOUT_MARGIN_LOW
  // ==============================================================================
  // Zweck: Zielspannung wenn OPERATION auf Margin Low gesetzt wird.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutMarginLow;


  // ==============================================================================
  // Register 0x27: VOUT_TRANSITION_RATE
  // ==============================================================================
  // Zweck: Bestimmt die Rate (Slew Rate), mit der sich die Ausgangsspannung ändert.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutTransitionRate;

  // ==============================================================================
  // Register 0x29: VOUT_SCALE_LOOP
  // ==============================================================================
  // Zweck: Skalierungsfaktor für den Feedback-Pfad (interner Spannungsteiler).
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutScaleLoop;

  // ==============================================================================
  // Register 0x33: FREQUENCY_SWITCH
  // ==============================================================================
  // Zweck: Setzt die Schaltfrequenz des Converters in kHz.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegFrequencySwitch;

  // ==============================================================================
  // Register 0x35: VIN_ON
  // ==============================================================================
  // Zweck: Eingangsspannungswert, bei dem der Converter den Betrieb aufnimmt.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegVinOn;

  // ==============================================================================
  // Register 0x36: VIN_OFF
  // ==============================================================================
  // Zweck: Eingangsspannungswert, bei dem der Converter den Betrieb einstellt.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegVinOff;

  // ==============================================================================
  // Register 0x37: INTERLEAVE
  // ==============================================================================
  // Zweck: Konfiguriert die Phasenverschiebung bei mehreren Geräten am Bus.
  // Datenformat: Write Word
  typedef union
  {
    u16 u16Value;
    struct
    {
      u16 u16GroupNumber  : 4; // [3:0]: Gruppennummer für die Synchronisation
      u16 u16OrderNumber  : 4; // [7:4]: Position innerhalb der Gruppe (Phasenlage)
      u16 u16Reserved     : 8; // [15:8]: Reserviert
    } Bits;
  } tunRegInterleave;

  // ==============================================================================
  // Register 0x40: VOUT_OV_FAULT_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die VOUT-Überspannungsabschaltung.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutOvFaultLimit;


  // ==============================================================================
  // Register 0x41: VOUT_OV_FAULT_RESPONSE
  // ==============================================================================
  // Zweck: Bestimmt die Reaktion des Geräts auf einen VOUT-Überspannungsfehler.
  // Datenformat: Write Byte
  // Default: 0x80 (Sofort aus, keine Wiederholung)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8RetrySetting   : 3; // [2:0]: 000=Kein Retry, 111=Dauerhafter Retry, sonst Anzahl
      u8 u8RetryDelay     : 3; // [5:3]: Zeit zwischen den Versuchen (Einheit gerätespezifisch)
      u8 u8ResponseConfig : 2; // [7:6]: 00=Ignore, 01=Weiterbetrieb, 10=Sofort Aus, 11=Reserviert
    } Bits;
  } tunRegVoutOvFaultResponse;

  // ==============================================================================
  // Register 0x42: VOUT_OV_WARN_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die VOUT-Überspannungswarnung.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutOvWarnLimit;

  // ==============================================================================
  // Register 0x43: VOUT_UV_WARN_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die VOUT-Unterspannungswarnung.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutUvWarnLimit;

  // ==============================================================================
  // Register 0x44: VOUT_UV_FAULT_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die VOUT-Unterspannungsabschaltung.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegVoutUvFaultLimit;

  // ==============================================================================
  // Register 0x45: VOUT_UV_FAULT_RESPONSE
  // ==============================================================================
  // Zweck: Bestimmt die Reaktion des Geräts auf einen VOUT-Unterspannungsfehler.
  // Datenformat: Write Byte
  // Default: 0x00 (Ignorieren laut PMBus, SiC45x meist 0x80 oder 0x00)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8RetrySetting   : 3; // [2:0]: 000=Kein Retry, 111=Dauerhafter Retry
      u8 u8RetryDelay     : 3; // [5:3]: Verzögerungszeit
      u8 u8ResponseConfig : 2; // [7:6]: 00=Ignore, 01=Weiterbetrieb, 10=Sofort Aus
    } Bits;
  } tunRegVoutUvFaultResponse;

  // ==============================================================================
  // Register 0x46: IOUT_OC_FAULT_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die Ausgangsstrom-Überlastabschaltung (Overcurrent).
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegIoutOcFaultLimit;

  // ==============================================================================
  // Register 0x47: IOUT_OC_FAULT_RESPONSE
  // ==============================================================================
  // Zweck: Bestimmt die Reaktion auf einen IOUT-Überlastfehler.
  // Datenformat: Write Byte
  // Default: 0x00 (Gerätespezifisch)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8RetrySetting   : 3; // [2:0]: 000=Kein Retry, 111=Dauerhafter Retry
      u8 u8RetryDelay     : 3; // [5:3]: Verzögerungszeit zwischen Versuchen
      u8 u8ResponseConfig : 2; // [7:6]: 00=Ignore, 01=Weiterbetrieb, 10=Sofort Aus
    } Bits;
  } tunRegIoutOcFaultResponse;

  // ==============================================================================
  // Register 0x4A: IOUT_OC_WARN_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die Ausgangsstrom-Überlastwarnung.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegIoutOcWarnLimit;

  // ==============================================================================
  // Register 0x4F: OT_FAULT_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die Übertemperaturabschaltung (Overtemperature).
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegOtFaultLimit;

  // ==============================================================================
  // Register 0x50: OT_FAULT_RESPONSE
  // ==============================================================================
  // Zweck: Bestimmt die Reaktion auf einen Übertemperaturfehler.
  // Datenformat: Write Byte
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8RetrySetting   : 3; // [2:0]: 000=Kein Retry, 111=Dauerhafter Retry
      u8 u8RetryDelay     : 3; // [5:3]: Verzögerungszeit
      u8 u8ResponseConfig : 2; // [7:6]: 00=Ignore, 01=Weiterbetrieb, 10=Sofort Aus
    } Bits;
  } tunRegOtFaultResponse;


  // ==============================================================================
  // Register 0x51: OT_WARN_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die Übertemperaturwarnung.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegOtWarnLimit;

  // ==============================================================================
  // Register 0x55: VIN_OV_FAULT_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die Eingangs-Überspannungsabschaltung.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegVinOvFaultLimit;

  // ==============================================================================
  // Register 0x56: VIN_OV_FAULT_RESPONSE
  // ==============================================================================
  // Zweck: Bestimmt die Reaktion auf einen VIN-Überspannungsfehler.
  // Datenformat: Write Byte
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8RetrySetting   : 3; // [2:0]: 000=Kein Retry, 111=Dauerhafter Retry
      u8 u8RetryDelay     : 3; // [5:3]: Verzögerungszeit
      u8 u8ResponseConfig : 2; // [7:6]: 00=Ignore, 01=Weiterbetrieb, 10=Sofort Aus
    } Bits;
  } tunRegVinOvFaultResponse;

  // ==============================================================================
  // Register 0x58: VIN_UV_WARN_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die Eingangs-Unterspannungswarnung.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegVinUvWarnLimit;

  // ==============================================================================
  // Register 0x5D: IIN_OC_WARN_LIMIT
  // ==============================================================================
  // Zweck: Schwellwert für die Eingangsstrom-Überlastwarnung.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegIinOcWarnLimit;


  // ==============================================================================
  // Register 0x5E: POWER_GOOD_ON
  // ==============================================================================
  // Zweck: Ausgangsspannungswert, bei dem das PGOOD-Signal auf TRUE gesetzt wird.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegPowerGoodOn;

  // ==============================================================================
  // Register 0x5F: POWER_GOOD_OFF
  // ==============================================================================
  // Zweck: Ausgangsspannungswert, bei dem das PGOOD-Signal auf FALSE gesetzt wird.
  // Datenformat: Write Word (Linear16)
  typedef union
  {
    u16 u16Value;
  } tunRegPowerGoodOff;

  // ==============================================================================
  // Register 0x60: TON_DELAY
  // ==============================================================================
  // Zweck: Zeit vom Empfang des Einschaltbefehls bis zum Start des Spannungsanstiegs.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegTonDelay;

  // ==============================================================================
  // Register 0x61: TON_RISE
  // ==============================================================================
  // Zweck: Zeit, in der die Ausgangsspannung von 0V auf den Zielwert ansteigt.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegTonRise;

  // ==============================================================================
  // Register 0x62: TON_MAX_FAULT_LIMIT
  // ==============================================================================
  // Zweck: Maximale Zeit, die der Ausgang benötigt, um die PGOOD-Schwelle zu erreichen.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegTonMaxFaultLimit;

  // ==============================================================================
  // Register 0x63: TON_MAX_FAULT_RESPONSE
  // ==============================================================================
  // Zweck: Reaktion, wenn die Ausgangsspannung nicht rechtzeitig ansteigt.
  // Datenformat: Write Byte
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8RetrySetting   : 3; // [2:0]: 000=Kein Retry, 111=Dauerhafter Retry
      u8 u8RetryDelay     : 3; // [5:3]: Verzögerungszeit
      u8 u8ResponseConfig : 2; // [7:6]: 00=Ignore, 01=Weiterbetrieb, 10=Sofort Aus
    } Bits;
  } tunRegTonMaxFaultResponse;




  // ==============================================================================
  // Register 0x64: TOFF_DELAY
  // ==============================================================================
  // Zweck: Zeit vom Empfang des Ausschaltbefehls bis zum Start des Spannungsabfalls.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegToffDelay;

  // ==============================================================================
  // Register 0x65: TOFF_FALL
  // ==============================================================================
  // Zweck: Zeit, in der die Ausgangsspannung vom Zielwert auf 0V abfällt.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegToffFall;

  // ==============================================================================
  // Register 0x66: TOFF_MAX_WARN_LIMIT
  // ==============================================================================
  // Zweck: Warnschwelle für die maximale Zeit, die der Ausgang zum Abschalten benötigt.
  // Datenformat: Write Word (Linear11)
  typedef union
  {
    u16 u16Value;
  } tunRegToffMaxWarnLimit;


  // ==============================================================================
  // Register 0x78: STATUS_BYTE
  // ==============================================================================
  // Zweck: Bietet eine kurze Zusammenfassung der wichtigsten Fehlerzustände.
  // Datenformat: Read Byte (Write 1 to Clear)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8NoneOfAbove    : 1; // [0]: Fehler aufgetreten, der nicht in Bits 1-7 steht
      u8 u8Cml            : 1; // [1]: Kommunikations-, Speicher- oder Logikfehler
      u8 u8Temperature    : 1; // [2]: Übertemperatur-Fehler oder -Warnung
      u8 u8VinUv          : 1; // [3]: Eingangs-Unterspannungsfehler
      u8 u8IoutOc         : 1; // [4]: Ausgangs-Überstromfehler
      u8 u8VoutOv         : 1; // [5]: Ausgangs-Überspannungsfehler
      u8 u8Off            : 1; // [6]: Gerät liefert keine Spannung (aus beliebigem Grund)
      u8 u8Busy           : 1; // [7]: Gerät ist beschäftigt (z.B. NVM Zugriff)
    } Bits;
  } tunRegStatusByte;

  // ==============================================================================
  // Register 0x79: STATUS_WORD
  // ==============================================================================
  // Zweck: Erweiterter Status. Das Low-Byte ist identisch mit STATUS_BYTE.
  // Datenformat: Read Word (Write 1 to Clear)
  typedef union
  {
    u16 u16Value;
    struct
    {
      // Low Byte (identisch zu STATUS_BYTE)
      u16 u16LowNone      : 1; // [0]: None of the above
      u16 u16LowCml       : 1; // [1]: CML Fault
      u16 u16LowTemp      : 1; // [2]: Temperature Fault/Warn
      u16 u16LowVinUv     : 1; // [3]: VIN UV Fault
      u16 u16LowIoutOc    : 1; // [4]: IOUT OC Fault
      u16 u16LowVoutOv    : 1; // [5]: VOUT OV Fault
      u16 u16LowOff       : 1; // [6]: Unit Off
      u16 u16LowBusy      : 1; // [7]: Busy
      // High Byte
      u16 u16HighUnknown  : 1; // [8]: Undefinierter Fehler
      u16 u16HighOther    : 1; // [9]: Anderes Warn- oder Fehlersignal
      u16 u16HighFans     : 1; // [10]: Fan Fault (Nicht genutzt bei SiC45x)
      u16 u16HighMfr      : 1; // [11]: Herstellerspezifischer Fehler
      u16 u16HighInput    : 1; // [12]: Eingangs-Fehler oder -Warnung (VIN/IIN)
      u16 u16HighIoutPout : 1; // [13]: Ausgangsstrom/-leistung Fehler oder Warnung
      u16 u16HighVout     : 1; // [14]: Ausgangsspannungs-Fehler oder -Warnung
      u16 u16HighNone     : 1; // [15]: Keine der obigen High-Byte Bedingungen
    } Bits;
  } tunRegStatusWord;

  // ==============================================================================
  // Register 0x7A: STATUS_VOUT
  // ==============================================================================
  // Zweck: Detaillierte Fehlerinformationen zur Ausgangsspannung.
  // Datenformat: Read Byte (Write 1 to Clear)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8VoutTrackingErr : 1; // [0]: VOUT Tracking Fehler
      u8 u8SoftStartErr    : 1; // [1]: Soft-Start Fehler
      u8 u8TonMaxFault     : 1; // [2]: TON_MAX Fehler
      u8 u8VoutMaxWarn     : 1; // [3]: VOUT_MAX Warnung (Anschlag am Limit)
      u8 u8VoutUvFault     : 1; // [4]: VOUT Unterspannungs-Fehler
      u8 u8VoutUvWarn      : 1; // [5]: VOUT Unterspannungs-Warnung
      u8 u8VoutOvWarn      : 1; // [6]: VOUT Überspannungs-Warnung
      u8 u8VoutOvFault     : 1; // [7]: VOUT Überspannungs-Fehler
    } Bits;
  } tunRegStatusVout;

  // ==============================================================================
  // Register 0x7B: STATUS_IOUT
  // ==============================================================================
  // Zweck: Detaillierte Fehlerinformationen zum Ausgangsstrom.
  // Datenformat: Read Byte (Write 1 to Clear)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Reserved        : 1; // [0]: Reserviert
      u8 u8IoutUcFault     : 1; // [1]: Ausgangs-Unterstromfehler (Undercurrent)
      u8 u8IoutOcLvFault   : 1; // [2]: IOUT OC Low Voltage Fehler
      u8 u8IoutOcWarn      : 1; // [3]: Ausgangs-Überstromwarnung
      u8 u8IoutOcFault     : 1; // [4]: Ausgangs-Überstromfehler
      u8 u8IoutOcAvWarn    : 1; // [5]: Durchschnittsstrom Überlastwarnung
      u8 u8IoutOcAvFault   : 1; // [6]: Durchschnittsstrom Überlastfehler
      u8 u8IoutPoutOcFault : 1; // [7]: Ausgangsstrom oder Leistung Fehler
    } Bits;
  } tunRegStatusIout;

  // ==============================================================================
  // Register 0x7C: STATUS_INPUT
  // ==============================================================================
  // Zweck: Detaillierte Fehlerinformationen zur Eingangsspannung/Strom.
  // Datenformat: Read Byte (Write 1 to Clear)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8PinLow          : 1; // [0]: Eingang PIN zu niedrig
      u8 u8IinOcWarn       : 1; // [1]: Eingangs-Überstromwarnung
      u8 u8IinOcFault      : 1; // [2]: Eingangs-Überstromfehler
      u8 u8UnitOffLowVin   : 1; // [3]: Gerät aus wegen zu geringer Eingangsspannung
      u8 u8VinUvFault      : 1; // [4]: Eingangs-Unterspannungsfehler
      u8 u8VinUvWarn       : 1; // [5]: Eingangs-Unterspannungs-Warnung
      u8 u8VinOvWarn       : 1; // [6]: Eingangs-Überspannungs-Warnung
      u8 u8VinOvFault      : 1; // [7]: Eingangs-Überspannungs-Fehler
    } Bits;
  } tunRegStatusInput;

  // ==============================================================================
  // Register 0x7D: STATUS_TEMPERATURE
  // ==============================================================================
  // Zweck: Fehlerinformationen zur Gerätetemperatur.
  // Datenformat: Read Byte (Write 1 to Clear)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Reserved        : 4; // [3:0]: Reserviert
      u8 u8UtFault         : 1; // [4]: Untertemperatur-Fehler
      u8 u8UtWarn          : 1; // [5]: Untertemperatur-Warnung
      u8 u8OtWarn          : 1; // [6]: Übertemperatur-Warnung
      u8 u8OtFault         : 1; // [7]: Übertemperatur-Fehler
    } Bits;
  } tunRegStatusTemperature;

  // ==============================================================================
  // Register 0x7E: STATUS_CML
  // ==============================================================================
  // Zweck: Fehler in Kommunikation, Logik oder Speicher (Communication, Memory, Logic).
  // Datenformat: Read Byte (Write 1 to Clear)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8OtherError      : 1; // [0]: Anderer Speicher- oder Logikfehler
      u8 u8CommError       : 1; // [1]: Kommunikationsfehler (z.B. unbekanntes Kommando)
      u8 u8Reserved        : 1; // [2]: Reserviert
      u8 u8ProcessorFault  : 1; // [3]: Interner Prozessorfehler
      u8 u8MemoryFault     : 1; // [4]: Speicherfehler (NVM CRC)
      u8 u8PacketError     : 1; // [5]: PEC Fehler
      u8 u8InvalidData     : 1; // [6]: Ungültige oder unzulässige Daten erhalten
      u8 u8InvalidCommand  : 1; // [7]: Ungültiges oder nicht unterstütztes Kommando
    } Bits;
  } tunRegStatusCml;

  // ==============================================================================
  // Register 0x80: STATUS_MFR_SPECIFIC
  // ==============================================================================
  // Zweck: Herstellerspezifische Status-Informationen für Vishay SiC45x.
  // Datenformat: Read Byte (Write 1 to Clear)
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8MfrBit0         : 1; // [0]: Herstellerspezifisch
      u8 u8MfrBit1         : 1; // [1]: Herstellerspezifisch
      u8 u8MfrBit2         : 1; // [2]: Herstellerspezifisch
      u8 u8MfrBit3         : 1; // [3]: Herstellerspezifisch
      u8 u8MfrBit4         : 1; // [4]: Herstellerspezifisch
      u8 u8MfrBit5         : 1; // [5]: Herstellerspezifisch
      u8 u8MfrBit6         : 1; // [6]: Herstellerspezifisch
      u8 u8FaultPresent    : 1; // [7]: 1 = Mindestens ein Fehlerzustand ist aktiv
    } Bits;
  } tunRegStatusMfrSpecific;

  // ==============================================================================
  // Register 0x88: READ_VIN
  // ==============================================================================
  // Zweck: Liefert die gemessene Eingangsspannung.
  // Datenformat: Read Word (Linear11)
  typedef union
  {
    u16 u16Value; // [15:0]: Wert im Linear11 Format (Mantisse + Exponent)
  } tunRegReadVin;

  // ==============================================================================
  // Register 0x89: READ_IIN
  // ==============================================================================
  // Zweck: Liefert den gemessenen Eingangsstrom.
  // Datenformat: Read Word (Linear11)
  typedef union
  {
    u16 u16Value; // [15:0]: Wert im Linear11 Format
  } tunRegReadIin;

  // ==============================================================================
  // Register 0x8B: READ_VOUT
  // ==============================================================================
  // Zweck: Liefert die aktuell gemessene Ausgangsspannung.
  // Datenformat: Read Word (Linear16)
  // Bit-Semantik: Wert = Mantisse * 2^(VOUT_MODE Exponent)
  typedef union
  {
    u16 u16Value; // [15:0]: Unsigned Mantissa
  } tunRegReadVout;

  // ==============================================================================
  // Register 0x8C: READ_IOUT
  // ==============================================================================
  // Zweck: Liefert den gemessenen Ausgangsstrom.
  // Datenformat: Read Word (Linear11)
  typedef union
  {
    u16 u16Value; // [15:0]: Wert im Linear11 Format
  } tunRegReadIout;

  // ==============================================================================
  // Register 0x8D: READ_TEMPERATURE
  // ==============================================================================
  // Zweck: Liefert die gemessene interne Temperatur des ICs.
  // Datenformat: Read Word (Linear11)
  typedef union
  {
    u16 u16Value; // [15:0]: Temperatur in °C
  } tunRegReadTemperature;

  // ==============================================================================
  // Register 0x94: READ_DUTY_CYCLE
  // ==============================================================================
  // Zweck: Liefert das aktuelle Tastverhältnis des PWM-Signals in Prozent.
  // Datenformat: Read Word (Linear11)
  typedef union
  {
    u16 u16Value; // [15:0]: Duty Cycle in %
  } tunRegReadDutyCycle;

  // ==============================================================================
  // Register 0x96: READ_POUT
  // ==============================================================================
  // Zweck: Liefert die berechnete Ausgangsleistung.
  // Datenformat: Read Word (Linear11)
  typedef union
  {
    u16 u16Value; // [15:0]: Leistung in Watt
  } tunRegReadPout;

  // ==============================================================================
  // Register 0x97: READ_PIN
  // ==============================================================================
  // Zweck: Liefert die gemessene Eingangsleistung.
  // Datenformat: Read Word (Linear11)
  typedef union
  {
    u16 u16Value; // [15:0]: Leistung in Watt
  } tunRegReadPin;

  // ==============================================================================
  // Register 0x98: PMBUS_REVISION
  // ==============================================================================
  // Zweck: Gibt die unterstützte PMBus-Spezifikation an.
  // Datenformat: Read Byte
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8PartII         : 4; // [3:0]: Revision Teil II (0011 = Rev 1.3)
      u8 u8PartI          : 4; // [7:4]: Revision Teil I (0011 = Rev 1.3)
    } Bits;
  } tunRegPmbusRevision;

  // ==============================================================================
  // Register 0x9E: MFR_SERIAL
  // ==============================================================================
  // Zweck: Herstellerspezifische Seriennummer oder Identifikation.
  // Datenformat: Block Read (Typischerweise 2 Bytes bei SiC45x)
  typedef union
  {
    u16 u16Value; // [15:0]: Seriennummer / Code
  } tunRegMfrSerial;

  // ==============================================================================
  // Register 0xAD: IC_DEVICE_ID
  // ==============================================================================
  // Zweck: Identifiziert den Gerätetyp (z.B. SiC450, 451, 453).
  // Datenformat: Block Read
  typedef union
  {
    u16 u16Value; // [15:0]: Eindeutige Geräte-ID
  } tunRegIcDeviceId;

  // ==============================================================================
  // Register 0xD7: MFR_BASE_ADDRESS
  // ==============================================================================
  // Zweck: Konfiguriert die Basis-I2C-Adresse des Geräts.
  // Datenformat: Read/Write Byte
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Address        : 7; // [6:0]: 7-Bit I2C Basis-Adresse
      u8 u8Reserved       : 1; // [7]: Reserviert
    } Bits;
  } tunRegMfrBaseAddress;

  // ==============================================================================
  // Register 0xDB: EEPROM_PASSWORD
  // ==============================================================================
  // Zweck: Passwort-Eingabe zum Entsperren von NVM/EEPROM Schreibzugriffen.
  // Datenformat: Write Word
  typedef union
  {
    u16 u16Value; // [15:0]: Passwort-Code
  } tunRegEepromPassword;

  // ==============================================================================
  // Register 0xE2: MFR_BASE_ADDRESS_2
  // ==============================================================================
  // Zweck: Alternative oder sekundäre Basis-Adresse für das Management.
  // Datenformat: Read/Write Byte
  typedef union
  {
    u8 u8Value;
    struct
    {
      u8 u8Address        : 7; // [6:0]: 7-Bit Alternative I2C Adresse
      u8 u8Reserved       : 1; // [7]: Reserviert
    } Bits;
  } tunRegMfrBaseAddress2;

  // Control‑& Basis‑Konfiguration
  tunRegOperation           munREG01_Operation;
  tunRegOnOffConfig         munREG02_OnOffConfig;
  tunRegWriteProtect        munREG10_WriteProtect;
  tunRegCapability          munREG19_Capability;

  // Output‑Voltage‑Konfiguration(VOUT)
  tunRegVoutMode            munREG20_VoutMode;
  tunRegVoutCommand         munREG21_VoutCommand;
  tunRegVoutTrim            munREG22_VoutTrim;
  tunRegVoutMax             munREG24_VoutMax;
  tunRegVoutMarginHigh      munREG25_VoutMarginHigh;
  tunRegVoutMarginLow       munREG26_VoutMarginLow;
  tunRegVoutTransitionRate  munREG27_VoutTransitionRate;
  tunRegVoutScaleLoop       munREG29_VoutScaleLoop;

  // Switching‑& Betriebsparameter
  tunRegFrequencySwitch     munREG33_FrequencySwitch;
  tunRegVinOn               munREG35_VinOn;
  tunRegVinOff              munREG36_VinOff;
  tunRegInterleave          munREG37_Interleave;

  // Fault‑& Warning‑Grenzen(Output / Input)
  tunRegVoutOvFaultLimit    munREG40_VoutOvFaultLimit;
  tunRegVoutOvFaultResponse munREG41_VoutOvFaultResponse;
  tunRegVoutOvWarnLimit     munREG42_VoutOvWarnLimit;
  tunRegVoutUvWarnLimit     munREG43_VoutUvWarnLimit;
  tunRegVoutUvFaultLimit    munREG44_VoutUvFaultLimit;
  tunRegVoutUvFaultResponse munREG45_VoutUvFaultResponse;
  tunRegIoutOcFaultLimit    munREG46_IoutOcFaultLimit;
  tunRegIoutOcFaultResponse munREG47_IoutOcFaultResponse;
  tunRegIoutOcWarnLimit     munREG4A_IoutOcWarnLimit;
  tunRegOtFaultLimit        munREG4F_OtFaultLimit;
  tunRegOtFaultResponse     munREG50_OtFaultResponse;
  tunRegOtWarnLimit         munREG51_OtWarnLimit;
  tunRegVinOvFaultLimit     munREG55_VinOvFaultLimit;
  tunRegVinOvFaultResponse  munREG56_VinOvFaultResponse;
  tunRegVinUvWarnLimit      munREG58_VinUvWarnLimit;
  tunRegIinOcWarnLimit      munREG5D_IinOcWarnLimit;

  // Power‑Good& Sequencing / Timing
  tunRegPowerGoodOn         munREG5E_PowerGoodOn;
  tunRegPowerGoodOff        munREG5F_PowerGoodOff;
  tunRegTonDelay            munREG60_TonDelay;
  tunRegTonRise             munREG61_TonRise;
  tunRegTonMaxFaultLimit    munREG62_TonMaxFaultLimit;
  tunRegTonMaxFaultResponse munREG63_TonMaxFaultResponse;
  tunRegToffDelay           munREG64_ToffDelay;
  tunRegToffFall            munREG65_ToffFall;
  tunRegToffMaxWarnLimit    munREG66_ToffMaxWarnLimit;

  //Status‑Register(Read‑Only)
  tunRegStatusByte          munREG78_StatusByte;
  tunRegStatusWord          munREG79_StatusWord;
  tunRegStatusVout          munREG7A_StatusVout;
  tunRegStatusIout          munREG7B_StatusIout;
  tunRegStatusInput         munREG7C_StatusInput;
  tunRegStatusTemperature   munREG7D_StatusTemperature;
  tunRegStatusCml           munREG7E_StatusCml;
  tunRegStatusMfrSpecific   munREG80_StatusMfrSpecific;

  // Telemetrie / Messwerte(READ_*)
  tunRegReadVin             munREG88_ReadVin;
  tunRegReadIin             munREG89_ReadIin;
  tunRegReadVout            munREG8B_ReadVout;
  tunRegReadIout            munREG8C_ReadIout;
  tunRegReadTemperature     munREG8D_ReadTemperature;
  tunRegReadDutyCycle       munREG94_ReadDutyCycle;
  tunRegReadPout            munREG96_ReadPout;
  tunRegReadPin             munREG97_ReadPin;
  tunRegPmbusRevision       munREG98_PmbusRevision;

  // Hersteller‑& Geräte‑Register
  tunRegMfrSerial           munREG9E_MfrSerial;
  tunRegIcDeviceId          munREGAD_IcDeviceId;
  tunRegMfrBaseAddress      munREGD7_MfrBaseAddress;
  tunRegEepromPassword      munREGDB_EepromPassword;
  tunRegMfrBaseAddress2     munREGE2_MfrBaseAddress2;

  u8* pu8GetMem(u8 lu8RegIdx)
  {
    switch ((tenRegIndex)lu8RegIdx)
    {
      case nREG01_OPERATION:              return (u8*)&munREG01_Operation; break;
      case nREG02_ON_OFF_CONFIG:          return (u8*)&munREG02_OnOffConfig; break;
      case nREG03_CLEAR_FAULTS:           return (u8*)null; break;
      case nREG10_WRITE_PROTECT:          return (u8*)&munREG10_WriteProtect; break;
      case nREG15_STORE_USER_ALL:         return (u8*)null; break;
      case nREG16_RESTORE_USER_ALL:       return (u8*)null; break;
      case nREG19_CAPABILITY:             return (u8*)&munREG19_Capability; break;
      case nREG20_VOUT_MODE:              return (u8*)&munREG20_VoutMode; break;
      case nREG21_VOUT_COMMAND:           return (u8*)&munREG21_VoutCommand; break;
      case nREG22_VOUT_TRIM:              return (u8*)&munREG22_VoutTrim; break;
      case nREG24_VOUT_MAX:               return (u8*)&munREG24_VoutMax; break;
      case nREG25_VOUT_MARGIN_HIGH:       return (u8*)&munREG25_VoutMarginHigh; break;
      case nREG26_VOUT_MARGIN_LOW:        return (u8*)&munREG26_VoutMarginLow; break;
      case nREG27_VOUT_TRANSITION_RATE:   return (u8*)&munREG27_VoutTransitionRate; break;
      case nREG29_VOUT_SCALE_LOOP:        return (u8*)&munREG29_VoutScaleLoop; break;
      case nREG33_FREQUENCY_SWITCH:       return (u8*)&munREG33_FrequencySwitch; break;
      case nREG35_VIN_ON:                 return (u8*)&munREG35_VinOn; break;
      case nREG36_VIN_OFF:                return (u8*)&munREG36_VinOff; break;
      case nREG37_INTERLEAVE:             return (u8*)&munREG37_Interleave; break;
      case nREG40_VOUT_OV_FAULT_LIMIT:    return (u8*)&munREG40_VoutOvFaultLimit; break;
      case nREG41_VOUT_OV_FAULT_RESPONSE: return (u8*)&munREG41_VoutOvFaultResponse; break;
      case nREG42_VOUT_OV_WARN_LIMIT:     return (u8*)&munREG42_VoutOvWarnLimit; break;
      case nREG43_VOUT_UV_WARN_LIMIT:     return (u8*)&munREG43_VoutUvWarnLimit; break;
      case nREG44_VOUT_UV_FAULT_LIMIT:    return (u8*)&munREG44_VoutUvFaultLimit; break;
      case nREG45_VOUT_UV_FAULT_RESPONSE: return (u8*)&munREG45_VoutUvFaultResponse; break;
      case nREG46_IOUT_OC_FAULT_LIMIT:    return (u8*)&munREG46_IoutOcFaultLimit; break;
      case nREG47_IOUT_OC_FAULT_RESPONSE: return (u8*)&munREG47_IoutOcFaultResponse; break;
      case nREG4A_IOUT_OC_WARN_LIMIT:     return (u8*)&munREG4A_IoutOcWarnLimit; break;
      case nREG4F_OT_FAULT_LIMIT:         return (u8*)&munREG4F_OtFaultLimit; break;
      case nREG50_OT_FAULT_RESPONSE:      return (u8*)&munREG50_OtFaultResponse; break;
      case nREG51_OT_WARN_LIMIT:          return (u8*)&munREG51_OtWarnLimit; break;
      case nREG55_VIN_OV_FAULT_LIMIT:     return (u8*)&munREG55_VinOvFaultLimit; break;
      case nREG56_VIN_OV_FAULT_RESPONSE:  return (u8*)&munREG56_VinOvFaultResponse; break;
      case nREG58_VIN_UV_WARN_LIMIT:      return (u8*)&munREG58_VinUvWarnLimit; break;
      case nREG5D_IIN_OC_WARN_LIMIT:      return (u8*)&munREG5D_IinOcWarnLimit; break;
      case nREG5E_POWER_GOOD_ON:          return (u8*)&munREG5E_PowerGoodOn; break;
      case nREG5F_POWER_GOOD_OFF:         return (u8*)&munREG5F_PowerGoodOff; break;
      case nREG60_TON_DELAY:              return (u8*)&munREG60_TonDelay; break;
      case nREG61_TON_RISE:               return (u8*)&munREG61_TonRise; break;
      case nREG62_TON_MAX_FAULT_LIMIT:    return (u8*)&munREG62_TonMaxFaultLimit; break;
      case nREG63_TON_MAX_FAULT_RESPONSE: return (u8*)&munREG63_TonMaxFaultResponse; break;
      case nREG64_TOFF_DELAY:             return (u8*)&munREG64_ToffDelay; break;
      case nREG65_TOFF_FALL:              return (u8*)&munREG65_ToffFall; break;
      case nREG66_TOFF_MAX_WARN_LIMIT:    return (u8*)&munREG66_ToffMaxWarnLimit; break;
      case nREG78_STATUS_BYTE:            return (u8*)&munREG78_StatusByte; break;
      case nREG79_STATUS_WORD:            return (u8*)&munREG79_StatusWord; break;
      case nREG7A_STATUS_VOUT:            return (u8*)&munREG7A_StatusVout; break;
      case nREG7B_STATUS_IOUT:            return (u8*)&munREG7B_StatusIout; break;
      case nREG7C_STATUS_INPUT:           return (u8*)&munREG7C_StatusInput; break;
      case nREG7D_STATUS_TEMPERATURE:     return (u8*)&munREG7D_StatusTemperature; break;
      case nREG7E_STATUS_CML:             return (u8*)&munREG7E_StatusCml; break;
      case nREG80_STATUS_MFR_SPECIFIC:    return (u8*)&munREG80_StatusMfrSpecific; break;
      case nREG88_READ_VIN:               return (u8*)&munREG88_ReadVin; break;
      case nREG89_READ_IIN:               return (u8*)&munREG89_ReadIin; break;
      case nREG8B_READ_VOUT:              return (u8*)&munREG8B_ReadVout; break;
      case nREG8C_READ_IOUT:              return (u8*)&munREG8C_ReadIout; break;
      case nREG8D_READ_TEMPERATURE:       return (u8*)&munREG8D_ReadTemperature; break;
      case nREG94_READ_DUTY_CYCLE:        return (u8*)&munREG94_ReadDutyCycle; break;
      case nREG96_READ_POUT:              return (u8*)&munREG96_ReadPout; break;
      case nREG97_READ_PIN:               return (u8*)&munREG97_ReadPin; break;
      case nREG98_PMBUS_REVISION:         return (u8*)&munREG98_PmbusRevision; break;
      case nREG9E_MFR_SERIAL:             return (u8*)&munREG9E_MfrSerial; break;
      case nREGAD_IC_DEVICE_ID:           return (u8*)&munREGAD_IcDeviceId; break;
      case nREGD7_MFR_BASE_ADDRESS:       return (u8*)&munREGD7_MfrBaseAddress; break;
      case nREGDB_EEPROM_PASSWORD:        return (u8*)&munREGDB_EepromPassword; break;
      case nREGE2_MFR_BASE_ADDRESS_2:     return (u8*)&munREGE2_MfrBaseAddress2; break;
      default: return null; break;
    }
  }

  class cSEQ01_Init1 : public  cSequence
  {
    public:

    const cEntry macRegList[2] = { cEntry(nREG03_CLEAR_FAULTS, True), cEntry(nREGXX_ALL, False)};

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ02_Init2 : public  cSequence
  {
    public:

    //const cEntry macRegList[2] = { cEntry(nREG35_VIN_ON, True), cEntry(nREG36_VIN_OFF, True) };
    const cEntry macRegList[2] = { /*cEntry(nREG35_VIN_ON, True), cEntry(nREG36_VIN_OFF, True),*/ cEntry(nREG40_VOUT_OV_FAULT_LIMIT, True), cEntry(nREG21_VOUT_COMMAND, True)};

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  cSEQ01_Init1 mcSEQ01_Init1;
  cSEQ02_Init2 mcSEQ02_Init2;


  cSequence* pcGetSequence(u8 lu8SeqIdx)
  {
    switch ((tenDefaultSequences)lu8SeqIdx)
    {
      case tenDefaultSequences::nSEQ01Init1: return &mcSEQ01_Init1;
      case tenDefaultSequences::nSEQ02Init2: return &mcSEQ02_Init2;
      default: return null;
    }
  }

  bool isReg(cSequence::cEntry lcEntry)
  {
    return (lcEntry.isReg(u8GetRegCnt()));
  }
  
  bool isReadAll(cSequence::cEntry lcEntry)
  {
    return (lcEntry.isRegAll(u8GetRegCnt()) && lcEntry.isRead());
  }
  
  bool isWriteAll(cSequence::cEntry lcEntry)
  {
    return (lcEntry.isRegAll(u8GetRegCnt()) && lcEntry.isWrite());
  }
  
  bool isDelay(cSequence::cEntry lcEntry)
  {
    return (lcEntry.isDelay(u8GetRegCnt()));
  }
};


#endif
