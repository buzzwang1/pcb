/**
 * \file
 *
 * \brief Component description for PORT
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _SAMD10_PORT_COMPONENT_
#define _SAMD10_PORT_COMPONENT_

 /* ========== Register definition for PORT peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PORT_DIR0              (0x41004400U) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (0x41004404U) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (0x41004408U) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (0x4100440CU) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (0x41004410U) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (0x41004414U) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (0x41004418U) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (0x4100441CU) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (0x41004420U) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (0x41004424U) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (0x41004428U) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_PMUX0             (0x41004430U) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (0x41004440U) /**< \brief (PORT) Pin Configuration 0 */
#else
#define REG_PORT_DIR0              (*(RwReg  *)0x41004400U) /**< \brief (PORT) Data Direction 0 */
#define REG_PORT_DIRCLR0           (*(RwReg  *)0x41004404U) /**< \brief (PORT) Data Direction Clear 0 */
#define REG_PORT_DIRSET0           (*(RwReg  *)0x41004408U) /**< \brief (PORT) Data Direction Set 0 */
#define REG_PORT_DIRTGL0           (*(RwReg  *)0x4100440CU) /**< \brief (PORT) Data Direction Toggle 0 */
#define REG_PORT_OUT0              (*(RwReg  *)0x41004410U) /**< \brief (PORT) Data Output Value 0 */
#define REG_PORT_OUTCLR0           (*(RwReg  *)0x41004414U) /**< \brief (PORT) Data Output Value Clear 0 */
#define REG_PORT_OUTSET0           (*(RwReg  *)0x41004418U) /**< \brief (PORT) Data Output Value Set 0 */
#define REG_PORT_OUTTGL0           (*(RwReg  *)0x4100441CU) /**< \brief (PORT) Data Output Value Toggle 0 */
#define REG_PORT_IN0               (*(RoReg  *)0x41004420U) /**< \brief (PORT) Data Input Value 0 */
#define REG_PORT_CTRL0             (*(RwReg  *)0x41004424U) /**< \brief (PORT) Control 0 */
#define REG_PORT_WRCONFIG0         (*(WoReg  *)0x41004428U) /**< \brief (PORT) Write Configuration 0 */
#define REG_PORT_PMUX0             (*(RwReg  *)0x41004430U) /**< \brief (PORT) Peripheral Multiplexing 0 */
#define REG_PORT_PINCFG0           (*(RwReg  *)0x41004440U) /**< \brief (PORT) Pin Configuration 0 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PORT peripheral ========== */
#define PORT_BITS                   32       // Number of PORT pins
#define PORT_DIR_DEFAULT_VAL        { 0x00000000 } // Default value for DIR of all pins
#define PORT_DIR_IMPLEMENTED        { 0xDBC3CFFC } // Implementation mask for DIR of all pins
#define PORT_DRVSTR                 1        // DRVSTR supported
#define PORT_DRVSTR_DEFAULT_VAL     { 0x00000000 } // Default value for DRVSTR of all pins
#define PORT_DRVSTR_IMPLEMENTED     { 0xD8C3CFFC } // Implementation mask for DRVSTR of all pins
#define PORT_EVENT_IMPLEMENTED      { 0x00000000 }
#define PORT_GROUPS                 1        // Number of 32-bit PORT groups
#define PORT_INEN_DEFAULT_VAL       { 0x10000000 } // Default value for INEN of all pins
#define PORT_INEN_IMPLEMENTED       { 0xDBC3CFFC } // Implementation mask for INEN of all pins
#define PORT_ODRAIN                 0        // ODRAIN supported
#define PORT_ODRAIN_DEFAULT_VAL     { 0x00000000 } // Default value for ODRAIN of all pins
#define PORT_ODRAIN_IMPLEMENTED     { 0x00000000 } // Implementation mask for ODRAIN of all pins
#define PORT_OUT_DEFAULT_VAL        { 0x10000000 } // Default value for OUT of all pins
#define PORT_OUT_IMPLEMENTED        { 0xDBC3CFFC } // Implementation mask for OUT of all pins
#define PORT_PIN_IMPLEMENTED        { 0xDBC3CFFC } // Implementation mask for all PORT pins
#define PORT_PMUXBIT0_DEFAULT_VAL   { 0x00000000 } // Default value for PMUX[0] of all pins
#define PORT_PMUXBIT0_IMPLEMENTED   { 0xCBC3CFFC } // Implementation mask for PMUX[0] of all pins
#define PORT_PMUXBIT1_DEFAULT_VAL   { 0x40000000 } // Default value for PMUX[1] of all pins
#define PORT_PMUXBIT1_IMPLEMENTED   { 0xCBC3CFF0 } // Implementation mask for PMUX[1] of all pins
#define PORT_PMUXBIT2_DEFAULT_VAL   { 0x40000000 } // Default value for PMUX[2] of all pins
#define PORT_PMUXBIT2_IMPLEMENTED   { 0xCBC3CFF0 } // Implementation mask for PMUX[2] of all pins
#define PORT_PMUXBIT3_DEFAULT_VAL   { 0x00000000 } // Default value for PMUX[3] of all pins
#define PORT_PMUXBIT3_IMPLEMENTED   { 0x00000000 } // Implementation mask for PMUX[3] of all pins
#define PORT_PMUXEN_DEFAULT_VAL     { 0x643C3003 } // Default value for PMUXEN of all pins
#define PORT_PMUXEN_IMPLEMENTED     { 0xCBC3CFFC } // Implementation mask for PMUXEN of all pins
#define PORT_PULLEN_DEFAULT_VAL     { 0x10000000 } // Default value for PULLEN of all pins
#define PORT_PULLEN_IMPLEMENTED     { 0xDBC3CFFC } // Implementation mask for PULLEN of all pins
#define PORT_SLEWLIM                0        // SLEWLIM supported
#define PORT_SLEWLIM_DEFAULT_VAL    { 0x00000000 } // Default value for SLEWLIM of all pins
#define PORT_SLEWLIM_IMPLEMENTED    { 0x00000000 } // Implementation mask for SLEWLIM of all pins


