/**
 * \file
 *
 * \brief Component description for DSU
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

#ifndef _SAMD10_DSU_COMPONENT_
#define _SAMD10_DSU_COMPONENT_


 /* ========== Register definition for DSU peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DSU_CTRL               (0x41002000U) /**< \brief (DSU) Control */
#define REG_DSU_STATUSA            (0x41002001U) /**< \brief (DSU) Status A */
#define REG_DSU_STATUSB            (0x41002002U) /**< \brief (DSU) Status B */
#define REG_DSU_STATUSC            (0x41002003U) /**< \brief (DSU) Status C */
#define REG_DSU_ADDR               (0x41002004U) /**< \brief (DSU) Address */
#define REG_DSU_LENGTH             (0x41002008U) /**< \brief (DSU) Length */
#define REG_DSU_DATA               (0x4100200CU) /**< \brief (DSU) Data */
#define REG_DSU_DCC0               (0x41002010U) /**< \brief (DSU) Debug Communication Channel 0 */
#define REG_DSU_DCC1               (0x41002014U) /**< \brief (DSU) Debug Communication Channel 1 */
#define REG_DSU_DID                (0x41002018U) /**< \brief (DSU) Device Identification */
#define REG_DSU_DCFG0              (0x410020F0U) /**< \brief (DSU) Device Configuration 0 */
#define REG_DSU_DCFG1              (0x410020F4U) /**< \brief (DSU) Device Configuration 1 */
#define REG_DSU_ENTRY0             (0x41003000U) /**< \brief (DSU) Coresight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (0x41003004U) /**< \brief (DSU) Coresight ROM Table Entry 1 */
#define REG_DSU_END                (0x41003008U) /**< \brief (DSU) Coresight ROM Table End */
#define REG_DSU_MEMTYPE            (0x41003FCCU) /**< \brief (DSU) Coresight ROM Table Memory Type */
#define REG_DSU_PID4               (0x41003FD0U) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID5               (0x41003FD4U) /**< \brief (DSU) Peripheral Identification 5 */
#define REG_DSU_PID6               (0x41003FD8U) /**< \brief (DSU) Peripheral Identification 6 */
#define REG_DSU_PID7               (0x41003FDCU) /**< \brief (DSU) Peripheral Identification 7 */
#define REG_DSU_PID0               (0x41003FE0U) /**< \brief (DSU) Peripheral Identification 0 */
#define REG_DSU_PID1               (0x41003FE4U) /**< \brief (DSU) Peripheral Identification 1 */
#define REG_DSU_PID2               (0x41003FE8U) /**< \brief (DSU) Peripheral Identification 2 */
#define REG_DSU_PID3               (0x41003FECU) /**< \brief (DSU) Peripheral Identification 3 */
#define REG_DSU_CID0               (0x41003FF0U) /**< \brief (DSU) Component Identification 0 */
#define REG_DSU_CID1               (0x41003FF4U) /**< \brief (DSU) Component Identification 1 */
#define REG_DSU_CID2               (0x41003FF8U) /**< \brief (DSU) Component Identification 2 */
#define REG_DSU_CID3               (0x41003FFCU) /**< \brief (DSU) Component Identification 3 */
#else
#define REG_DSU_CTRL               (*(WoReg8 *)0x41002000U) /**< \brief (DSU) Control */
#define REG_DSU_STATUSA            (*(RwReg8 *)0x41002001U) /**< \brief (DSU) Status A */
#define REG_DSU_STATUSB            (*(RoReg8 *)0x41002002U) /**< \brief (DSU) Status B */
#define REG_DSU_STATUSC            (*(RoReg8 *)0x41002003U) /**< \brief (DSU) Status C */
#define REG_DSU_ADDR               (*(RwReg  *)0x41002004U) /**< \brief (DSU) Address */
#define REG_DSU_LENGTH             (*(RwReg  *)0x41002008U) /**< \brief (DSU) Length */
#define REG_DSU_DATA               (*(RwReg  *)0x4100200CU) /**< \brief (DSU) Data */
#define REG_DSU_DCC0               (*(RwReg  *)0x41002010U) /**< \brief (DSU) Debug Communication Channel 0 */
#define REG_DSU_DCC1               (*(RwReg  *)0x41002014U) /**< \brief (DSU) Debug Communication Channel 1 */
#define REG_DSU_DID                (*(RoReg  *)0x41002018U) /**< \brief (DSU) Device Identification */
#define REG_DSU_DCFG0              (*(RwReg  *)0x410020F0U) /**< \brief (DSU) Device Configuration 0 */
#define REG_DSU_DCFG1              (*(RwReg  *)0x410020F4U) /**< \brief (DSU) Device Configuration 1 */
#define REG_DSU_ENTRY0             (*(RoReg  *)0x41003000U) /**< \brief (DSU) Coresight ROM Table Entry 0 */
#define REG_DSU_ENTRY1             (*(RoReg  *)0x41003004U) /**< \brief (DSU) Coresight ROM Table Entry 1 */
#define REG_DSU_END                (*(RoReg  *)0x41003008U) /**< \brief (DSU) Coresight ROM Table End */
#define REG_DSU_MEMTYPE            (*(RoReg  *)0x41003FCCU) /**< \brief (DSU) Coresight ROM Table Memory Type */
#define REG_DSU_PID4               (*(RoReg  *)0x41003FD0U) /**< \brief (DSU) Peripheral Identification 4 */
#define REG_DSU_PID5               (*(RoReg  *)0x41003FD4U) /**< \brief (DSU) Peripheral Identification 5 */
#define REG_DSU_PID6               (*(RoReg  *)0x41003FD8U) /**< \brief (DSU) Peripheral Identification 6 */
#define REG_DSU_PID7               (*(RoReg  *)0x41003FDCU) /**< \brief (DSU) Peripheral Identification 7 */
#define REG_DSU_PID0               (*(RoReg  *)0x41003FE0U) /**< \brief (DSU) Peripheral Identification 0 */
#define REG_DSU_PID1               (*(RoReg  *)0x41003FE4U) /**< \brief (DSU) Peripheral Identification 1 */
#define REG_DSU_PID2               (*(RoReg  *)0x41003FE8U) /**< \brief (DSU) Peripheral Identification 2 */
#define REG_DSU_PID3               (*(RoReg  *)0x41003FECU) /**< \brief (DSU) Peripheral Identification 3 */
#define REG_DSU_CID0               (*(RoReg  *)0x41003FF0U) /**< \brief (DSU) Component Identification 0 */
#define REG_DSU_CID1               (*(RoReg  *)0x41003FF4U) /**< \brief (DSU) Component Identification 1 */
#define REG_DSU_CID2               (*(RoReg  *)0x41003FF8U) /**< \brief (DSU) Component Identification 2 */
#define REG_DSU_CID3               (*(RoReg  *)0x41003FFCU) /**< \brief (DSU) Component Identification 3 */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DSU peripheral ========== */
#define DSU_CLK_AHB_DOMAIN                   // Clock domain of AHB clock
#define DSU_CLK_AHB_ID              3        // Index of AHB clock in PM.AHBMASK register

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR DSU */
/* ========================================================================== */
/** \addtogroup SAMD10_DSU Device Service Unit */
/*@{*/

