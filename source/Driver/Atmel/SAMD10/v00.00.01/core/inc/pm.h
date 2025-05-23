/**
 * \file
 *
 * \brief Component description for PM
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

#ifndef _SAMD10_PM_COMPONENT_
#define _SAMD10_PM_COMPONENT_

 /* ========== Register definition for PM peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_PM_CTRL                (0x40000400U) /**< \brief (PM) Control */
#define REG_PM_SLEEP               (0x40000401U) /**< \brief (PM) Sleep Mode */
#define REG_PM_EXTCTRL             (0x40000402U) /**< \brief (PM) External Reset Controller */
#define REG_PM_CPUSEL              (0x40000408U) /**< \brief (PM) CPU Clock Select */
#define REG_PM_APBASEL             (0x40000409U) /**< \brief (PM) APBA Clock Select */
#define REG_PM_APBBSEL             (0x4000040AU) /**< \brief (PM) APBB Clock Select */
#define REG_PM_APBCSEL             (0x4000040BU) /**< \brief (PM) APBC Clock Select */
#define REG_PM_AHBMASK             (0x40000414U) /**< \brief (PM) AHB Mask */
#define REG_PM_APBAMASK            (0x40000418U) /**< \brief (PM) APBA Mask */
#define REG_PM_APBBMASK            (0x4000041CU) /**< \brief (PM) APBB Mask */
#define REG_PM_APBCMASK            (0x40000420U) /**< \brief (PM) APBC Mask */
#define REG_PM_INTENCLR            (0x40000434U) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (0x40000435U) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (0x40000436U) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_RCAUSE              (0x40000438U) /**< \brief (PM) Reset Cause */
#else
#define REG_PM_CTRL                (*(RwReg8 *)0x40000400U) /**< \brief (PM) Control */
#define REG_PM_SLEEP               (*(RwReg8 *)0x40000401U) /**< \brief (PM) Sleep Mode */
#define REG_PM_EXTCTRL             (*(RwReg8 *)0x40000402U) /**< \brief (PM) External Reset Controller */
#define REG_PM_CPUSEL              (*(RwReg8 *)0x40000408U) /**< \brief (PM) CPU Clock Select */
#define REG_PM_APBASEL             (*(RwReg8 *)0x40000409U) /**< \brief (PM) APBA Clock Select */
#define REG_PM_APBBSEL             (*(RwReg8 *)0x4000040AU) /**< \brief (PM) APBB Clock Select */
#define REG_PM_APBCSEL             (*(RwReg8 *)0x4000040BU) /**< \brief (PM) APBC Clock Select */
#define REG_PM_AHBMASK             (*(RwReg  *)0x40000414U) /**< \brief (PM) AHB Mask */
#define REG_PM_APBAMASK            (*(RwReg  *)0x40000418U) /**< \brief (PM) APBA Mask */
#define REG_PM_APBBMASK            (*(RwReg  *)0x4000041CU) /**< \brief (PM) APBB Mask */
#define REG_PM_APBCMASK            (*(RwReg  *)0x40000420U) /**< \brief (PM) APBC Mask */
#define REG_PM_INTENCLR            (*(RwReg8 *)0x40000434U) /**< \brief (PM) Interrupt Enable Clear */
#define REG_PM_INTENSET            (*(RwReg8 *)0x40000435U) /**< \brief (PM) Interrupt Enable Set */
#define REG_PM_INTFLAG             (*(RwReg8 *)0x40000436U) /**< \brief (PM) Interrupt Flag Status and Clear */
#define REG_PM_RCAUSE              (*(RoReg8 *)0x40000438U) /**< \brief (PM) Reset Cause */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for PM peripheral ========== */
#define PM_CTRL_MCSEL_DFLL48M       3       
#define PM_CTRL_MCSEL_GCLK          0       
#define PM_CTRL_MCSEL_OSC8M         1       
#define PM_CTRL_MCSEL_XOSC          2       
#define PM_PM_CLK_APB_NUM           2 

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR PM */
/* ========================================================================== */
/** \addtogroup SAMD10_PM Power Manager */
/*@{*/

#define PM_U2206
#define REV_PM                      0x211

/* -------- PM_CTRL : (PM Offset: 0x00) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :2;               /*!< bit:  0.. 1  Reserved                           */
    uint8_t  CFDEN:1;          /*!< bit:      2  Clock Failure Detector Enable      */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  BKUPCLK:1;        /*!< bit:      4  Backup Clock Select                */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_CTRL_OFFSET              0x00         /**< \brief (PM_CTRL offset) Control */
#define PM_CTRL_RESETVALUE          0x00ul       /**< \brief (PM_CTRL reset_value) Control */

#define PM_CTRL_CFDEN_Pos           2            /**< \brief (PM_CTRL) Clock Failure Detector Enable */
#define PM_CTRL_CFDEN               (0x1ul << PM_CTRL_CFDEN_Pos)
#define PM_CTRL_BKUPCLK_Pos         4            /**< \brief (PM_CTRL) Backup Clock Select */
#define PM_CTRL_BKUPCLK             (0x1ul << PM_CTRL_BKUPCLK_Pos)
#define PM_CTRL_MASK                0x14ul       /**< \brief (PM_CTRL) MASK Register */

