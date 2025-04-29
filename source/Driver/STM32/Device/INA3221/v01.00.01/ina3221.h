#ifndef __INA3221_H__
#define __INA3221_H__

#include "Typedef.h"
#include "cI2C.h"



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define INA3221_I2C_ADDRESS_CONF_0       (0x40 << 1)     // A0 = GND, A1 = GND
  #define INA3221_I2C_ADDRESS_CONF_1       (0x41 << 1)     // A0 = VS+, A1 = GND
  #define INA3221_I2C_ADDRESS_CONF_2       (0x42 << 1)     // A0 = SDA, A1 = GND
  #define INA3221_I2C_ADDRESS_CONF_3       (0x43 << 1)     // A0 = SCL, A1 = GND
  #define INA3221_I2C_ADDRESS_DEFAULT      INA3221_I2C_ADDRESS_CONF_0
/*=========================================================================*/


/*=========================================================================
  CONFIG REGISTER (R/W)
  -----------------------------------------------------------------------*/
  #define INA3221_REG_CONFIG                      (0x00)

  typedef enum
  {
    INA3221_AveragingMode1    = 0, // default
    INA3221_AveragingMode4    = 1, 
    INA3221_AveragingMode16   = 2,
    INA3221_AveragingMode64   = 3,
    INA3221_AveragingMode128  = 4,
    INA3221_AveragingMode256  = 5,
    INA3221_AveragingMode512  = 6,
    INA3221_AveragingMode1024 = 7
  }INA3221AveragingMode_TypeDef;

  typedef enum
  {
    INA3221_BusConversionTime_140uS  = 0,
    INA3221_BusConversionTime_204uS  = 1,
    INA3221_BusConversionTime_332uS  = 2,
    INA3221_BusConversionTime_588uS  = 3,
    INA3221_BusConversionTime_1100uS = 4,  // default
    INA3221_BusConversionTime_2116uS = 5,
    INA3221_BusConversionTime_4156uS = 6,
    INA3221_BusConversionTime_8244uS = 7
  }INA3221BusConversionTime_TypeDef;

  typedef enum
  {
    INA3221_ShuntConversionTime_140uS  = 0,
    INA3221_ShuntConversionTime_204uS  = 1,
    INA3221_ShuntConversionTime_332uS  = 2,
    INA3221_ShuntConversionTime_588uS  = 3,
    INA3221_ShuntConversionTime_1100uS = 4,  // default
    INA3221_ShuntConversionTime_2116uS = 5,
    INA3221_ShuntConversionTime_4156uS = 6,
    INA3221_ShuntConversionTime_8244uS = 7
  }INA3221ShuntConversionTime_TypeDef;


  typedef enum
  {
    INA3221_Mode_PowerDown                    = 0,
    INA3221_Mode_ShuntVoltageTriggered        = 1,
    INA3221_Mode_BusVoltageTriggered          = 2,
    INA3221_Mode_ShuntAndBusVoltageTriggered  = 3,
    INA3221_Mode_PowerDown2                   = 4,
    INA3221_Mode_ShuntVoltageContinuous       = 5,
    INA3221_Mode_BusBoltageContinuous         = 6,
    INA3221_Mode_ShuntAndBusVoltageContinuous = 7     // Default
  }INA3221Mode_TypeDef;


  typedef enum
  {
    INA3221_MaskEnable_ConversionReadyFlag             = 1 << 0,
    INA3221_MaskEnable_TimingControlAlertFlagIndicator = 1 << 1,
    INA3221_MaskEnable_PowerValidAlertFlagIndicator    = 1 << 2,
    INA3221_MaskEnable_WarningAlertFlagIndicator1      = 1 << 3,
    INA3221_MaskEnable_WarningAlertFlagIndicator2      = 1 << 4,
    INA3221_MaskEnable_WarningAlertFlagIndicator3      = 1 << 5,
    INA3221_MaskEnable_SummationAlertFlagIndicator     = 1 << 6,
    INA3221_MaskEnable_CriticalAlertFlagIndicator1     = 1 << 7,
    INA3221_MaskEnable_CriticalAlertFlagIndicator2     = 1 << 8,
    INA3221_MaskEnable_CriticalAlertFlagIndicator3     = 1 << 9,
    INA3221_MaskEnable_CriticalAlertLatchEnable        = 1 << 10,
    INA3221_MaskEnable_WarningAlertLatchEnable         = 1 << 11,
    INA3221_MaskEnable_SummationChannelControl1        = 1 << 12,
    INA3221_MaskEnable_SummationChannelControl2        = 1 << 13,
    INA3221_MaskEnable_SummationChannelControl3        = 1 << 14,
  }INA3221MaskEnable_TypeDef;


  #define INA3221_CONFIG_DEFAULT                   (0x7127)
