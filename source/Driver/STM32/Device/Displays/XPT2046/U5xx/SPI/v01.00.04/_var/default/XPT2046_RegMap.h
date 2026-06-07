#pragma once

/**
 * Created by Gemini, 2026-06-01.
 * Resources: XPT2046 Datasheet / ADS7846 Datasheet.
 *
 * @class CXPT2046
 * @brief Hardware-Abstraktion für den XPT2046 Touch-Controller via SPI.
 * Bildet das 8-Bit Steuerwort (Control Byte) als Bitfeld ab.
 */

#include "TypeDef.h"
#include "ComRegMap.h"
#include "XPT2046_hw.h"

// --- Hauptklasse cSIC45X ---
struct cXPT2046_RegisterMap: public cXPT2046_HW
{
  typedef enum
  {
    nCMD90_Measure_Ya = 0,  ///< 90h: 1001.0000: 12-Bit, Differential, Y-Position
    nCMDD0_Measure_Xa,      ///< D0h: 1101.0000: 12-Bit, Differential, X-Position
    nCMDB0_Measure_Z1a,     ///< B0h: 1011.0000: 12-Bit, Differential, Z1-Druck
    nCMDC0_Measure_Z2a,     ///< C0h: 1100.0000: 12-Bit, Differential, Z2-Druck
    nCMD90_Measure_Yb,      ///< 90h: 1001.0000: 12-Bit, Differential, Y-Position
    nCMDD0_Measure_Xb,      ///< D0h: 1101.0000: 12-Bit, Differential, X-Position
    nCMDB0_Measure_Z1b,     ///< B0h: 1011.0000: 12-Bit, Differential, Z1-Druck
    nCMDC0_Measure_Z2b,     ///< C0h: 1100.0000: 12-Bit, Differential, Z2-Druck
    nCMDA4_Measure_VBAT,    ///< A4h: 1010.0100: 12-Bit, Single-Ended, VBAT (Power Down zwischen Messungen)
    nCMDE4_Measure_AUX,     ///< E4h: 1110.0100: 12-Bit, Single-Ended, AUX
    nCMD84_Measure_TEMP,    ///< 84h: 1000.0100: 12-Bit, Single-Ended, Temperatur
    nCMD98_Measure_Y_8Bit,  ///< 98h: 1001.1000: 8-Bit,  Differential, Y-Position (Schnellmodus)

    nREG_MAX_COUNT,               // Hilfswert für Array-Größen (66)

    nREGXX_ALL = nREG_MAX_COUNT,
    nDELAY     = nREG_MAX_COUNT + 1,
    nBLOCK     = nREG_MAX_COUNT + 2,
    nPTR       = nREG_MAX_COUNT + 3
  } tenRegIndex;

  u8 u8GetRegCnt()
  {
    return nREG_MAX_COUNT;
  }

  /** @brief Die Kanal-Auswahl-Bits (A2, A1, A0) des XPT2046 Steuerworts */
  enum tenChannelSelect : u8
  {
    nCHANNEL0_Temperature0 = 0, ///< 000b: Interner Temperatursensor Mode 0
    nCHANNEL1_Y_Position,       ///< 001b: Y-Achsen Messung (Treiber für X-Drivers aktiv)
    nCHANNEL2_Battery,          ///< 010b: Batterie-Spannungsmessung (VBAT)
    nCHANNEL3_Z1_Position,      ///< 011b: Touch-Druck Z1 Messung
    nCHANNEL4_Z2_Position,      ///< 100b: Touch-Druck Z2 Messung
    nCHANNEL5_X_Position,       ///< 101b: X-Achsen Messung (Treiber für Y-Drivers aktiv)
    nCHANNEL6_Auxiliary,        ///< 110b: Hilfseingang (AUXIN)
    nCHANNEL7_Temperature1      ///< 111b: Interner Temperatursensor Mode 1
  };

