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

class BotNetCmdPort_Consts
{
  public const int BN_CMD_PORT_HISTORY_LINES = 4;

  public const int BN_CMD_PORT_KEY_MAXLINE   = 32;
  public const int BN_CMD_PORT_KEY_BACKSPACE = 8;
  public const int BN_CMD_PORT_KEY_RETURN    = 0x0D;
  public const int BN_CMD_PORT_KEY_ESC       = 0x1B;

// VT100 => ESC = Start von Sonderzeichen
//#define BN_CMD_PORT_KEY_ARROW       CLI_KEY_ESC

//#define BN_CMD_PORT_KEY_ARROW_UP    256 + 'A'
//#define BN_CMD_PORT_KEY_ARROW_DOWN  256 + 'B'
//#define BN_CMD_PORT_KEY_ARROW_RIGHT 256 + 'C'
//#define BN_CMD_PORT_KEY_ARROW_LEFT  256 + 'D'
  public const int BN_CMD_PORT_KEY_ARROW       = 0xE0;
  public const int BN_CMD_PORT_KEY_ARROW_UP    = 256 + 72;
  public const int BN_CMD_PORT_KEY_ARROW_DOWN  = 256 + 80;
  public const int BN_CMD_PORT_KEY_ARROW_LEFT  = 256 + 75;
  public const int BN_CMD_PORT_KEY_ARROW_RIGHT = 256 + 77;

  public const String BN_CMD_PORT_PROMPT_INIT = ": ";
}


public class cBotNetStreamPort_BotNetCmdPort: cBotNetStreamPort
{
  public cBotNetStreamPort_CmdPrinter mcMyPrinter;

  public cBotNetStreamPort_BotNetCmdPort()
  {
    mcMyPrinter = new cBotNetStreamPort_CmdPrinter();
  }

  public override void vProcess()
  {
    if (mcRxRingBuf.Count > 0)
    {
      u8[] lu8Data = mcRxRingBuf.ToArray();
      mcRxRingBuf.Clear();
      mcMyPrinter.vPrint(System.Text.Encoding.Default.GetString(lu8Data));
    }
  }

  public override bool bProcessCmd(String lcStrCmd, String lcStrParam)
  {
    return false;
  }

  public override void vPrintCmds()
  {
  }
};