/*=========================================================================*/

/*=========================================================================
  SHUNT VOLTAGE REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_SHUNTVOLTAGE_CH1             (0x01)
  #define INA3221_REG_SHUNTVOLTAGE_CH2             (0x03)
  #define INA3221_REG_SHUNTVOLTAGE_CH3             (0x05)
/*=========================================================================*/

/*=========================================================================
  BUS VOLTAGE REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_BUSVOLTAGE_CH1               (0x02)
  #define INA3221_REG_BUSVOLTAGE_CH2               (0x04)
  #define INA3221_REG_BUSVOLTAGE_CH3               (0x06)
/*=========================================================================*/

/*=========================================================================
  CRITICAL ALERT (R/W)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_CRITICAL_ALERT_CH1           (0x07)
  #define INA3221_REG_CRITICAL_ALERT_CH2           (0x09)
  #define INA3221_REG_CRITICAL_ALERT_CH3           (0x0B)
/*=========================================================================*/

/*=========================================================================
  WARNING ALERT (R/W)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_WARNING_ALERT_CH1            (0x08)
  #define INA3221_REG_WARNING_ALERT_CH2            (0x0A)
  #define INA3221_REG_WARNING_ALERT_CH3            (0x0C)
  /*=========================================================================*/

/*=========================================================================
  SHUNT VOLTAGE SUM REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_SHUNT_VOLTAGE_SUM            (0x0D)
/*=========================================================================*/

/*=========================================================================
  SHUNT VOLTAGE SUM LIMIT REGISTER (R/W)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_CURRENT_LIMIT                (0x0E)
/*=========================================================================*/

/*=========================================================================
  MASK/ENABLE REGISTER (R/W)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_MASK_ENABLE                  (0x0F)
/*=========================================================================*/

/*=========================================================================
  POWER VALID UPPER LIMIT REGISTER (R/W)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_POWER_VALID_UPPER_LIMIT      (0x10)
/*=========================================================================*/

/*=========================================================================
  POWER VALID LOWER LIMIT REGISTER (R/W)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_POWER_VALID_LOWER_LIMIT      (0x11)
/*=========================================================================*/

/*=========================================================================
  MANUFACTURER ID REGISTER (R/W)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_MANUFACTURER_ID              (0xFE)
/*=========================================================================*/

/*=========================================================================
  DIE ID REGISTER (R)
  -------------------------------------------------------------------------*/
  #define INA3221_REG_DIE_ID                       (0xFF)
/*=========================================================================*/



class cINA3221: public cComNode
{
  public:

  struct tstRegConf
  {
    uint16 ui16Mode:3;
    uint16 ui16SCT2:3;
    uint16 ui16BCT2:3;
    uint16 ui16AVG2:3;
    uint16 ui16EnCh3:1;
    uint16 ui16EnCh2:1;
    uint16 ui16EnCh1:1;
    uint16 ui16RST:1;
  };

  union tunRegConf
  {
    uint16      ui16Value;
    tstRegConf  stBits;
  };

  typedef struct
  {
    uint16 ui16Reserved:3;
    uint16 ui16SD:12;
    uint16 ui16Sign:1;
  }tstRegAdcValue;

  union tunRegShuntVoltage
  {
    uint16         ui16Value;
    tstRegAdcValue stBits;
  };

  union tunRegBusVoltage
  {
    uint16         ui16Value;
    tstRegAdcValue stBits;
  };

  struct tstRegAlert
  {
    uint16 ui16Reserved : 3;
    uint16 ui16Alert : 13;
  };

  union tunRegAlert
  {
    uint16      ui16Value;
    tstRegAlert stBits;
  };

  union tunRegWarning
  {
    uint16      ui16Value;
    tstRegAlert stBits;
  };


  struct tstRegSum
  {
    uint16 ui16Reserved:1;
    uint16 ui16Sum:14;
    uint16 ui16Sign:1;
  };

  union tunRegSum
  {
    uint16    ui16Value;
    tstRegSum stBits;
  };

  union tunRegSumLimit
  {
    uint16    ui16Value;
    tstRegSum stBits;
  };

  struct tstRegMaskEnable
  {
    uint16 ui16ConversionReadyFlag:1;
    uint16 ui16TimingControlAlertFlagIndicator:1;
    uint16 ui16PowerValidAlertFlagIndicator:1;
    uint16 ui16WarningAlertFlagIndicator1:1;
    uint16 ui16WarningAlertFlagIndicator2:1;
    uint16 ui16WarningAlertFlagIndicator3:1;
    uint16 ui16SummationAlertFlagIndicator:1;
    uint16 ui16CriticalAlertFlagIndicator1:1;
    uint16 ui16CriticalAlertFlagIndicator2:1;
    uint16 ui16CriticalAlertFlagIndicator3:1;
    uint16 ui16CriticalAlertLatchEnable:1;
    uint16 ui16WarningAlertLatchEnable:1;
    uint16 ui16SummationChannelControl1:1;
    uint16 ui16SummationChannelControl2:1;
    uint16 ui16SummationChannelControl3:1;
};

