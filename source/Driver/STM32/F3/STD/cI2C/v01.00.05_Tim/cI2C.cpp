#include "cI2c.h"


cI2cMaster_ComNodeList::cI2cMaster_ComNodeList(uint32 lui32SlavesCnt):cComNodeList(lui32SlavesCnt) {}


cI2c::cI2c(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda)
{
  mI2C = lstI2c;

  mPinSCL = lcScl;
  mPinSDA = lcSda;

  munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
  munBusStatus    = tenComBusStatus::enBusNoError;
  munComStatus    = tenComStatus::enStatusIdle;
}

void cI2c::I2C_StopDMA()
{
  I2C_DMACmd(mI2C, I2C_DMAReq_Rx | I2C_DMAReq_Tx, DISABLE);
  if (mI2C == I2C1)
  {
    DMA_Cmd(DMA1_Channel6, DISABLE);
    DMA_Cmd(DMA1_Channel7, DISABLE);
  }
  if (mI2C == I2C2)
  {
    DMA_Cmd(DMA1_Channel4, DISABLE);
    DMA_Cmd(DMA1_Channel5, DISABLE);
  }
}

void cI2c::I2C_StartDMA(uint8* pBuffer, uint32 BufferSize, uint32 Direction)
{
  I2C_StopDMA();
  /* Initialize the DMA with the new parameters */
  if (Direction == I2C_DIRECTION_TX)
  {
    if (mI2C == I2C1)
    {
      DMA_ClearFlag(DMA1_FLAG_TC6);
      DMA1_Channel6->CNDTR = BufferSize;
      DMA1_Channel6->CMAR  = (uint32)pBuffer;
      DMA_Cmd(DMA1_Channel6, ENABLE);
    }
    else
    {
      DMA_ClearFlag(DMA1_FLAG_TC4);
      DMA1_Channel4->CNDTR = BufferSize;
      DMA1_Channel4->CMAR  = (uint32)pBuffer;
      DMA_Cmd(DMA1_Channel4, ENABLE);
    }
    I2C_DMACmd(mI2C, I2C_DMAReq_Tx, ENABLE);
  }
  else /* Reception */
  {
    if (mI2C == I2C1)
    {
      DMA_ClearFlag(DMA1_FLAG_TC7);
      DMA1_Channel7->CNDTR = BufferSize;
      DMA1_Channel7->CMAR  = (uint32)pBuffer;
      DMA_Cmd(DMA1_Channel7, ENABLE);
    }
    else
    {
      DMA_ClearFlag(DMA1_FLAG_TC5);
      DMA1_Channel5->CNDTR = BufferSize;
      DMA1_Channel5->CMAR  = (uint32)pBuffer;
      DMA_Cmd(DMA1_Channel5, ENABLE);
    }
    I2C_DMACmd(mI2C, I2C_DMAReq_Rx, ENABLE);
  }
}

bool cI2c::bCheckBus()
{
  if (I2C_GetFlagStatus(mI2C, I2C_ISR_BUSY))
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceError;
    munBusStatus    = tenComBusStatus::enBusNotReady;
    mSm = enStError;
    return False;
  }
  return True;
}

bool cI2c::bCheckPins()
{
  mPinSCL->vSetMode(GPIO_Mode_IN);
  mPinSDA->vSetMode(GPIO_Mode_IN);

  //SDA and SCL must be pulled high
  if ((mPinSCL == 0) || (mPinSDA == 0))
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
    munBusStatus    = tenComBusStatus::enDeviceInitErrorPins;
    mSm = enStError;
    return False;
  }
  return True;
}

void cI2c::vSetAdr2(uint8 lui8Adr2)
{
  I2C_DualAddressCmd(mI2C, DISABLE);
  if (lui8Adr2)
  {
    mui8Adr2 = lui8Adr2;
    I2C_OwnAddress2Config(mI2C, mui8Adr2, 0);
    I2C_DualAddressCmd(mI2C,    ENABLE);
  }
}

void cI2c::vClearClockLine()
{
  mPinSCL->vSetMode(GPIO_Mode_OUT);


  for (u8 lu8t = 0; lu8t < 9; lu8t++)
  {
    cClockInfo::Delay_us(4);
    mPinSCL->vSet0();
    cClockInfo::Delay_us(4);
    mPinSCL->vSet1();
  }

  mPinSCL->vSetMode(GPIO_Mode_AF);
  mPinSCL->vSetAF(GPIO_AF_4);
}

