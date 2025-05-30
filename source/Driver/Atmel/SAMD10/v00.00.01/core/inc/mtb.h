/**
 * \file
 *
 * \brief Component description for MTB
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

#ifndef _SAMD10_MTB_COMPONENT_
#define _SAMD10_MTB_COMPONENT_

 /* ========== Register definition for MTB peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_MTB_POSITION           (0x41006000U) /**< \brief (MTB) MTB Position */
#define REG_MTB_MASTER             (0x41006004U) /**< \brief (MTB) MTB Master */
#define REG_MTB_FLOW               (0x41006008U) /**< \brief (MTB) MTB Flow */
#define REG_MTB_BASE               (0x4100600CU) /**< \brief (MTB) MTB Base */
#define REG_MTB_ITCTRL             (0x41006F00U) /**< \brief (MTB) MTB Integration Mode Control */
#define REG_MTB_CLAIMSET           (0x41006FA0U) /**< \brief (MTB) MTB Claim Set */
#define REG_MTB_CLAIMCLR           (0x41006FA4U) /**< \brief (MTB) MTB Claim Clear */
#define REG_MTB_LOCKACCESS         (0x41006FB0U) /**< \brief (MTB) MTB Lock Access */
#define REG_MTB_LOCKSTATUS         (0x41006FB4U) /**< \brief (MTB) MTB Lock Status */
#define REG_MTB_AUTHSTATUS         (0x41006FB8U) /**< \brief (MTB) MTB Authentication Status */
#define REG_MTB_DEVARCH            (0x41006FBCU) /**< \brief (MTB) MTB Device Architecture */
#define REG_MTB_DEVID              (0x41006FC8U) /**< \brief (MTB) MTB Device Configuration */
#define REG_MTB_DEVTYPE            (0x41006FCCU) /**< \brief (MTB) MTB Device Type */
#define REG_MTB_PID4               (0x41006FD0U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID5               (0x41006FD4U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID6               (0x41006FD8U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID7               (0x41006FDCU) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID0               (0x41006FE0U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID1               (0x41006FE4U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID2               (0x41006FE8U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID3               (0x41006FECU) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID0               (0x41006FF0U) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID1               (0x41006FF4U) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID2               (0x41006FF8U) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID3               (0x41006FFCU) /**< \brief (MTB) CoreSight */
#else
#define REG_MTB_POSITION           (*(RwReg  *)0x41006000U) /**< \brief (MTB) MTB Position */
#define REG_MTB_MASTER             (*(RwReg  *)0x41006004U) /**< \brief (MTB) MTB Master */
#define REG_MTB_FLOW               (*(RwReg  *)0x41006008U) /**< \brief (MTB) MTB Flow */
#define REG_MTB_BASE               (*(RoReg  *)0x4100600CU) /**< \brief (MTB) MTB Base */
#define REG_MTB_ITCTRL             (*(RwReg  *)0x41006F00U) /**< \brief (MTB) MTB Integration Mode Control */
#define REG_MTB_CLAIMSET           (*(RwReg  *)0x41006FA0U) /**< \brief (MTB) MTB Claim Set */
#define REG_MTB_CLAIMCLR           (*(RwReg  *)0x41006FA4U) /**< \brief (MTB) MTB Claim Clear */
#define REG_MTB_LOCKACCESS         (*(RwReg  *)0x41006FB0U) /**< \brief (MTB) MTB Lock Access */
#define REG_MTB_LOCKSTATUS         (*(RoReg  *)0x41006FB4U) /**< \brief (MTB) MTB Lock Status */
#define REG_MTB_AUTHSTATUS         (*(RoReg  *)0x41006FB8U) /**< \brief (MTB) MTB Authentication Status */
#define REG_MTB_DEVARCH            (*(RoReg  *)0x41006FBCU) /**< \brief (MTB) MTB Device Architecture */
#define REG_MTB_DEVID              (*(RoReg  *)0x41006FC8U) /**< \brief (MTB) MTB Device Configuration */
#define REG_MTB_DEVTYPE            (*(RoReg  *)0x41006FCCU) /**< \brief (MTB) MTB Device Type */
#define REG_MTB_PID4               (*(RoReg  *)0x41006FD0U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID5               (*(RoReg  *)0x41006FD4U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID6               (*(RoReg  *)0x41006FD8U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID7               (*(RoReg  *)0x41006FDCU) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID0               (*(RoReg  *)0x41006FE0U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID1               (*(RoReg  *)0x41006FE4U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID2               (*(RoReg  *)0x41006FE8U) /**< \brief (MTB) CoreSight */
#define REG_MTB_PID3               (*(RoReg  *)0x41006FECU) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID0               (*(RoReg  *)0x41006FF0U) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID1               (*(RoReg  *)0x41006FF4U) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID2               (*(RoReg  *)0x41006FF8U) /**< \brief (MTB) CoreSight */
#define REG_MTB_CID3               (*(RoReg  *)0x41006FFCU) /**< \brief (MTB) CoreSight */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/* ========================================================================== */
/**  SOFTWARE API DEFINITION FOR MTB */
/* ========================================================================== */
/** \addtogroup SAMD10_MTB Cortex-M0+ Micro-Trace Buffer */
/*@{*/

