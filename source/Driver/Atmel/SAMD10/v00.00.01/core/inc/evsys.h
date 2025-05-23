/**
 * \file
 *
 * \brief Component description for EVSYS
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

#ifndef _SAMD10_EVSYS_COMPONENT_
#define _SAMD10_EVSYS_COMPONENT_

 /* ========== Register definition for EVSYS peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_EVSYS_CTRL             (0x42000400U) /**< \brief (EVSYS) Control */
#define REG_EVSYS_CHANNEL          (0x42000404U) /**< \brief (EVSYS) Channel */
#define REG_EVSYS_USER             (0x42000408U) /**< \brief (EVSYS) User Multiplexer */
#define REG_EVSYS_CHSTATUS         (0x4200040CU) /**< \brief (EVSYS) Channel Status */
#define REG_EVSYS_INTENCLR         (0x42000410U) /**< \brief (EVSYS) Interrupt Enable Clear */
#define REG_EVSYS_INTENSET         (0x42000414U) /**< \brief (EVSYS) Interrupt Enable Set */
#define REG_EVSYS_INTFLAG          (0x42000418U) /**< \brief (EVSYS) Interrupt Flag Status and Clear */
#else
#define REG_EVSYS_CTRL             (*(WoReg8 *)0x42000400U) /**< \brief (EVSYS) Control */
#define REG_EVSYS_CHANNEL          (*(RwReg  *)0x42000404U) /**< \brief (EVSYS) Channel */
#define REG_EVSYS_USER             (*(RwReg16*)0x42000408U) /**< \brief (EVSYS) User Multiplexer */
#define REG_EVSYS_CHSTATUS         (*(RoReg  *)0x4200040CU) /**< \brief (EVSYS) Channel Status */
#define REG_EVSYS_INTENCLR         (*(RwReg  *)0x42000410U) /**< \brief (EVSYS) Interrupt Enable Clear */
#define REG_EVSYS_INTENSET         (*(RwReg  *)0x42000414U) /**< \brief (EVSYS) Interrupt Enable Set */
#define REG_EVSYS_INTFLAG          (*(RwReg  *)0x42000418U) /**< \brief (EVSYS) Interrupt Flag Status and Clear */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for EVSYS peripheral ========== */
#define EVSYS_CHANNELS              6        // Number of Channels
#define EVSYS_CHANNELS_BITS         3        // Number of bits to select Channel
#define EVSYS_CHANNELS_MSB          5        // Number of Channels - 1
#define EVSYS_EXTEVT_NUM            0        // Number of External Event Generators
#define EVSYS_GCLK_ID_0             7
#define EVSYS_GCLK_ID_1             8
#define EVSYS_GCLK_ID_2             9
#define EVSYS_GCLK_ID_3             10
#define EVSYS_GCLK_ID_4             11
#define EVSYS_GCLK_ID_5             12
#define EVSYS_GCLK_ID_LSB           7
#define EVSYS_GCLK_ID_MSB           12
#define EVSYS_GCLK_ID_SIZE          6
#define EVSYS_GENERATORS            44       // Total Number of Event Generators
#define EVSYS_GENERATORS_BITS       6        // Number of bits to select Event Generator
#define EVSYS_USERS                 18       // Total Number of Event Users
#define EVSYS_USERS_BITS            5        // Number of bits to select Event User

// GENERATORS
#define EVSYS_ID_GEN_RTC_CMP_0      1
#define EVSYS_ID_GEN_RTC_CMP_1      2
#define EVSYS_ID_GEN_RTC_OVF        3
#define EVSYS_ID_GEN_RTC_PER_0      4
#define EVSYS_ID_GEN_RTC_PER_1      5
#define EVSYS_ID_GEN_RTC_PER_2      6
#define EVSYS_ID_GEN_RTC_PER_3      7
#define EVSYS_ID_GEN_RTC_PER_4      8
#define EVSYS_ID_GEN_RTC_PER_5      9
#define EVSYS_ID_GEN_RTC_PER_6      10
#define EVSYS_ID_GEN_RTC_PER_7      11
#define EVSYS_ID_GEN_EIC_EXTINT_0   12
#define EVSYS_ID_GEN_EIC_EXTINT_1   13
#define EVSYS_ID_GEN_EIC_EXTINT_2   14
#define EVSYS_ID_GEN_EIC_EXTINT_3   15
#define EVSYS_ID_GEN_EIC_EXTINT_4   16
#define EVSYS_ID_GEN_EIC_EXTINT_5   17
#define EVSYS_ID_GEN_EIC_EXTINT_6   18
#define EVSYS_ID_GEN_EIC_EXTINT_7   19
#define EVSYS_ID_GEN_DMAC_CH_0      20
#define EVSYS_ID_GEN_DMAC_CH_1      21
#define EVSYS_ID_GEN_DMAC_CH_2      22
#define EVSYS_ID_GEN_DMAC_CH_3      23
#define EVSYS_ID_GEN_TCC0_OVF       24
#define EVSYS_ID_GEN_TCC0_TRG       25
#define EVSYS_ID_GEN_TCC0_CNT       26
#define EVSYS_ID_GEN_TCC0_MCX_0     27
#define EVSYS_ID_GEN_TCC0_MCX_1     28
#define EVSYS_ID_GEN_TCC0_MCX_2     29
#define EVSYS_ID_GEN_TCC0_MCX_3     30
#define EVSYS_ID_GEN_TC1_OVF        31
#define EVSYS_ID_GEN_TC1_MCX_0      32
#define EVSYS_ID_GEN_TC1_MCX_1      33
#define EVSYS_ID_GEN_TC2_OVF        34
#define EVSYS_ID_GEN_TC2_MCX_0      35
#define EVSYS_ID_GEN_TC2_MCX_1      36
#define EVSYS_ID_GEN_ADC_RESRDY     37
#define EVSYS_ID_GEN_ADC_WINMON     38
#define EVSYS_ID_GEN_AC_COMP_0      39
#define EVSYS_ID_GEN_AC_COMP_1      40
#define EVSYS_ID_GEN_AC_WIN_0       41
#define EVSYS_ID_GEN_DAC_EMPTY      42
#define EVSYS_ID_GEN_PTC_EOC        43
#define EVSYS_ID_GEN_PTC_WCOMP      44

