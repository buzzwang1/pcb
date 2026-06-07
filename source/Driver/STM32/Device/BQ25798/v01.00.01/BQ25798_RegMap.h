#ifndef __BQ25798_REGMAP_H__
#define __BQ25798_REGMAP_H__

/**
 * Created by Gemini 3 Flash, 2026-04-02
 * Resource: TI BQ25798 Datasheet (SLUSEN4)
 */

#include "TypeDef.h"


// --- Hauptklasse cBQ25798 ---

struct __attribute__((packed)) cBQ25798_RegisterMap
{
  // Enum für die Register-Startadressen (Offsets)
  typedef enum {
      nREG_MIN_SYS_VOLTAGE      = 0x00, // Minimal System Voltage
      nREG_CHG_VOLTAGE_LIMIT    = 0x01, // Charge Voltage Limit (2 Bytes)
      nREG_CHG_CURRENT_LIMIT    = 0x03, // Charge Current Limit (2 Bytes)
      nREG_INPUT_VOLTAGE_LIMIT  = 0x05, // Input Voltage Limit
      nREG_INPUT_CURRENT_LIMIT  = 0x06, // Input Current Limit (2 Bytes)
      nREG_PRECHARGE_CTRL       = 0x08, // Precharge Control
      nREG_TERM_CTRL            = 0x09, // Termination Control
      nREG_RECHARGE_CTRL        = 0x0A, // Re-charge Control
      nREG_VOTG_REGULATION      = 0x0B, // VOTG regulation (2 Bytes)
      nREG_IOTG_REGULATION      = 0x0D, // IOTG regulation
      nREG_TIMER_CTRL           = 0x0E, // Timer Control
      nREG_CHARGER_CTRL_0       = 0x0F, // Charger Control 0
      nREG_CHARGER_CTRL_1       = 0x10, // Charger Control 1
      nREG_CHARGER_CTRL_2       = 0x11, // Charger Control 2
      nREG_CHARGER_CTRL_3       = 0x12, // Charger Control 3
      nREG_CHARGER_CTRL_4       = 0x13, // Charger Control 4
      nREG_CHARGER_CTRL_5       = 0x14, // Charger Control 5
      nREG_MPPT_CTRL            = 0x15, // MPPT Control
      nREG_TEMP_CTRL            = 0x16, // Temperature Control
      nREG_NTC_CTRL_0           = 0x17, // NTC Control 0
      nREG_NTC_CTRL_1           = 0x18, // NTC Control 1
      nREG_ICO_CURRENT_LIMIT    = 0x19, // ICO Current Limit (2 Bytes)
      nREG_CHARGER_STATUS_0     = 0x1B, // Charger Status 0
      nREG_CHARGER_STATUS_1     = 0x1C, // Charger Status 1
      nREG_CHARGER_STATUS_2     = 0x1D, // Charger Status 2
      nREG_CHARGER_STATUS_3     = 0x1E, // Charger Status 3
      nREG_CHARGER_STATUS_4     = 0x1F, // Charger Status 4
      nREG_FAULT_STATUS_0       = 0x20, // FAULT Status 0
      nREG_FAULT_STATUS_1       = 0x21, // FAULT Status 1
      nREG_CHARGER_FLAG_0       = 0x22, // Charger Flag 0
      nREG_CHARGER_FLAG_1       = 0x23, // Charger Flag 1
      nREG_CHARGER_FLAG_2       = 0x24, // Charger Flag 2
      nREG_CHARGER_FLAG_3       = 0x25, // Charger Flag 3
      nREG_FAULT_FLAG_0         = 0x26, // FAULT Flag 0
      nREG_FAULT_FLAG_1         = 0x27, // FAULT Flag 1
      nREG_CHARGER_MASK_0       = 0x28, // Charger Mask 0
      nREG_CHARGER_MASK_1       = 0x29, // Charger Mask 1
      nREG_CHARGER_MASK_2       = 0x2A, // Charger Mask 2
      nREG_CHARGER_MASK_3       = 0x2B, // Charger Mask 3
      nREG_FAULT_MASK_0         = 0x2C, // FAULT Mask 0
      nREG_FAULT_MASK_1         = 0x2D, // FAULT Mask 1
      nREG_ADC_CONTROL          = 0x2E, // ADC Control
      nREG_ADC_FUNC_DIS_0       = 0x2F, // ADC Function Disable 0
      nREG_ADC_FUNC_DIS_1       = 0x30, // ADC Function Disable 1
      nREG_IBUS_ADC             = 0x31, // IBUS ADC (2 Bytes)
      nREG_IBAT_ADC             = 0x33, // IBAT ADC (2 Bytes)
      nREG_VBUS_ADC             = 0x35, // VBUS ADC (2 Bytes)
      nREG_VAC1_ADC             = 0x37, // VAC1 ADC (2 Bytes)
      nREG_VAC2_ADC             = 0x39, // VAC2 ADC (2 Bytes)
      nREG_VBAT_ADC             = 0x3B, // VBAT ADC (2 Bytes)
      nREG_VSYS_ADC             = 0x3D, // VSYS ADC (2 Bytes)
      nREG_TS_ADC               = 0x3F, // TS ADC (2 Bytes)
      nREG_TDIE_ADC             = 0x41, // TDIE ADC (2 Bytes)
      nREG_DPLUS_ADC            = 0x43, // D+ ADC (2 Bytes)
      nREG_DMINUS_ADC           = 0x45, // D- ADC (2 Bytes)
      nREG_DPDM_DRIVER          = 0x47, // DPDM Driver
      nREG_PART_INFORMATION     = 0x48  // Part Information
  } tenRegisterList;

