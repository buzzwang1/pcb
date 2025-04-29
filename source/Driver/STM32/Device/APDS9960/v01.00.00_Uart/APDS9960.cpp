#include "APDS9960.h"


#define cAPDS9960_vSetStartRequest()  mStatus.IsStartRequested = (uint8)(mbInit |\
                                                                 mbSetMode |\
                                                                 mbReadAmbientLight |\
                                                                 mbSetControl |\
                                                                 mbReadProximitySensor |\
                                                                 mbSetupGestureSensor |\
                                                                 mbReadGestureSensor)

cAPDS9960::cAPDS9960(cI2cMaster* lpcI2C, uint8 lui8Adr, cUart* lcUart)
{
  mI2C   = lpcI2C;
  mAdr   = lui8Adr;

  mpcMsgRead.vMemAlloc(1, 10);
  mpcMsgReadData.vMemAlloc(1, 0);
  mpcMsgWrite.vMemAlloc(2, 0);

  menCmd      = enCmdIdle;
  mStatus.IsInit    = false;

  i8Setup();

  cAPDS9960_vSetStartRequest();

  mcUart = lcUart;

  mI2C->vAddSlave((cComNode*)this);
}

cAPDS9960::~cAPDS9960()
{
  mpcMsgRead.vMemFree();
  mpcMsgWrite.vMemFree();
}

void cAPDS9960::vComError(cComNode::tenError lenError, cComNode::tenState lenState)
{
  UNUSED(lenError);
  UNUSED(lenState);
  mStatus.IsStartRequested = false;
  menCmd     = enCmdIdle;
  mStatus.IsInit  = false;
  i8Setup();
}

void cAPDS9960::vComStart(cComNode::tenEvent lenEvent)
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
    else if (mbReadAmbientLight)
    {
      mbReadAmbientLight = false;
      menCmd = enCmdReadAmbientLight;
      vComDone();
    }
    else if (mbReadProximitySensor)
    {
      mbReadProximitySensor = false;
      menCmd = enCmdReadProximitySensor;
      vComDone();
    }
    else if (mbSetupGestureSensor)
    {
      mbSetupGestureSensor = false;
      menCmd = enCmdSetupGestureSensor;
      vComDone();
    }
    else if (mbReadGestureSensor)
    {
      mbReadGestureSensor = false;
      menCmd = enCmdReadGestureSensor;
      vComDone();
    }
    else if (mbSetControl)
    {
      mbSetControl = false;
      menCmd = enCmdSetControl;
      vComDone();
    }
    else if (mbSetMode)
    {
      mbSetMode = false;
      menCmd = enCmdSetMode;
      vComDone();
    }
  }

  cAPDS9960_vSetStartRequest();
}

