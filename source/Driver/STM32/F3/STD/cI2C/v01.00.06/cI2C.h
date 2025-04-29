#ifndef __cI2C_H__
#define __cI2C_H__

#include "Typedef.h"
#include "stm32f30x_i2c.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_misc.h"

#include "stm32f30x_dma.h"

#include "tGPPin.h"
#include "ComDat.h"
#include "ClockInfo.h"

class cComNodeI2c_Slave : public cComNode
{
public:
  cComDatMsg mpcMsg;

  cComNodeI2c_Slave(uint32 lui32TxDataSize, uint32 lui32RxDataSize)
  {
    mpcMsg.vMemAlloc(lui32TxDataSize, lui32RxDataSize);
  };

  //~cComNodeI2c_Slave() {};

  inline uint32 SizeOf()
  {
    return mpcMsg.SizeOf() -
           sizeof(mpcMsg) +
           sizeof(cComNodeI2c_Slave);
  }
};


class cI2cMaster_ComNodeList :public cComNodeList
{
  public:
    cI2cMaster_ComNodeList(uint32 lui32SlavesCnt)
      :cComNodeList(lui32SlavesCnt)
      {}

  inline uint32 SizeOf()
  {
    return cComNodeList::SizeOf() +
           sizeof(cI2cMaster_ComNodeList);
  }
};

class cI2c
{
  public:

  cComNode::tenState mSm;
  tenComDeviceStatus munDeviceStatus;
  tenComBusStatus    munBusStatus;
  tenComStatus       munComStatus;

  u16                  mu16TickCounter;
  cGpPin *mPinSCL;
  cGpPin *mPinSDA;

  I2C_TypeDef   *mI2C;

  uint8 mui8Adr1;
  uint8 mui8Adr2;

  cI2c(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda)
  {
    mI2C = lstI2c;

    mPinSCL = lcScl;
    mPinSDA = lcSda;

    munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
  }

  void  vSetAdr2(u8 lui8Adr2)
  {
    I2C_DualAddressCmd(mI2C, DISABLE);
    if (lui8Adr2)
    {
      mui8Adr2 = lui8Adr2;
      I2C_OwnAddress2Config(mI2C, mui8Adr2, 0);
      I2C_DualAddressCmd(mI2C, ENABLE);
    }
  }

  inline void vStopDMA()
  {
    if (mI2C == I2C1)
    {
      DMA1_Channel6->CCR &=  ~DMA_CCR_EN;
      DMA1_Channel7->CCR &=  ~DMA_CCR_EN;
    }
    if (mI2C == I2C2)
    {
      DMA1_Channel4->CCR &=  ~DMA_CCR_EN;
      DMA1_Channel5->CCR &=  ~DMA_CCR_EN;
    }
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    if (mI2C == I2C1) return DMA1_Channel7->CNDTR;
    if (mI2C == I2C2) return DMA1_Channel5->CNDTR;
    return 0;
  }