  /**
   * @brief Gibt die Länge eines Registers in Bytes basierend auf seinem Offset zurück.
   * Basierend auf Table 9-12 des BQ25798 Datenblatts.
   * * @param ui8Offset Das zu prüfende Register-Offset.
   * @return u8 Länge in Bytes (1 oder 2). Gibt 0 zurück, wenn der Offset ungültig/reserviert ist.
   */
  u8 u8GetRegLen(u8 ui8Offset) 
  {
    switch (ui8Offset) 
    {
        /* 16-Bit Register (Länge 2) */
        case 0x01: // Charge Voltage Limit [cite: 44]
        case 0x03: // Charge Current Limit [cite: 44]
        case 0x06: // Input Current Limit [cite: 44]
        case 0x0B: // VOTG Regulation [cite: 44]
        case 0x19: // ICO Current Limit [cite: 45]
        case 0x31: // IBUS ADC [cite: 48]
        case 0x33: // IBAT ADC [cite: 48]
        case 0x35: // VBUS ADC [cite: 48]
        case 0x37: // VAC1 ADC [cite: 48]
        case 0x39: // VAC2 ADC [cite: 48]
        case 0x3B: // VBAT ADC [cite: 48]
        case 0x3D: // VSYS ADC [cite: 48]
        case 0x3F: // TS ADC [cite: 48]
        case 0x41: // TDIE ADC [cite: 48]
        case 0x43: // D+ ADC [cite: 48]
        case 0x45: // D- ADC [cite: 48]
            return 2;

        /* Alle anderen definierten Register sind 8-Bit (Länge 1) */
        case 0x00:
        case 0x05:
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0D:
        case 0x0E:
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
        case 0x14:
        case 0x15:
        case 0x16:
        case 0x17:
        case 0x18:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
        case 0x1F:
        case 0x20:
        case 0x21:
        case 0x22:
        case 0x23:
        case 0x24:
        case 0x25:
        case 0x26:
        case 0x27:
        case 0x28:
        case 0x29:
        case 0x2A:
        case 0x2B:
        case 0x2C:
        case 0x2D:
        case 0x2E:
        case 0x2F:
        case 0x30:
        case 0x47:
        case 0x48:
            return 1;

        /* Reservierte Offsets  */
        default:
            return 0; 
    }
  }

  /**
   * BQ25798 COMPLETE Register Map (Table 9-12)
   * Compiler: GCC (__attribute__((packed)))
   * Bit Order: Bit 0 (LSB) Top -> Bit 7 (MSB) Bottom
   */