#define MTB_U2002
#define REV_MTB                     0x100

/* -------- MTB_POSITION : (MTB Offset: 0x000) (R/W 32) MTB Position -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t :2;               /*!< bit:  0.. 1  Reserved                           */
    uint32_t WRAP:1;           /*!< bit:      2  Pointer Value Wraps                */
    uint32_t POINTER:29;       /*!< bit:  3..31  Trace Packet Location Pointer      */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_POSITION_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_POSITION_OFFSET         0x000        /**< \brief (MTB_POSITION offset) MTB Position */

#define MTB_POSITION_WRAP_Pos       2            /**< \brief (MTB_POSITION) Pointer Value Wraps */
#define MTB_POSITION_WRAP           (0x1ul << MTB_POSITION_WRAP_Pos)
#define MTB_POSITION_POINTER_Pos    3            /**< \brief (MTB_POSITION) Trace Packet Location Pointer */
#define MTB_POSITION_POINTER_Msk    (0x1FFFFFFFul << MTB_POSITION_POINTER_Pos)
#define MTB_POSITION_POINTER(value) (MTB_POSITION_POINTER_Msk & ((value) << MTB_POSITION_POINTER_Pos))
#define MTB_POSITION_MASK           0xFFFFFFFCul /**< \brief (MTB_POSITION) MASK Register */

/* -------- MTB_MASTER : (MTB Offset: 0x004) (R/W 32) MTB Master -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t MASK:5;           /*!< bit:  0.. 4  Maximum Value of the Trace Buffer in SRAM */
    uint32_t TSTARTEN:1;       /*!< bit:      5  Trace Start Input Enable           */
    uint32_t TSTOPEN:1;        /*!< bit:      6  Trace Stop Input Enable            */
    uint32_t SFRWPRIV:1;       /*!< bit:      7  Special Function Register Write Privilege */
    uint32_t RAMPRIV:1;        /*!< bit:      8  SRAM Privilege                     */
    uint32_t HALTREQ:1;        /*!< bit:      9  Halt Request                       */
    uint32_t :21;              /*!< bit: 10..30  Reserved                           */
    uint32_t EN:1;             /*!< bit:     31  Main Trace Enable                  */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_MASTER_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_MASTER_OFFSET           0x004        /**< \brief (MTB_MASTER offset) MTB Master */
#define MTB_MASTER_RESETVALUE       0x00000000ul /**< \brief (MTB_MASTER reset_value) MTB Master */

