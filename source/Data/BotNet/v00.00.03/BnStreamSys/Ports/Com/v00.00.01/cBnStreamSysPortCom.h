#ifndef __BOTNET_STREAM_SYS_PORT_COM_H__
#define __BOTNET_STREAM_SYS_PORT_COM_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"

#include "cUsart.h"

#include "cBnStreamSysPortBase.h"

class cBotNetStreamPort_ComPort : public cBotNetStreamPort
{
public:

  cUart* mcUart;

  cBotNetStreamPort_ComPort(cUart* lcUart)
  {
    mszName = (char*)"BnComPort";

    mcUart = lcUart;

    if (mcUart->PortBase() == (u32)USART1) mszName += (char*)" U1";
    else
    if (mcUart->PortBase() == (u32)USART2) mszName += (char*)" U2";
    else
    if (mcUart->PortBase() == (u32)USART3) mszName += (char*)" U3";
    else  mszName += (char*)" U?";
  }

  inline uint32 SizeOf()
  {
    return sizeof(cBotNetStreamPort) + sizeof(cBotNetStreamPort_ComPort);
  }

  void vProcess(void)
  {
    // Rx Data from HW port
    if (mcUart->mcUartDataIn.cnt())
    {
      mcUart->mcUartDataIn.move(&mcTxRingBuf, mcUart->mcUartDataIn.cnt());
    }

    // Tx data to HW port
    if (mcRxRingBuf.cnt())
    {
      mcRxRingBuf.move(&mcUart->mcUartDataOut, mcRxRingBuf.cnt());
      if (mcUart->mcUartDataOut.cnt())
      {
        mcUart->vSend();
      }
    }
  }

  bool bProcessCmd(cStr &lcStrCmd, cStr &lcStrParam)
  {
    (void)lcStrCmd; /* avoid warning */
    (void)lcStrParam; /* avoid warning */
    return False;
  }

  virtual void vPrintCmds()
  {};
};


#endif // __BOTNET_STREAM_SYS_PORT_COM_H__