  /** @brief Definition der Zugriffsrechte für die Register */
  enum tenAccess : u8
  {
    nACCESS_RO = 0x01,             ///< Read-Only (Nur Lesen)
    nACCESS_RW = 0x02,             ///< Read/Write (Lesen und Schreiben)
    nACCESS_WO = 0x03
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
    // Ctrl, Len, Def,   Zugriffsart
    { 0x90,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Ya
    { 0xD0,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Xa
    { 0xB0,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Z1a
    { 0xC0,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Z2a
    { 0x90,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Yb
    { 0xD0,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Xb
    { 0xB0,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Z1b
    { 0xC0,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_Z2b
    { 0xA4,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_VBAT
    { 0xE4,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_AUX
    { 0x84,  2,   0x00,  nACCESS_RW }, // nCMD_Measure_TEMP
    { 0x98,  2,   0x00,  nACCESS_RW }  // nCMD_Measure_Y_8Bit
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

  /** * @union tunXPT2046_ControlByte
   * @brief Das exakte 8-Bit Steuerwort laut Datenblatt zur Konfiguration der anstehenden Messung.
   */
  union tunXPT2046_ControlByte
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8PD       : 2;           ///< bit 0..1: Power-Down Mode (00=Save Power, 01=Ref Off/IRQ On, 10=Ref On/IRQ Off, 11=Ref On/IRQ On)
      u8 u8SER_DFR  : 1;           ///< bit 2: 0 = Differential-Modus (empfohlen für X/Y), 1 = Single-Ended (für VBAT/AUX/TEMP)
      u8 u8MODE     : 1;           ///< bit 3: Auflösung (0 = 12-Bit Auflösung, 1 = 8-Bit Auflösung)
      u8 u8CHANNEL  : 3;           ///< bit 4..6: Kanalauswahl (tenChannelSelect, z.B. 0x01 für Y, 0x05 für X)
      u8 u8START    : 1;           ///< bit 7: Start-Bit. Muss immer '1' sein, damit das Steuerwort als gültig erkannt wird.
    } stBits;
  };

  /** * @union tunXPT2046_ResponseData
   * @brief Puffer für die vom XPT2046 zurückgegebenen ADC-Daten (In der Regel 2 Bytes für 12-Bit Wert).
   */
  union tunXPT2046_ResponseData
  {
    u8 au8Byte[2];
    struct
    {
      u8 u8Data_LSB;               ///< Byte 0: Enthält die unteren Bits (D3..D0), gefolgt von 4 Dummy-Nullen im 16-Bit SPI Takt
      u8 u8Data_MSB;               ///< Byte 1: Enthält die oberen Bits (D11..D4) der Wandlung
    } stParam;
  };

  struct tstRegInstances
  {
    tunXPT2046_ResponseData unCtrlYa;       ///< Konfiguration für Y-Messung
    tunXPT2046_ResponseData unCtrlXa;       ///< Konfiguration für X-Messung
    tunXPT2046_ResponseData unCtrlZ1a;      ///< Konfiguration für Z1-Messung
    tunXPT2046_ResponseData unCtrlZ2a;      ///< Konfiguration für Z2-Messung
    tunXPT2046_ResponseData unCtrlYb;       ///< Konfiguration für Y-Messung
    tunXPT2046_ResponseData unCtrlXb;       ///< Konfiguration für X-Messung
    tunXPT2046_ResponseData unCtrlZ1b;      ///< Konfiguration für Z1-Messung
    tunXPT2046_ResponseData unCtrlZ2b;      ///< Konfiguration für Z2-Messung
    tunXPT2046_ResponseData unCtrlBattery;  ///< Konfiguration für VBAT-Messung
    tunXPT2046_ResponseData unCtrlAux;      ///< Konfiguration für AUX-Messung
    tunXPT2046_ResponseData unCtrlTemp;     ///< Konfiguration für Temperatur-Messung
    tunXPT2046_ResponseData unCtrlY8Bit;    ///< Konfiguration für 8-Bit Y-Messung
    
    tunXPT2046_ResponseData unLastResponse; ///< Speicher für den letzten empfangenen ADC-Wert
  } stRegs;

  u8* pu8GetMem(u8 lu8Idx)
  {
    switch ((tenRegIndex)lu8Idx)
    {
      case nCMD90_Measure_Ya:       return stRegs.unCtrlYa.au8Byte;
      case nCMDD0_Measure_Xa:       return stRegs.unCtrlXa.au8Byte;
      case nCMDB0_Measure_Z1a:      return stRegs.unCtrlZ1a.au8Byte;
      case nCMDC0_Measure_Z2a:      return stRegs.unCtrlZ2a.au8Byte;
      case nCMD90_Measure_Yb:       return stRegs.unCtrlYb.au8Byte;
      case nCMDD0_Measure_Xb:       return stRegs.unCtrlXb.au8Byte;
      case nCMDB0_Measure_Z1b:      return stRegs.unCtrlZ1b.au8Byte;
      case nCMDC0_Measure_Z2b:      return stRegs.unCtrlZ2b.au8Byte;
      case nCMDA4_Measure_VBAT:     return stRegs.unCtrlBattery.au8Byte;
      case nCMDE4_Measure_AUX:      return stRegs.unCtrlAux.au8Byte;
      case nCMD84_Measure_TEMP:     return stRegs.unCtrlTemp.au8Byte;
      case nCMD98_Measure_Y_8Bit:   return stRegs.unCtrlY8Bit.au8Byte;
      default:                      return nullptr;
    }
  }

  class cSEQ01_Init1 : public cComSeq
  {
    public:
 
    //const cEntry macRegList[1] = { cRead(nREGXX_ALL) };
    const cEntry macRegList[1] = { cRead(nCMDD0_Measure_Xa) };
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

    
  class cSEQ07_ReadXYZ : public cComSeq
  {
    public:
  
    const cEntry macRegList[4] = { cRead(nCMDD0_Measure_Xa), cRead(nCMD90_Measure_Ya), cRead(nCMDD0_Measure_Xb), cRead(nCMD90_Measure_Yb) };
  
    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  cSEQ01_Init1    mcSEQ01_Init1;
  cSEQ07_ReadXYZ  mcSEQ07_ReadXYZ;

  enum tenSequences : u8
  {
    nSEQ00Reset    = cComSeq::tenDefaultSequences::nSEQ00Reset,
    nSEQ01Init1    = cComSeq::tenDefaultSequences::nSEQ01Init1,
    nSEQ02Init2    = cComSeq::tenDefaultSequences::nSEQ02Init2,
    nSEQ04Enable   = cComSeq::tenDefaultSequences::nSEQ04Enable,
    nSEQ05Disable  = cComSeq::tenDefaultSequences::nSEQ05Disable,
    nSEQ06DeInit   = cComSeq::tenDefaultSequences::nSEQ06DeInit,
    nSEQ07_ReadXYZ,
  };

  cComSeq* pcGetSequence(u8 lu8SeqIdx)
  {
    switch (lu8SeqIdx)
    {
      // If the last control byte written to the XPT2046 contains PD0 = 0, the pen - interrupt output function is enabled at the end
      // of the conversion.The end of the conversion occurs on the falling edge of DCLK after bit 1 of the converted data
      // is clocked out of the XPT2046.
      // -> Von daher kein ReadAll durchführen
      case tenSequences::nSEQ01Init1:    return &mcSEQ01_Init1;
      case tenSequences::nSEQ07_ReadXYZ: return &mcSEQ07_ReadXYZ;
      default: return null;
    }
  }

  cXPT2046_RegisterMap()
  {}
};
