#include "ina3221.h"

cINA3221::cINA3221(cI2cMaster* lpcI2C, uint8 lui8Adr, uint16 lui16Shunt1, uint16 lui16Shunt2, uint16 lui16Shunt3)
{

  if (lui8Adr < INA3221_I2C_ADDRESS_CONF_0)
  {
    lui8Adr <<= 1;
  }

  mShunt1 = lui16Shunt1;
  mShunt2 = lui16Shunt2;
  mShunt3 = lui16Shunt3;
  mI2C   = lpcI2C;
  mAdr   = lui8Adr;

  mManuID = 0;
  mDieID  = 0;

  mpcMsgRead.vMemAlloc(1, 2);
  mpcMsgWrite.vMemAlloc(3, 0);

  menCmd            = enCmdIdle;
  mStatus.IsInit    = false;

  // Auf false setzen, damit wenn Klasse mit new angelegt wird
  // dass sie einen Wert haben.
  mbInit             = false;
  mbReadReg          = false;
  mbWriteReg         = false;
  mbCmdCh1ReadVBus   = false;
  mbCmdCh2ReadVBus   = false;
  mbCmdCh3ReadVBus   = false;
  mbCmdCh1ReadVShunt = false;
  mbCmdCh2ReadVShunt = false;
  mbCmdCh3ReadVShunt = false;

  i8Setup(INA3221_Mode_ShuntAndBusVoltageContinuous,
          INA3221_ShuntConversionTime_332uS,
          INA3221_BusConversionTime_332uS,
          INA3221_AveragingMode1,
          True, True, True);

  mStatus.IsStartRequested = mbInit | mbReadReg | mbWriteReg;

  mI2C->vAddSlave((cComNode*)this);
}

#if _WIN32
  cINA3221::~cINA3221()
  {
    mpcMsgRead.vMemFree();
    mpcMsgWrite.vMemFree();
  }
#endif // _WIN32


void cINA3221::vComStart(cComNode::tenEvent lenEvent)
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
    else if (mbReadReg)
    {
      mbReadReg = false;
      menCmd = enCmdReadReg;
      vComDone();
    }
    else if (mbWriteReg)
    {
      mbWriteReg = false;
      menCmd = enCmdWriteReg;
      vComDone();
    }
    else if (mbCmdCh1ReadVBus)
    {
      mbCmdCh1ReadVBus = false;
      menCmd = enCmdCh1ReadVBus;
      vComDone();
    }
    else if (mbCmdCh2ReadVBus)
    {
      mbCmdCh2ReadVBus = false;
      menCmd = enCmdCh2ReadVBus;
      vComDone();
    }
    else if (mbCmdCh3ReadVBus)
    {
      mbCmdCh3ReadVBus = false;
      menCmd = enCmdCh3ReadVBus;
      vComDone();
    }
    else if (mbCmdCh1ReadVShunt)
    {
      mbCmdCh1ReadVShunt = false;
      menCmd = enCmdCh1ReadVShunt;
      vComDone();
    }
    else if (mbCmdCh2ReadVShunt)
    {
      mbCmdCh2ReadVShunt = false;
      menCmd = enCmdCh2ReadVShunt;
      vComDone();
    }
    else if (mbCmdCh3ReadVShunt)
    {
      mbCmdCh3ReadVShunt = false;
      menCmd = enCmdCh3ReadVShunt;
      vComDone();
    }

    mStatus.IsStartRequested = (mbInit |
                                mbReadReg |
                                mbWriteReg |
                                mbCmdCh1ReadVBus |
                                mbCmdCh2ReadVBus |
                                mbCmdCh3ReadVBus |
                                mbCmdCh1ReadVShunt |
                                mbCmdCh2ReadVShunt |
                                mbCmdCh3ReadVShunt);
  }
}

