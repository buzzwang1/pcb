/**
 * \file
 *
 * \brief Component description for EIC
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

#ifndef _SAMD10_EIC_COMPONENT_
#define _SAMD10_EIC_COMPONENT_

 /* ========== Register definition for EIC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EIC_CTRL               (0x40001800U) /**< \brief (EIC) Control */
#define REG_EIC_STATUS             (0x40001801U) /**< \brief (EIC) Status */
#define REG_EIC_NMICTRL            (0x40001802U) /**< \brief (EIC) Non-Maskable Interrupt Control */
#define REG_EIC_NMIFLAG            (0x40001803U) /**< \brief (EIC) Non-Maskable Interrupt Flag Status and Clear */
#define REG_EIC_EVCTRL             (0x40001804U) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (0x40001808U) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (0x4000180CU) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (0x40001810U) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_WAKEUP             (0x40001814U) /**< \brief (EIC) Wake-Up Enable */
#define REG_EIC_CONFIG0            (0x40001818U) /**< \brief (EIC) Configuration 0 */
#else
#define REG_EIC_CTRL               (*(RwReg8 *)0x40001800U) /**< \brief (EIC) Control */
#define REG_EIC_STATUS             (*(RoReg8 *)0x40001801U) /**< \brief (EIC) Status */
#define REG_EIC_NMICTRL            (*(RwReg8 *)0x40001802U) /**< \brief (EIC) Non-Maskable Interrupt Control */
#define REG_EIC_NMIFLAG            (*(RwReg8 *)0x40001803U) /**< \brief (EIC) Non-Maskable Interrupt Flag Status and Clear */
#define REG_EIC_EVCTRL             (*(RwReg  *)0x40001804U) /**< \brief (EIC) Event Control */
#define REG_EIC_INTENCLR           (*(RwReg  *)0x40001808U) /**< \brief (EIC) Interrupt Enable Clear */
#define REG_EIC_INTENSET           (*(RwReg  *)0x4000180CU) /**< \brief (EIC) Interrupt Enable Set */
#define REG_EIC_INTFLAG            (*(RwReg  *)0x40001810U) /**< \brief (EIC) Interrupt Flag Status and Clear */
#define REG_EIC_WAKEUP             (*(RwReg  *)0x40001814U) /**< \brief (EIC) Wake-Up Enable */
#define REG_EIC_CONFIG0            (*(RwReg  *)0x40001818U) /**< \brief (EIC) Configuration 0 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EIC peripheral ========== */
#define EIC_CONFIG_NUM              1        // Number of CONFIG registers
#define EIC_EXTINT_NUM              8        // Number of External Interrupts
#define EIC_GCLK_ID                 5        // Index of Generic Clock

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR EIC */
/* ========================================================================== */
/** \addtogroup SAMD10_EIC External Interrupt Controller */
/*@{*/

#define EIC_U2217
#define REV_EIC                     0x101

/* -------- EIC_CTRL : (EIC Offset: 0x00) (R/W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CTRL_OFFSET             0x00         /**< \brief (EIC_CTRL offset) Control */
#define EIC_CTRL_RESETVALUE         0x00ul       /**< \brief (EIC_CTRL reset_value) Control */

#define EIC_CTRL_SWRST_Pos          0            /**< \brief (EIC_CTRL) Software Reset */
#define EIC_CTRL_SWRST              (0x1ul << EIC_CTRL_SWRST_Pos)
#define EIC_CTRL_ENABLE_Pos         1            /**< \brief (EIC_CTRL) Enable */
#define EIC_CTRL_ENABLE             (0x1ul << EIC_CTRL_ENABLE_Pos)
#define EIC_CTRL_MASK               0x03ul       /**< \brief (EIC_CTRL) MASK Register */

/* -------- EIC_STATUS : (EIC Offset: 0x01) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_STATUS_OFFSET           0x01         /**< \brief (EIC_STATUS offset) Status */
#define EIC_STATUS_RESETVALUE       0x00ul       /**< \brief (EIC_STATUS reset_value) Status */

#define EIC_STATUS_SYNCBUSY_Pos     7            /**< \brief (EIC_STATUS) Synchronization Busy */
#define EIC_STATUS_SYNCBUSY         (0x1ul << EIC_STATUS_SYNCBUSY_Pos)
#define EIC_STATUS_MASK             0x80ul       /**< \brief (EIC_STATUS) MASK Register */

/* -------- EIC_NMICTRL : (EIC Offset: 0x02) (R/W  8) Non-Maskable Interrupt Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  NMISENSE:3;       /*!< bit:  0.. 2  Non-Maskable Interrupt Sense       */
    uint8_t  NMIFILTEN:1;      /*!< bit:      3  Non-Maskable Interrupt Filter Enable */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_NMICTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMICTRL_OFFSET          0x02         /**< \brief (EIC_NMICTRL offset) Non-Maskable Interrupt Control */
#define EIC_NMICTRL_RESETVALUE      0x00ul       /**< \brief (EIC_NMICTRL reset_value) Non-Maskable Interrupt Control */