  void vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    if (mI2C == I2C1)
    {
      DMA1->IFCR = DMA1_FLAG_TC6;
      DMA1_Channel6->CCR &= ~DMA_CCR_EN;
      DMA1_Channel6->CNDTR = BufferSize;
      DMA1_Channel6->CMAR = (uint32)pBuffer;
      DMA1_Channel6->CCR |= DMA_CCR_EN;
    }
    else
    {
      DMA1->IFCR = DMA1_FLAG_TC4;
      DMA1_Channel4->CCR &= ~DMA_CCR_EN;
      DMA1_Channel4->CNDTR = BufferSize;
      DMA1_Channel4->CMAR = (uint32)pBuffer;
      DMA1_Channel4->CCR |= DMA_CCR_EN;
    }
  }

  void vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    if (mI2C == I2C1)
    {
      DMA1->IFCR = DMA1_FLAG_TC7;
      DMA1_Channel7->CCR &= ~DMA_CCR_EN;
      DMA1_Channel7->CNDTR = BufferSize;
      DMA1_Channel7->CMAR = (uint32)pBuffer;
      DMA1_Channel7->CCR |= DMA_CCR_EN;
    }
    else
    {
      DMA1->IFCR = DMA1_FLAG_TC5;
      DMA1_Channel5->CCR &= ~DMA_CCR_EN;
      DMA1_Channel5->CNDTR = BufferSize;
      DMA1_Channel5->CMAR = (uint32)pBuffer;
      DMA1_Channel5->CCR |= DMA_CCR_EN;
    }
  }

  void  vClearClockLine()
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


  void  vInitHw(bool lbMaster) // __attribute__((optimize("-O0")))
  {
    I2C_InitTypeDef   I2C_InitStructure;

    /* I2C Periph clock enable */
    if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
    if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);
    if (mI2C == I2C3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, DISABLE);

    // DMA Rx
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    vStopDMA();

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
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
    I2C_InitStructure.I2C_DigitalFilter = 0x00;
    I2C_InitStructure.I2C_OwnAddress1 = mui8Adr1;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_Timing = 0x00100308;

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
      mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE;
    }
    else
    {
      mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_ADDRIE;
    }


    // DMA
    DMA_InitTypeDef  DMA_InitStructure;
    DMA_StructInit(&DMA_InitStructure);


    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (mI2C->RXDR); //=0x40005410 : address of data reading register of I2C1
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
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32) & (mI2C->TXDR); //=0x40005410 : address of data reading register of I2C1
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
    mSm             = cComNode::tenState::enStIdle;
  }

  void  vReInitHw(cComNode::tenConsts lenOption, bool lbMaster)
  {
    if (lenOption == cComNode::enResetHwFull)
    {
      vInitHw(lbMaster);
    }
    else
    {
      // I2C Peripheral Disnable
      I2C_Cmd(mI2C, DISABLE);
      vStopDMA();

      if (lbMaster)
      {
        //SDA and SCL must be pulled high
        if (!bCheckPins()) return;

        mPinSDA->vSetMode(GPIO_Mode_AF);
        mPinSCL->vSetMode(GPIO_Mode_AF);
      }

      mI2C->ICR = 0xFFFF;

      // I2C Peripheral Enable
      I2C_Cmd(mI2C, ENABLE);

      munDeviceStatus = tenComDeviceStatus::enDeviceReady;
      munBusStatus    = tenComBusStatus::enBusNoError;
      munComStatus    = tenComStatus::enStatusIdle;
      mSm             = cComNode::tenState::enStIdle;
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

  bool bCheckBus()
    {
    if (mI2C->ISR & I2C_ISR_BUSY)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceError;
      munBusStatus    = tenComBusStatus::enBusNotReady;
      mSm = cComNode::enStError;
      return False;
    }
    return True;
  }

  bool bCheckPins()
  {
    mPinSCL->vSetMode(GPIO_Mode_IN);
    mPinSDA->vSetMode(GPIO_Mode_IN);

    //SDA and SCL must be pulled high
    if ((mPinSCL->ui8Get() == 0) || (mPinSDA->ui8Get() == 0))
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enDeviceInitErrorPins;
      mSm             = cComNode::enStError;
      return False;
    }
    return True;
  }

  uint8 isErrorActive(bool lbMaster)
  {
    uint32 lui32ErrorMask;

    /*
    Note: in Slave mode
    In master receiver mode, NACK is automatically generated after last byte preceding STOP or RESTART condition, whatever the NACK bit value.When an overrun occurs in slave receiver NOSTRETCH mode, a NACK is automatica
    */

    if (((mSm == cComNode::enStRx) && ( lbMaster)) ||
        ((mSm == cComNode::enStTx) && (!lbMaster)))
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
};


class cI2cMaster : public cI2c
{
  public:

  uint16             mu16ReInitTicks;
  uint16             mu16ComStuckTicks;
  uint16             mu16ComStuckTicksReload;


  cI2cMaster_ComNodeList mcSlaves;
  cComNode              *mpcActiveSlave;
  cComDatMsg            *mpcActiveMsg;

  cComNode::tenState   menComState;
  uint8               *mpui8ComBuf;
  uint32               mui32ComByteCnt;