/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PORT */
/* ========================================================================== */
/** \addtogroup SAMD10_PORT Port Module */
/*@{*/

#define PORT_U2210
#define REV_PORT                    0x100

/* -------- PORT_DIR : (PORT Offset: 0x00) (R/W 32) GROUP Data Direction -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DIR:32;           /*!< bit:  0..31  Port Data Direction                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_DIR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_DIR_OFFSET             0x00         /**< \brief (PORT_DIR offset) Data Direction */
#define PORT_DIR_RESETVALUE         0x00000000ul /**< \brief (PORT_DIR reset_value) Data Direction */

#define PORT_DIR_DIR_Pos            0            /**< \brief (PORT_DIR) Port Data Direction */
#define PORT_DIR_DIR_Msk            (0xFFFFFFFFul << PORT_DIR_DIR_Pos)
#define PORT_DIR_DIR(value)         (PORT_DIR_DIR_Msk & ((value) << PORT_DIR_DIR_Pos))
#define PORT_DIR_MASK               0xFFFFFFFFul /**< \brief (PORT_DIR) MASK Register */

/* -------- PORT_DIRCLR : (PORT Offset: 0x04) (R/W 32) GROUP Data Direction Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DIRCLR:32;        /*!< bit:  0..31  Port Data Direction Clear          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_DIRCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_DIRCLR_OFFSET          0x04         /**< \brief (PORT_DIRCLR offset) Data Direction Clear */
#define PORT_DIRCLR_RESETVALUE      0x00000000ul /**< \brief (PORT_DIRCLR reset_value) Data Direction Clear */

#define PORT_DIRCLR_DIRCLR_Pos      0            /**< \brief (PORT_DIRCLR) Port Data Direction Clear */
#define PORT_DIRCLR_DIRCLR_Msk      (0xFFFFFFFFul << PORT_DIRCLR_DIRCLR_Pos)
#define PORT_DIRCLR_DIRCLR(value)   (PORT_DIRCLR_DIRCLR_Msk & ((value) << PORT_DIRCLR_DIRCLR_Pos))
#define PORT_DIRCLR_MASK            0xFFFFFFFFul /**< \brief (PORT_DIRCLR) MASK Register */