void cI2c::vInitHw(bool lbMaster)
{
  I2C_InitTypeDef   I2C_InitStructure;

  /* I2C Periph clock enable */
  if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
  if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);
  if (mI2C == I2C3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, DISABLE);

  // DMA Rx
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  I2C_StopDMA();

  //SDA and SCL must be pulled high
  if (lbMaster)
  {
    if (!bCheckPins()) vClearClockLine();
    if (!bCheckPins()) return;
  }


  mPinSCL->vSetMode(GPIO_Mode_AF);
  mPinSDA->vSetMode(GPIO_Mode_AF);
  mPinSCL->vSetAF(GPIO_AF_4);
  mPinSDA->vSetAF(GPIO_AF_4);

  /* I2C Periph clock enable */
  if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  if (mI2C == I2C3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);

  /* I2C Peripheral Disable */
  I2C_Cmd(mI2C, DISABLE);

  /* Configure the I2C clock source. The clock is derived from the HSI */
  RCC_I2CCLKConfig(RCC_I2C2CLK_HSI);

  /* I2C configuration */
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_AnalogFilter        = I2C_AnalogFilter_Enable;
  I2C_InitStructure.I2C_DigitalFilter       = 0x00;
  I2C_InitStructure.I2C_OwnAddress1         = mui8Adr1;
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_Timing              = 0x00100308;

  /* 8Mhz (HSI  */
  /*
    100khz  = 0x10420F13  PRESC = 1; SCLL = 0x13; SCLH = 0xF; SDADEL = 0x2; SCLDEL = 0x4 => 100khz
    400khz  = 0x00310309  PRESC = 0; SCLL = 0x09; SCLH = 0x3; SDADEL = 0x1; SCLDEL = 0x3 => 364Khz
    500khz  = 0x00100308  PRESC = 0; SCLL = 0x06; SCLH = 0x3; SDADEL = 0x0; SCLDEL = 0x1 => 384khz
    1000khz = 0x00100103  PRESC = 0; SCLL = 0x03; SCLH = 0x1; SDADEL = 0x0; SCLDEL = 0x1 => 555khz
  */

  /* Apply I2C configuration after enabling it */
  I2C_Init(mI2C, &I2C_InitStructure);
  if (!lbMaster)
  {
    vSetAdr2(mui8Adr2);
  }

  mI2C->ICR = 0xFFFF;

  /* I2C Peripheral Enable */
  I2C_Cmd(mI2C, ENABLE);

  if (isErrorActive(lbMaster))
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
    munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
    return;
  }

  if (mI2C->ISR & I2C_ISR_BUSY)
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
    munBusStatus    = tenComBusStatus::enBusNotReady;
    return;
  }


  NVIC_InitTypeDef NVIC_InitStructure;
  uint8            lui8NVIC_IRQChannel;

  /* Configure the I2C event priority */
  if (mI2C == I2C1) lui8NVIC_IRQChannel = I2C1_EV_IRQn;
  if (mI2C == I2C2) lui8NVIC_IRQChannel = I2C2_EV_IRQn;
  //if (mI2C == I2C3) lui8NVIC_IRQChannel = I2C3_EV_IRQn;
  NVIC_InitStructure.NVIC_IRQChannel                   = lui8NVIC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  if (mI2C == I2C1) lui8NVIC_IRQChannel = I2C1_ER_IRQn;
  if (mI2C == I2C2) lui8NVIC_IRQChannel = I2C2_ER_IRQn;
  //if (mI2C == I2C3) lui8NVIC_IRQChannel = I2C3_ER_IRQn;
  NVIC_InitStructure.NVIC_IRQChannel                   = lui8NVIC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  if (lbMaster)
  {
    I2C_ITConfig(mI2C, I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI /*| I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI*/, ENABLE);
  }
  else
  {
    I2C_ITConfig(mI2C, I2C_IT_ERRI /*| I2C_IT_TCI*/ | I2C_IT_STOPI /*| I2C_IT_NACKI*/ | I2C_IT_ADDRI /*| I2C_IT_RXI | I2C_IT_TXI*/, ENABLE);
  }


  // DMA
  DMA_InitTypeDef  DMA_InitStructure;
  DMA_StructInit(&DMA_InitStructure);


  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(mI2C->RXDR); //=0x40005410 : address of data reading register of I2C1
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //channel will be used for peripheral to memory transfer
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;    //setting normal mode (non circular)
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;    //medium priority
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;    //Location assigned to peripheral register will be source

  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //automatic memory increment disable for peripheral
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    //automatic memory increment enable for memory
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    //source peripheral data size = 8bit
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //destination memory data size = 8bit
  DMA_InitStructure.DMA_MemoryBaseAddr = 0; //variable to store data
  DMA_InitStructure.DMA_BufferSize = 0;    //number of data to be transfered

  if (mI2C == I2C1)
  {
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
  }
  if (mI2C == I2C2)
  {
    DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  }


  // DMA interrupt is not used
  /*if (mI2C == I2C1) lui8NVIC_IRQChannel = DMA1_Channel7_IRQn; //I2C1 connect to channel 7 of DMA1
  if (mI2C == I2C2) lui8NVIC_IRQChannel = DMA1_Channel5_IRQn;

  NVIC_InitStructure.NVIC_IRQChannel = lui8NVIC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/

  // DMA Tx
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32)&(mI2C->TXDR); //=0x40005410 : address of data reading register of I2C1
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //channel will be used for peripheral to memory transfer
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;    //setting normal mode (non circular)
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;    //medium priority
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;    //Location assigned to peripheral register will be destination

  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //automatic memory increment disable for peripheral
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;    //automatic memory increment enable for memory
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    //source peripheral data size = 8bit
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //destination memory data size = 8bit
  DMA_InitStructure.DMA_MemoryBaseAddr = 0; //variable to store data
  DMA_InitStructure.DMA_BufferSize = 0;    //number of data to be transfered

  if (mI2C == I2C1)
  {
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
  }
  if (mI2C == I2C2)
  {
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);
  }

  // DMA interrupt is not used
  /*if (mI2C == I2C1) lui8NVIC_IRQChannel = DMA1_Channel6_IRQn; //I2C1 connect to channel 7 of DMA1
  if (mI2C == I2C2) lui8NVIC_IRQChannel = DMA1_Channel4_IRQn;

  NVIC_InitStructure.NVIC_IRQChannel = lui8NVIC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);*/

  munDeviceStatus = tenComDeviceStatus::enDeviceReady;
  munBusStatus    = tenComBusStatus::enBusNoError;
  munComStatus    = tenComStatus::enStatusIdle;
  mSm             = enStIdle;
}

