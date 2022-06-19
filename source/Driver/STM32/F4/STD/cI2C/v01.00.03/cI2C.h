#ifndef __cI2C_H__
#define __cI2C_H__

#include "Typedef.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
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
      I2C_OwnAddress2Config(mI2C, mui8Adr2);
      I2C_DualAddressCmd(mI2C, ENABLE);
    }
  }

  inline void vStopDMA()
  {
    if (mI2C == I2C1)
    {
      DMA1_Stream6->CR &=  ~DMA_SxCR_EN;
      DMA1_Stream5->CR &=  ~DMA_SxCR_EN;
    }
    if (mI2C == I2C2)
    {
      DMA1_Stream7->CR &=  ~DMA_SxCR_EN;
      DMA1_Stream2->CR &=  ~DMA_SxCR_EN;
    }
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    if (mI2C == I2C1) return DMA1_Stream5->NDTR;
    if (mI2C == I2C2) return DMA1_Stream2->NDTR;
    return 0;
  }

  void vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    if (mI2C == I2C1)
    {
      DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);
      DMA1_Stream6->CR &= ~DMA_SxCR_EN;
      DMA1_Stream6->NDTR = BufferSize;
      DMA1_Stream6->M0AR = (uint32)pBuffer;
      DMA1_Stream6->CR |= DMA_SxCR_EN;
    }
    else
    {
      DMA_ClearFlag(DMA1_Stream7, DMA_FLAG_TCIF7);
      DMA1_Stream7->CR &= ~DMA_SxCR_EN;
      DMA1_Stream7->NDTR = BufferSize;
      DMA1_Stream7->M0AR = (uint32)pBuffer;
      DMA1_Stream7->CR |= DMA_SxCR_EN;
    }
  }

  void vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    if (mI2C == I2C1)
    {
      DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5);
      DMA1_Stream5->CR &= ~DMA_SxCR_EN;
      DMA1_Stream5->NDTR = BufferSize;
      DMA1_Stream5->M0AR = (uint32)pBuffer;
      DMA1_Stream5->CR |= DMA_SxCR_EN;
    }
    else
    {
      DMA_ClearFlag(DMA1_Stream2, DMA_FLAG_TCIF2);
      DMA1_Stream2->CR &= ~DMA_SxCR_EN;
      DMA1_Stream2->NDTR = BufferSize;
      DMA1_Stream2->M0AR = (uint32)pBuffer;
      DMA1_Stream2->CR |= DMA_SxCR_EN;
    }
  }

  void  vClearClockLine()
  {
    mPinSCL->vSetType(GPIO_OType_OD);
    mPinSCL->vSetMode(GPIO_Mode_OUT);

    for (u8 lu8t = 0; lu8t < 9; lu8t++)
    {
      cClockInfo::Delay_us(4);
      mPinSCL->vSet0();
      cClockInfo::Delay_us(4);
      mPinSCL->vSet1();
    }

    if (mI2C == I2C1) { mPinSCL->vSetAF(GPIO_AF_I2C1);}
    if (mI2C == I2C2) { mPinSCL->vSetAF(GPIO_AF_I2C2);}
    if (mI2C == I2C3) { mPinSCL->vSetAF(GPIO_AF_I2C3);}
  }


  void  vInitHw(bool lbMaster) // __attribute__((optimize("-O0")))
  {
    I2C_InitTypeDef   I2C_InitStructure;

    /* I2C Periph clock enable */
    if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
    if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);
    if (mI2C == I2C3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, DISABLE);

    // DMA Rx
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    vStopDMA();

    //SDA and SCL must be pulled high
    if (lbMaster)
    {
      if (!bCheckPins()) vClearClockLine();
      if (!bCheckPins()) return;
    }

    mPinSCL->vSetType(GPIO_OType_OD);
    mPinSDA->vSetType(GPIO_OType_OD);
    if (mI2C == I2C1) { mPinSCL->vSetAF(GPIO_AF_I2C1); mPinSDA->vSetAF(GPIO_AF_I2C1); }
    if (mI2C == I2C2) { mPinSCL->vSetAF(GPIO_AF_I2C2); mPinSDA->vSetAF(GPIO_AF_I2C2); }
    if (mI2C == I2C3) { mPinSCL->vSetAF(GPIO_AF_I2C3); mPinSDA->vSetAF(GPIO_AF_I2C3); }

    /* I2C Periph clock enable */
    if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    if (mI2C == I2C3) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);

    /* I2C Peripheral Disable */
    I2C_Cmd(mI2C, DISABLE);

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
      DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);
    }
    if (mI2C == I2C2)
    {
      DMA_InitStructure.DMA_Channel = DMA_Channel_7;
      DMA_Init(DMA1_Stream2, &DMA_InitStructure);
      DMA_ITConfig(DMA1_Stream2, DMA_IT_TC, ENABLE);
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

    I2C_DMACmd(mI2C, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = lui8NVIC_IRQChannel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

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
      mSm             = cComNode::tenState::enStIdle;
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

  bool bCheckBus()
  {
    if (mI2C->SR2 & I2C_FLAG_BUSY)
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
    uint32 lui32ErrorMaskSR1;

    /*
    Note: in Slave mode
    In master receiver mode, NACK is automatically generated after last byte preceding STOP or RESTART condition, whatever the NACK bit value.When an overrun occurs in slave receiver NOSTRETCH mode, a NACK is automatica
    */

    if (((mSm == cComNode::enStRx) && ( lbMaster)) ||
        ((mSm == cComNode::enStTx) && (!lbMaster)))
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

  void I2C_EV_DMAHandler(cComNode::tenEvent lenEvent)
  {
    vSm(cComNode::tenEventType::enEvTyIrq, lenEvent);

    if (mSm == cComNode::tenState::enStIdle)
    {
      bStartNext();
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
  void I2C_EV_IRQHandler()
  {
    cComNode::tenEvent lenEvent;

    lenEvent = cComNode::tenEvent::enEvDummy;
    if (mI2C->SR1 & I2C_FLAG_SB)
    {
      lenEvent = cComNode::tenEvent::enEvStart;
    }
    else if (mI2C->SR1 & I2C_FLAG_ADDR)
    {
      lenEvent = cComNode::tenEvent::enEvAdress;
      // Reading I2C_SR2 after reading I2C_SR1 clears the ADDR flag, even if the ADDR flag was
      if (mI2C->SR2)
      {
        __asm("nop");
      }
    }
    else if (mI2C->SR1 & I2C_FLAG_BTF) // Data byte transfer finished
    {
      lenEvent = cComNode::tenEvent::enEvI2cTc;
    }
    else if (mI2C->SR1 & I2C_FLAG_STOPF) // Data byte transfer finished
    {
      lenEvent = cComNode::tenEvent::enEvI2cTc;
      // cleared by software reading the SR1 register followed by a write in the CR1 register, or by
      // hardware when PE=0
      mI2C->SR1 = 0x0; mI2C->SR2 = 0x0;
    }

    vSm(cComNode::tenEventType::enEvTyIrq, lenEvent);

    if (mSm == cComNode::tenState::enStIdle)
    {
      bStartNext();
    }
  }

    /*
      I2Cx_ISR->NACKF: Not Acknowledge received flag: It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0
      I2Cx_ISR->BERR:  Bus error:                     It is cleared by software by setting BERRCF bit.:     This bit is cleared by hardware when PE=0.
      I2Cx_ISR->OVR:   Overrun/Underrun (slave mode): It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0.
      I2Cx_ISR->ARLO:  Arbitration lost:              It is cleared by software by setting the ARLOCF bit.: This bit is cleared by hardware when PE=0.
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
    uint32 lui32ErrorMaskSR1;
    uint32 lui32ErrorMaskSR2;

    lui32ErrorMaskSR1 = (I2C_FLAG_AF | I2C_FLAG_BERR | I2C_FLAG_ARLO | I2C_FLAG_OVR | I2C_FLAG_PECERR | I2C_FLAG_TIMEOUT | I2C_FLAG_SMBALERT) & 0xFFFF;
    lui32ErrorMaskSR2 = I2C_FLAG_BUSY;

    if ((mI2C->SR1 & lui32ErrorMaskSR1) || (mI2C->SR2 & lui32ErrorMaskSR2))
    {
      if (mI2C->SR1 & I2C_FLAG_AF)
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
        if (mI2C->SR2 & I2C_FLAG_BUSY)
        {
          munBusStatus = tenComBusStatus::enBusNotReady;
          mSm = cComNode::enStError;
        }
      }

      vStopDMA();
      mI2C->SR1 = 0x0; mI2C->SR2 = 0x0;
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

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent)  // __attribute__((optimize("-O0")))
  {
    uint8 lui8SlaveAdr;
    bool  lbLoop;
    cComNode::tenState mSm_Old;

    if (isErrorActive(True))
    {
      vError();
      return;
    }

    lui8SlaveAdr = mpcActiveSlave->mAdr;
    menComState = cComNode::tenState::enStIdle;

    mSm_Old = mSm;
    do
    {
      lbLoop = False;
      switch (mSm)
      {
        case cComNode::tenState::enStIdle:
          switch (lenEvent)
          {
            case cComNode::tenEvent::enEvStart:
              menComState = cComNode::tenState::enStStart;
              mSm         = cComNode::tenState::enStAdress;
              mI2C->CR1   = 1; /*PE*/
              I2C_GenerateSTART(mI2C, ENABLE);
            default:
              break;
          }
          break;

        case cComNode::tenState::enStAdress:
          if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
              (lenEvent     == cComNode::tenEvent::enEvStart))
          {
            //TX and maybe RX
            if (mpcActiveMsg->cTxData.muiLen > 0)
            {
              mSm             = cComNode::tenState::enStTx;
              mui32ComByteCnt = mpcActiveMsg->cTxData.muiLen;
              mpui8ComBuf     = mpcActiveMsg->cTxData.mpu8Data;

              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Transmitter);
              vStartDMATx(mpui8ComBuf, mui32ComByteCnt);
            }
            //RX only
            else if (mpcActiveMsg->cRxData.muiLen > 0)
            {
              mSm             = cComNode::tenState::enStRx;
              mui32ComByteCnt = mpcActiveMsg->cRxData.muiLen;
              mpui8ComBuf     = mpcActiveMsg->cRxData.mpu8Data;

              if (mpcActiveMsg->cRxData.muiLen > 1)
              {
                mI2C->CR1 |= (1 << 10); /* activate ACK */
              }

              if (mpcActiveMsg->cRxData.muiLen == 2)
              {
                mI2C->CR1 |= (1 << 11); /* activate POS */ /* The POS bit must be used only in 2-byte reception configuration
                                                              in master mode. It must be configured before data reception starts*/
              }

              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Receiver);
              vStartDMARx(mpui8ComBuf, mui32ComByteCnt);
            }
            else
            {
              mSm = cComNode::tenState::enStIdle;
            }
          }
          break;


        case cComNode::tenState::enStTx:
          {
            menComState = cComNode::tenState::enStData;
            if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
                (lenEvent     == cComNode::tenEvent::enEvI2cTc))
            {
              I2C_GenerateSTOP(mI2C, ENABLE);
              // Wenn es nicht zu empfangen gibt dann fertig
              if (mpcActiveMsg->cRxData.muiLen == 0)
              {
                mSm    = cComNode::tenState::enStEnd;
                lbLoop = True;
              }
              else
              {
                // Es gibt noch was zu empfangen
                mSm  = cComNode::tenState::enStWaitStart;
                I2C_GenerateSTART(mI2C, ENABLE);
              }
            }
          }
          break;

        case cComNode::tenState::enStWaitStart:
          {
            menComState = cComNode::tenState::enStData;
            if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
                (lenEvent     == cComNode::tenEvent::enEvStart))
            {
              mSm             = cComNode::tenState::enStRx;
              mui32ComByteCnt = mpcActiveMsg->cRxData.muiLen;
              mpui8ComBuf     = mpcActiveMsg->cRxData.mpu8Data;

              if (mpcActiveMsg->cRxData.muiLen > 1)
              {
                mI2C->CR1 |= (1 << 10); /* activate ACK */
              }

              if (mpcActiveMsg->cRxData.muiLen == 2)
              {
                mI2C->CR1 |= (1 << 11); /* activate POS */ /* The POS bit must be used only in 2-byte reception configuration
                                                              in master mode. It must be configured before data reception starts*/
              }

              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Receiver);
              vStartDMARx(mpui8ComBuf, mui32ComByteCnt);
            }
          }
          break;

        case cComNode::tenState::enStRx:
          {
            menComState = cComNode::tenState::enStData;
            if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
                (lenEvent     == cComNode::tenEvent::enEvDmaRxTc))
            {
              if (mpcActiveMsg->cRxData.muiLen)
              {
                // Nach dem letzten Byte kein Ack senden
                mI2C->CR1 &= ~(1 << 10); /* deactivate ACK */
              }
              I2C_GenerateSTOP(mI2C, ENABLE);

              mSm    = cComNode::tenState::enStEnd;
              lbLoop = True;
            }
          }
          break;

        case cComNode::tenState::enStEnd:
          mSm = cComNode::tenState::enStIdle;
          mpcActiveMsg->vDone();
          mpcActiveSlave->vComDone();
          break;

        default:
          break;
      }
    } while (lbLoop);

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



#endif // __cI2C_H__