#define EIC_NMICTRL_NMISENSE_Pos    0            /**< \brief (EIC_NMICTRL) Non-Maskable Interrupt Sense */
#define EIC_NMICTRL_NMISENSE_Msk    (0x7ul << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE(value) (EIC_NMICTRL_NMISENSE_Msk & ((value) << EIC_NMICTRL_NMISENSE_Pos))
#define   EIC_NMICTRL_NMISENSE_NONE_Val   0x0ul  /**< \brief (EIC_NMICTRL) No detection */
#define   EIC_NMICTRL_NMISENSE_RISE_Val   0x1ul  /**< \brief (EIC_NMICTRL) Rising-edge detection */
#define   EIC_NMICTRL_NMISENSE_FALL_Val   0x2ul  /**< \brief (EIC_NMICTRL) Falling-edge detection */
#define   EIC_NMICTRL_NMISENSE_BOTH_Val   0x3ul  /**< \brief (EIC_NMICTRL) Both-edges detection */
#define   EIC_NMICTRL_NMISENSE_HIGH_Val   0x4ul  /**< \brief (EIC_NMICTRL) High-level detection */
#define   EIC_NMICTRL_NMISENSE_LOW_Val    0x5ul  /**< \brief (EIC_NMICTRL) Low-level detection */
#define EIC_NMICTRL_NMISENSE_NONE   (EIC_NMICTRL_NMISENSE_NONE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_RISE   (EIC_NMICTRL_NMISENSE_RISE_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_FALL   (EIC_NMICTRL_NMISENSE_FALL_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_BOTH   (EIC_NMICTRL_NMISENSE_BOTH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_HIGH   (EIC_NMICTRL_NMISENSE_HIGH_Val << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMISENSE_LOW    (EIC_NMICTRL_NMISENSE_LOW_Val  << EIC_NMICTRL_NMISENSE_Pos)
#define EIC_NMICTRL_NMIFILTEN_Pos   3            /**< \brief (EIC_NMICTRL) Non-Maskable Interrupt Filter Enable */
#define EIC_NMICTRL_NMIFILTEN       (0x1ul << EIC_NMICTRL_NMIFILTEN_Pos)
#define EIC_NMICTRL_MASK            0x0Ful       /**< \brief (EIC_NMICTRL) MASK Register */

/* -------- EIC_NMIFLAG : (EIC Offset: 0x03) (R/W  8) Non-Maskable Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  NMI:1;            /*!< bit:      0  Non-Maskable Interrupt             */
    uint8_t  :7;               /*!< bit:  1.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EIC_NMIFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_NMIFLAG_OFFSET          0x03         /**< \brief (EIC_NMIFLAG offset) Non-Maskable Interrupt Flag Status and Clear */
#define EIC_NMIFLAG_RESETVALUE      0x00ul       /**< \brief (EIC_NMIFLAG reset_value) Non-Maskable Interrupt Flag Status and Clear */

#define EIC_NMIFLAG_NMI_Pos         0            /**< \brief (EIC_NMIFLAG) Non-Maskable Interrupt */
#define EIC_NMIFLAG_NMI             (0x1ul << EIC_NMIFLAG_NMI_Pos)
#define EIC_NMIFLAG_MASK            0x01ul       /**< \brief (EIC_NMIFLAG) MASK Register */

/* -------- EIC_EVCTRL : (EIC Offset: 0x04) (R/W 32) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINTEO0:1;      /*!< bit:      0  External Interrupt 0 Event Output Enable */
    uint32_t EXTINTEO1:1;      /*!< bit:      1  External Interrupt 1 Event Output Enable */
    uint32_t EXTINTEO2:1;      /*!< bit:      2  External Interrupt 2 Event Output Enable */
    uint32_t EXTINTEO3:1;      /*!< bit:      3  External Interrupt 3 Event Output Enable */
    uint32_t EXTINTEO4:1;      /*!< bit:      4  External Interrupt 4 Event Output Enable */
    uint32_t EXTINTEO5:1;      /*!< bit:      5  External Interrupt 5 Event Output Enable */
    uint32_t EXTINTEO6:1;      /*!< bit:      6  External Interrupt 6 Event Output Enable */
    uint32_t EXTINTEO7:1;      /*!< bit:      7  External Interrupt 7 Event Output Enable */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t EXTINTEO:8;       /*!< bit:  0.. 7  External Interrupt x Event Output Enable */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_EVCTRL_OFFSET           0x04         /**< \brief (EIC_EVCTRL offset) Event Control */
#define EIC_EVCTRL_RESETVALUE       0x00000000ul /**< \brief (EIC_EVCTRL reset_value) Event Control */

