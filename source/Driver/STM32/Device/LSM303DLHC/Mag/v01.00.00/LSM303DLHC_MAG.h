#ifndef __LSM303DLHC_MAG_H__
#define __LSM303DLHC_MAG_H__

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

#define LSM303DLHC_MAG_DEFAULT_MAG_ADDRESS   (0x1E << 1)
#define LSM303DLHC_MAG_I2C_ADDRESS_DEFAULT   LSM303DLHC_MAG_DEFAULT_MAG_ADDRESS

/*=========================================================================*/

/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/

/* Magnetic field Registers */
#define LSM303DLHC_MAG_CRA_REG_M                 0x00  /* Control register A magnetic field */
#define LSM303DLHC_MAG_CRB_REG_M                 0x01  /* Control register B magnetic field */
#define LSM303DLHC_MAG_MR_REG_M                  0x02  /* Control register MR magnetic field */
#define LSM303DLHC_MAG_OUT_X_H_M                 0x03  /* Output Register X magnetic field */
#define LSM303DLHC_MAG_OUT_X_L_M                 0x04  /* Output Register X magnetic field */
#define LSM303DLHC_MAG_OUT_Z_H_M                 0x05  /* Output Register Z magnetic field */
#define LSM303DLHC_MAG_OUT_Z_L_M                 0x06  /* Output Register Z magnetic field */
#define LSM303DLHC_MAG_OUT_Y_H_M                 0x07  /* Output Register Y magnetic field */
#define LSM303DLHC_MAG_OUT_Y_L_M                 0x08  /* Output Register Y magnetic field */

#define LSM303DLHC_MAG_SR_REG_M                  0x09  /* Status Register magnetic field */
#define LSM303DLHC_MAG_IRA_REG_M                 0x0A  /* IRA Register magnetic field */
#define LSM303DLHC_MAG_IRB_REG_M                 0x0B  /* IRB Register magnetic field */
#define LSM303DLHC_MAG_IRC_REG_M                 0x0C  /* IRC Register magnetic field */

#define LSM303DLHC_MAG_TEMP_OUT_H_M              0x31  /* Temperature Register magnetic field */
#define LSM303DLHC_MAG_TEMP_OUT_L_M              0x32  /* Temperature Register magnetic field */


/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/

// RA_REG_M         (00h)
#define LSM303DLHC_MAG_TEMPSENSOR_ENABLE         ((uint8_t) 0x80)   /*!< Temp sensor Enable */
#define LSM303DLHC_MAG_TEMPSENSOR_DISABLE        ((uint8_t) 0x00)   /*!< Temp sensor Disable */

#define LSM303DLHC_MAG_ODR_0_75_HZ              ((uint8_t) 0x00)  /*!< Output Data Rate = 0.75 Hz */
#define LSM303DLHC_MAG_ODR_1_5_HZ               ((uint8_t) 0x04)  /*!< Output Data Rate = 1.5 Hz */
#define LSM303DLHC_MAG_ODR_3_0_HZ               ((uint8_t) 0x08)  /*!< Output Data Rate = 3 Hz */
#define LSM303DLHC_MAG_ODR_7_5_HZ               ((uint8_t) 0x0C)  /*!< Output Data Rate = 7.5 Hz */
#define LSM303DLHC_MAG_ODR_15_HZ                ((uint8_t) 0x10)  /*!< Output Data Rate = 15 Hz */
#define LSM303DLHC_MAG_ODR_30_HZ                ((uint8_t) 0x14)  /*!< Output Data Rate = 30 Hz */
#define LSM303DLHC_MAG_ODR_75_HZ                ((uint8_t) 0x18)  /*!< Output Data Rate = 75 Hz */
#define LSM303DLHC_MAG_ODR_220_HZ               ((uint8_t) 0x1C)  /*!< Output Data Rate = 220 Hz */


// RB_REG_M         (01h)
#define  LSM303DLHC_MAG_FS_1_3_GA               ((uint8_t) 0x20)  /*!< Full scale = ±1.3 Gauss */
#define  LSM303DLHC_MAG_FS_1_9_GA               ((uint8_t) 0x40)  /*!< Full scale = ±1.9 Gauss */
#define  LSM303DLHC_MAG_FS_2_5_GA               ((uint8_t) 0x60)  /*!< Full scale = ±2.5 Gauss */
#define  LSM303DLHC_MAG_FS_4_0_GA               ((uint8_t) 0x80)  /*!< Full scale = ±4.0 Gauss */
#define  LSM303DLHC_MAG_FS_4_7_GA               ((uint8_t) 0xA0)  /*!< Full scale = ±4.7 Gauss */
#define  LSM303DLHC_MAG_FS_5_6_GA               ((uint8_t) 0xC0)  /*!< Full scale = ±5.6 Gauss */
#define  LSM303DLHC_MAG_FS_8_1_GA               ((uint8_t) 0xE0)  /*!< Full scale = ±8.1 Gauss */