void cINA3221::vComDone()
{
  uint16 lui16Temp;

  lui16Temp = mpcMsgRead.cRxData.mpu8Data[1] + (mpcMsgRead.cRxData.mpu8Data[0] << 8);


  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdInit:
      menCmd = enCmdInit2;

      // Write Reset
      tunRegConf lRegConf;
      lRegConf = mRegConf;
      lRegConf.stBits.ui16RST = 1;
      mpcMsgWrite.cTxData.mpu8Data[0] = INA3221_REG_CONFIG;
      mpcMsgWrite.cTxData.mpu8Data[1] = (uint8)(lRegConf.ui16Value >> 8);
      mpcMsgWrite.cTxData.mpu8Data[2] = (uint8)lRegConf.ui16Value;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;

    case enCmdInit2:
      menCmd = enCmdInit3;
      // Write Cfg
      vStartWriteReg(INA3221_REG_CONFIG);
      break;

    case enCmdInit3:
      menCmd = enCmdInit4;
      // Read Manufacturer ID
      vStartReadReg(INA3221_REG_MANUFACTURER_ID);
      break;

    case enCmdInit4:
      menCmd = enCmdInit5;
      vSetReg(INA3221_REG_MANUFACTURER_ID, lui16Temp);
      // Read Manufacturer ID
      vStartReadReg(INA3221_REG_DIE_ID);
      break;

    case enCmdInit5:
      vSetReg(INA3221_REG_DIE_ID, lui16Temp);
      if ((mManuID == 0x5449) && (mDieID == 0x3220))
      {
        mStatus.IsInit = true;
      }
      menCmd = enCmdIdle;
      break;

    case enCmdWriteReg:
      menCmd = enCmdWriteReg2;
      vStartWriteReg(mRegIdx);
      break;
    case enCmdWriteReg2:
      menCmd     = enCmdIdle;
      mRegConf.ui16Value = lui16Temp;
      break;

    case enCmdReadReg:
      menCmd = enCmdReadReg2;
      vStartReadReg(mRegIdx);
      break;
    case enCmdReadReg2:
      menCmd = enCmdIdle;
      vSetReg(mRegIdx, lui16Temp);
      break;

    case enCmdCh1ReadVBus:  menCmd = enCmdCh1ReadVBus2; vStartReadReg(INA3221_REG_BUSVOLTAGE_CH1); break;
    case enCmdCh1ReadVBus2: menCmd = enCmdIdle;         vSetReg(INA3221_REG_BUSVOLTAGE_CH1, lui16Temp); break;
    case enCmdCh2ReadVBus:  menCmd = enCmdCh2ReadVBus2; vStartReadReg(INA3221_REG_BUSVOLTAGE_CH2); break;
    case enCmdCh2ReadVBus2: menCmd = enCmdIdle;         vSetReg(INA3221_REG_BUSVOLTAGE_CH2, lui16Temp); break;
    case enCmdCh3ReadVBus:  menCmd = enCmdCh3ReadVBus2; vStartReadReg(INA3221_REG_BUSVOLTAGE_CH3); break;
    case enCmdCh3ReadVBus2: menCmd = enCmdIdle;         vSetReg(INA3221_REG_BUSVOLTAGE_CH3, lui16Temp); break;

    case enCmdCh1ReadVShunt:  menCmd = enCmdCh1ReadVShunt2; vStartReadReg(INA3221_REG_SHUNTVOLTAGE_CH1); break;
    case enCmdCh1ReadVShunt2: menCmd = enCmdIdle;           vSetReg(INA3221_REG_SHUNTVOLTAGE_CH1, lui16Temp); break;
    case enCmdCh2ReadVShunt:  menCmd = enCmdCh2ReadVShunt2; vStartReadReg(INA3221_REG_SHUNTVOLTAGE_CH2); break;
    case enCmdCh2ReadVShunt2: menCmd = enCmdIdle;           vSetReg(INA3221_REG_SHUNTVOLTAGE_CH2, lui16Temp); break;
    case enCmdCh3ReadVShunt:  menCmd = enCmdCh3ReadVShunt2; vStartReadReg(INA3221_REG_SHUNTVOLTAGE_CH3); break;
    case enCmdCh3ReadVShunt2: menCmd = enCmdIdle;           vSetReg(INA3221_REG_SHUNTVOLTAGE_CH3, lui16Temp); break;


    default:
      break;
  }
}


int8 cINA3221::i8Setup(INA3221Mode_TypeDef lenMode,
                       INA3221ShuntConversionTime_TypeDef lenShuntConversionTime,
                       INA3221BusConversionTime_TypeDef lenBusConversionTime,
                       INA3221AveragingMode_TypeDef lenAveragingMode,
                       bool bEnableCh1,
                       bool bEnableCh2,
                       bool bEnableCh3)
{
  int li8Error = FRET_OK;

  mbInit         = true;

  mRegConf.stBits.ui16Mode  = lenMode;
  mRegConf.stBits.ui16SCT2  = lenShuntConversionTime;
  mRegConf.stBits.ui16BCT2  = lenBusConversionTime;
  mRegConf.stBits.ui16AVG2  = lenAveragingMode;
  mRegConf.stBits.ui16EnCh3 = bEnableCh3;
  mRegConf.stBits.ui16EnCh2 = bEnableCh2;
  mRegConf.stBits.ui16EnCh1 = bEnableCh1;
  mRegConf.stBits.ui16RST   = 0;

  mStatus.IsStartRequested = 1;

  return li8Error;
}

