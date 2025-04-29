#include "MP8859.h"

cMP8859::cMP8859(cI2cMaster* lpcI2C, uint8 lui8Adr)
{

  if (lui8Adr < MP8859_I2C_ADDRESS_CONF_0)
  {
    lui8Adr <<= 1;
  }


  mI2C   = lpcI2C;
  mAdr   = lui8Adr;

  mpcMsgRead.vMemAlloc(1, 1);
  mpcMsgWrite.vMemAlloc(2, 0);

  menCmd            = enCmdIdle;
  mStatus.IsInit    = false;

  //i8Setup(MP8859_Mode_ShuntAndBusVoltageContinuous,
  //        MP8859_ShuntConversionTime_1100uS,
  //        MP8859_BusConversionTime_1100uS,
  //        MP8859_AveragingMode1,
  //        True, True, True);
  i8Setup();

  mStatus.IsStartRequested = mbInit;

  mI2C->vAddSlave((cComNode*)this);
}

#if _WIN32
  cMP8859::~cMP8859()
  {
    mpcMsgRead.vMemFree();
    mpcMsgWrite.vMemFree();
  }
#endif // _WIN32


void cMP8859::vComStart(cComNode::tenEvent lenEvent)
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
    else if (mbCmdSetCtrl1)
    {
      mbCmdSetCtrl1 = false;
      menCmd = enCmdSetCtrl1;
      vComDone();
    }

    mStatus.IsStartRequested = mbInit | mbCmdSetCurrent | mbCmdSetVoltage | mbCmdSetCtrl1;
  }
}

void cMP8859::vComDone()
{
  const u8 lu8RegList[] = { MP8859_VOUT_LOW,
                            MP8859_VOUT_HIGH,
                            MP8859_VOUT_GO,
                            MP8859_IOUT_LIM,
                            MP8859_CTL1,
                            MP8859_CTL2,
                            MP8859_STATUS,
                            MP8859_IRQ,
                            MP8859_MASK,
                            MP8859_ID1,
                            MP8859_MFR_ID,
                            MP8859_DEV_ID,
                            MP8859_IC_REV};
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
      vSetReg(lu8RegList[mRegIdx], lui8Temp);
      mRegIdx++;
      if (mRegIdx > sizeof(lu8RegList))
      {
        if ((mID    == 0x00) && (mMfrID == 0x09) &&
            (mDevID == 0x58) && (mIcID  == 0x01))
        {
          mStatus.IsInit = true;
        }
        menCmd = enCmdIdle;
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
      vStartWriteReg(MP8859_VOUT_LOW);
      break;
    case enCmdSetVoltage2:
      menCmd     = enCmdSetVoltage3;
      vStartWriteReg(MP8859_VOUT_HIGH);
      break;
    case enCmdSetVoltage3:
      menCmd = enCmdSetVoltage4;
      vStartWriteReg(MP8859_VOUT_GO);
      break;
    case enCmdSetVoltage4:
      menCmd = enCmdIdle;
      break;

    case enCmdSetCurrent:
      menCmd = enCmdSetCurrent2;
      vStartWriteReg(MP8859_IOUT_LIM);
      break;
    case enCmdSetCurrent2:
      menCmd = enCmdIdle;
      break;

    case enCmdSetCtrl1:
      menCmd = enCmdSetCtrl12;
      vStartWriteReg(MP8859_CTL1);
      break;
    case enCmdSetCtrl12:
      menCmd = enCmdIdle;
      break;

    default:
      break;
  }
}


int8 cMP8859::i8Setup()
{
  int li8Error = FRET_OK;
  mbInit = true;
  mStatus.IsStartRequested = 1;
  return li8Error;
}


void cMP8859::vStartWriteReg(u8 lu8Idx)
{
  u8 lu8Value = u8GetReg(lu8Idx);

  mpcMsgWrite.cTxData.mpu8Data[0] = lu8Idx;
  mpcMsgWrite.cTxData.mpu8Data[1] = lu8Value;
  mpcMsgWrite.vStart();
  mI2C->vStartMsg(&mpcMsgWrite);
}

void cMP8859::vStartReadReg(u8 lu8Idx)
{
  mpcMsgRead.cTxData.mpu8Data[0] = lu8Idx;
  mpcMsgRead.vStart();
  mI2C->vStartMsg(&mpcMsgRead);
}