#define EIC_EVCTRL_EXTINTEO0_Pos    0            /**< \brief (EIC_EVCTRL) External Interrupt 0 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO0        (1 << EIC_EVCTRL_EXTINTEO0_Pos)
#define EIC_EVCTRL_EXTINTEO1_Pos    1            /**< \brief (EIC_EVCTRL) External Interrupt 1 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO1        (1 << EIC_EVCTRL_EXTINTEO1_Pos)
#define EIC_EVCTRL_EXTINTEO2_Pos    2            /**< \brief (EIC_EVCTRL) External Interrupt 2 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO2        (1 << EIC_EVCTRL_EXTINTEO2_Pos)
#define EIC_EVCTRL_EXTINTEO3_Pos    3            /**< \brief (EIC_EVCTRL) External Interrupt 3 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO3        (1 << EIC_EVCTRL_EXTINTEO3_Pos)
#define EIC_EVCTRL_EXTINTEO4_Pos    4            /**< \brief (EIC_EVCTRL) External Interrupt 4 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO4        (1 << EIC_EVCTRL_EXTINTEO4_Pos)
#define EIC_EVCTRL_EXTINTEO5_Pos    5            /**< \brief (EIC_EVCTRL) External Interrupt 5 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO5        (1 << EIC_EVCTRL_EXTINTEO5_Pos)
#define EIC_EVCTRL_EXTINTEO6_Pos    6            /**< \brief (EIC_EVCTRL) External Interrupt 6 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO6        (1 << EIC_EVCTRL_EXTINTEO6_Pos)
#define EIC_EVCTRL_EXTINTEO7_Pos    7            /**< \brief (EIC_EVCTRL) External Interrupt 7 Event Output Enable */
#define EIC_EVCTRL_EXTINTEO7        (1 << EIC_EVCTRL_EXTINTEO7_Pos)
#define EIC_EVCTRL_EXTINTEO_Pos     0            /**< \brief (EIC_EVCTRL) External Interrupt x Event Output Enable */
#define EIC_EVCTRL_EXTINTEO_Msk     (0xFFul << EIC_EVCTRL_EXTINTEO_Pos)
#define EIC_EVCTRL_EXTINTEO(value)  (EIC_EVCTRL_EXTINTEO_Msk & ((value) << EIC_EVCTRL_EXTINTEO_Pos))
#define EIC_EVCTRL_MASK             0x000000FFul /**< \brief (EIC_EVCTRL) MASK Register */

/* -------- EIC_INTENCLR : (EIC Offset: 0x08) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT0:1;        /*!< bit:      0  External Interrupt 0 Enable        */
    uint32_t EXTINT1:1;        /*!< bit:      1  External Interrupt 1 Enable        */
    uint32_t EXTINT2:1;        /*!< bit:      2  External Interrupt 2 Enable        */
    uint32_t EXTINT3:1;        /*!< bit:      3  External Interrupt 3 Enable        */
    uint32_t EXTINT4:1;        /*!< bit:      4  External Interrupt 4 Enable        */
    uint32_t EXTINT5:1;        /*!< bit:      5  External Interrupt 5 Enable        */
    uint32_t EXTINT6:1;        /*!< bit:      6  External Interrupt 6 Enable        */
    uint32_t EXTINT7:1;        /*!< bit:      7  External Interrupt 7 Enable        */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t EXTINT:8;         /*!< bit:  0.. 7  External Interrupt x Enable        */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENCLR_OFFSET         0x08         /**< \brief (EIC_INTENCLR offset) Interrupt Enable Clear */
#define EIC_INTENCLR_RESETVALUE     0x00000000ul /**< \brief (EIC_INTENCLR reset_value) Interrupt Enable Clear */

#define EIC_INTENCLR_EXTINT0_Pos    0            /**< \brief (EIC_INTENCLR) External Interrupt 0 Enable */
#define EIC_INTENCLR_EXTINT0        (1 << EIC_INTENCLR_EXTINT0_Pos)
#define EIC_INTENCLR_EXTINT1_Pos    1            /**< \brief (EIC_INTENCLR) External Interrupt 1 Enable */
#define EIC_INTENCLR_EXTINT1        (1 << EIC_INTENCLR_EXTINT1_Pos)
#define EIC_INTENCLR_EXTINT2_Pos    2            /**< \brief (EIC_INTENCLR) External Interrupt 2 Enable */
#define EIC_INTENCLR_EXTINT2        (1 << EIC_INTENCLR_EXTINT2_Pos)
#define EIC_INTENCLR_EXTINT3_Pos    3            /**< \brief (EIC_INTENCLR) External Interrupt 3 Enable */
#define EIC_INTENCLR_EXTINT3        (1 << EIC_INTENCLR_EXTINT3_Pos)
#define EIC_INTENCLR_EXTINT4_Pos    4            /**< \brief (EIC_INTENCLR) External Interrupt 4 Enable */
#define EIC_INTENCLR_EXTINT4        (1 << EIC_INTENCLR_EXTINT4_Pos)
#define EIC_INTENCLR_EXTINT5_Pos    5            /**< \brief (EIC_INTENCLR) External Interrupt 5 Enable */
#define EIC_INTENCLR_EXTINT5        (1 << EIC_INTENCLR_EXTINT5_Pos)
#define EIC_INTENCLR_EXTINT6_Pos    6            /**< \brief (EIC_INTENCLR) External Interrupt 6 Enable */
#define EIC_INTENCLR_EXTINT6        (1 << EIC_INTENCLR_EXTINT6_Pos)
#define EIC_INTENCLR_EXTINT7_Pos    7            /**< \brief (EIC_INTENCLR) External Interrupt 7 Enable */
#define EIC_INTENCLR_EXTINT7        (1 << EIC_INTENCLR_EXTINT7_Pos)
#define EIC_INTENCLR_EXTINT_Pos     0            /**< \brief (EIC_INTENCLR) External Interrupt x Enable */
#define EIC_INTENCLR_EXTINT_Msk     (0xFFul << EIC_INTENCLR_EXTINT_Pos)
#define EIC_INTENCLR_EXTINT(value)  (EIC_INTENCLR_EXTINT_Msk & ((value) << EIC_INTENCLR_EXTINT_Pos))
#define EIC_INTENCLR_MASK           0x000000FFul /**< \brief (EIC_INTENCLR) MASK Register */

