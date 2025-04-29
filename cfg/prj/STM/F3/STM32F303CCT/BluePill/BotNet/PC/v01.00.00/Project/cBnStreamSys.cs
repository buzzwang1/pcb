using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using uint64 = System.UInt64;
using uint32 = System.UInt32;
using uint16 = System.UInt16;
using uint8 = System.Byte;
using u64 = System.UInt64;
using u32 = System.UInt32;
using u16 = System.UInt16;
using u8 = System.Byte;

using int64 = System.Int64;
using int32 = System.Int32;
using int16 = System.Int16;
using int8 = System.SByte;
using i64 = System.Int64;
using i32 = System.Int32;
using i16 = System.Int16;
using i8 = System.SByte;

public class cBotNetStreamSystem
{
  public cBotNetStreamPort[] mcPorts;
  public uint8               mu8PortCnt;

  public Queue<cBotNetMsg_Base> mcRxComBuf;
  public Queue<cBotNetMsg_Base> mcTxComBuf;

  //cBotNetMsg_StreamProt mMsgTx;

  public cBotNetStreamPort_BotNetCmdPort mcCmdPort;

  public cBotNet                mcBn;

  public cBotNetStreamSystem(cBotNet lcBn)
  {
    mcRxComBuf = new Queue<cBotNetMsg_Base>();
    mcTxComBuf = new Queue<cBotNetMsg_Base>();
    
    mcCmdPort = new cBotNetStreamPort_BotNetCmdPort();

    mcBn = lcBn;

    mcPorts =  new cBotNetStreamPort[cBnCfg.cBotNet_StreamSysPortsCnt];

    mu8PortCnt = 0;
    for (int i = 0; i < cBnCfg.cBotNet_StreamSysPortsCnt; i++)
    {
      mcPorts[i] = null;

    }

    AddPort(mcCmdPort);
  }

  public u16 AddPort(cBotNetStreamPort lcPort)
  {
    if (mu8PortCnt < cBnCfg.cBotNet_StreamSysPortsCnt)
    {
      mcPorts[mu8PortCnt] = lcPort;
      mcPorts[mu8PortCnt].mcPrinter = mcCmdPort.mcMyPrinter;
      mu8PortCnt++;
      return (u16)(mu8PortCnt-1);
    }
    return 0xFFFF;
  }

  public void ConnectPort(uint8 lu8SourcePortIdx, uint8 lu8DestPortIdx)
  {
    if ((mcPorts[lu8SourcePortIdx] != null) && (mcPorts[lu8DestPortIdx] != null))
    {
      mcPorts[lu8SourcePortIdx].Connect(mcPorts[lu8DestPortIdx]);
    }
  }

  public void DisconnectPort(uint8 lu8SourcePortIdx, uint8 lu8DestPortIdx)
  {
    if ((mcPorts[lu8SourcePortIdx] != null) && (mcPorts[lu8DestPortIdx] != null))
    {
      mcPorts[lu8SourcePortIdx].Disconnect(mcPorts[lu8DestPortIdx]);
    }
  }

  public void SetPortBnAdress(uint8 lu8PortIdx, uint16 lui16Adr)
  {
    if (mcPorts[lu8PortIdx] != null)
    {
      mcPorts[lu8PortIdx].mcBnDestAdr.Set(lui16Adr);
    }
  }

  public void put(cBotNetMsg_Base lcMsg)
  {
    mcRxComBuf.Enqueue(lcMsg);
  }

  public cBotNetMsg_Base get()
  {
    return mcTxComBuf.Dequeue();
  }

  public void vProcess()
  {
    cBotNetMsg_StreamProt mcMsgStreamRx;

    // Eingansdaten verarbeiten
    while (true)
    {
      if (mcRxComBuf.Count > 0)
      {
        mcMsgStreamRx = new cBotNetMsg_StreamProt(mcRxComBuf.Dequeue());

        uint8 lu8PortIdx = mcMsgStreamRx.mu8StreamIdx;

        if (mcPorts[lu8PortIdx] != null)
        {
          if (mcMsgStreamRx.mbCmd)
          {
            switch (mcMsgStreamRx.mu8Cmd)
            {
              case 1: // Clear Buffer
                mcPorts[lu8PortIdx].mcRxRingBuf.Clear();
                mcPorts[lu8PortIdx].mcTxRingBuf.Clear();
              break;
              case 2: // Connect Ports
                if ((mcMsgStreamRx.mcPayload[1] == 0) && (mcMsgStreamRx.mcPayload[2] == 0))
                {
                    // interene Verbindung
                  if (mcPorts[mcMsgStreamRx.mcPayload[3]] != null)
                  {
                    mcPorts[lu8PortIdx].Connect(mcPorts[mcMsgStreamRx.mcPayload[3]]);
                  }
                }
                else
                {
                  // externe Verbindung
                  mcPorts[lu8PortIdx].mcBnDestPortIdx = mcMsgStreamRx.mcPayload[3];
                }
              break;
              case 3: // Disconnect Buffer
                mcPorts[lu8PortIdx].Disconnect(mcPorts[mcMsgStreamRx.mcPayload[1]]);
              break;
              default:
              break;
            }
          }
          else
          {
            if (mcMsgStreamRx.mcPayload.Count >= 1) /* Mind. ein Datenbyte */
            {
              mcPorts[lu8PortIdx].Put(mcMsgStreamRx.mcPayload.ToArray());
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
        mcPorts[i].vProcess();
      }
    }
  }
};