void cAPDS9960::vComDone()
{
  uint8  lui8MsgSend;
  mpcMsgRead.cTxData.muiLen = 1;
  mpcMsgRead.cRxData.muiLen = 1;
  mpcMsgWrite.cTxData.muiLen = 2;

  switch (menCmd)
  {
    case enCmdIdle:
      break;

    case enCmdInit:
      mIdx    = 0;
      menCmd = enCmdInit2;
      //Read ID
      mpcMsgRead.cTxData.mpu8Data[0] = nAPDS9960_ID;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdInit2:

      if ((mpcMsgRead.cRxData.mpu8Data[0] == nAPDS9960_ID_1) ||
          (mpcMsgRead.cRxData.mpu8Data[0] == nAPDS9960_ID_2))
      {
        stRegSet.ui8ID = mpcMsgRead.cRxData.mpu8Data[0];
        menCmd         = enCmdInit3;

        lui8MsgSend = 0;
        do
        {
          if (mIdx < sizeof(APDS9960_mpaui8RegSetDefault))
          {
            if ((APDS9960_mpaui8RegSetDefault[mIdx+1] & nAPDS9960_RWI) == nAPDS9960_RWI)
            {
              lui8MsgSend = 1;

              mpcMsgWrite.cTxData.mpu8Data[0] = APDS9960_mpaui8RegSetDefault[mIdx];
              mIdx++;
              mIdx++;
              mpcMsgWrite.cTxData.mpu8Data[1] = APDS9960_mpaui8RegSetDefault[mIdx];
              mIdx++;
              mpcMsgWrite.vStart();
              mI2C->vStartMsg(&mpcMsgWrite);
            }
            else
            {
              mIdx += 3;
            }
          }
          else
          {
            lui8MsgSend = 1;
            mStatus.IsInit = true;
            menCmd    = enCmdIdle;
          }
        } while (lui8MsgSend == 0);
      }
      else
      {
        menCmd    = enCmdInit;
      }
      break;
    case enCmdInit3:
      lui8MsgSend = 0;
      do
      {
        if (mIdx < sizeof(APDS9960_mpaui8RegSetDefault))
        {
          if ((APDS9960_mpaui8RegSetDefault[mIdx+1] & nAPDS9960_RWI) == nAPDS9960_RWI)
          {
            lui8MsgSend = 1;

            mpcMsgWrite.cTxData.mpu8Data[0] = APDS9960_mpaui8RegSetDefault[mIdx];
            mIdx++;
            mIdx++;
            mpcMsgWrite.cTxData.mpu8Data[1] = APDS9960_mpaui8RegSetDefault[mIdx];
            mIdx++;
            mpcMsgWrite.vStart();
            mI2C->vStartMsg(&mpcMsgWrite);
          }
          else
          {
            mIdx += 3;
          }
        }
        else
        {
          lui8MsgSend = 1;
          mStatus.IsInit = true;
          menCmd    = enCmdIdle;
        }
      } while (lui8MsgSend == 0);
      break;


    case enCmdSetMode:
      menCmd = enCmdSetMode2;
      mpcMsgWrite.cTxData.mpu8Data[0] = nAPDS9960_ENABLE;
      mpcMsgWrite.cTxData.mpu8Data[1] = this->stRegSet.ui8ENABLE;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdSetMode2:
      menCmd     = enCmdIdle;
      break;

    case enCmdSetControl:
      menCmd = enCmdSetControl2;
      mpcMsgWrite.cTxData.mpu8Data[0] = nAPDS9960_CONTROL;
      mpcMsgWrite.cTxData.mpu8Data[1] = this->stRegSet.ui8CONTROL;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdSetControl2:
      menCmd     = enCmdIdle;
      break;

    case enCmdReadAmbientLight:
      // from 0x93 read 9 bytes Status, CL, CH, RL, RD, GL, GH ,BL, BH
      menCmd = enCmdReadAmbientLight2;
      mpcMsgRead.cTxData.muiLen = 1;
      mpcMsgRead.cRxData.muiLen = 9;
      mpcMsgRead.cTxData.mpu8Data[0] = nAPDS9960_STATUS;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;

    case enCmdReadAmbientLight2:
      {
        uint8* lpui8Dest = &(this->stRegSet.ui8STATUS);
        for (mIdx = 0; mIdx < 9; mIdx++)
        {
          lpui8Dest[mIdx] = mpcMsgRead.cRxData.mpu8Data[mIdx];
        }
        menCmd = enCmdIdle;
      }
      break;


    case enCmdReadProximitySensor:
      menCmd = enCmdReadProximitySensor2;
      mpcMsgRead.cTxData.mpu8Data[0] = nAPDS9960_STATUS;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;

    case enCmdReadProximitySensor2:
      this->stRegSet.ui8STATUS = mpcMsgRead.cRxData.mpu8Data[0];

      /* Proximity Valid.Indicates that a proximity cycle has completed since PEN was asserted or
         since PDATA was last read.A read of PDATA automatically clears PVALID*/
      if (this->stRegSet.ui8STATUS & nAPDS9960_PVALID)
      {
        menCmd = enCmdReadProximitySensor3;
        mpcMsgRead.cTxData.mpu8Data[0] = nAPDS9960_PDATA;
        mpcMsgRead.vStart();
        mI2C->vStartMsg(&mpcMsgRead);
      }
      else
      {
        menCmd = enCmdIdle;
      }

      break;
    case enCmdReadProximitySensor3:
      this->stRegSet.ui8PDATA = mpcMsgRead.cRxData.mpu8Data[0];
      menCmd = enCmdIdle;
      break;


    case enCmdSetupGestureSensor:
      menCmd = enCmdSetupGestureSensor2;
      mpcMsgWrite.cTxData.mpu8Data[0] = nAPDS9960_WTIME;
      mpcMsgWrite.cTxData.mpu8Data[1] = this->stRegSet.ui8WTIME;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdSetupGestureSensor2:
      menCmd = enCmdSetupGestureSensor3;
      mpcMsgWrite.cTxData.mpu8Data[0] = nAPDS9960_PPULSE;
      mpcMsgWrite.cTxData.mpu8Data[1] = this->stRegSet.ui8PPULSE;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdSetupGestureSensor3:
      menCmd = enCmdSetupGestureSensor4;
      mpcMsgWrite.cTxData.mpu8Data[0] = nAPDS9960_GCONF2;
      mpcMsgWrite.cTxData.mpu8Data[1] = this->stRegSet.ui8GCONF2;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdSetupGestureSensor4:
      menCmd = enCmdSetupGestureSensor5;
      mpcMsgWrite.cTxData.mpu8Data[0] = nAPDS9960_GCONF4;
      mpcMsgWrite.cTxData.mpu8Data[1] = this->stRegSet.ui8GCONF4;
      mpcMsgWrite.vStart();
      mI2C->vStartMsg(&mpcMsgWrite);
      break;
    case enCmdSetupGestureSensor5:
      menCmd     = enCmdIdle;
      break;



    case enCmdReadGestureSensor:
      // Check if Gesture availalbe
      menCmd = enCmdReadGestureSensor2;
      mpcMsgRead.cTxData.mpu8Data[0] = nAPDS9960_GSTATUS;
      mpcMsgRead.vStart();
      mI2C->vStartMsg(&mpcMsgRead);
      break;
    case enCmdReadGestureSensor2:
      this->stRegSet.ui8GSTATUS = mpcMsgRead.cRxData.mpu8Data[0];

      if (this->stRegSet.ui8GSTATUS & nAPDS9960_GVALID)
      {
        /* If we have valid data, read in FIFO */
        mpcMsgRead.cTxData.mpu8Data[0] = nAPDS9960_GFLVL;
        mpcMsgRead.vStart();
        mI2C->vStartMsg(&mpcMsgRead);
        menCmd = enCmdReadGestureSensor3;
      }
      else
      {
        menCmd = enCmdIdle;
      }
      break;


    case enCmdReadGestureSensor3:
      this->stRegSet.ui8GFLVL = mpcMsgRead.cRxData.mpu8Data[0];

      // If there's stuff in the FIFO, read it into our data block
      if (this->stRegSet.ui8GFLVL == 0)
      {
        menCmd = enCmdIdle;
      }
      else
      {
        this->mstGestureData.index = this->stRegSet.ui8GFLVL;
        this->mstGestureData.total_gestures = this->stRegSet.ui8GFLVL;
        menCmd = enCmdReadGestureSensor4;

        mpcMsgReadData.cRxData.mpu8Data = (uint8*)&(this->mstGestureData.stValues[0]);
        mpcMsgReadData.cRxData.muiLen = 4 * this->stRegSet.ui8GFLVL;
        mpcMsgReadData.cTxData.mpu8Data[0] = nAPDS9960_GFIFO_U;
        mpcMsgReadData.vStart();
        mI2C->vStartMsg(&mpcMsgReadData);
      }
      break;
    case enCmdReadGestureSensor4:
      bProcessGestureData();
      menCmd = enCmdIdle;
      break;

    default:
      break;
  }

  cAPDS9960_vSetStartRequest();
}


