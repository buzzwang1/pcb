#ifndef __BOTNET_STREAM_SYS_H__
#define __BOTNET_STREAM_SYS_H__

#include "Typedef.h"
#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
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
    UNUSED(lcPort);
    return 0xFFFF;
  }
  void ConnectPort(u8 lu8SourcePortIdx, u16 lu8DestPortAdr,  u8 lu8DestPortIdx)
  {
    if (lu8DestPortAdr != 0) // hier nur externe Connection
    {
      // externe Connection
      mcPorts[lu8SourcePortIdx]->Connect(lu8DestPortAdr, lu8DestPortIdx);
    }
  }

  void ConnectPort(uint8 lu8SourcePortIdx, uint8 lu8DestPortIdx)
  {
    ConnectPort(lu8SourcePortIdx, 0, lu8DestPortIdx);
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

  // Intern
  bool bPutInt(u8 lu8PoolIdx)
  {
    if (mcTxComBuf.space_left())
    {
      mcTxComBuf.put(lu8PoolIdx);
      cBnMsgPool::vPutMsg(lu8PoolIdx);
      cBnMsgPool::vSetDecoded(lu8PoolIdx);
      return True;
    }
    else
    {
      return False;
    }
  }

  u8 u8GetInt(cBotNetMsg_Base** lcMsg)
  {
    u8 lu8PoolIdx = mcRxComBuf.get();
    cBnMsgPool::vGetMsg(lu8PoolIdx, lcMsg);
    return lu8PoolIdx;
  }


  void vTick10ms()
  {
    // CMD-Port hat kein vTick10ms
    //u16 lu16t;
    //for (lu16t = 0; lu16t < 1 /*mu8PortCnt*/; lu16t++)
    //{
    //  mcPorts[lu16t]->vTick10ms();
    //}
  }

  void vProcess()
  {
    u8  mu8PoolIdx;
    u8  mu8PoolIdxRx;

    cBotNetMsg_Base*        mcMsgBase = null;

    // Eingansdaten verarbeiten
    while (1)
    {
      mu8PoolIdxRx = u8GetInt(&mcMsgBase);

      if (mu8PoolIdxRx > 0)
      {
        __asm("nop");
        cBotNetMsg_StreamProt2  mcMsgStream(mcMsgBase);
        mcMsgStream.vDecode2();

        u16 lu16Src = mcMsgBase->cGetSAdr().Get();
        u16 lu16Dst = mcMsgBase->cGetDAdr().Get();

        uint8 lu8PortIdx = mcMsgStream.mu8StreamIdx;

        if (mcPorts[lu8PortIdx] != null)
        {
          if (mcMsgStream.mbCmd)
          {
            u8 lu8Ack = 1;
            switch (mcMsgStream.mu8Cmd)
            {
              case 1: // Clear Buffer
                mcPorts[lu8PortIdx]->mcRxRingBuf.reset();
                mcPorts[lu8PortIdx]->mcTxRingBuf.reset();
                lu8Ack = 0;
              break;
              case 2: // Connect Port
                ConnectPort(lu8PortIdx, (mcMsgStream.mcPayload.mpu8Data[0] << 8) + mcMsgStream.mcPayload.mpu8Data[1], mcMsgStream.mcPayload.mpu8Data[2]);
                lu8Ack = 0;
              break;
              /*case 3: // Disconnect Port
                mcPorts[lu8PortIdx]->Disconnect(mcPorts[mcMsgStream.mcPayload.mpu8Data[0]]);
              break;*/
              default:
              break;
            }

            // Cmd = 0  => Ack Nachricht
            // Im Moment wird alles ge'Ack't
            mu8PoolIdx = cBnMsgPool::u8ReqMsg(16);
            cBnMsgPool::vGetMsg(mu8PoolIdx, &mcMsgBase);
            cBotNetMsg_StreamProt2 mcMsgStreamTx(mcMsgBase);

            mcMsgStreamTx.vPrepareCmd(lu16Dst, lu16Src, 0, (u8)lu8PortIdx);
            mcMsgStreamTx.mcPayload.Set(lu8Ack);
            mcMsgStreamTx.vEncode();
            bPutInt(mu8PoolIdx);
            cBnMsgPool::vReleaseMsg(mu8PoolIdx);
          }
          else
          {
            if (mcMsgStream.mcPayload.Len() >= 1) /* Mind. ein Datenbyte */
            {
              mcPorts[lu8PortIdx]->Put(&mcMsgStream.mcPayload);
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
    for (int i = 0; i < cBotNet_StreamSysPortsCnt; i++)
    {
      if (mcPorts[i] != null)
      {
        mcPorts[i]->vProcess();
      }
    }


    for (int i = 0; i < cBotNet_StreamSysPortsCnt; i++)
    {
      if (mcPorts[i] != null)
      {
        uint16 lui16Data2Copy;

        lui16Data2Copy = mcPorts[i]->mcTxRingBuf.cnt();


        // Überprüfen ob der Output genug freien Platz hat
        if (lui16Data2Copy)
        {
          // Externe Connections verarbeiten
          if (mcPorts[i]->mcBnDestAdr.Get() != 0)
          {
            if (!mcTxComBuf.space_left()) // worst case, genug Platz für eine Nachricht ?
            {
              lui16Data2Copy = 0;
              __asm("nop");
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
          // Bei externer Connections
          if (mcPorts[i]->mcBnDestAdr.Get() != 0)
          {
            //gegebenenfalls mit Adressen
            mu8PoolIdx = cBnMsgPool::u8ReqMsg(lui16Data2Copy + (cBotNet_MsgHeaderSize + 1));
            if (mu8PoolIdx > 0)
            {
              cBnMsgPool::vGetMsg(mu8PoolIdx, &mcMsgBase);
              cBotNetMsg_StreamProt2 mcMsgStreamTx(mcMsgBase);
              mcMsgStreamTx.vPrepareData(mcAdr.Get(), mcPorts[i]->mcBnDestAdr.Get(), mcPorts[i]->mcBnDestPortIdx);
              mcPorts[i]->Get(&mcMsgStreamTx.mcPayload, lui16Data2Copy);
              mcMsgStreamTx.vEncode();
              bPutInt(mu8PoolIdx);
              cBnMsgPool::vReleaseMsg(mu8PoolIdx);
            }
          }
          //else
          //{
          //  //interne Nachricht braucht keine Adressen
          //  mu8PoolIdx = cBnMsgPool::u8ReqMsg(lui16Data2Copy + (cBotNet_MsgHeaderSize + 1));
          //  if (mu8PoolIdx > 0)
          //  {
          //    cBnMsgPool::vGetMsg(mu8PoolIdx, &mcMsgStreamTx);
          //    mcMsgStreamTx.vPrepareData(0, 0, 0);
          //    mcPorts[i]->Get(&mcMsgStreamTx.mcPayload, lui16Data2Copy);
          //    mcMsgStreamTx.vEncode();
          //    bPutInt(mu8PoolIdx);
          //    cBnMsgPool::vSetLen(mu8PoolIdx, mcMsgStream.muiLen);
          //    cBnMsgPool::vReleaseMsg(mu8PoolIdx);
          //  }
          //}
        }
      }
    }
  }
};


#endif // __BOTNET_STREAM_SYS_H__