#define MTB_MASTER_MASK_Pos         0            /**< \brief (MTB_MASTER) Maximum Value of the Trace Buffer in SRAM */
#define MTB_MASTER_MASK_Msk         (0x1Ful << MTB_MASTER_MASK_Pos)
#define MTB_MASTER_MASK(value)      (MTB_MASTER_MASK_Msk & ((value) << MTB_MASTER_MASK_Pos))
#define MTB_MASTER_TSTARTEN_Pos     5            /**< \brief (MTB_MASTER) Trace Start Input Enable */
#define MTB_MASTER_TSTARTEN         (0x1ul << MTB_MASTER_TSTARTEN_Pos)
#define MTB_MASTER_TSTOPEN_Pos      6            /**< \brief (MTB_MASTER) Trace Stop Input Enable */
#define MTB_MASTER_TSTOPEN          (0x1ul << MTB_MASTER_TSTOPEN_Pos)
#define MTB_MASTER_SFRWPRIV_Pos     7            /**< \brief (MTB_MASTER) Special Function Register Write Privilege */
#define MTB_MASTER_SFRWPRIV         (0x1ul << MTB_MASTER_SFRWPRIV_Pos)
#define MTB_MASTER_RAMPRIV_Pos      8            /**< \brief (MTB_MASTER) SRAM Privilege */
#define MTB_MASTER_RAMPRIV          (0x1ul << MTB_MASTER_RAMPRIV_Pos)
#define MTB_MASTER_HALTREQ_Pos      9            /**< \brief (MTB_MASTER) Halt Request */
#define MTB_MASTER_HALTREQ          (0x1ul << MTB_MASTER_HALTREQ_Pos)
#define MTB_MASTER_EN_Pos           31           /**< \brief (MTB_MASTER) Main Trace Enable */
#define MTB_MASTER_EN               (0x1ul << MTB_MASTER_EN_Pos)
#define MTB_MASTER_MASK_            0x800003FFul /**< \brief (MTB_MASTER) MASK Register */

/* -------- MTB_FLOW : (MTB Offset: 0x008) (R/W 32) MTB Flow -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  struct {
    uint32_t AUTOSTOP:1;       /*!< bit:      0  Auto Stop Tracing                  */
    uint32_t AUTOHALT:1;       /*!< bit:      1  Auto Halt Request                  */
    uint32_t :1;               /*!< bit:      2  Reserved                           */
    uint32_t WATERMARK:29;     /*!< bit:  3..31  Watermark value                    */
  } bit;                       /*!< Structure used for bit  access                  */
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_FLOW_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_FLOW_OFFSET             0x008        /**< \brief (MTB_FLOW offset) MTB Flow */
#define MTB_FLOW_RESETVALUE         0x00000000ul /**< \brief (MTB_FLOW reset_value) MTB Flow */

#define MTB_FLOW_AUTOSTOP_Pos       0            /**< \brief (MTB_FLOW) Auto Stop Tracing */
#define MTB_FLOW_AUTOSTOP           (0x1ul << MTB_FLOW_AUTOSTOP_Pos)
#define MTB_FLOW_AUTOHALT_Pos       1            /**< \brief (MTB_FLOW) Auto Halt Request */
#define MTB_FLOW_AUTOHALT           (0x1ul << MTB_FLOW_AUTOHALT_Pos)
#define MTB_FLOW_WATERMARK_Pos      3            /**< \brief (MTB_FLOW) Watermark value */
#define MTB_FLOW_WATERMARK_Msk      (0x1FFFFFFFul << MTB_FLOW_WATERMARK_Pos)
#define MTB_FLOW_WATERMARK(value)   (MTB_FLOW_WATERMARK_Msk & ((value) << MTB_FLOW_WATERMARK_Pos))
#define MTB_FLOW_MASK               0xFFFFFFFBul /**< \brief (MTB_FLOW) MASK Register */