/*
void cAPDS9960::vTick10ms(void)
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
}*/


int8 cAPDS9960::i8Setup()
{
  int li8Error = FRET_OK;

  cMemTools::vMemSet((uint8*)(&this->stRegSet), 0, sizeof(this->stRegSet));
  this->stRegSet.ui8ATIME   = 0xFF;
  this->stRegSet.ui8WTIME   = 0xFF;
  this->stRegSet.ui8CONFIG1 = 0x40;
  this->stRegSet.ui8PPULSE  = 0x40;
  this->stRegSet.ui8CONFIG2 = 0x01;
  this->stRegSet.ui8GPULSE  = 0x40;

  this->menGestureSM = enSmStIdle;
  this->menDirection = nAPDS9960_DIR_NONE;

  mbInit           = true;
  mStatus.IsStartRequested = 1;

  return li8Error;
}

// ----------------------- Light Sensor -------------------------

int8 cAPDS9960::isEnabledAmbientLigth()
{
  if ((this->stRegSet.ui8ENABLE & (nAPDS9960_PON | nAPDS9960_AEN)) == (nAPDS9960_PON | nAPDS9960_AEN))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int8 cAPDS9960::i8EnableAmbientLight()
{
  mbSetMode = true;
  mStatus.IsStartRequested = 1;

  this->stRegSet.ui8ENABLE |= (nAPDS9960_PON | nAPDS9960_AEN);
  return FRET_OK;
}


int8 cAPDS9960::i8ReadAmbientLight()
{
  mbReadAmbientLight = true;
  mStatus.IsStartRequested = 1;
  return FRET_OK;
}

bool  cAPDS9960::boGetAmbientLight_Valid()
{

  if (this->stRegSet.ui8STATUS & nAPDS9960_AVALID)
  {
    return true;
  }
  else
  {
    return false;
  }
}

uint16 cAPDS9960::ui16GetAmbientLight_Clear()
{
  return (this->stRegSet.ui8CDATAH << 8) + this->stRegSet.ui8CDATAL;
}

uint16 cAPDS9960::ui16GetAmbientLight_Red()
{
  return (this->stRegSet.ui8RDATAH << 8) + this->stRegSet.ui8RDATAL;
}

uint16 cAPDS9960::ui16GetAmbientLight_Green()
{
  return (this->stRegSet.ui8GDATAH << 8) + this->stRegSet.ui8GDATAL;
}

uint16 cAPDS9960::ui16GetAmbientLight_Blue()
{
  return (this->stRegSet.ui8BDATAH << 8) + this->stRegSet.ui8BDATAL;
}



// ----------------------- Proximity Sensor -------------------------
int8 cAPDS9960::isEnabledProximitySensor()
{
  if ((this->stRegSet.ui8ENABLE & (nAPDS9960_PON | nAPDS9960_PEN)) == (nAPDS9960_PON | nAPDS9960_PEN))
  {
    if (this->stRegSet.ui8ENABLE & nAPDS9960_GEN)
    {
      return 0;
    }
    else
    {
      return 1;
    }
  }
  else
  {
    return 0;
  }
}


int8 cAPDS9960::i8EnableProximitySensor()
{
  mbSetMode = true;
  mbSetControl = true;

  this->stRegSet.ui8ENABLE  |= (nAPDS9960_PON | nAPDS9960_PEN);

  /* Proximity Gain */
  this->stRegSet.ui8CONTROL &= 0b11110011;
  this->stRegSet.ui8CONTROL |= (nAPDS9960_DEFAULT_PGAIN << 2);

  /* Led Drive */
  this->stRegSet.ui8CONTROL &= 0b00111111;
  this->stRegSet.ui8CONTROL |= (nAPDS9960_DEFAULT_LDRIVE << 6);

  mStatus.IsStartRequested = 1;

  return FRET_OK;
}


int8 cAPDS9960::i8ReadProximitySensor()
{
  mbReadProximitySensor = true;
  mStatus.IsStartRequested = 1;
  return FRET_OK;
}

bool  cAPDS9960::boGetProximitySensor_Valid()
{
  if (this->stRegSet.ui8STATUS & 2)
  {
    return true;
  }
  else
  {
    return false;
  }
}

uint8 cAPDS9960::ui8GetProximitySensor()
{
  return this->stRegSet.ui8PDATA;
}

// ----------------------- Gesture Sensor -------------------------
void cAPDS9960::vResetGestureParameters()
{
  mi16Gesture_ud_delta    = 0;
  mi16Gesture_lr_delta    = 0;
  mi16Gesture_ud_count    = 0;
  mi16Gesture_lr_count    = 0;
  mi16Gesture_near_count  = 0;
  mi16Gesture_far_count   = 0;
  mi16Gesture_state       = 0;
  mi16Gesture_motion = nAPDS9960_DIR_NONE;

  this->mstGestureData.index = 0;
  this->mstGestureData.total_gestures = 0;

  this->mstGestureDataTest.index = 0;
}


int8 cAPDS9960::isEnabledGestureSensor()
{
  if ((this->stRegSet.ui8ENABLE & (nAPDS9960_PON | nAPDS9960_PEN | nAPDS9960_GEN)) == (nAPDS9960_PON | nAPDS9960_PEN | nAPDS9960_GEN))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int8 cAPDS9960::i8EnableGestureSensor()
{
  mbSetMode = true;
  mbSetupGestureSensor = true;

  vResetGestureParameters();

  this->stRegSet.ui8ENABLE  |= (nAPDS9960_PON | nAPDS9960_PEN | nAPDS9960_GEN);

  this->stRegSet.ui8PPULSE = nAPDS9960_DEFAULT_GESTURE_PPULSE;
  this->stRegSet.ui8WTIME  = nAPDS9960_WTIME;


  this->stRegSet.ui8GCONF2 = APDS9960_ui8GestureControlReg2(nAPDS9960_GGAIN_2X, nAPDS9960_LED_DRIVE_50MA, nAPDS9960_GWTIME_0MS);

  /* Set Gesture Mode On */
  this->stRegSet.ui8GCONF4 |= 1;


  mStatus.IsStartRequested = 1;

  return FRET_OK;
}


int8 cAPDS9960::i8ReadGestureSensor()
{
  mbReadGestureSensor = true;
  mStatus.IsStartRequested = 1;
  return FRET_OK;
}


bool cAPDS9960::bProcessGestureData()
{
  uint8 lui8t;
  uint8 lui8UDLR;
  uint8 lui8U;
  uint8 lui8D;
  uint8 lui8L;
  uint8 lui8R;
  uint8 lui8OverThres;

  char lszValue[8];

  for (lui8t = 0; lui8t < this->mstGestureData.total_gestures; lui8t++)
  {
    lui8UDLR = 0;
    lui8OverThres = 0;

    lui8U = this->mstGestureData.stValues[lui8t].U;
    lui8D = this->mstGestureData.stValues[lui8t].D;
    lui8L = this->mstGestureData.stValues[lui8t].L;
    lui8R = this->mstGestureData.stValues[lui8t].R;


    cStrTools::uixItoa(lui8U, lszValue, 16);
    mcUart->vSend(lszValue);
    mcUart->vSend(" ");
    cStrTools::uixItoa(lui8D, lszValue, 16);
    mcUart->vSend(lszValue);
    mcUart->vSend(" ");
    cStrTools::uixItoa(lui8L, lszValue, 16);
    mcUart->vSend(lszValue);
    mcUart->vSend(" ");
    cStrTools::uixItoa(lui8R, lszValue, 16);
    mcUart->vSend(lszValue);


    if (lui8U > nAPDS9960_GESTURE_THRESHOLD_OUT) {lui8UDLR |= nAPDS9960_GESTURE_U;lui8OverThres++;}
    if (lui8D > nAPDS9960_GESTURE_THRESHOLD_OUT) {lui8UDLR |= nAPDS9960_GESTURE_D;lui8OverThres++;}
    if (lui8L > nAPDS9960_GESTURE_THRESHOLD_OUT) {lui8UDLR |= nAPDS9960_GESTURE_L;lui8OverThres++;}
    if (lui8R > nAPDS9960_GESTURE_THRESHOLD_OUT) {lui8UDLR |= nAPDS9960_GESTURE_R;lui8OverThres++;}


    this->mstGestureDataTest.stValues[mstGestureDataTest.index].U = lui8U;
    this->mstGestureDataTest.stValues[mstGestureDataTest.index].D = lui8D;
    this->mstGestureDataTest.stValues[mstGestureDataTest.index].L = lui8L;
    this->mstGestureDataTest.stValues[mstGestureDataTest.index].R = lui8R;

    this->mstGestureDataTest.index++;
    if (this->mstGestureDataTest.index >= 100) this->mstGestureDataTest.index = 0;

    switch (this->menGestureSM)
    {
      case enSmStIdle:
        if (lui8UDLR != 0)
        {
          if (/*(lui8OverThres == 3) &&*/
              (lui8U > lui8L) &&
              (lui8U > lui8D) &&
              //(lui8U > lui8U) &&
              (lui8U > lui8R))
              this->menGestureSM = enSmStDw1;
          else
          if (/*(lui8OverThres == 3) &&*/
              (lui8D > lui8L) &&
              //(lui8D > lui8D) &&
              (lui8D > lui8U) &&
              (lui8D > lui8R))
              this->menGestureSM = enSmStUw1;
          else
          if (/*(lui8OverThres == 3) &&*/
              //(lui8L > lui8L) &&
              (lui8L > lui8D) &&
              (lui8L > lui8U) &&
              (lui8L > lui8R))
              this->menGestureSM = enSmStRw1;
          else
          if (/*(lui8OverThres == 3) &&*/
              (lui8R > lui8L) &&
              (lui8R > lui8D) &&
              (lui8R > lui8U)) // &&
              //(lui8R > lui8R))
              this->menGestureSM = enSmStLw1;
          else
          if (lui8OverThres == 4) this->menGestureSM = enSmStWait;

        }
        break;
      case enSmStWait:
        this->menDirection = nAPDS9960_DIR_NONE;
        if (lui8UDLR == 0) this->menGestureSM = enSmStIdle;
        break;
      case enSmStWait2:
        if (lui8UDLR == 0) this->menGestureSM = enSmStIdle;
        break;

      // ---------------------------- Rightwards ----------------------

      case enSmStRw1:
        if (lui8OverThres == 0) this->menGestureSM = enSmStWait;
        else
        if (lui8OverThres == 4) this->menGestureSM++;
        break;
      case enSmStRw2:
        if (lui8OverThres != 4)
        {
          if (/*(lui8OverThres == 3) &&*/
              //(lui8L < lui8L) &&
              (lui8L < lui8D) &&
              (lui8L < lui8U) &&
              (lui8L < lui8R))
              {
                this->menGestureSM = enSmStWait2;
                this->menDirection = nAPDS9960_DIR_RIGHT;
              }
          else this->menGestureSM = enSmStWait;
        }
        break;


      // ---------------------------- Leftwards ----------------------
      case enSmStLw1:
        if (lui8OverThres == 0) this->menGestureSM = enSmStWait;
        else
        if (lui8OverThres == 4) this->menGestureSM++;
        break;
      case enSmStLw2:
        if (lui8OverThres != 4)
        {
          if (/*(lui8OverThres == 3) &&*/
              (lui8R < lui8L) &&
              (lui8R < lui8D) &&
              (lui8R < lui8U)) //&&
              //(lui8R < lui8R))
              {
                this->menGestureSM = enSmStWait2;
                this->menDirection = nAPDS9960_DIR_LEFT;
              }
          else this->menGestureSM = enSmStWait;
        }
        break;

      // ---------------------------- Upwards ----------------------
      case enSmStUw1:
        if (lui8OverThres == 0) this->menGestureSM = enSmStWait;
        else
        if (lui8OverThres == 4) this->menGestureSM++;
        break;
      case enSmStUw2:
        if (lui8OverThres != 4)
        {
          if (/*(lui8OverThres == 3) &&*/
              (lui8D < lui8L) &&
              //(lui8D < lui8D) &&
              (lui8D < lui8U) &&
              (lui8D < lui8R))
              {
                this->menGestureSM = enSmStWait2;
                this->menDirection = nAPDS9960_DIR_UP;
              }
          else this->menGestureSM = enSmStWait;
        }
        break;


      // ---------------------------- Downwards ----------------------
      case enSmStDw1:
        if (lui8OverThres == 0) this->menGestureSM = enSmStWait;
        else
        if (lui8OverThres == 4) this->menGestureSM++;
        break;
      case enSmStDw2:
        if (lui8OverThres != 4)
        {
          if (/*(lui8OverThres == 3) &&*/
              (lui8U < lui8L) &&
              (lui8U < lui8D) &&
              //(lui8U < lui8U)) &&
              (lui8U < lui8R))
              {
                this->menGestureSM = enSmStWait2;
                this->menDirection = nAPDS9960_DIR_DOWN;
              }
          else this->menGestureSM = enSmStWait;
        }
        break;

    }

    if (this->menGestureSM == enSmStWait) this->menDirection = nAPDS9960_DIR_NONE;
  }
  return false;
}

bool cAPDS9960::bDecodeGestureData()
{
  return false;
}