#define DSU_U2209
#define REV_DSU                     0x211

/* -------- DSU_CTRL : (DSU Offset: 0x0000) ( /W  8) Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  :1;               /*!< bit:      1  Reserved                           */
    uint8_t  CRC:1;            /*!< bit:      2  32-bit Cyclic Redundancy Code      */
    uint8_t  MBIST:1;          /*!< bit:      3  Memory built-in self-test          */
    uint8_t  CE:1;             /*!< bit:      4  Chip-Erase                         */
    uint8_t  :1;               /*!< bit:      5  Reserved                           */
    uint8_t  ARR:1;            /*!< bit:      6  Auxiliary Row Read                 */
    uint8_t  SMSA:1;           /*!< bit:      7  Start Memory Stream Access         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DSU_CTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CTRL_OFFSET             0x0000       /**< \brief (DSU_CTRL offset) Control */
#define DSU_CTRL_RESETVALUE         0x00ul       /**< \brief (DSU_CTRL reset_value) Control */

#define DSU_CTRL_SWRST_Pos          0            /**< \brief (DSU_CTRL) Software Reset */
#define DSU_CTRL_SWRST              (0x1ul << DSU_CTRL_SWRST_Pos)
#define DSU_CTRL_CRC_Pos            2            /**< \brief (DSU_CTRL) 32-bit Cyclic Redundancy Code */
#define DSU_CTRL_CRC                (0x1ul << DSU_CTRL_CRC_Pos)
#define DSU_CTRL_MBIST_Pos          3            /**< \brief (DSU_CTRL) Memory built-in self-test */
#define DSU_CTRL_MBIST              (0x1ul << DSU_CTRL_MBIST_Pos)
#define DSU_CTRL_CE_Pos             4            /**< \brief (DSU_CTRL) Chip-Erase */
#define DSU_CTRL_CE                 (0x1ul << DSU_CTRL_CE_Pos)
#define DSU_CTRL_ARR_Pos            6            /**< \brief (DSU_CTRL) Auxiliary Row Read */
#define DSU_CTRL_ARR                (0x1ul << DSU_CTRL_ARR_Pos)
#define DSU_CTRL_SMSA_Pos           7            /**< \brief (DSU_CTRL) Start Memory Stream Access */
#define DSU_CTRL_SMSA               (0x1ul << DSU_CTRL_SMSA_Pos)
#define DSU_CTRL_MASK               0xDDul       /**< \brief (DSU_CTRL) MASK Register */

/* -------- DSU_STATUSA : (DSU Offset: 0x0001) (R/W  8) Status A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  DONE:1;           /*!< bit:      0  Done                               */
    uint8_t  CRSTEXT:1;        /*!< bit:      1  CPU Reset Phase Extension          */
    uint8_t  BERR:1;           /*!< bit:      2  Bus Error                          */
    uint8_t  FAIL:1;           /*!< bit:      3  Failure                            */
    uint8_t  PERR:1;           /*!< bit:      4  Protection Error                   */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DSU_STATUSA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_STATUSA_OFFSET          0x0001       /**< \brief (DSU_STATUSA offset) Status A */
#define DSU_STATUSA_RESETVALUE      0x00ul       /**< \brief (DSU_STATUSA reset_value) Status A */