/* -------- MTB_BASE : (MTB Offset: 0x00C) (R/  32) MTB Base -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_BASE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_BASE_OFFSET             0x00C        /**< \brief (MTB_BASE offset) MTB Base */
#define MTB_BASE_MASK               0xFFFFFFFFul /**< \brief (MTB_BASE) MASK Register */

/* -------- MTB_ITCTRL : (MTB Offset: 0xF00) (R/W 32) MTB Integration Mode Control -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_ITCTRL_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_ITCTRL_OFFSET           0xF00        /**< \brief (MTB_ITCTRL offset) MTB Integration Mode Control */
#define MTB_ITCTRL_MASK             0xFFFFFFFFul /**< \brief (MTB_ITCTRL) MASK Register */

/* -------- MTB_CLAIMSET : (MTB Offset: 0xFA0) (R/W 32) MTB Claim Set -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_CLAIMSET_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_CLAIMSET_OFFSET         0xFA0        /**< \brief (MTB_CLAIMSET offset) MTB Claim Set */
#define MTB_CLAIMSET_MASK           0xFFFFFFFFul /**< \brief (MTB_CLAIMSET) MASK Register */

/* -------- MTB_CLAIMCLR : (MTB Offset: 0xFA4) (R/W 32) MTB Claim Clear -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_CLAIMCLR_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_CLAIMCLR_OFFSET         0xFA4        /**< \brief (MTB_CLAIMCLR offset) MTB Claim Clear */
#define MTB_CLAIMCLR_MASK           0xFFFFFFFFul /**< \brief (MTB_CLAIMCLR) MASK Register */

/* -------- MTB_LOCKACCESS : (MTB Offset: 0xFB0) (R/W 32) MTB Lock Access -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_LOCKACCESS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_LOCKACCESS_OFFSET       0xFB0        /**< \brief (MTB_LOCKACCESS offset) MTB Lock Access */
#define MTB_LOCKACCESS_MASK         0xFFFFFFFFul /**< \brief (MTB_LOCKACCESS) MASK Register */

/* -------- MTB_LOCKSTATUS : (MTB Offset: 0xFB4) (R/  32) MTB Lock Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_LOCKSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_LOCKSTATUS_OFFSET       0xFB4        /**< \brief (MTB_LOCKSTATUS offset) MTB Lock Status */
#define MTB_LOCKSTATUS_MASK         0xFFFFFFFFul /**< \brief (MTB_LOCKSTATUS) MASK Register */

/* -------- MTB_AUTHSTATUS : (MTB Offset: 0xFB8) (R/  32) MTB Authentication Status -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_AUTHSTATUS_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_AUTHSTATUS_OFFSET       0xFB8        /**< \brief (MTB_AUTHSTATUS offset) MTB Authentication Status */
#define MTB_AUTHSTATUS_MASK         0xFFFFFFFFul /**< \brief (MTB_AUTHSTATUS) MASK Register */

/* -------- MTB_DEVARCH : (MTB Offset: 0xFBC) (R/  32) MTB Device Architecture -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_DEVARCH_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_DEVARCH_OFFSET          0xFBC        /**< \brief (MTB_DEVARCH offset) MTB Device Architecture */
#define MTB_DEVARCH_MASK            0xFFFFFFFFul /**< \brief (MTB_DEVARCH) MASK Register */

/* -------- MTB_DEVID : (MTB Offset: 0xFC8) (R/  32) MTB Device Configuration -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_DEVID_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_DEVID_OFFSET            0xFC8        /**< \brief (MTB_DEVID offset) MTB Device Configuration */
#define MTB_DEVID_MASK              0xFFFFFFFFul /**< \brief (MTB_DEVID) MASK Register */

/* -------- MTB_DEVTYPE : (MTB Offset: 0xFCC) (R/  32) MTB Device Type -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_DEVTYPE_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_DEVTYPE_OFFSET          0xFCC        /**< \brief (MTB_DEVTYPE offset) MTB Device Type */
#define MTB_DEVTYPE_MASK            0xFFFFFFFFul /**< \brief (MTB_DEVTYPE) MASK Register */