int8 cINA3221::i8Setup()
{
  int li8Error = FRET_OK;
  mbInit = true;
  mStatus.IsStartRequested = 1;
  return li8Error;
}


void cINA3221::vStartWriteReg(u16 lu16Idx)
{
  u16 lu16Value = u16GetReg(lu16Idx);

  mpcMsgWrite.cTxData.mpu8Data[0] = lu16Idx;
  mpcMsgWrite.cTxData.mpu8Data[1] = (uint8)(lu16Value >> 8);
  mpcMsgWrite.cTxData.mpu8Data[2] = (uint8)lu16Value;
  mpcMsgWrite.vStart();
  mI2C->vStartMsg(&mpcMsgWrite);
}

void cINA3221::vStartReadReg(u16 lu16Idx)
{
  mpcMsgRead.cTxData.mpu8Data[0] = lu16Idx;
  mpcMsgRead.vStart();
  mI2C->vStartMsg(&mpcMsgRead);
}


void cINA3221::vSetReg(u16 lu16Idx, u16 lu16Value)
{
  switch (lu16Idx)
  {
    case INA3221_REG_CONFIG:                  mRegConf.ui16Value      = lu16Value; break;
    case INA3221_REG_SHUNTVOLTAGE_CH1:        mRegVShuntCh1.ui16Value = lu16Value; break;
    case INA3221_REG_BUSVOLTAGE_CH1:          mRegVBusCh1.ui16Value   = lu16Value; break;
    case INA3221_REG_SHUNTVOLTAGE_CH2:        mRegVShuntCh2.ui16Value = lu16Value; break;
    case INA3221_REG_BUSVOLTAGE_CH2:          mRegVBusCh2.ui16Value   = lu16Value; break;
    case INA3221_REG_SHUNTVOLTAGE_CH3:        mRegVShuntCh3.ui16Value = lu16Value; break;
    case INA3221_REG_BUSVOLTAGE_CH3:          mRegVBusCh3.ui16Value   = lu16Value;  break;
    case INA3221_REG_CRITICAL_ALERT_CH1:      mRegCritAlert1.ui16Value = lu16Value; break;
    case INA3221_REG_WARNING_ALERT_CH1:       mRegWarnAlert1.ui16Value = lu16Value; break;
    case INA3221_REG_CRITICAL_ALERT_CH2:      mRegWarnAlert2.ui16Value = lu16Value; break;
    case INA3221_REG_WARNING_ALERT_CH2:       mRegCritAlert2.ui16Value = lu16Value; break;
    case INA3221_REG_CRITICAL_ALERT_CH3:      mRegCritAlert3.ui16Value = lu16Value; break;
    case INA3221_REG_WARNING_ALERT_CH3:       mRegWarnAlert3.ui16Value = lu16Value; break;
    case INA3221_REG_SHUNT_VOLTAGE_SUM:       mRegCritSum.ui16Value       = lu16Value; break;
    case INA3221_REG_CURRENT_LIMIT:           mRegCritLimit.ui16Value     = lu16Value; break;
    case INA3221_REG_MASK_ENABLE:             mRegMaskEnable.ui16Value    = lu16Value; break;
    case INA3221_REG_POWER_VALID_UPPER_LIMIT: mRegPowerValidUp.ui16Value  = lu16Value; break;
    case INA3221_REG_POWER_VALID_LOWER_LIMIT: mRegPowerValidLow.ui16Value = lu16Value; break;
    case INA3221_REG_MANUFACTURER_ID:         mManuID = lu16Value; break;
    case INA3221_REG_DIE_ID:                  mDieID  = lu16Value; break;
  }
}

