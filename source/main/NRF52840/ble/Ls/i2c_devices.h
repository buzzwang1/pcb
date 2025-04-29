#ifndef __I2C_DEVICES_H__
#define __I2C_DEVICES_H__


#ifdef __cplusplus
extern "C" {
#endif

  /* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Common addresses definition for temperature sensor. */
#define TMP102_ADDR          (0x90U >> 1)

#define TMP102_REG_TEMP      0x00U
#define TMP102_REG_CONF      0x01U
#define TMP102_REG_THYST     0x02U
#define TMP102_REG_TOS       0x03U

/* Mode for TMP102. */
#define NORMAL_MODE 0U

#define LSM303DLHC_ADDRESS_0       (0x32 >> 1)
#define LSM303DLHC_ADDRESS_DEFAULT LSM303DLHC_ADDRESS_0

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




#define L3GD20_ADDRESS_0       (0xD2 >> 1)
#define L3GD20_ADDRESS_DEFAULT L3GD20_ADDRESS_0



/******************************************************************************/
/*************************** START REGISTER MAPPING  **************************/
/******************************************************************************/
#define L3GD20_WHO_AM_I_ADDR          0x0F  /* device identification register */
#define L3GD20_CTRL_REG1_ADDR         0x20  /* Control register 1 */
#define L3GD20_CTRL_REG2_ADDR         0x21  /* Control register 2 */
#define L3GD20_CTRL_REG3_ADDR         0x22  /* Control register 3 */
#define L3GD20_CTRL_REG4_ADDR         0x23  /* Control register 4 */
#define L3GD20_CTRL_REG5_ADDR         0x24  /* Control register 5 */
#define L3GD20_REFERENCE_REG_ADDR     0x25  /* Reference register */
#define L3GD20_OUT_TEMP_ADDR          0x26  /* Out temp register */
#define L3GD20_STATUS_REG_ADDR        0x27  /* Status register */
#define L3GD20_OUT_X_L_ADDR           0x28  /* Output Register X */
#define L3GD20_OUT_X_H_ADDR           0x29  /* Output Register X */
#define L3GD20_OUT_Y_L_ADDR           0x2A  /* Output Register Y */
#define L3GD20_OUT_Y_H_ADDR           0x2B  /* Output Register Y */
#define L3GD20_OUT_Z_L_ADDR           0x2C  /* Output Register Z */
#define L3GD20_OUT_Z_H_ADDR           0x2D  /* Output Register Z */ 
#define L3GD20_FIFO_CTRL_REG_ADDR     0x2E  /* Fifo control Register */
#define L3GD20_FIFO_SRC_REG_ADDR      0x2F  /* Fifo src Register */

#define L3GD20_INT1_CFG_ADDR          0x30  /* Interrupt 1 configuration Register */
#define L3GD20_INT1_SRC_ADDR          0x31  /* Interrupt 1 source Register */
#define L3GD20_INT1_TSH_XH_ADDR       0x32  /* Interrupt 1 Threshold X register */
#define L3GD20_INT1_TSH_XL_ADDR       0x33  /* Interrupt 1 Threshold X register */
#define L3GD20_INT1_TSH_YH_ADDR       0x34  /* Interrupt 1 Threshold Y register */
#define L3GD20_INT1_TSH_YL_ADDR       0x35  /* Interrupt 1 Threshold Y register */
#define L3GD20_INT1_TSH_ZH_ADDR       0x36  /* Interrupt 1 Threshold Z register */
#define L3GD20_INT1_TSH_ZL_ADDR       0x37  /* Interrupt 1 Threshold Z register */
#define L3GD20_INT1_DURATION_ADDR     0x38  /* Interrupt 1 DURATION register */

/******************************************************************************/
/**************************** END REGISTER MAPPING  ***************************/
/******************************************************************************/

#define I_AM_L3GD20		    ((uint8_t)0xD4)

#define L3GD20_MODE_POWERDOWN       ((uint8_t)0x00)
#define L3GD20_MODE_ACTIVE          ((uint8_t)0x08)

#define L3GD20_OUTPUT_DATARATE_1    ((uint8_t)0x00)
#define L3GD20_OUTPUT_DATARATE_2    ((uint8_t)0x40)
#define L3GD20_OUTPUT_DATARATE_3    ((uint8_t)0x80)
#define L3GD20_OUTPUT_DATARATE_4    ((uint8_t)0xC0)

#define L3GD20_X_ENABLE            ((uint8_t)0x02)
#define L3GD20_Y_ENABLE            ((uint8_t)0x01)
#define L3GD20_Z_ENABLE            ((uint8_t)0x04)
#define L3GD20_AXES_ENABLE         ((uint8_t)0x07)
#define L3GD20_AXES_DISABLE        ((uint8_t)0x00)

#define L3GD20_BANDWIDTH_1         ((uint8_t)0x00)
#define L3GD20_BANDWIDTH_2         ((uint8_t)0x10)
#define L3GD20_BANDWIDTH_3         ((uint8_t)0x20)
#define L3GD20_BANDWIDTH_4         ((uint8_t)0x30)

#define L3GD20_FULLSCALE_250               ((uint8_t)0x00)
#define L3GD20_FULLSCALE_500               ((uint8_t)0x10)
#define L3GD20_FULLSCALE_2000              ((uint8_t)0x20) 

#define L3GD20_BlockDataUpdate_Continous   ((uint8_t)0x00)
#define L3GD20_BlockDataUpdate_Single      ((uint8_t)0x80)

#define L3GD20_BLE_LSB                     ((uint8_t)0x00)
#define L3GD20_BLE_MSB	                   ((uint8_t)0x40)

#define L3GD20_HIGHPASSFILTER_DISABLE      ((uint8_t)0x00)
#define L3GD20_HIGHPASSFILTER_ENABLE	     ((uint8_t)0x10)

#define L3GD20_INT1INTERRUPT_DISABLE       ((uint8_t)0x00)
#define L3GD20_INT1INTERRUPT_ENABLE	   ((uint8_t)0x80)

#define L3GD20_INT2INTERRUPT_DISABLE       ((uint8_t)0x00)
#define L3GD20_INT2INTERRUPT_ENABLE	   ((uint8_t)0x08)

#define L3GD20_INT1INTERRUPT_LOW_EDGE      ((uint8_t)0x20)
#define L3GD20_INT1INTERRUPT_HIGH_EDGE     ((uint8_t)0x00)

#define L3GD20_BOOT_NORMALMODE             ((uint8_t)0x00)
#define L3GD20_BOOT_REBOOTMEMORY           ((uint8_t)0x80)


#define L3GD20_HPM_NORMAL_MODE_RES         ((uint8_t)0x00)
#define L3GD20_HPM_REF_SIGNAL              ((uint8_t)0x10)
#define L3GD20_HPM_NORMAL_MODE             ((uint8_t)0x20)
#define L3GD20_HPM_AUTORESET_INT           ((uint8_t)0x30)


#define L3GD20_HPFCF_0              0x00
#define L3GD20_HPFCF_1              0x01
#define L3GD20_HPFCF_2              0x02
#define L3GD20_HPFCF_3              0x03
#define L3GD20_HPFCF_4              0x04
#define L3GD20_HPFCF_5              0x05
#define L3GD20_HPFCF_6              0x06
#define L3GD20_HPFCF_7              0x07
#define L3GD20_HPFCF_8              0x08
#define L3GD20_HPFCF_9              0x09



#ifdef __cplusplus
}
#endif

#endif  //__I2C_DEVICES_H__