/* -------- PORT_DIRSET : (PORT Offset: 0x08) (R/W 32) GROUP Data Direction Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DIRSET:32;        /*!< bit:  0..31  Port Data Direction Set            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_DIRSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_DIRSET_OFFSET          0x08         /**< \brief (PORT_DIRSET offset) Data Direction Set */
#define PORT_DIRSET_RESETVALUE      0x00000000ul /**< \brief (PORT_DIRSET reset_value) Data Direction Set */

#define PORT_DIRSET_DIRSET_Pos      0            /**< \brief (PORT_DIRSET) Port Data Direction Set */
#define PORT_DIRSET_DIRSET_Msk      (0xFFFFFFFFul << PORT_DIRSET_DIRSET_Pos)
#define PORT_DIRSET_DIRSET(value)   (PORT_DIRSET_DIRSET_Msk & ((value) << PORT_DIRSET_DIRSET_Pos))
#define PORT_DIRSET_MASK            0xFFFFFFFFul /**< \brief (PORT_DIRSET) MASK Register */

/* -------- PORT_DIRTGL : (PORT Offset: 0x0C) (R/W 32) GROUP Data Direction Toggle -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DIRTGL:32;        /*!< bit:  0..31  Port Data Direction Toggle         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_DIRTGL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_DIRTGL_OFFSET          0x0C         /**< \brief (PORT_DIRTGL offset) Data Direction Toggle */
#define PORT_DIRTGL_RESETVALUE      0x00000000ul /**< \brief (PORT_DIRTGL reset_value) Data Direction Toggle */

#define PORT_DIRTGL_DIRTGL_Pos      0            /**< \brief (PORT_DIRTGL) Port Data Direction Toggle */
#define PORT_DIRTGL_DIRTGL_Msk      (0xFFFFFFFFul << PORT_DIRTGL_DIRTGL_Pos)
#define PORT_DIRTGL_DIRTGL(value)   (PORT_DIRTGL_DIRTGL_Msk & ((value) << PORT_DIRTGL_DIRTGL_Pos))
#define PORT_DIRTGL_MASK            0xFFFFFFFFul /**< \brief (PORT_DIRTGL) MASK Register */

/* -------- PORT_OUT : (PORT Offset: 0x10) (R/W 32) GROUP Data Output Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OUT:32;           /*!< bit:  0..31  Port Data Output Value             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_OUT_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_OUT_OFFSET             0x10         /**< \brief (PORT_OUT offset) Data Output Value */
#define PORT_OUT_RESETVALUE         0x00000000ul /**< \brief (PORT_OUT reset_value) Data Output Value */

#define PORT_OUT_OUT_Pos            0            /**< \brief (PORT_OUT) Port Data Output Value */
#define PORT_OUT_OUT_Msk            (0xFFFFFFFFul << PORT_OUT_OUT_Pos)
#define PORT_OUT_OUT(value)         (PORT_OUT_OUT_Msk & ((value) << PORT_OUT_OUT_Pos))
#define PORT_OUT_MASK               0xFFFFFFFFul /**< \brief (PORT_OUT) MASK Register */

/* -------- PORT_OUTCLR : (PORT Offset: 0x14) (R/W 32) GROUP Data Output Value Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OUTCLR:32;        /*!< bit:  0..31  Port Data Output Value Clear       */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_OUTCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_OUTCLR_OFFSET          0x14         /**< \brief (PORT_OUTCLR offset) Data Output Value Clear */
#define PORT_OUTCLR_RESETVALUE      0x00000000ul /**< \brief (PORT_OUTCLR reset_value) Data Output Value Clear */

#define PORT_OUTCLR_OUTCLR_Pos      0            /**< \brief (PORT_OUTCLR) Port Data Output Value Clear */
#define PORT_OUTCLR_OUTCLR_Msk      (0xFFFFFFFFul << PORT_OUTCLR_OUTCLR_Pos)
#define PORT_OUTCLR_OUTCLR(value)   (PORT_OUTCLR_OUTCLR_Msk & ((value) << PORT_OUTCLR_OUTCLR_Pos))
#define PORT_OUTCLR_MASK            0xFFFFFFFFul /**< \brief (PORT_OUTCLR) MASK Register */

/* -------- PORT_OUTSET : (PORT Offset: 0x18) (R/W 32) GROUP Data Output Value Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OUTSET:32;        /*!< bit:  0..31  Port Data Output Value Set         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_OUTSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_OUTSET_OFFSET          0x18         /**< \brief (PORT_OUTSET offset) Data Output Value Set */
#define PORT_OUTSET_RESETVALUE      0x00000000ul /**< \brief (PORT_OUTSET reset_value) Data Output Value Set */

