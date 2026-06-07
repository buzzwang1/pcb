#pragma once

/**
 * Created by Gemini 3 Flash, 2026-05-05.
 * Resources: TI TPS55288 Datasheet (SBOS516B), DefaultArm Framework.
 *
 * @class CTPS55288
 * @brief Professionelle Hardware-Abstraktion für den TPS55288 Monitor.
 * Jedes Register ist als eigene Union mit Bitfeldern (LSB-first) abgebildet.
 */

#include "TypeDef.h"
#include "ComRegMap.h"


// --- Hauptklasse TPS55288 ---
struct cTPS55288_RegisterMap
{
  typedef enum
  {
    nREG00_VrefLsb = 0,      ///< Ausgangsspannung Referenz (LSB)
    nREG01_VrefMsb,          ///< Ausgangsspannung Referenz (MSB)
    nREG02_IoutLimit,        ///< Ausgangsstrombegrenzung
    nREG03_VoutSlew,         ///< Slew-Rate der Spannungsänderung
    nREG04_VoutFs,           ///< Full-Scale Bereichsauswahl & OE
    nREG05_Cdc,              ///< Kabel-Spannungsabfall-Kompensation (CDC)
    nREG06_Mode,             ///< Betriebsmodus & Frequenzkonfiguration
    nREG07_Status,           ///< Status- und Fehlerregister
    nREG_MAX_COUNT,          ///< Gesamtanzahl der Register

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
    u8        u8DefaultValue;     ///< Power-On-Reset Defaultwert
    tenAccess enAccessType;        ///< Zugriffsart (RO/RW)
  };

  // Lookup-Tabelle: Indexiert durch eSiC45xRegIndex
  static constexpr tenRegLookupEntry gastRegLookupTable[nREG_MAX_COUNT] =
  {
    // Adr,  Len, Def,  Zugriffsart
    { 0x00,  1,   0xD2, nACCESS_RW }, // nREG00_VrefLsb
    { 0x01,  1,   0x00, nACCESS_RW }, // nREG01_VrefMsb
    { 0x02,  1,   0xE4, nACCESS_RW }, // nREG02_IoutLimit
    { 0x03,  1,   0x01, nACCESS_RW }, // nREG03_VoutSlew
    { 0x04,  1,   0x03, nACCESS_RW }, // nREG04_VoutFs
    { 0x05,  1,   0x00, nACCESS_RW }, // nREG05_Cdc
    { 0x06,  1,   0x22, nACCESS_RW }, // nREG06_Mode
    { 0x07,  1,   0x00, nACCESS_RO }  // nREG07_Status
  };

  static constexpr u16 mu16GetRegMaxLen = 1;

  u8 u8GetRegAdr(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Address;
  }

  u16 u16GetRegLen(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Length;
  }


/** @brief Definition der Schaltfrequenz (Frequency Options) */
  enum tenFswOptions : u8
  {
    nFSW_200kHz = 0, ///< 200 kHz
    nFSW_400kHz = 1, ///< 400 kHz (Default)
    nFSW_600kHz = 2, ///< 600 kHz
    nFSW_800kHz = 3, ///< 800 kHz
    nFSW_1000kHz = 4,///< 1000 kHz
    nFSW_1200kHz = 5,///< 1200 kHz
    nFSW_1400kHz = 6,///< 1400 kHz
    nFSW_1600kHz = 7 ///< 1600 kHz
  };

  /** @brief Definition des Überstromschutzes der Spule (Overcurrent Hiccup) */
  enum tenOcpMode : u8
  {
    nOCP_LatchOff = 0, ///< Latch-Off bei Überstrom
    nOCP_Hiccup   = 1  ///< Hiccup-Modus (Wiederanlaufversuche, Default)
  };


  // --- Register-Instanzen (Unions) ---

  /** @brief 00h: VREF_LSB Register (Default: 0x00)

      Internal feedback ratio
        00b = Set internal feedback ratio to 1,129mV / 0.2256 =>   5mV/digit (45mV =>  200mV...5324mV
        01b = Set internal feedback ratio to 1,129mV / 0.1128 =>  10mV/digit (45mV =>  400mV..10429mV
        10b = Set internal feedback ratio to 1,129mV / 0.0752 =>  15mV/digit (45mV =>  600mV..15973mV
        11b = Set internal feedback ratio to 1,129mV / 0.0564 =>  20mV/digit (45mV =>  800mV..21298mV
     
      U = (45mV + x * 1,129mV) / Ratio */
  union tunVrefLsb
  {
    u8 u8Raw;
    struct
    {
      u8 VrefLsb       : 8; ///< Bits 0-7: Untere 8-Bit der DAC-Referenzspannung
    } stBit;
  } unVrefLsb;