// USERS
#define EVSYS_ID_USER_DMAC_CH_0     0
#define EVSYS_ID_USER_DMAC_CH_1     1
#define EVSYS_ID_USER_DMAC_CH_2     2
#define EVSYS_ID_USER_DMAC_CH_3     3
#define EVSYS_ID_USER_TCC0_EV_0     4
#define EVSYS_ID_USER_TCC0_EV_1     5
#define EVSYS_ID_USER_TCC0_MC_0     6
#define EVSYS_ID_USER_TCC0_MC_1     7
#define EVSYS_ID_USER_TCC0_MC_2     8
#define EVSYS_ID_USER_TCC0_MC_3     9
#define EVSYS_ID_USER_TC1_EVU       10
#define EVSYS_ID_USER_TC2_EVU       11
#define EVSYS_ID_USER_ADC_START     12
#define EVSYS_ID_USER_ADC_SYNC      13
#define EVSYS_ID_USER_AC_SOC_0      14
#define EVSYS_ID_USER_AC_SOC_1      15
#define EVSYS_ID_USER_DAC_START     16
#define EVSYS_ID_USER_PTC_STCONV    17

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR EVSYS */
/* ========================================================================== */
/** \addtogroup SAMD10_EVSYS Event System Interface */
/*@{*/

#define EVSYS_U2208
#define REV_EVSYS                   0x101

/* -------- EVSYS_CTRL : (EVSYS Offset: 0x00) ( /W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :3;               /*!< bit:  1.. 3  Reserved                           */
    uint8_t  GCLKREQ:1;        /*!< bit:      4  Generic Clock Requests             */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} EVSYS_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CTRL_OFFSET           0x00         /**< \brief (EVSYS_CTRL offset) Control */
#define EVSYS_CTRL_RESETVALUE       0x00ul       /**< \brief (EVSYS_CTRL reset_value) Control */

#define EVSYS_CTRL_SWRST_Pos        0            /**< \brief (EVSYS_CTRL) Software Reset */
#define EVSYS_CTRL_SWRST            (0x1ul << EVSYS_CTRL_SWRST_Pos)
#define EVSYS_CTRL_GCLKREQ_Pos      4            /**< \brief (EVSYS_CTRL) Generic Clock Requests */
#define EVSYS_CTRL_GCLKREQ          (0x1ul << EVSYS_CTRL_GCLKREQ_Pos)
#define EVSYS_CTRL_MASK             0x11ul       /**< \brief (EVSYS_CTRL) MASK Register */

/* -------- EVSYS_CHANNEL : (EVSYS Offset: 0x04) (R/W 32) Channel -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CHANNEL:3;        /*!< bit:  0.. 2  Channel Selection                  */
    uint32_t :5;               /*!< bit:  3.. 7  Reserved                           */
    uint32_t SWEVT:1;          /*!< bit:      8  Software Event                     */
    uint32_t :7;               /*!< bit:  9..15  Reserved                           */
    uint32_t EVGEN:6;          /*!< bit: 16..21  Event Generator Selection          */
    uint32_t :2;               /*!< bit: 22..23  Reserved                           */
    uint32_t PATH:2;           /*!< bit: 24..25  Path Selection                     */
    uint32_t EDGSEL:2;         /*!< bit: 26..27  Edge Detection Selection           */
    uint32_t :4;               /*!< bit: 28..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_CHANNEL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHANNEL_OFFSET        0x04         /**< \brief (EVSYS_CHANNEL offset) Channel */
#define EVSYS_CHANNEL_RESETVALUE    0x00000000ul /**< \brief (EVSYS_CHANNEL reset_value) Channel */

