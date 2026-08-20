#ifndef __BOTNET_STREAM_SYS_H__
#define __BOTNET_STREAM_SYS_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cBnMsgPool.h"
#include "cStrT.h"

#include "cBnCfg.h"
#include "cBnAdr.h"
#include "cBnMsg.h"

#include "cBnStreamSysPortBase.h"
#include "cBnStreamSysPortCmd.h"

class cBotNet;

class cBotNetStreamSystem
{
  public:

  cBotNetStreamPort* mcPorts[cBotNet_StreamSysPortsCnt];
  uint8              mu8PortCnt;

  u8       mcRxComBufBuf[cBotNet_StreamSysRingBufSize];
  cRingBufT<u8, u16>  mcRxComBuf;
  u8       mcTxComBufBuf[cBotNet_StreamSysRingBufSize];
  cRingBufT<u8, u16>  mcTxComBuf;

  cBotNetStreamPort_BotNetCmdPort mcCmdPort;

  cBotNetAdress          mcAdr;

  cBotNet* mcBotNet;

  cBotNetStreamSystem(cBotNet* lcBn);

  u16 AddPort(cBotNetStreamPort* lcPort)
  {
    if (mu8PortCnt < cBotNet_StreamSysPortsCnt)
    {
      mcPorts[mu8PortCnt] = lcPort;
      mcPorts[mu8PortCnt]->vAddedToBn(mcBotNet);
      mu8PortCnt++;
      return (mu8PortCnt-1);
    }
    return 0xFFFF;
  }

  void ConnectPort(u8 lu8SourcePortIdx, u16 lu8DestPortAdr,  u8 lu8DestPortIdx)
  {
    if (mcPorts[lu8SourcePortIdx] != null)
    {
      if (lu8DestPortAdr == 0) // interne Connection
      {
        if (mcPorts[lu8DestPortIdx] != null)
        {
          mcPorts[lu8SourcePortIdx]->Connect(mcPorts[lu8DestPortIdx]);
        }
      }
      else
      {
        // externe Connection
        mcPorts[lu8SourcePortIdx]->Connect(lu8DestPortAdr, lu8DestPortIdx);
      }
    }
  }

  void ConnectPort(uint8 lu8SourcePortIdx, uint8 lu8DestPortIdx)
  {
    ConnectPort(lu8SourcePortIdx, 0, lu8DestPortIdx);
  }

  void DisconnectPort(uint8 lu8SourcePortIdx, uint8 lu8DestPortIdx)
  {
    if ((mcPorts[lu8SourcePortIdx] != null) && (mcPorts[lu8DestPortIdx] != null))
    {
      mcPorts[lu8SourcePortIdx]->Disconnect(mcPorts[lu8DestPortIdx]);
    }
  }

  void SetPortBnAdress(uint8 lu8PortIdx, uint16 lui16Adr)
  {
    if (mcPorts[lu8PortIdx] != null)
    {
      mcPorts[lu8PortIdx]->mcBnDestAdr.Set(lui16Adr);
    }
  }

  //Extern
  bool bPut(u8 lu8PoolIdx)
  {
    if (mcRxComBuf.space_left())
    {
      mcRxComBuf.put(lu8PoolIdx);
      cBnMsgPool::vPutMsg(lu8PoolIdx);
      return True;
    }
    else
    {
      return False;
    }
  }

  u8 u8Get()
  {
    u8 lu8PoolIdx = mcTxComBuf.get();
    return lu8PoolIdx;
  }


  u8 u8PutIntCmd(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u8 luCmd, u8 luStream, u8* lpuData, u8 luSize);  // __attribute__((optimize("-O0")));
  u8 u8PutIntData(cBotNetAdress lcSAdr, cBotNetAdress lcDAdr, u8 luStream, u8* lpuData, u8 luSize);  // __attribute__((optimize("-O0")));


  // Intern
  u8 u8PutInt(u8 lu8PoolIdx)
  {
    if (mcTxComBuf.space_left())
    {
      mcTxComBuf.put(lu8PoolIdx);
      cBnMsgPool::vPutMsg(lu8PoolIdx);
      return True;
    }
    return mcTxComBuf.space_left();
  }

  void vTick10ms()
  {
    u16 lu16t;
    for (lu16t = 0; lu16t < mu8PortCnt; lu16t++)
    {
      mcPorts[lu16t]->vTick10ms();
    }
  }