/* -------- PM_SLEEP : (PM Offset: 0x01) (R/W  8) Sleep Mode -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  IDLE:2;           /*!< bit:  0.. 1  Idle Mode Configuration            */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_SLEEP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_SLEEP_OFFSET             0x01         /**< \brief (PM_SLEEP offset) Sleep Mode */
#define PM_SLEEP_RESETVALUE         0x00ul       /**< \brief (PM_SLEEP reset_value) Sleep Mode */

#define PM_SLEEP_IDLE_Pos           0            /**< \brief (PM_SLEEP) Idle Mode Configuration */
#define PM_SLEEP_IDLE_Msk           (0x3ul << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_IDLE(value)        (PM_SLEEP_IDLE_Msk & ((value) << PM_SLEEP_IDLE_Pos))
#define   PM_SLEEP_IDLE_CPU_Val           0x0ul  /**< \brief (PM_SLEEP) The CPU clock domain is stopped */
#define   PM_SLEEP_IDLE_AHB_Val           0x1ul  /**< \brief (PM_SLEEP) The CPU and AHB clock domains are stopped */
#define   PM_SLEEP_IDLE_APB_Val           0x2ul  /**< \brief (PM_SLEEP) The CPU, AHB and APB clock domains are stopped */
#define PM_SLEEP_IDLE_CPU           (PM_SLEEP_IDLE_CPU_Val         << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_IDLE_AHB           (PM_SLEEP_IDLE_AHB_Val         << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_IDLE_APB           (PM_SLEEP_IDLE_APB_Val         << PM_SLEEP_IDLE_Pos)
#define PM_SLEEP_MASK               0x03ul       /**< \brief (PM_SLEEP) MASK Register */

/* -------- PM_EXTCTRL : (PM Offset: 0x02) (R/W  8) External Reset Controller -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SETDIS:1;         /*!< bit:      0  External Reset Disable             */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_EXTCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_EXTCTRL_OFFSET           0x02         /**< \brief (PM_EXTCTRL offset) External Reset Controller */
#define PM_EXTCTRL_RESETVALUE       0x00ul       /**< \brief (PM_EXTCTRL reset_value) External Reset Controller */

#define PM_EXTCTRL_SETDIS_Pos       0            /**< \brief (PM_EXTCTRL) External Reset Disable */
#define PM_EXTCTRL_SETDIS           (0x1ul << PM_EXTCTRL_SETDIS_Pos)
#define PM_EXTCTRL_MASK             0x01ul       /**< \brief (PM_EXTCTRL) MASK Register */

/* -------- PM_CPUSEL : (PM Offset: 0x08) (R/W  8) CPU Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CPUDIV:3;         /*!< bit:  0.. 2  CPU Prescaler Selection            */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_CPUSEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_CPUSEL_OFFSET            0x08         /**< \brief (PM_CPUSEL offset) CPU Clock Select */
#define PM_CPUSEL_RESETVALUE        0x00ul       /**< \brief (PM_CPUSEL reset_value) CPU Clock Select */

#define PM_CPUSEL_CPUDIV_Pos        0            /**< \brief (PM_CPUSEL) CPU Prescaler Selection */
#define PM_CPUSEL_CPUDIV_Msk        (0x7ul << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV(value)     (PM_CPUSEL_CPUDIV_Msk & ((value) << PM_CPUSEL_CPUDIV_Pos))
#define   PM_CPUSEL_CPUDIV_DIV1_Val       0x0ul  /**< \brief (PM_CPUSEL) Divide by 1 */
#define   PM_CPUSEL_CPUDIV_DIV2_Val       0x1ul  /**< \brief (PM_CPUSEL) Divide by 2 */
#define   PM_CPUSEL_CPUDIV_DIV4_Val       0x2ul  /**< \brief (PM_CPUSEL) Divide by 4 */
#define   PM_CPUSEL_CPUDIV_DIV8_Val       0x3ul  /**< \brief (PM_CPUSEL) Divide by 8 */
#define   PM_CPUSEL_CPUDIV_DIV16_Val      0x4ul  /**< \brief (PM_CPUSEL) Divide by 16 */
#define   PM_CPUSEL_CPUDIV_DIV32_Val      0x5ul  /**< \brief (PM_CPUSEL) Divide by 32 */
#define   PM_CPUSEL_CPUDIV_DIV64_Val      0x6ul  /**< \brief (PM_CPUSEL) Divide by 64 */
#define   PM_CPUSEL_CPUDIV_DIV128_Val     0x7ul  /**< \brief (PM_CPUSEL) Divide by 128 */
#define PM_CPUSEL_CPUDIV_DIV1       (PM_CPUSEL_CPUDIV_DIV1_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV2       (PM_CPUSEL_CPUDIV_DIV2_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV4       (PM_CPUSEL_CPUDIV_DIV4_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV8       (PM_CPUSEL_CPUDIV_DIV8_Val     << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV16      (PM_CPUSEL_CPUDIV_DIV16_Val    << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV32      (PM_CPUSEL_CPUDIV_DIV32_Val    << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV64      (PM_CPUSEL_CPUDIV_DIV64_Val    << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_CPUDIV_DIV128     (PM_CPUSEL_CPUDIV_DIV128_Val   << PM_CPUSEL_CPUDIV_Pos)
#define PM_CPUSEL_MASK              0x07ul       /**< \brief (PM_CPUSEL) MASK Register */