#define EVSYS_CHANNEL_CHANNEL_Pos   0            /**< \brief (EVSYS_CHANNEL) Channel Selection */
#define EVSYS_CHANNEL_CHANNEL_Msk   (0x7ul << EVSYS_CHANNEL_CHANNEL_Pos)
#define EVSYS_CHANNEL_CHANNEL(value) (EVSYS_CHANNEL_CHANNEL_Msk & ((value) << EVSYS_CHANNEL_CHANNEL_Pos))
#define EVSYS_CHANNEL_SWEVT_Pos     8            /**< \brief (EVSYS_CHANNEL) Software Event */
#define EVSYS_CHANNEL_SWEVT         (0x1ul << EVSYS_CHANNEL_SWEVT_Pos)
#define EVSYS_CHANNEL_EVGEN_Pos     16           /**< \brief (EVSYS_CHANNEL) Event Generator Selection */
#define EVSYS_CHANNEL_EVGEN_Msk     (0x3Ful << EVSYS_CHANNEL_EVGEN_Pos)
#define EVSYS_CHANNEL_EVGEN(value)  (EVSYS_CHANNEL_EVGEN_Msk & ((value) << EVSYS_CHANNEL_EVGEN_Pos))
#define EVSYS_CHANNEL_PATH_Pos      24           /**< \brief (EVSYS_CHANNEL) Path Selection */
#define EVSYS_CHANNEL_PATH_Msk      (0x3ul << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH(value)   (EVSYS_CHANNEL_PATH_Msk & ((value) << EVSYS_CHANNEL_PATH_Pos))
#define   EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val 0x0ul  /**< \brief (EVSYS_CHANNEL) Synchronous path */
#define   EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val 0x1ul  /**< \brief (EVSYS_CHANNEL) Resynchronized path */
#define   EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val 0x2ul  /**< \brief (EVSYS_CHANNEL) Asynchronous path */
#define EVSYS_CHANNEL_PATH_SYNCHRONOUS (EVSYS_CHANNEL_PATH_SYNCHRONOUS_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH_RESYNCHRONIZED (EVSYS_CHANNEL_PATH_RESYNCHRONIZED_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_PATH_ASYNCHRONOUS (EVSYS_CHANNEL_PATH_ASYNCHRONOUS_Val << EVSYS_CHANNEL_PATH_Pos)
#define EVSYS_CHANNEL_EDGSEL_Pos    26           /**< \brief (EVSYS_CHANNEL) Edge Detection Selection */
#define EVSYS_CHANNEL_EDGSEL_Msk    (0x3ul << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL(value) (EVSYS_CHANNEL_EDGSEL_Msk & ((value) << EVSYS_CHANNEL_EDGSEL_Pos))
#define   EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val 0x0ul  /**< \brief (EVSYS_CHANNEL) No event output when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val 0x1ul  /**< \brief (EVSYS_CHANNEL) Event detection only on the rising edge of the signal from the event generator when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val 0x2ul  /**< \brief (EVSYS_CHANNEL) Event detection only on the falling edge of the signal from the event generator when using the resynchronized or synchronous path */
#define   EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val 0x3ul  /**< \brief (EVSYS_CHANNEL) Event detection on rising and falling edges of the signal from the event generator when using the resynchronized or synchronous path */
#define EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT (EVSYS_CHANNEL_EDGSEL_NO_EVT_OUTPUT_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_RISING_EDGE (EVSYS_CHANNEL_EDGSEL_RISING_EDGE_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_FALLING_EDGE (EVSYS_CHANNEL_EDGSEL_FALLING_EDGE_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_EDGSEL_BOTH_EDGES (EVSYS_CHANNEL_EDGSEL_BOTH_EDGES_Val << EVSYS_CHANNEL_EDGSEL_Pos)
#define EVSYS_CHANNEL_MASK          0x0F3F0107ul /**< \brief (EVSYS_CHANNEL) MASK Register */

/* -------- EVSYS_USER : (EVSYS Offset: 0x08) (R/W 16) User Multiplexer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t USER:5;           /*!< bit:  0.. 4  User Multiplexer Selection         */
    uint16_t :3;               /*!< bit:  5.. 7  Reserved                           */
    uint16_t CHANNEL:4;        /*!< bit:  8..11  Channel Event Selection            */
    uint16_t :4;               /*!< bit: 12..15  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} EVSYS_USER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_USER_OFFSET           0x08         /**< \brief (EVSYS_USER offset) User Multiplexer */
#define EVSYS_USER_RESETVALUE       0x0000ul     /**< \brief (EVSYS_USER reset_value) User Multiplexer */

#define EVSYS_USER_USER_Pos         0            /**< \brief (EVSYS_USER) User Multiplexer Selection */
#define EVSYS_USER_USER_Msk         (0x1Ful << EVSYS_USER_USER_Pos)
#define EVSYS_USER_USER(value)      (EVSYS_USER_USER_Msk & ((value) << EVSYS_USER_USER_Pos))
#define EVSYS_USER_CHANNEL_Pos      8            /**< \brief (EVSYS_USER) Channel Event Selection */
#define EVSYS_USER_CHANNEL_Msk      (0xFul << EVSYS_USER_CHANNEL_Pos)
#define EVSYS_USER_CHANNEL(value)   (EVSYS_USER_CHANNEL_Msk & ((value) << EVSYS_USER_CHANNEL_Pos))
#define   EVSYS_USER_CHANNEL_0_Val        0x0ul  /**< \brief (EVSYS_USER) No Channel Output Selected */
#define EVSYS_USER_CHANNEL_0        (EVSYS_USER_CHANNEL_0_Val      << EVSYS_USER_CHANNEL_Pos)
#define EVSYS_USER_MASK             0x0F1Ful     /**< \brief (EVSYS_USER) MASK Register */