u16 cINA3221::u16GetReg(u16 lu16Idx)
{
  u16 lu16Ret = 0;
  switch (lu16Idx)
  {
    case INA3221_REG_CONFIG:                  lu16Ret = mRegConf.ui16Value; break;
    case INA3221_REG_SHUNTVOLTAGE_CH1:        lu16Ret = mRegVShuntCh1.ui16Value; break;
    case INA3221_REG_BUSVOLTAGE_CH1:          lu16Ret = mRegVBusCh1.ui16Value; break;
    case INA3221_REG_SHUNTVOLTAGE_CH2:        lu16Ret = mRegVShuntCh2.ui16Value; break;
    case INA3221_REG_BUSVOLTAGE_CH2:          lu16Ret = mRegVBusCh2.ui16Value; break;
    case INA3221_REG_SHUNTVOLTAGE_CH3:        lu16Ret = mRegVShuntCh3.ui16Value; break;
    case INA3221_REG_BUSVOLTAGE_CH3:          lu16Ret = mRegVBusCh3.ui16Value;  break;
    case INA3221_REG_CRITICAL_ALERT_CH1:      lu16Ret = mRegCritAlert1.ui16Value; break;
    case INA3221_REG_WARNING_ALERT_CH1:       lu16Ret = mRegWarnAlert1.ui16Value; break;
    case INA3221_REG_CRITICAL_ALERT_CH2:      lu16Ret = mRegWarnAlert2.ui16Value; break;
    case INA3221_REG_WARNING_ALERT_CH2:       lu16Ret = mRegCritAlert2.ui16Value; break;
    case INA3221_REG_CRITICAL_ALERT_CH3:      lu16Ret = mRegCritAlert3.ui16Value; break;
    case INA3221_REG_WARNING_ALERT_CH3:       lu16Ret = mRegWarnAlert3.ui16Value; break;
    case INA3221_REG_SHUNT_VOLTAGE_SUM:       lu16Ret = mRegCritSum.ui16Value; break;
    case INA3221_REG_CURRENT_LIMIT:           lu16Ret = mRegCritLimit.ui16Value; break;
    case INA3221_REG_MASK_ENABLE:             lu16Ret = mRegMaskEnable.ui16Value; break;
    case INA3221_REG_POWER_VALID_UPPER_LIMIT: lu16Ret = mRegPowerValidUp.ui16Value; break;
    case INA3221_REG_POWER_VALID_LOWER_LIMIT: lu16Ret = mRegPowerValidLow.ui16Value; break;
    case INA3221_REG_MANUFACTURER_ID:         lu16Ret = mManuID; break;
    case INA3221_REG_DIE_ID:                  lu16Ret = mDieID; break;
  }
  return lu16Ret;
}


int8 cINA3221::i8ReadVBus_digit(u8 lu8Ch)
{
  switch (lu8Ch)
  {
    case 3:  mbCmdCh3ReadVBus = True; break;
    case 2:  mbCmdCh2ReadVBus = True; break;
    default: mbCmdCh1ReadVBus = True; break;
  }
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}

int8 cINA3221::i8ReadVShunt_digit(u8 lu8Ch)
{
  switch (lu8Ch)
  {
    case 3:  mbCmdCh3ReadVShunt = True; break;
    case 2:  mbCmdCh2ReadVShunt = True; break;
    default: mbCmdCh1ReadVShunt = True; break;
  }

  mStatus.IsStartRequested = 1;

  return FRET_OK;
}

/*int32 cINA3221::i32CalcAdc(tstRegAdcValue lstValue)
{
  int32  li32Value;

  if (lstValue.ui16Sign)
  {
    li32Value = (int16)(-lstValue.ui16SD);
  }
  else
  {
    li32Value = (int16)(lstValue.ui16SD);
  }
  return li32Value;
}*/

int32 cINA3221::i32CalcIShunt_uA(u8 lu8Ch)
{
  // 10mV     = 10.000uV
  // 100mOhm  = 0.1 Ohm
  // => 10.000uV/100mOhm = 100mA = 100.000uA

  //!!! VShunt und nicht IShunt werden benutzt.
  switch (lu8Ch)
  {
    case 3:  return ((i32CalcVShunt_uV(lu8Ch) * 1000) / mShunt3); break;
    case 2:  return ((i32CalcVShunt_uV(lu8Ch) * 1000) / mShunt2); break;
    default: return ((i32CalcVShunt_uV(lu8Ch) * 1000) / mShunt1); break;
  }
}


int32 cINA3221::i32CalcVShunt_uV(u8 lu8Ch)
{
  int32  li32VShunt;  // [uV]

  // LSB = 40uV

  switch (lu8Ch)
  {
    case 3:  li32VShunt = (i16)mRegVShuntCh3.ui16Value; break;
    case 2:  li32VShunt = (i16)mRegVShuntCh2.ui16Value; break;
    default: li32VShunt = (i16)mRegVShuntCh1.ui16Value; break;
  }
  li32VShunt *= 5; // [uV]
  return li32VShunt;
}

int32 cINA3221::i32CalcVBus_uV(u8 lu8Ch)
{
  int32  li32VBus;  // [uV]
  // LSB = 8mV
  switch (lu8Ch)
  {
    case 3:  li32VBus = (i16)mRegVBusCh3.ui16Value; break;
    case 2:  li32VBus = (i16)mRegVBusCh2.ui16Value; break;
    default: li32VBus = (i16)mRegVBusCh1.ui16Value; break;
  }
  li32VBus *= 1000; // [uV]
  return li32VBus;
}


