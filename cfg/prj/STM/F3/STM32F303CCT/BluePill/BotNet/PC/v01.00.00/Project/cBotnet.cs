using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

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

public class cBotNet
{
  public cBotNetAdress          mcAdr;

  // --- Links
  public cBotNet_DownLink mcDownLinks;
  public cBotNetStreamSystem mcStreamSys;
  //cBotNet_MsgSysProcess *mcpMsgProcess;

  Thread           mcProcessThread;

  public cBotNet(uint16 lu16Adr, cBotNet_DownLink lcDownLink/*, cBotNet_MsgSysProcess*/)
  {
    /*mcpMsgProcess   = lcpMsgProcess;*/
    mcAdr = new cBotNetAdress();

    mcAdr.Set(lu16Adr);

    mcDownLinks = lcDownLink;
    mcStreamSys = new cBotNetStreamSystem(this);

    mcProcessThread = new Thread(delegate()
    {
      this.vProcess();
    });
    mcProcessThread.Start();
  }

  ~cBotNet()
  {
    mcProcessThread.Abort();
  }

  public void vDecodeMsgType(cBotNetMsg_Base lcMsg)
  {
    switch (lcMsg.mFrame.enMsgType)
    {
      case cBotNetMsgFrame.tenBotNetMsgType.enMessage:
      {
        cBotNetMsg_MsgProt lcMsgProt = new cBotNetMsg_MsgProt(lcMsg);

        if (lcMsgProt.mu16Idx > 63)
        {
          //mcpMsgProcess->vMsg(lcMsg, this);
        }
        else
        {
        }
      }
      break;

      case cBotNetMsgFrame.tenBotNetMsgType.enStream:
        mcStreamSys.put(lcMsg);
        break;

      case cBotNetMsgFrame.tenBotNetMsgType.enPacket:
        break;

      case cBotNetMsgFrame.tenBotNetMsgType.enMultiMessage:
        break;
    }
  }

  public void vDecode(cBotNetMsg_Base lcMsg)
  {
    //lcMsg.vDecode(mcAdr.Get(), mcAdr.Get());
    lcMsg.vDecode();

    if (mcAdr.isMe(lcMsg.mFrame.cDAdr.Get()))
    {
      /* Nachricht für mich */
      vDecodeMsgType(lcMsg);
    }
    else
    {
      /* Weiterleiten */
      mcDownLinks.put(lcMsg);
    }
  }

  public void vForwardTxMsg(cBotNetMsg_Base lcMsg)
  {
    //lcMsg.vDecode(mcAdr.Get(), mcAdr.Get());
    lcMsg.vDecode();

    if (mcAdr.isMe(lcMsg.mFrame.cDAdr.Get()))
    {
      /* Nachricht für mich */
      vDecodeMsgType(lcMsg);
    }
    else
    {
      /* Weiterleiten */
      mcDownLinks.put(lcMsg);
    }
  }


  public void vProcess()
  {
    cBotNetMsg_Base lcMsg;


    // Link - Check for Input Data
    while (true)
    {
      if (mcDownLinks.mcLinkRx.Count > 0)
      {
        // Process input data
        lcMsg = mcDownLinks.mcLinkRx.Dequeue();
        vDecode(lcMsg);
      }
      else
      {
        break;
      }
    }

    
    mcStreamSys.vProcess();
  }
};