/* -------- EVSYS_CHSTATUS : (EVSYS Offset: 0x0C) (R/  32) Channel Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t USRRDY0:1;        /*!< bit:      0  Channel 0 User Ready               */
    uint32_t USRRDY1:1;        /*!< bit:      1  Channel 1 User Ready               */
    uint32_t USRRDY2:1;        /*!< bit:      2  Channel 2 User Ready               */
    uint32_t USRRDY3:1;        /*!< bit:      3  Channel 3 User Ready               */
    uint32_t USRRDY4:1;        /*!< bit:      4  Channel 4 User Ready               */
    uint32_t USRRDY5:1;        /*!< bit:      5  Channel 5 User Ready               */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t CHBUSY0:1;        /*!< bit:      8  Channel 0 Busy                     */
    uint32_t CHBUSY1:1;        /*!< bit:      9  Channel 1 Busy                     */
    uint32_t CHBUSY2:1;        /*!< bit:     10  Channel 2 Busy                     */
    uint32_t CHBUSY3:1;        /*!< bit:     11  Channel 3 Busy                     */
    uint32_t CHBUSY4:1;        /*!< bit:     12  Channel 4 Busy                     */
    uint32_t CHBUSY5:1;        /*!< bit:     13  Channel 5 Busy                     */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t USRRDY:6;         /*!< bit:  0.. 5  Channel x User Ready               */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t CHBUSY:6;         /*!< bit:  8..13  Channel x Busy                     */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_CHSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_CHSTATUS_OFFSET       0x0C         /**< \brief (EVSYS_CHSTATUS offset) Channel Status */
#define EVSYS_CHSTATUS_RESETVALUE   0x0000003Ful /**< \brief (EVSYS_CHSTATUS reset_value) Channel Status */

#define EVSYS_CHSTATUS_USRRDY0_Pos  0            /**< \brief (EVSYS_CHSTATUS) Channel 0 User Ready */
#define EVSYS_CHSTATUS_USRRDY0      (1 << EVSYS_CHSTATUS_USRRDY0_Pos)
#define EVSYS_CHSTATUS_USRRDY1_Pos  1            /**< \brief (EVSYS_CHSTATUS) Channel 1 User Ready */
#define EVSYS_CHSTATUS_USRRDY1      (1 << EVSYS_CHSTATUS_USRRDY1_Pos)
#define EVSYS_CHSTATUS_USRRDY2_Pos  2            /**< \brief (EVSYS_CHSTATUS) Channel 2 User Ready */
#define EVSYS_CHSTATUS_USRRDY2      (1 << EVSYS_CHSTATUS_USRRDY2_Pos)
#define EVSYS_CHSTATUS_USRRDY3_Pos  3            /**< \brief (EVSYS_CHSTATUS) Channel 3 User Ready */
#define EVSYS_CHSTATUS_USRRDY3      (1 << EVSYS_CHSTATUS_USRRDY3_Pos)
#define EVSYS_CHSTATUS_USRRDY4_Pos  4            /**< \brief (EVSYS_CHSTATUS) Channel 4 User Ready */
#define EVSYS_CHSTATUS_USRRDY4      (1 << EVSYS_CHSTATUS_USRRDY4_Pos)
#define EVSYS_CHSTATUS_USRRDY5_Pos  5            /**< \brief (EVSYS_CHSTATUS) Channel 5 User Ready */
#define EVSYS_CHSTATUS_USRRDY5      (1 << EVSYS_CHSTATUS_USRRDY5_Pos)
#define EVSYS_CHSTATUS_USRRDY_Pos   0            /**< \brief (EVSYS_CHSTATUS) Channel x User Ready */
#define EVSYS_CHSTATUS_USRRDY_Msk   (0x3Ful << EVSYS_CHSTATUS_USRRDY_Pos)
#define EVSYS_CHSTATUS_USRRDY(value) (EVSYS_CHSTATUS_USRRDY_Msk & ((value) << EVSYS_CHSTATUS_USRRDY_Pos))
#define EVSYS_CHSTATUS_CHBUSY0_Pos  8            /**< \brief (EVSYS_CHSTATUS) Channel 0 Busy */
#define EVSYS_CHSTATUS_CHBUSY0      (1 << EVSYS_CHSTATUS_CHBUSY0_Pos)
#define EVSYS_CHSTATUS_CHBUSY1_Pos  9            /**< \brief (EVSYS_CHSTATUS) Channel 1 Busy */
#define EVSYS_CHSTATUS_CHBUSY1      (1 << EVSYS_CHSTATUS_CHBUSY1_Pos)
#define EVSYS_CHSTATUS_CHBUSY2_Pos  10           /**< \brief (EVSYS_CHSTATUS) Channel 2 Busy */
#define EVSYS_CHSTATUS_CHBUSY2      (1 << EVSYS_CHSTATUS_CHBUSY2_Pos)
#define EVSYS_CHSTATUS_CHBUSY3_Pos  11           /**< \brief (EVSYS_CHSTATUS) Channel 3 Busy */
#define EVSYS_CHSTATUS_CHBUSY3      (1 << EVSYS_CHSTATUS_CHBUSY3_Pos)
#define EVSYS_CHSTATUS_CHBUSY4_Pos  12           /**< \brief (EVSYS_CHSTATUS) Channel 4 Busy */
#define EVSYS_CHSTATUS_CHBUSY4      (1 << EVSYS_CHSTATUS_CHBUSY4_Pos)
#define EVSYS_CHSTATUS_CHBUSY5_Pos  13           /**< \brief (EVSYS_CHSTATUS) Channel 5 Busy */
#define EVSYS_CHSTATUS_CHBUSY5      (1 << EVSYS_CHSTATUS_CHBUSY5_Pos)
#define EVSYS_CHSTATUS_CHBUSY_Pos   8            /**< \brief (EVSYS_CHSTATUS) Channel x Busy */
#define EVSYS_CHSTATUS_CHBUSY_Msk   (0x3Ful << EVSYS_CHSTATUS_CHBUSY_Pos)
#define EVSYS_CHSTATUS_CHBUSY(value) (EVSYS_CHSTATUS_CHBUSY_Msk & ((value) << EVSYS_CHSTATUS_CHBUSY_Pos))
#define EVSYS_CHSTATUS_MASK         0x00003F3Ful /**< \brief (EVSYS_CHSTATUS) MASK Register */

