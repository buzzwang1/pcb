#include "ina219.h"

cINA219::cINA219(cI2cMaster* lpcI2C, uint8 lui8Adr, uint16 lui16Shunt)
{
  mRegConf.ui16Value = 0;
  mRegVShunt.ui16Value = 0;
  mRegVBus.ui16Value = 0;
  mRegIShunt.ui16Value = 0;
  mRegPwr = 0;
  mRegCal = 0;

  if (lui8Adr < INA219_I2C_ADDRESS_CONF_0)
  {
    lui8Adr <<= 1;
  }

  mShunt = lui16Shunt;
  mI2C   = lpcI2C;
  mAdr   = lui8Adr;

  mpcMsgRead.vMemAlloc(1, 2);
  mpcMsgWrite.vMemAlloc(3, 0);

  menCmd            = enCmdIdle;
  mStatus.IsInit    = false;

  i8Setup();

  mStatus.IsStartRequested = (mbInit |
                              mbReadConfig |
                              mbWriteConfig |
                              mbReadIShunt |
                              mbReadVShunt |
                              mbReadVBus);

  mI2C->vAddSlave((cComNode*)this);
}

cINA219::~cINA219()
{
  //mpcMsgRead.vMemFree();
  //mpcMsgWrite.vMemFree();
}


void cINA219::vComStart(cComNode::tenEvent lenEvent)
{
  UNUSED(lenEvent);

  if (IsError())
  {
    mStatus.IsError  = false;
    menCmd = enCmdIdle;
    mStatus.IsInit  = false;
    i8Setup();
  }

  if (menCmd == enCmdIdle)
  {
    if (mbInit)
    {
      mbInit = false;
      menCmd = enCmdInit;
      vComDone();
    }
    else if (mbReadConfig)
    {
      mbReadConfig = false;
      menCmd = enCmdReadConfig;
      vComDone();
    }

    else if (mbWriteConfig)
    {
      mbWriteConfig = false;
      menCmd = enCmdWriteConfig;
      vComDone();
    }
    else if (mbReadIShunt)
    {
      mbReadIShunt = false;
      menCmd = enCmdReadIShunt;
      vComDone();
    }
    else if (mbReadVShunt)
    {
      mbReadVShunt = false;
      menCmd = enCmdReadVShunt;
      vComDone();
    }
    else if (mbReadVBus)
    {
      mbReadVBus = false;
      menCmd = enCmdReadVBus;
      vComDone();
    }
  }
  mStatus.IsStartRequested = (mbInit |
                              mbReadConfig |
                              mbWriteConfig |
                              mbReadIShunt |
                              mbReadVShunt |
                              mbReadVBus);
}

void cINA219::vComDone()
{
  uint16 lui16Temp;

  lui16Temp = mpcMsgRead.cRxData.mpu8Data[1] + (mpcMsgRead.cRxData.mpu8Data[0] << 8);


  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdInit:
      mIdx    = 0;
      menCmd = enCmdInit2;
      //Write Reset
      tunRegConf lRegConf;

      lRegConf = mRegConf;
      lRegConf.stBits.ui16RST = 1;
      mpcMsgWrite.cTxData.mpu8Data[0] = INA219_REG_CONFIG;
      mpcMsgWrite.cTxData.mpu8Data[1] = (uint8)(lRegConf.ui16Value >> 8);
      mpcMsgWrite.cTxData.mpu8Data[2] = (uint8)lRegConf.ui16Value;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);

      break;
    case enCmdInit2:
      switch (mIdx)
      {
        case 0: // Reset callback - Initial Reset finished
    	  mIdx = 1;
    	  // Read back config
          mpcMsgRead.cTxData.mpu8Data[0] = INA219_REG_CONFIG;
          mpcMsgRead.vStart();
          mI2C->vStartMsg(&mpcMsgRead);
          break;
        case 1: // Read Config callback
          //If state is ok, write given configuration, else repeat
          if (lui16Temp == INA219_CONFIG_DEFAULT)
          {
        	if  (INA219_CONFIG_DEFAULT != mRegConf.ui16Value)
        	{
              mIdx = 2;
              mpcMsgWrite.cTxData.mpu8Data[0] = INA219_REG_CONFIG;
              mpcMsgWrite.cTxData.mpu8Data[1] = (uint8)(mRegConf.ui16Value >> 8);
              mpcMsgWrite.cTxData.mpu8Data[2] = (uint8)mRegConf.ui16Value;
              mpcMsgWrite.vStart();
              mI2C->vStartMsg(&mpcMsgWrite);
        	}
        	else
        	{
              mStatus.IsInit = true;
              menCmd   = enCmdIdle;
        	}
          }
          else
          {
            //Set Flag for Re-Init
            mStatus.IsInit = false;
            mbInit   = true;
            menCmd   = enCmdIdle;
          }
          break;
        case 2: // 2nd Write config callback
          mStatus.IsInit = true;
          menCmd   = enCmdIdle;
          break;
      }
      break;

    case enCmdReadConfig:
      mIdx    = 0;
      menCmd  = enCmdReadConfig2;
      mpcMsgRead.cTxData.mpu8Data[0] = INA219_REG_CONFIG;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadConfig2:
      menCmd     = enCmdIdle;
      mRegConf.ui16Value = lui16Temp;
      break;

    case enCmdWriteConfig:
      mIdx    = 0;
      menCmd = enCmdWriteConfig2;
      mpcMsgWrite.cTxData.mpu8Data[0] = INA219_REG_CONFIG;
      mpcMsgWrite.cTxData.mpu8Data[1] = (uint8)(mRegConf.ui16Value >> 8);
      mpcMsgWrite.cTxData.mpu8Data[2] = (uint8)mRegConf.ui16Value;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdWriteConfig2:
      menCmd     = enCmdIdle;
      break;

    case enCmdReadIShunt:
      mIdx    = 0;
      menCmd = enCmdReadIShunt2;
      mpcMsgRead.cTxData.mpu8Data[0] = INA219_REG_CURRENT;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadIShunt2:
      menCmd     = enCmdIdle;
      mRegIShunt.ui16Value = lui16Temp;
      break;

    case enCmdReadVShunt:
      mIdx    = 0;
      menCmd = enCmdReadVShunt2;
      mpcMsgRead.cTxData.mpu8Data[0] = INA219_REG_SHUNTVOLTAGE;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadVShunt2:
      menCmd     = enCmdIdle;
      mRegVShunt.ui16Value = lui16Temp;
      break;

    case enCmdReadVBus:
      mIdx    = 0;
      menCmd = enCmdReadVBus2;
      mpcMsgRead.cTxData.mpu8Data[0] = INA219_REG_BUSVOLTAGE;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadVBus2:
      menCmd     = enCmdIdle;
      mRegVBus.ui16Value = lui16Temp;
      break;

    default:
      break;
  }
}


