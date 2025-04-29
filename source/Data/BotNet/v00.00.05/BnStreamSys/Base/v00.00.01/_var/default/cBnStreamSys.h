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
  uint8              mu8PortCnt;

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

  void vReInit()
  {
    mcRxComBuf.reset();
    mcTxComBuf.reset();
  }

  inline uint32 SizeOf()
  {
    uint32 lui32Size;

    lui32Size = sizeof(cBotNetStreamSystem);
    lui32Size -= 2 * sizeof(cBotNetMsg_BaseDyn);
    lui32Size -= sizeof(cBotNetStreamPort_BotNetCmdPort);

    for (int i = 0; i < cBotNet_StreamSysPortsCnt; i++)
    {
      if (mcPorts[i] != null)
      {
        lui32Size += mcPorts[i]->SizeOf();
      }
    }

    lui32Size += mcMsgStreamRx.SizeOf();
    lui32Size += mcMsgStreamTx.SizeOf();

    return lui32Size;
  }

  u16 AddPort(cBotNetStreamPort *lcPort)
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
            switch (mcMsgStreamRx.mu8Cmd)
            {
              case 1: // Clear Buffer
                mcPorts[lu8PortIdx]->mcRxRingBuf.reset();
                mcPorts[lu8PortIdx]->mcTxRingBuf.reset();
              break;
              case 2: // Connect Ports
                ConnectPort(lu8PortIdx, (mcMsgStreamRx.mcPayload.mpu8Data[0] << 8) + mcMsgStreamRx.mcPayload.mpu8Data[1], mcMsgStreamRx.mcPayload.mpu8Data[2]);
              break;
              case 3: // Disconnect Buffer
                mcPorts[lu8PortIdx]->Disconnect(mcPorts[mcMsgStreamRx.mcPayload.mpu8Data[0]]);
              break;
              default:
              break;
            }
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


          if (mcMsgStreamTx.mcPayload.Len() > 0)
          {
            // Interne Connections verarbeiten
            for (int t = 0; t < cBotNet_StreamSysPortsCnxCnt; t++)
            {
              if (mcPorts[i]->mcConnection[t] != null)
              {
                mcPorts[i]->mcConnection[t]->Put(&mcMsgStreamTx.mcPayload);
              }
            }
          }
        }
      }
    }
  }
};


#endif // __BOTNET_STREAM_SYS_H__
