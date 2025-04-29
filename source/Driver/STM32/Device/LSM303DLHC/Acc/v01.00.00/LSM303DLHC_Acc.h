#ifndef __LSM303DLHC_ACC_H__
#define __LSM303DLHC_ACC_H__

#include "Typedef.h"
#include "cI2C.h"

/*
Der Sensor braucht etwa 50ms bei Temp und 20ms bei Hum um einen Wert zu liefern

Bei Hold:
Dann wird von Sensor der I2C für diese Zeit blockiert

Bei NoHold:
Gibt der Sensor NACK zurück, bis der Wert zur Verfügung steht.
Der Wert muss gepollt werden
*/



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

#define LSM303DLHC_ACC_DEFAULT_ACC_ADDRESS   (0x19 << 1)
#define LSM303DLHC_ACC_I2C_ADDRESS_DEFAULT   LSM303DLHC_ACC_DEFAULT_ACC_ADDRESS

/*=========================================================================*/


/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/
/* Acceleration Registers */
#define LSM303DLHC_ACC_CTRL_REG1_A               0x20  /* Control register 1 acceleration */
#define LSM303DLHC_ACC_CTRL_REG2_A               0x21  /* Control register 2 acceleration */
#define LSM303DLHC_ACC_CTRL_REG3_A               0x22  /* Control register 3 acceleration */
#define LSM303DLHC_ACC_CTRL_REG4_A               0x23  /* Control register 4 acceleration */
#define LSM303DLHC_ACC_CTRL_REG5_A               0x24  /* Control register 5 acceleration */
#define LSM303DLHC_ACC_CTRL_REG6_A               0x25  /* Control register 6 acceleration */
#define LSM303DLHC_ACC_REFERENCE_A               0x26  /* Reference register acceleration */
#define LSM303DLHC_ACC_STATUS_REG_A              0x27  /* Status register acceleration */
#define LSM303DLHC_ACC_OUT_X_L_A                 0x28  /* Output Register X acceleration */
#define LSM303DLHC_ACC_OUT_X_H_A                 0x29  /* Output Register X acceleration */
#define LSM303DLHC_ACC_OUT_Y_L_A                 0x2A  /* Output Register Y acceleration */
#define LSM303DLHC_ACC_OUT_Y_H_A                 0x2B  /* Output Register Y acceleration */
#define LSM303DLHC_ACC_OUT_Z_L_A                 0x2C  /* Output Register Z acceleration */
#define LSM303DLHC_ACC_OUT_Z_H_A                 0x2D  /* Output Register Z acceleration */ 
#define LSM303DLHC_ACC_FIFO_CTRL_REG_A           0x2E  /* Fifo control Register acceleration */
#define LSM303DLHC_ACC_FIFO_SRC_REG_A            0x2F  /* Fifo src Register acceleration */

#define LSM303DLHC_ACC_INT1_CFG_A                0x30  /* Interrupt 1 configuration Register acceleration */
#define LSM303DLHC_ACC_INT1_SOURCE_A             0x31  /* Interrupt 1 source Register acceleration */
#define LSM303DLHC_ACC_INT1_THS_A                0x32  /* Interrupt 1 Threshold register acceleration */
#define LSM303DLHC_ACC_INT1_DURATION_A           0x33  /* Interrupt 1 DURATION register acceleration */

#define LSM303DLHC_ACC_INT2_CFG_A                0x34  /* Interrupt 2 configuration Register acceleration */
#define LSM303DLHC_ACC_INT2_SOURCE_A             0x35  /* Interrupt 2 source Register acceleration */
#define LSM303DLHC_ACC_INT2_THS_A                0x36  /* Interrupt 2 Threshold register acceleration */
#define LSM303DLHC_ACC_INT2_DURATION_A           0x37  /* Interrupt 2 DURATION register acceleration */

#define LSM303DLHC_ACC_CLICK_CFG_A               0x38  /* Click configuration Register acceleration */
#define LSM303DLHC_ACC_CLICK_SOURCE_A            0x39  /* Click 2 source Register acceleration */
#define LSM303DLHC_ACC_CLICK_THS_A               0x3A  /* Click 2 Threshold register acceleration */

