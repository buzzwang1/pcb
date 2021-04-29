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

class cBnCfg
{
  public const int cBotNet_MsgSize = 64;

  public const int cBotNet_UpLinkComBufSize   = (cBotNet_MsgSize * 16);
  public const int cBotNet_DownLinkComBufSize = (cBotNet_MsgSize * 4);
  public const int cBotNet_DownLinkCnt = 14;

  public const int cBotNet_StreamSysPortsCnt = 16;
  public const int cBotNet_StreamSysPortsCnxCnt = 4;

  public const int cBotNet_StreamSysComBufSize = (cBotNet_MsgSize * 2);
  public const int cBotNet_StreamSysPortRingBufSize = (cBotNet_MsgSize);
}

/* Forward Deklaration */
//class cBotNet;
