#include "cI2c.h"

I2C_TypeDef I2C1_BASE;
I2C_TypeDef I2C2_BASE;
I2C_TypeDef I2C3_BASE;


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
  //LL_I2C_DisableDMAReq_RX(mI2C);
  //LL_I2C_DisableDMAReq_TX(mI2C);
  //if (mI2C == I2C1)
  //{
  //  DMA1_Channel6->CCR &=  ~DMA_CCR_EN;
  //  DMA1_Channel7->CCR &=  ~DMA_CCR_EN;
  //}
  //if (mI2C == I2C2)
  //{
  //  DMA1_Channel4->CCR &=  ~DMA_CCR_EN;
  //  DMA1_Channel5->CCR &=  ~DMA_CCR_EN;
  //}
}

void cI2c::I2C_StartDMA(uint8* pBuffer, uint32 BufferSize, uint32 Direction)
{
  //I2C_StopDMA();
  ///* Initialize the DMA with the new parameters */
  //if (Direction == I2C_DIRECTION_TX)
  //{
  //  if (mI2C == I2C1)
  //  {
  //    DMA1->IFCR = DMA_FLAG_TC6;
  //    DMA1_Channel6->CNDTR = BufferSize;
  //    DMA1_Channel6->CMAR  = (uint32)pBuffer;
  //    DMA1_Channel6->CCR  |=  DMA_CCR_EN;
  //  }
  //  else
  //  {
  //    DMA1->IFCR = DMA_FLAG_TC4;
  //    DMA1_Channel4->CNDTR = BufferSize;
  //    DMA1_Channel4->CMAR  = (uint32)pBuffer;
  //    DMA1_Channel4->CCR  |=  DMA_CCR_EN;
  //  }
  //  LL_I2C_EnableDMAReq_TX(mI2C);
  //}
  //else /* Reception */
  //{
  //  if (mI2C == I2C1)
  //  {
  //    DMA1->IFCR = DMA_FLAG_TC7;
  //    DMA1_Channel7->CNDTR = BufferSize;
  //    DMA1_Channel7->CMAR  = (uint32)pBuffer;
  //    DMA1_Channel7->CCR  |=  DMA_CCR_EN;
  //  }
  //  else
  //  {
  //    DMA1->IFCR = DMA_FLAG_TC5;
  //    DMA1_Channel5->CNDTR = BufferSize;
  //    DMA1_Channel5->CMAR  = (uint32)pBuffer;
  //    DMA1_Channel5->CCR  |=  DMA_CCR_EN;
  //  }
  //  LL_I2C_EnableDMAReq_RX(mI2C);
  //}
}

bool cI2c::bCheckBus()
{
  //if (mI2C->ISR & I2C_ISR_BUSY)
  //{
  //  munDeviceStatus = tenComDeviceStatus::enDeviceError;
  //  munBusStatus    = tenComBusStatus::enBusNotReady;
  //  mSm = enStError;
  //  return False;
  //}
  return True;
}

bool cI2c::bCheckPins()
{
  mPinSCL->vSetMode(GPIO_MODE_INPUT);
  mPinSDA->vSetMode(GPIO_MODE_INPUT);

  //SDA and SCL must be pulled high
  //if ((mPinSCL == 0) || (mPinSDA == 0))
  //{
  //  munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
  //  munBusStatus    = tenComBusStatus::enDeviceInitErrorPins;
  //  mSm = enStError;
  //  return False;
  //}
  return True;
}

void cI2c::vSetAdr2(uint8 lui8Adr2)
{
  //LL_I2C_DisableOwnAddress2(mI2C);
  if (lui8Adr2)
  {
    mui8Adr2 = lui8Adr2;
    //LL_I2C_SetOwnAddress2(mI2C, mui8Adr2, 0);
    //LL_I2C_EnableOwnAddress2(mI2C);
  }
}

