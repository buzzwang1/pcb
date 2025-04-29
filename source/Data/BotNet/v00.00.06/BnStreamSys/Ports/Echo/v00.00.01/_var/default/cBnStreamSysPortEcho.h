#ifndef __BOTNET_STREAM_SYS_PORT_ECHO_H__
#define __BOTNET_STREAM_SYS_PORT_ECHO_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"

#include "cBnStreamSysPortBase.h"

class cBotNetStreamPortEcho : public cBotNetStreamPort
{
public:
  cBotNetStreamPortEcho()
  {
    mszName = (char*)"BnComEcho";
  }

  inline uint32 SizeOf()
  {
    return 0;
  }

  void vProcess(void)
  {
    if (mcRxRingBuf.cnt())
    {
      mcRxRingBuf.move(&mcTxRingBuf, mcRxRingBuf.cnt());
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


#endif // __BOTNET_STREAM_SYS_PORT_ECHO_H__