int8 cINA219::i8Setup(uint8 lui8BRng, uint8 lui8PGA, uint8 lui8BAdc, uint8 lui8SAdc, uint8 lui8Mode)
{
  int li8Error = FRET_OK;

  mbInit         = true;
  mbReadConfig   = true;
  //mbWriteConfig  = true;

  mbReadIShunt   = true;
  mbReadVShunt   = true;
  mbReadVBus     = true;

  mRegConf.stBits.ui16BRNG = lui8BRng;
  mRegConf.stBits.ui16PG   = lui8PGA;
  mRegConf.stBits.ui16BADC = lui8BAdc;
  mRegConf.stBits.ui16SADC = lui8SAdc;
  mRegConf.stBits.ui16Mode = lui8Mode;

  mStatus.IsStartRequested = 1;

  return li8Error;
}

int8 cINA219::i8Setup()
{
  return i8Setup(INA219_BusVoltageRange_32V,
                 INA219_GainRange_8_320mV,
                 INA219_ShuntADCResolution_12Bit_1S_532uS,
                 INA219_ShuntADCResolution_12Bit_1S_532uS,
                 INA219_Mode_ShuntAndBusVoltageContinuous);
}


int8 cINA219::i8PerformCalibration_digit()
{
  int li8Error = FRET_OK;
  // Write new values (calculated from INA219 datasheet) of calibration
  // register
  //
  // WARNING: All calculations (except measuring voltage on bus and shunt
  // resistor - 0.1 ohm in this case) are based on "PROGRAMMING THE INA219
  // POWER MEASUREMENT ENGINE" chapter of INA219 datasheet

  //return (mI2CData[1] << 8) | mI2CData[2];
  return li8Error;
}

int8 cINA219::i8ReadConfig_digit()
{
  mbReadConfig = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}

int8 cINA219::i8ReadVBus_digit()
{
  mbReadVBus = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}

int8 cINA219::i8ReadVShunt_digit()
{
  mbReadVShunt = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}


int8 cINA219::i8ReadIShunt_digit()
{
  mbReadIShunt = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}


int32 cINA219::i32CalcIShunt_uA()
{
  // 10mV     = 10.000uV
  // 100mOhm  = 0.1 Ohm
  // => 10.000uV/100mOhm = 100mA = 100.000uA

  //!!! VShunt und nicht IShunt werden benutzt.
  return ((i32CalcVShunt_uV() * 1000) / mShunt);
}


int32 cINA219::i32CalcVShunt_uV()
{
  int32  li32VShunt;  // [uV]

  // LSB = 10μV

  /*switch (mRegConf.stBits.ui16PG)
  {
    case INA219_GainRange_1_40mV:  li32VShunt  = (int16)(mRegVShunt.stBits1.ui16SD); break;
    case INA219_GainRange_2_80mV:  li32VShunt  = (int16)(mRegVShunt.stBits2.ui16SD); break;
    case INA219_GainRange_4_160mV: li32VShunt  = (int16)(mRegVShunt.stBits4.ui16SD); break;
    case INA219_GainRange_8_320mV: li32VShunt  = (int16)(mRegVShunt.stBits8.ui16SD); break;
  }*/


  li32VShunt  = (int16)(mRegVShunt.ui16Value);
  li32VShunt *= 10; // [uV]

  return li32VShunt;
}


int32 cINA219::i32CalcVBus_uV()
{
  int32   li32VBus;
  li32VBus = mRegVBus.stBits.ui16BD * 4;
  return li32VBus * 1000;
}