void cI2c::vClearClockLine()
{
  if (mI2C == I2C1) { mPinSCL->vSetMode(GPIO_MODE_OUTPUT_OD);}
  if (mI2C == I2C2) { mPinSCL->vSetMode(GPIO_MODE_OUTPUT_OD);}
  if (mI2C == I2C3) { mPinSCL->vSetMode(GPIO_MODE_OUTPUT_OD);}

  for (u8 lu8t = 0; lu8t < 9; lu8t++)
  {
    cClockInfo::Delay_us(4);
    mPinSCL->vSet0();
    cClockInfo::Delay_us(4);
    mPinSCL->vSet1();
  }

  if (mI2C == I2C1) { mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C1);}
  if (mI2C == I2C2) { mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C2);}
  if (mI2C == I2C3) { mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C3);}

}

void cI2c::vInitHw(bool lbMaster)
{
  //I2C_HandleTypeDef lhI2c = {};

  //if (mI2C == I2C1) __HAL_RCC_I2C1_CLK_DISABLE();
  //if (mI2C == I2C2) __HAL_RCC_I2C2_CLK_DISABLE();
  //if (mI2C == I2C3) __HAL_RCC_I2C3_CLK_DISABLE();

  // DMA Rx
  //__HAL_RCC_DMA1_CLK_ENABLE();

  I2C_StopDMA();

  //SDA and SCL must be pulled high
  if (lbMaster)
  {
    if (!bCheckPins()) vClearClockLine();
    if (!bCheckPins()) return;
  }


  if (mI2C == I2C1) { mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C1);
                      mPinSDA->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C1);}
  if (mI2C == I2C2) { mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C2);
                      mPinSDA->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C2);}
  if (mI2C == I2C3) { mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C3);
                      mPinSDA->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C3);}

  /* I2C Periph clock enable */
  //if (mI2C == I2C1) __HAL_RCC_I2C1_CLK_ENABLE();
  //if (mI2C == I2C2) __HAL_RCC_I2C2_CLK_ENABLE();
  //if (mI2C == I2C3) __HAL_RCC_I2C3_CLK_ENABLE();

  /* I2C Peripheral Disable */
  //LL_I2C_Disable(mI2C);

  /* Configure the I2C clock source. The clock is derived from the HSI */
  //RCC_I2CCLKConfig(RCC_I2C2CLK_HSI);

  /* I2C configuration */
  //lhI2c.Instance = mI2C;
  ////lhI2c.Init.Timing          = 0x0010061A;
  //lhI2c.Init.Timing          = 0x00100C0D;
  //lhI2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
  //lhI2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
  //lhI2c.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  //lhI2c.Init.OwnAddress1     = mui8Adr1;
  //lhI2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  //lhI2c.Init.OwnAddress2     = mui8Adr2;
  //lhI2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  //lhI2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;

  /* 8Mhz (HSI  */
  /*
    100khz  = 0x10420F13  PRESC = 1; SCLL = 0x13; SCLH = 0xF; SDADEL = 0x2; SCLDEL = 0x4 => 100khz
    400khz  = 0x00310309  PRESC = 0; SCLL = 0x09; SCLH = 0x3; SDADEL = 0x1; SCLDEL = 0x3 => 364Khz
    500khz  = 0x00100308  PRESC = 0; SCLL = 0x06; SCLH = 0x3; SDADEL = 0x0; SCLDEL = 0x1 => 384khz
    1000khz = 0x00100103  PRESC = 0; SCLL = 0x03; SCLH = 0x1; SDADEL = 0x0; SCLDEL = 0x1 => 555khz
  */

  /* Peripheral clock enable */
  //HAL_I2C_Init(&lhI2c);
  //HAL_I2CEx_AnalogFilter_Config(&lhI2c, I2C_ANALOGFILTER_ENABLED);
  if (!lbMaster)
  {
    vSetAdr2(mui8Adr2);
  }

  mI2C->ICR = 0xFFFF;

  /* I2C Peripheral Enable */
  //LL_I2C_Enable(mI2C);

  //if (isErrorActive(lbMaster))
  //{
  //  munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
  //  munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
  //  return;
  //}

  //if (mI2C->ISR & I2C_ISR_BUSY)
  //{
  //  munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
  //  munBusStatus    = tenComBusStatus::enBusNotReady;
  //  return;
  //}

  //IRQn_Type lnNVIC_IRQChannel;
  //
  //if (mI2C == I2C1) lnNVIC_IRQChannel = I2C1_EV_IRQn; //I2C1 connect to channel 7 of DMA1
  //if (mI2C == I2C2) lnNVIC_IRQChannel = I2C2_EV_IRQn;
  ///* Set the TIMx priority */
  //HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 8);
  ///* Enable the TIMx global Interrupt */
  //HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);
  //
  //
  //if (mI2C == I2C1) lnNVIC_IRQChannel = I2C1_ER_IRQn; //I2C1 connect to channel 7 of DMA1
  //if (mI2C == I2C2) lnNVIC_IRQChannel = I2C2_ER_IRQn;
  ///* Set the TIMx priority */
  //HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 8);
  ///* Enable the TIMx global Interrupt */
  //HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);


  //if (lbMaster)
  //{
  //  mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE;
  //}
  //else
  //{
  //  mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_ADDRIE;
  //}

  // ----------- DMA ---------
  //DMA_HandleTypeDef lhDMA = {};
  //
  //if (mI2C == I2C1) lhDMA.Instance = DMA1_Channel7;
  //if (mI2C == I2C2) lhDMA.Instance = DMA1_Channel5;
  //
  //lhDMA.Init.Request             = DMA_REQUEST_3;
  //lhDMA.Init.Direction           = DMA_PERIPH_TO_MEMORY;
  //lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
  //lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
  //lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  //lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  //lhDMA.Init.Mode                = DMA_NORMAL;
  //lhDMA.Init.Priority            = DMA_PRIORITY_LOW;
  //
  //HAL_DMA_Init(&lhDMA);

  /* Configure DMA Channel data length */
  //lhDMA.Instance->CNDTR = 0;
  /* Configure DMA Channel destination address */
  //lhDMA.Instance->CPAR = (uint32_t)&(mI2C->RXDR);
  /* Configure DMA Channel source address */
  //lhDMA.Instance->CMAR = 0;

  // DMA interrupt is not used
  //if (mI2C == I2C1) lnNVIC_IRQChannel = DMA1_Channel7_IRQn; //I2C1 connect to channel 7 of DMA1
  //if (mI2C == I2C2) lnNVIC_IRQChannel = DMA1_Channel5_IRQn;
  /* Set the TIMx priority */
  //HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 8);
  /* Enable the TIMx global Interrupt */
  //HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);

  // DMA Tx
  //if (mI2C == I2C1) lhDMA.Instance = DMA1_Channel6;
  //if (mI2C == I2C2) lhDMA.Instance = DMA1_Channel4;
  //
  //lhDMA.Init.Request             = DMA_REQUEST_3;
  //lhDMA.Init.Direction           = DMA_MEMORY_TO_PERIPH;
  //lhDMA.Init.PeriphInc           = DMA_PINC_DISABLE;
  //lhDMA.Init.MemInc              = DMA_MINC_ENABLE;
  //lhDMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  //lhDMA.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
  //lhDMA.Init.Mode                = DMA_NORMAL;
  //lhDMA.Init.Priority            = DMA_PRIORITY_LOW;
  //
  //HAL_DMA_Init(&lhDMA);

  /* Configure DMA Channel data length */
  //lhDMA.Instance->CNDTR = 0;
  /* Configure DMA Channel destination address */
  //lhDMA.Instance->CPAR = (uint32)&(mI2C->TXDR);
  /* Configure DMA Channel source address */
  //lhDMA.Instance->CMAR = 0;

  // DMA interrupt is not used
  //if (mI2C == I2C1) lnNVIC_IRQChannel = DMA1_Channel6_IRQn; //I2C1 connect to channel 7 of DMA1
  //if (mI2C == I2C2) lnNVIC_IRQChannel = DMA1_Channel4_IRQn;
  /* Set the TIMx priority */
  //HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 8);
  /* Enable the TIMx global Interrupt */
  //HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);

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
    //LL_I2C_Disable(mI2C);
    I2C_StopDMA();

    if (lbMaster)
    {
      //SDA and SCL must be pulled high
      if (!bCheckPins()) return;

      mPinSCL->vSetMode(GPIO_MODE_AF_OD);
      mPinSCL->vSetMode(GPIO_MODE_AF_OD);
    }

    mI2C->ICR = 0xFFFF;

    // I2C Peripheral Enable
    //LL_I2C_Enable(mI2C);

    munDeviceStatus = tenComDeviceStatus::enDeviceReady;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
    mSm             = enStIdle;
  }

  //if (isErrorActive(lbMaster))
  //{
  //  munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
  //  munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
  //  return;
  //}
  //
  //if (mI2C->ISR & I2C_ISR_BUSY)
  //{
  //  munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
  //  munBusStatus    = tenComBusStatus::enBusNotReady;
  //  return;
  //}
}