/* -------- PM_APBASEL : (PM Offset: 0x09) (R/W  8) APBA Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  APBADIV:3;        /*!< bit:  0.. 2  APBA Prescaler Selection           */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_APBASEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBASEL_OFFSET           0x09         /**< \brief (PM_APBASEL offset) APBA Clock Select */
#define PM_APBASEL_RESETVALUE       0x00ul       /**< \brief (PM_APBASEL reset_value) APBA Clock Select */

#define PM_APBASEL_APBADIV_Pos      0            /**< \brief (PM_APBASEL) APBA Prescaler Selection */
#define PM_APBASEL_APBADIV_Msk      (0x7ul << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV(value)   (PM_APBASEL_APBADIV_Msk & ((value) << PM_APBASEL_APBADIV_Pos))
#define   PM_APBASEL_APBADIV_DIV1_Val     0x0ul  /**< \brief (PM_APBASEL) Divide by 1 */
#define   PM_APBASEL_APBADIV_DIV2_Val     0x1ul  /**< \brief (PM_APBASEL) Divide by 2 */
#define   PM_APBASEL_APBADIV_DIV4_Val     0x2ul  /**< \brief (PM_APBASEL) Divide by 4 */
#define   PM_APBASEL_APBADIV_DIV8_Val     0x3ul  /**< \brief (PM_APBASEL) Divide by 8 */
#define   PM_APBASEL_APBADIV_DIV16_Val    0x4ul  /**< \brief (PM_APBASEL) Divide by 16 */
#define   PM_APBASEL_APBADIV_DIV32_Val    0x5ul  /**< \brief (PM_APBASEL) Divide by 32 */
#define   PM_APBASEL_APBADIV_DIV64_Val    0x6ul  /**< \brief (PM_APBASEL) Divide by 64 */
#define   PM_APBASEL_APBADIV_DIV128_Val   0x7ul  /**< \brief (PM_APBASEL) Divide by 128 */
#define PM_APBASEL_APBADIV_DIV1     (PM_APBASEL_APBADIV_DIV1_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV2     (PM_APBASEL_APBADIV_DIV2_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV4     (PM_APBASEL_APBADIV_DIV4_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV8     (PM_APBASEL_APBADIV_DIV8_Val   << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV16    (PM_APBASEL_APBADIV_DIV16_Val  << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV32    (PM_APBASEL_APBADIV_DIV32_Val  << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV64    (PM_APBASEL_APBADIV_DIV64_Val  << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_APBADIV_DIV128   (PM_APBASEL_APBADIV_DIV128_Val << PM_APBASEL_APBADIV_Pos)
#define PM_APBASEL_MASK             0x07ul       /**< \brief (PM_APBASEL) MASK Register */

/* -------- PM_APBBSEL : (PM Offset: 0x0A) (R/W  8) APBB Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  APBBDIV:3;        /*!< bit:  0.. 2  APBB Prescaler Selection           */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_APBBSEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBBSEL_OFFSET           0x0A         /**< \brief (PM_APBBSEL offset) APBB Clock Select */
#define PM_APBBSEL_RESETVALUE       0x00ul       /**< \brief (PM_APBBSEL reset_value) APBB Clock Select */

#define PM_APBBSEL_APBBDIV_Pos      0            /**< \brief (PM_APBBSEL) APBB Prescaler Selection */
#define PM_APBBSEL_APBBDIV_Msk      (0x7ul << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV(value)   (PM_APBBSEL_APBBDIV_Msk & ((value) << PM_APBBSEL_APBBDIV_Pos))
#define   PM_APBBSEL_APBBDIV_DIV1_Val     0x0ul  /**< \brief (PM_APBBSEL) Divide by 1 */
#define   PM_APBBSEL_APBBDIV_DIV2_Val     0x1ul  /**< \brief (PM_APBBSEL) Divide by 2 */
#define   PM_APBBSEL_APBBDIV_DIV4_Val     0x2ul  /**< \brief (PM_APBBSEL) Divide by 4 */
#define   PM_APBBSEL_APBBDIV_DIV8_Val     0x3ul  /**< \brief (PM_APBBSEL) Divide by 8 */
#define   PM_APBBSEL_APBBDIV_DIV16_Val    0x4ul  /**< \brief (PM_APBBSEL) Divide by 16 */
#define   PM_APBBSEL_APBBDIV_DIV32_Val    0x5ul  /**< \brief (PM_APBBSEL) Divide by 32 */
#define   PM_APBBSEL_APBBDIV_DIV64_Val    0x6ul  /**< \brief (PM_APBBSEL) Divide by 64 */
#define   PM_APBBSEL_APBBDIV_DIV128_Val   0x7ul  /**< \brief (PM_APBBSEL) Divide by 128 */
#define PM_APBBSEL_APBBDIV_DIV1     (PM_APBBSEL_APBBDIV_DIV1_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV2     (PM_APBBSEL_APBBDIV_DIV2_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV4     (PM_APBBSEL_APBBDIV_DIV4_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV8     (PM_APBBSEL_APBBDIV_DIV8_Val   << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV16    (PM_APBBSEL_APBBDIV_DIV16_Val  << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV32    (PM_APBBSEL_APBBDIV_DIV32_Val  << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV64    (PM_APBBSEL_APBBDIV_DIV64_Val  << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_APBBDIV_DIV128   (PM_APBBSEL_APBBDIV_DIV128_Val << PM_APBBSEL_APBBDIV_Pos)
#define PM_APBBSEL_MASK             0x07ul       /**< \brief (PM_APBBSEL) MASK Register */

