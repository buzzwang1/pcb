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
  I2C_DMACmd(mI2C, DISABLE);
  //I2C_DMALastTransferCmd(mI2C, DISABLE);

  if (mI2C == I2C1)
  {
    DMA_Cmd(DMA1_Stream6, DISABLE);
    DMA_Cmd(DMA1_Stream5, DISABLE);
  }
  if (mI2C == I2C2)
  {
    DMA_Cmd(DMA1_Stream7, DISABLE);
    DMA_Cmd(DMA1_Stream2, DISABLE);
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
      //DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
      DMA1_Stream6->NDTR = BufferSize;
      DMA1_Stream6->M0AR = (uint32)pBuffer;
      DMA_ITConfig(DMA1_Stream6, DMA_IT_TE | DMA_IT_TC, ENABLE);
      DMA_Cmd(DMA1_Stream6, ENABLE);
    }
    else
    {
      //DMA_ClearFlag(DMA1_FLAG_TC4);
      DMA1_Stream7->NDTR = BufferSize;
      DMA1_Stream7->M0AR = (uint32)pBuffer;
      DMA_ITConfig(DMA1_Stream7, DMA_IT_TE | DMA_IT_TC, ENABLE);
      DMA_Cmd(DMA1_Stream7, ENABLE);
    }
    I2C_DMACmd(mI2C, ENABLE);
  }
  else /* Reception */
  {
    if (mI2C == I2C1)
    {
      //DMA_ClearFlag(DMA1_FLAG_TC7);
      DMA1_Stream5->NDTR = BufferSize;
      DMA1_Stream5->M0AR = (uint32)pBuffer;
      DMA_ITConfig(DMA1_Stream5, DMA_IT_TE | DMA_IT_TC, ENABLE);
      DMA_Cmd(DMA1_Stream5, ENABLE);
    }
    else
    {
      //DMA_ClearFlag(DMA1_FLAG_TC5);
      DMA1_Stream2->NDTR = BufferSize;
      DMA1_Stream2->M0AR = (uint32)pBuffer;
      DMA_ITConfig(DMA1_Stream2, DMA_IT_TE | DMA_IT_TC, ENABLE);
      DMA_Cmd(DMA1_Stream2, ENABLE);
    }
    I2C_DMALastTransferCmd(mI2C, ENABLE);
    I2C_DMACmd(mI2C, ENABLE);
  }
}

bool cI2c::bCheckBus()
{
  if (I2C_GetFlagStatus(mI2C, I2C_FLAG_BUSY))
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
    I2C_OwnAddress2Config(mI2C, mui8Adr2);
    I2C_DualAddressCmd(mI2C,    ENABLE);
  }
}