uint8 cI2c::isErrorActive(bool lbMaster)
{
  //uint32 lui32ErrorMask;

  /*
  Note: in Slave mode
  In master receiver mode, NACK is automatically generated after last byte preceding STOP or RESTART condition, whatever the NACK bit value.When an overrun occurs in slave receiver NOSTRETCH mode, a NACK is automatica
  */

  //if (((mSm == enStRx) && ( lbMaster)) ||
  //    ((mSm == enStTx) && (!lbMaster)))
  //{
  //  if (mI2C->ISR & I2C_ISR_STOPF)
  //  {
  //    lui32ErrorMask = I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
  //  }
  //  else
  //  {
  //    lui32ErrorMask = I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
  //  }
  //}
  //else
  //{
  //  lui32ErrorMask = I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
  //}
  //
  //if ((mI2C->ISR & lui32ErrorMask)) return 1;
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
  //uint32 lui32ErrorMask;
  //
  //lui32ErrorMask = I2C_ISR_BUSY | I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
  //
  //if (mI2C->ISR & lui32ErrorMask)
  //{
  //  if (mI2C->ISR & I2C_ISR_NACKF)
  //  {
  //    mSm = enStIdle;
  //    if (mpcActiveMsg != NULL)
  //    {
  //      if (mpcActiveMsg->isBusy())
  //      {
  //        mpcActiveMsg->vError();
  //
  //        if (mpcActiveSlave != NULL)
  //        {
  //          mpcActiveSlave->vComError(cComNode::enErNACK, menComState);
  //        }
  //      }
  //    }
  //  }
  //  else
  //  {
  //    if (mI2C->ISR & I2C_ISR_BUSY)
  //    {
  //      munBusStatus = tenComBusStatus::enBusNotReady;
  //      mSm = enStError;
  //    }
  //  }
  //
  //  I2C_StopDMA();
  //  mTimer.vStop();
  //  mI2C->ICR = 0xFFFF;
  //  mpcActiveMsg   = NULL;
  //  mpcActiveSlave = NULL;
  //}
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
    bStartNext();
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
    bStartNext();
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

  lui8SlaveAdr = (u8)mpcActiveSlave->mAdr;
  menComState = cComNode::tenComNodeState::enStIdle;

  mSm_Old = mSm;
  switch (mSm)
  {
    case enStIdle:
    case enStWait:
      switch (lenEvent)
      {
        case enEvStart:
          
          //mSMCurrent = cComNode::tenComNodeState::enStAdress;
          //if (mpcActiveSlave->mpcSimSlave)
          {
            //mpcActiveSlave->mpcSimSlave->mpcI2cNode->mAdrRx = mpcActiveSlave->mAdr;

            //TX and maybe RX
            if (mpcActiveMsg->cTxData.muiLen > 0)
            {
              mSm = enStTx;

              //Daten übertragen simulieren
              //mpcActiveSlave->mpcSimSlave->mpcI2cNode->bComStartRx();
              //mpcActiveSlave->mpcSimSlave->mpcI2cNode->mpcMsg.cRxData.Set(mpcActiveMsg->cTxData);
              //if (!mpcActiveMsg->cRxData.muiLen)
              //{
              //  mpcActiveSlave->mpcSimSlave->mpcI2cNode->vComDone();
              //}
            }
          }
          //RX only
          //else if (mpcActiveMsg->cRxData.muiLen > 0)
          {
            mSm = enStRx;

            //Daten übertragen simulieren
            //mpcActiveSlave->mpcSimSlave->mpcI2cNode->bComStartTx();
            //mpcActiveMsg->cRxData.Set(mpcActiveSlave->mpcSimSlave->mpcI2cNode->mpcMsg.cTxData);
            //mpcActiveSlave->mpcSimSlave->mpcI2cNode->vComDone();
          }
          // Nothing todo
          //else
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
        uint32 lui32ISR = 0;
        menComState = cComNode::tenComNodeState::enStData;
        switch (lenEvent)
        {
          case enEvIrq:
            //  if (mpcActiveMsg->cRxData.muiLen > 0)
            //  {
            //    mSm = enStRx;
            //
            //    //Daten übertragen simulieren
            //    mpcActiveSlave->mpcSimSlave->mpcI2cNode->bComStartTx();
            //    mpcActiveMsg->cRxData.Set(mpcActiveSlave->mpcSimSlave->mpcI2cNode->mpcMsg.cTxData);
            //    mpcActiveSlave->mpcSimSlave->mpcI2cNode->vComDone();
            //  }
            //  else
            //  {
            //    mSm = enStIdle;
            //    mpcActiveMsg->vDone();
            //    mpcActiveSlave->vComDone();
            //  }
            //}
            //else
            //{
            //  vError();
            //}
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
          if (1/*mI2C->ISR & (I2C_ISR_STOPF | I2C_ISR_TC*/)
          {
            /* Clear STOPF flag */
            //mI2C->ICR = I2C_ICR_STOPCF;
            //LL_I2C_DisableDMAReq_RX(mI2C);
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

bool cI2cMaster::bStartNext()
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

  //uint32 lui32ErrorMask;
  //
  //lui32ErrorMask = I2C_ISR_BUSY | I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;
  //menErrorActive = cComNode::enErUnknown;
  //
  //if ((mI2C->ISR & lui32ErrorMask) != 0)
  //{
  //  if      (mI2C->ISR & I2C_ISR_NACKF) menErrorActive = cComNode::enErNACK;
  //  else if (mI2C->ISR & I2C_ISR_BERR)  menErrorActive = cComNode::enErBusError;
  //  else if (mI2C->ISR & I2C_ISR_ARLO)  menErrorActive = cComNode::enErArbitrationLost;
  //  else if (mI2C->ISR & I2C_ISR_OVR)   menErrorActive = cComNode::enErOverrun;
  //  else if (mI2C->ISR & I2C_ISR_BUSY)  menErrorActive = cComNode::enErStartWhileBussy;
  //  else                                menErrorActive = cComNode::enErUnknown;
  //
  //
  //  // Cancel the running transmission
  //  if (mI2C->ISR & I2C_ISR_BUSY)
  //  {
  //    LL_I2C_GenerateStopCondition(mI2C);
  //  }
  //
  //  vReInitHw(enResetHwFull, False);
  //  mSm = enStIdle;
  //  mpcI2cNode->vComError(menErrorActive, cComNode::enStIdle);
  //}
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

  lui32ISR = 0;

  switch (mSm)
  {
    case enStIdle:
      if (1) //lui32ISR & I2C_ISR_ADDR)
      {
        mpcI2cNode->mAdr = (lui32ISR >> 16) & 0xFE;
        if (1) //lui32ISR & I2C_ISR_DIR)
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
        // mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
      }
      else
      {
        vError();
      }
      break;

    case enStTx:
    case enStRx:
      {
        if (1) //lui32ISR & I2C_ISR_ADDR)
        {
          mpcI2cNode->mAdr = (lui32ISR >> 16) & 0xFE;
          if (1) //lui32ISR & I2C_ISR_DIR)
          {
            mSm = enStTx; // 1: Read transfer, slave enters transmitter mode.

            mpcMsg->vBusy();
            mpcI2cNode->vComStart(cComNode::enStPrepareForTx);
            I2C_StartDMA(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen, I2C_DIRECTION_TX);
          }
          // mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
        }
        else
        if (1) //lui32ISR & I2C_ISR_STOPF)
        {
          mSm = enStIdle;

          //LL_I2C_DisableDMAReq_TX(mI2C);
          //mI2C->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; //Clear Stop and Nack detection flag

          mpcMsg->vDone();
          mpcI2cNode->vComDone();
        }
      }
      break;
    default:
      break;
  }
}