#define DSU_STATUSA_DONE_Pos        0            /**< \brief (DSU_STATUSA) Done */
#define DSU_STATUSA_DONE            (0x1ul << DSU_STATUSA_DONE_Pos)
#define DSU_STATUSA_CRSTEXT_Pos     1            /**< \brief (DSU_STATUSA) CPU Reset Phase Extension */
#define DSU_STATUSA_CRSTEXT         (0x1ul << DSU_STATUSA_CRSTEXT_Pos)
#define DSU_STATUSA_BERR_Pos        2            /**< \brief (DSU_STATUSA) Bus Error */
#define DSU_STATUSA_BERR            (0x1ul << DSU_STATUSA_BERR_Pos)
#define DSU_STATUSA_FAIL_Pos        3            /**< \brief (DSU_STATUSA) Failure */
#define DSU_STATUSA_FAIL            (0x1ul << DSU_STATUSA_FAIL_Pos)
#define DSU_STATUSA_PERR_Pos        4            /**< \brief (DSU_STATUSA) Protection Error */
#define DSU_STATUSA_PERR            (0x1ul << DSU_STATUSA_PERR_Pos)
#define DSU_STATUSA_MASK            0x1Ful       /**< \brief (DSU_STATUSA) MASK Register */

/* -------- DSU_STATUSB : (DSU Offset: 0x0002) (R/   8) Status B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  PROT:1;           /*!< bit:      0  Protected                          */
    uint8_t  DBGPRES:1;        /*!< bit:      1  Debugger Present                   */
    uint8_t  DCCD0:1;          /*!< bit:      2  Debug Communication Channel 0 Dirty */
    uint8_t  DCCD1:1;          /*!< bit:      3  Debug Communication Channel 1 Dirty */
    uint8_t  HPE:1;            /*!< bit:      4  Hot-Plugging Enable                */
    uint8_t  :3;               /*!< bit:  5.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  struct {
    uint8_t  :2;               /*!< bit:  0.. 1  Reserved                           */
    uint8_t  DCCD:2;           /*!< bit:  2.. 3  Debug Communication Channel x Dirty */
    uint8_t  :4;               /*!< bit:  4.. 7  Reserved                           */
  } vec;                       /*!< Structure used for vec  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DSU_STATUSB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_STATUSB_OFFSET          0x0002       /**< \brief (DSU_STATUSB offset) Status B */
#define DSU_STATUSB_RESETVALUE      0x00ul       /**< \brief (DSU_STATUSB reset_value) Status B */

#define DSU_STATUSB_PROT_Pos        0            /**< \brief (DSU_STATUSB) Protected */
#define DSU_STATUSB_PROT            (0x1ul << DSU_STATUSB_PROT_Pos)
#define DSU_STATUSB_DBGPRES_Pos     1            /**< \brief (DSU_STATUSB) Debugger Present */
#define DSU_STATUSB_DBGPRES         (0x1ul << DSU_STATUSB_DBGPRES_Pos)
#define DSU_STATUSB_DCCD0_Pos       2            /**< \brief (DSU_STATUSB) Debug Communication Channel 0 Dirty */
#define DSU_STATUSB_DCCD0           (1 << DSU_STATUSB_DCCD0_Pos)
#define DSU_STATUSB_DCCD1_Pos       3            /**< \brief (DSU_STATUSB) Debug Communication Channel 1 Dirty */
#define DSU_STATUSB_DCCD1           (1 << DSU_STATUSB_DCCD1_Pos)
#define DSU_STATUSB_DCCD_Pos        2            /**< \brief (DSU_STATUSB) Debug Communication Channel x Dirty */
#define DSU_STATUSB_DCCD_Msk        (0x3ul << DSU_STATUSB_DCCD_Pos)
#define DSU_STATUSB_DCCD(value)     (DSU_STATUSB_DCCD_Msk & ((value) << DSU_STATUSB_DCCD_Pos))
#define DSU_STATUSB_HPE_Pos         4            /**< \brief (DSU_STATUSB) Hot-Plugging Enable */
#define DSU_STATUSB_HPE             (0x1ul << DSU_STATUSB_HPE_Pos)
#define DSU_STATUSB_MASK            0x1Ful       /**< \brief (DSU_STATUSB) MASK Register */

/* -------- DSU_ADDR : (DSU Offset: 0x0004) (R/W 32) Address -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t AMOD:2;           /*!< bit:  0.. 1  Access Mode                        */
    uint32_t ADDR:30;          /*!< bit:  2..31  Address                            */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_ADDR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_ADDR_OFFSET             0x0004       /**< \brief (DSU_ADDR offset) Address */
#define DSU_ADDR_RESETVALUE         0x00000000ul /**< \brief (DSU_ADDR reset_value) Address */

#define DSU_ADDR_AMOD_Pos           0            /**< \brief (DSU_ADDR) Access Mode */
#define DSU_ADDR_AMOD_Msk           (0x3ul << DSU_ADDR_AMOD_Pos)
#define DSU_ADDR_AMOD(value)        (DSU_ADDR_AMOD_Msk & ((value) << DSU_ADDR_AMOD_Pos))
#define DSU_ADDR_ADDR_Pos           2            /**< \brief (DSU_ADDR) Address */
#define DSU_ADDR_ADDR_Msk           (0x3FFFFFFFul << DSU_ADDR_ADDR_Pos)
#define DSU_ADDR_ADDR(value)        (DSU_ADDR_ADDR_Msk & ((value) << DSU_ADDR_ADDR_Pos))
#define DSU_ADDR_MASK               0xFFFFFFFFul /**< \brief (DSU_ADDR) MASK Register */

/* -------- DSU_LENGTH : (DSU Offset: 0x0008) (R/W 32) Length -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint32_t LENGTH:30;        /*!< bit:  2..31  Length                             */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_LENGTH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_LENGTH_OFFSET           0x0008       /**< \brief (DSU_LENGTH offset) Length */
#define DSU_LENGTH_RESETVALUE       0x00000000ul /**< \brief (DSU_LENGTH reset_value) Length */