/* -------- EIC_INTENSET : (EIC Offset: 0x0C) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EXTINT0:1;        /*!< bit:      0  External Interrupt 0 Enable        */
    uint32_t EXTINT1:1;        /*!< bit:      1  External Interrupt 1 Enable        */
    uint32_t EXTINT2:1;        /*!< bit:      2  External Interrupt 2 Enable        */
    uint32_t EXTINT3:1;        /*!< bit:      3  External Interrupt 3 Enable        */
    uint32_t EXTINT4:1;        /*!< bit:      4  External Interrupt 4 Enable        */
    uint32_t EXTINT5:1;        /*!< bit:      5  External Interrupt 5 Enable        */
    uint32_t EXTINT6:1;        /*!< bit:      6  External Interrupt 6 Enable        */
    uint32_t EXTINT7:1;        /*!< bit:      7  External Interrupt 7 Enable        */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t EXTINT:8;         /*!< bit:  0.. 7  External Interrupt x Enable        */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTENSET_OFFSET         0x0C         /**< \brief (EIC_INTENSET offset) Interrupt Enable Set */
#define EIC_INTENSET_RESETVALUE     0x00000000ul /**< \brief (EIC_INTENSET reset_value) Interrupt Enable Set */

#define EIC_INTENSET_EXTINT0_Pos    0            /**< \brief (EIC_INTENSET) External Interrupt 0 Enable */
#define EIC_INTENSET_EXTINT0        (1 << EIC_INTENSET_EXTINT0_Pos)
#define EIC_INTENSET_EXTINT1_Pos    1            /**< \brief (EIC_INTENSET) External Interrupt 1 Enable */
#define EIC_INTENSET_EXTINT1        (1 << EIC_INTENSET_EXTINT1_Pos)
#define EIC_INTENSET_EXTINT2_Pos    2            /**< \brief (EIC_INTENSET) External Interrupt 2 Enable */
#define EIC_INTENSET_EXTINT2        (1 << EIC_INTENSET_EXTINT2_Pos)
#define EIC_INTENSET_EXTINT3_Pos    3            /**< \brief (EIC_INTENSET) External Interrupt 3 Enable */
#define EIC_INTENSET_EXTINT3        (1 << EIC_INTENSET_EXTINT3_Pos)
#define EIC_INTENSET_EXTINT4_Pos    4            /**< \brief (EIC_INTENSET) External Interrupt 4 Enable */
#define EIC_INTENSET_EXTINT4        (1 << EIC_INTENSET_EXTINT4_Pos)
#define EIC_INTENSET_EXTINT5_Pos    5            /**< \brief (EIC_INTENSET) External Interrupt 5 Enable */
#define EIC_INTENSET_EXTINT5        (1 << EIC_INTENSET_EXTINT5_Pos)
#define EIC_INTENSET_EXTINT6_Pos    6            /**< \brief (EIC_INTENSET) External Interrupt 6 Enable */
#define EIC_INTENSET_EXTINT6        (1 << EIC_INTENSET_EXTINT6_Pos)
#define EIC_INTENSET_EXTINT7_Pos    7            /**< \brief (EIC_INTENSET) External Interrupt 7 Enable */
#define EIC_INTENSET_EXTINT7        (1 << EIC_INTENSET_EXTINT7_Pos)
#define EIC_INTENSET_EXTINT_Pos     0            /**< \brief (EIC_INTENSET) External Interrupt x Enable */
#define EIC_INTENSET_EXTINT_Msk     (0xFFul << EIC_INTENSET_EXTINT_Pos)
#define EIC_INTENSET_EXTINT(value)  (EIC_INTENSET_EXTINT_Msk & ((value) << EIC_INTENSET_EXTINT_Pos))
#define EIC_INTENSET_MASK           0x000000FFul /**< \brief (EIC_INTENSET) MASK Register */

/* -------- EIC_INTFLAG : (EIC Offset: 0x10) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t EXTINT0:1;        /*!< bit:      0  External Interrupt 0               */
    __I uint32_t EXTINT1:1;        /*!< bit:      1  External Interrupt 1               */
    __I uint32_t EXTINT2:1;        /*!< bit:      2  External Interrupt 2               */
    __I uint32_t EXTINT3:1;        /*!< bit:      3  External Interrupt 3               */
    __I uint32_t EXTINT4:1;        /*!< bit:      4  External Interrupt 4               */
    __I uint32_t EXTINT5:1;        /*!< bit:      5  External Interrupt 5               */
    __I uint32_t EXTINT6:1;        /*!< bit:      6  External Interrupt 6               */
    __I uint32_t EXTINT7:1;        /*!< bit:      7  External Interrupt 7               */
    __I uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint32_t EXTINT:8;         /*!< bit:  0.. 7  External Interrupt x               */
    __I uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_INTFLAG_OFFSET          0x10         /**< \brief (EIC_INTFLAG offset) Interrupt Flag Status and Clear */
