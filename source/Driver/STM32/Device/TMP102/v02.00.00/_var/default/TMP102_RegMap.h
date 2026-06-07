#pragma once

/**
 * Created by Gemini 3 Flash, 2026-05-05.
 * Resources: TI TMP102 Datasheet (SBOS516B), DefaultArm Framework.
 *
 * @class CTMP102
 * @brief Professionelle Hardware-Abstraktion für den TMP102 Monitor.
 * Jedes Register ist als eigene Union mit Bitfeldern (LSB-first) abgebildet.
 */

#include "TypeDef.h"
#include "ComRegMap.h"


// --- Hauptklasse TMP102 ---
struct cTMP102_RegisterMap
{
  typedef enum
  {
    nREG00_Temperature   = 0,  ///< 00h: Temperature Register (Read-Only)
    nREG01_Config,      ///< 01h: Configuration Register (Read/Write)
    nREG02_TLow,               ///< 02h: T_LOW Alert Threshold Register (Read/Write)
    nREG03_THigh,              ///< 03h: T_HIGH Alert Threshold Register (Read/Write)
    nREG_MAX_COUNT,

    nREGXX_ALL = nREG_MAX_COUNT,
    nDELAY     = nREG_MAX_COUNT + 1,
    nBLOCK     = nREG_MAX_COUNT + 2
  } tenRegIndex;

  u8 u8GetRegCnt()
  {
    return nREG_MAX_COUNT;
  }

  /** @brief Definition der Zugriffsrechte für die Register */
  enum tenAccess : u8
  {
    nACCESS_RO = 0x01,             ///< Read-Only (Nur Lesen)
    nACCESS_RW = 0x02              ///< Read/Write (Lesen und Schreiben)
  };

  /** @brief Struktur für die compile-time fähige Lookup-Tabelle */
  struct tenRegLookupEntry
  {
    u8        u8Address;          ///< I2C Register-Adresse
    u8        u8Length;           ///< Länge in Bytes
    u16       u8DefaultValue;     ///< Power-On-Reset Defaultwert
    tenAccess enAccessType;       ///< Zugriffsart (RO/RW)
  };

  // Lookup-Tabelle: Indexiert durch eSiC45xRegIndex
  static constexpr tenRegLookupEntry gastRegLookupTable[nREG_MAX_COUNT] =
  {
    // Adr,  Len, Def,    Zugriffsart
    { 0x00,  2,   0x0000, nACCESS_RO }, // nREG00_Temperature (wird zyklisch aktualisiert)
    { 0x01,  2,   0x60A0, nACCESS_RW }, // nREG01_Configuration (Standardbetrieb)
    { 0x02,  2,   0x4B00, nACCESS_RW }, // nREG02_TLow (Default 75°C)
    { 0x03,  2,   0x5000, nACCESS_RW }  // nREG03_THigh (Default 80°C)
  };

  static constexpr u16 mu16GetRegMaxLen = 2;

  u8 u8GetRegAdr(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Address;
  }

  u16 u16GetRegLen(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Length;
  }

// --- Register-Instanzen (Unions) ---

  /** @brief 00h: Temperature Register (12-Bit oder 13-Bit Linksbuendig) */
  union tunTemperature
  {
    u8 u8Raw[2];                   ///< [0] = MSB-Byte, [1] = LSB-Byte
    struct
    {
      // Byte 1 (Zuletzt gesendet / LSB-Byte)
      u8 Reserved      : 3;        ///< Byte 1, Bits 0-2: Unbenutzt
      u8 ExtModeFlag   : 1;        ///< Byte 1, Bit 3: Zeigt 13-Bit Modus an (0 im Normalmodus)
      u8 TempLsb       : 4;        ///< Byte 1, Bits 4-7: Untere 4 Bits der Temperatur
      
      // Byte 0 (Zuerst gesendet / MSB-Byte)
      u8 TempMsb       : 8;        ///< Byte 0, Bits 0-7: Obere 8 Bits der Temperatur (Vorzeichen + Integer)
    } stBit;
  } unTemperature;

  /** @brief 01h: Configuration Register (16-Bit Kontrollregister) */
  union tunConfiguration
  {
    u8 u8Raw[2];                   ///< [0] = MSB-Byte, [1] = LSB-Byte
    struct
    {
      // Byte 1 (Zuletzt gesendet / LSB-Byte des Datenblatts)
      u8 Reserved      : 4;        ///< Byte 1, Bits 0-3: Reserviert
      u8 Em            : 1;        ///< Byte 1, Bit 4: Extended Mode (0 = 12-Bit, 1 = 13-Bit)
      u8 Al            : 1;        ///< Byte 1, Bit 5: Alert Status Flag (Read-Only)
      u8 Cr            : 2;        ///< Byte 1, Bits 6-7: Conversion Rate (00=0.25Hz, 01=1Hz, 10=4Hz, 11=8Hz)

