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
#include "NRF905_hw.h"
#include "cNRF905_Defs.h"

// --- Hauptklasse cSIC45X ---
struct cNRF905_RegisterMap: public cNRF905_HW, public cNRF905_Defs
{
  typedef enum
  {
    nCMD00_WriteConfig = 0, ///< 0000_0000b: Schreibe Konfigurations-Register (0 bis 9)
    nCMD10_ReadConfig,      ///< 0001_0000b: Lese Konfigurations-Register (0 bis 9)
    nCMD20_WriteTXPayload,  ///< 0010_0000b: Schreibe TX-Payload
    nCMD21_ReadTXPayload,   ///< 0010_0001b: Lese TX-Payload
    nCMD22_WriteTXAddress,  ///< 0010_0010b: Schreibe TX-Zieladresse
    nCMD23_ReadTXAddress,   ///< 0010_0011b: Lese TX-Zieladresse
    nCMD24_ReadRXPayload,   ///< 0010_0100b: Lese empfangene RX-Payload
    nCMD80_ChannelConfig,   ///< 1000_xxxxb: Schnelle Frequenz-Kanal-Konfiguration

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
      // Cmd,  Len, Def,    Zugriffsart
    { 0x00, 10,   0x6C,   nACCESS_RW }, ///< 0000_0000b: Schreibe Konfigurations-Register (0 bis 9)
    { 0x10, 10,   0x4C,   nACCESS_RW }, ///< 0001_0000b: Lese Konfigurations-Register (0 bis 9)
    { 0x20, 32,   0x44,   nACCESS_RW }, ///< 0010_0000b: Schreibe TX-Payload
    { 0x21, 32,   0x20,   nACCESS_RW }, ///< 0010_0001b: Lese TX-Payload
    { 0x22,  4,   0x20,   nACCESS_RW }, ///< 0010_0010b: Schreibe TX-Zieladresse
    { 0x23,  4,   0xE7,   nACCESS_RW }, ///< 0010_0011b: Lese TX-Zieladresse
    { 0x24, 32,   0x58,   nACCESS_RW }, ///< 0010_0100b: Lese empfangene RX-Payload
    { 0x80,  1,   0x58,   nACCESS_RW }  ///< 1000_xxxxb: Schnelle Frequenz-Kanal-Konfiguration
  };

  static constexpr u16 mu16GetRegMaxLen = 4;

  u8 u8GetRegAdr(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Address;
  }

  u16 u16GetRegLen(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Length;
  }

  // =========================================================================
  // INSTANZEN-STRUKTUR (MEMBERS)
  // =========================================================================

  struct tstRegInstances
  {
    // Die 10 Bytes der Configuration Register Bank (Byte 0 bis 9)
    tunReg00_Config         unReg00_Reg10_Cfg;
    tunDataBuffer           unReg20_Reg21_TxPayload;
    tunAddressBuffer        unReg22_Reg23_TxAdr;
    tunDataBuffer           unReg24_RxPayload;
    tunReg80_ChCfg          unReg80_ChCfg;
  } stRegs;

  u8* pu8GetMem(u8 lu8Idx)
  {
    switch ((tenRegIndex)lu8Idx)
    {
      case nCMD00_WriteConfig:     return stRegs.unReg00_Reg10_Cfg.au8Byte; // Zeiger auf den Anfang der gesamten Bank (Byte 0)
      case nCMD10_ReadConfig:      return stRegs.unReg00_Reg10_Cfg.au8Byte;
      case nCMD20_WriteTXPayload:  return stRegs.unReg20_Reg21_TxPayload.au8Byte;
      case nCMD21_ReadTXPayload:   return stRegs.unReg20_Reg21_TxPayload.au8Byte;
      case nCMD22_WriteTXAddress:  return stRegs.unReg22_Reg23_TxAdr.au8Byte;
      case nCMD23_ReadTXAddress:   return stRegs.unReg22_Reg23_TxAdr.au8Byte;
      case nCMD24_ReadRXPayload:   return stRegs.unReg24_RxPayload.au8Byte;
      default:                     return nullptr;
    }
  }

  class cSEQ01_Init1 : public cComSeq
  {
    public:

    const cEntry macRegList[3] = { cWrite(nCMD22_WriteTXAddress), cRead(nCMD23_ReadTXAddress), cRead(nCMD10_ReadConfig) };
  
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
    const cEntry macRegList[1] = { cWrite(nCMD00_WriteConfig) };

    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ08_WritePayload : public cComSeq
  {
    public:
    const cEntry macRegList[1] = { cWrite(nCMD20_WriteTXPayload) };

    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ09_ReadPayload : public cComSeq
  {
    public:
    const cEntry macRegList[1] = { cWrite(nCMD24_ReadRXPayload) };

    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ10_WriteTxAdress : public cComSeq
  {
    public:
    const cEntry macRegList[1] = { cWrite(nCMD22_WriteTXAddress) };

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

  cSEQ07_WriteCfg      mcSEQ07_WriteCfg;
  cSEQ08_WritePayload  mcSEQ08_WritePayload;
  cSEQ09_ReadPayload   mcSEQ09_ReadPayload;
  cSEQ10_WriteTxAdress mcSEQ10_WriteTxAdress;

  enum tenSequences : u8
  {
    nSEQ00Reset    = cComSeq::tenDefaultSequences::nSEQ00Reset,
    nSEQ01Init1    = cComSeq::tenDefaultSequences::nSEQ01Init1,
    nSEQ02Init2    = cComSeq::tenDefaultSequences::nSEQ02Init2,
    nSEQ04Enable   = cComSeq::tenDefaultSequences::nSEQ04Enable,
    nSEQ05Disable  = cComSeq::tenDefaultSequences::nSEQ05Disable,
    nSEQ06DeInit   = cComSeq::tenDefaultSequences::nSEQ06DeInit,

    nSEQ07_WriteCfg,
    nSEQ08_WritePayload,
    nSEQ09_ReadPayload,
    nSEQ10_WriteTxAdress,
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

      case tenSequences::nSEQ07_WriteCfg:      return &mcSEQ07_WriteCfg;
      case tenSequences::nSEQ08_WritePayload:  return &mcSEQ08_WritePayload;
      case tenSequences::nSEQ09_ReadPayload:   return &mcSEQ09_ReadPayload;
      case tenSequences::nSEQ10_WriteTxAdress: return &mcSEQ10_WriteTxAdress;
      default: return null;
    }
  }

  cNRF905_RegisterMap()
    : cNRF905_HW()
  {}

  u16 u16Start_Set_Mode(tenMode &lenMode, tenMode lenNewMode)
  {
    u16 lu16Ret_us = 0;

    if (lenMode != lenNewMode)
    {
      switch (lenNewMode)
      {
        case tenMode::nRADIO_MODE_PWR_DOWN:
          cNRF905_HW::vPwrOff();
          cNRF905_HW::vEnOff();
          cNRF905_HW::vCEOff();
          break;
        case tenMode::nRADIO_MODE_IDLE:
          cNRF905_HW::vPwrOn();
          cNRF905_HW::vCEOff();
          cNRF905_HW::vEnOff();
          if (lenMode == tenMode::nRADIO_MODE_PWR_DOWN) lu16Ret_us = 3000;
          break;
        case tenMode::nRADIO_MODE_RX:
          cNRF905_HW::vPwrOn();
          cNRF905_HW::vEnOff();
          cNRF905_HW::vCEOn();
          if (lenMode == tenMode::nRADIO_MODE_PWR_DOWN) lu16Ret_us = 3000 + 650;
          if (lenMode == tenMode::nRADIO_MODE_IDLE)     lu16Ret_us = 650;
          if (lenMode == tenMode::nRADIO_MODE_TX)       lu16Ret_us = 550;
          break;
        case tenMode::nRADIO_MODE_TX:
          cNRF905_HW::vPwrOn();
          cNRF905_HW::vCEOff();
          cNRF905_HW::vEnOn();
          if (lenMode == tenMode::nRADIO_MODE_PWR_DOWN) lu16Ret_us = 3000 + 650;
          if (lenMode == tenMode::nRADIO_MODE_IDLE)     lu16Ret_us = 650;
          if (lenMode == tenMode::nRADIO_MODE_RX)       lu16Ret_us = 550;
          break;
        case tenMode::nRADIO_MODE_TX_PULSE:
          cNRF905_HW::vCEOn();
          lu16Ret_us = 5;
          break;
        default:
          break;
      }
      lenMode = lenNewMode;
    }
    return lu16Ret_us;
  }



  u8 u8Read_Status_Pin()
  {
    return cNRF905_HW::mDR.ui8Get() + (cNRF905_HW::mCD.ui8Get() << 1) + (cNRF905_HW::mAM.ui8Get() << 2);
  }

  u8 u8Read_Status()
  {
    //vRead_Status_Spi();
    return u8Read_Status_Pin();
  }

  void vInitCfg()
  {
    vSetChannel_433(stRegs.unReg00_Reg10_Cfg, 115, True);
    stRegs.unReg00_Reg10_Cfg.stBits.RX_AddressWidth = 4;
    stRegs.unReg00_Reg10_Cfg.stBits.TX_AddressWidth = 4;

    stRegs.unReg00_Reg10_Cfg.stBits.RX_PayloadWidth = 0;
    stRegs.unReg00_Reg10_Cfg.stBits.TX_PayloadWidth = 0;
  }

  void vSetTxAdress(u32 lu32Adress) 
  {
    stRegs.unReg22_Reg23_TxAdr.u32Adr = lu32Adress;
  }

  void vSetRxAdress(u32 lu32Adress) 
  {
    stRegs.unReg00_Reg10_Cfg.stBits.unRxAdress.u32Adr = lu32Adress;
  }

  void vSetTxPayloadWidth(u8 lu8PayloadWidth) 
  {
    stRegs.unReg00_Reg10_Cfg.stBits.TX_AddressWidth = lu8PayloadWidth;
  }

  void vSetRxPayloadWidth(u8 lu8PayloadWidth) 
  {
    stRegs.unReg00_Reg10_Cfg.stBits.RX_AddressWidth = lu8PayloadWidth;
  }

};