#define DSU_LENGTH_LENGTH_Pos       2            /**< \brief (DSU_LENGTH) Length */
#define DSU_LENGTH_LENGTH_Msk       (0x3FFFFFFFul << DSU_LENGTH_LENGTH_Pos)
#define DSU_LENGTH_LENGTH(value)    (DSU_LENGTH_LENGTH_Msk & ((value) << DSU_LENGTH_LENGTH_Pos))
#define DSU_LENGTH_MASK             0xFFFFFFFCul /**< \brief (DSU_LENGTH) MASK Register */

/* -------- DSU_DATA : (DSU Offset: 0x000C) (R/W 32) Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Data                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DATA_OFFSET             0x000C       /**< \brief (DSU_DATA offset) Data */
#define DSU_DATA_RESETVALUE         0x00000000ul /**< \brief (DSU_DATA reset_value) Data */

#define DSU_DATA_DATA_Pos           0            /**< \brief (DSU_DATA) Data */
#define DSU_DATA_DATA_Msk           (0xFFFFFFFFul << DSU_DATA_DATA_Pos)
#define DSU_DATA_DATA(value)        (DSU_DATA_DATA_Msk & ((value) << DSU_DATA_DATA_Pos))
#define DSU_DATA_MASK               0xFFFFFFFFul /**< \brief (DSU_DATA) MASK Register */

/* -------- DSU_DCC : (DSU Offset: 0x0010) (R/W 32) Debug Communication Channel n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DATA:32;          /*!< bit:  0..31  Data                               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DCC_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DCC_OFFSET              0x0010       /**< \brief (DSU_DCC offset) Debug Communication Channel n */
#define DSU_DCC_RESETVALUE          0x00000000ul /**< \brief (DSU_DCC reset_value) Debug Communication Channel n */

#define DSU_DCC_DATA_Pos            0            /**< \brief (DSU_DCC) Data */
#define DSU_DCC_DATA_Msk            (0xFFFFFFFFul << DSU_DCC_DATA_Pos)
#define DSU_DCC_DATA(value)         (DSU_DCC_DATA_Msk & ((value) << DSU_DCC_DATA_Pos))
#define DSU_DCC_MASK                0xFFFFFFFFul /**< \brief (DSU_DCC) MASK Register */

/* -------- DSU_DID : (DSU Offset: 0x0018) (R/  32) Device Identification -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DEVSEL:8;         /*!< bit:  0.. 7  Device Select                      */
    uint32_t REVISION:4;       /*!< bit:  8..11  Revision Number                    */
    uint32_t DIE:4;            /*!< bit: 12..15  Die Number                         */
    uint32_t SERIES:6;         /*!< bit: 16..21  Series                             */
    uint32_t :1;               /*!< bit:     22  Reserved                           */
    uint32_t FAMILY:5;         /*!< bit: 23..27  Family                             */
    uint32_t PROCESSOR:4;      /*!< bit: 28..31  Processor                          */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DID_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DID_OFFSET              0x0018       /**< \brief (DSU_DID offset) Device Identification */

#define DSU_DID_DEVSEL_Pos          0            /**< \brief (DSU_DID) Device Select */
#define DSU_DID_DEVSEL_Msk          (0xFFul << DSU_DID_DEVSEL_Pos)
#define DSU_DID_DEVSEL(value)       (DSU_DID_DEVSEL_Msk & ((value) << DSU_DID_DEVSEL_Pos))
#define DSU_DID_REVISION_Pos        8            /**< \brief (DSU_DID) Revision Number */
#define DSU_DID_REVISION_Msk        (0xFul << DSU_DID_REVISION_Pos)
#define DSU_DID_REVISION(value)     (DSU_DID_REVISION_Msk & ((value) << DSU_DID_REVISION_Pos))
#define DSU_DID_DIE_Pos             12           /**< \brief (DSU_DID) Die Number */
#define DSU_DID_DIE_Msk             (0xFul << DSU_DID_DIE_Pos)
#define DSU_DID_DIE(value)          (DSU_DID_DIE_Msk & ((value) << DSU_DID_DIE_Pos))
#define DSU_DID_SERIES_Pos          16           /**< \brief (DSU_DID) Series */
#define DSU_DID_SERIES_Msk          (0x3Ful << DSU_DID_SERIES_Pos)
#define DSU_DID_SERIES(value)       (DSU_DID_SERIES_Msk & ((value) << DSU_DID_SERIES_Pos))
#define   DSU_DID_SERIES_0_Val            0x0ul  /**< \brief (DSU_DID) Cortex-M0+ processor, basic feature set */
#define   DSU_DID_SERIES_1_Val            0x1ul  /**< \brief (DSU_DID) Cortex-M0+ processor, USB */
#define DSU_DID_SERIES_0            (DSU_DID_SERIES_0_Val          << DSU_DID_SERIES_Pos)
#define DSU_DID_SERIES_1            (DSU_DID_SERIES_1_Val          << DSU_DID_SERIES_Pos)
#define DSU_DID_FAMILY_Pos          23           /**< \brief (DSU_DID) Family */
#define DSU_DID_FAMILY_Msk          (0x1Ful << DSU_DID_FAMILY_Pos)
#define DSU_DID_FAMILY(value)       (DSU_DID_FAMILY_Msk & ((value) << DSU_DID_FAMILY_Pos))
#define   DSU_DID_FAMILY_0_Val            0x0ul  /**< \brief (DSU_DID) General purpose microcontroller */
#define   DSU_DID_FAMILY_1_Val            0x1ul  /**< \brief (DSU_DID) PicoPower */
#define DSU_DID_FAMILY_0            (DSU_DID_FAMILY_0_Val          << DSU_DID_FAMILY_Pos)
#define DSU_DID_FAMILY_1            (DSU_DID_FAMILY_1_Val          << DSU_DID_FAMILY_Pos)
#define DSU_DID_PROCESSOR_Pos       28           /**< \brief (DSU_DID) Processor */
#define DSU_DID_PROCESSOR_Msk       (0xFul << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR(value)    (DSU_DID_PROCESSOR_Msk & ((value) << DSU_DID_PROCESSOR_Pos))
#define   DSU_DID_PROCESSOR_0_Val         0x0ul  /**< \brief (DSU_DID) Cortex-M0 */
#define   DSU_DID_PROCESSOR_1_Val         0x1ul  /**< \brief (DSU_DID) Cortex-M0+ */
#define   DSU_DID_PROCESSOR_2_Val         0x2ul  /**< \brief (DSU_DID) Cortex-M3 */
#define   DSU_DID_PROCESSOR_3_Val         0x3ul  /**< \brief (DSU_DID) Cortex-M4 */
#define DSU_DID_PROCESSOR_0         (DSU_DID_PROCESSOR_0_Val       << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_1         (DSU_DID_PROCESSOR_1_Val       << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_2         (DSU_DID_PROCESSOR_2_Val       << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_PROCESSOR_3         (DSU_DID_PROCESSOR_3_Val       << DSU_DID_PROCESSOR_Pos)
#define DSU_DID_MASK                0xFFBFFFFFul /**< \brief (DSU_DID) MASK Register */