/* -------- PM_APBCSEL : (PM Offset: 0x0B) (R/W  8) APBC Clock Select -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  APBCDIV:3;        /*!< bit:  0.. 2  APBC Prescaler Selection           */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_APBCSEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBCSEL_OFFSET           0x0B         /**< \brief (PM_APBCSEL offset) APBC Clock Select */
#define PM_APBCSEL_RESETVALUE       0x00ul       /**< \brief (PM_APBCSEL reset_value) APBC Clock Select */

#define PM_APBCSEL_APBCDIV_Pos      0            /**< \brief (PM_APBCSEL) APBC Prescaler Selection */
#define PM_APBCSEL_APBCDIV_Msk      (0x7ul << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV(value)   (PM_APBCSEL_APBCDIV_Msk & ((value) << PM_APBCSEL_APBCDIV_Pos))
#define   PM_APBCSEL_APBCDIV_DIV1_Val     0x0ul  /**< \brief (PM_APBCSEL) Divide by 1 */
#define   PM_APBCSEL_APBCDIV_DIV2_Val     0x1ul  /**< \brief (PM_APBCSEL) Divide by 2 */
#define   PM_APBCSEL_APBCDIV_DIV4_Val     0x2ul  /**< \brief (PM_APBCSEL) Divide by 4 */
#define   PM_APBCSEL_APBCDIV_DIV8_Val     0x3ul  /**< \brief (PM_APBCSEL) Divide by 8 */
#define   PM_APBCSEL_APBCDIV_DIV16_Val    0x4ul  /**< \brief (PM_APBCSEL) Divide by 16 */
#define   PM_APBCSEL_APBCDIV_DIV32_Val    0x5ul  /**< \brief (PM_APBCSEL) Divide by 32 */
#define   PM_APBCSEL_APBCDIV_DIV64_Val    0x6ul  /**< \brief (PM_APBCSEL) Divide by 64 */
#define   PM_APBCSEL_APBCDIV_DIV128_Val   0x7ul  /**< \brief (PM_APBCSEL) Divide by 128 */
#define PM_APBCSEL_APBCDIV_DIV1     (PM_APBCSEL_APBCDIV_DIV1_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV2     (PM_APBCSEL_APBCDIV_DIV2_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV4     (PM_APBCSEL_APBCDIV_DIV4_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV8     (PM_APBCSEL_APBCDIV_DIV8_Val   << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV16    (PM_APBCSEL_APBCDIV_DIV16_Val  << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV32    (PM_APBCSEL_APBCDIV_DIV32_Val  << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV64    (PM_APBCSEL_APBCDIV_DIV64_Val  << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_APBCDIV_DIV128   (PM_APBCSEL_APBCDIV_DIV128_Val << PM_APBCSEL_APBCDIV_Pos)
#define PM_APBCSEL_MASK             0x07ul       /**< \brief (PM_APBCSEL) MASK Register */

/* -------- PM_AHBMASK : (PM Offset: 0x14) (R/W 32) AHB Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t HPB0_:1;          /*!< bit:      0  HPB0 AHB Clock Mask                */
    uint32_t HPB1_:1;          /*!< bit:      1  HPB1 AHB Clock Mask                */
    uint32_t HPB2_:1;          /*!< bit:      2  HPB2 AHB Clock Mask                */
    uint32_t DSU_:1;           /*!< bit:      3  DSU AHB Clock Mask                 */
    uint32_t NVMCTRL_:1;       /*!< bit:      4  NVMCTRL AHB Clock Mask             */
    uint32_t DMAC_:1;          /*!< bit:      5  DMAC AHB Clock Mask                */
    uint32_t :26;              /*!< bit:  6..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_AHBMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_AHBMASK_OFFSET           0x14         /**< \brief (PM_AHBMASK offset) AHB Mask */
#define PM_AHBMASK_RESETVALUE       0x0000007Ful /**< \brief (PM_AHBMASK reset_value) AHB Mask */