#define LSM303DLHC_MAG_M_SENSITIVITY_XY_1_3Ga     1100  /*!< magnetometer X Y axes sensitivity for 1.3 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_XY_1_9Ga     855   /*!< magnetometer X Y axes sensitivity for 1.9 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_XY_2_5Ga     670   /*!< magnetometer X Y axes sensitivity for 2.5 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_XY_4Ga       450   /*!< magnetometer X Y axes sensitivity for 4 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_XY_4_7Ga     400   /*!< magnetometer X Y axes sensitivity for 4.7 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_XY_5_6Ga     330   /*!< magnetometer X Y axes sensitivity for 5.6 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_XY_8_1Ga     230   /*!< magnetometer X Y axes sensitivity for 8.1 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_Z_1_3Ga      980   /*!< magnetometer Z axis sensitivity for 1.3 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_Z_1_9Ga      760   /*!< magnetometer Z axis sensitivity for 1.9 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_Z_2_5Ga      600   /*!< magnetometer Z axis sensitivity for 2.5 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_Z_4Ga        400   /*!< magnetometer Z axis sensitivity for 4 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_Z_4_7Ga      355   /*!< magnetometer Z axis sensitivity for 4.7 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_Z_5_6Ga      295   /*!< magnetometer Z axis sensitivity for 5.6 Ga full scale [LSB/Ga] */
#define LSM303DLHC_MAG_M_SENSITIVITY_Z_8_1Ga      205   /*!< magnetometer Z axis sensitivity for 8.1 Ga full scale [LSB/Ga] */


// R_REG_M         (02h)
#define LSM303DLHC_MAG_CONTINUOS_CONVERSION      ((uint8_t) 0x00)   /*!< Continuous-Conversion Mode */
#define LSM303DLHC_MAG_SINGLE_CONVERSION         ((uint8_t) 0x01)   /*!< Single-Conversion Mode */
#define LSM303DLHC_MAG_SLEEP                     ((uint8_t) 0x02)   /*!< Sleep Mode */



class cLSM303DLHC_MAG: public cComNode
{
  public:

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdReadAll,
    enCmdReadAll2,
    enCmdWriteConfig1,
    enCmdWriteConfig2,
    enCmdWriteConfig3,
    enCmdWriteConfig4,
    enCmdReadMag,
    enCmdReadMag2,
    enCmdReadTemperature,
    enCmdReadTemperature2,
    /*enCmdReadTemperatureNoHold,
    enCmdReadTemperatureNoHold2,
    enCmdReadTemperatureNoHold3,
    enCmdReadHumidityHold,
    enCmdReadHumidityHold2,
    enCmdReadHumidityNoHold,
    enCmdReadHumidityNoHold2,
    enCmdReadHumidityNoHold3*/
  }tenLSM303DLHC_MAG_Cmd;

  uint8 u8RamMirrow[13 + 2];

  cI2cMaster*   mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenLSM303DLHC_MAG_Cmd   menCmd;
  uint8                   mIdx;

  bool mbReadAll;
  bool mbWriteConfig;
  bool mbReadMag;
  bool mbReadTemperature;

  cLSM303DLHC_MAG(cI2cMaster* lpcI2C);
  ~cLSM303DLHC_MAG();

  inline uint8 uReg2Idx(uint8 lu8Reg)
  {
    if (lu8Reg <= LSM303DLHC_MAG_IRC_REG_M)    return lu8Reg;
    if (lu8Reg == LSM303DLHC_MAG_TEMP_OUT_H_M) return (LSM303DLHC_MAG_TEMP_OUT_H_M - LSM303DLHC_MAG_IRC_REG_M);
    if (lu8Reg == LSM303DLHC_MAG_TEMP_OUT_L_M) return (LSM303DLHC_MAG_TEMP_OUT_L_M - LSM303DLHC_MAG_IRC_REG_M);
    return 0;
  }

  void  vSetup();

  void vComStart(cComNode::tenEvent lenEvent) override;
  void vComDone() override;
  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override;

  void vSetConfig(uint8 lui8TempEn, uint8 lui8DataOutputRate, uint8 lui8Gain, uint8 lui8Mode);

  void  vTick10ms(void);

  void  vReadMag();
  void  vReadTemperature();

  int16 i16GetMagX()
  {
    int16 li16Temp;
    li16Temp = (u8RamMirrow[LSM303DLHC_MAG_OUT_X_H_M] << 8) + u8RamMirrow[LSM303DLHC_MAG_OUT_X_H_M + 1];
    return li16Temp;
  }

  int16 i16GetMagY()
  {
    int16 li16Temp;
    li16Temp = (u8RamMirrow[LSM303DLHC_MAG_OUT_Y_H_M] << 8) + u8RamMirrow[LSM303DLHC_MAG_OUT_Y_H_M + 1];
    return li16Temp;
  }

  int16 i16GetMagZ()
  {
    int16 li16Temp;
    li16Temp = (u8RamMirrow[LSM303DLHC_MAG_OUT_Z_H_M] << 8) + u8RamMirrow[LSM303DLHC_MAG_OUT_Z_H_M + 1];
    return li16Temp;
  }

  int16 i16GetTemperature()
  {
    int16 li16Temp;

    li16Temp = (u8RamMirrow[LSM303DLHC_MAG_TEMP_OUT_H_M] << 8) + u8RamMirrow[LSM303DLHC_MAG_TEMP_OUT_H_M + 1];
    return li16Temp / 16;
  }

};

#endif // __INA291_H__