#define LSM303DLHC_ACC_TIME_LIMIT_A              0x3B  /* Time Limit Register acceleration */
#define LSM303DLHC_ACC_TIME_LATENCY_A            0x3C  /* Time Latency Register acceleration */
#define LSM303DLHC_ACC_TIME_WINDOW_A             0x3D  /* Time window register acceleration */


/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/

#define LSM303DLHC_ACC_NORMAL_MODE            ((uint8_t)0x00)
#define LSM303DLHC_ACC_LOWPOWER_MODE          ((uint8_t)0x08)


#define LSM303DLHC_ACC_ODR_1_HZ                ((uint8_t)0x10)  /*!< Output Data Rate = 1 Hz */
#define LSM303DLHC_ACC_ODR_10_HZ               ((uint8_t)0x20)  /*!< Output Data Rate = 10 Hz */
#define LSM303DLHC_ACC_ODR_25_HZ               ((uint8_t)0x30)  /*!< Output Data Rate = 25 Hz */
#define LSM303DLHC_ACC_ODR_50_HZ               ((uint8_t)0x40)  /*!< Output Data Rate = 50 Hz */
#define LSM303DLHC_ACC_ODR_100_HZ              ((uint8_t)0x50)  /*!< Output Data Rate = 100 Hz */
#define LSM303DLHC_ACC_ODR_200_HZ              ((uint8_t)0x60)  /*!< Output Data Rate = 200 Hz */
#define LSM303DLHC_ACC_ODR_400_HZ              ((uint8_t)0x70)  /*!< Output Data Rate = 400 Hz */
#define LSM303DLHC_ACC_ODR_1620_HZ_LP          ((uint8_t)0x80)  /*!< Output Data Rate = 1620 Hz only in Low Power Mode */
#define LSM303DLHC_ACC_ODR_1344_HZ             ((uint8_t)0x90)  /*!< Output Data Rate = 1344 Hz in Normal mode and 5376 Hz in Low Power Mode */


#define LSM303DLHC_ACC_X_ENABLE                ((uint8_t)0x01)
#define LSM303DLHC_ACC_Y_ENABLE                ((uint8_t)0x02)
#define LSM303DLHC_ACC_Z_ENABLE                ((uint8_t)0x04)
#define LSM303DLHC_ACC_AXES_ENABLE             ((uint8_t)0x07)
#define LSM303DLHC_ACC_AXES_DISABLE            ((uint8_t)0x00)


#define LSM303DLHC_ACC_HR_ENABLE               ((uint8_t)0x08)
#define LSM303DLHC_ACC_HR_DISABLE              ((uint8_t)0x00)


#define LSM303DLHC_ACC_FULLSCALE_2G            ((uint8_t)0x00)  /*!< ±2 g */
#define LSM303DLHC_ACC_FULLSCALE_4G            ((uint8_t)0x10)  /*!< ±4 g */
#define LSM303DLHC_ACC_FULLSCALE_8G            ((uint8_t)0x20)  /*!< ±8 g */
#define LSM303DLHC_ACC_FULLSCALE_16G           ((uint8_t)0x30)  /*!< ±16 g */


#define LSM303DLHC_ACC_BlockUpdate_Continous   ((uint8_t)0x00) /*!< Continuos Update */
#define LSM303DLHC_ACC_BlockUpdate_Single      ((uint8_t)0x80) /*!< Single Update: output registers not updated until MSB and LSB reading */


#define LSM303DLHC_ACC_BLE_LSB                 ((uint8_t)0x00) /*!< Little Endian: data LSB @ lower address */
#define LSM303DLHC_ACC_BLE_MSB	               ((uint8_t)0x40) /*!< Big Endian: data MSB @ lower address */


#define LSM303DLHC_ACC_BOOT_NORMALMODE         ((uint8_t)0x00)
#define LSM303DLHC_ACC_BOOT_REBOOTMEMORY       ((uint8_t)0x80)