#define PM_AHBMASK_HPB0_Pos         0            /**< \brief (PM_AHBMASK) HPB0 AHB Clock Mask */
#define PM_AHBMASK_HPB0             (0x1ul << PM_AHBMASK_HPB0_Pos)
#define PM_AHBMASK_HPB1_Pos         1            /**< \brief (PM_AHBMASK) HPB1 AHB Clock Mask */
#define PM_AHBMASK_HPB1             (0x1ul << PM_AHBMASK_HPB1_Pos)
#define PM_AHBMASK_HPB2_Pos         2            /**< \brief (PM_AHBMASK) HPB2 AHB Clock Mask */
#define PM_AHBMASK_HPB2             (0x1ul << PM_AHBMASK_HPB2_Pos)
#define PM_AHBMASK_DSU_Pos          3            /**< \brief (PM_AHBMASK) DSU AHB Clock Mask */
#define PM_AHBMASK_DSU              (0x1ul << PM_AHBMASK_DSU_Pos)
#define PM_AHBMASK_NVMCTRL_Pos      4            /**< \brief (PM_AHBMASK) NVMCTRL AHB Clock Mask */
#define PM_AHBMASK_NVMCTRL          (0x1ul << PM_AHBMASK_NVMCTRL_Pos)
#define PM_AHBMASK_DMAC_Pos         5            /**< \brief (PM_AHBMASK) DMAC AHB Clock Mask */
#define PM_AHBMASK_DMAC             (0x1ul << PM_AHBMASK_DMAC_Pos)
#define PM_AHBMASK_MASK             0x0000003Ful /**< \brief (PM_AHBMASK) MASK Register */

/* -------- PM_APBAMASK : (PM Offset: 0x18) (R/W 32) APBA Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC0_:1;          /*!< bit:      0  PAC0 APB Clock Enable              */
    uint32_t PM_:1;            /*!< bit:      1  PM APB Clock Enable                */
    uint32_t SYSCTRL_:1;       /*!< bit:      2  SYSCTRL APB Clock Enable           */
    uint32_t GCLK_:1;          /*!< bit:      3  GCLK APB Clock Enable              */
    uint32_t WDT_:1;           /*!< bit:      4  WDT APB Clock Enable               */
    uint32_t RTC_:1;           /*!< bit:      5  RTC APB Clock Enable               */
    uint32_t EIC_:1;           /*!< bit:      6  EIC APB Clock Enable               */
    uint32_t :25;              /*!< bit:  7..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_APBAMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBAMASK_OFFSET          0x18         /**< \brief (PM_APBAMASK offset) APBA Mask */
#define PM_APBAMASK_RESETVALUE      0x0000007Ful /**< \brief (PM_APBAMASK reset_value) APBA Mask */

#define PM_APBAMASK_PAC0_Pos        0            /**< \brief (PM_APBAMASK) PAC0 APB Clock Enable */
#define PM_APBAMASK_PAC0            (0x1ul << PM_APBAMASK_PAC0_Pos)
#define PM_APBAMASK_PM_Pos          1            /**< \brief (PM_APBAMASK) PM APB Clock Enable */
#define PM_APBAMASK_PM              (0x1ul << PM_APBAMASK_PM_Pos)
#define PM_APBAMASK_SYSCTRL_Pos     2            /**< \brief (PM_APBAMASK) SYSCTRL APB Clock Enable */
#define PM_APBAMASK_SYSCTRL         (0x1ul << PM_APBAMASK_SYSCTRL_Pos)
#define PM_APBAMASK_GCLK_Pos        3            /**< \brief (PM_APBAMASK) GCLK APB Clock Enable */
#define PM_APBAMASK_GCLK            (0x1ul << PM_APBAMASK_GCLK_Pos)
#define PM_APBAMASK_WDT_Pos         4            /**< \brief (PM_APBAMASK) WDT APB Clock Enable */
#define PM_APBAMASK_WDT             (0x1ul << PM_APBAMASK_WDT_Pos)
#define PM_APBAMASK_RTC_Pos         5            /**< \brief (PM_APBAMASK) RTC APB Clock Enable */
#define PM_APBAMASK_RTC             (0x1ul << PM_APBAMASK_RTC_Pos)
#define PM_APBAMASK_EIC_Pos         6            /**< \brief (PM_APBAMASK) EIC APB Clock Enable */
#define PM_APBAMASK_EIC             (0x1ul << PM_APBAMASK_EIC_Pos)
#define PM_APBAMASK_MASK            0x0000007Ful /**< \brief (PM_APBAMASK) MASK Register */

/* -------- PM_APBBMASK : (PM Offset: 0x1C) (R/W 32) APBB Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC1_:1;          /*!< bit:      0  PAC1 APB Clock Enable              */
    uint32_t DSU_:1;           /*!< bit:      1  DSU APB Clock Enable               */
    uint32_t NVMCTRL_:1;       /*!< bit:      2  NVMCTRL APB Clock Enable           */
    uint32_t PORT_:1;          /*!< bit:      3  PORT APB Clock Enable              */
    uint32_t DMAC_:1;          /*!< bit:      4  DMAC APB Clock Enable              */
    uint32_t :1;               /*!< bit:      5  Reserved                           */
    uint32_t HMATRIX_:1;       /*!< bit:      6  HMATRIX APB Clock Enable           */
    uint32_t :25;              /*!< bit:  7..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_APBBMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBBMASK_OFFSET          0x1C         /**< \brief (PM_APBBMASK offset) APBB Mask */
