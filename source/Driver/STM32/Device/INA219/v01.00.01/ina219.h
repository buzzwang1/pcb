#ifndef __INA291_H__
#define __INA291_H__

#include "Typedef.h"
#include "cI2C.h"



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define INA219_I2C_ADDRESS_CONF_0       (0x40 << 1)     // A0 = GND, A1 = GND
  #define INA219_I2C_ADDRESS_CONF_1       (0x41 << 1)     // A0 = VS+, A1 = GND
  #define INA219_I2C_ADDRESS_CONF_2       (0x42 << 1)     // A0 = SDA, A1 = GND
  #define INA219_I2C_ADDRESS_CONF_3       (0x43 << 1)     // A0 = SCL, A1 = GND
  #define INA219_I2C_ADDRESS_CONF_4       (0x44 << 1)     // A0 = GND, A1 = VS+
  #define INA219_I2C_ADDRESS_CONF_5       (0x45 << 1)     // A0 = VS+, A1 = VS+
  #define INA219_I2C_ADDRESS_CONF_6       (0x46 << 1)     // A0 = SDA, A1 = VS+
  #define INA219_I2C_ADDRESS_CONF_7       (0x47 << 1)     // A0 = SCL, A1 = VS+
  #define INA219_I2C_ADDRESS_CONF_8       (0x48 << 1)     // A0 = GND, A1 = SDA
  #define INA219_I2C_ADDRESS_CONF_9       (0x49 << 1)     // A0 = VS+, A1 = SDA
  #define INA219_I2C_ADDRESS_CONF_A       (0x4A << 1)     // A0 = SDA, A1 = SDA
  #define INA219_I2C_ADDRESS_CONF_B       (0x4B << 1)     // A0 = SCL, A1 = SDA
  #define INA219_I2C_ADDRESS_CONF_C       (0x4C << 1)     // A0 = GND, A1 = SCL
  #define INA219_I2C_ADDRESS_CONF_D       (0x4D << 1)     // A0 = VS+, A1 = SCL
  #define INA219_I2C_ADDRESS_CONF_E       (0x4E << 1)     // A0 = SDA, A1 = SCL
  #define INA219_I2C_ADDRESS_CONF_F       (0x4F << 1)     // A0 = SCL, A1 = SCL
  #define INA219_I2C_ADDRESS_DEFAULT      INA219_I2C_ADDRESS_CONF_0
/*=========================================================================*/


/*=========================================================================
  CONFIG REGISTER (R/W)
  -----------------------------------------------------------------------*/
  #define INA219_REG_CONFIG                      (0x00)

  typedef enum
  {
    INA219_BusVoltageRange_16V = 0,                   // '0'=16V
    INA219_BusVoltageRange_32V = 1                    // '1'=32V (default value)
  }INA219BusVoltageRange_TypeDef;                     // Bus voltage range (Bit 13),

  typedef enum
  {
    INA219_GainRange_1_40mV  = 0,                     // Gain 1, 40mV Range
    INA219_GainRange_2_80mV  = 1,                     // Gain 2, 80mV Range
    INA219_GainRange_4_160mV = 2,                     // Gain 4, 160mV Range
    INA219_GainRange_8_320mV = 3                      // Gain 8, 320mV Range (default)
  }INA219GainRange_TypeDef;

  typedef enum
  {
    INA219_BusADCResolution_9Bit   = 0,               // 9-bit bus res = 0..511
    INA219_BusADCResolution_10Bit  = 1,               // 10-bit bus res = 0..1023
    INA219_BusADCResolution_11Bit  = 2,               // 11-bit bus res = 0..2047
    INA219_BusADCResolution_12Bit  = 3                // 12-bit bus res = 0..4097
  }INA219BusADCResolution_TypeDef;                    // Bus ADC Resolution / Averaging

  typedef enum
  {
    INA219_ShuntADCResolution_9Bit_1S_84uS     =  0,  // 1 x 9-bit shunt sample
    INA219_ShuntADCResolution_10Bit_1S_148uS   =  1,  // 1 x 10-bit shunt sample
    INA219_ShuntADCResolution_11Bit_1S_276uS   =  2,  // 1 x 11-bit shunt sample
    INA219_ShuntADCResolution_12Bit_1S_532uS   =  3,  // 1 x 12-bit shunt sample, (DEFAULT)
    INA219_ShuntADCResolution_12Bit_2S_1060uS  =  9,  // 2 x 12-bit shunt samples averaged together
    INA219_ShuntADCResolution_12Bit_4S_2130uS  = 10,  // 4 x 12-bit shunt samples averaged together
    INA219_ShuntADCResolution_12Bit_8S_4260uS  = 11,  // 8 x 12-bit shunt samples averaged together
    INA219_ShuntADCResolution_12Bit_16S_8510uS = 12,  // 16 x 12-bit shunt samples averaged together
    INA219_ShuntADCResolution_12Bit_32S_17mS   = 13,  // 32 x 12-bit shunt samples averaged together
    INA219_ShuntADCResolution_12Bit_64S_34mS   = 14,  // 64 x 12-bit shunt samples averaged together
    INA219_ShuntADCResolution_12Bit_128S_69mS  = 15   // 128 x 12-bit shunt samples averaged together
  }INA219ShuntADCResolution_TypeDef;                  // Shunt ADC Resolution/Averaging

  typedef enum
  {
    INA219_Mode_PowerDown                    = 0,
    INA219_Mode_ShuntVoltageTriggered        = 1,
    INA219_Mode_BusVoltageTriggered          = 2,
    INA219_Mode_ShuntAndBusVoltageTriggered  = 3,
    INA219_Mode_ADCOff                       = 4,
    INA219_Mode_ShuntVoltageContinuous       = 5,
    INA219_Mode_BusBoltageContinuous         = 6,
    INA219_Mode_ShuntAndBusVoltageContinuous = 7     // Default
  }INA219Mode_TypeDef;


  #define INA219_CONFIG_DEFAULT                   (0x399F)
