#include "HTU21D.h"

cHTU21D::cHTU21D(cI2cMaster* lpcI2C, uint8 lui8Adr)
{
  mui16Temperature_digit = 0;
  mui16Humiditiy_digit   = 0;
  mui8User_digit         = 0;

  if (lui8Adr < HTU21D_DEFAULT_ADDRESS)
  {
    lui8Adr <<= 1;
  }

  mI2C   = lpcI2C;
  mAdr   = lui8Adr;

  mpcMsgRead.vMemAlloc(1, 3);
  mpcMsgWrite.vMemAlloc(2, 0);

  menCmd         = enCmdIdle;
  mStatus.IsInit = false;

  i8Setup();

  mStatus.IsStartRequested = (mbInit |
                              mbWriteReset |
                              mbReadUser |
                              mbReadTemperatureHold |
                              mbReadTemperatureNoHold |
                              mbReadHumidityHold |
                              mbReadHumidityNoHold);

  mI2C->vAddSlave((cComNode*)this);
}

cHTU21D::~cHTU21D()
{
  mpcMsgRead.vMemFree();
  mpcMsgWrite.vMemFree();
}

void cHTU21D::vComError(cComNode::tenError lenError, cComNode::tenState lenState)
{
  UNUSED(lenError);
  UNUSED(lenState);
  mStatus.IsStartRequested = false;
  if (lenError != enErNACK)
  {
    menCmd         = enCmdIdle;
    mStatus.IsInit = false;
    i8Setup();
  }
  else
  {
    if ((menCmd != enCmdReadHumidityNoHold3) && (menCmd != enCmdReadTemperatureNoHold3))
    {
      menCmd    = enCmdIdle;
      mStatus.IsInit  = false;
      i8Setup();
    }
  }
}

void cHTU21D::vComStart(cComNode::tenEvent lenEvent)
{
  UNUSED(lenEvent);
  if (menCmd == enCmdIdle)
  {
    if (mbInit)
    {
      mbInit = false;
      menCmd = enCmdInit;
      vComDone();
    }
    else if (mbWriteReset)
    {
      mbWriteReset = false;
      menCmd = enCmdWriteReset;
      vComDone();
    }
    else if (mbReadUser)
    {
      mbReadUser = false;
      menCmd = enCmdReadUser;
      vComDone();
    }
    else if (mbReadTemperatureHold)
    {
      mbReadTemperatureHold = false;
      menCmd = enCmdReadTemperatureHold;
      vComDone();
    }
    else if (mbReadTemperatureNoHold)
    {
      mbReadTemperatureNoHold = false;
      menCmd = enCmdReadTemperatureNoHold;
      vComDone();
    }
    else if (mbReadHumidityHold)
    {
      mbReadHumidityHold = false;
      menCmd = enCmdReadHumidityHold;
      vComDone();
    }
    else if (mbReadHumidityNoHold)
    {
      mbReadHumidityNoHold = false;
      menCmd = enCmdReadHumidityNoHold;
      vComDone();
    }
  }

  mStatus.IsStartRequested = (mbInit |
                              mbWriteReset |
                              mbReadUser |
                              mbReadTemperatureHold |
                              mbReadTemperatureNoHold |
                              mbReadHumidityHold |
                              mbReadHumidityNoHold);
}