/* -------- DSU_DCFG : (DSU Offset: 0x00F0) (R/W 32) Device Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t DCFG:32;          /*!< bit:  0..31  Device Configuration               */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_DCFG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_DCFG_OFFSET             0x00F0       /**< \brief (DSU_DCFG offset) Device Configuration */
#define DSU_DCFG_RESETVALUE         0x00000000ul /**< \brief (DSU_DCFG reset_value) Device Configuration */

#define DSU_DCFG_DCFG_Pos           0            /**< \brief (DSU_DCFG) Device Configuration */
#define DSU_DCFG_DCFG_Msk           (0xFFFFFFFFul << DSU_DCFG_DCFG_Pos)
#define DSU_DCFG_DCFG(value)        (DSU_DCFG_DCFG_Msk & ((value) << DSU_DCFG_DCFG_Pos))
#define DSU_DCFG_MASK               0xFFFFFFFFul /**< \brief (DSU_DCFG) MASK Register */

/* -------- DSU_ENTRY : (DSU Offset: 0x1000) (R/  32) Coresight ROM Table Entry n -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t EPRES:1;          /*!< bit:      0  Entry Present                      */
    uint32_t FMT:1;            /*!< bit:      1  Format                             */
    uint32_t :10;              /*!< bit:  2..11  Reserved                           */
    uint32_t ADDOFF:20;        /*!< bit: 12..31  Address Offset                     */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_ENTRY_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_ENTRY_OFFSET            0x1000       /**< \brief (DSU_ENTRY offset) Coresight ROM Table Entry n */

#define DSU_ENTRY_EPRES_Pos         0            /**< \brief (DSU_ENTRY) Entry Present */
#define DSU_ENTRY_EPRES             (0x1ul << DSU_ENTRY_EPRES_Pos)
#define DSU_ENTRY_FMT_Pos           1            /**< \brief (DSU_ENTRY) Format */
#define DSU_ENTRY_FMT               (0x1ul << DSU_ENTRY_FMT_Pos)
#define DSU_ENTRY_ADDOFF_Pos        12           /**< \brief (DSU_ENTRY) Address Offset */
#define DSU_ENTRY_ADDOFF_Msk        (0xFFFFFul << DSU_ENTRY_ADDOFF_Pos)
#define DSU_ENTRY_ADDOFF(value)     (DSU_ENTRY_ADDOFF_Msk & ((value) << DSU_ENTRY_ADDOFF_Pos))
#define DSU_ENTRY_MASK              0xFFFFF003ul /**< \brief (DSU_ENTRY) MASK Register */

/* -------- DSU_END : (DSU Offset: 0x1008) (R/  32) Coresight ROM Table End -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t END:32;           /*!< bit:  0..31  End Marker                         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_END_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_END_OFFSET              0x1008       /**< \brief (DSU_END offset) Coresight ROM Table End */
#define DSU_END_RESETVALUE          0x00000000ul /**< \brief (DSU_END reset_value) Coresight ROM Table End */

#define DSU_END_END_Pos             0            /**< \brief (DSU_END) End Marker */
#define DSU_END_END_Msk             (0xFFFFFFFFul << DSU_END_END_Pos)
#define DSU_END_END(value)          (DSU_END_END_Msk & ((value) << DSU_END_END_Pos))
#define DSU_END_MASK                0xFFFFFFFFul /**< \brief (DSU_END) MASK Register */

/* -------- DSU_MEMTYPE : (DSU Offset: 0x1FCC) (R/  32) Coresight ROM Table Memory Type -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t SMEMP:1;          /*!< bit:      0  System Memory Present              */
    uint32_t :31;              /*!< bit:  1..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_MEMTYPE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_MEMTYPE_OFFSET          0x1FCC       /**< \brief (DSU_MEMTYPE offset) Coresight ROM Table Memory Type */
#define DSU_MEMTYPE_RESETVALUE      0x00000000ul /**< \brief (DSU_MEMTYPE reset_value) Coresight ROM Table Memory Type */

