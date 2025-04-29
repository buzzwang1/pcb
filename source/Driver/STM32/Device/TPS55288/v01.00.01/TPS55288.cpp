#include "TPS55288.h"

cTPS55288::cTPS55288(cI2cMaster* lpcI2C, uint8 lui8Adr)
{

  if (lui8Adr < TPS55288_I2C_ADDRESS_CONF_0)
  {
    lui8Adr <<= 1;
  }


  mI2C   = lpcI2C;
  mAdr   = lui8Adr;

  mpcMsgRead.vMemAlloc(1, 1);
  mpcMsgWrite.vMemAlloc(2, 0);

  menCmd            = enCmdIdle;
  mStatus.IsInit    = false;

  //i8Setup(TPS55288_Mode_ShuntAndBusVoltageContinuous,
  //        TPS55288_ShuntConversionTime_1100uS,
  //        TPS55288_BusConversionTime_1100uS,
  //        TPS55288_AveragingMode1,
  //        True, True, True);
  i8Setup();

  mStatus.IsStartRequested = mbInit;

  mI2C->vAddSlave((cComNode*)this);
}

#if _WIN32
  cTPS55288::~cTPS55288()
  {
    mpcMsgRead.vMemFree();
    mpcMsgWrite.vMemFree();
  }
#endif // _WIN32


void cTPS55288::vComStart(cComNode::tenEvent lenEvent)
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
    else if (mbCmdSetCurrent)
    {
      mbCmdSetCurrent = false;
      menCmd = enCmdSetCurrent;
      vComDone();
    }
    else if (mbCmdSetVoltage)
    {
      mbCmdSetVoltage = false;
      menCmd = enCmdSetVoltage;
      vComDone();
    }
    else if (mbCmdSetMode1)
    {
      mbCmdSetMode1 = false;
      menCmd = enCmdSetMode1;
      vComDone();
    }

    mStatus.IsStartRequested = mbInit | mbCmdSetCurrent | mbCmdSetVoltage | mbCmdSetMode1;
  }
}

void cTPS55288::vComDone()
{
  const u8 lu8RegList[] = { TPS55288_VOUT_LOW,
                            TPS55288_VOUT_HIGH,
                            TPS55288_IOUT_LIMIT,
                            TPS55288_VOUT_SR,
                            TPS55288_VOUT_FS,
                            TPS55288_CDC,
                            TPS55288_MODE,
                            TPS55288_STATUS};
  u8 lui8Temp;
  lui8Temp = mpcMsgRead.cRxData.mpu8Data[0];

  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdInit:
      mRegIdx = 0;
      menCmd = enCmdInit2;
      vStartReadReg(lu8RegList[mRegIdx]);
      break;

    case enCmdInit2:
      mRegs.u8Regs[mRegIdx] = lui8Temp;
      mRegIdx++;
      if (mRegIdx > sizeof(lu8RegList))
      {
        mStatus.IsInit = true;
        menCmd = enCmdIdle;

        // Internal feedback ratio
        //   00b = Set internal feedback ratio to 1,129mV / 0.2256 =>   5mV/digit (45mV =>  200mV...5324mV
        //   01b = Set internal feedback ratio to 1,129mV / 0.1128 =>  10mV/digit (45mV =>  400mV..10429mV
        //   10b = Set internal feedback ratio to 1,129mV / 0.0752 =>  15mV/digit (45mV =>  600mV..15973mV
        //   11b = Set internal feedback ratio to 1,129mV / 0.0564 =>  20mV/digit (45mV =>  800mV..21298mV

        // U = (45mV + x * 1,129mV) / Ration

        u16 lu16Zwerg = mRegs.u8Regs[0] + (mRegs.u8Regs[1] << 8);
        switch (mRegs.stRegs.stRegFS.IntFb)
        {
          case 0:
            {
              float lfZwerg = (((float)lu16Zwerg * 1.129f + 45.0f) / 0.2256f);
              mu16Vout_mV = lfZwerg;
            }
            break;
          case 1:
            {
              float lfZwerg = (((float)lu16Zwerg * 1.129f + 45.0f) / 0.1128f);
              mu16Vout_mV = lfZwerg;
            }
            break;
          case 2:
            {
              float lfZwerg = (((float)lu16Zwerg * 1.129f + 45.0f) / 0.0752f);
              mu16Vout_mV = lfZwerg;
            }
            break;
          case 3:
            {
              float lfZwerg = (((float)lu16Zwerg * 1.129f + 45.0f) / 0.0564f);
              mu16Vout_mV = lfZwerg;
            }
            break;
        }

        mu16ILim_mA = mRegs.stRegs.stIOutLimit.Limit * 50;
      }
      else
      {
        vStartReadReg(lu8RegList[mRegIdx]);
      }
      break;

    case enCmdInit3:
      menCmd = enCmdInit4;
      break;

    case enCmdInit4:
      menCmd = enCmdInit5;
      break;

    case enCmdInit5:
      menCmd = enCmdIdle;
      break;

    case enCmdSetVoltage:
      menCmd = enCmdSetVoltage2;
      vStartWriteReg(TPS55288_VOUT_LOW);
      break;
    case enCmdSetVoltage2:
      menCmd     = enCmdSetVoltage3;
      vStartWriteReg(TPS55288_VOUT_HIGH);
      break;
    case enCmdSetVoltage3:
      menCmd = enCmdSetVoltage4;
      vStartWriteReg(TPS55288_MODE);
      break;
    case enCmdSetVoltage4:
      menCmd = enCmdIdle;
      break;

    case enCmdSetCurrent:
      menCmd = enCmdSetCurrent2;
      vStartWriteReg(TPS55288_IOUT_LIMIT);
      break;
    case enCmdSetCurrent2:
      menCmd = enCmdIdle;
      break;

    case enCmdSetMode1:
      menCmd = enCmdSetMode2;
      vStartWriteReg(TPS55288_MODE);
      break;
    case enCmdSetMode2:
      menCmd = enCmdIdle;
      break;

    default:
      break;
  }
}