#define EIC_INTFLAG_RESETVALUE      0x00000000ul /**< \brief (EIC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define EIC_INTFLAG_EXTINT0_Pos     0            /**< \brief (EIC_INTFLAG) External Interrupt 0 */
#define EIC_INTFLAG_EXTINT0         (1 << EIC_INTFLAG_EXTINT0_Pos)
#define EIC_INTFLAG_EXTINT1_Pos     1            /**< \brief (EIC_INTFLAG) External Interrupt 1 */
#define EIC_INTFLAG_EXTINT1         (1 << EIC_INTFLAG_EXTINT1_Pos)
#define EIC_INTFLAG_EXTINT2_Pos     2            /**< \brief (EIC_INTFLAG) External Interrupt 2 */
#define EIC_INTFLAG_EXTINT2         (1 << EIC_INTFLAG_EXTINT2_Pos)
#define EIC_INTFLAG_EXTINT3_Pos     3            /**< \brief (EIC_INTFLAG) External Interrupt 3 */
#define EIC_INTFLAG_EXTINT3         (1 << EIC_INTFLAG_EXTINT3_Pos)
#define EIC_INTFLAG_EXTINT4_Pos     4            /**< \brief (EIC_INTFLAG) External Interrupt 4 */
#define EIC_INTFLAG_EXTINT4         (1 << EIC_INTFLAG_EXTINT4_Pos)
#define EIC_INTFLAG_EXTINT5_Pos     5            /**< \brief (EIC_INTFLAG) External Interrupt 5 */
#define EIC_INTFLAG_EXTINT5         (1 << EIC_INTFLAG_EXTINT5_Pos)
#define EIC_INTFLAG_EXTINT6_Pos     6            /**< \brief (EIC_INTFLAG) External Interrupt 6 */
#define EIC_INTFLAG_EXTINT6         (1 << EIC_INTFLAG_EXTINT6_Pos)
#define EIC_INTFLAG_EXTINT7_Pos     7            /**< \brief (EIC_INTFLAG) External Interrupt 7 */
#define EIC_INTFLAG_EXTINT7         (1 << EIC_INTFLAG_EXTINT7_Pos)
#define EIC_INTFLAG_EXTINT_Pos      0            /**< \brief (EIC_INTFLAG) External Interrupt x */
#define EIC_INTFLAG_EXTINT_Msk      (0xFFul << EIC_INTFLAG_EXTINT_Pos)
#define EIC_INTFLAG_EXTINT(value)   (EIC_INTFLAG_EXTINT_Msk & ((value) << EIC_INTFLAG_EXTINT_Pos))
#define EIC_INTFLAG_MASK            0x000000FFul /**< \brief (EIC_INTFLAG) MASK Register */

/* -------- EIC_WAKEUP : (EIC Offset: 0x14) (R/W 32) Wake-Up Enable -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t WAKEUPEN0:1;      /*!< bit:      0  External Interrupt 0 Wake-up Enable */
    uint32_t WAKEUPEN1:1;      /*!< bit:      1  External Interrupt 1 Wake-up Enable */
    uint32_t WAKEUPEN2:1;      /*!< bit:      2  External Interrupt 2 Wake-up Enable */
    uint32_t WAKEUPEN3:1;      /*!< bit:      3  External Interrupt 3 Wake-up Enable */
    uint32_t WAKEUPEN4:1;      /*!< bit:      4  External Interrupt 4 Wake-up Enable */
    uint32_t WAKEUPEN5:1;      /*!< bit:      5  External Interrupt 5 Wake-up Enable */
    uint32_t WAKEUPEN6:1;      /*!< bit:      6  External Interrupt 6 Wake-up Enable */
    uint32_t WAKEUPEN7:1;      /*!< bit:      7  External Interrupt 7 Wake-up Enable */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t WAKEUPEN:8;       /*!< bit:  0.. 7  External Interrupt x Wake-up Enable */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_WAKEUP_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_WAKEUP_OFFSET           0x14         /**< \brief (EIC_WAKEUP offset) Wake-Up Enable */
#define EIC_WAKEUP_RESETVALUE       0x00000000ul /**< \brief (EIC_WAKEUP reset_value) Wake-Up Enable */