#define LSM303DLHC_ACC_HPM_NORMAL_MODE_RES     ((uint8_t)0x00)
#define LSM303DLHC_ACC_HPM_REF_SIGNAL          ((uint8_t)0x40)
#define LSM303DLHC_ACC_HPM_NORMAL_MODE         ((uint8_t)0x80)
#define LSM303DLHC_ACC_HPM_AUTORESET_INT       ((uint8_t)0xC0)


#define LSM303DLHC_ACC_HPFCF_8                 ((uint8_t)0x00)
#define LSM303DLHC_ACC_HPFCF_16                ((uint8_t)0x10)
#define LSM303DLHC_ACC_HPFCF_32                ((uint8_t)0x20)
#define LSM303DLHC_ACC_HPFCF_64                ((uint8_t)0x30)


#define LSM303DLHC_ACC_HIGHPASSFILTER_DISABLE  ((uint8_t)0x00)
#define LSM303DLHC_ACC_HIGHPASSFILTER_ENABLE   ((uint8_t)0x08)


#define LSM303DLHC_ACC_HPF_CLICK_DISABLE       ((uint8_t)0x00)
#define LSM303DLHC_ACC_HPF_CLICK_ENABLE	       ((uint8_t)0x04)


#define LSM303DLHC_ACC_HPF_AOI1_DISABLE        ((uint8_t)0x00)
#define LSM303DLHC_ACC_HPF_AOI1_ENABLE         ((uint8_t)0x01)


#define LSM303DLHC_ACC_HPF_AOI2_DISABLE        ((uint8_t)0x00)
#define LSM303DLHC_ACC_HPF_AOI2_ENABLE         ((uint8_t)0x02)


#define LSM303DLHC_ACC_IT1_CLICK               ((uint8_t)0x80)
#define LSM303DLHC_ACC_IT1_AOI1                ((uint8_t)0x40)
#define LSM303DLHC_ACC_IT1_AOI2                ((uint8_t)0x20)
#define LSM303DLHC_ACC_IT1_DRY1                ((uint8_t)0x10)
#define LSM303DLHC_ACC_IT1_DRY2                ((uint8_t)0x08)
#define LSM303DLHC_ACC_IT1_WTM                 ((uint8_t)0x04)
#define LSM303DLHC_ACC_IT1_OVERRUN             ((uint8_t)0x02)


#define LSM303DLHC_ACC_IT2_CLICK               ((uint8_t)0x80)
#define LSM303DLHC_ACC_IT2_INT1                ((uint8_t)0x40)
#define LSM303DLHC_ACC_IT2_INT2                ((uint8_t)0x20)
#define LSM303DLHC_ACC_IT2_BOOT                ((uint8_t)0x10)
#define LSM303DLHC_ACC_IT2_ACT                 ((uint8_t)0x08)
#define LSM303DLHC_ACC_IT2_HLACTIVE            ((uint8_t)0x02)


#define LSM303DLHC_ACC_OR_COMBINATION          ((uint8_t)0x00)  /*!< OR combination of enabled IRQs */
#define LSM303DLHC_ACC_AND_COMBINATION	       ((uint8_t)0x80)  /*!< AND combination of enabled IRQs */
#define LSM303DLHC_ACC_MOV_RECOGNITION         ((uint8_t)0x40)  /*!< 6D movement recognition */
#define LSM303DLHC_ACC_POS_RECOGNITION	       ((uint8_t)0xC0)  /*!< 6D position recognition */


#define LSM303DLHC_ACC_Z_HIGH                  ((uint8_t)0x20)  /*!< Z High enabled IRQs */
#define LSM303DLHC_ACC_Z_LOW	                 ((uint8_t)0x10)  /*!< Z low enabled IRQs */
#define LSM303DLHC_ACC_Y_HIGH                  ((uint8_t)0x08)  /*!< Y High enabled IRQs */
#define LSM303DLHC_ACC_Y_LOW	                 ((uint8_t)0x04)  /*!< Y low enabled IRQs */
#define LSM303DLHC_ACC_X_HIGH                  ((uint8_t)0x02)  /*!< X High enabled IRQs */
#define LSM303DLHC_ACC_X_LOW	                 ((uint8_t)0x01)  /*!< X low enabled IRQs */