void cI2c::vReInitHw(tencI2c_Consts lenOption, bool lbMaster)
{
  if (lenOption == enResetHwFull)
  {
    vInitHw(lbMaster);
  }
  else
  {
    // I2C Peripheral Disnable
    I2C_Cmd(mI2C, DISABLE);
    I2C_StopDMA();

    if (lbMaster)
    {
      //SDA and SCL must be pulled high
      if (!bCheckPins()) return;

      mPinSCL->vSetMode(GPIO_Mode_AF);
      mPinSCL->vSetMode(GPIO_Mode_AF);
    }

    mI2C->ICR = 0xFFFF;

    // I2C Peripheral Enable
    I2C_Cmd(mI2C, ENABLE);

    munDeviceStatus = tenComDeviceStatus::enDeviceReady;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
    mSm             = enStIdle;
  }

  if (isErrorActive(lbMaster))
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
    munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
    return;
  }

  if (mI2C->ISR & I2C_ISR_BUSY)
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
    munBusStatus    = tenComBusStatus::enBusNotReady;
    return;
  }
}



uint8 cI2c::isErrorActive(bool lbMaster)
{
  uint32 lui32ErrorMask;

  /*
  Note: in Slave mode
  In master receiver mode, NACK is automatically generated after last byte preceding STOP or RESTART condition, whatever the NACK bit value.When an overrun occurs in slave receiver NOSTRETCH mode, a NACK is automatica
  */

  if (((mSm == enStRx) && ( lbMaster)) ||
      ((mSm == enStTx) && (!lbMaster)))
  {
    if (mI2C->ISR & I2C_ISR_STOPF)
    {
      lui32ErrorMask = I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
    }
    else
    {
      lui32ErrorMask = I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
    }
  }
  else
  {
    lui32ErrorMask = I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
  }

  if ((mI2C->ISR & lui32ErrorMask)) return 1;
  return 0;
}


