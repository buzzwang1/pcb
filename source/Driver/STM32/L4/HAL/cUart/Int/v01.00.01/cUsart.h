#ifndef __C_USART_H__
#define __C_USART_H__

#include "TypeDef.h"
#include "stm32l4xx.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_lpuart.h"

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
  virtual void vSetWakeup(bool lbActivate) = 0;

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
      mPinTx(GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM, 1),
      mPinRx(GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_MEDIUM, 1)
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
    UART_HandleTypeDef UartHandle = {};

    /* Enable USART clock */
    switch (mu32Usart_BaseAdr)
    {
      case USART1_BASE: __HAL_RCC_USART1_CLK_ENABLE(); __HAL_RCC_USART1_CONFIG(RCC_USART1CLKSOURCE_SYSCLK); break;
      case USART2_BASE: __HAL_RCC_USART2_CLK_ENABLE(); __HAL_RCC_USART2_CONFIG(RCC_USART2CLKSOURCE_SYSCLK); break;
      case USART3_BASE: __HAL_RCC_USART3_CLK_ENABLE(); __HAL_RCC_USART3_CONFIG(RCC_USART3CLKSOURCE_SYSCLK); break;
    }

    UartHandle.Instance        = (USART_TypeDef*)this->Port();
    UartHandle.Init.BaudRate   = lu32Baudrate;
    UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits   = USART_STOPBITS_1;
    UartHandle.Init.Parity     = USART_PARITY_NONE;
    UartHandle.Init.Mode       = USART_MODE_TX_RX;
    UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


    /* Connect PXx to USARTx_Tx */
    mPinTx.vSetAF(GPIO_MODE_AF_PP, mui16PinAF);
    /* Connect PXx to USARTx_Rx */
    mPinRx.vSetAF(GPIO_MODE_AF_OD, mui16PinAF);

    // Init function calls internally user function HAL_UART_MspInit
    // which is written below
    HAL_UART_Init(&UartHandle);

    /* Enable USART */
    //USART_Cmd((USART_TypeDef*)Port(), ENABLE);

    //-----------------------------------------
    __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
    switch (mu32Usart_BaseAdr)
    {
      case USART1_BASE: HAL_NVIC_SetPriority(USART1_IRQn, 8, 8);HAL_NVIC_EnableIRQ(USART1_IRQn);;break;
      case USART2_BASE: HAL_NVIC_SetPriority(USART2_IRQn, 8, 8);HAL_NVIC_EnableIRQ(USART2_IRQn);;break;
      case USART3_BASE: HAL_NVIC_SetPriority(USART3_IRQn, 8, 8);HAL_NVIC_EnableIRQ(USART3_IRQn);;break;
    }
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
    // check if the USART1 transmission empty interrupt flag was set
    if ((LL_USART_IsActiveFlag_TXE((USART_TypeDef*)Port())) &&
        (LL_USART_IsEnabledIT_TXE((USART_TypeDef*)Port())))
    {
      LL_USART_DisableIT_TXE((USART_TypeDef*)Port());
      ((USART_TypeDef*)Port())->ICR = LL_USART_ISR_TXE;

      if (!mcUartDataOut.isEmpty())
      {
        LL_USART_TransmitData8((USART_TypeDef*)Port(), mcUartDataOut.get());
        LL_USART_EnableIT_TC((USART_TypeDef*)Port());
      }
    }
    else
    // check if the USART1 transmission complete interrupt flag was set
    if (LL_USART_IsActiveFlag_TC((USART_TypeDef*)Port()))
    {
      ((USART_TypeDef*)Port())->ICR = LL_USART_ISR_TC;
      if (!mcUartDataOut.isEmpty())
      {
        LL_USART_TransmitData8((USART_TypeDef*)Port(), mcUartDataOut.get());
      }
      else
      {
        /* Disable Transmission complete interrupt */        
        LL_USART_DisableIT_TC((USART_TypeDef*)Port());
      }

    }
    else
    // check if the USART1 receive data register not empty interrupt flag was set
    if (LL_USART_IsActiveFlag_RXNE((USART_TypeDef*)Port()))
    {
      mcUartDataIn.put(LL_USART_ReceiveData8((USART_TypeDef*)Port()));
    }
    else
    {
      ((USART_TypeDef*)Port())->ICR = 0x0ffff;
    }
  }

  inline void vSend(char8 lchData) override
  {
    vAdd(lchData);
    vSend();
  }

  inline void vSend(const u8* lpu8Data, u16 lu16Count) override
  {
    vAdd(lpu8Data, lu16Count);
    vSend();
  }

  inline void vSend(void) override
  {
    if (!((LL_USART_IsEnabledIT_TC((USART_TypeDef*)Port())) ||
          (LL_USART_IsEnabledIT_TXE((USART_TypeDef*)Port()))))
    {

      LL_USART_EnableIT_TXE((USART_TypeDef*)Port());
    }
  }

  inline void vSetWakeup(bool lbActivate)
  {
    if (lbActivate)
    {
      LL_LPUART_EnableInStopMode((USART_TypeDef*)Port());
    }
    else
    {
      LL_LPUART_DisableInStopMode((USART_TypeDef*)Port());
    }
  }
};


#endif // __C_USART_H__