#define PM_APBBMASK_RESETVALUE      0x0000007Ful /**< \brief (PM_APBBMASK reset_value) APBB Mask */

#define PM_APBBMASK_PAC1_Pos        0            /**< \brief (PM_APBBMASK) PAC1 APB Clock Enable */
#define PM_APBBMASK_PAC1            (0x1ul << PM_APBBMASK_PAC1_Pos)
#define PM_APBBMASK_DSU_Pos         1            /**< \brief (PM_APBBMASK) DSU APB Clock Enable */
#define PM_APBBMASK_DSU             (0x1ul << PM_APBBMASK_DSU_Pos)
#define PM_APBBMASK_NVMCTRL_Pos     2            /**< \brief (PM_APBBMASK) NVMCTRL APB Clock Enable */
#define PM_APBBMASK_NVMCTRL         (0x1ul << PM_APBBMASK_NVMCTRL_Pos)
#define PM_APBBMASK_PORT_Pos        3            /**< \brief (PM_APBBMASK) PORT APB Clock Enable */
#define PM_APBBMASK_PORT            (0x1ul << PM_APBBMASK_PORT_Pos)
#define PM_APBBMASK_DMAC_Pos        4            /**< \brief (PM_APBBMASK) DMAC APB Clock Enable */
#define PM_APBBMASK_DMAC            (0x1ul << PM_APBBMASK_DMAC_Pos)
#define PM_APBBMASK_HMATRIX_Pos     6            /**< \brief (PM_APBBMASK) HMATRIX APB Clock Enable */
#define PM_APBBMASK_HMATRIX         (0x1ul << PM_APBBMASK_HMATRIX_Pos)
#define PM_APBBMASK_MASK            0x0000005Ful /**< \brief (PM_APBBMASK) MASK Register */

/* -------- PM_APBCMASK : (PM Offset: 0x20) (R/W 32) APBC Mask -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PAC2_:1;          /*!< bit:      0  PAC2 APB Clock Enable              */
    uint32_t EVSYS_:1;         /*!< bit:      1  EVSYS APB Clock Enable             */
    uint32_t SERCOM0_:1;       /*!< bit:      2  SERCOM0 APB Clock Enable           */
    uint32_t SERCOM1_:1;       /*!< bit:      3  SERCOM1 APB Clock Enable           */
    uint32_t SERCOM2_:1;       /*!< bit:      4  SERCOM2 APB Clock Enable           */
    uint32_t TCC0_:1;          /*!< bit:      5  TCC0 APB Clock Enable              */
    uint32_t TC1_:1;           /*!< bit:      6  TC1 APB Clock Enable               */
    uint32_t TC2_:1;           /*!< bit:      7  TC2 APB Clock Enable               */
    uint32_t ADC_:1;           /*!< bit:      8  ADC APB Clock Enable               */
    uint32_t AC_:1;            /*!< bit:      9  AC APB Clock Enable                */
    uint32_t DAC_:1;           /*!< bit:     10  DAC APB Clock Enable               */
    uint32_t PTC_:1;           /*!< bit:     11  PTC APB Clock Enable               */
    uint32_t :20;              /*!< bit: 12..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} PM_APBCMASK_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_APBCMASK_OFFSET          0x20         /**< \brief (PM_APBCMASK offset) APBC Mask */
#define PM_APBCMASK_RESETVALUE      0x00000100ul /**< \brief (PM_APBCMASK reset_value) APBC Mask */