//-----------------------------------------------------------------------------------------------------------------

cI2cMaster::cI2cMaster(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint32 lui32Slaves)
  : cI2c(lstI2c, lcScl, lcSda), mcSlaves(lui32Slaves)
{
  mSm  = enStIdle;
  menComState = cComNode::tenComNodeState::enStIdle;

  mpcActiveSlave = NULL;
  mpcActiveMsg   = NULL;

  mui8Adr1 = mui8Adr2 = 0;

  mu16ReInitTicks = 100;
  mu16ComStuckTicksReload = 1000;
  mu16ComStuckTicks = mu16ComStuckTicksReload;

  vInitHw(True);
}

cI2cMaster::~cI2cMaster()
{
}


void cI2cMaster::vSetReInitTicks(u16 lu16ReInitTicks)
{
  if (!mu16ReInitTicks)
  {
    mu16ReInitTicks = lu16ReInitTicks;
  }
}

/*
  I2Cx_ISR->NACKF: Not Acknowledge received flag: It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0
  I2Cx_ISR->BERR:  Bus error:                     It is cleared by software by setting BERRCF bit.:     This bit is cleared by hardware when PE=0.
  I2Cx_ISR->OVR:   Overrun/Underrun (slave mode): It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0.
  I2Cx_ISR->ARLO:  Arbitration lost:              It is cleared by software by setting the ARLOCF bit.: This bit is cleared by hardware when PE=0.
*/
void cI2cMaster::vError()
{
  uint32 lui32ErrorMask;

  lui32ErrorMask = I2C_ISR_BUSY | I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;

  if (mI2C->ISR & lui32ErrorMask)
  {
    if (mI2C->ISR & I2C_ISR_NACKF)
    {
      mSm = enStIdle;
      if (mpcActiveMsg != NULL)
      {
        if (mpcActiveMsg->isBusy())
        {
          mpcActiveMsg->vError();

          if (mpcActiveSlave != NULL)
          {
            mpcActiveSlave->vComError(cComNode::enErNACK, menComState);
          }
        }
      }
    }
    else
    {
      if (mI2C->ISR & I2C_ISR_BUSY)
      {
        munBusStatus = tenComBusStatus::enBusNotReady;
        mSm = enStError;
      }
    }

    I2C_StopDMA();
    mTimer.vStop();
    mI2C->ICR = 0xFFFF;
    mpcActiveMsg   = NULL;
    mpcActiveSlave = NULL;
  }
}


void cI2cMaster::TIM_EV_IRQHandler()
{
  if (mpcActiveSlave)
  {
    mpcActiveSlave->vComStart(cComNode::enStMisc);
  }
}

/*
  RXNE:   Receive buffer not empty
  TXIS:   Transmit buffer interrupt status
  STOPF:  Stop detection interrupt flag
  TC/TCR: Transfer Complete/ Transfer complete Reload
  ADDR:   Address matched
  NACKF:  NACK reception
*/
void cI2cMaster::I2C_EV_IRQHandler()
{
  vSm(enEvIrq);

  if (mSm == enStIdle)
  {
    vStartNext();
  }
}

/*
  BERR:    Bus error
  ARLO:    Arbitration loss
  OVR:     Overrun/Underrun
  PECERR:  PEC error
  TIMEOUT: Timeout/tLOW error
  ALERT:   SMBus Alert
  Note: NACK is not an ER. It is a EV
*/
void cI2cMaster::I2C_ER_IRQHandler()
{
  vSm(enEvError);

  if (mSm == enStIdle)
  {
    vStartNext();
  }
}

void cI2cMaster::vAddSlave(cComNode *lpcSlave)
{
  mcSlaves.vAdd(lpcSlave);
}

void cI2cMaster::vRemoveSlave(cComNode *lpcSlave)
{
  mcSlaves.vRemove(lpcSlave);
}