void cHTU21D::vComDone()
{
  uint16 lui16Temp;

  lui16Temp = mpcMsgRead.cRxData.mpu8Data[1] + (mpcMsgRead.cRxData.mpu8Data[0] << 8);

  mpcMsgRead.cTxData.muiLen = 1;
  mpcMsgRead.cRxData.muiLen = 3;
  mpcMsgWrite.cTxData.muiLen = 2;

  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdInit:
      mIdx    = 0;
      menCmd = enCmdInit2;
      //Write Reset
      mpcMsgWrite.cTxData.mpu8Data[0] = HTU21D_RESET;
      mpcMsgWrite.cTxData.mpu8Data[1] = 0;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdInit2:
       mStatus.IsInit = true;
       menCmd    = enCmdIdle;
      break;

    case enCmdWriteReset:
      mIdx    = 0;
      menCmd = enCmdWriteReset2;
      mpcMsgWrite.cTxData.mpu8Data[0] = HTU21D_RESET;
      mpcMsgWrite.cTxData.mpu8Data[1] = 0;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdWriteReset2:
      menCmd     = enCmdIdle;
      break;

    case enCmdReadUser:
      mIdx    = 0;
      menCmd = enCmdReadUser2;
      mpcMsgRead.cRxData.muiLen = 1;
      mpcMsgRead.cTxData.mpu8Data[0] = HTU21D_READ_USER_REGISTER;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadUser2:
      menCmd          = enCmdIdle;
      mui8User_digit = mpcMsgRead.cRxData.mpu8Data[0];
      break;

    case enCmdReadTemperatureHold:
      mIdx    = 0;
      menCmd = enCmdReadTemperatureHold2;
      mpcMsgRead.cTxData.mpu8Data[0] = HTU21D_RA_TEMPERATURE_HOLD;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadTemperatureHold2:
      menCmd     = enCmdIdle;
      mui16Temperature_digit = lui16Temp;
      break;

    case enCmdReadHumidityHold:
      mIdx    = 0;
      menCmd = enCmdReadHumidityHold2;
      mpcMsgRead.cTxData.mpu8Data[0] = HTU21D_RA_HUMIDITY_HOLD;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadHumidityHold2:
      menCmd     = enCmdIdle;
      mui16Humiditiy_digit = lui16Temp;
      break;

    case enCmdReadTemperatureNoHold:
      mIdx    = 0;
      menCmd     = enCmdReadTemperatureNoHold2;
      mpcMsgRead.cRxData.muiLen = 0;
      mpcMsgRead.cTxData.mpu8Data[0] = HTU21D_RA_TEMPERATURE_NOHOLD;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadTemperatureNoHold2:
      menCmd     = enCmdReadTemperatureNoHold3;
      mpcMsgRead.cTxData.muiLen = 0;
      mpcMsgRead.cRxData.muiLen = 3;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadTemperatureNoHold3:
      menCmd     = enCmdIdle;
      mui16Temperature_digit = lui16Temp;
      break;

    case enCmdReadHumidityNoHold:
      mIdx    = 0;
      menCmd     = enCmdReadHumidityNoHold2;
      mpcMsgRead.cRxData.muiLen = 0;
      mpcMsgRead.cTxData.mpu8Data[0] = HTU21D_RA_HUMIDITY_NOHOLD;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadHumidityNoHold2:
      menCmd     = enCmdReadHumidityNoHold3;
      mpcMsgRead.cTxData.muiLen = 0;
      mpcMsgRead.cRxData.muiLen = 3;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadHumidityNoHold3:
      menCmd     = enCmdIdle;
      mui16Humiditiy_digit = lui16Temp;
      break;

    default:
      break;
  }
  mStatus.IsStartRequested = (mbInit |
                              mbWriteReset |
                              mbReadUser |
                              mbReadTemperatureHold |
                              mbReadTemperatureNoHold |
                              mbReadHumidityHold |
                              mbReadHumidityNoHold);
}



void cHTU21D::vTick10ms(void)
{
  switch (menCmd)
  {
    case enCmdReadHumidityNoHold3:
      menCmd     = enCmdReadHumidityNoHold2;
      vComDone();
      break;
    case enCmdReadTemperatureNoHold3:
      menCmd     = enCmdReadTemperatureNoHold2;
      vComDone();
      break;
    default:
      break;
  }
}


int8 cHTU21D::i8Setup()
{
  int li8Error = FRET_OK;

  mbInit                = true;
  mbReadUser            = true;
  mbReadTemperatureHold = true;
  mbReadHumidityHold    = true;

  mbWriteReset            = false;
  mbReadTemperatureNoHold = false;
  mbReadHumidityNoHold    = false;


  mStatus.IsStartRequested = 1;

  return li8Error;
}


int8 cHTU21D::i8ReadUser_digit()
{
  mbReadUser = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}

int8 cHTU21D::i8ReadTemperatureHold_digit()
{
  mbReadTemperatureHold = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}


int8 cHTU21D::i8ReadHumidityHold_digit()
{
  mbReadHumidityHold = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}


int8 cHTU21D::i8ReadTemperatureNoHold_digit()
{
  mbReadTemperatureNoHold = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}


int8 cHTU21D::i8ReadHumidityNoHold_digit()
{
  mbReadHumidityNoHold = true;
  mStatus.IsStartRequested = 1;

  return FRET_OK;
}



int32 cHTU21D::i32CalcTemperature()
{
  //return (((mui16Temperature_digit*17572) / 100) >> 16) - 47;
  return ((mui16Temperature_digit*17572) >> 16) - 4685;
}

int32 cHTU21D::i32CalcHumidity()
{
  return ((mui16Humiditiy_digit*12500) >> 16) - 600;
}