  u8                   mu8Lock;

  cI2cMaster(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint32 lui32Slaves)
    : cI2c(lstI2c, lcScl, lcSda), mcSlaves(lui32Slaves)
  {
    mSm  = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;

    mpcActiveSlave = NULL;
    mpcActiveMsg   = NULL;

    mui8Adr1 = mui8Adr2 = 0;
    mu8Lock        = 0;

    mu16ReInitTicks = 100;
    mu16ComStuckTicksReload = 1000;
    mu16ComStuckTicks = mu16ComStuckTicksReload;

    vInitHw(True);
  }

  ~cI2cMaster()
  {
  }

  inline uint32 SizeOf()
  {
    return mcSlaves.SizeOf() + sizeof(cI2cMaster);
  }

  inline void SetLock()   {mu8Lock = 1;}
  inline void ResetLock() {mu8Lock = 0;}

  /*
    RXNE:   Receive buffer not empty
    TXIS:   Transmit buffer interrupt status
    STOPF:  Stop detection interrupt flag
    TC/TCR: Transfer Complete/ Transfer complete Reload
    ADDR:   Address matched
    NACKF:  NACK reception
  */
  void I2C_EV_IRQHandler()
  {
    vSm(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvIrq);

    if (mSm == cComNode::tenState::enStIdle)
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
  void I2C_ER_IRQHandler()
  {
    vSm(cComNode::tenEventType::enEvTyError , cComNode::tenEvent::enEvError);

    if (mSm == cComNode::tenState::enStIdle)
    {
      bStartNext();
    }
  }

  void vSetReInitTicks(u16 lu16ReInitTicks)
  {
    if (!mu16ReInitTicks)
    {
      mu16ReInitTicks = lu16ReInitTicks;
    }
  }

  void vTick1ms()
  {
    mu16TickCounter++;
    //vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvTick);
  }

  /*
    I2Cx_ISR->NACKF: Not Acknowledge received flag: It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0
    I2Cx_ISR->BERR:  Bus error:                     It is cleared by software by setting BERRCF bit.:     This bit is cleared by hardware when PE=0.
    I2Cx_ISR->OVR:   Overrun/Underrun (slave mode): It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0.
    I2Cx_ISR->ARLO:  Arbitration lost:              It is cleared by software by setting the ARLOCF bit.: This bit is cleared by hardware when PE=0.
  */
  void vError()
  {
    uint32 lui32ErrorMask;

    lui32ErrorMask = I2C_ISR_BUSY | I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;

    if (mI2C->ISR & lui32ErrorMask)
    {
      if (mI2C->ISR & I2C_ISR_NACKF)
      {
        mSm = cComNode::tenState::enStIdle;
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
          mSm = cComNode::enStError;
        }
      }

      vStopDMA();
      mI2C->ICR = 0xFFFF;
      mu8Lock        = 0;
      mpcActiveMsg   = NULL;
      mpcActiveSlave = NULL;
    }
  }

  void vAddSlave(cComNode *lpcSlave)
  {
    mcSlaves.vAdd(lpcSlave);
  }

  void vRemoveSlave(cComNode *lpcSlave)
  {
    mcSlaves.vRemove(lpcSlave);
  }

  void vReInitAllSlave()
  {
    mcSlaves.vSetErrorAll(cComNode::tenError::enErReInit, cComNode::cComNode::tenState::enStIdle);
  }

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    uint8 lui8SlaveAdr;
    cComNode::tenState mSm_Old;

    UNUSED(lenEventType);

    if (isErrorActive(True))
    {
      vError();
      return;
    }

    lui8SlaveAdr = mpcActiveSlave->mAdr;
    menComState = cComNode::tenState::enStIdle;

