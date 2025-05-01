#include "LSM303DLHC_ACC.h"

#define cLSM303DLHC_ACC_vSetStartRequest()  mStatus.IsStartRequested = (uint8)(mbReadAll |\
                                                                               mbWriteControl |\
                                                                               mbReadAcc)

cLSM303DLHC_ACC::cLSM303DLHC_ACC(cI2cMaster* lpcI2C)
{
  /*mui16Temperature_digit = 0;
  mui16Humiditiy_digit   = 0;
  mui8User_digit         = 0;*/

  mI2C   = lpcI2C;
  mAdr   = LSM303DLHC_ACC_I2C_ADDRESS_DEFAULT;

  mpcMsgRead.vMemAlloc(1, 8);
  mpcMsgWrite.vMemAlloc(2, 0);

  menCmd      = enCmdIdle;
  mStatus.IsInit    = false;

  vSetup();

  cLSM303DLHC_ACC_vSetStartRequest();

  mI2C->vAddSlave((cComNode*)this);
}

cLSM303DLHC_ACC::~cLSM303DLHC_ACC()
{
  //mpcMsgRead.vMemFree();
  //mpcMsgWrite.vMemFree();
}

void cLSM303DLHC_ACC::vComError(cComNode::tenError lenError, cComNode::tenState lenState)
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

void cLSM303DLHC_ACC::vComStart(cComNode::tenEvent lenEvent)
{
  UNUSED(lenEvent);

  if (IsError())
  {
    mStatus.IsError  = false;
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
    else if (mbWriteControl)
    {
      mbWriteControl = false;
      menCmd = enCmdWriteControl1;
      vComDone();
    }
    else if (mbReadAcc)
    {
      mbReadAcc = false;
      menCmd = enCmdReadAcc;
      vComDone();
    }
  }

  cLSM303DLHC_ACC_vSetStartRequest();
}

void cLSM303DLHC_ACC::vComDone()
{
  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdReadAll:
      mIdx    = 0;

      mpcMsgRead.cTxData.muiLen = 1;
      mpcMsgRead.cRxData.muiLen = 1;

      menCmd = enCmdReadAll2;
      mpcMsgRead.cTxData.mpu8Data[0] = LSM303DLHC_ACC_CTRL_REG1_A;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;

    case enCmdReadAll2:
      if (mIdx < sizeof(u8RamMirrow))
      {
        u8RamMirrow[mIdx] = mpcMsgRead.cRxData.mpu8Data[0];

        mIdx++;
        if (mIdx < sizeof(u8RamMirrow))
        {
          mpcMsgRead.cTxData.mpu8Data[0]++;
          mpcMsgRead.vStart();
          mI2C->vStartMsg(&mpcMsgRead);
        }
        else
        {
          mStatus.IsInit = true;
          menCmd    = enCmdIdle;

        }
      }
      break;

    case enCmdWriteControl1:
      mpcMsgWrite.cTxData.muiLen = 2;
      mpcMsgWrite.cRxData.muiLen = 0;
      mIdx   = LSM303DLHC_ACC_CTRL_REG1_A;
      menCmd = enCmdWriteControl2;

      mpcMsgWrite.cTxData.mpu8Data[0] = mIdx;
      mpcMsgWrite.cTxData.mpu8Data[1] = u8RamMirrow[uReg2Idx(mIdx)];
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdWriteControl2:
      mIdx++;
      if (mIdx <= LSM303DLHC_ACC_CTRL_REG6_A)
      {
        mpcMsgWrite.cTxData.mpu8Data[0] = mIdx;
        mpcMsgWrite.cTxData.mpu8Data[1] = u8RamMirrow[uReg2Idx(mIdx)];
        mpcMsgWrite.vStart();
        mI2C->vStartMsg(&mpcMsgWrite);
      }
      else
      {
        menCmd     = enCmdIdle;
      }
      break;


    case enCmdReadAcc:
      menCmd = enCmdReadAcc2;
      mpcMsgRead.cTxData.muiLen = 1;
      mpcMsgRead.cRxData.muiLen = 6;

      /* In order to read multiple bytes, it is necessary to assert the
         most significant bit of the sub-address field. In other words,
         SUB(7) must be equal to 1 while SUB(6-0) represents the address
         of the first register to be read. */
      mpcMsgRead.cTxData.mpu8Data[0] = 0x80 | LSM303DLHC_ACC_OUT_X_L_A;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadAcc2:
      menCmd = enCmdIdle;
      for (uint8 lui8t = 0; lui8t < mpcMsgRead.cRxData.muiLen; lui8t++)
      {
        u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_OUT_X_L_A + lui8t)] = mpcMsgRead.cRxData.mpu8Data[lui8t];
      }
      break;

    default:
      break;
  }

  cLSM303DLHC_ACC_vSetStartRequest();
}


void cLSM303DLHC_ACC::vTick10ms(void)
{
}


void cLSM303DLHC_ACC::vSetup()
{
  mbReadAll             = true;
  mbWriteControl        = false;
  mbReadAcc             = false;

  mStatus.IsStartRequested = 1;
}


void cLSM303DLHC_ACC::vSetControl(uint8 lui8Reg1, uint8 lui8Reg2, uint8 lui8Reg3, uint8 lui8Reg4, uint8 lui8Reg5, uint8 lui8Reg6)
{
  u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_CTRL_REG1_A)] = lui8Reg1;
  u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_CTRL_REG2_A)] = lui8Reg2;
  u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_CTRL_REG3_A)] = lui8Reg3;
  u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_CTRL_REG4_A)] = lui8Reg4;
  u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_CTRL_REG5_A)] = lui8Reg5;
  u8RamMirrow[uReg2Idx(LSM303DLHC_ACC_CTRL_REG6_A)] = lui8Reg6;

  mbWriteControl = True;
  mStatus.IsStartRequested = 1;
}


void cLSM303DLHC_ACC::vReadAcc()
{
  mbReadAcc = true;
  mStatus.IsStartRequested = 1;
}