#define LSM303DLHC_ACC_Z_DOUBLE_CLICK          ((uint8_t)0x20)  /*!< Z double click IRQs */
#define LSM303DLHC_ACC_Z_SINGLE_CLICK	         ((uint8_t)0x10)  /*!< Z single click IRQs */
#define LSM303DLHC_ACC_Y_DOUBLE_CLICK          ((uint8_t)0x08)  /*!< Y double click IRQs */
#define LSM303DLHC_ACC_Y_SINGLE_CLICK	         ((uint8_t)0x04)  /*!< Y single click IRQs */
#define LSM303DLHC_ACC_X_DOUBLE_CLICK          ((uint8_t)0x02)  /*!< X double click IRQs */
#define LSM303DLHC_ACC_X_SINGLE_CLICK	         ((uint8_t)0x01)  /*!< X single click IRQs */


#define LSM303DLHC_ACC_INT1INTERRUPT_DISABLE   ((uint8_t)0x00)
#define LSM303DLHC_ACC_INT1INTERRUPT_ENABLE	   ((uint8_t)0x80)

 
#define LSM303DLHC_ACC_INT1INTERRUPT_LOW_EDGE  ((uint8_t)0x20)
#define LSM303DLHC_ACC_INT1INTERRUPT_HIGH_EDGE ((uint8_t)0x00)

class cLSM303DLHC_ACC: public cComNode
{
  public:

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdReadAll,
    enCmdReadAll2,
    enCmdWriteControl1,
    enCmdWriteControl2,
    enCmdReadAcc,
    enCmdReadAcc2,
    /*enCmdReadTemperatureNoHold,
    enCmdReadTemperatureNoHold2,
    enCmdReadTemperatureNoHold3,
    enCmdReadHumidityHold,
    enCmdReadHumidityHold2,
    enCmdReadHumidityNoHold,
    enCmdReadHumidityNoHold2,
    enCmdReadHumidityNoHold3*/
  }tenLSM303DLHC_ACC_Cmd;

  uint8 u8RamMirrow[LSM303DLHC_ACC_TIME_WINDOW_A - LSM303DLHC_ACC_CTRL_REG1_A];

  cI2cMaster*   mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenLSM303DLHC_ACC_Cmd   menCmd;
  uint8                   mIdx;

  bool mbReadAll;
  bool mbWriteControl;
  bool mbReadAcc;
  bool mbReadTemperature;

  cLSM303DLHC_ACC(cI2cMaster* lpcI2C);
  ~cLSM303DLHC_ACC();

  inline uint8 uReg2Idx(uint8 lu8Reg)
  {
    if (lu8Reg >= LSM303DLHC_ACC_CTRL_REG1_A) return (lu8Reg - LSM303DLHC_ACC_CTRL_REG1_A);
    return 0;
  }

  void  vSetup();

  void vComStart(cComNode::tenEvent lenEvent) override;
  void vComDone() override;
  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override;

  void vSetControl(uint8 lui8Reg1, uint8 lui8Reg2, uint8 lui8Reg3, 
                   uint8 lui8Reg4, uint8 lui8Reg5, uint8 lui8Reg6);

  void  vTick10ms(void);

  void  vReadAcc();


  int16 i16GetAccX()
  {
    int16 li16Temp;
    li16Temp = (u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_OUT_X_H_A)] << 8) + u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_OUT_X_L_A)];
    return li16Temp;
  }

  int16 i16GetAccY()
  {
    int16 li16Temp;
    li16Temp = (u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_OUT_Y_H_A)] << 8) + u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_OUT_Y_L_A)];
    return li16Temp;
  }

  int16 i16GetAccZ()
  {
    int16 li16Temp;
    li16Temp = (u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_OUT_Z_H_A)] << 8) + u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_OUT_Z_L_A)];
    return li16Temp;
  }

};

#endif // __INA291_H__