    mSm_Old = mSm;
    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
      case cComNode::tenState::enStWait:
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvStart:
            menComState = cComNode::tenState::enStAdress;
            //TX and maybe RX
            if (mpcActiveMsg->cTxData.muiLen > 0)
            {
              mSm = cComNode::tenState::enStTx;
              mui32ComByteCnt = mpcActiveMsg->cTxData.muiLen;
              mpui8ComBuf     = mpcActiveMsg->cTxData.mpu8Data;
              vStartDMATx(mpui8ComBuf, mui32ComByteCnt);
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
              mSm = cComNode::tenState::enStRx;
              vStartDMARx(mpcActiveMsg->cRxData.mpu8Data, mpcActiveMsg->cRxData.muiLen);
              I2C_TransferHandling(mI2C, lui8SlaveAdr, mui32ComByteCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
            }
            // Nothing todo
            else
            {
              mSm = cComNode::tenState::enStIdle;
              mpcActiveMsg->vDone();
              mpcActiveSlave->vComDone();
            }
            break;
          default:
              mI2C->ICR = 0xFFFF;
            break;
        }
        break;
      case cComNode::tenState::enStTx:
        {
          uint32 lui32ISR = mI2C->ISR;
          menComState = cComNode::tenState::enStData;
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvIrq:
              if (lui32ISR & I2C_ISR_STOPF)
              {
                /* Clear STOPF flag */
                mI2C->ICR = I2C_ICR_STOPCF;
                mSm = cComNode::tenState::enStIdle;
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
                if (mpcActiveMsg->cRxData.muiLen > 0)
                {
                  mSm = cComNode::enStRx;
                  vStartDMARx(mpcActiveMsg->cRxData.mpu8Data, mpcActiveMsg->cRxData.muiLen);
                  I2C_TransferHandling(mI2C, lui8SlaveAdr, mui32ComByteCnt, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
                }
                else
                {
                  mSm = cComNode::tenState::enStIdle;
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
              break;
          }
        }
        break;

      case cComNode::tenState::enStRx:
        menComState = cComNode::tenState::enStData;
        switch (lenEvent)
        {
          case cComNode::tenEvent::enEvIrq:
            if (mI2C->ISR & (I2C_ISR_STOPF | I2C_ISR_TC))
            {
              /* Clear STOPF flag */
              mI2C->ICR = I2C_ICR_STOPCF;
              mSm = cComNode::tenState::enStIdle;
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

  void vStartMsg(cComDatMsg *lpcActiveMsg) // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
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
        vSm(cComNode::tenEventType::enEvTySw, cComNode::tenEvent::enEvStart);
      }
    }
  }

  bool bStartNext() // __attribute__((optimize("-O0")))
  {
    if ((mSm == cComNode::tenState::enStIdle) && (!mu8Lock))
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
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvPrepareToSendData);
      }
    }

    if (mSm == cComNode::enStError)
    {
      if (mu16ReInitTicks)
      {
        mu16ReInitTicks--;

        if (!mu16ReInitTicks)
        {
          mpcActiveMsg   = NULL;
          mu8Lock        = 0;
          this->vReInitHw(cComNode::tenConsts::enResetHwFull, True);
          this->vReInitAllSlave();
          if (mSm == cComNode::enStError)
          {
            mu16ReInitTicks = 100;
          }
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
        if (mSm != cComNode::tenState::enStIdle)
        {
          mu16ComStuckTicks--;
        }
      }
      else
      {
        mSm = cComNode::enStError;
        munDeviceStatus   = tenComDeviceStatus::enDeviceReady;
        munBusStatus      = tenComBusStatus::enBusNoError;
        munComStatus      = tenComStatus::enStatusStuck;
        mu16ComStuckTicks = mu16ComStuckTicksReload;
        mu16ReInitTicks   = 100;
      }
    }

    if (mSm == cComNode::enStError)
    {
      return False;
    }
    return True;
  }
};



class cI2cSlave : public cI2c
{
  public:
  cComDatMsg          *mpcMsg;

  cComNode            *mpcComNode;
  cComNode::tenError   menErrorActive;

  cI2cSlave(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint8 lui8Adr1, uint8 lui8Adr2)
  : cI2c(lstI2c, lcScl, lcSda)
  {
    mI2C       = lstI2c;
    mSm        = cComNode::tenState::enStIdle;
    mpcComNode = NULL;

    mui8Adr1 = lui8Adr1;
    mui8Adr2 = lui8Adr2;

    mPinSCL = lcScl;
    mPinSDA = lcSda;
  }


