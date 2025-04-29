#ifndef __cI2C_H__
#define __cI2C_H__

#include "Typedef.h"
#include "ch32v30x.h"
#include "ch32v30x_rcc.h"
#include "ch32v30x_i2c.h"
#include "ch32v30x_dma.h"
#include "tGPPin.h"
#include "ComDat.h"
#include "ClockInfo.h"

//#define I2C_TEST

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

  bool mbMaster;
  cComNode::tenConsts menOption;

  cI2c(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, bool lbMaster)
  {
    mI2C = lstI2c;

    mPinSCL = lcScl;
    mPinSDA = lcSda;

    munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;

    mbMaster        = lbMaster;
    menOption       = cComNode::tenConsts::enNop;

    #ifdef I2C_TEST
      // Alles auf Output PP
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
      GPIOA->CFGHR = 0x33333333;
      GPIOA->CFGLR = 0x33333333;
      GPIOB->CFGLR = 0x00003333;
    #endif  
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
    mI2C->CTLR2 &= ~(1 << 11); // DMAEN;
    if (mI2C == I2C1)
    {
      DMA1_Channel6->CFGR &= ~(1 << 0); // EN
      DMA1_Channel7->CFGR &= ~(1 << 0); // EN
    }
    if (mI2C == I2C2)
    {
      DMA1_Channel4->CFGR &= ~(1 << 0); // EN
      DMA1_Channel5->CFGR &= ~(1 << 0); // EN
    }
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    if (mI2C == I2C1) return DMA1_Channel7->CNTR;
    if (mI2C == I2C2) return DMA1_Channel5->CNTR;
    return 0;
  }

  void vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    if (mI2C == I2C1)
    {
      DMA1->INTFCR             = DMA1_IT_TC6;
      DMA1_Channel6->CFGR     &= ~(1 << 0); // ~EN
      DMA1_Channel6->CNTR      = BufferSize;
      DMA1_Channel6->MADDR     = (uint32)pBuffer;
      DMA1_Channel6->CFGR     |=  ((1 << 0) /* | DMA_CHXCTL_FTFIE */); // EN
    }
    else // I2C2
    {
      DMA1->INTFCR             = DMA1_IT_TC4;
      DMA1_Channel4->CFGR     &= ~(1 << 0); // ~EN
      DMA1_Channel4->CNTR      = BufferSize;
      DMA1_Channel4->MADDR     = (uint32)pBuffer;
      DMA1_Channel4->CFGR     |=  ((1 << 0) /* | DMA_CHXCTL_FTFIE */); // EN
    }
    mI2C->CTLR2 |= (1 << 11); // DMAEN;
  }

  void vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    if (mI2C == I2C1)
    {
      DMA1->INTFCR             = DMA1_IT_TC7;
      DMA1_Channel7->CFGR     &= ~(1 << 0); // ~EN
      DMA1_Channel7->CNTR      = BufferSize;
      DMA1_Channel7->MADDR     = (uint32)pBuffer;
      DMA1_Channel7->CFGR     |= (1 << 0) | (1 << 1); // EN
    }
    else // I2C2
    {      
      DMA1->INTFCR             = DMA1_IT_TC5;
      DMA1_Channel5->CFGR     &= ~(1 << 0); // ~EN
      DMA1_Channel5->CNTR      = BufferSize;
      DMA1_Channel5->MADDR     = (uint32)pBuffer;
      DMA1_Channel5->CFGR     |= (1 << 0) | (1 << 1); // EN
    }
    mI2C->CTLR2 |= (1 << 11); // DMAEN;
  }

  void  vClearClockLine()
  {
    mPinSCL->vSetCnf(cGpCfg::tenCnf::nOd);
    mPinSCL->vSetMode(cGpCfg::tenMode::nOutput_50MHz);

    for (u8 lu8t = 0; lu8t < 9; lu8t++)
    {
      cClockInfo::Delay_us(4);
      mPinSCL->vSet0();
      cClockInfo::Delay_us(4);
      mPinSCL->vSet1();
    }

    mPinSCL->vSetCnf(cGpCfg::tenCnf::nAfOd);
  }


  void  vInitHw() // __attribute__((optimize("-O0")))
  {
    I2C_InitTypeDef   I2C_InitStructure;

    /* I2C Periph clock disable */
    if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, DISABLE);
    if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, DISABLE);

    // DMA Rx
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    vStopDMA();

    //SDA and SCL must be pulled high
    if (mbMaster)
    {
      if (!bCheckPins()) vClearClockLine();
      if (!bCheckPins()) return;
    }


    mPinSCL->vSetCnf(cGpCfg::tenCnf::nAfOd);
    mPinSCL->vSetMode(cGpCfg::tenMode::nOutput_50MHz);
    mPinSDA->vSetCnf(cGpCfg::tenCnf::nAfOd);
    mPinSDA->vSetMode(cGpCfg::tenMode::nOutput_50MHz);


    /* I2C Periph clock enable */
    if (mI2C == I2C1) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    if (mI2C == I2C2) RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);


    /* I2C Peripheral Disable */
    I2C_Cmd(mI2C, DISABLE);
    I2C_SoftwareResetCmd(mI2C, ENABLE);
    I2C_SoftwareResetCmd(mI2C, DISABLE);



    /* I2C configuration */
    I2C_StructInit(&I2C_InitStructure);
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;
    I2C_InitStructure.I2C_ClockSpeed          = 400000;
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1         = mui8Adr1;
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;

    /* 8Mhz (HSI  */
    /*
      100khz  = 0x10420F13  PRESC = 1; SCLL = 0x13; SCLH = 0xF; SDADEL = 0x2; SCLDEL = 0x4 => 100khz
      400khz  = 0x00310309  PRESC = 0; SCLL = 0x09; SCLH = 0x3; SDADEL = 0x1; SCLDEL = 0x3 => 364Khz
      500khz  = 0x00100308  PRESC = 0; SCLL = 0x06; SCLH = 0x3; SDADEL = 0x0; SCLDEL = 0x1 => 384khz
      1000khz = 0x00100103  PRESC = 0; SCLL = 0x03; SCLH = 0x1; SDADEL = 0x0; SCLDEL = 0x1 => 555khz
    */


    /* Apply I2C configuration after enabling it */
    I2C_Init(mI2C, &I2C_InitStructure);
    if (!mbMaster)
    {
      vSetAdr2(mui8Adr2);
    }

    mI2C->STAR1 = mI2C->STAR2 = 0;

    if (isErrorActive())
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
      return;
    }

    if (mI2C->STAR2 & I2C_STAR2_BUSY)
    {
      munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
      munBusStatus    = tenComBusStatus::enBusNotReady;
      return;
    }

    IRQn_Type lui8NVIC_IRQChannel;

    /* Configure the I2C event priority */
    if (mI2C == I2C1) lui8NVIC_IRQChannel = I2C1_EV_IRQn;
    if (mI2C == I2C2) lui8NVIC_IRQChannel = I2C2_EV_IRQn;
    NVIC_SetPriority(lui8NVIC_IRQChannel, (2 << 5));
    NVIC_EnableIRQ(lui8NVIC_IRQChannel);

    if (mI2C == I2C1) lui8NVIC_IRQChannel = I2C1_ER_IRQn;
    if (mI2C == I2C2) lui8NVIC_IRQChannel = I2C2_ER_IRQn;
    NVIC_SetPriority(lui8NVIC_IRQChannel, (2 << 5));
    NVIC_EnableIRQ(lui8NVIC_IRQChannel);

    mI2C->CTLR2 |= I2C_CTLR2_ITERREN | I2C_CTLR2_ITEVTEN;

    // ----------- DMA ---------
    DMA_Channel_TypeDef* lstDMA_Channel;
    DMA_InitTypeDef      DMA_InitStructure;

    // configure the USART TX DMA channel
    if (mI2C == I2C1) lstDMA_Channel = DMA1_Channel6;
    if (mI2C == I2C2) lstDMA_Channel = DMA1_Channel4;
    
    // configure the USART TX DMA channel
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&mI2C->DATAR; /* USART2->DATAR:0x40004404 */
    DMA_InitStructure.DMA_MemoryBaseAddr     = (u32)0;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = 0;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_Init(lstDMA_Channel, &DMA_InitStructure);
    
    // DMA Interrupts werden nicht benötigt. Für das Ende des Transfers wird das BTF benutzt.
    // Prio 0(höchste) 7(kleinste)
    // if (mI2C == I2C1) lui8NVIC_IRQChannel = DMA1_Channel6_IRQn;
    // if (mI2C == I2C2) lui8NVIC_IRQChannel = DMA1_Channel4_IRQn;
    // NVIC_SetPriority(lui8NVIC_IRQChannel, (2 << 5));
    // NVIC_EnableIRQ(lui8NVIC_IRQChannel);

    // DMA Rx
    if (mI2C == I2C1) lstDMA_Channel = DMA1_Channel7;
    if (mI2C == I2C2) lstDMA_Channel = DMA1_Channel5;
    DMA_InitStructure.DMA_DIR        = DMA_DIR_PeripheralSRC;
    DMA_Init(lstDMA_Channel, &DMA_InitStructure);

    // DMA Interrupts werden nicht benötigt. Für das Ende des Transfers wird das BTF benutzt.
    // Prio 0(höchste) 7(kleinste)
    if (mbMaster)
    {
      if (mI2C == I2C1) lui8NVIC_IRQChannel = DMA1_Channel7_IRQn;
      if (mI2C == I2C2) lui8NVIC_IRQChannel = DMA1_Channel5_IRQn;
      NVIC_SetPriority(lui8NVIC_IRQChannel, (2 << 5));
      NVIC_EnableIRQ(lui8NVIC_IRQChannel);
    }

    munDeviceStatus = tenComDeviceStatus::enDeviceReady;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
    mSm             = cComNode::tenState::enStIdle;

    /* I2C Peripheral Enable */
    I2C_Cmd(mI2C, ENABLE);
    I2C_DMACmd(I2C1, ENABLE);
  }

  void  vReInitHw(cComNode::tenConsts lenOption)
  {
    menOption = lenOption;
    if (lenOption != cComNode::tenConsts::enNop)
    {
      // Interrupt deaktivieren, sonst kommt die 10ms LP nicht dran.
        mI2C->CTLR2 &= ((~I2C_CTLR2_ITERREN) & (~I2C_CTLR2_ITEVTEN));
    }
  }

  void vTick10ms()
  {
    cComNode::tenConsts lenOption;
    lenOption = menOption;
    menOption = cComNode::tenConsts::enNop;
    if (lenOption != cComNode::tenConsts::enNop)
    {
      if (lenOption == cComNode::enResetHwFull)
      {
        vInitHw();
      }
      else
      {
        // I2C Peripheral Disable
        I2C_Cmd(mI2C, DISABLE);
        vStopDMA();

        if (mbMaster)
        {
          //SDA and SCL must be pulled high
          if (!bCheckPins()) return;

          mPinSCL->vSetCnf(cGpCfg::tenCnf::nAfOd);
          mPinSDA->vSetCnf(cGpCfg::tenCnf::nAfOd);
        }

        mI2C->STAR1 = mI2C->STAR2 = 0;

        mI2C->CTLR2 |= I2C_CTLR2_ITERREN | I2C_CTLR2_ITEVTEN;

        // I2C Peripheral Enable
        I2C_Cmd(mI2C, ENABLE);
        if (!mbMaster) mI2C->CTLR1 |= I2C_CTLR1_ACK;

        munDeviceStatus = tenComDeviceStatus::enDeviceReady;
        munBusStatus    = tenComBusStatus::enBusNoError;
        munComStatus    = tenComStatus::enStatusIdle;
        mSm             = cComNode::tenState::enStIdle;
      }

      if (isErrorActive())
      {
        munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
        munBusStatus    = tenComBusStatus::enDeviceInitErrorDevice;
        return;
      }

      if (mI2C->STAR2 & I2C_STAR2_BUSY)
      {
        munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
        munBusStatus    = tenComBusStatus::enBusNotReady;
        return;
      }
    }
  }

  bool bCheckBus()
  {
    if (mI2C->STAR2 & I2C_STAR2_BUSY)
    {
      // In case starting a new transmission as long the bus is not stopped, will cause an error
      // Therefore, Busy wait, until HW STOP finished.
      // because, there is no Event/Irq that can be used for it.
      // Stop event generation.It can be set or cleared by user code, or cleared by hardware when a stop event is detected
      u16 lu8Timeout = 1000;
      while ((lu8Timeout) && (mI2C->CTLR1 & (1 << 9))) // stop
      {
        lu8Timeout--;
      }

      if (mI2C->STAR2 & I2C_STAR2_BUSY)
      {
        munDeviceStatus = tenComDeviceStatus::enDeviceError;
        munBusStatus = tenComBusStatus::enBusNotReady;
        mSm = cComNode::enStError;
        return False;
      }
      else
      {
        return True;
      }
    }
    return True;
  }

  bool bCheckPins()
  {
    mPinSCL->vSetMode(cGpCfg::tenMode::nInput);
    mPinSCL->vSetCnf(cGpCfg::tenCnf::nFloating);

    mPinSDA->vSetMode(cGpCfg::tenMode::nInput);
    mPinSDA->vSetCnf(cGpCfg::tenCnf::nFloating);

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

  void vClearSTOPandNACK()
  {
    //// Todo
  }


  uint8 isErrorActive()
  {
    uint32 lui32ErrorMask;

    /*
    Note: in Slave mode
    In master receiver mode, NACK is automatically generated after last byte preceding STOP or RESTART condition, whatever the NACK bit value.When an overrun occurs in slave receiver NOSTRETCH mode, a NACK is automatica
    */

    if (((mSm == cComNode::enStRx) && ( mbMaster)) ||
        ((mSm == cComNode::enStTx) && (!mbMaster)))
    {
      if (mI2C->STAR1 & I2C_STAR1_STOPF)
      {
        lui32ErrorMask =                (I2C_STAR1_BERR | I2C_STAR1_ARLO | I2C_STAR1_OVR | I2C_STAR1_PECERR | I2C_STAR1_TIMEOUT | I2C_STAR1_SMBALERT);
      }
      else
      {
        lui32ErrorMask = (I2C_STAR1_AF | I2C_STAR1_BERR | I2C_STAR1_ARLO | I2C_STAR1_OVR | I2C_STAR1_PECERR | I2C_STAR1_TIMEOUT | I2C_STAR1_SMBALERT);
      }
    }
    else
    {
      lui32ErrorMask =   (I2C_STAR1_AF | I2C_STAR1_BERR | I2C_STAR1_ARLO | I2C_STAR1_OVR | I2C_STAR1_PECERR | I2C_STAR1_TIMEOUT | I2C_STAR1_SMBALERT);
    }

    if ((mI2C->STAR1 & lui32ErrorMask)) return 1;
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
  bool                 isNoErrorHandling;

  cI2cMaster(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, uint32 lui32Slaves)
    : cI2c(lstI2c, lcScl, lcSda, True), mcSlaves(lui32Slaves)
  {
    mSm  = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;

    mpcActiveSlave = NULL;
    mpcActiveMsg   = NULL;
    isNoErrorHandling = False;

    mui8Adr1 = mui8Adr2 = 0;
    mu8Lock        = 0;

    mu16ReInitTicks = 100;
    mu16ComStuckTicksReload = 2000;
    mu16ComStuckTicks = mu16ComStuckTicksReload;

    vInitHw();
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
    volatile u16 lu16SR1;
    volatile u16 lu16SR2;

    __asm("nop");
    lu16SR1 = mI2C->STAR1;
    lu16SR2 = mI2C->STAR2;
    UNUSED(lu16SR2);

    u16 lu16Temp;
    
    lu16Temp  =  lu16SR1 & 0x0FFF;
    lu16Temp |= (lu16SR2 & 0x0F) << 12;
    
    lenEvent = cComNode::tenEvent::enEvDummy;
    if (lu16SR1 & I2C_STAR1_SB)
    {
      lenEvent = cComNode::tenEvent::enEvStart;
      // EVT5: SB=1, reading SR1 and then writing the address to the DR register will clear the event.
    }
    else if (lu16SR1 & I2C_STAR1_ADDR)
    {
      // EVT6; ADDR=1, reading SR1 then reading SR2 will clear the event
      lenEvent = cComNode::tenEvent::enEvAdress;
    }
    else if (lu16SR1 & I2C_STAR1_BTF) // Data byte transfer finished
    {
      lenEvent = cComNode::tenEvent::enEvI2cTc;
    }
    else if (lu16SR1 & I2C_STAR1_STOPF) // Data byte transfer finished
    {
      lenEvent = cComNode::tenEvent::enEvI2cTc;
      // cleared by software reading the SR1 register followed by a write in the CR1 register, or by
      // hardware when PE=0
      I2C_GenerateSTOP(mI2C, DISABLE);
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


  void vSupressErrorHandling()
  {
    isNoErrorHandling = True;
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
    u32 lui32ErrorMaskSR1;
    u32 lui32ErrorMaskSR2;

    lui32ErrorMaskSR1 = I2C_STAR1_AF | I2C_STAR1_BERR | I2C_STAR1_ARLO | I2C_STAR1_OVR | I2C_STAR1_PECERR | I2C_STAR1_TIMEOUT | I2C_STAR1_SMBALERT;
    lui32ErrorMaskSR2 = I2C_STAR2_BUSY;

    if ((mI2C->STAR1 & lui32ErrorMaskSR1) || (mI2C->STAR2 & lui32ErrorMaskSR2))
    {
      if (mI2C->STAR1 & I2C_STAR1_AF)
      {
        mSm = cComNode::tenState::enStIdle;
        if (mpcActiveMsg != NULL)
        {
          if (mpcActiveMsg->isBusy())
          {
            mpcActiveMsg->vError();

            if (mpcActiveSlave != NULL)
            {
              // Über das Interface kann isNoErrorHandling gesetzt werden, um die Fehlerbehandlung zu unterdrücken
              // Das wird z.B. benutzt wenn nach Nack nochmals wiederhohlt werden soll.
              mpcActiveSlave->vComError(cComNode::enErNACK, menComState);
            }
          }
        }
      }
      else
      {
        if (mI2C->STAR2 & I2C_STAR2_BUSY)
        {
          munBusStatus = tenComBusStatus::enBusNotReady;
          mSm = cComNode::enStError;
        }
      }

      if (isNoErrorHandling)
      {
        isNoErrorHandling = False;
      }
      else
      {
        vStopDMA();
        I2C_GenerateSTOP(mI2C, ENABLE);
        mI2C->STAR1 = mI2C->STAR2 = 0;
        mu8Lock        = 0;
        mpcActiveMsg   = NULL;
        mpcActiveSlave = NULL;
      }
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

    if (isErrorActive())
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
              mI2C->CTLR1 = 1; /*PE*/
              I2C_GenerateSTART(mI2C, ENABLE);
              // After the START bit is set, the I2C module automatically switches to master mode, the MSL bit is set, and a Start
              // event is generated.After the start event is generated, the SB bit is set. If the ITEVTEN bit is set, an interrupt is generated.
            default:
              break;
          }
          break;

        case cComNode::tenState::enStAdress:
          if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
              (lenEvent     == cComNode::tenEvent::enEvStart))
          {
            //TX and maybe RX, then TX first
            if (mpcActiveMsg->cTxData.muiLen > 0)
            {
              mSm             = cComNode::tenState::enStWaitAdress;
              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Transmitter);
              // When the number of data transfer bytes set in the DMA controller has been completed, the DMA controller
              // will send an EOT / EOT_1 signal indicating the end of the transmission to the I2C interface.
              // BTF will be set at the end of the transmission
            }
            //RX only
            else if (mpcActiveMsg->cRxData.muiLen > 0)
            {
              mSm             = cComNode::tenState::enStRx;
              mui32ComByteCnt = mpcActiveMsg->cRxData.muiLen;
              mpui8ComBuf     = mpcActiveMsg->cRxData.mpu8Data;

              mI2C->CTLR1 |= (1 << 10); /* activate ACK */
              mI2C->CTLR2 |= (1 << 12); /* activate LAST, DMA generates NACK at EOT */
              vStartDMARx(mpui8ComBuf, mui32ComByteCnt);
              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Receiver);
              // When the number of data transfer bytes set in the DMA controller has been completed, the DMA controller
              // will send an EOT / EOT_1 signal indicating the end of the transmission to the I2C interface.
              // BTF will be set at the end of the transmission
            }
            else
            {
              mSm = cComNode::tenState::enStIdle;
            }
          }
          break;

        case cComNode::tenState::enStWaitAdress:
          {
          if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
              (lenEvent     == cComNode::tenEvent::enEvAdress))
            {
              mSm = cComNode::tenState::enStTx;
              mui32ComByteCnt = mpcActiveMsg->cTxData.muiLen;
              mpui8ComBuf     = mpcActiveMsg->cTxData.mpu8Data;
              vStartDMATx(mpui8ComBuf, mui32ComByteCnt);
            }
            break;
          }

        case cComNode::tenState::enStTx:
          {
            menComState = cComNode::tenState::enStData;
            if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
                (lenEvent     == cComNode::tenEvent::enEvI2cTc))
            {
              // Wenn es nicht zu empfangen gibt dann fertig
              if (mpcActiveMsg->cRxData.muiLen == 0)
              {                 
                mSm    = cComNode::tenState::enStEnd;
                lbLoop = True;

                // A stop event is generated after the current byte transfer or the current
                I2C_GenerateSTOP(mI2C, ENABLE);
              }
              else
              {
                // Es gibt noch was zu empfangen => repeated start
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

              mI2C->CTLR1 |= (1 << 10); /* activate ACK */
              mI2C->CTLR2 |= (1 << 12); /* activate LAST, DMA generates NACK at EOT */
              vStartDMARx(mpui8ComBuf, mui32ComByteCnt);
              I2C_Send7bitAddress(mI2C, lui8SlaveAdr, I2C_Direction_Receiver);
            }
          }
          break;

        case cComNode::tenState::enStRx:
          {
            menComState = cComNode::tenState::enStData;
            if ((lenEventType == cComNode::tenEventType::enEvTyIrq) &&
                (lenEvent     == cComNode::tenEvent::enEvDmaRxTc))
            {
              mSm = cComNode::tenState::enStEnd;
              mI2C->CTLR1 &= ~(1 << 10); /* deactivate ACK */

              I2C_GenerateSTOP(mI2C, ENABLE);

              mSm    = cComNode::tenState::enStEnd;
              lbLoop = True;
            }
          }
          break;

        case cComNode::tenState::enStEnd:
          {
            mSm = cComNode::tenState::enStIdle;
            mI2C->CTLR2 &= ~(1 << 12); /* deactivate LAST */
            vStopDMA();
            mpcActiveMsg->vDone();

            // In case starting a new transmission as long the bus is not stopped, will cause an error
            // Therefore, Busy wait, until HW STOP finished.
            // because, there is no Event/Irq that can be used for it.
            // Stop event generation.It can be set or cleared by user code, or cleared by hardware when a stop event is detected
            u16 lu8Timeout = 1000;
            while ((lu8Timeout) && (mI2C->CTLR1 & (1 << 9))) // stop
            {
              lu8Timeout--;
            }

            mpcActiveSlave->vComDone();
          }
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


  void vStartMsg(cComDatMsg* lpcActiveMsg, cComNode* lpcActiveSlave = NULL) // __attribute__((optimize("-O0")))
  {
    if (mSm == cComNode::tenState::enStIdle)
    {
      bool lbStart;

      lbStart = true;

      if (mpcActiveMsg != NULL) // actual message
      {
        if (mpcActiveMsg->isBusy())
        {
          lbStart = false;
        }
      }

      if (lpcActiveMsg != NULL)  // given message
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
        if (lpcActiveSlave != NULL)
        {
          mpcActiveSlave = lpcActiveSlave;
          mpcActiveSlave->mStatus.IsStartRequested = 0;
        }
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
          this->vReInitHw(cComNode::tenConsts::enResetHwFull);
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
  : cI2c(lstI2c, lcScl, lcSda, False)
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
        this->vReInitHw(cComNode::tenConsts::enResetHwFull);
      }
    }
  }

  void vSetNode(cComNode *lpcComNode)
  {
    mpcComNode = lpcComNode;
    // HW erst hier, weil solange mpcActiveSlave noch auf null zeigt
    if (mpcComNode != null)
    {
      mui8Adr1 = mpcComNode->mAdr;
      vInitHw();
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
    // On error, check if is an slave specific (a) error or an
    // general bus error (b)
    //
    // In case of
    //   (a): Cancel the running transmission, block the bus, reinit the DMA, inform the slave, free the bus
    //   (b): Cancel the running transmission, block the bus, reinit the Bus, inform all slaves, free the bus

    u16 lu16ISR1;
    u16 lu16ISR2;

    lu16ISR1 = mI2C->STAR1;
    lu16ISR2 = mI2C->STAR2;
    mI2C->CTLR1 &= mI2C->CTLR1;

    menErrorActive = cComNode::enErUnknown;

    if      (lu16ISR1 & I2C_STAR1_AF)    menErrorActive = cComNode::enErNACK;
    else if (lu16ISR1 & I2C_STAR1_BERR)  menErrorActive = cComNode::enErBusError;
    else if (lu16ISR1 & I2C_STAR1_ARLO)  menErrorActive = cComNode::enErArbitrationLost;
    else if (lu16ISR1 & I2C_STAR1_OVR)   menErrorActive = cComNode::enErOverrun;
    else if (lu16ISR2 & I2C_STAR2_BUSY)  menErrorActive = cComNode::enErStartWhileBussy;
    else                                 menErrorActive = cComNode::enErUnknown;

    vReInitHw(cComNode::tenConsts::enResetHwFull);
    mSm = cComNode::tenState::enStIdle;
    mpcComNode->vComError(menErrorActive, mSm);
  }
 
  void vSetMsg(cComDatMsg* lpcMsg) { mpcMsg = lpcMsg; };

  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    volatile u16 lu16ISR1;
    volatile u16 lu16ISR2;

    UNUSED(lenEvent);
    UNUSED(lenEventType);


    // 2 vor 1 einlesen, damit Clock Streching bestehen bleibt.
    lu16ISR2 = mI2C->STAR2;
    lu16ISR1 = mI2C->STAR1;
    mI2C->CTLR1 &= mI2C->CTLR1; // Stop event flag. After the user reads the status register1, writing to the control register1 will clear this bit
    if (lu16ISR1 & I2C_STAR1_ADDR)
    {
      if (lu16ISR2 & I2C_STAR2_DUALF)
      {
        mpcComNode->mAdr = (mI2C->OADDR2) & 0xFE; // Das Dual bit ausmaskieren
      }
      else
      {
        mpcComNode->mAdr = (mI2C->OADDR1);
      }
    }

    bool isError = False;

    // Am Ende vom Slave Transmission schickt der Master ein NACK
    // Eigentlich sollte auch das Stop-Bit gesetzt sein, ist es aber nicht !?!
    if ((mSm == cComNode::enStTx) && (u16GetDmaCounter() == 0))
    {
      //if (mI2C->STAR1 & I2C_STAR1_STOPF)
      if (lu16ISR1 & (I2C_STAR1_BERR | I2C_STAR1_ARLO | I2C_STAR1_OVR | I2C_STAR1_PECERR | I2C_STAR1_TIMEOUT | I2C_STAR1_SMBALERT)) isError = True;
      if ((lu16ISR1 & I2C_STAR1_AF) && (!isError))
      {
        mI2C->STAR1 &= ~I2C_STAR1_AF; // This bit can be reset by user writing 0
        mSm = cComNode::tenState::enStIdle;
        mpcMsg->vDone();
        mpcComNode->vComDone();
        return;
      }
    }
    else
    {
      if (lu16ISR1 & (I2C_STAR1_AF | I2C_STAR1_BERR | I2C_STAR1_ARLO | I2C_STAR1_OVR | I2C_STAR1_PECERR | I2C_STAR1_TIMEOUT | I2C_STAR1_SMBALERT)) isError = True;
    }

    if (isError)
    {
      lu16ISR2 = mI2C->STAR2;
      vError();
      return;
    }

    
    switch (mSm)
    {
      case cComNode::tenState::enStIdle:
        if (lu16ISR1 & I2C_STAR1_ADDR)
        {
          if (lu16ISR2 & I2C_STAR2_TRA)
          {
            mSm = cComNode::enStTx; // Write transfer, slave enters transmitter mode.
            mpcMsg->vBusy();
            mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForTx);
            lu16ISR2 = mI2C->STAR2;
            vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);
            // clock stretching in slave mode when the ADDB or BTF flag bit is set until it is cleared by software.
            mpcComNode->vComStart(cComNode::tenEvent::enEvAfterTxStarted);
          }
          else
          {
            mSm = cComNode::enStRx; // Read transfer, slave enters receiver mode.
            mpcMsg->vBusy();
            mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForRx);
            vStartDMARx(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen);
            lu16ISR2 = mI2C->STAR2;
            // clock stretching in slave mode when the ADDB or BTF flag bit is set until it is cleared by software.
            // ADDR After the user reads the status register 1, the read operation of the status register2 will clear this bit
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
          if (lu16ISR1 & I2C_STAR1_ADDR)
          {
            if (lu16ISR2 & I2C_STAR2_TRA)
            {
              mSm = cComNode::enStTx; // 1: Read transfer, slave enters transmitter mode.

              mpcMsg->vBusy();
              mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForTx);
              lu16ISR2 = mI2C->STAR2;
              vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);              
              // clock stretching in slave mode when the ADDB or BTF flag bit is set until it is cleared by software.
              mpcComNode->vComStart(cComNode::tenEvent::enEvAfterTxStarted);
            }
          }
          else
          if (lu16ISR1 & I2C_STAR1_STOPF)
          {
            mSm = cComNode::tenState::enStIdle;
            mI2C->STAR1 &= ~I2C_STAR1_AF; // This bit can be reset by user writing 0
            mpcMsg->vDone();
            mpcComNode->vComDone();
          }
        }
        break;
      default:
        break;
    }
    lu16ISR2 = mI2C->STAR2;
  }
};


#endif // __cI2C_H__