#define PORT_OUTSET_OUTSET_Pos      0            /**< \brief (PORT_OUTSET) Port Data Output Value Set */
#define PORT_OUTSET_OUTSET_Msk      (0xFFFFFFFFul << PORT_OUTSET_OUTSET_Pos)
#define PORT_OUTSET_OUTSET(value)   (PORT_OUTSET_OUTSET_Msk & ((value) << PORT_OUTSET_OUTSET_Pos))
#define PORT_OUTSET_MASK            0xFFFFFFFFul /**< \brief (PORT_OUTSET) MASK Register */

/* -------- PORT_OUTTGL : (PORT Offset: 0x1C) (R/W 32) GROUP Data Output Value Toggle -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OUTTGL:32;        /*!< bit:  0..31  Port Data Output Value Toggle      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_OUTTGL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_OUTTGL_OFFSET          0x1C         /**< \brief (PORT_OUTTGL offset) Data Output Value Toggle */
#define PORT_OUTTGL_RESETVALUE      0x00000000ul /**< \brief (PORT_OUTTGL reset_value) Data Output Value Toggle */

#define PORT_OUTTGL_OUTTGL_Pos      0            /**< \brief (PORT_OUTTGL) Port Data Output Value Toggle */
#define PORT_OUTTGL_OUTTGL_Msk      (0xFFFFFFFFul << PORT_OUTTGL_OUTTGL_Pos)
#define PORT_OUTTGL_OUTTGL(value)   (PORT_OUTTGL_OUTTGL_Msk & ((value) << PORT_OUTTGL_OUTTGL_Pos))
#define PORT_OUTTGL_MASK            0xFFFFFFFFul /**< \brief (PORT_OUTTGL) MASK Register */

/* -------- PORT_IN : (PORT Offset: 0x20) (R/  32) GROUP Data Input Value -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t IN:32;            /*!< bit:  0..31  Port Data Input Value              */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_IN_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_IN_OFFSET              0x20         /**< \brief (PORT_IN offset) Data Input Value */
#define PORT_IN_RESETVALUE          0x00000000ul /**< \brief (PORT_IN reset_value) Data Input Value */

#define PORT_IN_IN_Pos              0            /**< \brief (PORT_IN) Port Data Input Value */
#define PORT_IN_IN_Msk              (0xFFFFFFFFul << PORT_IN_IN_Pos)
#define PORT_IN_IN(value)           (PORT_IN_IN_Msk & ((value) << PORT_IN_IN_Pos))
#define PORT_IN_MASK                0xFFFFFFFFul /**< \brief (PORT_IN) MASK Register */

/* -------- PORT_CTRL : (PORT Offset: 0x24) (R/W 32) GROUP Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SAMPLING:32;      /*!< bit:  0..31  Input Sampling Mode                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_CTRL_OFFSET            0x24         /**< \brief (PORT_CTRL offset) Control */
#define PORT_CTRL_RESETVALUE        0x00000000ul /**< \brief (PORT_CTRL reset_value) Control */

#define PORT_CTRL_SAMPLING_Pos      0            /**< \brief (PORT_CTRL) Input Sampling Mode */
#define PORT_CTRL_SAMPLING_Msk      (0xFFFFFFFFul << PORT_CTRL_SAMPLING_Pos)
#define PORT_CTRL_SAMPLING(value)   (PORT_CTRL_SAMPLING_Msk & ((value) << PORT_CTRL_SAMPLING_Pos))
#define PORT_CTRL_MASK              0xFFFFFFFFul /**< \brief (PORT_CTRL) MASK Register */