  ~cI2cSlave()
  {
  }

  void vTick1ms()
  {
    static cComNode::tenState mSm_old;
    mu16TickCounter++;
    
    if (mSm == cComNode::tenState::enStIdle)
    {
      mu16TickCounter = 0;
    }
    else
    {
      if (mSm_old != mSm)
      {
        mSm_old = mSm;
        mu16TickCounter = 0;
      }

      // 25ms keine Zusatandsänderung
      // Dann HW Reseten
      if (mu16TickCounter > 25)
      {
        mSm_old = mSm;
        mu16TickCounter = 0;
        mSm = cComNode::enStError;
        munDeviceStatus   = tenComDeviceStatus::enDeviceReady;
        munBusStatus      = tenComBusStatus::enBusNoError;
        munComStatus      = tenComStatus::enStatusStuck;
        mpcComNode->vComError(menErrorActive, mSm_old);
        I2C_GenerateSTOP(mI2C, ENABLE);
        this->vReInitHw(cComNode::tenConsts::enResetHwFull, False);
      }
    }
  }

  void vSetNode(cComNode *lpcComNode)
  {
    mpcComNode = lpcComNode;
    /* HW erst hier, weil solange mpcActiveSlave noch auf null zeigt */
    if (mpcComNode != null)
    {
      vInitHw(False);
    }
  }

  void I2C_EV_IRQHandler()
  {
    vSm(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvIrq);
  }

  void I2C_ER_IRQHandler()
  {
    vSm(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvError);
  }

  void  vError()
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

      vReInitHw(cComNode::tenConsts::enResetHwFull, False);
      mSm = cComNode::tenState::enStIdle;
      mpcComNode->vComError(menErrorActive, mSm);
    }
  }

  void vSetMsg(cComDatMsg* lpcMsg) { mpcMsg = lpcMsg; };


  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    uint32 lui32ISR;

    UNUSED(lenEvent);
    UNUSED(lenEventType);

    if (isErrorActive(False))
    {
      vError();
    }

    lui32ISR = mI2C->ISR;

    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
        if (lui32ISR & I2C_ISR_ADDR)
        {
          mpcComNode->mAdr = (lui32ISR >> 16) & 0xFE;
          if (lui32ISR & I2C_ISR_DIR)
          {
            mSm = cComNode::enStTx; // Write transfer, slave enters transmitter mode.
            mpcMsg->vBusy();
            mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForTx);
            vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);
            mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
            mpcComNode->vComStart(cComNode::tenEvent::enEvAfterTxStarted);
          }
          else
          {
            mSm = cComNode::enStRx; // Read transfer, slave enters receiver mode.
            mpcMsg->vBusy();
            mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForRx);
            vStartDMARx(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen);
            mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
            mpcComNode->vComStart(cComNode::tenEvent::enEvAfterRxStarted);
          }
        }
        else
        {
          vError();
        }
        break;

      case cComNode::enStTx:
      case cComNode::enStRx:
        {
          if (lui32ISR & I2C_ISR_ADDR)
          {
            mpcComNode->mAdr = (lui32ISR >> 16) & 0xFE;
            if (lui32ISR & I2C_ISR_DIR)
            {
              mSm = cComNode::enStTx; // 1: Read transfer, slave enters transmitter mode.

              mpcMsg->vBusy();
              mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForTx);
              vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);
              mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
              mpcComNode->vComStart(cComNode::tenEvent::enEvAfterTxStarted);
            }
          }
          else
          if (lui32ISR & I2C_ISR_STOPF)
          {
            mSm = cComNode::tenState::enStIdle;
            mI2C->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; //Clear Stop and Nack detection flag

            mpcMsg->vDone();
            mpcComNode->vComDone();
          }
        }
        break;
      default:
        break;
    }
  }
};


#endif // __cI2C_H__