  /** @brief 01h: VREF_MSB Register (Default: 0x00) */
  union tunVrefMsb
  {
    u8 u8Raw;
    struct
    {
      u8 VrefMsb       : 2; ///< Bits 0-1: Obere 2-Bit der DAC-Referenz (Gesamt 10-Bit)
      u8 Reserved      : 6; ///< Bits 2-7: Reserviert
    } stBit;
  } unVrefMsb;

  /** @brief 02h: IOUT_LIMIT Register (Default: 0x64) 
      10mOhm at VISP-VISN
      ILIM =   0 =>    0 mA
      ILIM =   1 =>   50 mA
      ILIM =   2 =>  100 mA
      ILIM =   3 =>  150 mA
      ILIM =   4 =>  200 mA
      ILIM =   5 =>  250 mA
      ILIM = 100 => 5000 mA (default)
      ILIM = 127 => 6350 mA */
  union tunIoutLimit
  {
    u8 u8Raw;
    struct
    {
      u8 IoutLimit     : 7; ///< Bits 0-6: Ausgangsstrombegrenzung (DAC-Wert)
      u8 IoutLimitEn   : 1; ///< Bit 7: Aktivierung der Strombegrenzung (1 = Aktiv)
    } stBit;
  } unIoutLimit;

  /** @brief 03h: VOUT_SR Register (Default: 0x01) */
  union tunVoutSlew
  {
    u8 u8Raw;
    struct
    {
      u8 Sr                  : 2; ///< Bits 0-1: Spannungsanstiegsgeschwindigkeit (SR)
      u8 Reserved1           : 2; ///< Bits 2-3: Reserviert (RESERVED)
      u8 OcpDelay            : 2; ///< Bits 4-5: Überstrom-Verzögerungszeit (OCP_DELAY)
      u8 Reserved2           : 2; ///< Bits 6-7: Reserviert (RESERVED)
    } stBit;
  } unVoutSlew;

  /** @brief 04h: VOUT_FS Register (Default: 0x03) */
  union tunVoutFs
  {
    u8 u8Raw;
    struct
    {
      u8 IntFb         : 2; ///< Bits 0-1: Internes Feedback-Teilerverhältnis (INTFB)
      u8 Reserved      : 5; ///< Bits 2-6: Reserviert (RESERVED)
      u8 Fb            : 1; ///< Bit 7: Feedback-Pfad Auswahl (0 = Intern, 1 = Extern) (FB)
    } stBit;
  } unVoutFs;

  /** @brief 05h: CDC Register (Default: 0x00) */
  union tunCdc
  {
    u8 u8Raw;
    struct
    {
      u8 Cdc           : 1; ///< Bit 0: Cable Droop Compensation Enable
      u8 CdcOption     : 3; ///< Bits 1-3: CDC Option Einstellungen
      u8 Reserved      : 1; ///< Bit 4: Reserviert
      u8 OvpMask       : 1; ///< Bit 5: Overvoltage Protection Mask
      u8 OcpMask       : 1; ///< Bit 6: Overcurrent Protection Mask
      u8 ScMask        : 1; ///< Bit 7: Short-Circuit Protection Mask
    } stBit;
  } unCdc;

  /** @brief 06h: MODE Register (Default: 0x22) */
  union tunMode
  {
    u8 u8Raw;
    struct
    {
      u8 Mode          : 1; ///< Bit 0: Mode selection bit
      u8 Pfm           : 1; ///< Bit 1: PFM mode enable bit
      u8 I2cAdd        : 1; ///< Bit 2: I2C Address configuration bit
      u8 Vcc           : 1; ///< Bit 3: VCC LDO status/control bit
      u8 Dischg        : 1; ///< Bit 4: Active discharge control bit
      u8 Hiccup        : 1; ///< Bit 5: Hiccup protection control bit
      u8 FswDbl        : 1; ///< Bit 6: Frequency Double control bit
      u8 Oe            : 1; ///< Bit 7: Output Enable bit
    } stBit;
  } unMode;