  union tunRegMaskEnable
  {
    uint16           ui16Value;
    tstRegMaskEnable stBits;
  };

  union tunRegPowerValidUpperLimit
  {
    uint16         ui16Value;
    tstRegAdcValue stBits;
  };

  union tunRegPowerValidLowerLimit
  {
    uint16         ui16Value;
    tstRegAdcValue stBits;
  };


  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdInit3,
    enCmdInit4,
    enCmdInit5,
    enCmdReadReg,
    enCmdReadReg2,
    enCmdWriteReg,
    enCmdWriteReg2,
    enCmdCh1ReadVBus,
    enCmdCh1ReadVBus2,
    enCmdCh2ReadVBus,
    enCmdCh2ReadVBus2,
    enCmdCh3ReadVBus,
    enCmdCh3ReadVBus2,
    enCmdCh1ReadVShunt,
    enCmdCh1ReadVShunt2,
    enCmdCh2ReadVShunt,
    enCmdCh2ReadVShunt2,
    enCmdCh3ReadVShunt,
    enCmdCh3ReadVShunt2,
  }tenIna3221_Cmd;


  tunRegConf                 mRegConf;
  tunRegShuntVoltage         mRegVShuntCh1;
  tunRegBusVoltage           mRegVBusCh1;
  tunRegShuntVoltage         mRegVShuntCh2;
  tunRegBusVoltage           mRegVBusCh2;
  tunRegShuntVoltage         mRegVShuntCh3;
  tunRegBusVoltage           mRegVBusCh3;  
  tunRegAlert                mRegCritAlert1;
  tunRegWarning              mRegWarnAlert1;
  tunRegAlert                mRegCritAlert2;
  tunRegWarning              mRegWarnAlert2;
  tunRegAlert                mRegCritAlert3;
  tunRegWarning              mRegWarnAlert3;
  tunRegSum                  mRegCritSum;
  tunRegSumLimit             mRegCritLimit;
  tunRegMaskEnable           mRegMaskEnable;
  tunRegPowerValidUpperLimit mRegPowerValidUp;
  tunRegPowerValidLowerLimit mRegPowerValidLow;
  uint16                     mManuID;
  uint16                     mDieID;

  uint16             mShunt1; // [mOhm]
  uint16             mShunt2; // [mOhm]
  uint16             mShunt3; // [mOhm]

  uint16             mRegIdx;

  cI2cMaster*        mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenIna3221_Cmd     menCmd;

  bool mbInit;
  bool mbReadReg;
  bool mbWriteReg;
  bool mbCmdCh1ReadVBus;
  bool mbCmdCh2ReadVBus;
  bool mbCmdCh3ReadVBus;
  bool mbCmdCh1ReadVShunt;
  bool mbCmdCh2ReadVShunt;
  bool mbCmdCh3ReadVShunt;


  cINA3221(cI2cMaster* lpcI2C, uint8 lui8Adr, uint16 lui16Shunt1, uint16 lui16Shunt2, uint16 lui16Shunt3);
  
  #if _WIN32
    ~cINA3221();
  #endif // _WIN32

  int8  i8Setup(INA3221Mode_TypeDef lenMode, 
                INA3221ShuntConversionTime_TypeDef lenShuntConversionTime, 
                INA3221BusConversionTime_TypeDef lenBusConversionTime,
                INA3221AveragingMode_TypeDef lenAveragingMode,
                bool bEnableCh1,
                bool bEnableCh2,
                bool bEnableCh3);
  int8  i8Setup();

  uint32 SizeOf() {return 0;};
  void  vComStart(cComNode::tenEvent lenEvent) override;
  void  vComDone() override;

  void  vSetReg(u16 lu16Idx, u16 lu16Value);
  u16   u16GetReg(u16 lu16Idx);

  void  vStartWriteReg(u16 lu16Idx);
  void  vStartReadReg(u16 lu16Idx);

  int8  i8ReadVBus_digit(u8 lu8Ch);
  int8  i8ReadVShunt_digit(u8 lu8Ch);

  //int32 i32CalcAdc(tstRegAdcValue lstValue);
  int32 i32CalcIShunt_uA(u8 lu8Ch);
  int32 i32CalcVShunt_uV(u8 lu8Ch);
  int32 i32CalcVBus_uV(u8 lu8Ch);
};

#endif // __INA3221_H__
