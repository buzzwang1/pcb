/**
 * \file
 *
 * \brief Component description for DAC
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

#ifndef _SAMD10_DAC_COMPONENT_
#define _SAMD10_DAC_COMPONENT_


 /* ========== Register definition for DAC peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_DAC_CTRLA              (0x42002800U) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (0x42002801U) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (0x42002802U) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (0x42002804U) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (0x42002805U) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (0x42002806U) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (0x42002807U) /**< \brief (DAC) Status */
#define REG_DAC_DATA               (0x42002808U) /**< \brief (DAC) Data */
#define REG_DAC_DATABUF            (0x4200280CU) /**< \brief (DAC) Data Buffer */
#else
#define REG_DAC_CTRLA              (*(RwReg8 *)0x42002800U) /**< \brief (DAC) Control A */
#define REG_DAC_CTRLB              (*(RwReg8 *)0x42002801U) /**< \brief (DAC) Control B */
#define REG_DAC_EVCTRL             (*(RwReg8 *)0x42002802U) /**< \brief (DAC) Event Control */
#define REG_DAC_INTENCLR           (*(RwReg8 *)0x42002804U) /**< \brief (DAC) Interrupt Enable Clear */
#define REG_DAC_INTENSET           (*(RwReg8 *)0x42002805U) /**< \brief (DAC) Interrupt Enable Set */
#define REG_DAC_INTFLAG            (*(RwReg8 *)0x42002806U) /**< \brief (DAC) Interrupt Flag Status and Clear */
#define REG_DAC_STATUS             (*(RoReg8 *)0x42002807U) /**< \brief (DAC) Status */
#define REG_DAC_DATA               (*(RwReg16*)0x42002808U) /**< \brief (DAC) Data */
#define REG_DAC_DATABUF            (*(RwReg16*)0x4200280CU) /**< \brief (DAC) Data Buffer */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========== Instance parameters for DAC peripheral ========== */
#define DAC_DMAC_ID_EMPTY           19       // Index of DMAC EMPTY trigger
#define DAC_GCLK_ID                 22       // Index of Generic Clock

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR DAC */
/* ========================================================================== */
/** \addtogroup SAMD10_DAC Digital Analog Converter */
/*@{*/

#define DAC_U2214
#define REV_DAC                     0x110

/* -------- DAC_CTRLA : (DAC Offset: 0x0) (R/W  8) Control A -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  SWRST:1;          /*!< bit:      0  Software Reset                     */
    uint8_t  ENABLE:1;         /*!< bit:      1  Enable                             */
    uint8_t  RUNSTDBY:1;       /*!< bit:      2  Run in Standby                     */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_CTRLA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_CTRLA_OFFSET            0x0          /**< \brief (DAC_CTRLA offset) Control A */
#define DAC_CTRLA_RESETVALUE        0x00ul       /**< \brief (DAC_CTRLA reset_value) Control A */

#define DAC_CTRLA_SWRST_Pos         0            /**< \brief (DAC_CTRLA) Software Reset */
#define DAC_CTRLA_SWRST             (0x1ul << DAC_CTRLA_SWRST_Pos)
#define DAC_CTRLA_ENABLE_Pos        1            /**< \brief (DAC_CTRLA) Enable */
#define DAC_CTRLA_ENABLE            (0x1ul << DAC_CTRLA_ENABLE_Pos)
#define DAC_CTRLA_RUNSTDBY_Pos      2            /**< \brief (DAC_CTRLA) Run in Standby */
#define DAC_CTRLA_RUNSTDBY          (0x1ul << DAC_CTRLA_RUNSTDBY_Pos)
#define DAC_CTRLA_MASK              0x07ul       /**< \brief (DAC_CTRLA) MASK Register */

/* -------- DAC_CTRLB : (DAC Offset: 0x1) (R/W  8) Control B -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  EOEN:1;           /*!< bit:      0  External Output Enable             */
    uint8_t  IOEN:1;           /*!< bit:      1  Internal Output Enable             */
    uint8_t  LEFTADJ:1;        /*!< bit:      2  Left Adjusted Data                 */
    uint8_t  VPD:1;            /*!< bit:      3  Voltage Pump Disable               */
    uint8_t  BDWP:1;           /*!< bit:      4  Bypass DATABUF Write Protection    */
    uint8_t  :1;               /*!< bit:      5  Reserved                           */
    uint8_t  REFSEL:2;         /*!< bit:  6.. 7  Reference Selection                */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_CTRLB_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_CTRLB_OFFSET            0x1          /**< \brief (DAC_CTRLB offset) Control B */