void cI2c::vInitHw(bool lbMaster)
{
  I2C_InitTypeDef   I2C_InitStructure;

  /* I2C Periph clock enable */
  if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
  if (mI2C == I2C3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);

  // DMA Rx
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

  /* I2C Peripheral Enable */
  I2C_Cmd(mI2C, DISABLE);

  I2C_StopDMA();

  //SDA and SCL must be pulled high
  if (lbMaster)
  {
    if (!bCheckPins()) return;
  }

  if (mI2C == I2C1) { mPinSCL->vSetAF(GPIO_AF_I2C1); mPinSDA->vSetAF(GPIO_AF_I2C1); }
  if (mI2C == I2C2) { mPinSCL->vSetAF(GPIO_AF_I2C2); mPinSDA->vSetAF(GPIO_AF_I2C2); }
  if (mI2C == I2C3) { mPinSCL->vSetAF(GPIO_AF_I2C3); mPinSDA->vSetAF(GPIO_AF_I2C3); }


  /* Configure the I2C clock source. The clock is derived from the HSI */
  //RCC_FMPI2C1ClockSourceConfig(RCC_SYSCLKSource_HSI)

  /* I2C configuration */
  I2C_StructInit(&I2C_InitStructure);
  I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
  I2C_InitStructure.I2C_ClockSpeed          = 400000;
  I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1         = 0x00;
  I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

  /* 8Mhz (HSI  */
  /*
    100khz  = 0x10420F13  PRESC = 1; SCLL = 0x13; SCLH = 0xF; SDADEL = 0x2; SCLDEL = 0x4 => 100khz
    400khz  = 0x00310309  PRESC = 0; SCLL = 0x09; SCLH = 0x3; SDADEL = 0x1; SCLDEL = 0x3 => 364Khz
    500khz  = 0x00100308  PRESC = 0; SCLL = 0x06; SCLH = 0x3; SDADEL = 0x0; SCLDEL = 0x1 => 384khz
    1000khz = 0x00100103  PRESC = 0; SCLL = 0x03; SCLH = 0x1; SDADEL = 0x0; SCLDEL = 0x1 => 555khz
  */

   /* Configure I2C Filters */
   //I2C_AnalogFilterCmd(mI2C, ENABLE);
   //I2C_DigitalFilterConfig(mI2C, 0x0F);

  /* Apply I2C configuration after enabling it */
  I2C_Init(mI2C, &I2C_InitStructure);
  if (!lbMaster)
  {
    vSetAdr2(mui8Adr2);
  }

  mI2C->SR1 = 0x0; mI2C->SR2 = 0x0;

  /* I2C Peripheral Enable */
  I2C_Cmd(mI2C, ENABLE);

  if (isErrorActive(lbMaster))
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
    munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
    return;
  }

  if (mI2C->SR2 & I2C_FLAG_BUSY)
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
    I2C_ITConfig(mI2C, I2C_IT_EVT | I2C_IT_ERR, ENABLE);
    //I2C_ITConfig(mI2C, I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI | I2C_IT_NACKI /*| I2C_IT_ADDRI | I2C_IT_RXI | I2C_IT_TXI*/, ENABLE);
  }
  else
  {
    //I2C_ITConfig(mI2C, I2C_IT_ERRI | I2C_IT_TCI | I2C_IT_STOPI /*| I2C_IT_NACKI*/ | I2C_IT_ADDRI /*| I2C_IT_RXI | I2C_IT_TXI*/, ENABLE);
  }


  // DMA
  DMA_InitTypeDef  DMA_InitStructure;
  DMA_StructInit(&DMA_InitStructure);

  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(mI2C->DR); //=0x40005410 : address of data reading register of I2C1
  //DMA_InitStructure.DMA_M2M      = DMA_M2M_Disable;
  DMA_InitStructure.DMA_Mode     = DMA_Mode_Normal;    //setting normal mode (non circular)
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;    //medium priority
  DMA_InitStructure.DMA_DIR      = DMA_DIR_PeripheralToMemory;    //Location assigned to peripheral register will be source

  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; //automatic memory increment disable for peripheral
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;    //automatic memory increment enable for memory
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    //source peripheral data size = 8bit
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;    //destination memory data size = 8bit
  DMA_InitStructure.DMA_Memory0BaseAddr    = 0; //variable to store data
  DMA_InitStructure.DMA_BufferSize         = 0;    //number of data to be transfered

  if (mI2C == I2C1)
  {
    DMA_InitStructure.DMA_Channel = DMA_Channel_1;
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);
  }
  if (mI2C == I2C2)
  {
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_Init(DMA1_Stream2, &DMA_InitStructure);
  }

  if (mI2C == I2C1) lui8NVIC_IRQChannel = DMA1_Stream5_IRQn; //I2C1 connect to channel 7 of DMA1
  if (mI2C == I2C2) lui8NVIC_IRQChannel = DMA1_Stream2_IRQn;

  NVIC_InitStructure.NVIC_IRQChannel = lui8NVIC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  // DMA Tx
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32)&(mI2C->DR); //=0x40005410 : address of data reading register of I2C1
  //DMA_InitStructure.DMA_M2M      = DMA_M2M_Disable; //channel will be used for peripheral to memory transfer
  DMA_InitStructure.DMA_Mode     = DMA_Mode_Normal;    //setting normal mode (non circular)
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;    //medium priority
  DMA_InitStructure.DMA_DIR      = DMA_DIR_MemoryToPeripheral;    //Location assigned to peripheral register will be destination

  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable; //automatic memory increment disable for peripheral
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;    //automatic memory increment enable for memory
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;    //source peripheral data size = 8bit
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;    //destination memory data size = 8bit
  DMA_InitStructure.DMA_Memory0BaseAddr    = 0; //variable to store data
  DMA_InitStructure.DMA_BufferSize         = 0;    //number of data to be transfered
  if (mI2C == I2C1)
  {
    DMA_InitStructure.DMA_Channel = DMA_Channel_1;
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);
  }
  if (mI2C == I2C2)
  {
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;
    DMA_Init(DMA1_Stream7, &DMA_InitStructure);
  }

  if (mI2C == I2C1) lui8NVIC_IRQChannel = DMA1_Stream6_IRQn; //I2C1 connect to channel 7 of DMA1
  if (mI2C == I2C2) lui8NVIC_IRQChannel = DMA1_Stream7_IRQn;

  NVIC_InitStructure.NVIC_IRQChannel = lui8NVIC_IRQChannel;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

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

      if (mI2C == I2C1) { mPinSCL->vSetAF(GPIO_AF_I2C1); mPinSDA->vSetAF(GPIO_AF_I2C1); }
      if (mI2C == I2C2) { mPinSCL->vSetAF(GPIO_AF_I2C2); mPinSDA->vSetAF(GPIO_AF_I2C2); }
      if (mI2C == I2C3) { mPinSCL->vSetAF(GPIO_AF_I2C3); mPinSDA->vSetAF(GPIO_AF_I2C3); }
    }

    mI2C->SR1 = 0x0; mI2C->SR2 = 0x0;

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

  if (mI2C->SR2 & I2C_FLAG_BUSY)
  {
    munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
    munBusStatus    = tenComBusStatus::enBusNotReady;
    return;
  }
}