void cI2cMaster::vReInitAllSlave()
{
  mcSlaves.vSetErrorAll(cComNode::tenComNodeError::enErReInit, cComNode::enStIdle);
}


void cI2cMaster::vSm(tencI2c_Events lenEvent)
{
  uint8 lui8SlaveAdr;
  tencI2c_States mSm_Old;

  if (isErrorActive(True))
  {
    vError();
    return;
  }

  lui8SlaveAdr = mpcActiveSlave->mAdr;
  menComState = cComNode::tenComNodeState::enStIdle;

  mSm_Old = mSm;
  switch (mSm)
  {
    case enStIdle:
    case enStWait:
      switch (lenEvent)
      {
        case enEvStart:
          menComState = cComNode::tenComNodeState::enStAdress;
          //TX and maybe RX
          if (mpcActiveMsg->cTxData.muiLen > 0)
          {
            mSm = enStTx;
            mui32ComByteCnt = mpcActiveMsg->cTxData.muiLen;
            mpui8ComBuf     = mpcActiveMsg->cTxData.mpu8Data;
            I2C_StartDMA(mpui8ComBuf, mui32ComByteCnt, I2C_DIRECTION_TX);
            if (mui32ComByteCnt>255)
            {
              I2C_TransferHandling(mI2C, lui8SlaveAdr, 255, I2C_Reload_Mode, I2C_Generate_Start_Write);
            }
            else
            {
              if (mpcActiveMsg->cRxData.muiLen > 0)
              {
                I2C_TransferHandling(mI2C, lui8SlaveAdr, mui32ComByteCnt, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
              }
              else
              {
                I2C_TransferHandling(mI2C, lui8SlaveAdr, mui32ComByteCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
              }
            }
          }
          //RX only
          else if (mpcActiveMsg->cRxData.muiLen > 0)
          {
            mSm = enStRx;
            mui32ComByteCnt = mpcActiveMsg->cRxData.muiLen;
            mpui8ComBuf     = mpcActiveMsg->cRxData.mpu8Data;
            I2C_StartDMA(mpui8ComBuf, mui32ComByteCnt, I2C_DIRECTION_RX);
            I2C_TransferHandling(mI2C, lui8SlaveAdr, mui32ComByteCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
          }
          // Nothing todo
          else
          {
            mSm = enStIdle;
            mpcActiveMsg->vDone();
            mpcActiveSlave->vComDone();
          }
          break;
        default:
          break;
      }
      break;
    case enStTx:
      {
        uint32 lui32ISR = mI2C->ISR;
        menComState = cComNode::tenComNodeState::enStData;
        switch (lenEvent)
        {
          case enEvIrq:
            if (lui32ISR & I2C_ISR_STOPF)
            {
              /* Clear STOPF flag */
              mI2C->ICR = I2C_ICR_STOPCF;
              mSm = enStIdle;
              mpcActiveMsg->vDone();
              mpcActiveSlave->vComDone();
            }
            else if (lui32ISR & I2C_ISR_TCR)
            {
              mui32ComByteCnt -=255;
              mpui8ComBuf     +=255;
              if (mui32ComByteCnt>255)
              {
                I2C_TransferHandling(mI2C, lui8SlaveAdr, 255, I2C_Reload_Mode, I2C_No_StartStop);
              }
              else
              {
                uint32 lTmpreg;
                lTmpreg = mI2C->CR2;
                lTmpreg &= (uint32)~((uint32_t)(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP));
                lTmpreg |= (uint32)mui32ComByteCnt << 16;
                if (mpcActiveMsg->cRxData.muiLen == 0)
                {
                  lTmpreg |= I2C_CR2_AUTOEND;
                }
                mI2C->CR2 = lTmpreg;
                mui32ComByteCnt = 0;
              }
            }
            else if (lui32ISR & I2C_ISR_TC)
            {
              I2C_DMACmd(mI2C, I2C_DMAReq_Tx, DISABLE);

              if (mpcActiveMsg->cRxData.muiLen > 0)
              {
                mSm = enStRx;
                mui32ComByteCnt = mpcActiveMsg->cRxData.muiLen;
                mpui8ComBuf     = mpcActiveMsg->cRxData.mpu8Data;
                I2C_StartDMA(mpui8ComBuf, mui32ComByteCnt, I2C_DIRECTION_RX);
                I2C_TransferHandling(mI2C, lui8SlaveAdr, mui32ComByteCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
              }
              else
              {
                mSm = enStIdle;
                mpcActiveMsg->vDone();
                mpcActiveSlave->vComDone();
              }
            }
            else
            {
              vError();
            }
            break;
          default:
            vError();
            break;
        }
      }
      break;

    case enStRx:
      menComState = cComNode::tenComNodeState::enStData;
      switch (lenEvent)
      {
        case enEvIrq:
          if (mI2C->ISR & (I2C_ISR_STOPF | I2C_ISR_TC))
          {
            /* Clear STOPF flag */
            mI2C->ICR = I2C_ICR_STOPCF;
            I2C_DMACmd(mI2C, I2C_DMAReq_Rx, DISABLE);
            mSm = enStIdle;
            mpcActiveMsg->vDone();
            mpcActiveSlave->vComDone();
          }
          else
          {
            vError();
          }
          break;
        default:
          vError();
          break;
      }
      break;

    default:
      break;
  }

  if (mSm_Old != mSm)
  {
    mu16ComStuckTicks = mu16ComStuckTicksReload;
  }
}

void cI2cMaster::Tick()
{
}

void cI2cMaster::vStartMsg(cComDatMsg *lpcActiveMsg)
{
  if (mSm == enStIdle)
  {
    bool lbStart;

    lbStart = true;

    if (mpcActiveMsg != NULL)
    {
      if (mpcActiveMsg->isBusy())
      {
        lbStart = false;
      }
    }

    if (lpcActiveMsg != NULL)
    {
      if (lpcActiveMsg->isBusy())
      {
        lbStart = false;
      }
    }

    lbStart &= bCheckBus();

    if (lbStart)
    {
      mpcActiveMsg = lpcActiveMsg;
      mpcActiveMsg->vBusy();
      vSm(enEvStart);
    }
  }
}

bool cI2cMaster::vStartNext()
{
  if (mSm == enStIdle)
  {
    bool lbStart;

    lbStart = true;

    if (mpcActiveMsg != NULL)
    {
      if (mpcActiveMsg->isBusy())
      {
        lbStart = false;
      }
    }

    if (lbStart)
    {
      lbStart &= (!mcSlaves.isEmpty());
    }

    lbStart &= bCheckBus();

    if (lbStart)
    {
      mpcActiveSlave = (cComNode*)mcSlaves.cGetNext();
      mpcActiveSlave->mStatus.IsStartRequested = 0;
      mpcActiveSlave->vComStart(cComNode::enStPrepareToSendData);
    }
  }

  if (mSm == enStError)
  {
    if (mu16ReInitTicks)
    {
      mu16ReInitTicks--;

      if (!mu16ReInitTicks)
      {
        mpcActiveMsg   = NULL;
        this->vReInitHw(enResetHwFull, True);
        this->vReInitAllSlave();
      }
    }
  }
  else
  {
    mu16ReInitTicks   = 100;
  }

  if (munComStatus != tenComStatus::enStatusStuck)
  {
    if (mu16ComStuckTicks)
    {
      mu16ComStuckTicks--;
    }
    else
    {
      mSm = enStError;
      munDeviceStatus   = tenComDeviceStatus::enDeviceReady;
      munBusStatus      = tenComBusStatus::enBusNoError;
      munComStatus      = tenComStatus::enStatusStuck;
      mu16ComStuckTicks = mu16ComStuckTicksReload;
      mu16ReInitTicks   = 100;
    }
  }

  if (mSm == enStError)
  {
    return False;
  }
  return True;

}


//-----------------------------------------------------------------------------------------------------


cI2cSlave::cI2cSlave(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint8 lui8Adr1, uint8 lui8Adr2)
: cI2c(lstI2c, lcScl, lcSda)
{
  mI2C       = lstI2c;
  mSm        = enStIdle;
  mpcI2cNode = NULL;

  mui8Adr1 = lui8Adr1;
  mui8Adr2 = lui8Adr2;

  mPinSCL = lcScl;
  mPinSDA = lcSda;
}

cI2cSlave::~cI2cSlave()
{
}

void cI2cSlave::SetSlaveNode(cComNode *lpcI2cNode)
{
  mpcI2cNode = lpcI2cNode;
  /* HW erst hier, weil solange mpcActiveSlave noch auf null zeigt */
  if (mpcI2cNode != null)
  {
    vInitHw(False);
  }
}


void cI2cSlave::vError()
{
  /* On error, check if is an slave specific (a) error or an
     general bus error (b)

     In case of
       (a): Cancel the running transmission, block the bus, reinit the DMA, inform the slave, free the bus
       (b): Cancel the running transmission, block the bus, reinit the Bus, inform all slaves, free the bus
  */

  uint32 lui32ErrorMask;

  lui32ErrorMask = I2C_ISR_BUSY | I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
  menErrorActive = cComNode::enErUnknown;

  if ((mI2C->ISR & lui32ErrorMask) != 0)
  {
    if      (mI2C->ISR & I2C_ISR_NACKF) menErrorActive = cComNode::enErNACK;
    else if (mI2C->ISR & I2C_ISR_BERR)  menErrorActive = cComNode::enErBusError;
    else if (mI2C->ISR & I2C_ISR_ARLO)  menErrorActive = cComNode::enErArbitrationLost;
    else if (mI2C->ISR & I2C_ISR_OVR)   menErrorActive = cComNode::enErOverrun;
    else if (mI2C->ISR & I2C_ISR_BUSY)  menErrorActive = cComNode::enErStartWhileBussy;
    else                                menErrorActive = cComNode::enErUnknown;


    // Cancel the running transmission
    if (mI2C->ISR & I2C_ISR_BUSY)
    {
      I2C_GenerateSTOP(mI2C, ENABLE);
    }

    vReInitHw(enResetHwFull, False);
    mSm = enStIdle;
    mpcI2cNode->vComError(menErrorActive, cComNode::enStIdle);
  }
}

void cI2cSlave::I2C_EV_IRQHandler()
{
  vSm(enEvIrq);
}

void cI2cSlave::I2C_ER_IRQHandler()
{
  vSm(enEvError);
}


void cI2cSlave::vSm(tencI2c_Events lenEvent)
{
  uint32 lui32ISR;

  UNUSED(lenEvent);

  if (isErrorActive(False))
  {
    vError();
  }

  lui32ISR = mI2C->ISR;

  switch (mSm)
  {
    case enStIdle:
      if (lui32ISR & I2C_ISR_ADDR)
      {
        mpcI2cNode->mAdr = (lui32ISR >> 16) & 0xFE;
        if (lui32ISR & I2C_ISR_DIR)
        {
          mSm = enStTx; // 1: Read transfer, slave enters transmitter mode.

          mpcMsg->vBusy();
          mpcI2cNode->vComStart(cComNode::enStPrepareForTx);
          I2C_StartDMA(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen, I2C_DIRECTION_TX);
        }
        else
        {
          mSm = enStRx; // 1: Write transfer, slave enters receiver mode.

          mpcMsg->vBusy();
          mpcI2cNode->vComStart(cComNode::enStPrepareForRx);
          I2C_StartDMA(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen, I2C_DIRECTION_RX);
        }
        mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
      }
      else
      {
        vError();
      }
      break;

    case enStTx:
    case enStRx:
      {
        if (lui32ISR & I2C_ISR_ADDR)
        {
          mpcI2cNode->mAdr = (lui32ISR >> 16) & 0xFE;
          if (lui32ISR & I2C_ISR_DIR)
          {
            mSm = enStTx; // 1: Read transfer, slave enters transmitter mode.

            mpcMsg->vBusy();
            mpcI2cNode->vComStart(cComNode::enStPrepareForTx);
            I2C_StartDMA(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen, I2C_DIRECTION_TX);
          }
          mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
        }
        else
        if (lui32ISR & I2C_ISR_STOPF)
        {
          mSm = enStIdle;

          I2C_DMACmd(mI2C, I2C_DMAReq_Tx, DISABLE);
          mI2C->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; //Clear Stop and Nack detection flag

          mpcMsg->vDone();
          mpcI2cNode->vComDone();
        }
      }
      break;
    default:
      break;
  }
}