#define DAC_CTRLB_RESETVALUE        0x00ul       /**< \brief (DAC_CTRLB reset_value) Control B */

#define DAC_CTRLB_EOEN_Pos          0            /**< \brief (DAC_CTRLB) External Output Enable */
#define DAC_CTRLB_EOEN              (0x1ul << DAC_CTRLB_EOEN_Pos)
#define DAC_CTRLB_IOEN_Pos          1            /**< \brief (DAC_CTRLB) Internal Output Enable */
#define DAC_CTRLB_IOEN              (0x1ul << DAC_CTRLB_IOEN_Pos)
#define DAC_CTRLB_LEFTADJ_Pos       2            /**< \brief (DAC_CTRLB) Left Adjusted Data */
#define DAC_CTRLB_LEFTADJ           (0x1ul << DAC_CTRLB_LEFTADJ_Pos)
#define DAC_CTRLB_VPD_Pos           3            /**< \brief (DAC_CTRLB) Voltage Pump Disable */
#define DAC_CTRLB_VPD               (0x1ul << DAC_CTRLB_VPD_Pos)
#define DAC_CTRLB_BDWP_Pos          4            /**< \brief (DAC_CTRLB) Bypass DATABUF Write Protection */
#define DAC_CTRLB_BDWP              (0x1ul << DAC_CTRLB_BDWP_Pos)
#define DAC_CTRLB_REFSEL_Pos        6            /**< \brief (DAC_CTRLB) Reference Selection */
#define DAC_CTRLB_REFSEL_Msk        (0x3ul << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL(value)     (DAC_CTRLB_REFSEL_Msk & ((value) << DAC_CTRLB_REFSEL_Pos))
#define   DAC_CTRLB_REFSEL_INT1V_Val      0x0ul  /**< \brief (DAC_CTRLB) Internal 1.0V reference */
#define   DAC_CTRLB_REFSEL_AVCC_Val       0x1ul  /**< \brief (DAC_CTRLB) AVCC */
#define   DAC_CTRLB_REFSEL_VREFP_Val      0x2ul  /**< \brief (DAC_CTRLB) External reference */
#define DAC_CTRLB_REFSEL_INT1V      (DAC_CTRLB_REFSEL_INT1V_Val    << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_AVCC       (DAC_CTRLB_REFSEL_AVCC_Val     << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_REFSEL_VREFP      (DAC_CTRLB_REFSEL_VREFP_Val    << DAC_CTRLB_REFSEL_Pos)
#define DAC_CTRLB_MASK              0xDFul       /**< \brief (DAC_CTRLB) MASK Register */

/* -------- DAC_EVCTRL : (DAC Offset: 0x2) (R/W  8) Event Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  STARTEI:1;        /*!< bit:      0  Start Conversion Event Input       */
    uint8_t  EMPTYEO:1;        /*!< bit:      1  Data Buffer Empty Event Output     */
    uint8_t  :6;               /*!< bit:  2.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_EVCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_EVCTRL_OFFSET           0x2          /**< \brief (DAC_EVCTRL offset) Event Control */
#define DAC_EVCTRL_RESETVALUE       0x00ul       /**< \brief (DAC_EVCTRL reset_value) Event Control */

#define DAC_EVCTRL_STARTEI_Pos      0            /**< \brief (DAC_EVCTRL) Start Conversion Event Input */
#define DAC_EVCTRL_STARTEI          (0x1ul << DAC_EVCTRL_STARTEI_Pos)
#define DAC_EVCTRL_EMPTYEO_Pos      1            /**< \brief (DAC_EVCTRL) Data Buffer Empty Event Output */
#define DAC_EVCTRL_EMPTYEO          (0x1ul << DAC_EVCTRL_EMPTYEO_Pos)
#define DAC_EVCTRL_MASK             0x03ul       /**< \brief (DAC_EVCTRL) MASK Register */

/* -------- DAC_INTENCLR : (DAC Offset: 0x4) (R/W  8) Interrupt Enable Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  UNDERRUN:1;       /*!< bit:      0  Underrun Interrupt Enable          */
    uint8_t  EMPTY:1;          /*!< bit:      1  Data Buffer Empty Interrupt Enable */
    uint8_t  SYNCRDY:1;        /*!< bit:      2  Synchronization Ready Interrupt Enable */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTENCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTENCLR_OFFSET         0x4          /**< \brief (DAC_INTENCLR offset) Interrupt Enable Clear */