/*=========================================================================*/

/*=========================================================================
  SHUNT VOLTAGE REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA219_REG_SHUNTVOLTAGE                (0x01)
/*=========================================================================*/

/*=========================================================================
  BUS VOLTAGE REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA219_REG_BUSVOLTAGE                  (0x02)
/*=========================================================================*/

/*=========================================================================
  POWER REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA219_REG_POWER                       (0x03)
/*=========================================================================*/

/*=========================================================================
  CURRENT REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA219_REG_CURRENT                     (0x04)
/*=========================================================================*/

/*=========================================================================
  CALIBRATION REGISTER (R/W)
  -------------------------------------------------------------------------*/
  #define INA219_REG_CALIBRATION                 (0x05)
/*=========================================================================*/

  #define INA219_I2C_TIMEOUT           ((uint32_t)0x3FFFF) /*!< I2C Time out */
  #define INA219_FLAG_TIMEOUT         ((uint32_t)0x1000)
  #define INA219_LONG_TIMEOUT         ((uint32_t)(10 * INA219_FLAG_TIMEOUT))

class cINA219:public cComNode
{
  public:

  struct tstRegConf
  {
    uint16 ui16Mode:3;
    uint16 ui16SADC:4;
    uint16 ui16BADC:4;
    uint16 ui16PG:2;
    uint16 ui16BRNG:1;
    uint16 ui16NotUsed1:1;
    uint16 ui16RST:1;
  };

  union tunRegConf
  {
    uint16      ui16Value;
    tstRegConf  stBits;
  };

  struct tstRegBusVoltage
  {
    uint16 ui16OVF:1;
    uint16 ui16CNVR:1;
    uint16 ui16NotUsed1:1;
    uint16 ui16BD:13;
  };

  union tunRegBusVoltage
  {
    uint16           ui16Value;
    tstRegBusVoltage stBits;
  };

  struct tstRegShuntVoltage8
  {
    uint16 ui16SD:15;
    uint16 ui16Sign:1;
  };
  struct tstRegShuntVoltage4
  {
    uint16 ui16SD:14;
    uint16 ui16Sign:1;
  };
  struct tstRegShuntVoltage2
  {
    uint16 ui16SD:13;
    uint16 ui16Sign:1;
  };
  struct tstRegShuntVoltage1
  {
    uint16 ui16SD:12;
    uint16 ui16Sign:1;
  };

  union tunRegShuntVoltage
  {
    uint16              ui16Value;
    tstRegShuntVoltage8 stBits8;
    tstRegShuntVoltage4 stBits4;
    tstRegShuntVoltage2 stBits2;
    tstRegShuntVoltage1 stBits1;
  };


  struct tstRegCurrent
  {
    uint16 ui16CD:15;
    uint16 ui16Sign:1;
  };

  union tunRegCurrent
  {
    uint16        ui16Value;
    tstRegCurrent stBits;
  };


  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdReadConfig,
    enCmdReadConfig2,
    enCmdWriteConfig,
    enCmdWriteConfig2,
    enCmdReadIShunt,
    enCmdReadIShunt2,
    enCmdReadVShunt,
    enCmdReadVShunt2,
    enCmdReadVBus,
    enCmdReadVBus2
  }tenIna219_Cmd;

  tunRegConf         mRegConf;
  tunRegShuntVoltage mRegVShunt;
  tunRegBusVoltage   mRegVBus;
  tunRegCurrent      mRegIShunt;
  uint16             mRegPwr;
  uint16             mRegCal;
  uint16             mShunt; // [mOhm]

  cI2cMaster*        mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenIna219_Cmd      menCmd;
  uint8              mIdx;

  bool mbInit;
  bool mbReadConfig;
  bool mbWriteConfig;
  bool mbReadIShunt;
  bool mbReadVShunt;
  bool mbReadVBus;



  cINA219(cI2cMaster* lpcI2C, uint8 lui8Adr, uint16 lui16Shunt);
  ~cINA219();

  int8  i8Setup(uint8 lui8BRng, uint8 lui8PGA, uint8 lui8BAdc, uint8 lui8SAdc, uint8 lui8Mode);
  int8  i8Setup();

  uint32 SizeOf() {return 0;};
  void  vComStart(cComNode::tenEvent lenEvent);
  void  vComDone();

  int8  i8ReadConfig_digit();
  int8  i8PerformCalibration_digit();
  int8  i8ReadVBus_digit();
  int8  i8ReadVShunt_digit();
  int8  i8ReadIShunt_digit();

  int32 i32CalcIShunt_uA();
  int32 i32CalcVShunt_uV();
  int32 i32CalcVBus_uV();
};

#endif // __INA291_H__