#define EIC_WAKEUP_WAKEUPEN0_Pos    0            /**< \brief (EIC_WAKEUP) External Interrupt 0 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN0        (1 << EIC_WAKEUP_WAKEUPEN0_Pos)
#define EIC_WAKEUP_WAKEUPEN1_Pos    1            /**< \brief (EIC_WAKEUP) External Interrupt 1 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN1        (1 << EIC_WAKEUP_WAKEUPEN1_Pos)
#define EIC_WAKEUP_WAKEUPEN2_Pos    2            /**< \brief (EIC_WAKEUP) External Interrupt 2 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN2        (1 << EIC_WAKEUP_WAKEUPEN2_Pos)
#define EIC_WAKEUP_WAKEUPEN3_Pos    3            /**< \brief (EIC_WAKEUP) External Interrupt 3 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN3        (1 << EIC_WAKEUP_WAKEUPEN3_Pos)
#define EIC_WAKEUP_WAKEUPEN4_Pos    4            /**< \brief (EIC_WAKEUP) External Interrupt 4 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN4        (1 << EIC_WAKEUP_WAKEUPEN4_Pos)
#define EIC_WAKEUP_WAKEUPEN5_Pos    5            /**< \brief (EIC_WAKEUP) External Interrupt 5 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN5        (1 << EIC_WAKEUP_WAKEUPEN5_Pos)
#define EIC_WAKEUP_WAKEUPEN6_Pos    6            /**< \brief (EIC_WAKEUP) External Interrupt 6 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN6        (1 << EIC_WAKEUP_WAKEUPEN6_Pos)
#define EIC_WAKEUP_WAKEUPEN7_Pos    7            /**< \brief (EIC_WAKEUP) External Interrupt 7 Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN7        (1 << EIC_WAKEUP_WAKEUPEN7_Pos)
#define EIC_WAKEUP_WAKEUPEN_Pos     0            /**< \brief (EIC_WAKEUP) External Interrupt x Wake-up Enable */
#define EIC_WAKEUP_WAKEUPEN_Msk     (0xFFul << EIC_WAKEUP_WAKEUPEN_Pos)
#define EIC_WAKEUP_WAKEUPEN(value)  (EIC_WAKEUP_WAKEUPEN_Msk & ((value) << EIC_WAKEUP_WAKEUPEN_Pos))
#define EIC_WAKEUP_MASK             0x000000FFul /**< \brief (EIC_WAKEUP) MASK Register */

/* -------- EIC_CONFIG : (EIC Offset: 0x18) (R/W 32) Configuration n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SENSE0:3;         /*!< bit:  0.. 2  Input Sense 0 Configuration        */
    uint32_t FILTEN0:1;        /*!< bit:      3  Filter 0 Enable                    */
    uint32_t SENSE1:3;         /*!< bit:  4.. 6  Input Sense 1 Configuration        */
    uint32_t FILTEN1:1;        /*!< bit:      7  Filter 1 Enable                    */
    uint32_t SENSE2:3;         /*!< bit:  8..10  Input Sense 2 Configuration        */
    uint32_t FILTEN2:1;        /*!< bit:     11  Filter 2 Enable                    */
    uint32_t SENSE3:3;         /*!< bit: 12..14  Input Sense 3 Configuration        */
    uint32_t FILTEN3:1;        /*!< bit:     15  Filter 3 Enable                    */
    uint32_t SENSE4:3;         /*!< bit: 16..18  Input Sense 4 Configuration        */
    uint32_t FILTEN4:1;        /*!< bit:     19  Filter 4 Enable                    */
    uint32_t SENSE5:3;         /*!< bit: 20..22  Input Sense 5 Configuration        */
    uint32_t FILTEN5:1;        /*!< bit:     23  Filter 5 Enable                    */
    uint32_t SENSE6:3;         /*!< bit: 24..26  Input Sense 6 Configuration        */
    uint32_t FILTEN6:1;        /*!< bit:     27  Filter 6 Enable                    */
    uint32_t SENSE7:3;         /*!< bit: 28..30  Input Sense 7 Configuration        */
    uint32_t FILTEN7:1;        /*!< bit:     31  Filter 7 Enable                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EIC_CONFIG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EIC_CONFIG_OFFSET           0x18         /**< \brief (EIC_CONFIG offset) Configuration n */
#define EIC_CONFIG_RESETVALUE       0x00000000ul /**< \brief (EIC_CONFIG reset_value) Configuration n */