#define PM_APBCMASK_PAC2_Pos        0            /**< \brief (PM_APBCMASK) PAC2 APB Clock Enable */
#define PM_APBCMASK_PAC2            (0x1ul << PM_APBCMASK_PAC2_Pos)
#define PM_APBCMASK_EVSYS_Pos       1            /**< \brief (PM_APBCMASK) EVSYS APB Clock Enable */
#define PM_APBCMASK_EVSYS           (0x1ul << PM_APBCMASK_EVSYS_Pos)
#define PM_APBCMASK_SERCOM0_Pos     2            /**< \brief (PM_APBCMASK) SERCOM0 APB Clock Enable */
#define PM_APBCMASK_SERCOM0         (0x1ul << PM_APBCMASK_SERCOM0_Pos)
#define PM_APBCMASK_SERCOM1_Pos     3            /**< \brief (PM_APBCMASK) SERCOM1 APB Clock Enable */
#define PM_APBCMASK_SERCOM1         (0x1ul << PM_APBCMASK_SERCOM1_Pos)
#define PM_APBCMASK_SERCOM2_Pos     4            /**< \brief (PM_APBCMASK) SERCOM2 APB Clock Enable */
#define PM_APBCMASK_SERCOM2         (0x1ul << PM_APBCMASK_SERCOM2_Pos)
#define PM_APBCMASK_TCC0_Pos        5            /**< \brief (PM_APBCMASK) TCC0 APB Clock Enable */
#define PM_APBCMASK_TCC0            (0x1ul << PM_APBCMASK_TCC0_Pos)
#define PM_APBCMASK_TC1_Pos         6            /**< \brief (PM_APBCMASK) TC1 APB Clock Enable */
#define PM_APBCMASK_TC1             (0x1ul << PM_APBCMASK_TC1_Pos)
#define PM_APBCMASK_TC2_Pos         7            /**< \brief (PM_APBCMASK) TC2 APB Clock Enable */
#define PM_APBCMASK_TC2             (0x1ul << PM_APBCMASK_TC2_Pos)
#define PM_APBCMASK_ADC_Pos         8            /**< \brief (PM_APBCMASK) ADC APB Clock Enable */
#define PM_APBCMASK_ADC             (0x1ul << PM_APBCMASK_ADC_Pos)
#define PM_APBCMASK_AC_Pos          9            /**< \brief (PM_APBCMASK) AC APB Clock Enable */
#define PM_APBCMASK_AC              (0x1ul << PM_APBCMASK_AC_Pos)
#define PM_APBCMASK_DAC_Pos         10           /**< \brief (PM_APBCMASK) DAC APB Clock Enable */
#define PM_APBCMASK_DAC             (0x1ul << PM_APBCMASK_DAC_Pos)
#define PM_APBCMASK_PTC_Pos         11           /**< \brief (PM_APBCMASK) PTC APB Clock Enable */
#define PM_APBCMASK_PTC             (0x1ul << PM_APBCMASK_PTC_Pos)
#define PM_APBCMASK_MASK            0x00000FFFul /**< \brief (PM_APBCMASK) MASK Register */

/* -------- PM_INTENCLR : (PM Offset: 0x34) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready Interrupt Enable       */
    uint8_t  CFD:1;            /*!< bit:      1  Clock Failure Detector Interrupt Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTENCLR_OFFSET          0x34         /**< \brief (PM_INTENCLR offset) Interrupt Enable Clear */
#define PM_INTENCLR_RESETVALUE      0x00ul       /**< \brief (PM_INTENCLR reset_value) Interrupt Enable Clear */

#define PM_INTENCLR_CKRDY_Pos       0            /**< \brief (PM_INTENCLR) Clock Ready Interrupt Enable */
#define PM_INTENCLR_CKRDY           (0x1ul << PM_INTENCLR_CKRDY_Pos)
#define PM_INTENCLR_CFD_Pos         1            /**< \brief (PM_INTENCLR) Clock Failure Detector Interrupt Enable */
#define PM_INTENCLR_CFD             (0x1ul << PM_INTENCLR_CFD_Pos)
#define PM_INTENCLR_MASK            0x03ul       /**< \brief (PM_INTENCLR) MASK Register */

/* -------- PM_INTENSET : (PM Offset: 0x35) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready Interrupt Enable       */
    uint8_t  CFD:1;            /*!< bit:      1  Clock Failure Detector Interrupt Enable */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTENSET_OFFSET          0x35         /**< \brief (PM_INTENSET offset) Interrupt Enable Set */
#define PM_INTENSET_RESETVALUE      0x00ul       /**< \brief (PM_INTENSET reset_value) Interrupt Enable Set */

#define PM_INTENSET_CKRDY_Pos       0            /**< \brief (PM_INTENSET) Clock Ready Interrupt Enable */
#define PM_INTENSET_CKRDY           (0x1ul << PM_INTENSET_CKRDY_Pos)
#define PM_INTENSET_CFD_Pos         1            /**< \brief (PM_INTENSET) Clock Failure Detector Interrupt Enable */
#define PM_INTENSET_CFD             (0x1ul << PM_INTENSET_CFD_Pos)
#define PM_INTENSET_MASK            0x03ul       /**< \brief (PM_INTENSET) MASK Register */

/* -------- PM_INTFLAG : (PM Offset: 0x36) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  CKRDY:1;          /*!< bit:      0  Clock Ready                        */
    __I uint8_t  CFD:1;            /*!< bit:      1  Clock Failure Detector             */
    __I uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_INTFLAG_OFFSET           0x36         /**< \brief (PM_INTFLAG offset) Interrupt Flag Status and Clear */
#define PM_INTFLAG_RESETVALUE       0x00ul       /**< \brief (PM_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define PM_INTFLAG_CKRDY_Pos        0            /**< \brief (PM_INTFLAG) Clock Ready */
#define PM_INTFLAG_CKRDY            (0x1ul << PM_INTFLAG_CKRDY_Pos)
#define PM_INTFLAG_CFD_Pos          1            /**< \brief (PM_INTFLAG) Clock Failure Detector */
#define PM_INTFLAG_CFD              (0x1ul << PM_INTFLAG_CFD_Pos)
#define PM_INTFLAG_MASK             0x03ul       /**< \brief (PM_INTFLAG) MASK Register */