uint8 cI2c::isErrorActive(bool lbMaster)
{
  uint32 lui32ErrorMaskSR1;

  /*
  Note: in Slave mode
  In master receiver mode, NACK is automatically generated after last byte preceding STOP or RESTART condition, whatever the NACK bit value.When an overrun occurs in slave receiver NOSTRETCH mode, a NACK is automatica
  */

  if ((mSm == enStTx) || (lbMaster))
  {
    if (mI2C->SR1 & I2C_FLAG_STOPF)
    {
      lui32ErrorMaskSR1 =               (I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR | I2C_FLAG_PECERR | I2C_FLAG_TIMEOUT | I2C_FLAG_SMBALERT) & 0xFFFF;
    }
    else
    {
      lui32ErrorMaskSR1 = (I2C_FLAG_AF | I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR | I2C_FLAG_PECERR | I2C_FLAG_TIMEOUT | I2C_FLAG_SMBALERT) & 0xFFFF;
    }
  }
  else
  {
    lui32ErrorMaskSR1 =   (I2C_FLAG_AF | I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR | I2C_FLAG_PECERR | I2C_FLAG_TIMEOUT | I2C_FLAG_SMBALERT) & 0xFFFF;
  }

  if ((mI2C->SR1 & lui32ErrorMaskSR1)) return 1;
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

void cI2cMaster::vError()
{
  uint32 lui32ErrorMaskSR1;
  uint32 lui32ErrorMaskSR2;

  lui32ErrorMaskSR1 = (I2C_FLAG_AF | I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR | I2C_FLAG_PECERR | I2C_FLAG_TIMEOUT | I2C_FLAG_SMBALERT) & 0xFFFF;
  lui32ErrorMaskSR2 = I2C_FLAG_BUSY;

  if ((mI2C->SR1 & lui32ErrorMaskSR1) || (mI2C->SR2 & lui32ErrorMaskSR2))
  {
    if (mI2C->SR1 & I2C_FLAG_AF)
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
      if (mI2C->SR2 & I2C_FLAG_BUSY)
      {
        munBusStatus = tenComBusStatus::enBusNotReady;
        mSm = enStError;
      }
    }

    I2C_StopDMA();
    mI2C->SR1 = 0x0; mI2C->SR2 = 0x0;
    mpcActiveMsg   = NULL;
    mpcActiveSlave = NULL;
  }
}

void cI2cMaster::I2C_EV_DMAHandler()
{
  vSm(enEvDma);

  if (mSm == enStIdle)
  {
    vStartNext();
  }
}

void cI2cMaster::I2C_EV_IRQHandler()
{
  vSm(enEvIrq);

  if (mSm == enStIdle)
  {
    vStartNext();
  }
}