/* -------- MTB_PID4 : (MTB Offset: 0xFD0) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID4_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID4_OFFSET             0xFD0        /**< \brief (MTB_PID4 offset) CoreSight */
#define MTB_PID4_MASK               0xFFFFFFFFul /**< \brief (MTB_PID4) MASK Register */

/* -------- MTB_PID5 : (MTB Offset: 0xFD4) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID5_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID5_OFFSET             0xFD4        /**< \brief (MTB_PID5 offset) CoreSight */
#define MTB_PID5_MASK               0xFFFFFFFFul /**< \brief (MTB_PID5) MASK Register */

/* -------- MTB_PID6 : (MTB Offset: 0xFD8) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID6_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID6_OFFSET             0xFD8        /**< \brief (MTB_PID6 offset) CoreSight */
#define MTB_PID6_MASK               0xFFFFFFFFul /**< \brief (MTB_PID6) MASK Register */

/* -------- MTB_PID7 : (MTB Offset: 0xFDC) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID7_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID7_OFFSET             0xFDC        /**< \brief (MTB_PID7 offset) CoreSight */
#define MTB_PID7_MASK               0xFFFFFFFFul /**< \brief (MTB_PID7) MASK Register */

/* -------- MTB_PID0 : (MTB Offset: 0xFE0) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID0_OFFSET             0xFE0        /**< \brief (MTB_PID0 offset) CoreSight */
#define MTB_PID0_MASK               0xFFFFFFFFul /**< \brief (MTB_PID0) MASK Register */

/* -------- MTB_PID1 : (MTB Offset: 0xFE4) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID1_OFFSET             0xFE4        /**< \brief (MTB_PID1 offset) CoreSight */
#define MTB_PID1_MASK               0xFFFFFFFFul /**< \brief (MTB_PID1) MASK Register */

/* -------- MTB_PID2 : (MTB Offset: 0xFE8) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID2_OFFSET             0xFE8        /**< \brief (MTB_PID2 offset) CoreSight */
#define MTB_PID2_MASK               0xFFFFFFFFul /**< \brief (MTB_PID2) MASK Register */

/* -------- MTB_PID3 : (MTB Offset: 0xFEC) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_PID3_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_PID3_OFFSET             0xFEC        /**< \brief (MTB_PID3 offset) CoreSight */
#define MTB_PID3_MASK               0xFFFFFFFFul /**< \brief (MTB_PID3) MASK Register */

/* -------- MTB_CID0 : (MTB Offset: 0xFF0) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_CID0_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_CID0_OFFSET             0xFF0        /**< \brief (MTB_CID0 offset) CoreSight */
#define MTB_CID0_MASK               0xFFFFFFFFul /**< \brief (MTB_CID0) MASK Register */

/* -------- MTB_CID1 : (MTB Offset: 0xFF4) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_CID1_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_CID1_OFFSET             0xFF4        /**< \brief (MTB_CID1 offset) CoreSight */
#define MTB_CID1_MASK               0xFFFFFFFFul /**< \brief (MTB_CID1) MASK Register */

/* -------- MTB_CID2 : (MTB Offset: 0xFF8) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_CID2_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_CID2_OFFSET             0xFF8        /**< \brief (MTB_CID2 offset) CoreSight */
#define MTB_CID2_MASK               0xFFFFFFFFul /**< \brief (MTB_CID2) MASK Register */

/* -------- MTB_CID3 : (MTB Offset: 0xFFC) (R/  32) CoreSight -------- */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef union {
  uint32_t reg;                /*!< Type      used for register access              */
} MTB_CID3_Type;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#define MTB_CID3_OFFSET             0xFFC        /**< \brief (MTB_CID3 offset) CoreSight */
#define MTB_CID3_MASK               0xFFFFFFFFul /**< \brief (MTB_CID3) MASK Register */