#define DAC_INTENCLR_RESETVALUE     0x00ul       /**< \brief (DAC_INTENCLR reset_value) Interrupt Enable Clear */

#define DAC_INTENCLR_UNDERRUN_Pos   0            /**< \brief (DAC_INTENCLR) Underrun Interrupt Enable */
#define DAC_INTENCLR_UNDERRUN       (0x1ul << DAC_INTENCLR_UNDERRUN_Pos)
#define DAC_INTENCLR_EMPTY_Pos      1            /**< \brief (DAC_INTENCLR) Data Buffer Empty Interrupt Enable */
#define DAC_INTENCLR_EMPTY          (0x1ul << DAC_INTENCLR_EMPTY_Pos)
#define DAC_INTENCLR_SYNCRDY_Pos    2            /**< \brief (DAC_INTENCLR) Synchronization Ready Interrupt Enable */
#define DAC_INTENCLR_SYNCRDY        (0x1ul << DAC_INTENCLR_SYNCRDY_Pos)
#define DAC_INTENCLR_MASK           0x07ul       /**< \brief (DAC_INTENCLR) MASK Register */

/* -------- DAC_INTENSET : (DAC Offset: 0x5) (R/W  8) Interrupt Enable Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  UNDERRUN:1;       /*!< bit:      0  Underrun Interrupt Enable          */
    uint8_t  EMPTY:1;          /*!< bit:      1  Data Buffer Empty Interrupt Enable */
    uint8_t  SYNCRDY:1;        /*!< bit:      2  Synchronization Ready Interrupt Enable */
    uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTENSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTENSET_OFFSET         0x5          /**< \brief (DAC_INTENSET offset) Interrupt Enable Set */
#define DAC_INTENSET_RESETVALUE     0x00ul       /**< \brief (DAC_INTENSET reset_value) Interrupt Enable Set */

#define DAC_INTENSET_UNDERRUN_Pos   0            /**< \brief (DAC_INTENSET) Underrun Interrupt Enable */
#define DAC_INTENSET_UNDERRUN       (0x1ul << DAC_INTENSET_UNDERRUN_Pos)
#define DAC_INTENSET_EMPTY_Pos      1            /**< \brief (DAC_INTENSET) Data Buffer Empty Interrupt Enable */
#define DAC_INTENSET_EMPTY          (0x1ul << DAC_INTENSET_EMPTY_Pos)
#define DAC_INTENSET_SYNCRDY_Pos    2            /**< \brief (DAC_INTENSET) Synchronization Ready Interrupt Enable */
#define DAC_INTENSET_SYNCRDY        (0x1ul << DAC_INTENSET_SYNCRDY_Pos)
#define DAC_INTENSET_MASK           0x07ul       /**< \brief (DAC_INTENSET) MASK Register */

/* -------- DAC_INTFLAG : (DAC Offset: 0x6) (R/W  8) Interrupt Flag Status and Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union { // __I to avoid read-modify-write on write-to-clear register
  struct {
    __I uint8_t  UNDERRUN:1;       /*!< bit:      0  Underrun                           */
    __I uint8_t  EMPTY:1;          /*!< bit:      1  Data Buffer Empty                  */
    __I uint8_t  SYNCRDY:1;        /*!< bit:      2  Synchronization Ready              */
    __I uint8_t  :5;               /*!< bit:  3.. 7  Reserved                           */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_INTFLAG_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_INTFLAG_OFFSET          0x6          /**< \brief (DAC_INTFLAG offset) Interrupt Flag Status and Clear */
#define DAC_INTFLAG_RESETVALUE      0x00ul       /**< \brief (DAC_INTFLAG reset_value) Interrupt Flag Status and Clear */

#define DAC_INTFLAG_UNDERRUN_Pos    0            /**< \brief (DAC_INTFLAG) Underrun */
#define DAC_INTFLAG_UNDERRUN        (0x1ul << DAC_INTFLAG_UNDERRUN_Pos)
#define DAC_INTFLAG_EMPTY_Pos       1            /**< \brief (DAC_INTFLAG) Data Buffer Empty */
#define DAC_INTFLAG_EMPTY           (0x1ul << DAC_INTFLAG_EMPTY_Pos)
#define DAC_INTFLAG_SYNCRDY_Pos     2            /**< \brief (DAC_INTFLAG) Synchronization Ready */
#define DAC_INTFLAG_SYNCRDY         (0x1ul << DAC_INTFLAG_SYNCRDY_Pos)
#define DAC_INTFLAG_MASK            0x07ul       /**< \brief (DAC_INTFLAG) MASK Register */