#define DSU_MEMTYPE_SMEMP_Pos       0            /**< \brief (DSU_MEMTYPE) System Memory Present */
#define DSU_MEMTYPE_SMEMP           (0x1ul << DSU_MEMTYPE_SMEMP_Pos)
#define DSU_MEMTYPE_MASK            0x00000001ul /**< \brief (DSU_MEMTYPE) MASK Register */

/* -------- DSU_PID4 : (DSU Offset: 0x1FD0) (R/  32) Peripheral Identification 4 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t JEPCC:4;          /*!< bit:  0.. 3  JEP-106 Continuation Code          */
    uint32_t FKBC:4;           /*!< bit:  4.. 7  4KB count                          */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID4_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID4_OFFSET             0x1FD0       /**< \brief (DSU_PID4 offset) Peripheral Identification 4 */
#define DSU_PID4_RESETVALUE         0x00000000ul /**< \brief (DSU_PID4 reset_value) Peripheral Identification 4 */

#define DSU_PID4_JEPCC_Pos          0            /**< \brief (DSU_PID4) JEP-106 Continuation Code */
#define DSU_PID4_JEPCC_Msk          (0xFul << DSU_PID4_JEPCC_Pos)
#define DSU_PID4_JEPCC(value)       (DSU_PID4_JEPCC_Msk & ((value) << DSU_PID4_JEPCC_Pos))
#define DSU_PID4_FKBC_Pos           4            /**< \brief (DSU_PID4) 4KB count */
#define DSU_PID4_FKBC_Msk           (0xFul << DSU_PID4_FKBC_Pos)
#define DSU_PID4_FKBC(value)        (DSU_PID4_FKBC_Msk & ((value) << DSU_PID4_FKBC_Pos))
#define DSU_PID4_MASK               0x000000FFul /**< \brief (DSU_PID4) MASK Register */

/* -------- DSU_PID5 : (DSU Offset: 0x1FD4) (R/  32) Peripheral Identification 5 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID5_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID5_OFFSET             0x1FD4       /**< \brief (DSU_PID5 offset) Peripheral Identification 5 */
#define DSU_PID5_MASK               0x00000000ul /**< \brief (DSU_PID5) MASK Register */

/* -------- DSU_PID6 : (DSU Offset: 0x1FD8) (R/  32) Peripheral Identification 6 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID6_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID6_OFFSET             0x1FD8       /**< \brief (DSU_PID6 offset) Peripheral Identification 6 */
#define DSU_PID6_MASK               0x00000000ul /**< \brief (DSU_PID6) MASK Register */

/* -------- DSU_PID7 : (DSU Offset: 0x1FDC) (R/  32) Peripheral Identification 7 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID7_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID7_OFFSET             0x1FDC       /**< \brief (DSU_PID7 offset) Peripheral Identification 7 */
#define DSU_PID7_MASK               0x00000000ul /**< \brief (DSU_PID7) MASK Register */

/* -------- DSU_PID0 : (DSU Offset: 0x1FE0) (R/  32) Peripheral Identification 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PARTNBL:8;        /*!< bit:  0.. 7  Part Number Low                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID0_OFFSET             0x1FE0       /**< \brief (DSU_PID0 offset) Peripheral Identification 0 */
#define DSU_PID0_RESETVALUE         0x00000000ul /**< \brief (DSU_PID0 reset_value) Peripheral Identification 0 */

#define DSU_PID0_PARTNBL_Pos        0            /**< \brief (DSU_PID0) Part Number Low */
#define DSU_PID0_PARTNBL_Msk        (0xFFul << DSU_PID0_PARTNBL_Pos)
#define DSU_PID0_PARTNBL(value)     (DSU_PID0_PARTNBL_Msk & ((value) << DSU_PID0_PARTNBL_Pos))
#define DSU_PID0_MASK               0x000000FFul /**< \brief (DSU_PID0) MASK Register */

/* -------- DSU_PID1 : (DSU Offset: 0x1FE4) (R/  32) Peripheral Identification 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PARTNBH:4;        /*!< bit:  0.. 3  Part Number High                   */
    uint32_t JEPIDCL:4;        /*!< bit:  4.. 7  Low part of the JEP-106 Identity Code */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID1_OFFSET             0x1FE4       /**< \brief (DSU_PID1 offset) Peripheral Identification 1 */
#define DSU_PID1_RESETVALUE         0x000000FCul /**< \brief (DSU_PID1 reset_value) Peripheral Identification 1 */

#define DSU_PID1_PARTNBH_Pos        0            /**< \brief (DSU_PID1) Part Number High */
#define DSU_PID1_PARTNBH_Msk        (0xFul << DSU_PID1_PARTNBH_Pos)
#define DSU_PID1_PARTNBH(value)     (DSU_PID1_PARTNBH_Msk & ((value) << DSU_PID1_PARTNBH_Pos))
#define DSU_PID1_JEPIDCL_Pos        4            /**< \brief (DSU_PID1) Low part of the JEP-106 Identity Code */
#define DSU_PID1_JEPIDCL_Msk        (0xFul << DSU_PID1_JEPIDCL_Pos)
#define DSU_PID1_JEPIDCL(value)     (DSU_PID1_JEPIDCL_Msk & ((value) << DSU_PID1_JEPIDCL_Pos))
#define DSU_PID1_MASK               0x000000FFul /**< \brief (DSU_PID1) MASK Register */