/* -------- EVSYS_INTENCLR : (EVSYS Offset: 0x10) (R/W 32) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OVR0:1;           /*!< bit:      0  Channel 0 Overrun Interrupt Enable */
    uint32_t OVR1:1;           /*!< bit:      1  Channel 1 Overrun Interrupt Enable */
    uint32_t OVR2:1;           /*!< bit:      2  Channel 2 Overrun Interrupt Enable */
    uint32_t OVR3:1;           /*!< bit:      3  Channel 3 Overrun Interrupt Enable */
    uint32_t OVR4:1;           /*!< bit:      4  Channel 4 Overrun Interrupt Enable */
    uint32_t OVR5:1;           /*!< bit:      5  Channel 5 Overrun Interrupt Enable */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t EVD0:1;           /*!< bit:      8  Channel 0 Event Detection Interrupt Enable */
    uint32_t EVD1:1;           /*!< bit:      9  Channel 1 Event Detection Interrupt Enable */
    uint32_t EVD2:1;           /*!< bit:     10  Channel 2 Event Detection Interrupt Enable */
    uint32_t EVD3:1;           /*!< bit:     11  Channel 3 Event Detection Interrupt Enable */
    uint32_t EVD4:1;           /*!< bit:     12  Channel 4 Event Detection Interrupt Enable */
    uint32_t EVD5:1;           /*!< bit:     13  Channel 5 Event Detection Interrupt Enable */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t OVR:6;            /*!< bit:  0.. 5  Channel x Overrun Interrupt Enable */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t EVD:6;            /*!< bit:  8..13  Channel x Event Detection Interrupt Enable */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTENCLR_OFFSET       0x10         /**< \brief (EVSYS_INTENCLR offset) Interrupt Enable Clear */
#define EVSYS_INTENCLR_RESETVALUE   0x00000000ul /**< \brief (EVSYS_INTENCLR reset_value) Interrupt Enable Clear */

