#ifndef __BOTNET_STREAM_SYS_H__
#define __BOTNET_STREAM_SYS_H__

#include "Typedef.h"


#include "cRingBufT.h"
#include "cbArrayT.h"
#include "cbArrayExtT.h"
#include "cComBuf.h"
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

  cComBuf  mcRxComBuf;
  cComBuf  mcTxComBuf;

  cBotNetMsg_BaseDyn     mcMsgRx;
  cBotNetMsg_StreamProt  mcMsgStreamRx;

  cBotNetMsg_BaseDyn     mcMsgTx;
  cBotNetMsg_StreamProt  mcMsgStreamTx;

  cBotNetStreamPort_BotNetCmdPort mcCmdPort;

  cBotNetAdress          mcAdr;

  cBotNet* mcBotNet;

  cBotNetStreamSystem(cBotNet* lcBn);

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


  inline bool bPut(cbArrayExtT<uint16> *lcEntry)
  {
    if (mcRxComBuf.isFitting(lcEntry))
    {
      mcRxComBuf.put(lcEntry);
      return True;
    }
    else
    {
      return False;
    }
  }

  inline void get(cbArrayExtT<uint16> *lcEntry)
  {
    mcTxComBuf.get(lcEntry);
  }

  void vProcess()
  {
    // Eingansdaten verarbeiten
    while (1)
    {
      mcRxComBuf.get(&mcMsgStreamRx);

      if (mcMsgStreamRx.Len() > 0)
      {
        /* Msg Decoder */
        mcMsgStreamRx.vDecode(0, 0);

        uint8 lu8PortIdx = mcMsgStreamRx.mu8StreamIdx;

        if (mcPorts[lu8PortIdx] != null)
        {
          if (mcMsgStreamRx.mbCmd)
          {
            u8 lu8Ack = 1;
            switch (mcMsgStreamRx.mu8Cmd)
            {
              case 1: // Clear Buffer
                mcPorts[lu8PortIdx]->mcRxRingBuf.reset();
                mcPorts[lu8PortIdx]->mcTxRingBuf.reset();
                lu8Ack = 0;
              break;
              case 2: // Connect Port
                ConnectPort(lu8PortIdx, (mcMsgStreamRx.mcPayload.mpu8Data[0] << 8) + mcMsgStreamRx.mcPayload.mpu8Data[1], mcMsgStreamRx.mcPayload.mpu8Data[2]);
                lu8Ack = 0;
              break;
              /*case 3: // Disconnect Port
                mcPorts[lu8PortIdx]->Disconnect(mcPorts[mcMsgStreamRx.mcPayload.mpu8Data[0]]);
              break;*/
              default:
              break;
            }

            // Cmd = 0  => Ack Nachricht
            // Im Moment wird alles ge'Ack't
            mcMsgStreamTx.vPrepareCmd(mcMsgStreamRx.cGetDAdr().Get(), mcMsgStreamRx.cGetSAdr().Get(), 0, (u8)lu8PortIdx);
            mcMsgStreamTx.mcPayload.Set(lu8Ack);
            mcMsgStreamTx.vEncode();
            mcTxComBuf.put(&mcMsgStreamTx);
          }
          else
          {
            if (mcMsgStreamRx.mcPayload.Len() >= 1) /* Mind. ein Datenbyte */
            {
              mcPorts[lu8PortIdx]->Put(&mcMsgStreamRx.mcPayload);
            }
          }
        }
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
            if (mcTxComBuf.space_left() < cBotNet_MsgSize) // worst case, genug Platz für eine komplette Nachricht ?
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
          // Bei externer Connections
          if (mcPorts[i]->mcBnDestAdr.Get() != 0)
          {
            //gegebenenfalls mit Adressen
            mcMsgStreamTx.vPrepareData(mcAdr.Get(), mcPorts[i]->mcBnDestAdr.Get(), mcPorts[i]->mcBnDestPortIdx);
            mcPorts[i]->Get(&mcMsgStreamTx.mcPayload, lui16Data2Copy);
            mcMsgStreamTx.vEncode();
            mcTxComBuf.put(&mcMsgStreamTx);
          }
          else
          {
            //interne Nachricht braucht keine Adressen
            mcMsgStreamTx.vPrepareData(0, 0, 0);
            mcPorts[i]->Get(&mcMsgStreamTx.mcPayload, lui16Data2Copy);
          }
        }
      }
    }
  }
};


#endif // __BOTNET_STREAM_SYS_H__
