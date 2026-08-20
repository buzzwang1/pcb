#pragma once

/**
 * Created by Gemini 3 Flash, 2026-05-05.
 * Resources: TI Ili9341 Datasheet (SBOS516B), DefaultArm Framework.
 *
 * @class CIli9341
 * @brief Professionelle Hardware-Abstraktion für den Ili9341 Monitor.
 * Jedes Register ist als eigene Union mit Bitfeldern (LSB-first) abgebildet.
 */

#include "TypeDef.h"
#include "ComRegMap.h"
#include "Ili9341_hw.h"

// --- Hauptklasse cSIC45X ---
struct cIli9341_RegisterMap: public cILI9341_HW
{
  typedef enum
  {
    nCMD00_NOP                 = 0, ///< 00h: No Operation
    nCMD01_SoftwareReset,           ///< 01h: Software Reset
    nCMD04_ReadDisplayID,           ///< 04h: Read Display Identification Information
    nCMD09_ReadDisplayStatus,       ///< 09h: Read Display Status
    nCMD0A_ReadDisplayPowerMode,    ///< 0Ah: Read Display Power Mode
    nCMD0B_ReadDisplayMADCTL,       ///< 0Bh: Read Display MADCTL
    nCMD0C_ReadDisplayPixelFormat,  ///< 0Ch: Read Display Pixel Format
    nCMD0D_ReadDisplayImageMode,    ///< 0Dh: Read Display Image Mode
    nCMD0E_ReadDisplaySignalMode,   ///< 0Eh: Read Display Signal Mode
    nCMD10_EnterSleepMode,          ///< 10h: Enter Sleep Mode
    nCMD11_SleepOut,                ///< 11h: Sleep Out
    nCMD12_PartialModeOn,           ///< 12h: Partial Mode ON
    nCMD13_NormalDisplayModeOn,     ///< 13h: Normal Display Mode ON
    nCMD20_DisplayInversionOff,     ///< 20h: Display Inversion OFF
    nCMD21_DisplayInversionOn,      ///< 21h: Display Inversion ON
    nCMD26_GammaSet,                ///< 26h: Gamma Set
    nCMD28_DisplayOff,              ///< 28h: Display OFF
    nCMD29_DisplayOn,               ///< 29h: Display ON
    nCMD2A_ColumnAddressSet,        ///< 2Ah: Column Address Set (X-Fenster)
    nCMD2B_PageAddressSet,          ///< 2Bh: Page Address Set (Y-Fenster)
    nCMD2C_MemoryWrite,             ///< 2Ch: Memory Write
    nCMD2E_MemoryRead,              ///< 2Eh: Memory Read
    nCMD30_PartialArea,             ///< 30h: Partial Area Definition
    nCMD33_VerticalScrollingDef,    ///< 33h: Vertical Scrolling Definition
    nCMD35_TearingEffectLineOff,    ///< 35h: Tearing Effect Line OFF
    nCMD36_MemoryAccessControl,     ///< 36h: MADCTL
    nCMD37_VerticalScrollingStart,  ///< 37h: Vertical Scrolling Start Address
    nCMD38_IdleModeOff,             ///< 38h: Idle Mode OFF
    nCMD39_IdleModeOn,              ///< 39h: Idle Mode ON
    nCMD3A_PixelFormatSet,          ///< 3Ah: COLMOD (Pixel Format)
    nCMD3C_WriteMemoryContinue,     ///< 3Ch: Write Memory Continue
    nCMD3E_ReadMemoryContinue,      ///< 3Eh: Read Memory Continue
    nCMDB1_FrameRateControlNormal,  ///< B1h: Frame Rate Control in Normal Mode
    nCMDB2_FrameRateControlIdle,    ///< B2h: Frame Rate Control in Idle Mode
    nCMDB3_FrameRateControlPartial, ///< B3h: Frame Rate Control in Partial Mode
    nCMDB4_DisplayInversionControl, ///< B4h: Display Inversion Control
    nCMDB6_DisplayFunctionControl,  ///< B6h: Display Function Control
    nCMDC0_PowerControl1,           ///< C0h: Power Control 1
    nCMDC1_PowerControl2,           ///< C1h: Power Control 2
    nCMDC5_VcomControl1,            ///< C5h: VCOM Control 1
    nCMDC7_VcomControl2,            ///< C7h: VCOM Control 2
    nCMDCB_PowerControlA,           ///< CBh: Power Control A (Neu hinzugefügt)
    nCMDCF_PowerControlB,           ///< CFh: Power Control B (Neu hinzugefügt)
    nCMDE0_PositiveGammaCorrection, ///< E0h: Positive Gamma Correction
    nCMDE1_NegativeGammaCorrection, ///< E1h: Negative Gamma Correction
    nCMDE8_DriverTimingControlA,    ///< E8h: Driver Timing Control A (Neu hinzugefügt)
    nCMDEA_DriverTimingControlB,    ///< EAh: Driver Timing Control B (Neu hinzugefügt)
    nCMDED_PowerOnSequenceControl,  ///< EDh: Power On Sequence Control (Neu hinzugefügt)
    nCMDF2_Enable3G,                ///< F2h: Enable 3G Gamma Control (Neu hinzugefügt)
    nCMDF6_InterfaceControl,        ///< F6h: Interface Control (Neu hinzugefügt)
    nCMDF7_PumpRatioControl,        ///< F7h: Pump ratio control (Neu hinzugefügt)

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
    { 0x00,  0,   0x00,   nACCESS_WO }, // nCMD00_NOP
    { 0x01,  0,   0x00,   nACCESS_WO }, // nCMD01_SoftwareReset
    { 0x04,  3,   0x00,   nACCESS_RO }, // nCMD04_ReadDisplayID
    { 0x09,  4,   0x00,   nACCESS_RO }, // nCMD09_ReadDisplayStatus
    { 0x0A,  1,   0x08,   nACCESS_RO }, // nCMD0A_ReadDisplayPowerMode
    { 0x0B,  1,   0x00,   nACCESS_RO }, // nCMD0B_ReadDisplayMADCTL
    { 0x0C,  1,   0x06,   nACCESS_RO }, // nCMD0C_ReadDisplayPixelFormat
    { 0x0D,  1,   0x00,   nACCESS_RO }, // nCMD0D_ReadDisplayImageMode
    { 0x0E,  1,   0x00,   nACCESS_RO }, // nCMD0E_ReadDisplaySignalMode
    { 0x10,  0,   0x00,   nACCESS_WO }, // nCMD10_EnterSleepMode
    { 0x11,  0,   0x00,   nACCESS_WO }, // nCMD11_SleepOut
    { 0x12,  0,   0x00,   nACCESS_WO }, // nCMD12_PartialModeOn
    { 0x13,  0,   0x00,   nACCESS_WO }, // nCMD13_NormalDisplayModeOn
    { 0x20,  0,   0x00,   nACCESS_WO }, // nCMD20_DisplayInversionOff
    { 0x21,  0,   0x00,   nACCESS_WO }, // nCMD21_DisplayInversionOn
    { 0x26,  1,   0x01,   nACCESS_WO }, // nCMD26_GammaSet
    { 0x28,  0,   0x00,   nACCESS_WO }, // nCMD28_DisplayOff
    { 0x29,  0,   0x00,   nACCESS_WO }, // nCMD29_DisplayOn
    { 0x2A,  4,   0x00,   nACCESS_WO }, // nCMD2A_ColumnAddressSet
    { 0x2B,  4,   0x00,   nACCESS_WO }, // nCMD2B_PageAddressSet
    { 0x2C,  0,   0x00,   nACCESS_WO }, // nCMD2C_MemoryWrite
    { 0x2E,  0,   0x00,   nACCESS_RO }, // nCMD2E_MemoryRead
    { 0x30,  4,   0x00,   nACCESS_WO }, // nCMD30_PartialArea
    { 0x33,  6,   0x00,   nACCESS_WO }, // nCMD33_VerticalScrollingDef
    { 0x35,  1,   0x00,   nACCESS_WO }, // nCMD35_TearingEffectLineOff
    { 0x36,  1,   0x00,   nACCESS_WO }, // nCMD36_MemoryAccessControl
    { 0x37,  2,   0x00,   nACCESS_WO }, // nCMD37_VerticalScrollingStart
    { 0x38,  0,   0x00,   nACCESS_WO }, // nCMD38_IdleModeOff
    { 0x39,  0,   0x00,   nACCESS_WO }, // nCMD39_IdleModeOn
    { 0x3A,  1,   0x55,   nACCESS_WO }, // nCMD3A_PixelFormatSet
    { 0x3C,  0,   0x00,   nACCESS_WO }, // nCMD3C_WriteMemoryContinue
    { 0x3E,  0,   0x00,   nACCESS_RO }, // nCMD3E_ReadMemoryContinue
    { 0xB1,  2,   0x001B, nACCESS_WO }, // nCMDB1_FrameRateControlNormal
    { 0xB2,  2,   0x001B, nACCESS_WO }, // nCMDB2_FrameRateControlIdle
    { 0xB3,  2,   0x001B, nACCESS_WO }, // nCMDB3_FrameRateControlPartial
    { 0xB4,  1,   0x00,   nACCESS_WO }, // nCMDB4_DisplayInversionControl
    { 0xB6,  3,   0x0A82, nACCESS_WO }, // nCMDB6_DisplayFunctionControl
    { 0xC0,  1,   0x23,   nACCESS_WO }, // nCMDC0_PowerControl1
    { 0xC1,  1,   0x10,   nACCESS_WO }, // nCMDC1_PowerControl2
    { 0xC5,  2,   0x3E28, nACCESS_WO }, // nCMDC5_VcomControl1
    { 0xC7,  1,   0x86,   nACCESS_WO }, // nCMDC7_VcomControl2
    { 0xCB,  5,   0x39,   nACCESS_WO }, // nCMDCB_PowerControlA (Benötigt 5 Parameter)
    { 0xCF,  3,   0x00,   nACCESS_WO }, // nCMDCF_PowerControlB (Benötigt 3 Parameter)
    { 0xE0,  15,  0x00,   nACCESS_WO }, // nCMDE0_PositiveGammaCorrection
    { 0xE1,  15,  0x00,   nACCESS_WO }, // nCMDE1_NegativeGammaCorrection
    { 0xE8,  3,   0x85,   nACCESS_WO }, // nCMDE8_DriverTimingControlA (Benötigt 3 Parameter)
    { 0xEA,  2,   0x00,   nACCESS_WO }, // nCMDEA_DriverTimingControlB (Benötigt 2 Parameter)
    { 0xED,  4,   0x64,   nACCESS_WO }, // nCMDED_PowerOnSequenceControl (Benötigt 4 Parameter)
    { 0xF2,  1,   0x00,   nACCESS_WO }, // nCMDF2_Enable3G (Benötigt 1 Parameter)
    { 0xF6,  3,   0x01,   nACCESS_WO }, // nCMDF6_InterfaceControl (Benötigt 3 Parameter)
    { 0xF7,  1,   0x20,   nACCESS_WO }  // nCMDF7_PumpRatioControl
  };