#define EVSYS_INTENCLR_OVR0_Pos     0            /**< \brief (EVSYS_INTENCLR) Channel 0 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR0         (1 << EVSYS_INTENCLR_OVR0_Pos)
#define EVSYS_INTENCLR_OVR1_Pos     1            /**< \brief (EVSYS_INTENCLR) Channel 1 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR1         (1 << EVSYS_INTENCLR_OVR1_Pos)
#define EVSYS_INTENCLR_OVR2_Pos     2            /**< \brief (EVSYS_INTENCLR) Channel 2 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR2         (1 << EVSYS_INTENCLR_OVR2_Pos)
#define EVSYS_INTENCLR_OVR3_Pos     3            /**< \brief (EVSYS_INTENCLR) Channel 3 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR3         (1 << EVSYS_INTENCLR_OVR3_Pos)
#define EVSYS_INTENCLR_OVR4_Pos     4            /**< \brief (EVSYS_INTENCLR) Channel 4 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR4         (1 << EVSYS_INTENCLR_OVR4_Pos)
#define EVSYS_INTENCLR_OVR5_Pos     5            /**< \brief (EVSYS_INTENCLR) Channel 5 Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR5         (1 << EVSYS_INTENCLR_OVR5_Pos)
#define EVSYS_INTENCLR_OVR_Pos      0            /**< \brief (EVSYS_INTENCLR) Channel x Overrun Interrupt Enable */
#define EVSYS_INTENCLR_OVR_Msk      (0x3Ful << EVSYS_INTENCLR_OVR_Pos)
#define EVSYS_INTENCLR_OVR(value)   (EVSYS_INTENCLR_OVR_Msk & ((value) << EVSYS_INTENCLR_OVR_Pos))
#define EVSYS_INTENCLR_EVD0_Pos     8            /**< \brief (EVSYS_INTENCLR) Channel 0 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD0         (1 << EVSYS_INTENCLR_EVD0_Pos)
#define EVSYS_INTENCLR_EVD1_Pos     9            /**< \brief (EVSYS_INTENCLR) Channel 1 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD1         (1 << EVSYS_INTENCLR_EVD1_Pos)
#define EVSYS_INTENCLR_EVD2_Pos     10           /**< \brief (EVSYS_INTENCLR) Channel 2 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD2         (1 << EVSYS_INTENCLR_EVD2_Pos)
#define EVSYS_INTENCLR_EVD3_Pos     11           /**< \brief (EVSYS_INTENCLR) Channel 3 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD3         (1 << EVSYS_INTENCLR_EVD3_Pos)
#define EVSYS_INTENCLR_EVD4_Pos     12           /**< \brief (EVSYS_INTENCLR) Channel 4 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD4         (1 << EVSYS_INTENCLR_EVD4_Pos)
#define EVSYS_INTENCLR_EVD5_Pos     13           /**< \brief (EVSYS_INTENCLR) Channel 5 Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD5         (1 << EVSYS_INTENCLR_EVD5_Pos)
#define EVSYS_INTENCLR_EVD_Pos      8            /**< \brief (EVSYS_INTENCLR) Channel x Event Detection Interrupt Enable */
#define EVSYS_INTENCLR_EVD_Msk      (0x3Ful << EVSYS_INTENCLR_EVD_Pos)
#define EVSYS_INTENCLR_EVD(value)   (EVSYS_INTENCLR_EVD_Msk & ((value) << EVSYS_INTENCLR_EVD_Pos))
#define EVSYS_INTENCLR_MASK         0x00003F3Ful /**< \brief (EVSYS_INTENCLR) MASK Register */

/* -------- EVSYS_INTENSET : (EVSYS Offset: 0x14) (R/W 32) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t OVR0:1;           /*!< bit:      0  Channel 0 Overrun Interrupt Enable */
    uint32_t OVR1:1;           /*!< bit:      1  Channel 1 Overrun Interrupt Enable */
    uint32_t OVR2:1;           /*!< bit:      2  Channel 2 Overrun Interrupt Enable */
    uint32_t OVR3:1;           /*!< bit:      3  Channel 3 Overrun Interrupt Enable */
    uint32_t OVR4:1;           /*!< bit:      4  Channel 4 Overrun Interrupt Enable */
    uint32_t OVR5:1;           /*!< bit:      5  Channel 5 Overrun Interrupt Enable */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t EVD0:1;           /*!< bit:      8  Channel 0 Event Detection Interrupt Enable */
    uint32_t EVD1:1;           /*!< bit:      9  Channel 1 Event Detection Interrupt Enable */
    uint32_t EVD2:1;           /*!< bit:     10  Channel 2 Event Detection Interrupt Enable */
    uint32_t EVD3:1;           /*!< bit:     11  Channel 3 Event Detection Interrupt Enable */
    uint32_t EVD4:1;           /*!< bit:     12  Channel 4 Event Detection Interrupt Enable */
    uint32_t EVD5:1;           /*!< bit:     13  Channel 5 Event Detection Interrupt Enable */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint32_t OVR:6;            /*!< bit:  0.. 5  Channel x Overrun Interrupt Enable */
    uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    uint32_t EVD:6;            /*!< bit:  8..13  Channel x Event Detection Interrupt Enable */
    uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTENSET_OFFSET       0x14         /**< \brief (EVSYS_INTENSET offset) Interrupt Enable Set */
#define EVSYS_INTENSET_RESETVALUE   0x00000000ul /**< \brief (EVSYS_INTENSET reset_value) Interrupt Enable Set */

