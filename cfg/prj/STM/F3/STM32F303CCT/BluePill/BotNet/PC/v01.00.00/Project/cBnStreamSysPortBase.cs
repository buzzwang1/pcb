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

public abstract class cBotNetStreamPort
{
  public cBotNetAdress mcBnDestAdr;     // == 0 interne Connection, != 0 exterene Connecion In dem Fall wird noch der Port Idx benötigt
  public uint8 mcBnDestPortIdx;

  public String         mszName;

  public Queue<u8> mcRxRingBuf;
  public Queue<u8> mcTxRingBuf;

  public cBotNetStreamPort[] mcConnection;

  public cBotNetStreamPort_CmdPrinter mcPrinter;

  public cBotNetStreamPort()
  {
    mcRxRingBuf = new Queue<u8>();
    mcTxRingBuf = new Queue<u8>();

    mcConnection = new cBotNetStreamPort[cBnCfg.cBotNet_StreamSysPortsCnxCnt];

    mcBnDestAdr = new cBotNetAdress();

    mszName = "";

    mcBnDestAdr.Set(0);
    mcBnDestPortIdx = 0;

    for (int i = 0; i < cBnCfg.cBotNet_StreamSysPortsCnxCnt; i++)
    {
      mcConnection[i] = null;
    }
  }


  public void Connect(cBotNetStreamPort lcPort)
  {
    // schon connected ?
    for (int i = 0; i < cBnCfg.cBotNet_StreamSysPortsCnxCnt; i++)
    {
      if (mcConnection[i] == lcPort)
      {
        return;
      }
    }

    for (int i = 0; i < cBnCfg.cBotNet_StreamSysPortsCnxCnt; i++)
    {
      if (mcConnection[i] == null)
      {
        mcConnection[i] = lcPort;
        return;
      }
    }
  }

  public void Put(u8 [] lpcBAryExt)
  {
    foreach (u8 lu8Item in lpcBAryExt)
    {
      mcRxRingBuf.Enqueue(lu8Item);
    }
  }


  public u8 [] Get()
  {
    List<u8> llstRet = new List<byte>();

    while (mcRxRingBuf.Count > 0)
    {
      llstRet.Add(mcRxRingBuf.Dequeue());
    }

    return llstRet.ToArray();
  }


  public void Disconnect(cBotNetStreamPort lcPort)
  {
    for (int i = 0; i < cBnCfg.cBotNet_StreamSysPortsCnxCnt; i++)
    {
      if (mcConnection[i] == lcPort)
      {
        mcConnection[i] = null;
        return;
      }
    }
  }

  public abstract void vProcess();
  public abstract bool bProcessCmd(String lcStrCmd, String lcStrParam);
  public abstract void vPrintCmds();
};