  void vProcess()
  {
    u8  mu8PoolIdx;
    u8  mu8PoolIdxRx;

    // Eingansdaten verarbeiten
    while (1)
    {
      mu8PoolIdxRx = mcRxComBuf.get();

      if (mu8PoolIdxRx > 0)
      {
        cBotNetMsg_StreamProt mcMsgStream; cBnMsgPool::vGetMsg(mcMsgStream, mu8PoolIdxRx);

        u8 lu8PortIdx = mcMsgStream.uGetStreamIdx();

        if (mcPorts[lu8PortIdx] != null)
        {
          if (mcMsgStream.isCommand())
          {
            u8 lu8Ack = 1;
            switch (mcMsgStream.uGetCommand())
            {
              case 1: // Clear Buffer
                mcPorts[lu8PortIdx]->mcRxRingBuf.reset();
                mcPorts[lu8PortIdx]->mcTxRingBuf.reset();
                lu8Ack = 0;
                break;
              case 2: // Connect Port
                ConnectPort(lu8PortIdx, (mcMsgStream.GetPayload()[0] << 8) + mcMsgStream.GetPayload()[1], mcMsgStream.GetPayload()[2]);
                lu8Ack = 0;
                break;
              case 3: // Disconnect Port
                mcPorts[lu8PortIdx]->Disconnect(mcPorts[mcMsgStream.GetPayload()[0]]);
                lu8Ack = 0;
              break;
              default:
                break;
            }

            // Cmd = 0  => Ack Nachricht
            // Im Moment wird alles ge'Ack't
            u8PutIntCmd(mcMsgStream.cGetDAdr(), mcMsgStream.cGetSAdr(), 0, (u8)lu8PortIdx, &lu8Ack, 1);
          }
          else
          {
            if (mcMsgStream.GetPayload().Len() >= 1) /* Mind. ein Datenbyte */
            {
              mcPorts[lu8PortIdx]->Put(mcMsgStream.GetPayload());
            }
          }
        }
        cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
      }
      else
      {
        break;
      }
    }

    // Interne Daten verarbeiten
    for (int i = 0; i < mu8PortCnt; i++)
    {
      if (mcPorts[i] != null)
      {
        mcPorts[i]->vProcess();
      }
    }


    for (int i = 0; i < mu8PortCnt; i++)
    {
      if (mcPorts[i] != null)
      {
        uint16 lui16Data2Copy;

        lui16Data2Copy = mcPorts[i]->mcTxRingBuf.cnt();

        // Überprüfen ob die Connections genug freien Platz haben
        for (int t = 0; (t < cBotNet_StreamSysPortsCnxCnt) && (lui16Data2Copy); t++)
        {
          if (mcPorts[i]->mcConnection[t] != null)
          {
            if (lui16Data2Copy > mcPorts[i]->mcConnection[t]->mcRxRingBuf.space_left())
            {
              lui16Data2Copy = mcPorts[i]->mcConnection[t]->mcRxRingBuf.space_left();
            }
          }
        }

        // Überprüfen ob der Output genug freien Platz hat
        if (lui16Data2Copy)
        {
          // Externe Connections verarbeiten
          if (mcPorts[i]->mcBnDestAdr.Get() != 0)
          {
            if (!mcTxComBuf.space_left()) // worst case, genug Platz für eine Nachricht ?
            {
              lui16Data2Copy = 0;
            }
            else
            {
              // ggf. Daten kürzen, damit sie in eine Nachricht passen
              if (lui16Data2Copy > (cBotNet_MsgSize - (cBotNet_MsgHeaderSize + 1)))  // Max. Größe einer Nachricht, minus max. Größe BnHeader, minus Größe StreamProtHeader
                                                                                     // + 1 für BnStream Protokoll Header
              {
                lui16Data2Copy = (cBotNet_MsgSize - (cBotNet_MsgHeaderSize + 1));
              }
            }
          }
        }


        //Daten übernehmen
        if (lui16Data2Copy)
        {
          cBotNetMsg_StreamProt mcMsgStreamTx; cBnMsgPool::vReqMsg(mcMsgStreamTx, mu8PoolIdx, lui16Data2Copy + (cBotNet_MsgHeaderSize + 1));
          if (mu8PoolIdx)
          {

            // Bei externer Connections
            if (mcPorts[i]->mcBnDestAdr.Get() != 0)
            {
              //gegebenenfalls mit Adressen
              mcPorts[i]->Get(mcMsgStreamTx.cPrepareData(mcPorts[i]->mcBnDestPortIdx), lui16Data2Copy);
              mcMsgStreamTx.vEncode(mcAdr, mcPorts[i]->mcBnDestAdr);
              cBnMsgPool::vSetLen(mu8PoolIdx, mcMsgStreamTx.muLen);
              u8PutInt(mu8PoolIdx);
            }
            else
            {
              //interne Nachricht braucht keine Adressen
              mcPorts[i]->Get(mcMsgStreamTx.cPrepareData(mcPorts[i]->mcBnDestPortIdx), lui16Data2Copy);
              mcMsgStreamTx.vEncode(0, 0);
              cBnMsgPool::vSetLen(mu8PoolIdx, mcMsgStreamTx.muLen);
              u8PutInt(mu8PoolIdx);
            }
            cBnMsgPool::vReleaseMsg(mu8PoolIdx);
		  }


          if (mcMsgStreamTx.mcPayload.Len() > 0)
          {
            // Interne Connections verarbeiten
            for (int t = 0; t < cBotNet_StreamSysPortsCnxCnt; t++)
            {
              if (mcPorts[i]->mcConnection[t] != null)
              {
                mcPorts[i]->mcConnection[t]->Put(mcMsgStreamTx.GetPayload());
              }
            }
          }
        }
      }
    }
  }
};


#endif // __BOTNET_STREAM_SYS_H__
