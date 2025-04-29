#ifndef __C_USARTHD_H__
#define __C_USARTHD_H__

#include "Typedef.h"
#include "gd32f1x0.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_usart.h"
#include "gd32f1x0_dma.h"

#include "cRingBufT.h"
#include "tGPPin.h"
#include "ClockInfo.h"


class cUartHd
{
  public:
  cRingBufDynT<u8, u16> mcUartDataIn;
  cRingBufDynT<u8, u16> mcUartDataOut;

  cUartHd(const u16 mui16TxBufSize,
          const u16 mui16RxBufSize)
     : mcUartDataIn(mui16RxBufSize),
       mcUartDataOut(mui16TxBufSize)
  {
  }

  virtual void vHwInit(const uint32 lu32Baudrate,
                       const u16 mui16PinAF) = 0;


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


template <const u32 mu32Usart_BaseAdr,
          const u32 mu32TxPort_BaseAdr, const u16 mui16TxPinNr>
class tcUartHd: public cUartHd
{
public:

  tcGpPin<mu32TxPort_BaseAdr, mui16TxPinNr> mPinTx;

  using cUartHd::vAdd;
  using cUartHd::vSend;
  using cUartHd::vPrintMem;

  tcUartHd(const u16 mui16TxBufSize,
           const u16 mui16RxBufSize)
    : cUartHd(mui16TxBufSize, mui16RxBufSize),
      mPinTx(GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, GPIO_OTYPE_OD, GPIO_OSPEED_10MHZ, 1)
  {
  }

  tcUartHd(const uint32 lu32Baudrate,
           const u16 mui16PinAF,
           const u16 mui16TxBufSize,
           const u16 mui16RxBufSize)
    : tcUartHd(mui16TxBufSize, mui16RxBufSize)
  {
    vHwInit(lu32Baudrate, mui16PinAF);
  }

  void vHwInit(const uint32 lu32Baudrate,
               const u16 mui16PinAF) override
  {
    // Enable USART clock
    switch (mu32Usart_BaseAdr)
    {
      case USART0: rcu_periph_clock_enable(RCU_USART0);break;
      case USART1: rcu_periph_clock_enable(RCU_USART1);break;
    }
		
    // Usart Peripheral Disable
    usart_deinit(mu32Usart_BaseAdr);
		usart_halfduplex_enable(mu32Usart_BaseAdr);
    usart_baudrate_set(mu32Usart_BaseAdr,    lu32Baudrate);
    usart_receive_config(mu32Usart_BaseAdr,  USART_RECEIVE_ENABLE);		
		
		
    // Connect PXx to USARTx_Tx
		mPinTx.vSetMode(GPIO_MODE_AF, GPIO_PUPD_PULLUP);
    mPinTx.vSetAF(mui16PinAF);
	
		
    // Enable USART
		usart_enable(mu32Usart_BaseAdr);

    //-----------------------------------------
		usart_interrupt_enable(mu32Usart_BaseAdr, USART_INT_RBNE);
		
    switch (mu32Usart_BaseAdr)
    {
      case USART0: nvic_irq_enable(USART0_IRQn, 1, 1);break;
      case USART1: nvic_irq_enable(USART1_IRQn, 1, 1);break;
    }
  }

  inline u32 Port()
  {
    return mu32Usart_BaseAdr;
  }


  inline void vIRQHandler(void)
  {
    // check if the USART1 transmission empty interrupt flag was set
    if (usart_interrupt_flag_get(mu32Usart_BaseAdr, USART_INT_FLAG_TBE))
    {
			usart_interrupt_disable(mu32Usart_BaseAdr, USART_INT_TBE);
			USART_INTC(mu32Usart_BaseAdr) = USART_FLAG_TBE;

      if (!mcUartDataOut.isEmpty())
      {
				usart_data_transmit(mu32Usart_BaseAdr, mcUartDataOut.get());
				usart_interrupt_enable(mu32Usart_BaseAdr, USART_INT_TC);
      }
    }
    else
    // check if the USART1 transmission complete interrupt flag was set
    if (usart_interrupt_flag_get(mu32Usart_BaseAdr, USART_INT_FLAG_TC))
    {
      USART_INTC(mu32Usart_BaseAdr) = USART_FLAG_TC;
      if (!mcUartDataOut.isEmpty())
      {
        usart_data_transmit(mu32Usart_BaseAdr, mcUartDataOut.get());
      }
      else
      {
        usart_transmit_config(mu32Usart_BaseAdr, USART_TRANSMIT_DISABLE);
        usart_receive_config(mu32Usart_BaseAdr,  USART_RECEIVE_ENABLE);		

        // Disable Transmission complete interrupt        
  			usart_interrupt_disable(mu32Usart_BaseAdr, USART_INT_TC);
      }
    }
    else
    // check if the USART1 receive data register not empty interrupt flag was set
		if (usart_interrupt_flag_get(mu32Usart_BaseAdr, USART_INT_FLAG_RBNE))
    {
      mcUartDataIn.put(usart_data_receive(mu32Usart_BaseAdr));
    }
    else
    {
			USART_INTC(mu32Usart_BaseAdr) = 0x0ffff;
    }
  }

  inline void vSend(char8 lchData) override
  {
    vAdd(lchData);
    vSend();
  }

  inline void vSend(void) override
  {
    if (!((USART_REG_VAL(mu32Usart_BaseAdr, USART_INT_TC)  & BIT(USART_BIT_POS(USART_INT_TC))) ||
          (USART_REG_VAL(mu32Usart_BaseAdr, USART_INT_TBE) & BIT(USART_BIT_POS(USART_INT_TBE)))))
    {
      usart_receive_config(mu32Usart_BaseAdr,  USART_RECEIVE_DISABLE);
      usart_transmit_config(mu32Usart_BaseAdr, USART_TRANSMIT_ENABLE);

      usart_interrupt_enable(mu32Usart_BaseAdr, USART_INT_TBE);
    }
  }

};


#endif // __C_USARTHD_H__
