#ifndef __cI2C_H__
#define __cI2C_H__

#include "Typedef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_i2c.h"
#include "stm32u5xx_ll_i2c.h"
#include "stm32u5xx_ll_dma.h"

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

  cGpPin *mPinSCL;
  cGpPin *mPinSDA;

  u8    mu8DmaCh;
  DMA_Channel_TypeDef* mstDmaCh;
  u32   mu32DmaRxTr1;
  u32   mu32DmaRxTr2;
  u32   mu32DmaRxPeriph;
  u32   mu32DmaTxTr1;
  u32   mu32DmaTxTr2;
  u32   mu32DmaTxPeriph;
  
  u32   mu32Baud;

  I2C_TypeDef   *mI2C;

  uint8 mui8Adr1;
  uint8 mui8Adr2;

  bool mbMaster;
  cComNode::tenConsts menOption;

  cI2c(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, u8 lu8DmaCh, bool lbMaster)
  {
    mI2C = lstI2c;
	  mSm  = cComNode::tenState::enStIdle;

    mPinSCL = lcScl;
    mPinSDA = lcSda;

    mu8DmaCh = lu8DmaCh;
    mstDmaCh = ((DMA_Channel_TypeDef*)(GPDMA1_BASE + LL_DMA_CH_OFFSET_TAB[mu8DmaCh]));
    munDeviceStatus = tenComDeviceStatus::enDeviceNotInit;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;

    mbMaster        = lbMaster;
    menOption       = cComNode::tenConsts::enNop;
  }

  // vAdrEnable und vAdrDisable sollte nicht während einer
  // laufenden Adressierung ausgeführt werden
  void vAdrEnable()  { LL_I2C_EnableOwnAddress1(mI2C); };
  void vAdrDisable() { LL_I2C_DisableOwnAddress1(mI2C); };
  bool isAdrEnabled() { return LL_I2C_IsEnabledOwnAddress1(mI2C); };

  void vActivateNack() { LL_I2C_AcknowledgeNextData(mI2C, LL_I2C_NACK); };

  void  vSetAdr2(u8 lui8Adr2)
  {
    //GPIOB->BSRR = (1 << 15);

    LL_I2C_DisableOwnAddress2(mI2C);
    if (lui8Adr2)
    {
      mui8Adr2 = lui8Adr2;
      LL_I2C_SetOwnAddress2(mI2C, mui8Adr2, 0);
      LL_I2C_EnableOwnAddress2(mI2C);
    }

    //GPIOB->BSRR = (1 << (16 + 15));
  }

  inline void vStopDMA()
  {
    mstDmaCh->CCR &= ~DMA_CCR_EN;
  }

  // Überpruft ob sich der DMA counter geändert hat. Um rauszufinden, ob irgendwas empfangen wurde
  u16 u16GetDmaCounter()
  {
    return (mstDmaCh->CBR1 & 0xFFFF);
  }

  void vStartDMATx(uint8* pBuffer, uint32 BufferSize)
  {
    mstDmaCh->CFCR = DMA_CFCR_TCF;
    mstDmaCh->CCR &= ~DMA_CCR_EN;
    mstDmaCh->CBR1 = BufferSize;
    mstDmaCh->CSAR = (uint32)pBuffer;
    mstDmaCh->CDAR = mu32DmaTxPeriph;
    mstDmaCh->CTR1 = mu32DmaTxTr1;
    mstDmaCh->CTR2 = mu32DmaTxTr2;
    mstDmaCh->CCR |= DMA_CCR_EN;
  }

  void vStartDMARx(uint8* pBuffer, uint32 BufferSize)
  {
    mstDmaCh->CFCR = DMA_CFCR_TCF;
    mstDmaCh->CCR &= ~DMA_CCR_EN;
    mstDmaCh->CBR1 = BufferSize;
    mstDmaCh->CSAR = mu32DmaRxPeriph;
    mstDmaCh->CDAR = (uint32)pBuffer;
    mstDmaCh->CTR1 = mu32DmaRxTr1;
    mstDmaCh->CTR2 = mu32DmaRxTr2;
    mstDmaCh->CCR |= DMA_CCR_EN;
  }

  void  vClearClockLine()
  {
    mPinSCL->vSetMode(GPIO_MODE_OUTPUT_OD);

    //for (u8 lu8t = 0; lu8t < 9; lu8t++)
    {
      cClockInfo::Delay_us(1);
      mPinSCL->vSet0();
      cClockInfo::Delay_us(1);
      mPinSCL->vSet1();
    }

    // Es is immer AF4 egal ob I2C1, I2C2, I2C3, oder I2C4
    mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C1);
  }

  u32 u32BaudToRegister()
  {
    // Prepared 
    //  - I2C Clock = 16Mhz HSI
    //  - Digital Filter DNF = 2 
    //  - Analog Filter on.
    //  - Used CubeI2CTool
    //     - Rise Time = 0
    //     - Fall Time = 0
    //        - Baud =  20.000 => 0x10108EFB
    //        - Baud =  40.000 => 0x10103D84
    //        - Baud =  50.000 => 0x00303BF9 
    //        - Baud = 100.000 => 0x2010131D
    //        - Baud = 200.000 => 0x00303BF9 
    //        - Baud = 300.000 => 0x00100525
    //        - Baud = 400.000 => 0x00100418 
    // 
    //
    if (mu32Baud <=  20000) return 0x10108EFB;
    if (mu32Baud <=  40000) return 0x10103D84;
    if (mu32Baud <=  50000) return 0x00303BF9;
    if (mu32Baud <= 100000)  return 0x2010131D;
    if (mu32Baud <= 200000)  return 0x00303BF9;
    if (mu32Baud <= 300000)  return 0x00100525;
    /*if (mu32Baud <= 400000)*/  return 0x00100418;
  }

  void vInitDma()
  {
    /* Enable DMA clock */
    __HAL_RCC_GPDMA1_CLK_ENABLE();

    /* DMA node configuration declaration */
    LL_DMA_InitTypeDef           lstDmaInit;

    LL_DMA_StructInit(&lstDmaInit);

    lstDmaInit.BlkHWRequest  = LL_DMA_HWREQUEST_SINGLEBURST;
    lstDmaInit.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;

    lstDmaInit.SrcBurstLength  = 1;
    lstDmaInit.DestBurstLength = 1;

    lstDmaInit.SrcDataWidth  = LL_DMA_SRC_DATAWIDTH_BYTE;
    lstDmaInit.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;


    lstDmaInit.Priority = LL_DMA_LOW_PRIORITY_LOW_WEIGHT;
    lstDmaInit.BlkDataLength = 0;

    lstDmaInit.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;


    // -Port 0 should be allocated for transfers toand from
    //  peripherals, because there is a direct hardware datapath
    //  between this portand the APB peripherals, outside the  AHB matrix
    // -Port 1 should be allocated for transfers toand from
    //  memory, which are performed through the AHB interconnect.

    if (mI2C == I2C1) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C1_RX;
    if (mI2C == I2C2) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C2_RX;
    if (mI2C == I2C3) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C3_RX;
    if (mI2C == I2C4) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C4_RX;
    lstDmaInit.SrcAddress  = LL_I2C_DMA_GetRegAddr(mI2C, LL_I2C_DMA_REG_DATA_RECEIVE);
    lstDmaInit.DestAddress = 0;
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_FIXED;
    lstDmaInit.DestIncMode = LL_DMA_DEST_INCREMENT;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT0; // Source is periph
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT1; // Dest is memory
    LL_DMA_Init(GPDMA1, mu8DmaCh, &lstDmaInit);

    mu32DmaRxTr1    = mstDmaCh->CTR1;
    mu32DmaRxTr2    = mstDmaCh->CTR2;
    mu32DmaRxPeriph = mstDmaCh->CSAR;

    if (mI2C == I2C1) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C1_TX;
    if (mI2C == I2C2) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C2_TX;
    if (mI2C == I2C3) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C3_TX;
    if (mI2C == I2C4) lstDmaInit.Request = LL_GPDMA1_REQUEST_I2C4_TX;
    lstDmaInit.SrcAddress  = 0;
    lstDmaInit.DestAddress = LL_I2C_DMA_GetRegAddr(mI2C, LL_I2C_DMA_REG_DATA_TRANSMIT);
    lstDmaInit.SrcIncMode  = LL_DMA_SRC_INCREMENT;
    lstDmaInit.DestIncMode = LL_DMA_DEST_FIXED;
    lstDmaInit.Direction   = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    lstDmaInit.SrcAllocatedPort  = LL_DMA_DEST_ALLOCATED_PORT1; // Source is memory
    lstDmaInit.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT0; // Dest is periph
    LL_DMA_Init(GPDMA1, mu8DmaCh, &lstDmaInit);

    mu32DmaTxTr1    = mstDmaCh->CTR1;
    mu32DmaTxTr2    = mstDmaCh->CTR2;
    mu32DmaTxPeriph = mstDmaCh->CDAR;
  }

  void  vInitHw() // __attribute__((optimize("-O0")))
  {
    I2C_HandleTypeDef lhI2c = {};

    if (mI2C == I2C1) __HAL_RCC_I2C1_CLK_DISABLE();
    if (mI2C == I2C2) __HAL_RCC_I2C2_CLK_DISABLE();
    if (mI2C == I2C3) __HAL_RCC_I2C3_CLK_DISABLE();
    if (mI2C == I2C4) __HAL_RCC_I2C4_CLK_DISABLE();

    vStopDMA();

    //SDA and SCL must be pulled high
    if (mbMaster)
    {
      if (!bCheckPins()) vClearClockLine();
      if (!bCheckPins()) return;
    }

    // Es is immer AF4 egal ob I2C1, I2C2, I2C3, oder I2C4
    mPinSCL->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C1);
    mPinSDA->vSetAF(GPIO_MODE_AF_OD, GPIO_AF4_I2C1);


    // I2C Periph clock enable
    if (mI2C == I2C1) { __HAL_RCC_I2C1_CLK_ENABLE(); __HAL_RCC_I2C1_CONFIG(RCC_I2C1CLKSOURCE_HSI); } // 16Mhz
    if (mI2C == I2C2) { __HAL_RCC_I2C2_CLK_ENABLE(); __HAL_RCC_I2C2_CONFIG(RCC_I2C2CLKSOURCE_HSI); } // 16Mhz
    if (mI2C == I2C3) { __HAL_RCC_I2C3_CLK_ENABLE(); __HAL_RCC_I2C3_CONFIG(RCC_I2C3CLKSOURCE_HSI); } // 16Mhz
    if (mI2C == I2C4) { __HAL_RCC_I2C4_CLK_ENABLE(); __HAL_RCC_I2C4_CONFIG(RCC_I2C4CLKSOURCE_HSI); } // 16Mhz	

    // I2C Peripheral Disable
    LL_I2C_Disable(mI2C);


    // I2C configuration
    // I2C configuration
    lhI2c.Instance = mI2C;
    lhI2c.Init.Timing          = u32BaudToRegister();
    lhI2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLED;
    lhI2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
    lhI2c.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    lhI2c.Init.OwnAddress1     = mui8Adr1;
    lhI2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    lhI2c.Init.OwnAddress2     = mui8Adr2;
    lhI2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    lhI2c.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;


    // Peripheral clock enable
    HAL_I2C_Init(&lhI2c);
    HAL_I2CEx_AnalogFilter_Config(&lhI2c, I2C_ANALOGFILTER_ENABLED);
    HAL_I2CEx_DigitalFilter_Config(&lhI2c, 2); // These bits are used to configure the digital noise filter on SDA and SCL input. The digital
                                               // filter, filters spikes with a length of up to DNF[3:0] * t I2CCLK

    if (!mbMaster)
    {
      vSetAdr2(mui8Adr2);
    }

    mI2C->ICR = 0xFFFF;

    // I2C Peripheral Enable
    LL_I2C_Enable(mI2C);

    LL_I2C_EnableDMAReq_RX(mI2C);
    LL_I2C_EnableDMAReq_TX(mI2C);

    if (isErrorActive())
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

    IRQn_Type lnNVIC_IRQChannel;

    if (mI2C == I2C1) lnNVIC_IRQChannel = I2C1_EV_IRQn;
    if (mI2C == I2C2) lnNVIC_IRQChannel = I2C2_EV_IRQn;
    if (mI2C == I2C3) lnNVIC_IRQChannel = I2C3_EV_IRQn;
	  if (mI2C == I2C4) lnNVIC_IRQChannel = I2C4_EV_IRQn;
    
    // Set the TIMx priority
    // Slave soll etwas höhere Prio haben als wie Master
    if (mbMaster) HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 8);
             else HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 7);

    // Enable the TIMx global Interrupt
    HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);


    if (mI2C == I2C1) lnNVIC_IRQChannel = I2C1_ER_IRQn;
    if (mI2C == I2C2) lnNVIC_IRQChannel = I2C2_ER_IRQn;
    if (mI2C == I2C3) lnNVIC_IRQChannel = I2C3_ER_IRQn;
    if (mI2C == I2C4) lnNVIC_IRQChannel = I2C4_ER_IRQn;
    // Set the TIMx priority
    // Slave soll etwas höhere Prio haben als wie Master
    if (mbMaster) HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 8);
             else HAL_NVIC_SetPriority(lnNVIC_IRQChannel, 8, 7);

    // Enable the TIMx global Interrupt
    HAL_NVIC_EnableIRQ(lnNVIC_IRQChannel);


    if (mbMaster)
    {
      mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE;
    }
    else
    {
      mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_ADDRIE;
    }

    vInitDma();

    munDeviceStatus = tenComDeviceStatus::enDeviceReady;
    munBusStatus    = tenComBusStatus::enBusNoError;
    munComStatus    = tenComStatus::enStatusIdle;
    mSm             = cComNode::tenState::enStIdle;
  }

  void  vReInitHw(cComNode::tenConsts lenOption)
  {
    menOption = lenOption;
    if (lenOption != cComNode::tenConsts::enNop)
    {
      // Interrupt deaktivieren, sonst kommt die 10ms LP nicht dran.
      mI2C->CR1 &= ((~I2C_CR1_ERRIE) & (~I2C_CR1_TCIE) & (~I2C_CR1_STOPIE) & (~I2C_CR1_NACKIE) & (~I2C_CR1_ADDRIE));
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
        // I2C Peripheral Disnable
        LL_I2C_Disable(mI2C);
        vStopDMA();

        if (mbMaster)
        {
          //SDA and SCL must be pulled high
          if (!bCheckPins()) return;

          mPinSDA->vSetMode(GPIO_MODE_AF_OD);
          mPinSCL->vSetMode(GPIO_MODE_AF_OD);
        }

        mI2C->ICR = 0xFFFF;

        if (mbMaster)
        {
          mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | I2C_CR1_NACKIE;
        }
        else
        {
          mI2C->CR1 |= I2C_CR1_ERRIE | I2C_CR1_STOPIE | I2C_CR1_ADDRIE;
        }
        // I2C Peripheral Enable
        LL_I2C_Enable(mI2C);

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

      if (mI2C->ISR & I2C_ISR_BUSY)
      {
        munDeviceStatus = tenComDeviceStatus::enDeviceInitError;
        munBusStatus    = tenComBusStatus::enBusNotReady;
        return;
      }
    }
  }

  void vWaitUntilStop()
  {
    u16 lu8Timeout = 100;
    while ((lu8Timeout) && (!(mI2C->ISR & I2C_ISR_STOPF))) // stop
    {
      lu8Timeout--;
    }
  }

  void vWaitWhileBusy()
  {
    u16 lu8Timeout = 100;
    while ((lu8Timeout) && (mI2C->ISR & I2C_ISR_BUSY))
    {
      lu8Timeout--;
    }
  }

  bool bCheckBus()
    {
    if (mI2C->ISR & I2C_ISR_BUSY)
    {
      // In case starting a new transmission as long the bus is not stopped, will cause an error
      // Therefore, Busy wait, until HW STOP finished.
      // because, there is no Event/Irq that can be used for it.
      // Stop event generation.It can be set or cleared by user code, or cleared by hardware when a stop event is detected
      u16 lu8Timeout = 1000;
      while ((lu8Timeout) && (mI2C->ISR & I2C_ISR_STOPF)) // stop
      {
        lu8Timeout--;
      }

      if (mI2C->ISR & I2C_ISR_BUSY)
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
    mPinSCL->vSetMode(GPIO_MODE_INPUT);
    mPinSDA->vSetMode(GPIO_MODE_INPUT);

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
    mI2C->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; //Clear Stop and Nack detection flag
  }

  uint8 isErrorActive()
  {
    uint32 lui32ErrorMask;

    /*
      Note: in Slave mode
      NACK:
      This flag is set by hardware when a NACK is received after a byte transmission. It is cleared by software by setting the NACKCF bit.
      In master receiver mode, NACK is automatically generated after last byte preceding STOP or RESTART condition, whatever the NACK bit value.
      In slave transmitter mode, if a NACK happens, check the number of transmitted byes, to check if the transmission was completed

      OVR:
      This flag is set by hardware in slave mode with NOSTRETCH=1, when an overrun/underrun error occurs. It is cleared by software by setting the OVRCF bit.
    */

    if ((mSm == cComNode::enStTx) && (!mbMaster))
    {
      // Slave als Transmitter: NACK vom Master im letzten Byte zeigt Ende der Übertragung an.
      if (u16GetDmaCounter() == 0)
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
  uint16             mu16ReInitTicksReload;
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

  cI2cMaster(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, u8 lu8DmaCh, uint32 lui32Slaves, u32 lu32Baud = 400000)
    : cI2c(lstI2c, lcScl, lcSda, lu8DmaCh, True), mcSlaves(lui32Slaves)
  {
    mSm  = cComNode::tenState::enStIdle;
    menComState = cComNode::tenState::enStIdle;

    mu32Baud = lu32Baud;

    mpcActiveSlave = NULL;
    mpcActiveMsg   = NULL;

    mui8Adr1 = mui8Adr2 = 0;
    mu8Lock        = 0;
    isNoErrorHandling = False;

    mu16ReInitTicksReload = 100;
    mu16ReInitTicks = mu16ReInitTicksReload;
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

  inline void SetLock(u8 lu8Val = 1)   {mu8Lock = lu8Val;}
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

  void vSupressErrorHandling()
  {
    isNoErrorHandling = True;
  }

  void vTick1ms()
  {
    if (mSm == cComNode::enStError)
    {
      if (mu16ReInitTicks)
      {
        mu16ReInitTicks--;

        if (!mu16ReInitTicks)
        {
          mpcActiveMsg = NULL;
          mu8Lock = 0;
          this->vReInitHw(cComNode::tenConsts::enResetHwFull);
          this->vReInitAllSlave();
          mu16ReInitTicks = mu16ReInitTicksReload;
        }
      }
    }
    else
    {
      mu16ReInitTicks = mu16ReInitTicksReload;
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
        munDeviceStatus = tenComDeviceStatus::enDeviceReady;
        munBusStatus = tenComBusStatus::enBusNoError;
        munComStatus = tenComStatus::enStatusStuck;
        mu16ComStuckTicks = mu16ComStuckTicksReload;
        mu16ReInitTicks = mu16ReInitTicksReload;
      }
    }
  }

  void vResetCom()
  {
    vStopDMA();
    mI2C->ICR      = 0xFFFF;
    mu8Lock        = 0;
    mpcActiveMsg   = NULL;
    mpcActiveSlave = NULL;

    mSm = cComNode::tenState::enStIdle;
  }

  /*
    I2Cx_ISR->NACKF: Not Acknowledge received flag: It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0
    I2Cx_ISR->BERR:  Bus error:                     It is cleared by software by setting BERRCF bit.:     This bit is cleared by hardware when PE=0.
    I2Cx_ISR->OVR:   Overrun/Underrun (slave mode): It is cleared by software by setting the OVRCF bit.:  This bit is cleared by hardware when PE=0.
    I2Cx_ISR->ARLO:  Arbitration lost:              It is cleared by software by setting the ARLOCF bit.: This bit is cleared by hardware when PE=0.
  */
  void vError()
  {
    u32 lui32ErrorMask;
    cComNode::tenError lenError = cComNode::enErUnknown;

    lui32ErrorMask = I2C_ISR_BUSY | I2C_ISR_NACKF | I2C_ISR_BERR | I2C_ISR_ARLO | I2C_ISR_OVR | I2C_ISR_PECERR | I2C_ISR_TIMEOUT | I2C_ISR_ALERT;

    if (mI2C->ISR & lui32ErrorMask)
    {
      if      (mI2C->ISR & I2C_ISR_NACKF) lenError = cComNode::enErNACK;
      else if (mI2C->ISR & I2C_ISR_BERR)  lenError = cComNode::enErBusError;
      else if (mI2C->ISR & I2C_ISR_ARLO)  lenError = cComNode::enErArbitrationLost;
      else if (mI2C->ISR & I2C_ISR_OVR)   lenError = cComNode::enErOverrun;
      else if (mI2C->ISR & I2C_ISR_BUSY)  lenError = cComNode::enErStartWhileBussy;

      // Über das Interface kann isNoErrorHandling gesetzt werden, um die Fehlerbehandlung zu unterdrücken
      // Das wird z.B. benutzt wenn nach Nack nochmals wiederhohlt werden soll.
      if (mI2C->ISR & I2C_ISR_NACKF)
      {
        // If a NACK is received : the TXIS flag is not set, and a STOP condition is automatically
        //   sent after the NACK reception.the NACKF flag is set in the I2C_ISR register, and an
        //   interrupt is generated if the NACKIE bit is set.
        //
        // Bei NACK wird von der HW die Stop Sequenz gestarted
        // Warten bis diese fertig ist ist.
        vWaitUntilStop();
        vClearSTOPandNACK();

        if (mpcActiveMsg != NULL)
        {
          if (mpcActiveMsg->isBusy())
          {
            mpcActiveMsg->vError();
          }
        }
      }
    }

    mSm = cComNode::tenState::enStIdle;

    if (mpcActiveSlave != NULL)
    {
      mpcActiveSlave->vComError(lenError, menComState);
    }

    if (isNoErrorHandling)
    {
      isNoErrorHandling = False;
    }
    else
    {
      vResetCom();
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


    // Test
    //if ((mI2C->ISR & I2C_ISR_NACKF))
    //{
    //  GPIOA->BRR = (1 << 5);
    //  __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
    //  __asm("nop"); __asm("nop"); __asm("nop"); __asm("nop");
    //  GPIOA->BSRR = (1 << 5);
    //}


    if (isErrorActive())
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
                LL_I2C_HandleTransfer(mI2C, lui8SlaveAdr, LL_I2C_ADDRSLAVE_7BIT, 255, LL_I2C_MODE_RELOAD, LL_I2C_GENERATE_START_WRITE);
              }
              else
              {
                if (mpcActiveMsg->cRxData.muiLen > 0)
                {
                  LL_I2C_HandleTransfer(mI2C, lui8SlaveAdr, LL_I2C_ADDRSLAVE_7BIT, mui32ComByteCnt, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
                }
                else
                {
                  LL_I2C_HandleTransfer(mI2C, lui8SlaveAdr, LL_I2C_ADDRSLAVE_7BIT, mui32ComByteCnt, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);
                }
              }
            }
            //RX only
            else if (mpcActiveMsg->cRxData.muiLen > 0)
            {
              mSm = cComNode::tenState::enStRx;
              vStartDMARx(mpcActiveMsg->cRxData.mpu8Data, mpcActiveMsg->cRxData.muiLen);
              LL_I2C_HandleTransfer(mI2C, lui8SlaveAdr, LL_I2C_ADDRSLAVE_7BIT, mpcActiveMsg->cRxData.muiLen, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
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
                  LL_I2C_HandleTransfer(mI2C, lui8SlaveAdr, LL_I2C_ADDRSLAVE_7BIT, 255, LL_I2C_MODE_RELOAD, LL_I2C_GENERATE_NOSTARTSTOP);
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
                  LL_I2C_HandleTransfer(mI2C, lui8SlaveAdr, LL_I2C_ADDRSLAVE_7BIT, mpcActiveMsg->cRxData.muiLen, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);
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
              vError();
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
        vError();
        break;
    }

    if (mSm_Old != mSm)
    {
      mu16ComStuckTicks = mu16ComStuckTicksReload;
    }
  }

  void vStartMsg(cComDatMsg *lpcActiveMsg, cComNode* lpcActiveSlave = NULL) // __attribute__((optimize("-O0")))
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

      if (lbStart)
      {
        lbStart = bCheckBus();
      }


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

      if (lbStart)
      {
        lbStart = bCheckBus();
      }

      if (lbStart)
      {
        mpcActiveSlave = (cComNode*)mcSlaves.cGetNext();
        mpcActiveSlave->mStatus.IsStartRequested = 0;
        mpcActiveSlave->vComStart(cComNode::tenEvent::enEvPrepareToSendData);
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
  u16 mu16ComStuckTicksReload;
  u16 mu16ComStuckTicks;

  cComDatMsg          *mpcMsg;

  cComNode            *mpcComNode;
  cComNode::tenError   menErrorActive;

  bool                 isNoErrorHandling;

  cI2cSlave(I2C_TypeDef *lstI2c, cGpPin *lcScl, cGpPin *lcSda, u8 lu8DmaCh, uint8 lui8Adr1, uint8 lui8Adr2, u32 lu32Baud = 400000)
  : cI2c(lstI2c, lcScl, lcSda, lu8DmaCh, False)
  {
    mI2C       = lstI2c;
    mpcComNode = NULL;

    mu32Baud = lu32Baud;

    mui8Adr1 = lui8Adr1;
    mui8Adr2 = lui8Adr2;

    mPinSCL = lcScl;
    mPinSDA = lcSda;

    isNoErrorHandling = False;
    mu16ComStuckTicksReload = 2000;
    mu16ComStuckTicks = mu16ComStuckTicksReload;
  }


  ~cI2cSlave()
  {
  }

  void vTick1ms()
  {
    if (mSm != cComNode::tenState::enStIdle)
    {
      if (mu16ComStuckTicks)
      {
        mu16ComStuckTicks--;
      }
      else
      {
        mSm = cComNode::enStError;
        munDeviceStatus = tenComDeviceStatus::enDeviceReady;
        munBusStatus = tenComBusStatus::enBusNoError;
        munComStatus = tenComStatus::enStatusStuck;
        mu16ComStuckTicks = mu16ComStuckTicksReload;

        vReInitHw(cComNode::tenConsts::enResetHwFull);
        mpcComNode->vComError(cComNode::tenError::enErStuck, mSm);
        mSm = cComNode::tenState::enStIdle;
      }
    }
  }

  void vSetNode(cComNode *lpcComNode)
  {
    mpcComNode = lpcComNode;
    /* HW erst hier, weil solange mpcActiveSlave noch auf null zeigt */
    if (mpcComNode != null)
    {
      mui8Adr1 = mpcComNode->mAdr;
      vInitHw();
    }
    //RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
    //GPIOB->MODER &= ~(2 << (2 * 15));
  }

  void I2C_EV_IRQHandler()
  {
    vSm(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvIrq);
  }

  void I2C_ER_IRQHandler()
  {
    vSm(cComNode::tenEventType::enEvTyIrq, cComNode::tenEvent::enEvError);
  }

  void vResetCom()
  {
    vReInitHw(cComNode::tenConsts::enResetHwFull);
    vTick10ms();
    mSm = cComNode::tenState::enStIdle;
  }

  void vSupressErrorHandling()
  {
    isNoErrorHandling = True;
  }


  void vError()
  {
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
    }

    mpcComNode->vComError(menErrorActive, mSm);

    if (isNoErrorHandling)
    {
      isNoErrorHandling = False;
    }
    else
    {
      vResetCom();
    }
  }

  void vSetMsg(cComDatMsg* lpcMsg) { mpcMsg = lpcMsg; };


  void vSm(cComNode::tenEventType lenEventType, cComNode::tenEvent lenEvent) // __attribute__((optimize("-O0")))
  {
    uint32 lui32ISR;
    cComNode::tenState mSm_Old;

    UNUSED(lenEvent);
    UNUSED(lenEventType);

    mSm_Old = mSm;

    if (isErrorActive())
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
            mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForTx);
            vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);
            mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
            mpcComNode->vComStart(cComNode::tenEvent::enEvAfterTxStarted);
            mpcMsg->vBusy();
          }
          else
          {
            mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForRx);
            mSm = cComNode::enStRx; // Read transfer, slave enters receiver mode.
            vStartDMARx(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen);
            mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
            mpcComNode->vComStart(cComNode::tenEvent::enEvAfterRxStarted);
            mpcMsg->vBusy();
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
          if (lui32ISR & I2C_ISR_STOPF)
          {
            mSm = cComNode::tenState::enStIdle;
            mI2C->ICR = I2C_ICR_STOPCF | I2C_ICR_NACKCF; //Clear Stop and Nack detection flag

            mpcMsg->vDone();
            mpcComNode->vComDone();
          }
          
          // Falls mpcComNode->vComDone länger dauert kann in der Zwischenzeit
          // eine neue Anfrage from Master kommen
          // Oder Repeated Start
          if (lui32ISR & I2C_ISR_ADDR)
          {
            mpcComNode->mAdr = (lui32ISR >> 16) & 0xFE;
            if (lui32ISR & I2C_ISR_DIR)
            {
              mSm = cComNode::enStTx; // Write transfer, slave enters transmitter mode.
              mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForTx);
              vStartDMATx(mpcMsg->cTxData.mpu8Data, mpcMsg->cTxData.muiLen);
              mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
              mpcComNode->vComStart(cComNode::tenEvent::enEvAfterTxStarted);
              mpcMsg->vBusy();
            }
            else
            {
              mpcComNode->vComStart(cComNode::tenEvent::enEvPrepareForRx);
              mSm = cComNode::enStRx; // Read transfer, slave enters receiver mode.
              vStartDMARx(mpcMsg->cRxData.mpu8Data, mpcMsg->cRxData.muiLen);
              mI2C->ICR = I2C_ICR_ADDRCF; //Clear Adress bit, the SCL clock is stretched while the ADDR flag is set
              mpcComNode->vComStart(cComNode::tenEvent::enEvAfterRxStarted);
              mpcMsg->vBusy();
            }
          }
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
};


#endif // __cI2C_H__
