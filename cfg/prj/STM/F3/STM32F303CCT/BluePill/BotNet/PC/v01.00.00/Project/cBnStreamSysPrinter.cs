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

public class cBotNetStreamPort_CmdPrinter
{
  public String mpui8TextBufOut;

  public cBotNetStreamPort_CmdPrinter()
  {
    mpui8TextBufOut = "";
  }

  void vLimit()
  {
    if (mpui8TextBufOut.Length > 10000)
    {
      mpui8TextBufOut = mpui8TextBufOut.Remove(0,1000);
    }
  }

  public void vPrint(String lszStr)
  {
    mpui8TextBufOut += lszStr;
    vLimit();
  }

  public void vPutChr(char li8Char)
  {
    vPrint(li8Char.ToString());
  }

  public void vPrintLn(String lszStr)
  {
    vPrint(lszStr + "\n");
  }
};