  // 00h: Minimal System Voltage
  typedef union {
      u8 u8Byte;
      struct {
          u8 VSYS_MIN_0 : 1; // Bit 0 (Top)
          u8 VSYS_MIN_1 : 1;
          u8 VSYS_MIN_2 : 1;
          u8 VSYS_MIN_3 : 1;
          u8 VSYS_MIN_4 : 1;
          u8 VSYS_MIN_5 : 1;
          u8 RESERVED   : 2; // Bit 7 (Bottom)
      } stBits;
  } tunREG00_Minimal_System_Voltage;

  // 01h: Charge Voltage Limit (16-bit)
  typedef union {
      u16 u16Word;
      struct {
          u16 VREG_0    : 1; u16 VREG_1    : 1; u16 VREG_2    : 1; u16 VREG_3    : 1;
          u16 VREG_4    : 1; u16 VREG_5    : 1; u16 VREG_6    : 1; u16 VREG_7    : 1;
          u16 VREG_8    : 1; u16 VREG_9    : 1; u16 VREG_10   : 1;
          u16 RESERVED  : 5; // Bit 15 (Bottom)
      } stBits;
  } tunREG01_Charge_Voltage_Limit;

  // 03h: Charge Current Limit (16-bit)
  typedef union {
      u16 u16Word;
      struct {
          u16 ICHG_0    : 1; u16 ICHG_1    : 1; u16 ICHG_2    : 1; u16 ICHG_3    : 1;
          u16 ICHG_4    : 1; u16 ICHG_5    : 1; u16 ICHG_6    : 1; u16 ICHG_7    : 1;
          u16 ICHG_8    : 1;
          u16 RESERVED  : 7; // Bit 15 (Bottom)
      } stBits;
  } tunREG03_Charge_Current_Limit;

  // 05h: Input Voltage Limit
  typedef union {
      u8 u8Byte;
      struct {
          u8 VINDPM_0 : 1; u8 VINDPM_1 : 1; u8 VINDPM_2 : 1; u8 VINDPM_3 : 1;
          u8 VINDPM_4 : 1; u8 VINDPM_5 : 1; u8 VINDPM_6 : 1; u8 VINDPM_7 : 1;
      } stBits;
  } tunREG05_Input_Voltage_Limit;

  // 06h: Input Current Limit (16-bit)
  typedef union {
      u16 u16Word;
      struct {
          u16 IINDPM_0  : 1; u16 IINDPM_1  : 1; u16 IINDPM_2  : 1; u16 IINDPM_3  : 1;
          u16 IINDPM_4  : 1; u16 IINDPM_5  : 1; u16 IINDPM_6  : 1; u16 IINDPM_7  : 1;
          u16 IINDPM_8  : 1;
          u16 RESERVED  : 7;
      } stBits;
  } tunREG06_Input_Current_Limit;

  // 08h: Precharge Control
  typedef union {
      u8 u8Byte;
      struct {
          u8 IPRECHG_0 : 1; u8 IPRECHG_1 : 1; u8 IPRECHG_2 : 1; u8 IPRECHG_3 : 1;
          u8 IPRECHG_4 : 1;
          u8 RESERVED  : 3;
      } stBits;
  } tunREG08_Precharge_Control;

  // 09h: Termination Control
  typedef union {
      u8 u8Byte;
      struct {
          u8 ITERM_0    : 1; u8 ITERM_1 : 1; u8 ITERM_2 : 1; u8 ITERM_3 : 1; u8 ITERM_4 : 1;
          u8 STOP_WD_EN : 1;
          u8 RESERVED   : 1;
          u8 REG_RST    : 1; // Bit 7
      } stBits;
  } tunREG09_Termination_Control;

  // 0Ah: Re-charge Control
  typedef union {
      u8 u8Byte;
      struct {
          u8 VRECHG_0 : 1; u8 VRECHG_1 : 1;
          u8 RESERVED : 6;
      } stBits;
  } tunREG0A_Re_charge_Control;