/* -------- DAC_STATUS : (DAC Offset: 0x7) (R/   8) Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint8_t  :7;               /*!< bit:  0.. 6  Reserved                           */
    uint8_t  SYNCBUSY:1;       /*!< bit:      7  Synchronization Busy Status        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint8_t reg;                 /*!< Type      used for register access              */
} DAC_STATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_STATUS_OFFSET           0x7          /**< \brief (DAC_STATUS offset) Status */
#define DAC_STATUS_RESETVALUE       0x00ul       /**< \brief (DAC_STATUS reset_value) Status */

#define DAC_STATUS_SYNCBUSY_Pos     7            /**< \brief (DAC_STATUS) Synchronization Busy Status */
#define DAC_STATUS_SYNCBUSY         (0x1ul << DAC_STATUS_SYNCBUSY_Pos)
#define DAC_STATUS_MASK             0x80ul       /**< \brief (DAC_STATUS) MASK Register */

/* -------- DAC_DATA : (DAC Offset: 0x8) (R/W 16) Data -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DATA:16;          /*!< bit:  0..15  Data value to be converted         */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DATA_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DATA_OFFSET             0x8          /**< \brief (DAC_DATA offset) Data */
#define DAC_DATA_RESETVALUE         0x0000ul     /**< \brief (DAC_DATA reset_value) Data */

#define DAC_DATA_DATA_Pos           0            /**< \brief (DAC_DATA) Data value to be converted */
#define DAC_DATA_DATA_Msk           (0xFFFFul << DAC_DATA_DATA_Pos)
#define DAC_DATA_DATA(value)        (DAC_DATA_DATA_Msk & ((value) << DAC_DATA_DATA_Pos))
#define DAC_DATA_MASK               0xFFFFul     /**< \brief (DAC_DATA) MASK Register */

/* -------- DAC_DATABUF : (DAC Offset: 0xC) (R/W 16) Data Buffer -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint16_t DATABUF:16;       /*!< bit:  0..15  Data Buffer                        */
  } bit;                       /*!< Structure used for bit  access                  */
  uint16_t reg;                /*!< Type      used for register access              */
} DAC_DATABUF_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define DAC_DATABUF_OFFSET          0xC          /**< \brief (DAC_DATABUF offset) Data Buffer */
#define DAC_DATABUF_RESETVALUE      0x0000ul     /**< \brief (DAC_DATABUF reset_value) Data Buffer */

#define DAC_DATABUF_DATABUF_Pos     0            /**< \brief (DAC_DATABUF) Data Buffer */
#define DAC_DATABUF_DATABUF_Msk     (0xFFFFul << DAC_DATABUF_DATABUF_Pos)
#define DAC_DATABUF_DATABUF(value)  (DAC_DATABUF_DATABUF_Msk & ((value) << DAC_DATABUF_DATABUF_Pos))
#define DAC_DATABUF_MASK            0xFFFFul     /**< \brief (DAC_DATABUF) MASK Register */

/** \brief DAC hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO DAC_CTRLA_Type            CTRLA;       /**< \brief Offset: 0x0 (R/W  8) Control A */
  __IO DAC_CTRLB_Type            CTRLB;       /**< \brief Offset: 0x1 (R/W  8) Control B */
  __IO DAC_EVCTRL_Type           EVCTRL;      /**< \brief Offset: 0x2 (R/W  8) Event Control */
       RoReg8                    Reserved1[0x1];
  __IO DAC_INTENCLR_Type         INTENCLR;    /**< \brief Offset: 0x4 (R/W  8) Interrupt Enable Clear */
  __IO DAC_INTENSET_Type         INTENSET;    /**< \brief Offset: 0x5 (R/W  8) Interrupt Enable Set */
  __IO DAC_INTFLAG_Type          INTFLAG;     /**< \brief Offset: 0x6 (R/W  8) Interrupt Flag Status and Clear */
  __I  DAC_STATUS_Type           STATUS;      /**< \brief Offset: 0x7 (R/   8) Status */
  __IO DAC_DATA_Type             DATA;        /**< \brief Offset: 0x8 (R/W 16) Data */
       RoReg8                    Reserved2[0x2];
  __IO DAC_DATABUF_Type          DATABUF;     /**< \brief Offset: 0xC (R/W 16) Data Buffer */
} Dac;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */



/*@}*/

#endif /* _SAMD10_DAC_COMPONENT_ */