/* -------- DSU_PID2 : (DSU Offset: 0x1FE8) (R/  32) Peripheral Identification 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t JEPIDCH:3;        /*!< bit:  0.. 2  JEP-106 Identity Code High         */
    uint32_t JEPU:1;           /*!< bit:      3  JEP-106 Identity Code is used      */
    uint32_t REVISION:4;       /*!< bit:  4.. 7  Revision Number                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID2_OFFSET             0x1FE8       /**< \brief (DSU_PID2 offset) Peripheral Identification 2 */
#define DSU_PID2_RESETVALUE         0x00000009ul /**< \brief (DSU_PID2 reset_value) Peripheral Identification 2 */

#define DSU_PID2_JEPIDCH_Pos        0            /**< \brief (DSU_PID2) JEP-106 Identity Code High */
#define DSU_PID2_JEPIDCH_Msk        (0x7ul << DSU_PID2_JEPIDCH_Pos)
#define DSU_PID2_JEPIDCH(value)     (DSU_PID2_JEPIDCH_Msk & ((value) << DSU_PID2_JEPIDCH_Pos))
#define DSU_PID2_JEPU_Pos           3            /**< \brief (DSU_PID2) JEP-106 Identity Code is used */
#define DSU_PID2_JEPU               (0x1ul << DSU_PID2_JEPU_Pos)
#define DSU_PID2_REVISION_Pos       4            /**< \brief (DSU_PID2) Revision Number */
#define DSU_PID2_REVISION_Msk       (0xFul << DSU_PID2_REVISION_Pos)
#define DSU_PID2_REVISION(value)    (DSU_PID2_REVISION_Msk & ((value) << DSU_PID2_REVISION_Pos))
#define DSU_PID2_MASK               0x000000FFul /**< \brief (DSU_PID2) MASK Register */

/* -------- DSU_PID3 : (DSU Offset: 0x1FEC) (R/  32) Peripheral Identification 3 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t CUSMOD:4;         /*!< bit:  0.. 3  ARM CUSMOD                         */
    uint32_t REVAND:4;         /*!< bit:  4.. 7  Revision Number                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_PID3_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_PID3_OFFSET             0x1FEC       /**< \brief (DSU_PID3 offset) Peripheral Identification 3 */
#define DSU_PID3_RESETVALUE         0x00000000ul /**< \brief (DSU_PID3 reset_value) Peripheral Identification 3 */

#define DSU_PID3_CUSMOD_Pos         0            /**< \brief (DSU_PID3) ARM CUSMOD */
#define DSU_PID3_CUSMOD_Msk         (0xFul << DSU_PID3_CUSMOD_Pos)
#define DSU_PID3_CUSMOD(value)      (DSU_PID3_CUSMOD_Msk & ((value) << DSU_PID3_CUSMOD_Pos))
#define DSU_PID3_REVAND_Pos         4            /**< \brief (DSU_PID3) Revision Number */
#define DSU_PID3_REVAND_Msk         (0xFul << DSU_PID3_REVAND_Pos)
#define DSU_PID3_REVAND(value)      (DSU_PID3_REVAND_Msk & ((value) << DSU_PID3_REVAND_Pos))
#define DSU_PID3_MASK               0x000000FFul /**< \brief (DSU_PID3) MASK Register */

/* -------- DSU_CID0 : (DSU Offset: 0x1FF0) (R/  32) Component Identification 0 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLEB0:8;     /*!< bit:  0.. 7  Preamble Byte 0                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID0_OFFSET             0x1FF0       /**< \brief (DSU_CID0 offset) Component Identification 0 */
#define DSU_CID0_RESETVALUE         0x00000000ul /**< \brief (DSU_CID0 reset_value) Component Identification 0 */

#define DSU_CID0_PREAMBLEB0_Pos     0            /**< \brief (DSU_CID0) Preamble Byte 0 */
#define DSU_CID0_PREAMBLEB0_Msk     (0xFFul << DSU_CID0_PREAMBLEB0_Pos)
#define DSU_CID0_PREAMBLEB0(value)  (DSU_CID0_PREAMBLEB0_Msk & ((value) << DSU_CID0_PREAMBLEB0_Pos))
#define DSU_CID0_MASK               0x000000FFul /**< \brief (DSU_CID0) MASK Register */

/* -------- DSU_CID1 : (DSU Offset: 0x1FF4) (R/  32) Component Identification 1 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLE:4;       /*!< bit:  0.. 3  Preamble                           */
    uint32_t CCLASS:4;         /*!< bit:  4.. 7  Component Class                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID1_OFFSET             0x1FF4       /**< \brief (DSU_CID1 offset) Component Identification 1 */
#define DSU_CID1_RESETVALUE         0x00000000ul /**< \brief (DSU_CID1 reset_value) Component Identification 1 */

#define DSU_CID1_PREAMBLE_Pos       0            /**< \brief (DSU_CID1) Preamble */
#define DSU_CID1_PREAMBLE_Msk       (0xFul << DSU_CID1_PREAMBLE_Pos)
#define DSU_CID1_PREAMBLE(value)    (DSU_CID1_PREAMBLE_Msk & ((value) << DSU_CID1_PREAMBLE_Pos))
#define DSU_CID1_CCLASS_Pos         4            /**< \brief (DSU_CID1) Component Class */
#define DSU_CID1_CCLASS_Msk         (0xFul << DSU_CID1_CCLASS_Pos)
#define DSU_CID1_CCLASS(value)      (DSU_CID1_CCLASS_Msk & ((value) << DSU_CID1_CCLASS_Pos))
#define DSU_CID1_MASK               0x000000FFul /**< \brief (DSU_CID1) MASK Register */

