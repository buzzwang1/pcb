#ifndef __C_USART_H__
#define __C_USART_H__

#include "TypeDef.h"
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_misc.h"

#include "cRingBufT.h"
#include "tGPPin.h"
#include "ClockInfo.h"


class cUart
{
  public:
  cRingBufDynT<u8, u16> mcUartDataIn;
  cRingBufDynT<u8, u16> mcUartDataOut;

  cUart(const u16 mui16TxBufSize,
        const u16 mui16RxBufSize)
     : mcUartDataIn(mui16RxBufSize),
       mcUartDataOut(mui16TxBufSize)
  {
  }

  virtual void vHwInit(const uint32 lu32Baudrate,
                       const u16 mui16PinAF) = 0;


  virtual u32  PortBase(void) = 0;
  virtual void vIRQHandler(void) = 0;
  virtual void vSend(char8 lchData) = 0;
  virtual void vSend(const u8* lpu8Data, u16 lu16Count) = 0;
  virtual void vSend(void) = 0;

  inline void vAdd(char8 lchData)
  {
    mcUartDataOut.put(lchData);
  }

  inline void vAdd(const char8* lpchStr)
  {
    while (*lpchStr != 0)
    {
      vAdd(*lpchStr);
      lpchStr++;
    }
  }

  inline void vAdd(const u8* lpu8Data, u16 lu16Count)
  {
    while (lu16Count)
    {
      vAdd(*lpu8Data++);
      lu16Count--;
    }
  }

  inline void vSend(const char8* lpchStr)
  {
    vAdd(lpchStr);
    vAdd(13);
    vSend(10);
  }

  void vPrintMem(uint8 lui8Pre, uint8* lpAdr, uint8 lui8len)
  {
    uint8 lui8t;
    uint8 lui8Val;

    vAdd(lui8Pre);
    vAdd(' ');

    for (lui8t = 0; (lui8t < lui8len); lui8t++)
    {
      lui8Val = ((*lpAdr) >> 4);
      if (lui8Val < 10) vAdd('0' + lui8Val);
      else vAdd('A' + lui8Val - 10);

      lui8Val = ((*lpAdr) & 0xF);
      if (lui8Val < 10) vAdd('0' + lui8Val);
      else vAdd('A' + lui8Val - 10);

      vSend(' ');
      lpAdr++;
    }
  }
};


template <const uintptr_t mu32Usart_BaseAdr,
          const uintptr_t mu32TxPort_BaseAdr, const u16 mui16TxPinNr,
          const uintptr_t mu32RxPort_BaseAdr, const u16 mui16RxPinNr>
class tcUart: public cUart
{
public:

  tcGpPin<mu32TxPort_BaseAdr, mui16TxPinNr> mPinTx;
  tcGpPin<mu32RxPort_BaseAdr, mui16RxPinNr> mPinRx;

  using cUart::vAdd;
  using cUart::vSend;
  using cUart::vPrintMem;

  tcUart(const u16 mui16TxBufSize,
         const u16 mui16RxBufSize)
    : cUart(mui16TxBufSize, mui16RxBufSize),
      mPinTx(GPIO_Mode_AF, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_50MHz, 1),
      mPinRx(GPIO_Mode_AF, GPIO_OType_OD, GPIO_PuPd_UP, GPIO_Speed_50MHz, 1)
  {
  }

  tcUart(const uint32 lu32Baudrate,
         const u16 mui16PinAF,
         const u16 mui16TxBufSize,
         const u16 mui16RxBufSize)
    : tcUart(mui16TxBufSize, mui16RxBufSize)
  {
    vHwInit(lu32Baudrate, mui16PinAF);
  }

  void vHwInit(const uint32 lu32Baudrate,
               const u16 mui16PinAF) override
  {
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable USART clock */
    switch (mu32Usart_BaseAdr)
    {
      case USART1_BASE: RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);break;
      case USART2_BASE: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);break;
      case USART3_BASE: RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);break;
    }

    USART_InitStructure.USART_BaudRate   = lu32Baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;
    USART_InitStructure.USART_Parity     = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;

    /* Connect PXx to USARTx_Tx */
    mPinTx.vSetAF(mui16PinAF);
    /* Connect PXx to USARTx_Rx */
    mPinRx.vSetAF(mui16PinAF);

    /* USART configuration */
    USART_Init((USART_TypeDef*)Port(), &USART_InitStructure);

    /* Enable USART */
    USART_Cmd((USART_TypeDef*)Port(), ENABLE);

    //-----------------------------------------
    switch (mu32Usart_BaseAdr)
    {
      case USART1_BASE: NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;break;
      case USART2_BASE: NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;break;
      case USART3_BASE: NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;break;
    }

    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    /* NVIC Configuration */
    NVIC_Init(&NVIC_InitStructure);

    /* Enable Receive Data register not empty interrupt */
    USART_ITConfig((USART_TypeDef*)Port(), USART_IT_RXNE, ENABLE);
  }
  
  inline u32 PortBase() override
  {
    return mu32Usart_BaseAdr;
  }

  inline volatile USART_TypeDef* Port()
  {
    return (volatile USART_TypeDef*)mu32Usart_BaseAdr;
  }

  inline void vIRQHandler(void)
  {
    // check if the USART1 transmission complete interrupt flag was set
    if (USART_GetITStatus((USART_TypeDef*)Port(), USART_IT_TXE) == SET)
    {
      if (!mcUartDataOut.isEmpty())
      {
        USART_SendData((USART_TypeDef*)Port(), mcUartDataOut.get());
      }
    }
    else
    // check if the USART1 receive data register not empty interrupt flag was set
    if (USART_GetFlagStatus((USART_TypeDef*)Port(), USART_ISR_RXNE) == SET)
    {
      mcUartDataIn.put(USART_ReceiveData((USART_TypeDef*)Port()));
    }
    else
    {
      ((USART_TypeDef*)Port())->ICR = 0x0ffff;
    }

    if (mcUartDataOut.isEmpty())
    {
      /* Disable Transmission complete interrupt */
      USART_ITConfig((USART_TypeDef*)Port(), USART_IT_TXE, DISABLE);
    }
  }

  inline void vSend(char8 lchData) override
  {
    vAdd(lchData);
    USART_ITConfig((USART_TypeDef*)Port(), USART_IT_TXE, ENABLE);
  }

  inline void vSend(const u8* lpu8Data, u16 lu16Count) override
  {
    vAdd(lpu8Data, lu16Count);
    vSend();
  }

  inline void vSend() override
  {
    USART_ITConfig((USART_TypeDef*)Port(), USART_IT_TXE, ENABLE);
  }


};


#endif // __C_USART_H__