#define EVSYS_INTENSET_OVR0_Pos     0            /**< \brief (EVSYS_INTENSET) Channel 0 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR0         (1 << EVSYS_INTENSET_OVR0_Pos)
#define EVSYS_INTENSET_OVR1_Pos     1            /**< \brief (EVSYS_INTENSET) Channel 1 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR1         (1 << EVSYS_INTENSET_OVR1_Pos)
#define EVSYS_INTENSET_OVR2_Pos     2            /**< \brief (EVSYS_INTENSET) Channel 2 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR2         (1 << EVSYS_INTENSET_OVR2_Pos)
#define EVSYS_INTENSET_OVR3_Pos     3            /**< \brief (EVSYS_INTENSET) Channel 3 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR3         (1 << EVSYS_INTENSET_OVR3_Pos)
#define EVSYS_INTENSET_OVR4_Pos     4            /**< \brief (EVSYS_INTENSET) Channel 4 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR4         (1 << EVSYS_INTENSET_OVR4_Pos)
#define EVSYS_INTENSET_OVR5_Pos     5            /**< \brief (EVSYS_INTENSET) Channel 5 Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR5         (1 << EVSYS_INTENSET_OVR5_Pos)
#define EVSYS_INTENSET_OVR_Pos      0            /**< \brief (EVSYS_INTENSET) Channel x Overrun Interrupt Enable */
#define EVSYS_INTENSET_OVR_Msk      (0x3Ful << EVSYS_INTENSET_OVR_Pos)
#define EVSYS_INTENSET_OVR(value)   (EVSYS_INTENSET_OVR_Msk & ((value) << EVSYS_INTENSET_OVR_Pos))
#define EVSYS_INTENSET_EVD0_Pos     8            /**< \brief (EVSYS_INTENSET) Channel 0 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD0         (1 << EVSYS_INTENSET_EVD0_Pos)
#define EVSYS_INTENSET_EVD1_Pos     9            /**< \brief (EVSYS_INTENSET) Channel 1 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD1         (1 << EVSYS_INTENSET_EVD1_Pos)
#define EVSYS_INTENSET_EVD2_Pos     10           /**< \brief (EVSYS_INTENSET) Channel 2 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD2         (1 << EVSYS_INTENSET_EVD2_Pos)
#define EVSYS_INTENSET_EVD3_Pos     11           /**< \brief (EVSYS_INTENSET) Channel 3 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD3         (1 << EVSYS_INTENSET_EVD3_Pos)
#define EVSYS_INTENSET_EVD4_Pos     12           /**< \brief (EVSYS_INTENSET) Channel 4 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD4         (1 << EVSYS_INTENSET_EVD4_Pos)
#define EVSYS_INTENSET_EVD5_Pos     13           /**< \brief (EVSYS_INTENSET) Channel 5 Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD5         (1 << EVSYS_INTENSET_EVD5_Pos)
#define EVSYS_INTENSET_EVD_Pos      8            /**< \brief (EVSYS_INTENSET) Channel x Event Detection Interrupt Enable */
#define EVSYS_INTENSET_EVD_Msk      (0x3Ful << EVSYS_INTENSET_EVD_Pos)
#define EVSYS_INTENSET_EVD(value)   (EVSYS_INTENSET_EVD_Msk & ((value) << EVSYS_INTENSET_EVD_Pos))
#define EVSYS_INTENSET_MASK         0x00003F3Ful /**< \brief (EVSYS_INTENSET) MASK Register */

/* -------- EVSYS_INTFLAG : (EVSYS Offset: 0x18) (R/W 32) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint32_t OVR0:1;           /*!< bit:      0  Channel 0 Overrun                  */
    __I uint32_t OVR1:1;           /*!< bit:      1  Channel 1 Overrun                  */
    __I uint32_t OVR2:1;           /*!< bit:      2  Channel 2 Overrun                  */
    __I uint32_t OVR3:1;           /*!< bit:      3  Channel 3 Overrun                  */
    __I uint32_t OVR4:1;           /*!< bit:      4  Channel 4 Overrun                  */
    __I uint32_t OVR5:1;           /*!< bit:      5  Channel 5 Overrun                  */
    __I uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    __I uint32_t EVD0:1;           /*!< bit:      8  Channel 0 Event Detection          */
    __I uint32_t EVD1:1;           /*!< bit:      9  Channel 1 Event Detection          */
    __I uint32_t EVD2:1;           /*!< bit:     10  Channel 2 Event Detection          */
    __I uint32_t EVD3:1;           /*!< bit:     11  Channel 3 Event Detection          */
    __I uint32_t EVD4:1;           /*!< bit:     12  Channel 4 Event Detection          */
    __I uint32_t EVD5:1;           /*!< bit:     13  Channel 5 Event Detection          */
    __I uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    __I uint32_t OVR:6;            /*!< bit:  0.. 5  Channel x Overrun                  */
    __I uint32_t :2;               /*!< bit:  6.. 7  Reserved                           */
    __I uint32_t EVD:6;            /*!< bit:  8..13  Channel x Event Detection          */
    __I uint32_t :18;              /*!< bit: 14..31  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} EVSYS_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define EVSYS_INTFLAG_OFFSET        0x18         /**< \brief (EVSYS_INTFLAG offset) Interrupt Flag Status and Clear */