int8 cTPS55288::i8Setup()
{
  int li8Error = FRET_OK;
  mbInit = true;
  mStatus.IsStartRequested = 1;
  return li8Error;
}


void cTPS55288::vStartWriteReg(u8 lu8Idx)
{
  u8 lu8Value = mRegs.u8Regs[lu8Idx];

  mpcMsgWrite.cTxData.mpu8Data[0] = lu8Idx;
  mpcMsgWrite.cTxData.mpu8Data[1] = lu8Value;
  mpcMsgWrite.vStart();
  mI2C->vStartMsg(&mpcMsgWrite);
}

void cTPS55288::vStartReadReg(u8 lu8Idx)
{
  mpcMsgRead.cTxData.mpu8Data[0] = lu8Idx;
  mpcMsgRead.vStart();
  mI2C->vStartMsg(&mpcMsgRead);
}



int8 cTPS55288::i8SetVoltage_mV(u16 lu16Value_mV, bool lbForce)
{
  if (!mRegs.stRegs.stRegMODE.OE) lbForce = True;
  if ((lu16Value_mV == mu16Vout_mV) &&  (!lbForce)) return FRET_OK;
  mu16Vout_mV = lu16Value_mV;

  // Internal feedback ratio
  //   00b = Set internal feedback ratio to 1,129mV / 0.2256 =>   5mV/digit (45mV =>  200mV...5324mV
  //   01b = Set internal feedback ratio to 1,129mV / 0.1128 =>  10mV/digit (45mV =>  400mV..10429mV
  //   10b = Set internal feedback ratio to 1,129mV / 0.0752 =>  15mV/digit (45mV =>  600mV..15973mV
  //   11b = Set internal feedback ratio to 1,129mV / 0.0564 =>  20mV/digit (45mV =>  800mV..21298mV

  // U = (45mV + x * 1,129mV) / Ration

  u16 lu16Zwerg = 0;
  switch (mRegs.stRegs.stRegFS.IntFb)
  {
    case 0: 
      {
        float lfZwerg = (((float)lu16Value_mV * 0.2256f) - 45.0f) / 1.129f;
        if (lfZwerg > 0) lu16Zwerg = lfZwerg;
      }
      break;
    case 1: 
      {
        float lfZwerg = (((float)lu16Value_mV * 0.1128f) - 45.0f) / 1.129f;
        if (lfZwerg > 0) lu16Zwerg = lfZwerg;
      }
      break;
    case 2: 
      {
        float lfZwerg = (((float)lu16Value_mV * 0.0752f) - 45.0f) / 1.129f;
        if (lfZwerg > 0) lu16Zwerg = lfZwerg;
      }
      break;
    case 3: 
      {
        float lfZwerg = (((float)lu16Value_mV * 0.0564f) - 45.0f) / 1.129f;
        if (lfZwerg > 0) lu16Zwerg = lfZwerg;
      }
      break;
  }

  if (lu16Zwerg > 1023) lu16Zwerg = 1023;

  mRegs.stRegs.u8VoutL = (u8)lu16Zwerg;
  mRegs.stRegs.u8VoutH = (u8)(lu16Zwerg >> 8);

  mRegs.stRegs.stRegMODE.OE = 1;
  mStatus.IsStartRequested = 1;
  mbCmdSetVoltage = True;
  return FRET_OK;
}

int8 cTPS55288::i8SetCurrent_mA(u16 lu16ILim_mA, bool lbForce)
{
  u16 lILim;
  // 10mOhm at VISP-VISN
  // ILIM =   0 =>    0 mA
  // ILIM =   1 =>   50 mA
  // ILIM =   2 =>  100 mA
  // ILIM =   3 =>  150 mA
  // ILIM =   4 =>  200 mA
  // ILIM =   5 =>  250 mA
  // ILIM = 127 => 6350 mA

  if ((lu16ILim_mA == mu16ILim_mA) && (!lbForce)) return FRET_OK;
  mu16ILim_mA = lu16ILim_mA;

  lILim = ((lu16ILim_mA + 49) / 50);

  mRegs.stRegs.stIOutLimit.En = 1;
  mRegs.stRegs.stIOutLimit.Limit = lILim;

  mStatus.IsStartRequested = 1;
  mbCmdSetCurrent = True;
  return FRET_OK;
}


int8 cTPS55288::i8SetMode(u8 lu8Value)
{
  //vSetReg(TPS55288_CTL1, lu8Value);
  mRegs.u8Regs[TPS55288_MODE] = lu8Value;

  mStatus.IsStartRequested = 1;  
  mbCmdSetMode1 = True;
  return FRET_OK;
}