/* -------- PORT_WRCONFIG : (PORT Offset: 0x28) ( /W 32) GROUP Write Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PINMASK:16;       /*!< bit:  0..15  Pin Mask for Multiple Pin Configuration */
    uint32_t PMUXEN:1;         /*!< bit:     16  Peripheral Multiplexer Enable      */
    uint32_t INEN:1;           /*!< bit:     17  Input Enable                       */
    uint32_t PULLEN:1;         /*!< bit:     18  Pull Enable                        */
    uint32_t :3;               /*!< bit: 19..21  Reserved                           */
    uint32_t DRVSTR:1;         /*!< bit:     22  Output Driver Strength Selection   */
    uint32_t :1;               /*!< bit:     23  Reserved                           */
    uint32_t PMUX:4;           /*!< bit: 24..27  Peripheral Multiplexing            */
    uint32_t WRPMUX:1;         /*!< bit:     28  Write PMUX                         */
    uint32_t :1;               /*!< bit:     29  Reserved                           */
    uint32_t WRPINCFG:1;       /*!< bit:     30  Write PINCFG                       */
    uint32_t HWSEL:1;          /*!< bit:     31  Half-Word Select                   */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PORT_WRCONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_WRCONFIG_OFFSET        0x28         /**< \brief (PORT_WRCONFIG offset) Write Configuration */
#define PORT_WRCONFIG_RESETVALUE    0x00000000ul /**< \brief (PORT_WRCONFIG reset_value) Write Configuration */

#define PORT_WRCONFIG_PINMASK_Pos   0            /**< \brief (PORT_WRCONFIG) Pin Mask for Multiple Pin Configuration */
#define PORT_WRCONFIG_PINMASK_Msk   (0xFFFFul << PORT_WRCONFIG_PINMASK_Pos)
#define PORT_WRCONFIG_PINMASK(value) (PORT_WRCONFIG_PINMASK_Msk & ((value) << PORT_WRCONFIG_PINMASK_Pos))
#define PORT_WRCONFIG_PMUXEN_Pos    16           /**< \brief (PORT_WRCONFIG) Peripheral Multiplexer Enable */
#define PORT_WRCONFIG_PMUXEN        (0x1ul << PORT_WRCONFIG_PMUXEN_Pos)
#define PORT_WRCONFIG_INEN_Pos      17           /**< \brief (PORT_WRCONFIG) Input Enable */
#define PORT_WRCONFIG_INEN          (0x1ul << PORT_WRCONFIG_INEN_Pos)
#define PORT_WRCONFIG_PULLEN_Pos    18           /**< \brief (PORT_WRCONFIG) Pull Enable */
#define PORT_WRCONFIG_PULLEN        (0x1ul << PORT_WRCONFIG_PULLEN_Pos)
#define PORT_WRCONFIG_DRVSTR_Pos    22           /**< \brief (PORT_WRCONFIG) Output Driver Strength Selection */
#define PORT_WRCONFIG_DRVSTR        (0x1ul << PORT_WRCONFIG_DRVSTR_Pos)
#define PORT_WRCONFIG_PMUX_Pos      24           /**< \brief (PORT_WRCONFIG) Peripheral Multiplexing */
#define PORT_WRCONFIG_PMUX_Msk      (0xFul << PORT_WRCONFIG_PMUX_Pos)
#define PORT_WRCONFIG_PMUX(value)   (PORT_WRCONFIG_PMUX_Msk & ((value) << PORT_WRCONFIG_PMUX_Pos))
#define PORT_WRCONFIG_WRPMUX_Pos    28           /**< \brief (PORT_WRCONFIG) Write PMUX */
#define PORT_WRCONFIG_WRPMUX        (0x1ul << PORT_WRCONFIG_WRPMUX_Pos)
#define PORT_WRCONFIG_WRPINCFG_Pos  30           /**< \brief (PORT_WRCONFIG) Write PINCFG */
#define PORT_WRCONFIG_WRPINCFG      (0x1ul << PORT_WRCONFIG_WRPINCFG_Pos)
#define PORT_WRCONFIG_HWSEL_Pos     31           /**< \brief (PORT_WRCONFIG) Half-Word Select */
#define PORT_WRCONFIG_HWSEL         (0x1ul << PORT_WRCONFIG_HWSEL_Pos)
#define PORT_WRCONFIG_MASK          0xDF47FFFFul /**< \brief (PORT_WRCONFIG) MASK Register */

/* -------- PORT_PMUX : (PORT Offset: 0x30) (R/W  8) GROUP Peripheral Multiplexing n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PMUXE:4;          /*!< bit:  0.. 3  Peripheral Multiplexing Even       */
    uint8_t  PMUXO:4;          /*!< bit:  4.. 7  Peripheral Multiplexing Odd        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PORT_PMUX_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_PMUX_OFFSET            0x30         /**< \brief (PORT_PMUX offset) Peripheral Multiplexing n */