/** \brief MTB hardware registers */
#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
typedef struct {
  __IO MTB_POSITION_Type         POSITION;    /**< \brief Offset: 0x000 (R/W 32) MTB Position */
  __IO MTB_MASTER_Type           MASTER;      /**< \brief Offset: 0x004 (R/W 32) MTB Master */
  __IO MTB_FLOW_Type             FLOW;        /**< \brief Offset: 0x008 (R/W 32) MTB Flow */
  __I  MTB_BASE_Type             BASE;        /**< \brief Offset: 0x00C (R/  32) MTB Base */
       RoReg8                    Reserved1[0xEF0];
  __IO MTB_ITCTRL_Type           ITCTRL;      /**< \brief Offset: 0xF00 (R/W 32) MTB Integration Mode Control */
       RoReg8                    Reserved2[0x9C];
  __IO MTB_CLAIMSET_Type         CLAIMSET;    /**< \brief Offset: 0xFA0 (R/W 32) MTB Claim Set */
  __IO MTB_CLAIMCLR_Type         CLAIMCLR;    /**< \brief Offset: 0xFA4 (R/W 32) MTB Claim Clear */
       RoReg8                    Reserved3[0x8];
  __IO MTB_LOCKACCESS_Type       LOCKACCESS;  /**< \brief Offset: 0xFB0 (R/W 32) MTB Lock Access */
  __I  MTB_LOCKSTATUS_Type       LOCKSTATUS;  /**< \brief Offset: 0xFB4 (R/  32) MTB Lock Status */
  __I  MTB_AUTHSTATUS_Type       AUTHSTATUS;  /**< \brief Offset: 0xFB8 (R/  32) MTB Authentication Status */
  __I  MTB_DEVARCH_Type          DEVARCH;     /**< \brief Offset: 0xFBC (R/  32) MTB Device Architecture */
       RoReg8                    Reserved4[0x8];
  __I  MTB_DEVID_Type            DEVID;       /**< \brief Offset: 0xFC8 (R/  32) MTB Device Configuration */
  __I  MTB_DEVTYPE_Type          DEVTYPE;     /**< \brief Offset: 0xFCC (R/  32) MTB Device Type */
  __I  MTB_PID4_Type             PID4;        /**< \brief Offset: 0xFD0 (R/  32) CoreSight */
  __I  MTB_PID5_Type             PID5;        /**< \brief Offset: 0xFD4 (R/  32) CoreSight */
  __I  MTB_PID6_Type             PID6;        /**< \brief Offset: 0xFD8 (R/  32) CoreSight */
  __I  MTB_PID7_Type             PID7;        /**< \brief Offset: 0xFDC (R/  32) CoreSight */
  __I  MTB_PID0_Type             PID0;        /**< \brief Offset: 0xFE0 (R/  32) CoreSight */
  __I  MTB_PID1_Type             PID1;        /**< \brief Offset: 0xFE4 (R/  32) CoreSight */
  __I  MTB_PID2_Type             PID2;        /**< \brief Offset: 0xFE8 (R/  32) CoreSight */
  __I  MTB_PID3_Type             PID3;        /**< \brief Offset: 0xFEC (R/  32) CoreSight */
  __I  MTB_CID0_Type             CID0;        /**< \brief Offset: 0xFF0 (R/  32) CoreSight */
  __I  MTB_CID1_Type             CID1;        /**< \brief Offset: 0xFF4 (R/  32) CoreSight */
  __I  MTB_CID2_Type             CID2;        /**< \brief Offset: 0xFF8 (R/  32) CoreSight */
  __I  MTB_CID3_Type             CID3;        /**< \brief Offset: 0xFFC (R/  32) CoreSight */
} Mtb;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

/*@}*/

#endif /* _SAMD10_MTB_COMPONENT_ */
