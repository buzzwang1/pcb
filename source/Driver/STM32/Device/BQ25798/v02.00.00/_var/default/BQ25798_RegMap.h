#ifndef __BQ25798_REGMAP_H__
#define __BQ25798_REGMAP_H__

/**
 * Created by Gemini 3 Flash, 2026-04-02
 * Resource: TI BQ25798 Datasheet (SLUSEN4)
 */

#include "TypeDef.h"
#include "ComRegMap.h"

// --- Hauptklasse cBQ25798 ---

struct cBQ25798_RegisterMap
{
  // Enum für die Register-Startadressen (Offsets)
  typedef enum {
      nREG00_MIN_SYS_VOLTAGE    = 0, // Minimal System Voltage
      nREG01_CHG_VOLTAGE_LIMIT,   // Charge Voltage Limit (2 Bytes)
      nREG03_CHG_CURRENT_LIMIT,   // Charge Current Limit (2 Bytes)
      nREG05_INPUT_VOLTAGE_LIMIT, // Input Voltage Limit
      nREG06_INPUT_CURRENT_LIMIT, // Input Current Limit (2 Bytes)
      nREG08_PRECHARGE_CTRL,      // Precharge Control
      nREG09_TERM_CTRL,           // Termination Control
      nREG0A_RECHARGE_CTRL,       // Re-charge Control
      nREG0B_VOTG_REGULATION,     // VOTG regulation (2 Bytes)
      nREG0D_IOTG_REGULATION,     // IOTG regulation

      nREG0E_TIMER_CTRL,          // Timer Control
      nREG0F_CHARGER_CTRL_0,      // Charger Control 0
      nREG10_CHARGER_CTRL_1,      // Charger Control 1
      nREG11_CHARGER_CTRL_2,      // Charger Control 2
      nREG12_CHARGER_CTRL_3,      // Charger Control 3
      nREG13_CHARGER_CTRL_4,      // Charger Control 4
      nREG14_CHARGER_CTRL_5,      // Charger Control 5
      nREG15_MPPT_CTRL,           // MPPT Control
      nREG16_TEMP_CTRL,           // Temperature Control
      nREG17_NTC_CTRL_0,          // NTC Control 0

      nREG18_NTC_CTRL_1,          // NTC Control 1
      nREG19_ICO_CURRENT_LIMIT,   // ICO Current Limit (2 Bytes)
      nREG1B_CHARGER_STATUS_0,    // Charger Status 0
      nREG1C_CHARGER_STATUS_1,    // Charger Status 1
      nREG1D_CHARGER_STATUS_2,    // Charger Status 2
      nREG1E_CHARGER_STATUS_3,    // Charger Status 3
      nREG1F_CHARGER_STATUS_4,    // Charger Status 4
      nREG20_FAULT_STATUS_0,      // FAULT Status 0
      nREG21_FAULT_STATUS_1,      // FAULT Status 1
      nREG22_CHARGER_FLAG_0,      // Charger Flag 0
      nREG23_CHARGER_FLAG_1,      // Charger Flag 1

      nREG24_CHARGER_FLAG_2,      // Charger Flag 2
      nREG25_CHARGER_FLAG_3,      // Charger Flag 3
      nREG26_FAULT_FLAG_0,        // FAULT Flag 0
      nREG27_FAULT_FLAG_1,        // FAULT Flag 1
      nREG28_CHARGER_MASK_0,      // Charger Mask 0
      nREG29_CHARGER_MASK_1,      // Charger Mask 1
      nREG2A_CHARGER_MASK_2,      // Charger Mask 2
      nREG2B_CHARGER_MASK_3,      // Charger Mask 3
      nREG2C_FAULT_MASK_0,        // FAULT Mask 0
      nREG2D_FAULT_MASK_1,        // FAULT Mask 1

      nREG2E_ADC_CONTROL,         // ADC Control
      nREG2F_ADC_FUNC_DIS_0,      // ADC Function Disable 0
      nREG30_ADC_FUNC_DIS_1,      // ADC Function Disable 1
      nREG31_IBUS_ADC,            // IBUS ADC (2 Bytes)
      nREG33_IBAT_ADC,            // IBAT ADC (2 Bytes)
      nREG35_VBUS_ADC,            // VBUS ADC (2 Bytes)
      nREG37_VAC1_ADC,            // VAC1 ADC (2 Bytes)
      nREG39_VAC2_ADC,            // VAC2 ADC (2 Bytes)
      nREG3B_VBAT_ADC,            // VBAT ADC (2 Bytes)
      nREG3D_VSYS_ADC,            // VSYS ADC (2 Bytes)

      nREG3F_TS_ADC,              // TS ADC (2 Bytes)
      nREG41_TDIE_ADC,            // TDIE ADC (2 Bytes)
      nREG43_DPLUS_ADC,           // D+ ADC (2 Bytes)
      nREG45_DMINUS_ADC,          // D- ADC (2 Bytes)
      nREG47_DPDM_DRIVER,         // DPDM Driver
      nREG48_PART_INFORMATION,    // Part Information

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
      { 0x00,  1,   0x10,   nACCESS_RW }, // nREG00_MIN_SYS_VOLTAGE
      { 0x01,  2,   0x0334, nACCESS_RW }, // nREG01_CHG_VOLTAGE_LIMIT (16 Bit)
      { 0x03,  2,   0x0028, nACCESS_RW }, // nREG03_CHG_CURRENT_LIMIT (16 Bit)
      { 0x05,  1,   0x24,   nACCESS_RW }, // nREG05_INPUT_VOLTAGE_LIMIT
      { 0x06,  2,   0x012C, nACCESS_RW }, // nREG06_INPUT_CURRENT_LIMIT (16 Bit)
      { 0x08,  1,   0x4C,   nACCESS_RW }, // nREG08_PRECHARGE_CTRL
      { 0x09,  1,   0x05,   nACCESS_RW }, // nREG09_TERM_CTRL
      { 0x0A,  1,   0xA3,   nACCESS_RW }, // nREG0A_RECHARGE_CTRL
      { 0x0B,  2,   0x00DC, nACCESS_RW }, // nREG0B_VOTG_REGULATION    (16 Bit)
      { 0x0D,  1,   0x4B,   nACCESS_RW }, // nREG0D_IOTG_REGULATION

      { 0x0E,  1,   0xBD,   nACCESS_RW }, // nREG0E_TIMER_CTRL
      { 0x0F,  1,   0xA2,   nACCESS_RW }, // nREG0F_CHARGER_CTRL_0
      { 0x10,  1,   0x30,   nACCESS_RW }, // nREG10_CHARGER_CTRL_1
      { 0x11,  1,   0x00,   nACCESS_RW }, // nREG11_CHARGER_CTRL_2
      { 0x12,  1,   0x40,   nACCESS_RW }, // nREG12_CHARGER_CTRL_3
      { 0x13,  1,   0x00,   nACCESS_RW }, // nREG13_CHARGER_CTRL_4
      { 0x14,  1,   0x00,   nACCESS_RW }, // nREG14_CHARGER_CTRL_5
      { 0x15,  1,   0xAA,   nACCESS_RW }, // nREG15_MPPT_CTRL
      { 0x16,  1,   0xC0,   nACCESS_RW }, // nREG16_TEMP_CTRL
      { 0x17,  1,   0x3F,   nACCESS_RW },  // nREG17_NTC_CTRL_0

      { 0x18,  1,   0x00,   nACCESS_RW }, // nREG18_NTC_CTRL_1
      { 0x19,  2,   0x0000, nACCESS_RO }, // nREG19_ICO_CURRENT_LIMIT  (16 Bit, deckt 0x19 & 0x1A ab)
      { 0x1B,  1,   0x00,   nACCESS_RO }, // nREG1B_CHARGER_STATUS_0
      { 0x1C,  1,   0x00,   nACCESS_RO }, // nREG1C_CHARGER_STATUS_1
      { 0x1D,  1,   0x00,   nACCESS_RO }, // nREG1D_CHARGER_STATUS_2
      { 0x1E,  1,   0x00,   nACCESS_RO }, // nREG1E_CHARGER_STATUS_3
      { 0x1F,  1,   0x00,   nACCESS_RO }, // nREG1F_CHARGER_STATUS_4
      { 0x20,  1,   0x00,   nACCESS_RO }, // nREG20_FAULT_STATUS_0
      { 0x21,  1,   0x00,   nACCESS_RO }, // nREG21_FAULT_STATUS_1
      { 0x22,  1,   0x00,   nACCESS_RO }, // nREG22_CHARGER_FLAG_0

      { 0x23,  1,   0x00,   nACCESS_RO }, // nREG23_CHARGER_FLAG_1
      { 0x24,  1,   0x00,   nACCESS_RO }, // nREG24_CHARGER_FLAG_2
      { 0x25,  1,   0x00,   nACCESS_RO }, // nREG25_CHARGER_FLAG_3
      { 0x26,  1,   0x00,   nACCESS_RO }, // nREG26_FAULT_FLAG_0
      { 0x27,  1,   0x00,   nACCESS_RO }, // nREG27_FAULT_FLAG_1
      { 0x28,  1,   0x00,   nACCESS_RW }, // nREG28_CHARGER_MASK_0
      { 0x29,  1,   0x00,   nACCESS_RW }, // nREG29_CHARGER_MASK_1
      { 0x2A,  1,   0x00,   nACCESS_RW }, // nREG2A_CHARGER_MASK_2
      { 0x2B,  1,   0x00,   nACCESS_RW }, // nREG2B_CHARGER_MASK_3
      { 0x2C,  1,   0x00,   nACCESS_RW }, // nREG2C_FAULT_MASK_0

      { 0x2D,  1,   0x00,   nACCESS_RW }, // nREG2D_FAULT_MASK_1
      { 0x2E,  1,   0x00,   nACCESS_RW }, // nREG2E_ADC_CONTROL
      { 0x2F,  1,   0x00,   nACCESS_RW }, // nREG2F_ADC_FUNC_DIS_0
      { 0x30,  1,   0x00,   nACCESS_RW }, // nREG30_ADC_FUNC_DIS_1
      { 0x31,  2,   0x0000, nACCESS_RO }, // nREG31_IBUS_ADC          (16 Bit, deckt 0x31 & 0x32 ab)
      { 0x33,  2,   0x0000, nACCESS_RO }, // nREG33_IBAT_ADC          (16 Bit, deckt 0x33 & 0x34 ab)
      { 0x35,  2,   0x0000, nACCESS_RO }, // nREG35_VBUS_ADC          (16 Bit, deckt 0x35 & 0x36 ab)
      { 0x37,  2,   0x0000, nACCESS_RO }, // nREG37_VAC1_ADC          (16 Bit, deckt 0x37 & 0x38 ab)
      { 0x39,  2,   0x0000, nACCESS_RO }, // nREG39_VAC2_ADC          (16 Bit, deckt 0x39 & 0x3A ab)
      { 0x3B,  2,   0x0000, nACCESS_RO }, // nREG3B_VBAT_ADC          (16 Bit, deckt 0x3B & 0x3C ab)

      { 0x3D,  2,   0x0000, nACCESS_RO }, // nREG3D_VSYS_ADC          (16 Bit, deckt 0x3D & 0x3E ab)
      { 0x3F,  2,   0x0000, nACCESS_RO }, // nREG3F_TS_ADC            (16 Bit, deckt 0x3F & 0x40 ab)
      { 0x41,  2,   0x0000, nACCESS_RO }, // nREG41_TDIE_ADC          (16 Bit, deckt 0x41 & 0x42 ab)
      { 0x43,  2,   0x0000, nACCESS_RO }, // nREG43_DPLUS_ADC         (16 Bit, deckt 0x43 & 0x44 ab)
      { 0x45,  2,   0x0000, nACCESS_RO }, // nREG45_DMINUS_ADC        (16 Bit, deckt 0x45 & 0x46 ab)
      { 0x47,  1,   0x00,   nACCESS_RW }, // nREG47_DPDM_DRIVER
      { 0x48,  1,   0x19,   nACCESS_RO }  // nREG48_PART_INFORMATION
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

  /**
   * BQ25798 COMPLETE Register Map (Table 9-12)
   * Compiler: GCC (__attribute__((packed)))
   * Bit Order: Bit 0 (LSB) Top -> Bit 7 (MSB) Bottom
   */
  // 00h: Minimal System Voltage
  union {
      u8 u8Byte;
      struct {
          u8 VSYS_MIN : 6;
          u8 RESERVED : 2;
      } stBits;
  } tunREG00_Minimal_System_Voltage;

  // 01h: Charge Voltage Limit (16-bit)
  union {
      u16 u16Word;
      struct {
          u16 Hb : 8;
          u16 Lb : 8;
      } stBits;
  } tunREG01_Charge_Voltage_Limit;

  // 03h: Charge Current Limit (16-bit)
  union {
      u16 u16Word;
      struct {
          u16 Hb : 8;
          u16 Lb : 8;
      } stBits;
  } tunREG03_Charge_Current_Limit;

  // 05h: Input Voltage Limit
  union {
      u8 u8Byte;
      struct {
          u8 VINDPM : 8;
      } stBits;
  } tunREG05_Input_Voltage_Limit;

  // 06h: Input Current Limit (16-bit)
  union {
      u16 u16Word;
      struct {
          u16 Hb : 8;
          u16 Lb : 8;
      } stBits;
  } tunREG06_Input_Current_Limit;

  // 08h: Precharge Control
  union {
      u8 u8Byte;
      struct {
          u8 IPRECHG   : 6;
          u8 VBAT_LOWV : 2;
      } stBits;
  } tunREG08_Precharge_Control;

  // 09h: Termination Control
  union {
      u8 u8Byte;
      struct {
          u8 ITERM      : 5;
          u8 STOP_WD_EN : 1;
          u8 REG_RST    : 1; // Bit 7
          u8 RESERVED   : 1;
      } stBits;
  } tunREG09_Termination_Control;

  // 0Ah: Re-charge Control
  union {
      u8 u8Byte;
      struct {
          u8 VRECHG : 4;
          u8 TRECHG : 4;
          u8 CELL   : 4;
      } stBits;
  } tunREG0A_Re_charge_Control;

  // 0Bh: VOTG Regulation (16-bit)
  union {
      u16 u16Word;
      struct {
          u16 VOTG_0 : 1; u16 VOTG_1 : 1; u16 VOTG_2 : 1; u16 VOTG_3 : 1;
          u16 VOTG_4 : 1; u16 VOTG_5 : 1; u16 VOTG_6 : 1; u16 VOTG_7 : 1;
          u16 VOTG_8 : 1; u16 VOTG_9 : 1; u16 VOTG_10 : 1;
          u16 RESERVED : 5;
      } stBits;
  } tunREG0B_VOTG_regulation;

  // 0Dh: IOTG Regulation
  union {
      u8 u8Byte;
      struct {
          u8 IOTG_0 : 1; u8 IOTG_1 : 1; u8 IOTG_2 : 1; u8 IOTG_3 : 1;
          u8 IOTG_4 : 1; u8 IOTG_5 : 1; u8 IOTG_6 : 1;
          u8 RESERVED : 1;
      } stBits;
  } tunREG0D_IOTG_regulation;

  // 0Eh: Timer Control
  union {
      u8 u8Byte;
      struct {
          u8 TOPOFF_TIMER : 2;
          u8 CHG_TIMER    : 2;
          u8 WDT          : 2;
          u8 EN_TOPOFF    : 1;
          u8 EN_CHG_TIMER : 1;
      } stBits;
  } tunREG0E_Timer_Control;

  // 0Fh: Charger Control 0
  union {
      u8 u8Byte;
      struct {
          u8 EN_BACKUP       : 1; // Bit 0: Ermöglicht den Backup-Modus (OTG aktiviert sich automatisch bei VBUS-Abfall) [0=Disable (Def.), 1=Enable]
          u8 EN_TERM         : 1; // Bit 1: Ladeterminierung aktivieren [0=Disable, 1=Enable (Def.)]
          u8 EN_HIZ          : 1; // Bit 2: HIZ-Modus aktivieren (wird bei VBUS-Plug-in automatisch auf 0 gesetzt) [0=Disable (Def.), 1=Enable]
          u8 FORCE_ICO       : 1; // Bit 3: Start des Input Current Optimizer (ICO) erzwingen (Bit setzt sich nach Start selbst zurück) [0=Normal (Def.), 1=Force ICO]
          u8 EN_ICO          : 1; // Bit 4: Input Current Optimizer (ICO) aktivieren [0=Disable (Def.), 1=Enable]
          u8 EN_CHG          : 1; // Bit 5: Laden aktivieren [0=Charge Disable, 1=Charge Enable (Def.)]
          u8 FORCE_IBATDIS   : 1; // Bit 6: Batterientladestrom erzwingen (unabhängig vom OVP-Status) [0=IDLE (Def.), 1=Force discharge]
          u8 EN_AUTO_IBATDIS : 1; // Bit 7: Automatische Batterieentladung bei Batterie-Überspannung (OVP) aktivieren [0=Disable, 1=Enable (Def.)]
      } stBits;
  } tunREG0F_Charger_Control_0;

  // 10h: Charger Control 1
  union {
      u8 u8Byte;
      struct {
          u8 WD_MODE     : 3;
          u8 WD_RST      : 1;
          u8 VAC_OVP     : 2;
          u8 VBUS_BACKUP : 2;
      } stBits;
  } tunREG10_Charger_Control_1;

  // 11h: Charger Control 2
  union {
      u8 u8Byte;
      struct {
          u8 EN_EXTLOAD       : 1;
          u8 EN_BAT_UVLO      : 1;
          u8 VBUS_VREG_PROMPT : 1;
          u8 EN_VBUS_VREG     : 1;
          u8 FORCE_VBUS_VREG  : 1;
          u8 EN_VINDPM_TRACK  : 2;
          u8 EN_BATFET_RST    : 1;
      } stBits;
  } tunREG11_Charger_Control_2;

  // 12h: Charger Control 3
  union {
      u8 u8Byte;
      struct {
          u8 BATFET_DLY       : 1;
          u8 BATFET_OFF       : 1;
          u8 BATFET_CTRL_WVBUS: 1;
          u8 DIS_STAT         : 1;
          u8 EN_INT_IBAT      : 1;
          u8 RESERVED         : 1;
          u8 DIS_SW           : 1;
          u8 PFM_OTG_DIS      : 1;
      } stBits;
  } tunREG12_Charger_Control_3;

  // 13h: Charger Control 4
  union {
      u8 u8Byte;
      struct {
          u8 EN_IBUS_OCP       : 1; // Bit 0: 1=IBUS Überstromschutz aktivieren (Default), 0=Deaktivieren [cite: 94]
          u8 FORCE_VINDPM_DET  : 1; // Bit 1: 1=VINDPM-Erkennung erzwingen, 0=Normal (Default) [cite: 94]
          u8 DIS_VOTG_UVP      : 1; // Bit 2: 1=OTG Unterspannungsschutz deaktivieren, 0=Aktiv (Default) [cite: 94]
          u8 DIS_VSYS_SHORT    : 1; // Bit 3: 1=VSYS Kurzschlussschutz (Hiccup) deaktivieren, 0=Aktiv (Default) [cite: 94]
          u8 DIS_STAT          : 1; // Bit 4: 1=STAT-Pin Funktion deaktivieren, 0=Aktiv (Default) [cite: 94]
          u8 PWM_FREQ          : 1; // Bit 5: Schaltfrequenz: 0h=1.5 MHz, 1h=750 kHz (Default via PROG-Pin) [cite: 94, 97]
          u8 EN_ACDRV1         : 1; // Bit 6: 1=ACFET1-RBFET1 einschalten, 0=Aus (Default). Bei POR verriegelt auf 0 wenn kein FET erkannt [cite: 94, 95]
          u8 EN_ACDRV2         : 1; // Bit 7: 1=ACFET2-RBFET2 einschalten, 0=Aus (Default). Bei POR verriegelt auf 0 wenn kein FET erkannt [cite: 94, 95]
      } stBits;
  } tunREG13_Charger_Control_4;

  // 14h: Charger Control 5
  union {
      u8 u8Byte;
      struct {
          u8 EN_BATOC         : 1;
          u8 EN_EXTILIM       : 1;
          u8 EN_IINDPM        : 1;
          u8 IBAT_REG_1       : 2;
          u8 EN_IBAT          : 1;
          u8 RESERVED         : 1;
          u8 SFET_PRESENT     : 1;
      } stBits;
  } tunREG14_Charger_Control_5;

  // 15h: MPPT Control
  union {
      u8 u8Byte;
      struct {
          u8 EN_MPPT          : 1;
          u8 VOC_RATE         : 2;
          u8 VOC_DLY          : 2;
          u8 VOC_PCT          : 3;
      } stBits;
  } tunREG15_MPPT_Control;

  // 16h: Temperature Control
  union {
      u8 u8Byte;
      struct {
          u8 TS_REG_BIT       : 1;
          u8 TS_HOT_BIT       : 2;
          u8 TS_WARM_BIT      : 2;
          u8 TS_COOL_BIT      : 2;
          u8 TS_COLD_BIT      : 1;
      } stBits;
  } tunREG16_Temperature_Control;

  // 17h: NTC Control 0
  union {
      u8 u8Byte;
      struct {
          u8 JEITA_VSET       : 1;
          u8 JEITA_ISETPCT    : 3;
          u8 RESERVED         : 4;
      } stBits;
  } tunREG17_NTC_Control_0;

  // 18h: NTC Control 1
  union {
      u8 u8Byte;
      struct {
        // Bit 0: TS_IGNORE
            // 0h = NOT ignore (Default), 1h = Ignore
        u8 TS_IGNORE : 1;

        // Bit 1: BCOLD
        // OTG mode TS COLD temperature threshold
        // 0h = -10°C (Default), 1h = -20°C
        u8 BCOLD : 1;

        // Bits 2-3: BHOT_1:0
        // OTG mode TS HOT temperature threshold
        // 0h = 55°C, 1h = 60°C (Default), 2h = 65°C, 3h = Disable
        u8 BHOT : 2;

        // Bits 4-5: TS_WARM_1:0
        // JEITA VT3 comparator voltage falling thresholds
        // 0h = 48.4% (40°C), 1h = 44.8% (45°C) (Default), 2h = 41.2% (50°C), 3h = 37.7% (55°C)
        u8 TS_WARM : 2;

        // Bits 6-7: TS_COOL_1:0
        // JEITA VT2 comparator voltage rising thresholds
        // 0h = 71.1% (5°C), 1h = 68.4% (10°C) (Default), 2h = 65.5% (15°C), 3h = 62.4% (20°C)
        u8 TS_COOL : 2;
      } stBits;
  } tunREG18_NTC_Control_1;

  // 19h: ICO Current Limit (16-bit)
  union {
      u16 u16Word;
      struct {
          u16 ICO_ILIM_0 : 1; u16 ICO_ILIM_1 : 1; u16 ICO_ILIM_2 : 1; u16 ICO_ILIM_3 : 1;
          u16 ICO_ILIM_4 : 1; u16 ICO_ILIM_5 : 1; u16 ICO_ILIM_6 : 1; u16 ICO_ILIM_7 : 1;
          u16 ICO_ILIM_8 : 1;
          u16 RESERVED   : 7;
      } stBits;
  } tunREG19_ICO_Current_Limit;

  // 1Bh: Charger Status 0
  union {
      u8 u8Byte;
      struct {
          u8 VBUS_PRESENT_STAT : 1;
          u8 AC1_PRESENT_STAT  : 1;
          u8 AC2_PRESENT_STAT  : 1;
          u8 PG_STAT           : 1;
          u8 RESERVED          : 1;
          u8 WD_STAT           : 1;
          u8 VINDPM_STAT       : 1;
          u8 IINDPM_STAT       : 1;
      } stBits;
  } tunREG1B_Charger_Status_0;

  // 1Ch: Charger Status 1
  union {
      u8 u8Byte;
      struct {
        u8 BC12_DONE_STAT  : 1; // Bit 7: 1 = In IINDPM regulation
        u8 VBUS_STAT       : 4; // Bit 3-6: 0:No Input, 1:USB SDP, 3:USB CDP, 4:USB DCP, 11:Unknown, 15:OTG
        u8 CHG_STAT        : 3; // Bit 0-2: 0:Not Chg, 1:Trickle, 2:Pre, 3:Fast, 4:Term, 5:Top-off, 6:VINDPM, 7:IINDPM
      } stBits;
  } tunREG1C_Charger_Status_1;

  // 1Dh: Charger Status 2
  union {
      u8 u8Byte;
      struct {
          u8 VBAT_PRESENT_STAT : 1;
          u8 DPDM_STAT         : 1;
          u8 TREG_STAT         : 1;
          u8 RESERVED          : 3;
          u8 ICO_STAT          : 2;
      } stBits;
  } tunREG1D_Charger_Status_2;

  // 1Eh: Charger Status 3
  union {
      u8 u8Byte;
      struct {
          u8 RESERVED         : 1; // Bit 0: Reserviert
          u8 PRECHG_TMR_STAT  : 1; // Bit 1: Pre-charge timer status
          u8 TRICHG_TMR_STAT  : 1; // Bit 2: Trickle charge timer status
          u8 CHG_TMR_STAT     : 1; // Bit 3: Fast charge timer status
          u8 VSYS_STAT        : 1; // Bit 4: VSYS Regulation Status
          u8 ADC_DONE_STAT    : 1; // Bit 5: ADC Conversion Status
          u8 ACRB1_STAT       : 1; // Bit 6: ACFET1-RBFET1 status
          u8 ACRB2_STAT       : 1; // Bit 7: ACFET2-RBFET2 status
      } stBits;
  } tunREG1E_Charger_Status_3;

  // 1Fh: Charger Status 4
  union {
      u8 u8Byte;
      struct {
          u8 TS_HOT_STAT      : 1;
          u8 TS_WARM_STAT     : 1;
          u8 TS_COOL_STAT     : 1;
          u8 TS_COLD_STAT     : 1;
          u8 VBATOTG_LOW_STAT : 1;
          u8 RESERVED         : 3;
      } stBits;
  } tunREG1F_Charger_Status_4;

  // 20h: FAULT Status 0
  union {
      u8 u8Byte;
      struct {
          u8 VAC1_OVP_STAT  : 1; // Bit 0: 1 = VAC1 Over-voltage protection
          u8 VAC2_OVP_STAT  : 1; // Bit 1: 1 = VAC2 Over-voltage protection
          u8 CONV_OCP_STAT  : 1; // Bit 2: 1 = Converter Over-current protection
          u8 IBAT_OCP_STAT  : 1; // Bit 3: 1 = IBAT Over-current protection
          u8 IBUS_OCP_STAT  : 1; // Bit 4: 1 = IBUS Over-current protection
          u8 VBAT_OVP_STAT  : 1; // Bit 5: 1 = VBAT Over-voltage protection
          u8 VBUS_OVP_STAT  : 1; // Bit 6: 1 = VBUS Over-voltage protection
          u8 IBAT_REG_STAT  : 1; // Bit 7: 1 = In battery discharging current regulation
      } stBits;
  } tunREG20_FAULT_Status_0;

  // 21h: FAULT Status 1
  union {
      u8 u8Byte;
      struct {
          u8 RESERVED      : 2;
          u8 TSHUT_STAT    : 1;
          u8 RESERVED2     : 1;
          u8 OTG_UVP_STAT  : 1;
          u8 OTG_OVP_STAT  : 1;
          u8 VSYS_OVP_STAT : 1;
          u8 VSYS_SHORT_STAT: 1;
      } stBits;
  } tunREG21_FAULT_Status_1;

  // 22h: Charger Flag 0
  union {
      u8 u8Byte;
      struct {
          u8 VBUS_PRESENT_FLAG : 1;
          u8 AC1_PRESENT_FLAG  : 1;
          u8 AC2_PRESENT_FLAG  : 1;
          u8 PG_FLAG           : 1;
          u8 POORSRC_FLAG      : 1;
          u8 WD_FLAG           : 1;
          u8 VINDPM_FLAG       : 1;
          u8 IINDPM_FLAG       : 1;
      } stBits;
  } tunREG22_Charger_Flag_0;

  // 23h: Charger Flag 1
  union {
      u8 u8Byte;
      struct {
          u8 CHG_FLAG         : 1;
          u8 ICO_FLAG         : 1;
          u8 RESERVED         : 1;
          u8 VBUS_FLAG        : 1;
          u8 RESERVED2        : 1;
          u8 TREG_FLAG        : 1;
          u8 VBAT_PRESENT_FLAG: 1;
          u8 BC12_DONE_FLAG   : 1;
      } stBits;
  } tunREG23_Charger_Flag_1;

  // 24h: Charger Flag 2
  union  {
      u8 u8Byte;
      struct {
          u8 TOPOFF_TMR_FLAG  : 1; // Bit 0: 1 = Top-off timer expired
          u8 PRECHG_TMR_FLAG  : 1; // Bit 1: 1 = Pre-charge timer expired
          u8 TRICHG_TMR_FLAG  : 1; // Bit 2: 1 = Trickle charge timer expired
          u8 CHG_TMR_FLAG     : 1; // Bit 3: 1 = Fast charge timer expired
          u8 VSYS_FLAG        : 1; // Bit 4: 1 = Entered/Exited VSYSMIN regulation
          u8 ADC_DONE_FLAG    : 1; // Bit 5: 1 = ADC conversion completed
          u8 DPDM_DONE_FLAG   : 1; // Bit 6: 1 = D+/D- detection completed
          u8 RESERVED         : 1; // Bit 7: Reserviert
      } stBits;
  } tunREG24_Charger_Flag_2;

  // 25h: Charger Flag 3
  union {
      u8 u8Byte;
      struct {
          u8 TS_HOT_FLAG      : 1;
          u8 TS_WARM_FLAG     : 1;
          u8 TS_COOL_FLAG     : 1;
          u8 TS_COLD_FLAG     : 1;
          u8 VBATOTG_LOW_FLAG : 1;
          u8 RESERVED         : 3;
      } stBits;
  } tunREG25_Charger_Flag_3;

  // 26h: FAULT Flag 0
  union {
      u8 u8Byte;
      struct {
          u8 VAC1_OVP_FLAG : 1;
          u8 VAC2_OVP_FLAG : 1;
          u8 CONV_OCP_FLAG : 1;
          u8 IBAT_OCP_FLAG : 1;
          u8 IBUS_OCP_FLAG : 1;
          u8 VBAT_OVP_FLAG : 1;
          u8 VBUS_OVP_FLAG : 1;
          u8 IBAT_REG_FLAG : 1;
      } stBits;
  } tunREG26_FAULT_Flag_0;

  // 27h: FAULT Flag 1
  union {
      u8 u8Byte;
      struct {
          u8 RESERVED      : 2;
          u8 TSHUT_FLAG    : 1;
          u8 RESERVED2     : 1;
          u8 OTG_UVP_FLAG  : 1;
          u8 OTG_OVP_FLAG  : 1;
          u8 VSYS_OVP_FLAG : 1;
          u8 VSYS_SHORT_FLAG: 1;
      } stBits;
  } tunREG27_FAULT_Flag_1;

  // 28h: Charger Mask 0
  union {
      u8 u8Byte;
      struct {
          u8 VBUS_PRESENT_MASK : 1;
          u8 AC1_PRESENT_MASK  : 1;
          u8 AC2_PRESENT_MASK  : 1;
          u8 PG_MASK           : 1;
          u8 POORSRC_MASK      : 1;
          u8 WD_MASK           : 1;
          u8 VINDPM_MASK       : 1;
          u8 IINDPM_MASK       : 1;
      } stBits;
  } tunREG28_Charger_Mask_0;

  // 29h: Charger Mask 1
  union {
      u8 u8Byte;
      struct {
          u8 CHG_MASK         : 1;
          u8 ICO_MASK         : 1;
          u8 RESERVED         : 1;
          u8 VBUS_MASK        : 1;
          u8 RESERVED2        : 1;
          u8 TREG_MASK        : 1;
          u8 VBAT_PRESENT_MASK: 1;
          u8 BC12_DONE_MASK   : 1;
      } stBits;
  } tunREG29_Charger_Mask_1;

  // 2Ah: Charger Mask 2
  union {
      u8 u8Byte;
      struct {
          u8 DPDM_DONE_MASK   : 1;
          u8 ADC_DONE_MASK    : 1;
          u8 VSYS_MASK        : 1;
          u8 CHG_TMR_MASK     : 1;
          u8 TRICHG_TMR_MASK  : 1;
          u8 PRECHG_TMR_MASK  : 1;
          u8 TOPOFF_TMR_MASK  : 1;
          u8 RESERVED         : 1;
      } stBits;
  } tunREG2A_Charger_Mask_2;

  // 2Bh: Charger Mask 3
  union {
      u8 u8Byte;
      struct {
          u8 TS_HOT_MASK      : 1;
          u8 TS_WARM_MASK     : 1;
          u8 TS_COOL_MASK     : 1;
          u8 TS_COLD_MASK     : 1;
          u8 VBATOTG_LOW_MASK : 1;
          u8 RESERVED         : 3;
      } stBits;
  } tunREG2B_Charger_Mask_3;

  // 2Ch: FAULT Mask 0
  union {
      u8 u8Byte;
      struct {
          u8 VAC1_OVP_MASK : 1;
          u8 VAC2_OVP_MASK : 1;
          u8 CONV_OCP_MASK : 1;
          u8 IBAT_OCP_MASK : 1;
          u8 IBUS_OCP_MASK : 1;
          u8 VBAT_OVP_MASK : 1;
          u8 VBUS_OVP_MASK : 1;
          u8 IBAT_REG_MASK : 1;
      } stBits;
  } tunREG2C_FAULT_Mask_0;

  // 2Dh: FAULT Mask 1
  union {
      u8 u8Byte;
      struct {
          u8 RESERVED      : 2;
          u8 TSHUT_MASK    : 1;
          u8 RESERVED2     : 1;
          u8 OTG_UVP_MASK  : 1;
          u8 OTG_OVP_MASK  : 1;
          u8 VSYS_OVP_MASK : 1;
          u8 VSYS_SHORT_MASK: 1;
      } stBits;
  } tunREG2D_FAULT_Mask_1;

  // 2Eh: ADC Control
  union {
      u8 u8Byte;
      struct {
          u8 RESERVED      : 2; // Bit 0-1: Reserviert [Default: 00b]
          u8 ADC_AVG_INIT  : 1; // Bit 2: ADC Durchschnitt-Initialisierung [0=Bestehenden Wert nutzen (Def.), 1=Mit neuer Konvertierung starten]
          u8 ADC_AVG       : 1; // Bit 3: ADC Durchschnitts-Steuerung [0=Einzelwert (Def.), 1=Laufender Durchschnitt (nicht für IBAT Entladung)]
          u8 ADC_SAMPLE    : 2; // Bit 4-5: ADC Abtastgeschwindigkeit/Auflösung: 0h=15-bit, 1h=14-bit, 2h=13-bit, 3h=12-bit (Def., nicht empfohlen)
          u8 ADC_RATE      : 1; // Bit 6: ADC Konvertierungsrate: 0=Kontinuierlich (Def.), 1=One-Shot
          u8 ADC_EN        : 1; // Bit 7: ADC Steuerung: 0=Deaktiviert (Def.), 1=Aktiviert
     } stBits;
  } tunREG2E_ADC_Control;

  // 2Fh: ADC Function Disable 0
  union {
      u8 u8Byte;
      struct {
          u8 IBAT_ADC_DIS  : 1;
          u8 IBUS_ADC_DIS  : 1;
          u8 VBUS_ADC_DIS  : 1;
          u8 VBAT_ADC_DIS  : 1;
          u8 VSYS_ADC_DIS  : 1;
          u8 TS_ADC_DIS    : 1;
          u8 TDIE_ADC_DIS  : 1;
          u8 RESERVED      : 1;
      } stBits;
  } tunREG2F_ADC_Function_Disable_0;

  // 30h: ADC Function Disable 1
  union {
      u8 u8Byte;
      struct {
          u8 RESERVED      : 4;
          u8 VAC1_ADC_DIS  : 1;
          u8 VAC2_ADC_DIS  : 1;
          u8 DM_ADC_DIS    : 1;
          u8 DP_ADC_DIS    : 1;
      } stBits;
  } tunREG30_ADC_Function_Disable_1;

   // 31h: IBUS ADC (Input Current)
  union {
    i16 i16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mA
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG31_IBUS_ADC;

  // 33h: IBAT ADC (Battery Charge/Discharge Current)
  union {
    i16 i16Word;       // Signed, da Entladestrom negativ sein kann
    struct {
      // Bit 0-15: 1 LSB = 1mA (Zweierkomplement)
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG33_IBAT_ADC;

  // 35h: VBUS ADC (VBUS Voltage)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mV
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG35_VBUS_ADC;

  // 37h: VAC1 ADC (Voltage at VAC1)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mV
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG37_VAC1_ADC;

  // 39h: VAC2 ADC (Voltage at VAC2)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mV
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG39_VAC2_ADC;

  // 3Bh: VBAT ADC (Battery Voltage)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mV
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG3B_VBAT_ADC;

  // 3Dh: VSYS ADC (System Voltage)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mV
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG3D_VSYS_ADC;

  // 3Fh: TS ADC (Temperature Sense Pin Voltage)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: Prozentsatz von REGN (oder mV je nach Einstellung)
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG3F_TS_ADC;

  // 41h: TDIE ADC (Die Temperature)
  union {
    i16 i16Word;       // Signed für Temperaturen
    struct {
      // Bit 0-15: 1 LSB = 0.5°C (Offset beachten)
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG41_TDIE_ADC;

  // 43h: DP ADC (D+ Pin Voltage)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mV
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG43_DP_ADC;

  // 45h: DM ADC (D- Pin Voltage)
  union {
    u16 u16Word;
    struct {
      // Bit 0-15: 1 LSB = 1mV
      u16 Hb : 8;
      u16 Lb : 8;
    } stBits;
  } tunREG45_DM_ADC;

  // 47h: DPDM Driver
  union {
      u8 u8Byte;
      struct {
          u8 DMINUS_VSEL   : 2;
          u8 DPLUS_VSEL    : 2;
          u8 DPDM_DRV_EN   : 1;
          u8 RESERVED      : 3;
      } stBits;
  } tunREG47_DPDM_Driver;

  // 48h: Part Information
  union {
      u8 u8Byte;
      struct {
          u8 DEV_REV       : 3;
          u8 PN            : 3;
          u8 RESERVED      : 2;
      } stBits;
  } tunREG48_Part_Information;


  // Registermap 00h bis 48h
  // --- Configuration Registers ---
  u8* pu8GetMem(u8 lu8Idx)
  {
    switch ((tenRegIndex)lu8Idx)
    {
      case nREG00_MIN_SYS_VOLTAGE:     return (u8*)&tunREG00_Minimal_System_Voltage; break;
      case nREG01_CHG_VOLTAGE_LIMIT:   return (u8*)&tunREG01_Charge_Voltage_Limit;   break;
      case nREG03_CHG_CURRENT_LIMIT:   return (u8*)&tunREG03_Charge_Current_Limit;   break;
      case nREG05_INPUT_VOLTAGE_LIMIT: return (u8*)&tunREG05_Input_Voltage_Limit;    break;
      case nREG06_INPUT_CURRENT_LIMIT: return (u8*)&tunREG06_Input_Current_Limit;    break;
      case nREG08_PRECHARGE_CTRL:      return (u8*)&tunREG08_Precharge_Control;      break;
      case nREG09_TERM_CTRL:           return (u8*)&tunREG09_Termination_Control;    break;
      case nREG0A_RECHARGE_CTRL:       return (u8*)&tunREG0A_Re_charge_Control;      break;
      case nREG0B_VOTG_REGULATION:     return (u8*)&tunREG0B_VOTG_regulation;        break;
      case nREG0D_IOTG_REGULATION:     return (u8*)&tunREG0D_IOTG_regulation;        break;
      case nREG0E_TIMER_CTRL:          return (u8*)&tunREG0E_Timer_Control;          break;
      case nREG0F_CHARGER_CTRL_0:      return (u8*)&tunREG0F_Charger_Control_0;      break;
      case nREG10_CHARGER_CTRL_1:      return (u8*)&tunREG10_Charger_Control_1;      break;
      case nREG11_CHARGER_CTRL_2:      return (u8*)&tunREG11_Charger_Control_2;      break;
      case nREG12_CHARGER_CTRL_3:      return (u8*)&tunREG12_Charger_Control_3;      break;
      case nREG13_CHARGER_CTRL_4:      return (u8*)&tunREG13_Charger_Control_4;      break;
      case nREG14_CHARGER_CTRL_5:      return (u8*)&tunREG14_Charger_Control_5;      break;
      case nREG15_MPPT_CTRL:           return (u8*)&tunREG15_MPPT_Control;           break;
      case nREG16_TEMP_CTRL:           return (u8*)&tunREG16_Temperature_Control;    break;
      case nREG17_NTC_CTRL_0:          return (u8*)&tunREG17_NTC_Control_0;          break;
      case nREG18_NTC_CTRL_1:          return (u8*)&tunREG18_NTC_Control_1;          break;
      case nREG19_ICO_CURRENT_LIMIT:   return (u8*)&tunREG19_ICO_Current_Limit;      break;
  
      // --- Status Registers ---
      case nREG1B_CHARGER_STATUS_0: return (u8*)&tunREG1B_Charger_Status_0; break;
      case nREG1C_CHARGER_STATUS_1: return (u8*)&tunREG1C_Charger_Status_1; break;
      case nREG1D_CHARGER_STATUS_2: return (u8*)&tunREG1D_Charger_Status_2; break;
      case nREG1E_CHARGER_STATUS_3: return (u8*)&tunREG1E_Charger_Status_3; break;
      case nREG1F_CHARGER_STATUS_4: return (u8*)&tunREG1F_Charger_Status_4; break;
      case nREG20_FAULT_STATUS_0:   return (u8*)&tunREG20_FAULT_Status_0;   break;
      case nREG21_FAULT_STATUS_1:   return (u8*)&tunREG21_FAULT_Status_1;   break;

      // --- Flag Registers ---
      case nREG22_CHARGER_FLAG_0: return (u8*)&tunREG22_Charger_Flag_0; break;
      case nREG23_CHARGER_FLAG_1: return (u8*)&tunREG23_Charger_Flag_1; break;
      case nREG24_CHARGER_FLAG_2: return (u8*)&tunREG24_Charger_Flag_2; break;
      case nREG25_CHARGER_FLAG_3: return (u8*)&tunREG25_Charger_Flag_3; break;
      case nREG26_FAULT_FLAG_0:   return (u8*)&tunREG26_FAULT_Flag_0;   break;
      case nREG27_FAULT_FLAG_1:   return (u8*)&tunREG27_FAULT_Flag_1;   break;
  
      // --- Mask Registers ---
      case nREG28_CHARGER_MASK_0: return (u8*)&tunREG28_Charger_Mask_0; break;
      case nREG29_CHARGER_MASK_1: return (u8*)&tunREG29_Charger_Mask_1; break;
      case nREG2A_CHARGER_MASK_2: return (u8*)&tunREG2A_Charger_Mask_2; break;
      case nREG2B_CHARGER_MASK_3: return (u8*)&tunREG2B_Charger_Mask_3; break;
      case nREG2C_FAULT_MASK_0:   return (u8*)&tunREG2C_FAULT_Mask_0;   break;
      case nREG2D_FAULT_MASK_1:   return (u8*)&tunREG2D_FAULT_Mask_1;   break;
  
      // --- ADC Control & Results ---
      case nREG2E_ADC_CONTROL:    return (u8*)&tunREG2E_ADC_Control;            break;
      case nREG2F_ADC_FUNC_DIS_0: return (u8*)&tunREG2F_ADC_Function_Disable_0; break;
      case nREG30_ADC_FUNC_DIS_1: return (u8*)&tunREG30_ADC_Function_Disable_1; break;
      case nREG31_IBUS_ADC:       return (u8*)&tunREG31_IBUS_ADC;               break;
      case nREG33_IBAT_ADC:       return (u8*)&tunREG33_IBAT_ADC;               break;
      case nREG35_VBUS_ADC:       return (u8*)&tunREG35_VBUS_ADC;               break;
      case nREG37_VAC1_ADC:       return (u8*)&tunREG37_VAC1_ADC;               break;
      case nREG39_VAC2_ADC:       return (u8*)&tunREG39_VAC2_ADC;               break;
      case nREG3B_VBAT_ADC:       return (u8*)&tunREG3B_VBAT_ADC;               break;
      case nREG3D_VSYS_ADC:       return (u8*)&tunREG3D_VSYS_ADC;               break;
      case nREG3F_TS_ADC:         return (u8*)&tunREG3F_TS_ADC;                 break;
      case nREG41_TDIE_ADC:       return (u8*)&tunREG41_TDIE_ADC;               break;
      case nREG43_DPLUS_ADC:      return (u8*)&tunREG43_DP_ADC;                 break;
      case nREG45_DMINUS_ADC:     return (u8*)&tunREG45_DM_ADC;                 break;
  
      // --- Device Info ---
      case nREG47_DPDM_DRIVER:      return (u8*)&tunREG47_DPDM_Driver; break;
      case nREG48_PART_INFORMATION: return (u8*)&tunREG48_Part_Information; break;
      default:                      return (u8*)0;
    }
  }

  class cSEQ01_Init1 : public cComSeq
  {
    public:
  
    const cEntry macRegList[1] = { cRead(nREGXX_ALL) };
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ06_DeInit : public cComSeq
  {
    public:
    // To deactivate ADC and EN_IBAT
    const cEntry macRegList[2] = { cWrite(nREG2E_ADC_CONTROL), cWrite(nREG14_CHARGER_CTRL_5) };
  
    cEntry* pacGet() override
    {
       return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };
  
  class cSEQ07_WriteControlLimits : public cComSeq
  {
    public:
  
    const cEntry macRegList[6] = { cWrite(nREG00_MIN_SYS_VOLTAGE), cWrite(nREG01_CHG_VOLTAGE_LIMIT), cWrite(nREG03_CHG_CURRENT_LIMIT),
                                   cWrite(nREG05_INPUT_VOLTAGE_LIMIT), cWrite(nREG06_INPUT_CURRENT_LIMIT), cWrite(nREG19_ICO_CURRENT_LIMIT) };
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ08_WriteControlCharge : public cComSeq
  {
    public:
  
    const cEntry macRegList[3] = { cWrite(nREG08_PRECHARGE_CTRL), cWrite(nREG09_TERM_CTRL) , cWrite(nREG0A_RECHARGE_CTRL) };
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };


  class cSEQ09_WriteControlToGo : public cComSeq
  {
    public:
  
    const cEntry macRegList[2] = { cWrite(nREG0B_VOTG_REGULATION), cWrite(nREG0D_IOTG_REGULATION) };
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ10_WriteControlTimer : public cComSeq
  {
    public:
  
    const cEntry macRegList[1] = { cWrite(nREG0E_TIMER_CTRL)};
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ11_WriteControlCharger : public cComSeq
  {
    public:
  
    const cEntry macRegList[6] = { cWrite(nREG0F_CHARGER_CTRL_0), cWrite(nREG10_CHARGER_CTRL_1),
                                   cWrite(nREG11_CHARGER_CTRL_2), cWrite(nREG12_CHARGER_CTRL_3),
                                   cWrite(nREG13_CHARGER_CTRL_4), cWrite(nREG14_CHARGER_CTRL_5) };
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ12_WriteControlMppt : public cComSeq
  {
    public:
  
    const cEntry macRegList[1] = { cWrite(nREG15_MPPT_CTRL)};
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ13_WriteControlTemp : public cComSeq
  {
    public:
  
    const cEntry macRegList[3] = { cWrite(nREG16_TEMP_CTRL), cWrite(nREG17_NTC_CTRL_0), cWrite(nREG18_NTC_CTRL_1) };
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ14_WriteControlAdc : public cComSeq
  {
    public:
  
    const cEntry macRegList[1] = { cWrite(nREG2E_ADC_CONTROL)};
  
    cEntry* pacGet() override
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen() override
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ15_ReadStatusCharger : public cComSeq
  {
    public:
  
    const cEntry macRegList[5] = { cRead(nREG1B_CHARGER_STATUS_0), cRead(nREG1C_CHARGER_STATUS_1),
                                   cRead(nREG1D_CHARGER_STATUS_2), cRead(nREG1E_CHARGER_STATUS_3),
                                   cRead(nREG1F_CHARGER_STATUS_4) };
  
    cEntry* pacGet()
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen()
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ16_ReadStatusFaults : public cComSeq
  {
    public:
  
    const cEntry macRegList[2] = { cRead(nREG20_FAULT_STATUS_0), cRead(nREG21_FAULT_STATUS_1) };
  
    cEntry* pacGet()
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen()
    {
      return sizeof(macRegList);
    }
  };

  class cSEQ17_ReadStatusAdc : public cComSeq
  {
    public:
  
    const cEntry macRegList[11] = { cRead(nREG31_IBUS_ADC), cRead(nREG33_IBAT_ADC),
                                    cRead(nREG35_VBUS_ADC), cRead(nREG37_VAC1_ADC),
                                    cRead(nREG39_VAC2_ADC), cRead(nREG3B_VBAT_ADC),
                                    cRead(nREG3D_VSYS_ADC), cRead(nREG3F_TS_ADC),
                                    cRead(nREG41_TDIE_ADC), cRead(nREG43_DPLUS_ADC),
                                    cRead(nREG45_DMINUS_ADC) };
  
    cEntry* pacGet()
    {
      return (cEntry*) macRegList;
    }
  
    u8 u8GetLen()
    {
      return sizeof(macRegList);
    }
  };
  
  cSEQ01_Init1      mcSEQ01_Init1;
  cSEQ06_DeInit     mcSEQ06_DeInit;

  cSEQ07_WriteControlLimits  mcSEQ07_WriteControlLimits;
  cSEQ08_WriteControlCharge  mcSEQ08_WriteControlCharge;
  cSEQ09_WriteControlToGo    mcSEQ09_WriteControlToGo;
  cSEQ10_WriteControlTimer   mcSEQ10_WriteControlTimer;
  cSEQ11_WriteControlCharger mcSEQ11_WriteControlCharger;
  cSEQ12_WriteControlMppt    mcSEQ12_WriteControlMppt;
  cSEQ13_WriteControlTemp    mcSEQ13_WriteControlTemp;
  cSEQ14_WriteControlAdc     mcSEQ14_WriteControlAdc;
  cSEQ15_ReadStatusCharger   mcSEQ15_ReadStatusCharger;
  cSEQ16_ReadStatusFaults    mcSEQ16_ReadStatusFaults;
  cSEQ17_ReadStatusAdc       mcSEQ17_ReadStatusAdc;


  enum tenSequences : u8
  {
    nSEQ00Reset    = cComSeq::tenDefaultSequences::nSEQ00Reset,
    nSEQ01Init1    = cComSeq::tenDefaultSequences::nSEQ01Init1,
    nSEQ02Init2    = cComSeq::tenDefaultSequences::nSEQ02Init2,
    nSEQ04Enable   = cComSeq::tenDefaultSequences::nSEQ04Enable,
    nSEQ05Disable  = cComSeq::tenDefaultSequences::nSEQ05Disable,
    nSEQ06DeInit   = cComSeq::tenDefaultSequences::nSEQ06DeInit,

    nSEQ07_WriteControlLimits,
    nSEQ08_WriteControlCharge,
    nSEQ09_WriteControlToGo,
    nSEQ10_WriteControlTimer,
    nSEQ11_WriteControlCharger,
    nSEQ12_WriteControlMppt,
    nSEQ13_WriteControlTemp,
    nSEQ14_WriteControlAdc,
    nSEQ15_ReadStatusCharger,
    nSEQ16_ReadStatusFaults,
    nSEQ17_ReadStatusAdc
  };


  cComSeq* pcGetSequence(u8 lu8SeqIdx)
  {
    switch (lu8SeqIdx)
    {
      case tenSequences::nSEQ01Init1:     return &mcSEQ01_Init1;
      case tenSequences::nSEQ06DeInit:    return &mcSEQ06_DeInit;

      case tenSequences::nSEQ07_WriteControlLimits:  return &mcSEQ07_WriteControlLimits;
      case tenSequences::nSEQ08_WriteControlCharge:  return &mcSEQ08_WriteControlCharge;
      case tenSequences::nSEQ09_WriteControlToGo:    return &mcSEQ09_WriteControlToGo;
      case tenSequences::nSEQ10_WriteControlTimer:   return &mcSEQ10_WriteControlTimer;
      case tenSequences::nSEQ11_WriteControlCharger: return &mcSEQ11_WriteControlCharger;
      case tenSequences::nSEQ12_WriteControlMppt:    return &mcSEQ12_WriteControlMppt;
      case tenSequences::nSEQ13_WriteControlTemp:    return &mcSEQ13_WriteControlTemp;
      case tenSequences::nSEQ14_WriteControlAdc:     return &mcSEQ14_WriteControlAdc;
      case tenSequences::nSEQ15_ReadStatusCharger:   return &mcSEQ15_ReadStatusCharger;
      case tenSequences::nSEQ16_ReadStatusFaults:    return &mcSEQ16_ReadStatusFaults;
      case tenSequences::nSEQ17_ReadStatusAdc:       return &mcSEQ17_ReadStatusAdc;

      default: return null;
    }
  }

  cBQ25798_RegisterMap()
  {}

};

#endif