  // 0Bh: VOTG Regulation (16-bit)
  typedef union {
      u16 u16Word;
      struct {
          u16 VOTG_0 : 1; u16 VOTG_1 : 1; u16 VOTG_2 : 1; u16 VOTG_3 : 1;
          u16 VOTG_4 : 1; u16 VOTG_5 : 1; u16 VOTG_6 : 1; u16 VOTG_7 : 1;
          u16 VOTG_8 : 1; u16 VOTG_9 : 1; u16 VOTG_10 : 1;
          u16 RESERVED : 5;
      } stBits;
  } tunREG0B_VOTG_regulation;

  // 0Dh: IOTG Regulation
  typedef union {
      u8 u8Byte;
      struct {
          u8 IOTG_0 : 1; u8 IOTG_1 : 1; u8 IOTG_2 : 1; u8 IOTG_3 : 1;
          u8 IOTG_4 : 1; u8 IOTG_5 : 1; u8 IOTG_6 : 1;
          u8 RESERVED : 1;
      } stBits;
  } tunREG0D_IOTG_regulation;

  // 0Eh: Timer Control
  typedef union {
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
  typedef union {
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
  typedef union {
      u8 u8Byte;
      struct {
          u8 VAC_OVP      : 3;
          u8 WD_RST       : 1;
          u8 FORCE_ICO    : 1;
          u8 EN_OTG       : 1;
          u8 EN_ACDRV2    : 1;
          u8 EN_ACDRV1    : 1;
      } stBits;
  } tunREG10_Charger_Control_1;

  // 11h: Charger Control 2
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
      u8 u8Byte;
      struct {
          u8 SF_EN            : 1;
          u8 SF_WAIT          : 2;
          u8 SF_IDLE          : 2;
          u8 EN_BACKUP        : 1;
          u8 EN_SDRV_CTRL     : 1;
          u8 EN_SDRV_VBDCHG   : 1;
      } stBits;
  } tunREG14_Charger_Control_5;

  // 15h: MPPT Control
  typedef union {
      u8 u8Byte;
      struct {
          u8 VOC_PCT          : 3;
          u8 VOC_DLY          : 2;
          u8 EN_MPPT          : 1;
          u8 RESERVED         : 2;
      } stBits;
  } tunREG15_MPPT_Control;

  // 16h: Temperature Control
  typedef union {
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
  typedef union {
      u8 u8Byte;
      struct {
          u8 JEITA_VSET       : 1;
          u8 JEITA_ISETPCT    : 3;
          u8 RESERVED         : 4;
      } stBits;
  } tunREG17_NTC_Control_0;

  // 18h: NTC Control 1
  typedef union {
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
  typedef union {
      u16 u16Word;
      struct {
          u16 ICO_ILIM_0 : 1; u16 ICO_ILIM_1 : 1; u16 ICO_ILIM_2 : 1; u16 ICO_ILIM_3 : 1;
          u16 ICO_ILIM_4 : 1; u16 ICO_ILIM_5 : 1; u16 ICO_ILIM_6 : 1; u16 ICO_ILIM_7 : 1;
          u16 ICO_ILIM_8 : 1;
          u16 RESERVED   : 7;
      } stBits;
  } tunREG19_ICO_Current_Limit;

  // 1Bh: Charger Status 0
  typedef union {
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
  typedef union {
      u8 u8Byte;
      struct {
        u8 BC12_DONE_STAT  : 1; // Bit 7: 1 = In IINDPM regulation
        u8 VBUS_STAT       : 4; // Bit 3-6: 0:No Input, 1:USB SDP, 3:USB CDP, 4:USB DCP, 11:Unknown, 15:OTG
        u8 CHG_STAT        : 3; // Bit 0-2: 0:Not Chg, 1:Trickle, 2:Pre, 3:Fast, 4:Term, 5:Top-off, 6:VINDPM, 7:IINDPM
      } stBits;
  } tunREG1C_Charger_Status_1;

  // 1Dh: Charger Status 2
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union  {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
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
  typedef union {
    u16 u16Word;
    struct {
      u16 IBUS_ADC : 16; // Bit 0-15: 1 LSB = 1mA
    } stBits;
  } tunREG31_IBUS_ADC;

  // 33h: IBAT ADC (Battery Charge/Discharge Current)
  typedef union {
    i16 i16Word;       // Signed, da Entladestrom negativ sein kann
    struct {
      i16 IBAT_ADC : 16; // Bit 0-15: 1 LSB = 1mA (Zweierkomplement)
    } stBits;
  } tunREG33_IBAT_ADC;

  // 35h: VBUS ADC (VBUS Voltage)
  typedef union {
    u16 u16Word;
    struct {
      u16 VBUS_ADC : 16; // Bit 0-15: 1 LSB = 1mV
    } stBits;
  } tunREG35_VBUS_ADC;

  // 37h: VAC1 ADC (Voltage at VAC1)
  typedef union {
    u16 u16Word;
    struct {
      u16 VAC1_ADC : 16; // Bit 0-15: 1 LSB = 1mV
    } stBits;
  } tunREG37_VAC1_ADC;

  // 39h: VAC2 ADC (Voltage at VAC2)
  typedef union {
    u16 u16Word;
    struct {
      u16 VAC2_ADC : 16; // Bit 0-15: 1 LSB = 1mV
    } stBits;
  } tunREG39_VAC2_ADC;

  // 3Bh: VBAT ADC (Battery Voltage)
  typedef union {
    u16 u16Word;
    struct {
      u16 VBAT_ADC : 16; // Bit 0-15: 1 LSB = 1mV
    } stBits;
  } tunREG3B_VBAT_ADC;

  // 3Dh: VSYS ADC (System Voltage)
  typedef union {
    u16 u16Word;
    struct {
      u16 VSYS_ADC : 16; // Bit 0-15: 1 LSB = 1mV
    } stBits;
  } tunREG3D_VSYS_ADC;

  // 3Fh: TS ADC (Temperature Sense Pin Voltage)
  typedef union {
    u16 u16Word;
    struct {
      u16 TS_ADC : 16;   // Bit 0-15: Prozentsatz von REGN (oder mV je nach Einstellung)
    } stBits;
  } tunREG3F_TS_ADC;

  // 41h: TDIE ADC (Die Temperature)
  typedef union {
    i16 i16Word;       // Signed für Temperaturen
    struct {
      i16 TDIE_ADC : 16; // Bit 0-15: 1 LSB = 0.5°C (Offset beachten)
    } stBits;
  } tunREG41_TDIE_ADC;

  // 43h: DP ADC (D+ Pin Voltage)
  typedef union {
    u16 u16Word;
    struct {
      u16 DP_ADC : 16;   // Bit 0-15: 1 LSB = 1mV
    } stBits;
  } tunREG43_DP_ADC;

  // 45h: DM ADC (D- Pin Voltage)
  typedef union {
    u16 u16Word;
    struct {
      u16 DM_ADC : 16;   // Bit 0-15: 1 LSB = 1mV
    } stBits;
  } tunREG45_DM_ADC;

  // 47h: DPDM Driver
  typedef union {
      u8 u8Byte;
      struct {
          u8 DMINUS_VSEL   : 2;
          u8 DPLUS_VSEL    : 2;
          u8 DPDM_DRV_EN   : 1;
          u8 RESERVED      : 3;
      } stBits;
  } tunREG47_DPDM_Driver;

  // 48h: Part Information
  typedef union {
      u8 u8Byte;
      struct {
          u8 DEV_REV       : 3;
          u8 PN            : 3;
          u8 RESERVED      : 2;
      } stBits;
  } tunREG48_Part_Information;

  // Registermap 00h bis 48h
// --- Configuration Registers ---
  tunREG00_Minimal_System_Voltage    REG00_VSYS_Min;
  tunREG01_Charge_Voltage_Limit      REG01_VREG;
  tunREG03_Charge_Current_Limit      REG03_ICHG;
  tunREG05_Input_Voltage_Limit       REG05_VINDPM;
  tunREG06_Input_Current_Limit       REG06_IINDPM;
  tunREG08_Precharge_Control         REG08_IPRECHG;
  tunREG09_Termination_Control       REG09_ITERM;
  tunREG0A_Re_charge_Control         REG0A_VRECHG;
  tunREG0B_VOTG_regulation           REG0B_VOTG;
  tunREG0D_IOTG_regulation           REG0D_IOTG;
  tunREG0E_Timer_Control             REG0E_Timer;
  tunREG0F_Charger_Control_0         REG0F_CHG_Ctrl0;
  tunREG10_Charger_Control_1         REG10_CHG_Ctrl1;
  tunREG11_Charger_Control_2         REG11_CHG_Ctrl2;
  tunREG12_Charger_Control_3         REG12_CHG_Ctrl3;
  tunREG13_Charger_Control_4         REG13_CHG_Ctrl4;
  tunREG14_Charger_Control_5         REG14_CHG_Ctrl5;
  tunREG15_MPPT_Control              REG15_MPPT;
  tunREG16_Temperature_Control       REG16_Temp_Ctrl;
  tunREG17_NTC_Control_0             REG17_NTC_Ctrl0;
  tunREG18_NTC_Control_1             REG18_NTC_Ctrl1;
  tunREG19_ICO_Current_Limit         REG19_ICO_ILIM;

  // --- Status Registers ---
  tunREG1B_Charger_Status_0          REG1B_Status0;
  tunREG1C_Charger_Status_1          REG1C_Status1;
  tunREG1D_Charger_Status_2          REG1D_Status2;
  tunREG1E_Charger_Status_3          REG1E_Status3;
  tunREG1F_Charger_Status_4          REG1F_Status4;
  tunREG20_FAULT_Status_0            REG20_Fault0;
  tunREG21_FAULT_Status_1            REG21_Fault1;

  // --- Flag Registers ---
  tunREG22_Charger_Flag_0            REG22_Flag0;
  tunREG23_Charger_Flag_1            REG23_Flag1;
  tunREG24_Charger_Flag_2            REG24_Flag2;
  tunREG25_Charger_Flag_3            REG25_Flag3;
  tunREG26_FAULT_Flag_0              REG26_FaultFlag0;
  tunREG27_FAULT_Flag_1              REG27_FaultFlag1;

  // --- Mask Registers ---
  tunREG28_Charger_Mask_0            REG28_Mask0;
  tunREG29_Charger_Mask_1            REG29_Mask1;
  tunREG2A_Charger_Mask_2            REG2A_Mask2;
  tunREG2B_Charger_Mask_3            REG2B_Mask3;
  tunREG2C_FAULT_Mask_0              REG2C_FaultMask0;
  tunREG2D_FAULT_Mask_1              REG2D_FaultMask1;

  // --- ADC Control & Results ---
  tunREG2E_ADC_Control               REG2E_ADC_Ctrl;
  tunREG2F_ADC_Function_Disable_0    REG2F_ADC_Dis0;
  tunREG30_ADC_Function_Disable_1    REG30_ADC_Dis1;
  tunREG31_IBUS_ADC                  REG31_IBUS_ADC;
  tunREG33_IBAT_ADC                  REG33_IBAT_ADC;
  tunREG35_VBUS_ADC                  REG35_VBUS_ADC;
  tunREG37_VAC1_ADC                  REG37_VAC1_ADC;
  tunREG39_VAC2_ADC                  REG39_VAC2_ADC;
  tunREG3B_VBAT_ADC                  REG3B_VBAT_ADC;
  tunREG3D_VSYS_ADC                  REG3D_VSYS_ADC;
  tunREG3F_TS_ADC                    REG3F_TS_ADC;
  tunREG41_TDIE_ADC                  REG41_TDIE_ADC;
  tunREG43_DP_ADC                    REG43_DP_ADC;
  tunREG45_DM_ADC                    REG45_DM_ADC;

  // --- Device Info ---
  tunREG47_DPDM_Driver               REG47_DPDM_Drv;
  tunREG48_Part_Information          REG48_PartInfo;
};

#endif