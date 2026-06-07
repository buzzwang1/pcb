#pragma once

/**
 * Created by Gemini 3 Flash, 2026-05-05.
 * Resources: TI INA3221 Datasheet (SBOS516B), DefaultArm Framework.
 *
 * @class CINA3221
 * @brief Professionelle Hardware-Abstraktion für den INA3221 Monitor.
 * Jedes Register ist als eigene Union mit Bitfeldern (LSB-first) abgebildet.
 */

#include "TypeDef.h"
#include "ComRegMap.h"


// --- Hauptklasse cSIC45X ---
struct cIna3221_RegisterMap
{
  typedef enum
  {
    nREG00_Config = 0,             //  0: Konfigurationsregister
    nREG01_Ch1ShuntVolt,           //  1: Kanal 1 Shunt-Spannung
    nREG02_Ch1BusVolt,             //  2: Kanal 1 Bus-Spannung
    nREG03_Ch2ShuntVolt,           //  3: Kanal 2 Shunt-Spannung
    nREG04_Ch2BusVolt,             //  4: Kanal 2 Bus-Spannung
    nREG05_Ch3ShuntVolt,           //  5: Kanal 3 Shunt-Spannung
    nREG06_Ch3BusVolt,             //  6: Kanal 3 Bus-Spannung
    nREG07_CritAlert1,             //  7: Kritischer Grenzwert Kanal 1
    nREG08_WarnAlert1,             //  8: Warn-Grenzwert Kanal 1
    nREG09_CritAlert2,             //  9: Kritischer Grenzwert Kanal 2
    nREG0A_WarnAlert2,             // 10: Warn-Grenzwert Kanal 2
    nREG0B_CritAlert3,             // 11: Kritischer Grenzwert Kanal 3
    nREG0C_WarnAlert3,             // 12: Warn-Grenzwert Kanal 3
    nREG0D_ShuntVoltSum,           // 13: Summe der Shunt-Spannungen
    nREG0E_ShuntVoltSumLimit,      // 14: Grenzwert für Shunt-Spannungssumme
    nREG0F_MaskEnable,             // 15: Maskierungs- und Aktivierungsregister
    nREG10_PwrValidUpperLimit,     // 16: Obere Grenze Power-Valid
    nREG11_PwrValidLowerLimit,     // 17: Untere Grenze Power-Valid
    nREGFE_ManufID,                // 18: Hersteller-Identifikation
    nREGFF_DieID,                  // 19: Chip-Identifikation

    nREG_MAX_COUNT,               // Hilfswert für Array-Größen (66)

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
    { 0x00,  2,   0x7127, nACCESS_RW }, // nREG00_Configuration
    { 0x01,  2,   0x0000, nACCESS_RO }, // nREG01_Ch1ShuntVolt
    { 0x02,  2,   0x0000, nACCESS_RO }, // nREG02_Ch1BusVolt
    { 0x03,  2,   0x0000, nACCESS_RO }, // nREG03_Ch2ShuntVolt
    { 0x04,  2,   0x0000, nACCESS_RO }, // nREG04_Ch2BusVolt
    { 0x05,  2,   0x0000, nACCESS_RO }, // nREG05_Ch3ShuntVolt
    { 0x06,  2,   0x0000, nACCESS_RO }, // nREG06_Ch3BusVolt
    { 0x07,  2,   0x7FFF, nACCESS_RW }, // nREG07_Ch1CritAlert (Default positive Full-Scale)
    { 0x08,  2,   0x7FFF, nACCESS_RW }, // nREG08_Ch1WarnAlert
    { 0x09,  2,   0x7FFF, nACCESS_RW }, // nREG09_Ch2CritAlert
    { 0x0A,  2,   0x7FFF, nACCESS_RW }, // nREG0A_Ch2WarnAlert
    { 0x0B,  2,   0x7FFF, nACCESS_RW }, // nREG0B_Ch3CritAlert
    { 0x0C,  2,   0x7FFF, nACCESS_RW }, // nREG0C_Ch3WarnAlert
    { 0x0D,  2,   0x0000, nACCESS_RO }, // nREG0D_ShuntVoltSum
    { 0x0E,  2,   0x7FFF, nACCESS_RW }, // nREG0E_ShuntVoltSumL
    { 0x0F,  2,   0x0002, nACCESS_RW }, // nREG0F_MaskEnable
    { 0x10,  2,   0x2710, nACCESS_RW }, // nREG10_PvUpperLimit (Default: 10.00V linksbuendig)
    { 0x11,  2,   0x2328, nACCESS_RW }, // nREG11_PvLowerLimit (Default: 9.00V linksbuendig)
    { 0xFE,  2,   0x5449, nACCESS_RO }, // nREGFE_ManufacturerID ("TI")
    { 0xFF,  2,   0x3220, nACCESS_RO }  // nREGFF_DieID
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


/** @brief Definition der Mittelwertbildung (Averaging Mode)[cite: 2] */
  enum tenAvgMode : u16
  {
    nAVG_1    = 0, ///< 1 Sample (Default)[cite: 2]
    nAVG_4    = 1, ///< 4 Samples[cite: 2]
    nAVG_16   = 2, ///< 16 Samples[cite: 2]
    nAVG_64   = 3, ///< 64 Samples[cite: 2]
    nAVG_128  = 4, ///< 128 Samples[cite: 2]
    nAVG_256  = 5, ///< 256 Samples[cite: 2]
    nAVG_512  = 6, ///< 512 Samples[cite: 2]
    nAVG_1024 = 7  ///< 1024 Samples[cite: 2]
  };

  /** @brief Definition der Konvertierungszeit (Bus & Shunt)[cite: 2] */
  enum tenConvTime : u16
  {
    nCONV_140us  = 0, ///< 140 µs[cite: 2]
    nCONV_204us  = 1, ///< 204 µs[cite: 2]
    nCONV_332us  = 2, ///< 332 µs[cite: 2]
    nCONV_588us  = 3, ///< 588 µs[cite: 2]
    nCONV_1100us = 4, ///< 1.1 ms (Default)[cite: 2]
    nCONV_2116us = 5, ///< 2.116 ms[cite: 2]
    nCONV_4156us = 6, ///< 4.156 ms[cite: 2]
    nCONV_8244us = 7  ///< 8.244 ms[cite: 2]
  };

  /** @brief Betriebsmodi des INA3221[cite: 2] */
  enum tenOperatingMode : u16
  {
    nMODE_PowerDown          = 0, ///< Power-Down[cite: 2]
    nMODE_Shunt_Triggered    = 1, ///< Shunt-Spannung, getriggert[cite: 2]
    nMODE_Bus_Triggered      = 2, ///< Bus-Spannung, getriggert[cite: 2]
    nMODE_ShuntBus_Triggered = 3, ///< Shunt & Bus, getriggert[cite: 2]
    nMODE_PowerDown2         = 4, ///< Power-Down (identisch zu 0)[cite: 2]
    nMODE_Shunt_Cont         = 5, ///< Shunt-Spannung, kontinuierlich[cite: 2]
    nMODE_Bus_Cont           = 6, ///< Bus-Spannung, kontinuierlich[cite: 2]
    nMODE_ShuntBus_Cont      = 7  ///< Shunt & Bus, kontinuierlich (Default)[cite: 2]
  };


  // --- Register-Instanzen (Unions) ---

  /** @brief 00h: Configuration Register (Default: 0x7127) */
  union tunConfig
  {
      u16 u16Raw;
      struct
      {
          u16 Mode            : 3; ///< Betriebsmodus (z.B. Shunt/Bus kontinuierlich)
          u16 ShuntConvTime   : 3; ///< Konvertierungszeit Shunt-Spannung
          u16 BusConvTime     : 3; ///< Konvertierungszeit Bus-Spannung
          u16 AveragingMode   : 3; ///< Anzahl der Mittelwertbildungen
          u16 Ch3Enable       : 1; ///< Aktiviert Kanal 3
          u16 Ch2Enable       : 1; ///< Aktiviert Kanal 2
          u16 Ch1Enable       : 1; ///< Aktiviert Kanal 1
          u16 Reset           : 1; ///< System-Reset (Bit 15)
      } stBit;
  } unConfig;

  /** @brief 01h-0Ch, 10h-11h: Spannungsregister (13-Bit linksbündig) */
  union tunVoltage
  {
      u16 u16Raw;
      struct
      {
        u16 Hb : 8;
        u16 Lb : 8;
      } stByte;
  } unCh1Shunt, unCh1Bus, unCh2Shunt, unCh2Bus, unCh3Shunt, unCh3Bus,
    unCrit1, unWarn1, unCrit2, unWarn2, unCrit3, unWarn3,
    unPwrValidUpper, unPwrValidLower;

  /** @brief 0Dh-0Eh: Summenregister (15-Bit linksbündig) */
  union tunShuntSum
  {
      u16 u16Raw;
      struct
      {
          u16 Reserved : 1; ///< Bit 0: Reserviert
          u16 Value    : 15;///< Bits 1-15: Summenwert (Zweierkomplement)
      } stBit;
  } unShuntSum, unShuntSumLimit;

  /** @brief 0Fh: Mask/Enable Register (Status und Alert-Steuerung) */
  union tunMaskEnable
  {
      u16 u16Raw;
      struct {
          u16 CVRF            : 1; ///< Conversion Ready Flag
          u16 TCC             : 1; ///< Timing Control Control
          u16 PVF             : 1; ///< Power-Valid Alert Flag
          u16 SF              : 1; ///< Shunt Voltage Sum Alert Flag
          u16 WF3             : 1; ///< Warning Flag Kanal 3
          u16 WF2             : 1; ///< Warning Flag Kanal 2
          u16 WF1             : 1; ///< Warning Flag Kanal 1
          u16 CF3             : 1; ///< Critical Flag Kanal 3
          u16 CF2             : 1; ///< Critical Flag Kanal 2
          u16 CF1             : 1; ///< Critical Flag Kanal 1
          u16 CEN             : 1; ///< Critical Alert Latch Enable
          u16 WEN             : 1; ///< Warning Alert Latch Enable
          u16 SCC3            : 1; ///< Kanal 3 in Summation einbeziehen
          u16 SCC2            : 1; ///< Kanal 2 in Summation einbeziehen
          u16 SCC1            : 1; ///< Kanal 1 in Summation einbeziehen
          u16 Reserved        : 1; ///< Bit 15: Reserviert
      } stBit;
  } unMaskEnable;

  /** @brief FEh-FFh: ID Register */
  union tunID
  {
      u16 u16Raw;
  } unManufID, unDieID;

  u8* pu8GetMem(u8 lu8Idx)
  {
    switch ((tenRegIndex)lu8Idx)
    {
      case nREG00_Config:             return (u8*)&unConfig.u16Raw;
      case nREG01_Ch1ShuntVolt:       return (u8*)&unCh1Shunt.u16Raw;
      case nREG02_Ch1BusVolt:         return (u8*)&unCh1Bus.u16Raw;
      case nREG03_Ch2ShuntVolt:       return (u8*)&unCh2Shunt.u16Raw;
      case nREG04_Ch2BusVolt:         return (u8*)&unCh2Bus.u16Raw;
      case nREG05_Ch3ShuntVolt:       return (u8*)&unCh3Shunt.u16Raw;
      case nREG06_Ch3BusVolt:         return (u8*)&unCh3Bus.u16Raw;
      case nREG07_CritAlert1:         return (u8*)&unCrit1.u16Raw;
      case nREG08_WarnAlert1:         return (u8*)&unWarn1.u16Raw;
      case nREG09_CritAlert2:         return (u8*)&unCrit2.u16Raw;
      case nREG0A_WarnAlert2:         return (u8*)&unWarn2.u16Raw;
      case nREG0B_CritAlert3:         return (u8*)&unCrit3.u16Raw;
      case nREG0C_WarnAlert3:         return (u8*)&unWarn3.u16Raw;
      case nREG0D_ShuntVoltSum:       return (u8*)&unShuntSum.u16Raw;
      case nREG0E_ShuntVoltSumLimit:  return (u8*)&unShuntSumLimit.u16Raw;
      case nREG0F_MaskEnable:         return (u8*)&unMaskEnable.u16Raw;
      case nREG10_PwrValidUpperLimit: return (u8*)&unPwrValidUpper.u16Raw;
      case nREG11_PwrValidLowerLimit: return (u8*)&unPwrValidLower.u16Raw;
      case nREGFE_ManufID:            return (u8*)&unManufID.u16Raw;
      case nREGFF_DieID:              return (u8*)&unDieID.u16Raw;
      default:                        return (u8*)0;
    }
  }

  class cSEQ00_Reset : public cComSeq
  {
    public:
    //                                              Reset, aber Mode = 0 (Shut Down)
    //                                              nREG00_Config    R
    const cEntry macRegList[5] = { cComSeqBlockWr(3,         0x00, 0b10000000, 0b00000000) };

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ01_Init1 : public cComSeq
  {
    public:

    const cEntry macRegList[12] = { cRead(nREG00_Config),     cRead(nREGFE_ManufID), cRead(nREGFF_DieID),
                                    cRead(nREG07_CritAlert1), cRead(nREG08_WarnAlert1),
                                    cRead(nREG09_CritAlert2), cRead(nREG0A_WarnAlert2),
                                    cRead(nREG0B_CritAlert3), cRead(nREG0C_WarnAlert3),
                                    cRead(nREG0F_MaskEnable), cRead(nREG10_PwrValidUpperLimit), cRead(nREG11_PwrValidLowerLimit)
    };

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ02_Init2 : public  cComSeq
  {
    public:
    cIna3221_RegisterMap* mpcRegMap;
    tunConfig munConfig;

    const cEntry macRegList[1] = { cWrite(nREG00_Config) };

    cSEQ02_Init2(cIna3221_RegisterMap* lpcRegMap)
    {
      mpcRegMap = lpcRegMap;
      munConfig.u16Raw = (0b01110001 << 8) + (0b00100111);
    }

    void vSetCfg(tunConfig &lenConfig)
    {
      munConfig.u16Raw = lenConfig.u16Raw;
    }

    cEntry* pacGet() override
    {
      // High und Lowbyte tauschen
      u8 lu8Hb = munConfig.u16Raw >> 8;
      u8 lu8Lb = (u8)munConfig.u16Raw;
      mpcRegMap->unConfig.u16Raw = (lu8Lb << 8) + lu8Hb;
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
    //                                              Defaultwerte, aber Mode = 0 (Shut Down)
    //                                              nREG00_Config    RCCCAAAV    VVVVVMMM
    const cEntry macRegList[5] = { cComSeqBlockWr(3,         0x00, 0b01110001, 0b00100000) };

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }

  };

  class cSEQ07_WriteCfg : public cComSeq
  {
    public:

    const cEntry macRegList[1] = { cWrite(nREG00_Config)};

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };


  class cSEQ08_ReadAllCh : public cComSeq
  {
    public:

    const cEntry macRegList[6] = { cRead(nREG01_Ch1ShuntVolt), cRead(nREG02_Ch1BusVolt),
                                   cRead(nREG03_Ch2ShuntVolt), cRead(nREG04_Ch2BusVolt),
                                   cRead(nREG05_Ch3ShuntVolt), cRead(nREG06_Ch3BusVolt) };

    cEntry* pacGet()
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen()
    {
      return sizeof(macRegList);
    }
  };

  cSEQ00_Reset      mcSEQ00_Reset;
  cSEQ01_Init1      mcSEQ01_Init1;
  cSEQ02_Init2      mcSEQ02_Init2;
  cSEQ05_Disable    mcSEQ05_Disable;
  cSEQ07_WriteCfg   mcSEQ07_WriteCfg;
  cSEQ08_ReadAllCh  mcSEQ08_ReadAllCh;


  enum tenSequences : u8
  {
    nSEQ00Reset    = cComSeq::tenDefaultSequences::nSEQ00Reset,
    nSEQ01Init1    = cComSeq::tenDefaultSequences::nSEQ01Init1,
    nSEQ02Init2    = cComSeq::tenDefaultSequences::nSEQ02Init2,
    nSEQ04Enable   = cComSeq::tenDefaultSequences::nSEQ04Enable,
    nSEQ05Disable  = cComSeq::tenDefaultSequences::nSEQ05Disable,
    nSEQ06DeInit   = cComSeq::tenDefaultSequences::nSEQ06DeInit,
    nSEQ07WriteCfg,
    nSEQ08ReadAllCh,
  };


  cComSeq* pcGetSequence(u8 lu8SeqIdx)
  {
    switch (lu8SeqIdx)
    {
      case tenSequences::nSEQ00Reset:     return &mcSEQ00_Reset;
      case tenSequences::nSEQ01Init1:     return &mcSEQ01_Init1;
      case tenSequences::nSEQ02Init2:     return &mcSEQ02_Init2;
      case tenSequences::nSEQ05Disable:   return &mcSEQ05_Disable;
      case tenSequences::nSEQ07WriteCfg:  return &mcSEQ07_WriteCfg;
      case tenSequences::nSEQ08ReadAllCh: return &mcSEQ08_ReadAllCh;
      default: return null;
    }
  }

  cIna3221_RegisterMap()
    : mcSEQ02_Init2(this)
  {}
};