#define PORT_PMUX_RESETVALUE        0x00ul       /**< \brief (PORT_PMUX reset_value) Peripheral Multiplexing n */

#define PORT_PMUX_PMUXE_Pos         0            /**< \brief (PORT_PMUX) Peripheral Multiplexing Even */
#define PORT_PMUX_PMUXE_Msk         (0xFul << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE(value)      (PORT_PMUX_PMUXE_Msk & ((value) << PORT_PMUX_PMUXE_Pos))
#define   PORT_PMUX_PMUXE_A_Val           0x0ul  /**< \brief (PORT_PMUX) Peripheral function A selected */
#define   PORT_PMUX_PMUXE_B_Val           0x1ul  /**< \brief (PORT_PMUX) Peripheral function B selected */
#define   PORT_PMUX_PMUXE_C_Val           0x2ul  /**< \brief (PORT_PMUX) Peripheral function C selected */
#define   PORT_PMUX_PMUXE_D_Val           0x3ul  /**< \brief (PORT_PMUX) Peripheral function D selected */
#define   PORT_PMUX_PMUXE_E_Val           0x4ul  /**< \brief (PORT_PMUX) Peripheral function E selected */
#define   PORT_PMUX_PMUXE_F_Val           0x5ul  /**< \brief (PORT_PMUX) Peripheral function F selected */
#define   PORT_PMUX_PMUXE_G_Val           0x6ul  /**< \brief (PORT_PMUX) Peripheral function G selected */
#define   PORT_PMUX_PMUXE_H_Val           0x7ul  /**< \brief (PORT_PMUX) Peripheral function H selected */
#define PORT_PMUX_PMUXE_A           (PORT_PMUX_PMUXE_A_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE_B           (PORT_PMUX_PMUXE_B_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE_C           (PORT_PMUX_PMUXE_C_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE_D           (PORT_PMUX_PMUXE_D_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE_E           (PORT_PMUX_PMUXE_E_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE_F           (PORT_PMUX_PMUXE_F_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE_G           (PORT_PMUX_PMUXE_G_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXE_H           (PORT_PMUX_PMUXE_H_Val         << PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_PMUXO_Pos         4            /**< \brief (PORT_PMUX) Peripheral Multiplexing Odd */
#define PORT_PMUX_PMUXO_Msk         (0xFul << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO(value)      (PORT_PMUX_PMUXO_Msk & ((value) << PORT_PMUX_PMUXO_Pos))
#define   PORT_PMUX_PMUXO_A_Val           0x0ul  /**< \brief (PORT_PMUX) Peripheral function A selected */
#define   PORT_PMUX_PMUXO_B_Val           0x1ul  /**< \brief (PORT_PMUX) Peripheral function B selected */
#define   PORT_PMUX_PMUXO_C_Val           0x2ul  /**< \brief (PORT_PMUX) Peripheral function C selected */
#define   PORT_PMUX_PMUXO_D_Val           0x3ul  /**< \brief (PORT_PMUX) Peripheral function D selected */
#define   PORT_PMUX_PMUXO_E_Val           0x4ul  /**< \brief (PORT_PMUX) Peripheral function E selected */
#define   PORT_PMUX_PMUXO_F_Val           0x5ul  /**< \brief (PORT_PMUX) Peripheral function F selected */
#define   PORT_PMUX_PMUXO_G_Val           0x6ul  /**< \brief (PORT_PMUX) Peripheral function G selected */
#define   PORT_PMUX_PMUXO_H_Val           0x7ul  /**< \brief (PORT_PMUX) Peripheral function H selected */
#define PORT_PMUX_PMUXO_A           (PORT_PMUX_PMUXO_A_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO_B           (PORT_PMUX_PMUXO_B_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO_C           (PORT_PMUX_PMUXO_C_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO_D           (PORT_PMUX_PMUXO_D_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO_E           (PORT_PMUX_PMUXO_E_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO_F           (PORT_PMUX_PMUXO_F_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO_G           (PORT_PMUX_PMUXO_G_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_PMUXO_H           (PORT_PMUX_PMUXO_H_Val         << PORT_PMUX_PMUXO_Pos)
#define PORT_PMUX_MASK              0xFFul       /**< \brief (PORT_PMUX) MASK Register */