  /** @brief 07h: STATUS Register (Read-Only) */
  union tunStatus
  {
    u8 u8Raw;
    struct
    {
      u8 Status        : 2; ///< Bits 0-1: Converter status bits
      u8 Reserved      : 3; ///< Bits 2-4: Reserviert
      u8 Ovp           : 1; ///< Bit 5: Overvoltage protection flag
      u8 Ocp           : 1; ///< Bit 6: Overcurrent protection flag
      u8 Scp           : 1; ///< Bit 7: Short-circuit protection flag
    } stBit;
  } unStatus;

  u8* pu8GetMem(u8 lu8Idx)
  {
    switch ((tenRegIndex)lu8Idx)
    {
      case nREG00_VrefLsb:            return (u8*)&unVrefLsb.u8Raw;
      case nREG01_VrefMsb:            return (u8*)&unVrefMsb.u8Raw;
      case nREG02_IoutLimit:          return (u8*)&unIoutLimit.u8Raw;
      case nREG03_VoutSlew:           return (u8*)&unVoutSlew.u8Raw;
      case nREG04_VoutFs:             return (u8*)&unVoutFs.u8Raw;
      case nREG05_Cdc:                return (u8*)&unCdc.u8Raw;
      case nREG06_Mode:               return (u8*)&unMode.u8Raw;
      case nREG07_Status:             return (u8*)&unStatus.u8Raw;
      default:                        return (u8*)0;
    }
  }


  class cSEQ01_Init1 : public cComSeq
  {
    public:
    //                                               Defaultwerte, aber DISCHG = 1
    //                                                nREG06_Mode    OFHDVIPM
    const cEntry macRegList[13] = { cComSeqBlockWr(2,        0x06, 0b00110000), 
    //                                               Defaultwerte
    //                                                nREG02_IoutLimit  ECCCCCCC
                                    cComSeqBlockWr(2,        0x02,    0b11100100),
    //                                               Defaultwerte
    //                                                nREG04_VoutFs     FRRRRRII
                                    cComSeqBlockWr(2,        0x04,    0b00000011),
                                    cRead(nREGXX_ALL) };

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
    //                                              Defaultwerte, aber DISCHG = 1
    //                                               nREG06_Mode    OFHDVIPM
    const cEntry macRegList[4] = { cComSeqBlockWr(2,        0x06, 0b00110000) };

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ07_SetOutput : public cComSeq
  {
    public:

    // Zuerst Strombegrenzung, dann Spannung
    // Zuerst Reg0 dann erst Reg1 schreiben
    // Writing register 01h enables the TPS55288 to load the 01h and 00h data into the internal 10-bit DAC. Writing the
    // register 00h does not impact the internal reference voltage.
    const cEntry macRegList[3] = { cWrite(nREG02_IoutLimit), cWrite(nREG00_VrefLsb), cWrite(nREG01_VrefMsb)};

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ08_WriteMode : public cComSeq
  {
    public:

    const cEntry macRegList[1] = { cWrite(nREG06_Mode)};

    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ09_ReadStatus : public cComSeq
  {
    public:

    const cEntry macRegList[1] = { cRead(nREG07_Status)};

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
  cSEQ07_SetOutput  mcSEQ07_SetOutput;
  cSEQ08_WriteMode  mcSEQ08_WriteMode;
  cSEQ09_ReadStatus mcSEQ09_ReadStatus;


  enum tenSequences : u8
  {
    nSEQ00Reset    = cComSeq::tenDefaultSequences::nSEQ00Reset,  
    nSEQ01Init1    = cComSeq::tenDefaultSequences::nSEQ01Init1,
    nSEQ02Init2    = cComSeq::tenDefaultSequences::nSEQ02Init2,
    nSEQ04Enable   = cComSeq::tenDefaultSequences::nSEQ04Enable,
    nSEQ05Disable  = cComSeq::tenDefaultSequences::nSEQ05Disable,
    nSEQ06DeInit   = cComSeq::tenDefaultSequences::nSEQ06DeInit,
    nSEQ07SetOutput,
    nSEQ08WriteMode,
    nSEQ09ReadStatus,
  };


  cComSeq* pcGetSequence(u8 lu8SeqIdx)
  {
    switch (lu8SeqIdx)
    {
      case tenSequences::nSEQ01Init1:      return &mcSEQ01_Init1;
      case tenSequences::nSEQ05Disable:    return &mcSEQ05_Disable;
      case tenSequences::nSEQ07SetOutput:  return &mcSEQ07_SetOutput;
      case tenSequences::nSEQ08WriteMode:  return &mcSEQ08_WriteMode;
      case tenSequences::nSEQ09ReadStatus: return &mcSEQ08_WriteMode;
      default: return null;
    }
  }

  cTPS55288_RegisterMap()
  {}
};