void cI2cMaster::I2C_ER_IRQHandler()
{
  /*
    I2Cx_ISR->NACKF: Not Acknowledge received flag: It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0
    I2Cx_ISR->BERR:  Bus error:                     It is cleared by software by setting BERRCF bit.:     This bit is cleared by hardware when PE=0.
    I2Cx_ISR->OVR:   Overrun/Underrun (slave mode): It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0.
    I2Cx_ISR->ARLO:  Arbitration lost:              It is cleared by software by setting the ARLOCF bit.: This bit is cleared by hardware when PE=0.
  */

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
  uint16 lui16Timeout;
  uint16 lui16Timeout_Reload;
  tencI2c_States mSm_Old;

  if (isErrorActive(True))
  {
    vError();
    return;
  }

  lui16Timeout_Reload = cClockInfo::mstClocks.PCLK2_Frequency / 1000000;

  lui8SlaveAdr = mpcActiveSlave->mAdr;
  menComState = cComNode::tenComNodeState::enStIdle;

  mSm_Old = mSm;
  switch (mSm)
  {
    case enStIdle:
      switch (lenEvent)
      {
        case enEvStart:
          mui32DataIdx = 0;

          //TX and maybe RX
          if (mpcActiveMsg->cTxData.muiLen > 0)
          {
            mSm = enStTx;
            mui32ComByteCnt = mpcActiveMsg->cTxData.muiLen;
            mpui8ComBuf     = mpcActiveMsg->cTxData.mpu8Data;

            mI2C->CR1 = 1; /*PE*/
            mI2C->SR1 = 0;
            mI2C->SR2 = 0;
            I2C_GenerateSTART(mI2C, ENABLE);
          }
          //RX only
          else if (mpcActiveMsg->cRxData.muiLen > 0)
          {
            mSm = enStRx;
            mui32ComByteCnt = mpcActiveMsg->cRxData.muiLen;
            mpui8ComBuf     = mpcActiveMsg->cRxData.mpu8Data;

            mI2C->CR1 = 1; /*PE*/
            mI2C->SR1 = 0;
            mI2C->SR2 = 0;

            if (mpcActiveMsg->cRxData.muiLen > 1)
            {
              mI2C->CR1 |= (1 << 10); /* activate ACK */
            }

            if (mpcActiveMsg->cRxData.muiLen == 2)
            {
              mI2C->CR1 |= (1 << 11); /* activate POS */ /* The POS bit must be used only in 2-byte reception configuration
                                                            in master mode. It must be configured before data reception starts*/
            }

            lui16Timeout = lui16Timeout_Reload;
            I2C_GenerateSTART(mI2C, ENABLE);
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
        menComState = cComNode::tenComNodeState::enStData;
        switch (lenEvent)
        {
          case enEvDma:
            lui16Timeout = lui16Timeout_Reload;
            while ((!I2C_GetFlagStatus(mI2C, I2C_FLAG_BTF | I2C_FLAG_TXE)) && (lui16Timeout)) {lui16Timeout--;}
            I2C_DMACmd(mI2C, DISABLE);

            if (lui16Timeout)
            {
              if (mpcActiveMsg->cRxData.muiLen == 0)
              {
                /* Stop. No repeated start*/
                I2C_GenerateSTOP(mI2C, ENABLE);
                while (I2C1->SR2 & 2 /* Busy */) {}

                mSm = enStIdle;
                mpcActiveMsg->vDone();
                mpcActiveSlave->vComDone();
              }
              else
              {
                mSm = enStRx;
                mui32ComByteCnt = mpcActiveMsg->cRxData.muiLen;
                mpui8ComBuf     = mpcActiveMsg->cRxData.mpu8Data;

                /* Not Stop. Repeated start*/
                mI2C->CR1 = 1; /*PE*/
                mI2C->SR1 = 0;
                mI2C->SR2 = 0;

                if (mpcActiveMsg->cRxData.muiLen > 1)
                {
                  mI2C->CR1 |= (1 << 10); /* activate ACK */
                }

                if (mpcActiveMsg->cRxData.muiLen == 2)
                {
                  mI2C->CR1 |= (1 << 11); /* activate POS */ /* The POS bit must be used only in 2-byte reception configuration
                                                                in master mode. It must be configured before data reception starts*/
                }

                lui16Timeout = lui16Timeout_Reload;
                I2C_GenerateSTART(mI2C, ENABLE);
              }
            }
            if (!lui16Timeout)
            {
              vError();
            }
            break;
          case enEvIrq:
            if (I2C_CheckEvent(mI2C, I2C_EVENT_MASTER_MODE_SELECT))
            {
              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Transmitter);
              I2C_StartDMA(mpui8ComBuf, mui32ComByteCnt, I2C_DIRECTION_TX);
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
        case enEvDma:
            lui16Timeout = lui16Timeout_Reload;
            //while ((!I2C_GetFlagStatus(mI2C, I2C_FLAG_BTF)) && (!isErrorActive())) {}
            I2C_GenerateSTOP(mI2C, ENABLE);
            while ((I2C1->SR2 & 2 /* Busy */) && (lui16Timeout)) {lui16Timeout--;}
            I2C_DMACmd(mI2C, DISABLE);

            mSm = enStIdle;
            mpcActiveMsg->vDone();
            mpcActiveSlave->vComDone();

            if (!lui16Timeout)
            {
              vError();
            }
            break;

        case enEvIrq:
          {
            volatile uint32 SR1 = mI2C->SR1;
            volatile uint32 SR2 = mI2C->SR2;
            if (SR1 & 2) // Master Adress set
            {
              if (mpcActiveMsg->cRxData.muiLen == 1)
              {
                mI2C->CR1 |= (1 << 9); /* activate Stop */
              }
              I2C_StartDMA(mpui8ComBuf, mui32ComByteCnt, I2C_DIRECTION_RX);
            }
            else
            if (SR2 & 1)
            {
              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Receiver);
            }
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