/* -------- PORT_PINCFG : (PORT Offset: 0x40) (R/W  8) GROUP Pin Configuration n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PMUXEN:1;         /*!< bit:      0  Peripheral Multiplexer Enable      */
    uint8_t  INEN:1;           /*!< bit:      1  Input Enable                       */
    uint8_t  PULLEN:1;         /*!< bit:      2  Pull Enable                        */
    uint8_t  :3;               /*!< bit:  3.. 5  Reserved                           */
    uint8_t  DRVSTR:1;         /*!< bit:      6  Output Driver Strength Selection   */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PORT_PINCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PORT_PINCFG_OFFSET          0x40         /**< \brief (PORT_PINCFG offset) Pin Configuration n */
#define PORT_PINCFG_RESETVALUE      0x00ul       /**< \brief (PORT_PINCFG reset_value) Pin Configuration n */

#define PORT_PINCFG_PMUXEN_Pos      0            /**< \brief (PORT_PINCFG) Peripheral Multiplexer Enable */
#define PORT_PINCFG_PMUXEN          (0x1ul << PORT_PINCFG_PMUXEN_Pos)
#define PORT_PINCFG_INEN_Pos        1            /**< \brief (PORT_PINCFG) Input Enable */
#define PORT_PINCFG_INEN            (0x1ul << PORT_PINCFG_INEN_Pos)
#define PORT_PINCFG_PULLEN_Pos      2            /**< \brief (PORT_PINCFG) Pull Enable */
#define PORT_PINCFG_PULLEN          (0x1ul << PORT_PINCFG_PULLEN_Pos)
#define PORT_PINCFG_DRVSTR_Pos      6            /**< \brief (PORT_PINCFG) Output Driver Strength Selection */
#define PORT_PINCFG_DRVSTR          (0x1ul << PORT_PINCFG_DRVSTR_Pos)
#define PORT_PINCFG_MASK            0x47ul       /**< \brief (PORT_PINCFG) MASK Register */

/** \brief PortGroup hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PORT_DIR_Type             DIR;         /**< \brief Offset: 0x00 (R/W 32) Data Direction */
  __IO PORT_DIRCLR_Type          DIRCLR;      /**< \brief Offset: 0x04 (R/W 32) Data Direction Clear */
  __IO PORT_DIRSET_Type          DIRSET;      /**< \brief Offset: 0x08 (R/W 32) Data Direction Set */
  __IO PORT_DIRTGL_Type          DIRTGL;      /**< \brief Offset: 0x0C (R/W 32) Data Direction Toggle */
  __IO PORT_OUT_Type             OUT;         /**< \brief Offset: 0x10 (R/W 32) Data Output Value */
  __IO PORT_OUTCLR_Type          OUTCLR;      /**< \brief Offset: 0x14 (R/W 32) Data Output Value Clear */
  __IO PORT_OUTSET_Type          OUTSET;      /**< \brief Offset: 0x18 (R/W 32) Data Output Value Set */
  __IO PORT_OUTTGL_Type          OUTTGL;      /**< \brief Offset: 0x1C (R/W 32) Data Output Value Toggle */
  __I  PORT_IN_Type              IN;          /**< \brief Offset: 0x20 (R/  32) Data Input Value */
  __IO PORT_CTRL_Type            CTRL;        /**< \brief Offset: 0x24 (R/W 32) Control */
  __O  PORT_WRCONFIG_Type        WRCONFIG;    /**< \brief Offset: 0x28 ( /W 32) Write Configuration */
       RoReg8                    Reserved1[0x4];
  __IO PORT_PMUX_Type            PMUX[16];    /**< \brief Offset: 0x30 (R/W  8) Peripheral Multiplexing n */
  __IO PORT_PINCFG_Type          PINCFG[32];  /**< \brief Offset: 0x40 (R/W  8) Pin Configuration n */
       RoReg8                    Reserved2[0x20];
} PortGroup;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/** \brief PORT hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
       PortGroup                 Group[1];    /**< \brief Offset: 0x00 PortGroup groups [GROUPS] */
} Port;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
#define SECTION_PORT_IOBUS

/*@}*/

#endif /* _SAMD10_PORT_COMPONENT_ */
