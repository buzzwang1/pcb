#ifndef __C_USART_H__
#define __C_USART_H__

#include "TypeDef.h"


#include "cRingBufT.h"
#include "tGPPin.h"
#include "ClockInfo.h"


typedef struct
{
  u32 CR1;         /*!< USART Control register 1,                 Address offset: 0x00 */
  u32 CR2;         /*!< USART Control register 2,                 Address offset: 0x04 */
  u32 CR3;         /*!< USART Control register 3,                 Address offset: 0x08 */
  u32 BRR;         /*!< USART Baud rate register,                 Address offset: 0x0C */
  u16 GTPR;        /*!< USART Guard time and prescaler register,  Address offset: 0x10 */
  u16 RESERVED2;       /*!< Reserved, 0x12                                                 */
  u32 RTOR;        /*!< USART Receiver Time Out register,         Address offset: 0x14 */
  u16 RQR;         /*!< USART Request register,                   Address offset: 0x18 */
  u16 RESERVED3;       /*!< Reserved, 0x1A                                                 */
  u32 ISR;         /*!< USART Interrupt and status register,      Address offset: 0x1C */
  u32 ICR;         /*!< USART Interrupt flag Clear register,      Address offset: 0x20 */
  u16 RDR;         /*!< USART Receive Data register,              Address offset: 0x24 */
  u16 RESERVED4;       /*!< Reserved, 0x26                                                 */
  u16 TDR;         /*!< USART Transmit Data register,             Address offset: 0x28 */
  u16 RESERVED5;       /*!< Reserved, 0x2A                                                 */
} USART_TypeDef;

extern USART_TypeDef USART1_BASE;
extern USART_TypeDef USART2_BASE;
extern USART_TypeDef USART3_BASE;

#define USART1               ((USART_TypeDef *) &USART1_BASE)
#define USART2               ((USART_TypeDef *) &USART2_BASE)
#define USART3               ((USART_TypeDef *) &USART3_BASE)

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
      mPinTx(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, 0),
      mPinRx(GPIO_MODE_AF_OD, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, 0)
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
    //UART_HandleTypeDef UartHandle = {};
    //
    ///* Enable USART clock */
    //switch (mu32Usart_BaseAdr)
    //{
    //  case USART1_BASE: __HAL_RCC_USART1_CLK_ENABLE();break;
    //  case USART2_BASE: __HAL_RCC_USART2_CLK_ENABLE();break;
    //  case USART3_BASE: __HAL_RCC_USART3_CLK_ENABLE();break;
    //}
    //
    //UartHandle.Instance        = (USART_TypeDef*)this->Port();
    //UartHandle.Init.BaudRate   = lu32Baudrate;
    //UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    //UartHandle.Init.StopBits   = USART_STOPBITS_1;
    //UartHandle.Init.Parity     = USART_PARITY_NONE;
    //UartHandle.Init.Mode       = USART_MODE_TX_RX;
    //UartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    //UartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;


    /* Connect PXx to USARTx_Tx */
    mPinTx.vSetAF(GPIO_MODE_AF_PP, mui16PinAF);
    /* Connect PXx to USARTx_Rx */
    mPinRx.vSetAF(GPIO_MODE_AF_OD, mui16PinAF);

    // Init function calls internally user function HAL_UART_MspInit
    // which is written below
    //HAL_UART_Init(&UartHandle);
    //
    ///* Enable USART */
    ////USART_Cmd((USART_TypeDef*)Port(), ENABLE);
    //
    ////-----------------------------------------
    //__HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
    //switch (mu32Usart_BaseAdr)
    //{
    //  case USART1_BASE: HAL_NVIC_SetPriority(USART1_IRQn, 8, 8);HAL_NVIC_EnableIRQ(USART1_IRQn);;break;
    //  case USART2_BASE: HAL_NVIC_SetPriority(USART2_IRQn, 8, 8);HAL_NVIC_EnableIRQ(USART2_IRQn);;break;
    //  case USART3_BASE: HAL_NVIC_SetPriority(USART3_IRQn, 8, 8);HAL_NVIC_EnableIRQ(USART3_IRQn);;break;
    //}
  }

  u32 PortBase() override
  {
    return mu32Usart_BaseAdr;
  }

  volatile USART_TypeDef* Port()
  {
    return (volatile USART_TypeDef*)mu32Usart_BaseAdr;
  }

  inline void vIRQHandler(void)
  {
    // check if the USART1 transmission complete interrupt flag was set
    //if ((LL_USART_IsActiveFlag_TXE((USART_TypeDef*)Port())) &&
    //    (LL_USART_IsEnabledIT_TXE((USART_TypeDef*)Port())))
    //{
    //  if (!mcUartDataOut.isEmpty())
    //  {
    //    LL_USART_TransmitData8((USART_TypeDef*)Port(), mcUartDataOut.get());
    //  }
    //}
    //else
    //// check if the USART1 receive data register not empty interrupt flag was set
    //if (LL_USART_IsActiveFlag_RXNE((USART_TypeDef*)Port()))
    //{
    //  mcUartDataIn.put(LL_USART_ReceiveData8((USART_TypeDef*)Port()));
    //}
    //else
    //{
    //  ((USART_TypeDef*)Port())->ICR = 0x0ffff;
    //}
    //
    //if (mcUartDataOut.isEmpty())
    //{
    //  /* Disable Transmission complete interrupt */
    //  LL_USART_DisableIT_TXE((USART_TypeDef*)Port());
    //}
  }

  inline void vSend(char8 lchData) override
  {
    vAdd(lchData);
    //LL_USART_EnableIT_TXE((USART_TypeDef*)Port());
  }

  inline void vSend(void) override
  {
    //LL_USART_EnableIT_TXE((USART_TypeDef*)Port());
  }

};


#endif // __C_USART_H__