/* -------- DSU_CID2 : (DSU Offset: 0x1FF8) (R/  32) Component Identification 2 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLEB2:8;     /*!< bit:  0.. 7  Preamble Byte 2                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID2_OFFSET             0x1FF8       /**< \brief (DSU_CID2 offset) Component Identification 2 */
#define DSU_CID2_RESETVALUE         0x00000000ul /**< \brief (DSU_CID2 reset_value) Component Identification 2 */

#define DSU_CID2_PREAMBLEB2_Pos     0            /**< \brief (DSU_CID2) Preamble Byte 2 */
#define DSU_CID2_PREAMBLEB2_Msk     (0xFFul << DSU_CID2_PREAMBLEB2_Pos)
#define DSU_CID2_PREAMBLEB2(value)  (DSU_CID2_PREAMBLEB2_Msk & ((value) << DSU_CID2_PREAMBLEB2_Pos))
#define DSU_CID2_MASK               0x000000FFul /**< \brief (DSU_CID2) MASK Register */

/* -------- DSU_CID3 : (DSU Offset: 0x1FFC) (R/  32) Component Identification 3 -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t PREAMBLEB3:8;     /*!< bit:  0.. 7  Preamble Byte 3                    */
    uint32_t :24;              /*!< bit:  8..31  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} DSU_CID3_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DSU_CID3_OFFSET             0x1FFC       /**< \brief (DSU_CID3 offset) Component Identification 3 */
#define DSU_CID3_RESETVALUE         0x00000000ul /**< \brief (DSU_CID3 reset_value) Component Identification 3 */

#define DSU_CID3_PREAMBLEB3_Pos     0            /**< \brief (DSU_CID3) Preamble Byte 3 */
#define DSU_CID3_PREAMBLEB3_Msk     (0xFFul << DSU_CID3_PREAMBLEB3_Pos)
#define DSU_CID3_PREAMBLEB3(value)  (DSU_CID3_PREAMBLEB3_Msk & ((value) << DSU_CID3_PREAMBLEB3_Pos))
#define DSU_CID3_MASK               0x000000FFul /**< \brief (DSU_CID3) MASK Register */

/** \brief DSU hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __O  DSU_CTRL_Type             CTRL;        /**< \brief Offset: 0x0000 ( /W  8) Control */
  __IO DSU_STATUSA_Type          STATUSA;     /**< \brief Offset: 0x0001 (R/W  8) Status A */
  __I  DSU_STATUSB_Type          STATUSB;     /**< \brief Offset: 0x0002 (R/   8) Status B */
       RoReg8                    Reserved1[0x1];
  __IO DSU_ADDR_Type             ADDR;        /**< \brief Offset: 0x0004 (R/W 32) Address */
  __IO DSU_LENGTH_Type           LENGTH;      /**< \brief Offset: 0x0008 (R/W 32) Length */
  __IO DSU_DATA_Type             DATA;        /**< \brief Offset: 0x000C (R/W 32) Data */
  __IO DSU_DCC_Type              DCC[2];      /**< \brief Offset: 0x0010 (R/W 32) Debug Communication Channel n */
  __I  DSU_DID_Type              DID;         /**< \brief Offset: 0x0018 (R/  32) Device Identification */
       RoReg8                    Reserved2[0xD4];
  __IO DSU_DCFG_Type             DCFG[2];     /**< \brief Offset: 0x00F0 (R/W 32) Device Configuration */
       RoReg8                    Reserved3[0xF08];
  __I  DSU_ENTRY_Type            ENTRY[2];    /**< \brief Offset: 0x1000 (R/  32) Coresight ROM Table Entry n */
  __I  DSU_END_Type              END;         /**< \brief Offset: 0x1008 (R/  32) Coresight ROM Table End */
       RoReg8                    Reserved4[0xFC0];
  __I  DSU_MEMTYPE_Type          MEMTYPE;     /**< \brief Offset: 0x1FCC (R/  32) Coresight ROM Table Memory Type */
  __I  DSU_PID4_Type             PID4;        /**< \brief Offset: 0x1FD0 (R/  32) Peripheral Identification 4 */
  __I  DSU_PID5_Type             PID5;        /**< \brief Offset: 0x1FD4 (R/  32) Peripheral Identification 5 */
  __I  DSU_PID6_Type             PID6;        /**< \brief Offset: 0x1FD8 (R/  32) Peripheral Identification 6 */
  __I  DSU_PID7_Type             PID7;        /**< \brief Offset: 0x1FDC (R/  32) Peripheral Identification 7 */
  __I  DSU_PID0_Type             PID0;        /**< \brief Offset: 0x1FE0 (R/  32) Peripheral Identification 0 */
  __I  DSU_PID1_Type             PID1;        /**< \brief Offset: 0x1FE4 (R/  32) Peripheral Identification 1 */
  __I  DSU_PID2_Type             PID2;        /**< \brief Offset: 0x1FE8 (R/  32) Peripheral Identification 2 */
  __I  DSU_PID3_Type             PID3;        /**< \brief Offset: 0x1FEC (R/  32) Peripheral Identification 3 */
  __I  DSU_CID0_Type             CID0;        /**< \brief Offset: 0x1FF0 (R/  32) Component Identification 0 */
  __I  DSU_CID1_Type             CID1;        /**< \brief Offset: 0x1FF4 (R/  32) Component Identification 1 */
  __I  DSU_CID2_Type             CID2;        /**< \brief Offset: 0x1FF8 (R/  32) Component Identification 2 */
  __I  DSU_CID3_Type             CID3;        /**< \brief Offset: 0x1FFC (R/  32) Component Identification 3 */
} Dsu;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD10_DSU_COMPONENT_ */