#define EVSYS_INTFLAG_RESETVALUE    0x00000000ul /**< \brief (EVSYS_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define EVSYS_INTFLAG_OVR0_Pos      0            /**< \brief (EVSYS_INTFLAG) Channel 0 Overrun */
#define EVSYS_INTFLAG_OVR0          (1 << EVSYS_INTFLAG_OVR0_Pos)
#define EVSYS_INTFLAG_OVR1_Pos      1            /**< \brief (EVSYS_INTFLAG) Channel 1 Overrun */
#define EVSYS_INTFLAG_OVR1          (1 << EVSYS_INTFLAG_OVR1_Pos)
#define EVSYS_INTFLAG_OVR2_Pos      2            /**< \brief (EVSYS_INTFLAG) Channel 2 Overrun */
#define EVSYS_INTFLAG_OVR2          (1 << EVSYS_INTFLAG_OVR2_Pos)
#define EVSYS_INTFLAG_OVR3_Pos      3            /**< \brief (EVSYS_INTFLAG) Channel 3 Overrun */
#define EVSYS_INTFLAG_OVR3          (1 << EVSYS_INTFLAG_OVR3_Pos)
#define EVSYS_INTFLAG_OVR4_Pos      4            /**< \brief (EVSYS_INTFLAG) Channel 4 Overrun */
#define EVSYS_INTFLAG_OVR4          (1 << EVSYS_INTFLAG_OVR4_Pos)
#define EVSYS_INTFLAG_OVR5_Pos      5            /**< \brief (EVSYS_INTFLAG) Channel 5 Overrun */
#define EVSYS_INTFLAG_OVR5          (1 << EVSYS_INTFLAG_OVR5_Pos)
#define EVSYS_INTFLAG_OVR_Pos       0            /**< \brief (EVSYS_INTFLAG) Channel x Overrun */
#define EVSYS_INTFLAG_OVR_Msk       (0x3Ful << EVSYS_INTFLAG_OVR_Pos)
#define EVSYS_INTFLAG_OVR(value)    (EVSYS_INTFLAG_OVR_Msk & ((value) << EVSYS_INTFLAG_OVR_Pos))
#define EVSYS_INTFLAG_EVD0_Pos      8            /**< \brief (EVSYS_INTFLAG) Channel 0 Event Detection */
#define EVSYS_INTFLAG_EVD0          (1 << EVSYS_INTFLAG_EVD0_Pos)
#define EVSYS_INTFLAG_EVD1_Pos      9            /**< \brief (EVSYS_INTFLAG) Channel 1 Event Detection */
#define EVSYS_INTFLAG_EVD1          (1 << EVSYS_INTFLAG_EVD1_Pos)
#define EVSYS_INTFLAG_EVD2_Pos      10           /**< \brief (EVSYS_INTFLAG) Channel 2 Event Detection */
#define EVSYS_INTFLAG_EVD2          (1 << EVSYS_INTFLAG_EVD2_Pos)
#define EVSYS_INTFLAG_EVD3_Pos      11           /**< \brief (EVSYS_INTFLAG) Channel 3 Event Detection */
#define EVSYS_INTFLAG_EVD3          (1 << EVSYS_INTFLAG_EVD3_Pos)
#define EVSYS_INTFLAG_EVD4_Pos      12           /**< \brief (EVSYS_INTFLAG) Channel 4 Event Detection */
#define EVSYS_INTFLAG_EVD4          (1 << EVSYS_INTFLAG_EVD4_Pos)
#define EVSYS_INTFLAG_EVD5_Pos      13           /**< \brief (EVSYS_INTFLAG) Channel 5 Event Detection */
#define EVSYS_INTFLAG_EVD5          (1 << EVSYS_INTFLAG_EVD5_Pos)
#define EVSYS_INTFLAG_EVD_Pos       8            /**< \brief (EVSYS_INTFLAG) Channel x Event Detection */
#define EVSYS_INTFLAG_EVD_Msk       (0x3Ful << EVSYS_INTFLAG_EVD_Pos)
#define EVSYS_INTFLAG_EVD(value)    (EVSYS_INTFLAG_EVD_Msk & ((value) << EVSYS_INTFLAG_EVD_Pos))
#define EVSYS_INTFLAG_MASK          0x00003F3Ful /**< \brief (EVSYS_INTFLAG) MASK Register */

/** \brief EVSYS hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __O  EVSYS_CTRL_Type           CTRL;        /**< \brief Offset: 0x00 ( /W  8) Control */
       RoReg8                    Reserved1[0x3];
  __IO EVSYS_CHANNEL_Type        CHANNEL;     /**< \brief Offset: 0x04 (R/W 32) Channel */
  __IO EVSYS_USER_Type           USER;        /**< \brief Offset: 0x08 (R/W 16) User Multiplexer */
       RoReg8                    Reserved2[0x2];
  __I  EVSYS_CHSTATUS_Type       CHSTATUS;    /**< \brief Offset: 0x0C (R/  32) Channel Status */
  __IO EVSYS_INTENCLR_Type       INTENCLR;    /**< \brief Offset: 0x10 (R/W 32) Interrupt Enable Clear */
  __IO EVSYS_INTENSET_Type       INTENSET;    /**< \brief Offset: 0x14 (R/W 32) Interrupt Enable Set */
  __IO EVSYS_INTFLAG_Type        INTFLAG;     /**< \brief Offset: 0x18 (R/W 32) Interrupt Flag Status and Clear */
} Evsys;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD10_EVSYS_COMPONENT_ */