void cMP8859::vSetReg(u8 lu8Idx, u8 lu8Value)
{
  switch (lu8Idx)
  {
    case MP8859_VOUT_LOW:  mu16Vout         = ((u16)lu8Value) + (mu16Vout & 0xF8); break;
    case MP8859_VOUT_HIGH: mu16Vout         = (((u16)lu8Value) << 3) + (mu16Vout & 0x7); break;
    case MP8859_VOUT_GO:   *(u8*)&mstVoutGo = lu8Value; break;
    case MP8859_IOUT_LIM:  mILim            = lu8Value; break;
    case MP8859_CTL1:      *(u8*)&mstCTL1   = lu8Value; break;
    case MP8859_CTL2:      *(u8*)&mstCTL2   = lu8Value; break;
    case MP8859_STATUS:    *(u8*)&mstStatus = lu8Value; break;
    case MP8859_IRQ:       *(u8*)&mstIrq    = lu8Value; break;
    case MP8859_MASK:      *(u8*)&mstIrq    = lu8Value; break;
    case MP8859_ID1:       mID              = lu8Value; break;
    case MP8859_MFR_ID:    mMfrID           = lu8Value; break;
    case MP8859_DEV_ID:    mDevID           = lu8Value; break;
    case MP8859_IC_REV:    mIcID            = lu8Value; break;
  }
}

u8 cMP8859::u8GetReg(u8 lu8Idx)
{
  u8 lu8Ret = 0;
  switch (lu8Idx)
  {
    case MP8859_VOUT_LOW:  lu8Ret = mu16Vout & 0x7;   break;
    case MP8859_VOUT_HIGH: lu8Ret = (mu16Vout >> 3);  break;
    case MP8859_VOUT_GO:   lu8Ret = *(u8*)&mstVoutGo; break;
    case MP8859_IOUT_LIM:  lu8Ret = mILim;            break;
    case MP8859_CTL1:      lu8Ret = *(u8*)&mstCTL1;   break;
    case MP8859_CTL2:      lu8Ret = *(u8*)&mstCTL2;   break;
    case MP8859_STATUS:    lu8Ret = *(u8*)&mstStatus; break;
    case MP8859_IRQ:       lu8Ret = *(u8*)&mstIrq;    break;
    case MP8859_MASK:      lu8Ret = *(u8*)&mstIrq;    break;
    case MP8859_ID1:       lu8Ret = mID;              break;
    case MP8859_MFR_ID:    lu8Ret = mMfrID;           break;
    case MP8859_DEV_ID:    lu8Ret = mDevID;           break;
    case MP8859_IC_REV:    lu8Ret = mIcID;            break;
  }
  return lu8Ret;
}


int8 cMP8859::i8SetVoltage_mV(u16 lu16Value, bool lbForce)
{
  lu16Value /= 10;
  if ((lu16Value == mu16Vout) &&  (!lbForce)) return FRET_OK;
  mu16Vout = lu16Value;
  mstVoutGo.Go_Bit = 1;
  mStatus.IsStartRequested = 1;
  mbCmdSetVoltage = True;
  return FRET_OK;
}

int8 cMP8859::i8SetCurrent_mA(u16 lu16Value, bool lbForce)
{
  u16 lILim;
  // Gemessen 11.10.2023
  // ILIM = 1 =>   0 mA
  // ILIM = 2 =>  22 mA
  // ILIM = 3 =>  90 mA
  // ILIM = 4 => 100 mA
  // ILIM = 5 => 150 mA
  // ILIM = 6 => 200 mA

  if (lu16Value >= 100)
  {
    lILim = (lu16Value / 50);
    if ((lILim * 50) < lu16Value) lILim++;
    lILim += 2;
  }
  else
  {
    if (lu16Value >= 90) lILim = 4;
    else if (lu16Value > 25) lILim = 3;
    else if (lu16Value >  0) lILim = 1;
    else lILim = 0;
  }
  if ((lILim == mILim) &&  (!lbForce)) return FRET_OK;
  mILim = lILim;
  mStatus.IsStartRequested = 1;
  mbCmdSetCurrent = True;
  return FRET_OK;
}


int8 cMP8859::i8SetCtrl1(u8 lu8Value)
{
  mStatus.IsStartRequested = 1;
  vSetReg(MP8859_CTL1, lu8Value);
  mbCmdSetCtrl1 = True;
  return FRET_OK;
}

