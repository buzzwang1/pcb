using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

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


public class cBotNet_LinkBase
{
  public Queue<cBotNetMsg_Base> mcLinkRx;
  public Queue<cBotNetMsg_Base> mcLinkTx;

  public cBotNet_LinkBase()
  {
    mcLinkRx = new Queue<cBotNetMsg_Base>();
    mcLinkTx = new Queue<cBotNetMsg_Base>();
  }

  public void put(cBotNetMsg_Base lcMsg)
  {
    mcLinkTx.Enqueue(lcMsg);
  }

  public cBotNetMsg_Base get()
  {
    return mcLinkRx.Dequeue();
  }
};

public class cBotNet_DownLink : cBotNet_LinkBase
{
  public cBotNet_DownLink()
    : base()
  { }
};