  static constexpr u16 mu16GetRegMaxLen = 15;

  u8 u8GetRegAdr(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Address;
  }

  u16 u16GetRegLen(u8 lu8Idx)
  {
    return gastRegLookupTable[(tenRegIndex)lu8Idx].u8Length;
  }

  // --- Register-Instanzen (Unions) ---

  /** @union unCMD04_ReadDisplayID (3 Bytes Payload RO) */
  union
  {
    u8 au8Byte[3];
    struct
    {
      u8 u8ID1;                    ///< LCD-Hersteller-ID
      u8 u8ID2;                    ///< LCD-Treiber-Version
      u8 u8ID3;                    ///< LCD-Treiber-ID
    } stParam;
  }unCMD04_ReadDisplayID;

  /** @union unCMD09_ReadDisplayStatus (4 Bytes Payload RO) */
  union
  {
    u8 au8Byte[4];
    struct
    {
      u8 u8Status1;                ///< Status-Byte 1 (Dummy-Read / Parameter-Bits)
      u8 u8Status2;                ///< Status-Byte 2
      u8 u8Status3;                ///< Status-Byte 3
      u8 u8Status4;                ///< Status-Byte 4
    } stParam;
  }unCMD09_ReadDisplayStatus;

  /** @union unCMD0A_ReadDisplayPowerMode (1 Byte RO) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8Reserved : 2;
      u8 u8GammaCtrl: 1;           ///< bit 2: Gamma Control Status
      u8 u8NormalOn : 1;           ///< bit 3: Normal Display Mode Status
      u8 u8SleepOut : 1;           ///< bit 4: Sleep Out Status
      u8 u8PartialOn: 1;           ///< bit 5: Partial Mode Status
      u8 u8DisplayOn: 1;           ///< bit 6: Display ON/OFF Status
      u8 u8BoosterOn: 1;           ///< bit 7: Booster Status
    } stBits;
  }unCMD0A_ReadDisplayPowerMode;

  /** @union unCMD0B_ReadDisplayMADCTL (1 Byte RO) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8Reserved : 2;
      u8 u8MH        : 1;          ///< bit 2: Horizontal Refresh Order
      u8 u8BGR       : 1;          ///< bit 3: RGB/BGR Order
      u8 u8ML        : 1;          ///< bit 4: Vertical Refresh Order
      u8 u8MV        : 1;          ///< bit 5: Row/Column Exchange
      u8 u8MX        : 1;          ///< bit 6: Column Address Order
      u8 u8MY        : 1;          ///< bit 7: Row Address Order
    } stBits;
  }unCMD0B_ReadDisplayMADCTL;

  /** @union unCMD0C_ReadDisplayPixelFormat (1 Byte RO) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8DPI      : 3;           ///< bit 0..2: RGB Interface Pixel Format
      u8 u8Reserved : 1;
      u8 u8MCU      : 3;           ///< bit 4..6: MCU Interface Pixel Format
      u8 u8Reserved2: 1;
    } stBits;
  }unCMD0C_ReadDisplayPixelFormat;

  /** @union unCMD0D_ReadDisplayImageMode (1 Byte RO) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8GammaCurve : 3;         ///< bit 0..2: Aktuelle Gamma-Kurve
      u8 u8Reserved   : 2;
      u8 u8Inversion  : 1;         ///< bit 5: Display Inversion Status
      u8 u8Reserved2  : 2;
    } stBits;
  }unCMD0D_ReadDisplayImageMode;

  /** @union unCMD0E_ReadDisplaySignalMode (1 Byte RO) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8Reserved  : 5;
      u8 u8TearingOn : 1;          ///< bit 5: Tearing Effect Line Status
      u8 u8TE_Mode   : 1;          ///< bit 6: Tearing Effect Output Mode (0 oder 1)
      u8 u8Reserved2 : 1;
    } stBits;
  }unCMD0E_ReadDisplaySignalMode;



  /** @union unCMD26_GammaSet */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8GammaCurveSelect : 4;   ///< bit 0..3: Gamma-Kurven Bitmaske
      u8 u8Reserved         : 4;
    } stBits;
  }unCMD26_GammaSet;


  /** @union unCMD2A_ColumnAddressSet (4 Bytes Payload) */
  union
  {
    u8 au8Byte[4];
    struct
    {
      u8 u8StartCol_MSB;           ///< Byte 0: Start-Spalte MSB
      u8 u8StartCol_LSB;           ///< Byte 1: Start-Spalte LSB
      u8 u8EndCol_MSB;             ///< Byte 2: End-Spalte MSB
      u8 u8EndCol_LSB;             ///< Byte 3: End-Spalte LSB
    } stParam;
  }unCMD2A_ColumnAddressSet;

  /** @union unCMD2B_PageAddressSet (4 Bytes Payload) */
  union
  {
    u8 au8Byte[4];
    struct
    {
      u8 u8StartPage_MSB;          ///< Byte 0: Start-Zeile MSB
      u8 u8StartPage_LSB;          ///< Byte 1: Start-Zeile LSB
      u8 u8EndPage_MSB;            ///< Byte 2: End-Zeile MSB
      u8 u8EndPage_LSB;            ///< Byte 3: End-Zeile LSB
    } stParam;
  }unCMD2B_PageAddressSet;


  /** @union unCMD30_PartialArea (4 Bytes Payload) */
  union
  {
    u8 au8Byte[4];
    struct
    {
      u8 u8StartRow_MSB;           ///< Byte 0: Start-Zeile MSB
      u8 u8StartRow_LSB;           ///< Byte 1: Start-Zeile LSB
      u8 u8EndRow_MSB;             ///< Byte 2: End-Zeile MSB
      u8 u8EndRow_LSB;             ///< Byte 3: End-Zeile LSB
    } stParam;
  }unCMD30_PartialArea;

  /** @union unCMD33_VerticalScrollingDef (6 Bytes Payload) */
  union
  {
    u8 au8Byte[6];
    struct
    {
      u8 u8TopFixedArea_MSB;       ///< Byte 0: Obere feste Zone MSB
      u8 u8TopFixedArea_LSB;       ///< Byte 1: Obere feste Zone LSB
      u8 u8ScrollArea_MSB;         ///< Byte 2: Scroll-Fenstergröße MSB
      u8 u8ScrollArea_LSB;         ///< Byte 3: Scroll-Fenstergröße LSB
      u8 u8BottomFixedArea_MSB;    ///< Byte 4: Untere feste Zone MSB
      u8 u8BottomFixedArea_LSB;    ///< Byte 5: Untere feste Zone LSB
    } stParam;
  }unCMD33_VerticalScrollingDef;

  /** @union unCMD35_TearingEffectLineOff / Line On (1 Byte Payload) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8TEMode   : 1;           ///< bit 0: Tearing-Modus (0=V-Blanking, 1=V+H Blanking)
      u8 u8Reserved : 7;
    } stBits;
  }unCMD35_TearingEffectLineCtrl;

  /** @union unCMD36_MemoryAccessControl (MADCTL) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8Reserved1 : 2;
      u8 u8MH        : 1;          ///< bit 2: Horizontal Refresh Order
      u8 u8BGR       : 1;          ///< bit 3: RGB/BGR Order (0=RGB, 1=BGR Panel)
      u8 u8ML        : 1;          ///< bit 4: Vertical Refresh Order
      u8 u8MV        : 1;          ///< bit 5: Row/Column Exchange (Querformat)
      u8 u8MX        : 1;          ///< bit 6: Column Address Order
      u8 u8MY        : 1;          ///< bit 7: Row Address Order
    } stBits;
  }unCMD36_MemoryAccessControl;

  /** @union unCMD37_VerticalScrollingStart (2 Bytes Payload) */
  union
  {
    u8 au8Byte[2];
    struct
    {
      u8 u8ScrollOffset_MSB;       ///< Byte 0: Zeilenversatz MSB
      u8 u8ScrollOffset_LSB;       ///< Byte 1: Zeilenversatz LSB
    } stParam;
  }unCMD37_VerticalScrollingStart;


  /** @union unCMD3A_PixelFormatSet (COLMOD) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8DPI      : 3;           ///< bit 0..2: RGB Interface Pixel Format (0x05=16bit, 0x06=18bit)
      u8 u8Reserved : 1;
      u8 u8MCU      : 3;           ///< bit 4..6: MCU Interface Pixel Format (0x05=16bit RGB565)
      u8 u8Reserved2: 1;
    } stBits;
  }unCMD3A_PixelFormatSet;


  /** @union tunCMDB1_FrameRateControlNormal (2 Bytes Payload) */
  union
  {
    u8 au8Byte[2];
    struct
    {
      u8 u8DIVA : 2;               ///< Byte 0, bit 0..1: Division Ratio des internen Takts
      u8 u8Res1 : 6;
      u8 u8RTNA : 5;               ///< Byte 1, bit 0..4: Frame Rate (Zeilenperioden pro Frame)
      u8 u8Res2 : 3;
    } stBits;
  }unCMDB1_FrameRateControl;


  /** @union unCMDB2_FrameRateControlIdl (2 Bytes Payload) */
  union
  {
    u8 au8Byte[2];
    struct
    {
      u8 u8DIVA : 2;               ///< Byte 0, bit 0..1: Division Ratio des internen Takts
      u8 u8Res1 : 6;
      u8 u8RTNA : 5;               ///< Byte 1, bit 0..4: Frame Rate (Zeilenperioden pro Frame)
      u8 u8Res2 : 3;
    } stBits;
  }unCMDB2_FrameRateControlIdle;

  /** @union unCMDB3_FrameRateControlPartial (2 Bytes Payload) */
  union
  {
    u8 au8Byte[2];
    struct
    {
      u8 u8DIVA : 2;               ///< Byte 0, bit 0..1: Division Ratio des internen Takts
      u8 u8Res1 : 6;
      u8 u8RTNA : 5;               ///< Byte 1, bit 0..4: Frame Rate (Zeilenperioden pro Frame)
      u8 u8Res2 : 3;
    } stBits;
  }unCMDB3_FrameRateControlPartial;

  /** @union unCMDB4_DisplayInversionControl (1 Byte Payload) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8NLA      : 2;           ///< bit 0..1: Inversion im Normal-Modus (0=Line, 1=Frame)
      u8 u8NLB      : 2;           ///< bit 2..3: Inversion im Idle-Modus
      u8 u8NLC      : 2;           ///< bit 4..5: Inversion im Partial-Modus
      u8 u8Reserved : 2;
    } stBits;
  }unCMDB4_DisplayInversionControl;

  /** @union unCMDB6_DisplayFunctionControl (3 Bytes Payload) */
  union
  {
    u8 au8Byte[3];
    struct
    {
      u8 u8ISC      : 4;           ///< Byte 0, bit 0..3: Scan-Intervall für Gate-Treiber
      u8 u8PT       : 2;           ///< Byte 0, bit 4..5: Source-Ausgangsperiode
      u8 u8PTG      : 2;           ///< Byte 0, bit 6..7: Gate-Scan-Modus im Teilbildmodus
      u8 u8NL       : 6;           ///< Byte 1, bit 0..5: Anzahl der LCD-Zeilen (z.B. 0x27 = 320 Zeilen)
      u8 u8Res1     : 2;
      u8 u8REV      : 1;           ///< Byte 2, bit 0: Display Inversion Source/Gate-Struktur
      u8 u8SM       : 1;           ///< Byte 2, bit 1: Gate-Treiber Scan-Richtung
      u8 u8GS       : 1;           ///< Byte 2, bit 2: Gate Scan Richtung
      u8 u8SS       : 1;           ///< Byte 2, bit 3: Source Scan Richtung
      u8 u8PCD_DIV  : 4;           ///< Byte 2, bit 4..7: Interner Taktteiler
    } stBits;
  }unCMDB6_DisplayFunctionControl;

  /** @union unCMDC0_PowerControl1 (1 Byte Payload) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8VRH      : 6;           ///< bit 0..5: Setzt die GVDD-Spannung
      u8 u8Reserved : 2;
    } stBits;
  }unCMDC0_PowerControl1;

  /** @union unCMDC1_PowerControl2 (1 Byte Payload) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8BT       : 3;           ///< bit 0..2: Multiplikationsfaktor der Ladungspumpen
      u8 u8Reserved : 5;
    } stBits;
  }unCMDC1_PowerControl2;

  /** @union unCMDC5_VcomControl1 (2 Bytes Payload) */
  union
  {
    u8 au8Byte[2];
    struct
    {
      u8 u8VCOML    : 7;           ///< Byte 0, bit 0..6: Setzt VCOML Spannung
      u8 u8Res1     : 1;
      u8 u8VCOMH    : 7;           ///< Byte 1, bit 0..6: Setzt VCOMH Spannung
      u8 u8Res2     : 1;
    } stBits;
  }unCMDC5_VcomControl1;

  /** @union unCMDC7_VcomControl2 (1 Byte Payload) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8VCOMOFFS : 7;           ///< bit 0..6: Setzt VCOM Offset-Spannung
      u8 u8nVM      : 1;           ///< bit 7: Speicher-Steuerung für VCOM-Offset
    } stBits;
  }unCMDC7_VcomControl2;

  /** @union unCMDCB_PowerControlA (Extended, 5 Bytes Payload) */
  union
  {
    u8 au8Byte[5];
    struct
    {
      u8 u8RegData0;               ///< Param 1 (Festwert 0x39)
      u8 u8RegData1;               ///< Param 2 (Festwert 0x2C)
      u8 u8RegData2;               ///< Param 3 (Festwert 0x00)
      u8 u8REG_VDD  : 3;           ///< Param 4, bit 0..2: VDD Spannungseinstellung
      u8 u8Res1     : 5;
      u8 u8VBC      : 3;           ///< Param 5, bit 0..2: Booster AC-Frequenzsteuerung
      u8 u8Res2     : 5;
    } stBits;
  }unCMDCB_PowerControlA;

  /** @union unCMDCF_PowerControlB (Extended, 3 Bytes Payload) */
  union
  {
    u8 au8Byte[3];
    struct
    {
      u8 u8Data0;                  ///< Param 1: Power Control Setting 1
      u8 u8DRV_esa  : 1;           ///< Param 2, bit 0: Driver ESA Enable
      u8 u8Res1     : 1;
      u8 u8PCEQ     : 2;           ///< Param 2, bit 2..3: Power Control Equalizer
      u8 u8DRV_gva  : 3;           ///< Param 2, bit 4..6: Gate Driver Voltage Adjustment
      u8 u8Res2     : 1;
      u8 u8PowerCtrl3;             ///< Param 3: Power Control Setting 3
    } stBits;
  }unCMDCF_PowerControlB;

  /** @union unCMDE0_PositiveGammaCorrection (15 Bytes Payload) */
  union
  {
    u8 au8Byte[15];
    struct
    {
      u8 au8GammaVP[15];           ///< 15 anwenderspezifische Graustufen-Kurvenpunkte
    } stParam;
  }unCMDE0_PosetiveGammaCorrection;

  /** @union unCMDE1_NegativeGammaCorrection (15 Bytes Payload) */
  union
  {
    u8 au8Byte[15];
    struct
    {
      u8 au8GammaVP[15];           ///< 15 anwenderspezifische Graustufen-Kurvenpunkte
    } stParam;
  }unCMDE1_NegativeGammaCorrection;

  /** @union unCMDE8_DriverTimingControlA (Extended, 3 Bytes Payload) */
  union
  {
    u8 au8Byte[3];
    struct
    {
      u8 u8NonOverlapDly : 4;      ///< Param 1, bit 0..3: Gate/Source Überlappungs-Verzögerung
      u8 u8Res1          : 4;
      u8 u8GateDriverDly : 4;      ///< Param 2, bit 0..3: Gate-Treiber Schaltzeitpunkt
      u8 u8Res2          : 4;
      u8 u8SourceDriverTiming;     ///< Param 3: Source Driver Timing Control
    } stBits;
  }unCMDE8_DriverTimingControlA;

  /** @union unCMDEA_DriverTimingControlB (Extended, 2 Bytes Payload) */
  union
  {
    u8 au8Byte[2];
    struct
    {
      u8 u8GateDriverTiming1;      ///< Param 1: Gate Driver Timing 1
      u8 u8GateDriverTiming2;      ///< Param 2: Gate Driver Timing 2
    } stParam;
  }unCMDEA_DriverTimingControlB;

  /** @union unCMDED_PowerOnSequenceControl (Extended, 4 Bytes Payload) */
  union
  {
    u8 au8Byte[4];
    struct
    {
      u8 u8SoftStartDly : 4;       ///< Param 1, bit 0..3: Soft-Start Verzögerung
      u8 u8Res1         : 4;
      u8 u8PowerOnDly   : 4;       ///< Param 2, bit 0..3: Power-On Sequenz Verzögerung
      u8 u8Res2         : 4;
      u8 u8EnChgPump    : 1;       ///< Param 3, bit 0: Ladungspumpe aktivieren
      u8 u8Res3         : 7;
      u8 u8DDly         : 4;       ///< Param 4, bit 0..3: Discharge Delay Control
      u8 u8Res4         : 4;
    } stBits;
  }unCMDED_PowerOnSequenceControl;

  /** @union unCMDF2_Enable3G (Extended, 1 Byte Payload) */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8Enable3GGamma : 1;      ///< bit 0: Aktiviert 3-Gamma Control
      u8 u8Reserved      : 7;
    } stBits;
  }unCMDF2_Enable3G;

  /** @union unCMDF6_InterfaceControl (Extended, 3 Bytes Payload) */
  union
  {
    u8 au8Byte[3];
    struct
    {
      u8 u8DM          : 2;        ///< Param 1, bit 0..1: Display Operation Mode
      u8 u8RM          : 1;        ///< Param 1, bit 2: Interface Selection (MCU / RGB)
      u8 u8Res1        : 2;
      u8 u8ENC_DITHER  : 1;        ///< Param 1, bit 5: Dithering aktivieren
      u8 u8Res2        : 2;
      u8 u8MDT         : 2;        ///< Param 2, bit 0..1: Memory Data Transfer Format
      u8 u8Res3        : 6;
      u8 u8WEMODE      : 1;        ///< Param 3, bit 0: Write Data Masking Option
      u8 u8Res4        : 7;
    } stBits;
  }unCMDF6_InterfaceControl;


  /** @union tunCMDF7_PumpRatioControl */
  union
  {
    u8 au8Byte[1];
    struct
    {
      u8 u8Ratio : 2;           ///< bit 0..1: Pumpen-Übersetzungsverhältnis (Ratio Control)
      u8 u8Reserved : 2;           ///< bit 2..3: Reserviert
      u8 u8Internal : 2;           ///< bit 4..5: Interner Booster Steuerwert (üblicherweise 0x20 aktiv)
      u8 u8Reserved2 : 2;           ///< bit 6..7: Reserviert
    } stBits;
  }unCMDF7_PumpRatioControl;

  static constexpr u16  mu16Width    = 240;
  static constexpr u16  mu16Height   = 320;
  static constexpr u32  mu32PixelCnt = mu16Width * mu16Height;
  
  static constexpr u32  mu32GramAdr  = 0x20040000;

  u8* pu8GetMem(u8 lu8Idx)
  {
    switch ((tenRegIndex)lu8Idx)
    {
      case nCMD04_ReadDisplayID:           return unCMD04_ReadDisplayID.au8Byte;
      case nCMD09_ReadDisplayStatus:       return unCMD09_ReadDisplayStatus.au8Byte;
      case nCMD0A_ReadDisplayPowerMode:    return unCMD0A_ReadDisplayPowerMode.au8Byte;
      case nCMD0B_ReadDisplayMADCTL:       return unCMD0B_ReadDisplayMADCTL.au8Byte;
      case nCMD0C_ReadDisplayPixelFormat:  return unCMD0C_ReadDisplayPixelFormat.au8Byte;
      case nCMD0D_ReadDisplayImageMode:    return unCMD0D_ReadDisplayImageMode.au8Byte;
      case nCMD0E_ReadDisplaySignalMode:   return unCMD0E_ReadDisplaySignalMode.au8Byte;
      case nCMD26_GammaSet:                return unCMD26_GammaSet.au8Byte;
      case nCMD2A_ColumnAddressSet:        return unCMD2A_ColumnAddressSet.au8Byte;
      case nCMD2B_PageAddressSet:          return unCMD2B_PageAddressSet.au8Byte;
      case nCMD30_PartialArea:             return unCMD30_PartialArea.au8Byte;
      case nCMD33_VerticalScrollingDef:    return unCMD33_VerticalScrollingDef.au8Byte;
      case nCMD35_TearingEffectLineOff:    return unCMD35_TearingEffectLineCtrl.au8Byte;
      case nCMD36_MemoryAccessControl:     return unCMD36_MemoryAccessControl.au8Byte;
      case nCMD37_VerticalScrollingStart:  return unCMD37_VerticalScrollingStart.au8Byte;
      case nCMD3A_PixelFormatSet:          return unCMD3A_PixelFormatSet.au8Byte;
      case nCMDB1_FrameRateControlNormal:  return unCMDB1_FrameRateControl.au8Byte;
      case nCMDB2_FrameRateControlIdle:    return unCMDB2_FrameRateControlIdle.au8Byte;
      case nCMDB3_FrameRateControlPartial: return unCMDB3_FrameRateControlPartial.au8Byte;
      case nCMDB4_DisplayInversionControl: return unCMDB4_DisplayInversionControl.au8Byte;
      case nCMDB6_DisplayFunctionControl:  return unCMDB6_DisplayFunctionControl.au8Byte;
      case nCMDC0_PowerControl1:           return unCMDC0_PowerControl1.au8Byte;
      case nCMDC1_PowerControl2:           return unCMDC1_PowerControl2.au8Byte;
      case nCMDC5_VcomControl1:            return unCMDC5_VcomControl1.au8Byte;
      case nCMDC7_VcomControl2:            return unCMDC7_VcomControl2.au8Byte;
      case nCMDCB_PowerControlA:           return unCMDCB_PowerControlA.au8Byte;
      case nCMDCF_PowerControlB:           return unCMDCF_PowerControlB.au8Byte;
      case nCMDE0_PositiveGammaCorrection: return unCMDE0_PosetiveGammaCorrection.au8Byte;
      case nCMDE1_NegativeGammaCorrection: return unCMDE1_NegativeGammaCorrection.au8Byte;
      case nCMDE8_DriverTimingControlA:    return unCMDE8_DriverTimingControlA.au8Byte;
      case nCMDEA_DriverTimingControlB:    return unCMDEA_DriverTimingControlB.au8Byte;
      case nCMDED_PowerOnSequenceControl:  return unCMDED_PowerOnSequenceControl.au8Byte;
      case nCMDF2_Enable3G:                return unCMDF2_Enable3G.au8Byte;
      case nCMDF6_InterfaceControl:        return unCMDF6_InterfaceControl.au8Byte;
      case nCMDF7_PumpRatioControl:        return unCMDF7_PumpRatioControl.au8Byte;        // 0xF7
      default:                             return (u8*)0;
    }
  }

  class cSEQ00_Reset : public cComSeq
  {
    public:
    // It will be necessary to wait 5msec before sending new command following software reset
    // The display module loads all display supplier factory default values to the registers during this 5msec
    // If Software Reset is applied during Sleep Out mode, it will be necessary to wait 120msec before sending Sleep out command
    // Software Reset Command cannot be sent during Sleep Out sequence
    const cEntry macRegList[1 + 3] = { cWrite(nCMD01_SoftwareReset), cComSeqDelay(5)};

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

    cIli9341_RegisterMap* mpcRegMap;

    const cEntry macRegList[22] = { cWrite(nCMDCB_PowerControlA),         cWrite(nCMDCF_PowerControlB),
                                    cWrite(nCMDE8_DriverTimingControlA),  cWrite(nCMDEA_DriverTimingControlB),
                                    cWrite(nCMDED_PowerOnSequenceControl),
                                    cWrite(nCMDF7_PumpRatioControl),
                                    cWrite(nCMDC0_PowerControl1),         cWrite(nCMDC1_PowerControl2),
                                    cWrite(nCMDC5_VcomControl1),          cWrite(nCMDC7_VcomControl2),
                                    cWrite(nCMD36_MemoryAccessControl),
                                    cWrite(nCMD3A_PixelFormatSet),
                                    cWrite(nCMDB1_FrameRateControlNormal),
                                    cWrite(nCMDB6_DisplayFunctionControl),
                                    cWrite(nCMDF2_Enable3G),
                                    cWrite(nCMD26_GammaSet),
                                    cWrite(nCMDE0_PositiveGammaCorrection),
                                    cWrite(nCMDE1_NegativeGammaCorrection),
                                    cComSeqDelay(100),
                                    cWrite(nCMD11_SleepOut), // This command turns off sleep mode.
                                                             // In this mode e.g. the DC/DC converter is enabled, Internal oscillator is started,
                                                             // and panel scanning is started.
                                                             // It will be necessary to wait 5msec before sending next command
                                                             // It will be necessary to wait 120msec after sending Sleep In command (when in Sleep Out mode)
                                                             // before Sleep Out command can be sent.
                                  };

    cSEQ01_Init1(cIli9341_RegisterMap *lpcIli9341_RegisterMap)
    {
      mpcRegMap = lpcIli9341_RegisterMap;
    }

    cEntry* pacGet() override
    {

      mpcRegMap->unCMDCB_PowerControlA.au8Byte[0] = 0x39;
      mpcRegMap->unCMDCB_PowerControlA.au8Byte[1] = 0x2C;
      mpcRegMap->unCMDCB_PowerControlA.au8Byte[2] = 0x00;
      mpcRegMap->unCMDCB_PowerControlA.au8Byte[3] = 0x34;
      mpcRegMap->unCMDCB_PowerControlA.au8Byte[4] = 0x02;

      mpcRegMap->unCMDCF_PowerControlB.au8Byte[0] = 0x00;
      mpcRegMap->unCMDCF_PowerControlB.au8Byte[1] = 0xC1;
      mpcRegMap->unCMDCF_PowerControlB.au8Byte[2] = 0x30;

      mpcRegMap->unCMDE8_DriverTimingControlA.au8Byte[0] = 0x85;
      mpcRegMap->unCMDE8_DriverTimingControlA.au8Byte[1] = 0x00;
      mpcRegMap->unCMDE8_DriverTimingControlA.au8Byte[2] = 0x78;

      mpcRegMap->unCMDEA_DriverTimingControlB.au8Byte[0] = 0x00;
      mpcRegMap->unCMDEA_DriverTimingControlB.au8Byte[1] = 0x00;

      mpcRegMap->unCMDED_PowerOnSequenceControl.au8Byte[0] = 0x64;
      mpcRegMap->unCMDED_PowerOnSequenceControl.au8Byte[1] = 0x03;
      mpcRegMap->unCMDED_PowerOnSequenceControl.au8Byte[2] = 0x12;
      mpcRegMap->unCMDED_PowerOnSequenceControl.au8Byte[3] = 0x81;

      mpcRegMap->unCMDF7_PumpRatioControl.au8Byte[0] = 0x20;

      mpcRegMap->unCMDC0_PowerControl1.au8Byte[0] = 0x23;
      mpcRegMap->unCMDC1_PowerControl2.au8Byte[0] = 0x10;

      mpcRegMap->unCMDC5_VcomControl1.au8Byte[0] = 0x3E;
      mpcRegMap->unCMDC5_VcomControl1.au8Byte[1] = 0x28;
      mpcRegMap->unCMDC7_VcomControl2.au8Byte[0] = 0x86;

      //mpcRegMap->unCMD36_MemoryAccessControl.au8Byte[0] = 0x50; // Portrait_1
      //mpcRegMap->unCMD36_MemoryAccessControl.au8Byte[0] = 0x80; // Portrait_2
      mpcRegMap->unCMD36_MemoryAccessControl.au8Byte[0] = 0x20; // Landscape_1
      //mpcRegMap->unCMD36_MemoryAccessControl.au8Byte[0] = 0xE0; // Landscape_2

      // 0 = RGB color filter panel
      // 1 = BGR color filter panel
      mpcRegMap->unCMD36_MemoryAccessControl.au8Byte[0] |= 0x08;

      mpcRegMap->unCMD3A_PixelFormatSet.au8Byte[0] = 0x55; // 16Bit per Pixel

      mpcRegMap->unCMDB1_FrameRateControl.au8Byte[0] = 0x00;
      mpcRegMap->unCMDB1_FrameRateControl.au8Byte[1] = 0x18;

      mpcRegMap->unCMDB6_DisplayFunctionControl.au8Byte[0] = 0x08;
      mpcRegMap->unCMDB6_DisplayFunctionControl.au8Byte[1] = 0x82;
      mpcRegMap->unCMDB6_DisplayFunctionControl.au8Byte[2] = 0x27;

      mpcRegMap->unCMDF2_Enable3G.au8Byte[0] = 0x00;

      mpcRegMap->unCMD26_GammaSet.au8Byte[0] = 0x01;

      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[0]  = 0x0F;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[1]  = 0x31;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[2]  = 0x2B;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[3]  = 0x0C;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[4]  = 0x0E;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[5]  = 0x08;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[6]  = 0x4E;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[7]  = 0xF1;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[8]  = 0x37;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[9]  = 0x07;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[10] = 0x10;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[11] = 0x03;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[12] = 0x0E;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[13] = 0x09;
      mpcRegMap->unCMDE0_PosetiveGammaCorrection.au8Byte[14] = 0x00;

      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[0]  = 0x00;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[1]  = 0x0E;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[2]  = 0x14;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[3]  = 0x03;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[4]  = 0x11;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[5]  = 0x07;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[6]  = 0x31;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[7]  = 0xC1;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[8]  = 0x48;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[9]  = 0x08;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[10] = 0x0F;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[11] = 0x0C;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[12] = 0x31;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[13] = 0x36;
      mpcRegMap->unCMDE1_NegativeGammaCorrection.au8Byte[14] = 0x0F;


      mpcRegMap->unCMD2A_ColumnAddressSet.au8Byte[0] = 0x00;
      mpcRegMap->unCMD2A_ColumnAddressSet.au8Byte[1] = 0x00;
      mpcRegMap->unCMD2A_ColumnAddressSet.au8Byte[2] = (320 - 1) >> 8;
      mpcRegMap->unCMD2A_ColumnAddressSet.au8Byte[3] = (320 - 1) & 0xFF;

      mpcRegMap->unCMD2B_PageAddressSet.au8Byte[0] = 0x00;
      mpcRegMap->unCMD2B_PageAddressSet.au8Byte[1] = 0x00;
      mpcRegMap->unCMD2B_PageAddressSet.au8Byte[2] = (240 - 1) >> 8;
      mpcRegMap->unCMD2B_PageAddressSet.au8Byte[3] = (240 - 1) & 0xFF;

      return (cEntry*) macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ04_Enable : public cComSeq
  {
  public:

    //                                2 frame Min (34ms)
    const cEntry macRegList[3 + 1] = {cComSeqDelay(80), cWrite(nCMD29_DisplayOn)};

    cEntry* pacGet() override
    {
      //cmd2(ILI9341_RESET);
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ05_Disable : public cComSeq
  {
    public:
    //                                                           2 frame Min (34ms)
    const cEntry macRegList[1 + 3] = { cWrite(nCMD28_DisplayOff), cComSeqDelay(50)};

    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };


  class cSEQ06_DeInit : public cComSeq
  {
    public:
    const cEntry macRegList[1 + 3] = { cWrite(nCMD10_EnterSleepMode),
                                       cComSeqDelay(120) // It takes 120msec to get into Sleep In mode after SLPIN command issued.
                                     };

    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };


  class cSEQ07_WriteGRAMCfg : public cComSeq
  {
    public:

    const cEntry macRegList[3] = { cWrite(nCMD2A_ColumnAddressSet), cWrite(nCMD2B_PageAddressSet), cWrite(nCMD2C_MemoryWrite)};

    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ08_WriteGRAMData : public cComSeq
  {
    public:
    cIli9341_RegisterMap* mpcRegMap;

    // Es wird 32Bit weise übertragen. Die Hardware kann max. 64k auf einmal. Daher muss es aufgeteilt werden
    // Weil 32Bit weise kopiert wird, müsss es durch 4 teilbar sein
    // PixelCnt ist 76800, d.h. 153600 Bytes
    // 64000 + 64000 + 25600
    const cEntry macRegList[7 * 3] = { cComSeqPtrWr(64000, (u32)(mpcRegMap->mu32GramAdr +      0)),
                                       cComSeqPtrWr(64000, (u32)(mpcRegMap->mu32GramAdr +  64000)),
                                       cComSeqPtrWr(25600, (u32)(mpcRegMap->mu32GramAdr + 128000))
                                     };

    cSEQ08_WriteGRAMData(cIli9341_RegisterMap* lpcRegMap)
    {
      mpcRegMap = lpcRegMap;
    }

    cEntry* pacGet() override
    {
      return (cEntry*)macRegList;
    }

    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  cSEQ00_Reset      mcSEQ00_Reset;
  cSEQ01_Init1      mcSEQ01_Init1;
  cSEQ04_Enable     mcSEQ04_Enable;
  cSEQ05_Disable    mcSEQ05_Disable;
  cSEQ06_DeInit     mcSEQ06_DeInit;

  cSEQ07_WriteGRAMCfg   mcSEQ07_WriteGRAMCfg;
  cSEQ08_WriteGRAMData  mcSEQ08_WriteGRAMData;

  enum tenSequences : u8
  {
    nSEQ00Reset    = cComSeq::tenDefaultSequences::nSEQ00Reset,
    nSEQ01Init1    = cComSeq::tenDefaultSequences::nSEQ01Init1,
    nSEQ02Init2    = cComSeq::tenDefaultSequences::nSEQ02Init2,
    nSEQ04Enable   = cComSeq::tenDefaultSequences::nSEQ04Enable,
    nSEQ05Disable  = cComSeq::tenDefaultSequences::nSEQ05Disable,
    nSEQ06DeInit   = cComSeq::tenDefaultSequences::nSEQ06DeInit,
    nSEQ07WriteGRAMCfg,
    nSEQ08WriteGRAMData,
  };


  cComSeq* pcGetSequence(u8 lu8SeqIdx)
  {
    switch (lu8SeqIdx)
    {
      case tenSequences::nSEQ00Reset:     return &mcSEQ00_Reset;
      case tenSequences::nSEQ01Init1:     return &mcSEQ01_Init1;
      case tenSequences::nSEQ04Enable:    return &mcSEQ04_Enable;
      case tenSequences::nSEQ05Disable:   return &mcSEQ05_Disable;
      case tenSequences::nSEQ06DeInit:    return &mcSEQ06_DeInit;

      case tenSequences::nSEQ07WriteGRAMCfg:  return &mcSEQ07_WriteGRAMCfg;
      case tenSequences::nSEQ08WriteGRAMData: return &mcSEQ08_WriteGRAMData;
      default: return null;
    }
  }

  constexpr u8* pGRAM()
  {
    return (u8*)mu32GramAdr;
  }

  void vClearGRAM()
  {
    cMemTools::vMemSet(pGRAM(), 31, mu32PixelCnt * 2);
  }

  //void vSetGRAM(u8* lpau8GRAM)
  //{
  //  mpau8GRAM = lpau8GRAM;
  //}

  cIli9341_RegisterMap()
    : mcSEQ01_Init1(this),
      mcSEQ08_WriteGRAMData(this)
  {
  }
};