      // Byte 0 (Zuerst gesendet / MSB-Byte des Datenblatts)
      u8 Sd            : 1;        ///< Byte 0, Bit 0: Shutdown Mode (0 = Continuous, 1 = Low-Power Shutdown)
      u8 Tm            : 1;        ///< Byte 0, Bit 1: Thermostat Mode (0 = Comparator, 1 = Interrupt)
      u8 Pol           : 1;        ///< Byte 0, Bit 2: ALERT Pin Polarity (0 = Active Low, 1 = Active High)
      u8 F             : 2;        ///< Byte 0, Bits 3-4: Fault Queue Control (00=1 Fault, 01=2, 10=4, 11=6)
      u8 R             : 2;        ///< Byte 0, Bits 5-6: Converter Resolution (Default 11, Read-Only)
      u8 Os            : 1;        ///< Byte 0, Bit 7: One-Shot / Conversion Ready
    } stBit;
  } unConfiguration;

  /** @brief 02h: T_LOW Register (Untere Temperaturschwelle für ALERT) */
  union tunTLow
  {
    u8 u8Raw[2];                   ///< [0] = MSB-Byte, [1] = LSB-Byte
    struct
    {
      // Byte 1 (LSB-Byte)
      u8 Reserved      : 4;        ///< Byte 1, Bits 0-3: Reserviert
      u8 TempLsb       : 4;        ///< Byte 1, Bits 4-7: Untere Bits des Grenzwerts
      
      // Byte 0 (MSB-Byte)
      u8 TempMsb       : 8;        ///< Byte 0, Bits 0-7: Obere Bits des Grenzwerts
    } stBit;
  } unTLow;

  /** @brief 03h: T_HIGH Register (Obere Temperaturschwelle für ALERT) */
  union tunTHigh
  {
    u8 u8Raw[2];                   ///< [0] = MSB-Byte, [1] = LSB-Byte
    struct
    {
      // Byte 1 (LSB-Byte)
      u8 Reserved      : 4;        ///< Byte 1, Bits 0-3: Reserviert
      u8 TempLsb       : 4;        ///< Byte 1, Bits 4-7: Untere Bits des Grenzwerts
      
      // Byte 0 (MSB-Byte)
      u8 TempMsb       : 8;        ///< Byte 0, Bits 0-7: Obere Bits des Grenzwerts
    } stBit;
  } unTHigh;

  u8* pu8GetMem(u8 lu8Idx)
  {
    switch ((tenRegIndex)lu8Idx)
    {
      case nREG00_Temperature: return (u8*)&unTemperature.u8Raw[0];
      case nREG01_Config:      return (u8*)&unConfiguration.u8Raw[0];
      case nREG02_TLow:        return (u8*)&unTLow.u8Raw[0];
      case nREG03_THigh:       return (u8*)&unTHigh.u8Raw[0];
      default:                 return (u8*)0;
    }
  }


  class cSEQ01_Init1 : public cComSeq
  {
    public:

    //                                              Defaultwerte
    //                                              nREG01_Config    ORRFFPTS    CCAE0000
    const cEntry macRegList[6] = { cComSeqBlockWr(3,         0x01, 0b01100000, 0b01000000), cRead(nREGXX_ALL) };

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ05_Disable : public  cComSeq
  {
    public:
    //                                              Defaultwerte, aber Shut Down gesetzt
    //                                              nREG01_Config    ORRFFPTS    CCAE0000
    const cEntry macRegList[5] = { cComSeqBlockWr(3,         0x01, 0b01100001, 0b01000000) };

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ07_ReadTemp : public cComSeq
  {
    public:

    const cEntry macRegList[1] = { cRead(nREG00_Temperature)};

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };


  cSEQ01_Init1      mcSEQ01_Init1;
  cSEQ05_Disable    mcSEQ05_Disable;
  cSEQ07_ReadTemp   mcSEQ07_ReadTemp;

  enum tenSequences : u8
  {
    nSEQ00Reset    = cComSeq::tenDefaultSequences::nSEQ00Reset,  
    nSEQ01Init1    = cComSeq::tenDefaultSequences::nSEQ01Init1,
    nSEQ02Init2    = cComSeq::tenDefaultSequences::nSEQ02Init2,
    nSEQ04Enable   = cComSeq::tenDefaultSequences::nSEQ04Enable,
    nSEQ05Disable  = cComSeq::tenDefaultSequences::nSEQ05Disable,
    nSEQ06DeInit   = cComSeq::tenDefaultSequences::nSEQ06DeInit,
    nSEQ07_ReadTemp,
  };


  cComSeq* pcGetSequence(u8 lu8SeqIdx)
  {
    switch (lu8SeqIdx)
    {
      case tenSequences::nSEQ01Init1:     return &mcSEQ01_Init1;
      case tenSequences::nSEQ05Disable:   return &mcSEQ05_Disable;
      case tenSequences::nSEQ07_ReadTemp: return &mcSEQ07_ReadTemp;
      default: return null;
    }
  }

  cTMP102_RegisterMap()
  {}
};
