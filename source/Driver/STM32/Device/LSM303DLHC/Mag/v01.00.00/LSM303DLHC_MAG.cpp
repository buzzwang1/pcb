#include "LSM303DLHC_MAG.h"

#define cLSM303DLHC_MAG_vSetStartRequest()  mStatus.IsStartRequested = (uint8)(mbReadAll |\
                                                                               mbWriteConfig |\
                                                                               mbReadMag |\
                                                                               mbReadTemperature)

cLSM303DLHC_MAG::cLSM303DLHC_MAG(cI2cMaster* lpcI2C)
{
  /*mui16Temperature_digit = 0;
  mui16Humiditiy_digit   = 0;
  mui8User_digit         = 0;*/

  mI2C   = lpcI2C;
  mAdr   = LSM303DLHC_MAG_I2C_ADDRESS_DEFAULT;

  mpcMsgRead.vMemAlloc(1, 8);
  mpcMsgWrite.vMemAlloc(2, 0);

  menCmd      = enCmdIdle;
  mStatus.IsInit    = false;

  vSetup();

  cLSM303DLHC_MAG_vSetStartRequest();

  mI2C->vAddSlave((cComNode*)this);
}

cLSM303DLHC_MAG::~cLSM303DLHC_MAG()
{
  mpcMsgRead.vMemFree();
  mpcMsgWrite.vMemFree();
}

void cLSM303DLHC_MAG::vComError(cComNode::tenError lenError, cComNode::tenState lenState)
{
  UNUSED(lenError);
  UNUSED(lenState);

  //cComNode::vComError(lenError, lenState);
  mStatus.IsStartRequested  = false;
  if (lenError != enErNACK)
  {
    mStatus.IsInit   = false;
    mStatus.IsError  = true;
  }
}

void cLSM303DLHC_MAG::vComStart(cComNode::tenEvent lenEvent)
{
  UNUSED(lenEvent);

  if (IsError())
  {
    mStatus.IsError = false;
    menCmd     = enCmdIdle;
    vSetup();
  }

  if (menCmd == enCmdIdle)
  {
    if (mbReadAll)
    {
      mbReadAll = false;
      menCmd    = enCmdReadAll;
      vComDone();
    }
    else if (mbWriteConfig)
    {
      mbWriteConfig = false;
      menCmd = enCmdWriteConfig1;
      vComDone();
    }
    else if (mbReadMag)
    {
      mbReadMag = false;
      menCmd = enCmdReadMag;
      vComDone();
    }
    else if (mbReadTemperature)
    {
      mbReadTemperature = false;
      menCmd = enCmdReadTemperature;
      vComDone();
    }
  }

  cLSM303DLHC_MAG_vSetStartRequest();
}