/* -------- PM_RCAUSE : (PM Offset: 0x38) (R/   8) Reset Cause -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  POR:1;            /*!< bit:      0  Power On Reset                     */
    uint8_t  BOD12:1;          /*!< bit:      1  Brown Out 12 Detector Reset        */
    uint8_t  BOD33:1;          /*!< bit:      2  Brown Out 33 Detector Reset        */
    uint8_t  :1;               /*!< bit:      3  Reserved                           */
    uint8_t  EXT:1;            /*!< bit:      4  External Reset                     */
    uint8_t  WDT:1;            /*!< bit:      5  Watchdog Reset                     */
    uint8_t  SYST:1;           /*!< bit:      6  System Reset Request               */
    uint8_t  :1;               /*!< bit:      7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} PM_RCAUSE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define PM_RCAUSE_OFFSET            0x38         /**< \brief (PM_RCAUSE offset) Reset Cause */
#define PM_RCAUSE_RESETVALUE        0x01ul       /**< \brief (PM_RCAUSE reset_value) Reset Cause */

#define PM_RCAUSE_POR_Pos           0            /**< \brief (PM_RCAUSE) Power On Reset */
#define PM_RCAUSE_POR               (0x1ul << PM_RCAUSE_POR_Pos)
#define PM_RCAUSE_BOD12_Pos         1            /**< \brief (PM_RCAUSE) Brown Out 12 Detector Reset */
#define PM_RCAUSE_BOD12             (0x1ul << PM_RCAUSE_BOD12_Pos)
#define PM_RCAUSE_BOD33_Pos         2            /**< \brief (PM_RCAUSE) Brown Out 33 Detector Reset */
#define PM_RCAUSE_BOD33             (0x1ul << PM_RCAUSE_BOD33_Pos)
#define PM_RCAUSE_EXT_Pos           4            /**< \brief (PM_RCAUSE) External Reset */
#define PM_RCAUSE_EXT               (0x1ul << PM_RCAUSE_EXT_Pos)
#define PM_RCAUSE_WDT_Pos           5            /**< \brief (PM_RCAUSE) Watchdog Reset */
#define PM_RCAUSE_WDT               (0x1ul << PM_RCAUSE_WDT_Pos)
#define PM_RCAUSE_SYST_Pos          6            /**< \brief (PM_RCAUSE) System Reset Request */
#define PM_RCAUSE_SYST              (0x1ul << PM_RCAUSE_SYST_Pos)
#define PM_RCAUSE_MASK              0x77ul       /**< \brief (PM_RCAUSE) MASK Register */

/** \brief PM hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO PM_CTRL_Type              CTRL;        /**< \brief Offset: 0x00 (R/W  8) Control */
  __IO PM_SLEEP_Type             SLEEP;       /**< \brief Offset: 0x01 (R/W  8) Sleep Mode */
  __IO PM_EXTCTRL_Type           EXTCTRL;     /**< \brief Offset: 0x02 (R/W  8) External Reset Controller */
       RoReg8                    Reserved1[0x5];
  __IO PM_CPUSEL_Type            CPUSEL;      /**< \brief Offset: 0x08 (R/W  8) CPU Clock Select */
  __IO PM_APBASEL_Type           APBASEL;     /**< \brief Offset: 0x09 (R/W  8) APBA Clock Select */
  __IO PM_APBBSEL_Type           APBBSEL;     /**< \brief Offset: 0x0A (R/W  8) APBB Clock Select */
  __IO PM_APBCSEL_Type           APBCSEL;     /**< \brief Offset: 0x0B (R/W  8) APBC Clock Select */
       RoReg8                    Reserved2[0x8];
  __IO PM_AHBMASK_Type           AHBMASK;     /**< \brief Offset: 0x14 (R/W 32) AHB Mask */
  __IO PM_APBAMASK_Type          APBAMASK;    /**< \brief Offset: 0x18 (R/W 32) APBA Mask */
  __IO PM_APBBMASK_Type          APBBMASK;    /**< \brief Offset: 0x1C (R/W 32) APBB Mask */
  __IO PM_APBCMASK_Type          APBCMASK;    /**< \brief Offset: 0x20 (R/W 32) APBC Mask */
       RoReg8                    Reserved3[0x10];
  __IO PM_INTENCLR_Type          INTENCLR;    /**< \brief Offset: 0x34 (R/W  8) Interrupt Enable Clear */
  __IO PM_INTENSET_Type          INTENSET;    /**< \brief Offset: 0x35 (R/W  8) Interrupt Enable Set */
  __IO PM_INTFLAG_Type           INTFLAG;     /**< \brief Offset: 0x36 (R/W  8) Interrupt Flag Status and Clear */
       RoReg8                    Reserved4[0x1];
  __I  PM_RCAUSE_Type            RCAUSE;      /**< \brief Offset: 0x38 (R/   8) Reset Cause */
} Pm;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD10_PM_COMPONENT_ */