#define EIC_CONFIG_SENSE0_Pos       0            /**< \brief (EIC_CONFIG) Input Sense 0 Configuration */
#define EIC_CONFIG_SENSE0_Msk       (0x7ul << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0(value)    (EIC_CONFIG_SENSE0_Msk & ((value) << EIC_CONFIG_SENSE0_Pos))
#define   EIC_CONFIG_SENSE0_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE0_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising-edge detection */
#define   EIC_CONFIG_SENSE0_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling-edge detection */
#define   EIC_CONFIG_SENSE0_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both-edges detection */
#define   EIC_CONFIG_SENSE0_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High-level detection */
#define   EIC_CONFIG_SENSE0_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low-level detection */
#define EIC_CONFIG_SENSE0_NONE      (EIC_CONFIG_SENSE0_NONE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_RISE      (EIC_CONFIG_SENSE0_RISE_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_FALL      (EIC_CONFIG_SENSE0_FALL_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_BOTH      (EIC_CONFIG_SENSE0_BOTH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_HIGH      (EIC_CONFIG_SENSE0_HIGH_Val    << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_SENSE0_LOW       (EIC_CONFIG_SENSE0_LOW_Val     << EIC_CONFIG_SENSE0_Pos)
#define EIC_CONFIG_FILTEN0_Pos      3            /**< \brief (EIC_CONFIG) Filter 0 Enable */
#define EIC_CONFIG_FILTEN0          (0x1ul << EIC_CONFIG_FILTEN0_Pos)
#define EIC_CONFIG_SENSE1_Pos       4            /**< \brief (EIC_CONFIG) Input Sense 1 Configuration */
#define EIC_CONFIG_SENSE1_Msk       (0x7ul << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1(value)    (EIC_CONFIG_SENSE1_Msk & ((value) << EIC_CONFIG_SENSE1_Pos))
#define   EIC_CONFIG_SENSE1_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE1_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE1_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE1_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE1_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE1_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE1_NONE      (EIC_CONFIG_SENSE1_NONE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_RISE      (EIC_CONFIG_SENSE1_RISE_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_FALL      (EIC_CONFIG_SENSE1_FALL_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_BOTH      (EIC_CONFIG_SENSE1_BOTH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_HIGH      (EIC_CONFIG_SENSE1_HIGH_Val    << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_SENSE1_LOW       (EIC_CONFIG_SENSE1_LOW_Val     << EIC_CONFIG_SENSE1_Pos)
#define EIC_CONFIG_FILTEN1_Pos      7            /**< \brief (EIC_CONFIG) Filter 1 Enable */
#define EIC_CONFIG_FILTEN1          (0x1ul << EIC_CONFIG_FILTEN1_Pos)
#define EIC_CONFIG_SENSE2_Pos       8            /**< \brief (EIC_CONFIG) Input Sense 2 Configuration */
#define EIC_CONFIG_SENSE2_Msk       (0x7ul << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2(value)    (EIC_CONFIG_SENSE2_Msk & ((value) << EIC_CONFIG_SENSE2_Pos))
#define   EIC_CONFIG_SENSE2_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE2_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE2_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE2_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE2_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE2_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE2_NONE      (EIC_CONFIG_SENSE2_NONE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_RISE      (EIC_CONFIG_SENSE2_RISE_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_FALL      (EIC_CONFIG_SENSE2_FALL_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_BOTH      (EIC_CONFIG_SENSE2_BOTH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_HIGH      (EIC_CONFIG_SENSE2_HIGH_Val    << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_SENSE2_LOW       (EIC_CONFIG_SENSE2_LOW_Val     << EIC_CONFIG_SENSE2_Pos)
#define EIC_CONFIG_FILTEN2_Pos      11           /**< \brief (EIC_CONFIG) Filter 2 Enable */
#define EIC_CONFIG_FILTEN2          (0x1ul << EIC_CONFIG_FILTEN2_Pos)
#define EIC_CONFIG_SENSE3_Pos       12           /**< \brief (EIC_CONFIG) Input Sense 3 Configuration */
#define EIC_CONFIG_SENSE3_Msk       (0x7ul << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3(value)    (EIC_CONFIG_SENSE3_Msk & ((value) << EIC_CONFIG_SENSE3_Pos))
#define   EIC_CONFIG_SENSE3_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE3_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE3_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE3_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE3_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE3_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE3_NONE      (EIC_CONFIG_SENSE3_NONE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_RISE      (EIC_CONFIG_SENSE3_RISE_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_FALL      (EIC_CONFIG_SENSE3_FALL_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_BOTH      (EIC_CONFIG_SENSE3_BOTH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_HIGH      (EIC_CONFIG_SENSE3_HIGH_Val    << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_SENSE3_LOW       (EIC_CONFIG_SENSE3_LOW_Val     << EIC_CONFIG_SENSE3_Pos)
#define EIC_CONFIG_FILTEN3_Pos      15           /**< \brief (EIC_CONFIG) Filter 3 Enable */
#define EIC_CONFIG_FILTEN3          (0x1ul << EIC_CONFIG_FILTEN3_Pos)
#define EIC_CONFIG_SENSE4_Pos       16           /**< \brief (EIC_CONFIG) Input Sense 4 Configuration */
#define EIC_CONFIG_SENSE4_Msk       (0x7ul << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4(value)    (EIC_CONFIG_SENSE4_Msk & ((value) << EIC_CONFIG_SENSE4_Pos))
#define   EIC_CONFIG_SENSE4_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE4_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE4_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE4_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE4_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE4_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE4_NONE      (EIC_CONFIG_SENSE4_NONE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_RISE      (EIC_CONFIG_SENSE4_RISE_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_FALL      (EIC_CONFIG_SENSE4_FALL_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_BOTH      (EIC_CONFIG_SENSE4_BOTH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_HIGH      (EIC_CONFIG_SENSE4_HIGH_Val    << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_SENSE4_LOW       (EIC_CONFIG_SENSE4_LOW_Val     << EIC_CONFIG_SENSE4_Pos)
#define EIC_CONFIG_FILTEN4_Pos      19           /**< \brief (EIC_CONFIG) Filter 4 Enable */
#define EIC_CONFIG_FILTEN4          (0x1ul << EIC_CONFIG_FILTEN4_Pos)
#define EIC_CONFIG_SENSE5_Pos       20           /**< \brief (EIC_CONFIG) Input Sense 5 Configuration */
#define EIC_CONFIG_SENSE5_Msk       (0x7ul << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5(value)    (EIC_CONFIG_SENSE5_Msk & ((value) << EIC_CONFIG_SENSE5_Pos))
#define   EIC_CONFIG_SENSE5_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE5_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE5_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE5_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE5_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE5_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE5_NONE      (EIC_CONFIG_SENSE5_NONE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_RISE      (EIC_CONFIG_SENSE5_RISE_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_FALL      (EIC_CONFIG_SENSE5_FALL_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_BOTH      (EIC_CONFIG_SENSE5_BOTH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_HIGH      (EIC_CONFIG_SENSE5_HIGH_Val    << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_SENSE5_LOW       (EIC_CONFIG_SENSE5_LOW_Val     << EIC_CONFIG_SENSE5_Pos)
#define EIC_CONFIG_FILTEN5_Pos      23           /**< \brief (EIC_CONFIG) Filter 5 Enable */
#define EIC_CONFIG_FILTEN5          (0x1ul << EIC_CONFIG_FILTEN5_Pos)
#define EIC_CONFIG_SENSE6_Pos       24           /**< \brief (EIC_CONFIG) Input Sense 6 Configuration */
#define EIC_CONFIG_SENSE6_Msk       (0x7ul << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6(value)    (EIC_CONFIG_SENSE6_Msk & ((value) << EIC_CONFIG_SENSE6_Pos))
#define   EIC_CONFIG_SENSE6_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE6_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE6_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE6_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE6_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE6_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE6_NONE      (EIC_CONFIG_SENSE6_NONE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_RISE      (EIC_CONFIG_SENSE6_RISE_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_FALL      (EIC_CONFIG_SENSE6_FALL_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_BOTH      (EIC_CONFIG_SENSE6_BOTH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_HIGH      (EIC_CONFIG_SENSE6_HIGH_Val    << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_SENSE6_LOW       (EIC_CONFIG_SENSE6_LOW_Val     << EIC_CONFIG_SENSE6_Pos)
#define EIC_CONFIG_FILTEN6_Pos      27           /**< \brief (EIC_CONFIG) Filter 6 Enable */
#define EIC_CONFIG_FILTEN6          (0x1ul << EIC_CONFIG_FILTEN6_Pos)
#define EIC_CONFIG_SENSE7_Pos       28           /**< \brief (EIC_CONFIG) Input Sense 7 Configuration */
#define EIC_CONFIG_SENSE7_Msk       (0x7ul << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7(value)    (EIC_CONFIG_SENSE7_Msk & ((value) << EIC_CONFIG_SENSE7_Pos))
#define   EIC_CONFIG_SENSE7_NONE_Val      0x0ul  /**< \brief (EIC_CONFIG) No detection */
#define   EIC_CONFIG_SENSE7_RISE_Val      0x1ul  /**< \brief (EIC_CONFIG) Rising edge detection */
#define   EIC_CONFIG_SENSE7_FALL_Val      0x2ul  /**< \brief (EIC_CONFIG) Falling edge detection */
#define   EIC_CONFIG_SENSE7_BOTH_Val      0x3ul  /**< \brief (EIC_CONFIG) Both edges detection */
#define   EIC_CONFIG_SENSE7_HIGH_Val      0x4ul  /**< \brief (EIC_CONFIG) High level detection */
#define   EIC_CONFIG_SENSE7_LOW_Val       0x5ul  /**< \brief (EIC_CONFIG) Low level detection */
#define EIC_CONFIG_SENSE7_NONE      (EIC_CONFIG_SENSE7_NONE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_RISE      (EIC_CONFIG_SENSE7_RISE_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_FALL      (EIC_CONFIG_SENSE7_FALL_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_BOTH      (EIC_CONFIG_SENSE7_BOTH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_HIGH      (EIC_CONFIG_SENSE7_HIGH_Val    << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_SENSE7_LOW       (EIC_CONFIG_SENSE7_LOW_Val     << EIC_CONFIG_SENSE7_Pos)
#define EIC_CONFIG_FILTEN7_Pos      31           /**< \brief (EIC_CONFIG) Filter 7 Enable */
#define EIC_CONFIG_FILTEN7          (0x1ul << EIC_CONFIG_FILTEN7_Pos)
#define EIC_CONFIG_MASK             0xFFFFFFFFul /**< \brief (EIC_CONFIG) MASK Register */

/** \brief EIC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO EIC_CTRL_Type             CTRL;        /**< \brief Offset: 0x00 (R/W  8) Control */
  __I  EIC_STATUS_Type           STATUS;      /**< \brief Offset: 0x01 (R/   8) Status */
  __IO EIC_NMICTRL_Type          NMICTRL;     /**< \brief Offset: 0x02 (R/W  8) Non-Maskable Interrupt Control */
  __IO EIC_NMIFLAG_Type          NMIFLAG;     /**< \brief Offset: 0x03 (R/W  8) Non-Maskable Interrupt Flag Status and Clear */
  __IO EIC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x04 (R/W 32) Event Control */
  __IO EIC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x08 (R/W 32) Interrupt Enable Clear */
  __IO EIC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x0C (R/W 32) Interrupt Enable Set */
  __IO EIC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x10 (R/W 32) Interrupt Flag Status and Clear */
  __IO EIC_WAKEUP_Type           WAKEUP;      /**< \brief Offset: 0x14 (R/W 32) Wake-Up Enable */
  __IO EIC_CONFIG_Type           CONFIG[1];   /**< \brief Offset: 0x18 (R/W 32) Configuration n */
} Eic;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD10_EIC_COMPONENT_ */
