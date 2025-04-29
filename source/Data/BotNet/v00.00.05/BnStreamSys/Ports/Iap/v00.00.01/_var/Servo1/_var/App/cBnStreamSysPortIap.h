#ifndef __BOTNET_STREAM_SYS_PORT_IAP_H__
#define __BOTNET_STREAM_SYS_PORT_IAP_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
#include "cStrT.h"

#include "cBnStreamSysPortBase.h"
#include "cCli.h"
#include "iap_platform.h"
#include "core_cm3.h"

class cBotNet;


class cBotNetStreamPort_BotNetMemPort: public cBotNetStreamPort
{
  public:

  typedef enum
  {
    enIdle = 0,
    enWaitForCmdSessionStart,
    enWaitForCmdData,
    enWaitForWriteData,
    enProcessing,
    enExit,
  }tenMainStates;

  typedef enum
  {
    enNop = 0,
    enInterfaceInfo1,
    enInterfaceInfo2,

    enInterfaceMemoryInfo1,
    enInterfaceMemoryInfo2,
    enInterfaceMemoryInfo3,
    enInterfaceMemoryInfo4,
    enInterfaceMemoryInfo5,

    enWrite1,
    enWrite2,
    enWrite3,

    enRead1,
    enRead2,
    enRead3,

    enChecksum1,
    enChecksum2,

    enCall1,
    enCall2,

    enJump1,
    enJump2,

    enExit1,

    enEnter1,
    enEnter2,

  }tenCmdStates;

  tenMainStates menMainState;

  uint8  mui8InBuf[32];
  uint8  mui8OutBuf[32];
  uint8 mui8WaitReset;

  cBotNetStreamPort_BotNetMemPort(u8 lu8PartitionCnt, const tstMemPart_Partition* lstPartitions)
  {
    UNUSED(lu8PartitionCnt);
    UNUSED(lstPartitions);

    mszName    = "BnMemPort v01.00.02";

    menMainState    = enWaitForCmdSessionStart;
    mui8WaitReset   = 0;
  }

  virtual void vReInit()
  {
    cBotNetStreamPort::vReInit();
    menMainState = enWaitForCmdSessionStart;
    mui8WaitReset = 0;
  }
  
  void vTick10ms(void)
  {
    if (mui8WaitReset > 0)
    {
      mui8WaitReset--;

      if (mui8WaitReset == 0)
      {
        NVIC_SystemReset();
      }
    }
  }

  void vAddedToBn(cBotNet* lcBotNet) override { UNUSED(lcBotNet);}

  void vProcess(void)
  {
    if (menMainState == enWaitForCmdSessionStart)
    {
      if (mcRxRingBuf.cnt() >= 32)
      {
        mcRxRingBuf.get(mui8InBuf, 1);

        if (mui8InBuf[0] == 0x81) // Enter
        {
          uint32* lui32Temp;

          mcRxRingBuf.get(&mui8InBuf[1], 31);

          for (u8 luit=0; luit < 32; luit++) mui8OutBuf[luit] = mui8InBuf[luit];

          lui32Temp = ((u32*)&mui8InBuf[ 4]);

          if (*lui32Temp == 0x01020304)
          {
            mui8OutBuf[1] = 0; // ACK
            //Reset in 100ms, damit die Antwort noch raus kann.
            mui8WaitReset = 10;
          }
          else
          {
            mui8OutBuf[1] = 1; //NACK
          }
          mcTxRingBuf.put(mui8OutBuf, 32);
        }
      }
    }
  }
};



#endif // __BOTNET_STREAM_SYS_PORT_IAP_H__