void cLSM303DLHC_MAG::vComDone()
{
  static const uint8 mpaui8ReadAllSequence[] =
  {
    LSM303DLHC_MAG_CRA_REG_M,
    LSM303DLHC_MAG_CRB_REG_M,
    LSM303DLHC_MAG_MR_REG_M,
    LSM303DLHC_MAG_OUT_X_H_M,
    LSM303DLHC_MAG_OUT_X_L_M,
    LSM303DLHC_MAG_OUT_Z_H_M,
    LSM303DLHC_MAG_OUT_Z_L_M,
    LSM303DLHC_MAG_OUT_Y_H_M,
    LSM303DLHC_MAG_OUT_Y_L_M,

    LSM303DLHC_MAG_SR_REG_M,

    LSM303DLHC_MAG_IRA_REG_M,
    LSM303DLHC_MAG_IRB_REG_M,
    LSM303DLHC_MAG_IRC_REG_M,

    LSM303DLHC_MAG_TEMP_OUT_H_M,
    LSM303DLHC_MAG_TEMP_OUT_L_M
  };

  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdReadAll:
      mIdx    = 0;
      mpcMsgRead.cTxData.muiLen = 1;
      mpcMsgRead.cRxData.muiLen = 1;

      menCmd = enCmdReadAll2;
      mpcMsgRead.cTxData.mpu8Data[0] = mpaui8ReadAllSequence[mIdx];
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;

    case enCmdReadAll2:
      if (mIdx < sizeof(mpaui8ReadAllSequence))
      {
        u8RamMirrow[uReg2Idx(mIdx)] = mpcMsgRead.cRxData.mpu8Data[0];
      }

      if (mIdx < sizeof(mpaui8ReadAllSequence) - 1)
      {
        mIdx++;
        mpcMsgRead.cTxData.mpu8Data[0] = mpaui8ReadAllSequence[mIdx];
        mpcMsgRead.vStart();
        mI2C->vStartMsg(&mpcMsgRead);
      }
      else
      {
        mStatus.IsInit = true;
        menCmd    = enCmdIdle;

      }
      break;

    case enCmdWriteConfig1:
      mpcMsgWrite.cTxData.muiLen = 2;
      mpcMsgWrite.cRxData.muiLen = 0;

      menCmd = enCmdWriteConfig2;
      mpcMsgWrite.cTxData.mpu8Data[0] = LSM303DLHC_MAG_CRA_REG_M;
      mpcMsgWrite.cTxData.mpu8Data[1] = u8RamMirrow[LSM303DLHC_MAG_CRA_REG_M];
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdWriteConfig2:
      menCmd = enCmdWriteConfig3;
      mpcMsgWrite.cTxData.mpu8Data[0] = LSM303DLHC_MAG_CRB_REG_M;
      mpcMsgWrite.cTxData.mpu8Data[1] = u8RamMirrow[LSM303DLHC_MAG_CRB_REG_M];
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdWriteConfig3:
      menCmd = enCmdWriteConfig4;
      mpcMsgWrite.cTxData.mpu8Data[0] = LSM303DLHC_MAG_MR_REG_M;
      mpcMsgWrite.cTxData.mpu8Data[1] = u8RamMirrow[LSM303DLHC_MAG_MR_REG_M];
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdWriteConfig4:
      menCmd     = enCmdIdle;
      break;


    case enCmdReadMag:
      menCmd = enCmdReadMag2;
      mpcMsgRead.cRxData.muiLen = 6;
      mpcMsgRead.cTxData.muiLen = 1;
      mpcMsgRead.cTxData.mpu8Data[0] = LSM303DLHC_MAG_OUT_X_H_M;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadMag2:
      menCmd = enCmdIdle;
      for (uint8 lui8t = 0; lui8t < mpcMsgRead.cRxData.muiLen; lui8t++)
      {
        u8RamMirrow[LSM303DLHC_MAG_OUT_X_H_M + lui8t] = mpcMsgRead.cRxData.mpu8Data[lui8t];
      }
      break;

    case enCmdReadTemperature:
      menCmd = enCmdReadTemperature2;
      mpcMsgRead.cRxData.muiLen = 2;
      mpcMsgRead.cTxData.muiLen = 1;
      mpcMsgRead.cTxData.mpu8Data[0] = LSM303DLHC_MAG_TEMP_OUT_H_M;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadTemperature2:
      menCmd     = enCmdIdle;
      for (uint8 lui8t = 0; lui8t < mpcMsgRead.cRxData.muiLen; lui8t++)
      {
        u8RamMirrow[LSM303DLHC_MAG_TEMP_OUT_H_M + lui8t] = mpcMsgRead.cRxData.mpu8Data[lui8t];
      }
      break;

    default:
      break;
  }

  cLSM303DLHC_MAG_vSetStartRequest();
}


void cLSM303DLHC_MAG::vTick10ms(void)
{
}


void cLSM303DLHC_MAG::vSetup()
{
  mbReadAll             = true;
  mbWriteConfig         = false;
  mbReadMag             = false;
  mbReadTemperature     = false;

  mStatus.IsStartRequested = 1;
}


void cLSM303DLHC_MAG::vSetConfig(uint8 lui8TempEn, uint8 lui8DataOutputRate, uint8 lui8Gain, uint8 lui8Mode)
{
  u8RamMirrow[LSM303DLHC_MAG_CRA_REG_M] = lui8TempEn | lui8DataOutputRate;
  u8RamMirrow[LSM303DLHC_MAG_CRB_REG_M] = lui8Gain;
  u8RamMirrow[LSM303DLHC_MAG_MR_REG_M]  = lui8Mode;

  mbWriteConfig = True;
  mStatus.IsStartRequested = 1;
}


void cLSM303DLHC_MAG::vReadMag()
{
  mbReadMag = true;
  mStatus.IsStartRequested = 1;
}

void cLSM303DLHC_MAG::vReadTemperature()
{
  mbReadTemperature = true;
  mStatus.IsStartRequested = 1;
}

