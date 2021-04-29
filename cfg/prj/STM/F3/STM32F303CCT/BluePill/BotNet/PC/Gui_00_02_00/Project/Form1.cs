using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.ComponentModel.Design;
using System.Data;
using System.Drawing;
using System.Linq;
using System.IO;
using System.Text;
using System.IO.Ports;
using System.Windows.Forms;
using System.Diagnostics;
using System.Threading;

namespace Project
{
  public partial class Form1 : Form
  {
    SerialPort ComPort = new SerialPort();

    Byte []mui8BufIn  = new Byte[32];
    int mui8BufInIdx = 0;

    Byte []mui8BufOut = new Byte[32];
    
    Stopwatch moTimestampAbs = new Stopwatch();
    Stopwatch moTimestampRel = new Stopwatch();

    int miFrameIndexAbs;
    int miFrameIndexRel;
    int miFrameCounter;
    
    Stopwatch moTimerDataIn = new Stopwatch();


    public class Diag_Client
    {
      public class Diag_Client_Mem
      {
        public Diag_Client_Mem()
        {
          ui32Index = 0;
          ui16Sort = 0;
          ui16Type = 0;
          ui32PageSize = 0;
          ui32StartAdress = 0;
          ui32Size = 0;
          szInfo = "";
        }

        public String toString()
        {
          String lszString;
          lszString  = (ui32Index + 1).ToString() + ". " + szInfo + "\r\n";
          lszString += "   " +
                       "Sort:" + ui16Sort.ToString()  + " 0x" + ui16Sort.ToString("X2") + " " + 
                       "Type: " + ui16Type.ToString() + " 0x" + ui16Type.ToString("X2") + "\r\n";
          lszString += "   " +
                       "PageSize:" + ui32PageSize.ToString()       + " 0x" + ui32PageSize.ToString("X2") + " " +
                       "Adr.: "    + ui32StartAdress.ToString()    + " 0x" + ui32StartAdress.ToString("X2") + " " +
                       "Size: "    + ui32Size.ToString()           + " 0x" + ui32Size.ToString("X2") + "\r\n";
          return lszString;

        }

        public UInt32 ui32Index;
        public UInt16 ui16Sort;
        public UInt16 ui16Type;
        public UInt32 ui32PageSize;
        public UInt32 ui32StartAdress;
        public UInt32 ui32Size;
        public String szInfo;
      }

      public Diag_Client()
      {
        szInterfacename = "";
        szDevicename = "";
        szInfotext = "";

        ui32WorkMemSize = 0;
        ui32WorkMemAdr = 0;
        ui32MemCnt = 0;

        i32BufDataMisoIdx = 0;
        bUpdateBuf = false;
        iCmdMode = 0;
        bReadBlockIsActive = false;
      }

      public String szInterfacename;
      public String szDevicename;
      public String szInfotext;

      public UInt32 ui32WorkMemSize;
      public UInt32 ui32WorkMemAdr;
      public UInt32 ui32MemCnt;

      public int iCmdMode; // 0 = normal, 1 = write, 2 = writeblock, 5=read, 6=readblock

      public UInt32 ui32LastCheckSum;
      public UInt32 ui32CheckSumData;

      public int    i32BufDataMisoIdx;
      public bool   bUpdateBuf;
      public Byte[] aubBufDataMiso;
      public Byte[] aubBufDataMosi;

      //ReadBlock
      public UInt32 ui32Read_StartAdr;
      public UInt32 ui32Read_Len;
      public UInt32 ui32Read_BlockSize;
      public UInt32 ui32Read_BlockReadCnt;
      public UInt32 ui32Read_BlockReadCntEnd;
      public bool bReadBlockIsActive;

      //WriteBlock
      public UInt32 ui32Write_StartAdr;
      public UInt32 ui32Write_Len;
      public UInt32 ui32Write_BlockSize;
      public UInt32 ui32Write_BlockWriteCnt;
      public UInt32 ui32Write_BlockWriteCntEnd;
      public bool bWriteBlockIsActive;

      public Diag_Client_Mem[] MemInfo;
    }

    public Diag_Client mcDiag_Client = new Diag_Client();

    internal delegate void SerialDataReceivedEventHandlerDelegate(
             object sender, SerialDataReceivedEventArgs e);

    internal delegate void SerialPinChangedEventHandlerDelegate(
             object sender, SerialPinChangedEventArgs e);
    private SerialPinChangedEventHandler SerialPinChangedEventHandler1;
    
    delegate void SetTextCallback(string text);
    delegate void DiagUpdateCallback(Diag_Client lcDiagData);
    delegate void DiagStartWriteCallback(Diag_Client lcDiagData);


    public Form1()
    {
      InitializeComponent();
      SerialPinChangedEventHandler1 = new SerialPinChangedEventHandler(PinChanged);
      ComPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(port_DataReceived_1);
    }


    private UInt32 ReadTextDezOrBin(String lszInput)
    {
      UInt32 lui32Size;

      try
      {
        if (lszInput.ToLower().Contains("x"))
        {
          lui32Size = Convert.ToUInt32(lszInput.Replace("x", ""), 16);
        }
        else
        {
          lui32Size = System.Convert.ToUInt32(lszInput);
        }
      }
      catch
      {
        lui32Size = 0;
      }

      return lui32Size;
    }


    private Byte[] baFileToByteArray(String lszFile, UInt32 lui32Offset)
    {
      Byte[] lui8Buf;

      try
      {
        lui8Buf = File.ReadAllBytes(lszFile).Skip((Int32)lui32Offset).ToArray();
      }
      catch
      {
        lui8Buf = new Byte[0];
      }
      return lui8Buf;
    }


    private int vFileFromByteArray(String lszFile, Byte[] lui8Buf)
    {
      try
      {
        File.WriteAllBytes(lszFile, lui8Buf);
        return 0;
      }
      catch
      {
        return 1;
      }
    }




    private void vBufShow(Byte[] mui8Buf)
    {
      UInt32 lui32Temp;
      UInt32 lui32Offset;

      lui32Offset = 0;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] + 
                  ((UInt32)mui8Buf[lui32Offset+1] << 8) + 
                  ((UInt32)mui8Buf[lui32Offset+2] << 16) + 
                  ((UInt32)mui8Buf[lui32Offset+3] << 24);
      TextBox_Command0_1.Text = lui32Temp.ToString("X8");

      lui32Offset+=4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      TextBox_Command0_2.Text = lui32Temp.ToString("X8");

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      TextBox_Command0_3.Text = lui32Temp.ToString("X8");

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      TextBox_Command0_4.Text = lui32Temp.ToString("X8");

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      TextBox_Command0_5.Text = lui32Temp.ToString("X8");

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      TextBox_Command0_6.Text = lui32Temp.ToString("X8");

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      TextBox_Command0_7.Text = lui32Temp.ToString("X8");

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      TextBox_Command0_8.Text = lui32Temp.ToString("X8");

      String lszString;
      lszString = "";
      for (Int32 lit = 0; lit < 32; lit++)
      {
        if (((lit % 4) == 0) && (lit > 0)) lszString += " ";
        lszString += mui8Buf[lit].ToString("X2") + ".";
      }

      lszString += "   ";
      
      for (Int32 lit = 0; lit < 32; lit++)
      {
        if (mui8Buf[lit] < ' ')
        {
          lszString += '_';
        }
        else
        {
          lszString += Convert.ToChar(mui8Buf[lit]);
        }
      }

      TextBox_Command0.Text = lszString;
    }

    private String vBuf2String(Byte[] mui8Buf, bool lbisInput)
    {
      String lszString;
      String lszString2;

      lszString = "";

      lszString2 = this.miFrameIndexAbs.ToString() + " ";
      while (lszString2.Length < 5) lszString2 = " " + lszString2;
      lszString += lszString2;

      lszString2 = this.miFrameIndexRel.ToString() + " ";
      while (lszString2.Length < 4) lszString2 = " " + lszString2;
      lszString += lszString2;

      lszString2 = this.moTimestampAbs.ElapsedMilliseconds.ToString("#,###,##0") + " ";
      while (lszString2.Length < 9) lszString2 = " " + lszString2;
      lszString += lszString2;

      lszString2 = this.moTimestampRel.ElapsedMilliseconds.ToString("#,###,##0") + " ";
      while (lszString2.Length < 7) lszString2 = " " + lszString2;
      lszString += lszString2;


      if (lbisInput) lszString += " <- ";
                else lszString += "-> ";

      for (Int32 lit = 0; lit < 32; lit++)
      {
        if (((lit % 4) == 0) && (lit > 0)) lszString += " ";
                
        if (((lit % 4) == 3) && (lit > 0))
          lszString += mui8Buf[lit].ToString("X2");
        else
          lszString += mui8Buf[lit].ToString("X2") + ".";
        
      }

      lszString += "   ";

      for (Int32 lit = 0; lit < 32; lit++)
      {
        if (mui8Buf[lit] < ' ')
        {
          lszString += '_';
        }
        else
        {
          lszString += Convert.ToChar(mui8Buf[lit]);
        }
      }
      return lszString;
    }


    private String vBuf2String2(Byte[] mui8Buf)
    {
      String lszString;
      String lszString2;
      int liIdx;
      int liLen;
      
      lszString = "";
      lszString2 = "";

      liIdx = 0;
      liLen = 0;

      for (liIdx = 0; liIdx < mui8Buf.Length; liIdx += 16)
      {
        liLen = mui8Buf.Length - liIdx;
        if (liLen > 16) liLen = 16;

        lszString2 = liIdx.ToString("X8") + ": ";
        for (Int32 lit = 0; lit < liLen; lit++)
        {
          lszString2 += mui8Buf[liIdx + lit].ToString("X2");
        }


        while (lszString2.Length < 45) lszString2 += " ";

        for (Int32 lit = 0; lit < liLen; lit++)
        {
          if (mui8Buf[liIdx + lit] < ' ')
          {
            lszString2 += '_';
          }
          else
          {
            lszString2 += Convert.ToChar(mui8Buf[liIdx + lit]);
          }
        }
        lszString += lszString2 + "\r\n";
      }

      return lszString;
    }

    private String vBufPrint(Byte[] mui8Buf, bool lbisInput, bool lbNewline, bool lbNewCmd)
    {
      String lszString;

      if (lbNewline) lszString = "\r\n";
                else lszString = "";

      if (lbNewCmd) lszString = "---------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n";

      lszString += vBuf2String(mui8Buf, lbisInput);

      return lszString;
    }


    private void vBufClear(Byte[] mui8Buf)
    {
      for (Int32 lit = 0; lit < 32; lit++)
      {
        mui8Buf[lit] = 0x55;
      }
    }

    private void vBufMemCopy(Byte[] mui8BufScr, Byte[] mui8BufDst, int liLen, int liOffsetScr,  int liOffsetDst)
    {
      for (Int32 lit = 0; (lit < 32) && (lit < liLen); lit++)
      {
        mui8BufDst[lit + liOffsetDst] = mui8BufScr[lit + liOffsetScr];
      }
    }

    private void vBufMemCopy(Byte[] mui8BufScr, Byte[] mui8BufDst, int liLen)
    {
      vBufMemCopy(mui8BufScr, mui8BufDst, liLen, 0, 0);
    }

    String szByteArraytoString(Byte[] mui8Buf)
    {
      String lszStr;
      int liIndex0;

      lszStr = System.Text.Encoding.Default.GetString(mui8Buf);

      liIndex0 = lszStr.IndexOf('\0');

      if (liIndex0 == 0) lszStr = "";
      else
        if (liIndex0 > 0)
        {
          lszStr = lszStr.Substring(0, liIndex0);
        }
      return lszStr;
    }


    Byte[] bUint32toByteArray(UInt32 lui32Value)
    {
      Byte[] mui8Buf = new Byte[4];

      mui8Buf[0] = (Byte)(lui32Value & 0xFF);
      mui8Buf[1] = (Byte)((lui32Value >> 8) & 0xFF);
      mui8Buf[2] = (Byte)((lui32Value >> 16) & 0xFF);
      mui8Buf[3] = (Byte)((lui32Value >> 24) & 0xFF);

      return mui8Buf;
    }


    private void vBufDecode(Byte[] mui8Buf, Diag_Client lcDiag_Client)
    {
      UInt32 lui32Temp;
      UInt32 lui32Offset;
      UInt32 lui32Idx;

      switch (mui8Buf[0])
      {
        case 0x00: // Interface Info
          switch (mui8Buf[1])
          {
            case 0x00:
              Byte[] lszInterfacename = new Byte[14];
              Byte[] lszDevicename = new Byte[16];

              vBufMemCopy(mui8Buf, lszInterfacename, 14, 2, 0);
              vBufMemCopy(mui8Buf, lszDevicename, 16, 16, 0);

              lcDiag_Client.szInterfacename = szByteArraytoString(lszInterfacename);
              lcDiag_Client.szDevicename = szByteArraytoString(lszDevicename);
              break;
            case 0x01:
              Byte[] lszInfotext = new Byte[30];

              vBufMemCopy(mui8Buf, lszInfotext, 30, 2, 0);

              lcDiag_Client.szInfotext = szByteArraytoString(lszInfotext);
              break;
          }
          break;

        case 0x01: // Interface Memory Info
          switch (mui8Buf[1])
          {
            case 0x00:
              lui32Offset = 12;
              lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                          ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                          ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                          ((UInt32)mui8Buf[lui32Offset + 3] << 24);
              lcDiag_Client.ui32MemCnt = lui32Temp;

              lui32Offset = 20;
              lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                          ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                          ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                          ((UInt32)mui8Buf[lui32Offset + 3] << 24);
              lcDiag_Client.ui32WorkMemAdr = lui32Temp;

              lui32Offset = 28;
              lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                          ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                          ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                          ((UInt32)mui8Buf[lui32Offset + 3] << 24);
              lcDiag_Client.ui32WorkMemSize = lui32Temp;

              lcDiag_Client.MemInfo = new Diag_Client.Diag_Client_Mem[lcDiag_Client.ui32MemCnt];
              for (int lit = 0; lit < lcDiag_Client.ui32MemCnt; lit++)
              {
                lcDiag_Client.MemInfo[lit] = new Diag_Client.Diag_Client_Mem();
              }
              break;

            case 0x01:
              switch (mui8Buf[3])
              {
                case 0x00:
                  lui32Offset = 4;
                  lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                              ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                              ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                              ((UInt32)mui8Buf[lui32Offset + 3] << 24);
                  lui32Idx = lui32Temp;

                  if (lui32Idx < lcDiag_Client.MemInfo.Length)
                  {
                    lcDiag_Client.MemInfo[lui32Idx].ui32Index = lui32Idx;

                    lui32Offset = 8;
                    lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                                ((UInt32)mui8Buf[lui32Offset + 1] << 8);
                    lcDiag_Client.MemInfo[lui32Idx].ui16Sort = (UInt16)lui32Temp;

                    lui32Offset = 10;
                    lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                                ((UInt32)mui8Buf[lui32Offset + 1] << 8);
                    lcDiag_Client.MemInfo[lui32Idx].ui16Type = (UInt16)lui32Temp;

                    lui32Offset = 12;
                    lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                                ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                                ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                                ((UInt32)mui8Buf[lui32Offset + 3] << 24);
                    lcDiag_Client.MemInfo[lui32Idx].ui32PageSize = lui32Temp;

                    lui32Offset = 20;
                    lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                                ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                                ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                                ((UInt32)mui8Buf[lui32Offset + 3] << 24);
                    lcDiag_Client.MemInfo[lui32Idx].ui32StartAdress = lui32Temp;

                    lui32Offset = 28;
                    lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                                ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                                ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                                ((UInt32)mui8Buf[lui32Offset + 3] << 24);
                    lcDiag_Client.MemInfo[lui32Idx].ui32Size = lui32Temp;
                  }
                  break;

                case 0x01:
                  Byte[] lszInfo = new Byte[24];

                  lui32Offset = 4;
                  lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                              ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                              ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                              ((UInt32)mui8Buf[lui32Offset + 3] << 24);
                  lui32Idx = lui32Temp;

                  if (lui32Idx < lcDiag_Client.MemInfo.Length)
                  {
                    vBufMemCopy(mui8Buf, lszInfo, 24, 8, 0);
                    lcDiag_Client.MemInfo[lui32Idx].szInfo = szByteArraytoString(lszInfo);
                  }
                  break;
              }
              break;
          }
          break;

        case 0x10: // Write
          switch (mui8Buf[2])
          {
            case 0x00: //Ack Nachricht

              this.mcDiag_Client.ui32Write_StartAdr = ReadTextDezOrBin(this.TextBox_CommandWrite_Adr.Text);
              this.mcDiag_Client.ui32Write_Len = ReadTextDezOrBin(this.TextBox_CommandWrite_Len.Text);
              this.mcDiag_Client.ui32Write_BlockSize = ReadTextDezOrBin(this.TextBox_CommandWriteBlock_BlSize.Text);

              lcDiag_Client.iCmdMode = 1;
              lcDiag_Client.ui32LastCheckSum = 0;
              lcDiag_Client.ui32CheckSumData = 0;

              if (this.mcDiag_Client.ui32Write_BlockSize > 0)
              {
                UInt32 lui32Len;
                if (this.mcDiag_Client.bWriteBlockIsActive == false)
                {
                  lcDiag_Client.iCmdMode = 2;
                  this.mcDiag_Client.ui32Write_BlockWriteCnt = 0;
                  this.mcDiag_Client.bWriteBlockIsActive = true;

                  lui32Len = this.mcDiag_Client.ui32Write_Len - this.mcDiag_Client.ui32Write_BlockWriteCnt;
                  if (lui32Len > this.mcDiag_Client.ui32Write_BlockSize)
                  {
                    lui32Len = this.mcDiag_Client.ui32Write_BlockSize;
                  }

                  this.mcDiag_Client.ui32Write_BlockWriteCntEnd = this.mcDiag_Client.ui32Write_BlockWriteCnt + lui32Len;
                }
                else
                {
                  lui32Len = this.mcDiag_Client.ui32Write_Len - this.mcDiag_Client.ui32Write_BlockWriteCnt;
                  if (lui32Len > this.mcDiag_Client.ui32Write_BlockSize)
                  {
                    lui32Len = this.mcDiag_Client.ui32Write_BlockSize;
                  }

                  this.mcDiag_Client.ui32Write_BlockWriteCntEnd = this.mcDiag_Client.ui32Write_BlockWriteCnt + lui32Len;
                }
              }

              if (lcDiag_Client.bWriteBlockIsActive)
              {
                UInt32 liu32Index;
                //Checksumme der gesendeten Daten
                for (liu32Index = lcDiag_Client.ui32Write_BlockWriteCnt; liu32Index < lcDiag_Client.ui32Write_BlockWriteCntEnd; liu32Index++)
                {
                  lcDiag_Client.ui32CheckSumData += lcDiag_Client.aubBufDataMosi[liu32Index];
                }
              }

              break;
            case 0x01: //Ack Response
              lui32Offset = 20;
              lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                          ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                          ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                          ((UInt32)mui8Buf[lui32Offset + 3] << 24);

              lcDiag_Client.ui32LastCheckSum = lui32Temp;

              lcDiag_Client.ui32CheckSumData++;

              if (lcDiag_Client.bWriteBlockIsActive)
              {
                if (lui32Temp != lcDiag_Client.ui32CheckSumData)
                {
                  this.BeginInvoke(new SetTextCallback(SetText), new object[] { "CheckSum NOK (" + lui32Temp.ToString() + "<->" + lcDiag_Client.ui32CheckSumData.ToString() + ")\r\n" });
                }
                else
                {
                  this.BeginInvoke(new SetTextCallback(SetText), new object[] { "CheckSum OK (" + lui32Temp.ToString() + ")\r\n" });
                }
              }


              if (lcDiag_Client.bWriteBlockIsActive)
              {
                if (lcDiag_Client.ui32Write_BlockWriteCnt < lcDiag_Client.ui32Write_Len)
                {
                  UInt32 lui32Len;

                  lui32Len = lcDiag_Client.ui32Write_Len - lcDiag_Client.ui32Write_BlockWriteCnt;
                  if (lui32Len > lcDiag_Client.ui32Write_BlockSize) lui32Len = lcDiag_Client.ui32Write_BlockSize;

                  lcDiag_Client.ui32Write_BlockWriteCntEnd = lcDiag_Client.ui32Write_BlockWriteCnt + lui32Len;

                  vBufClear(this.mui8BufOut);
                  this.mui8BufOut[0] = 0x10;
                  this.mui8BufOut[1] = 128;
                  vBufMemCopy(bUint32toByteArray(lcDiag_Client.ui32Write_StartAdr + lcDiag_Client.ui32Write_BlockWriteCnt), mui8BufOut, 4, 0, 8);
                  vBufMemCopy(bUint32toByteArray(lui32Len), mui8BufOut, 4, 0, 16);
                  vBufShow(this.mui8BufOut);
                  Send(false);
                }
                else
                {
                  lcDiag_Client.bReadBlockIsActive = false;
                  lcDiag_Client.bUpdateBuf = true;
                }
              }
              else
              {
                lcDiag_Client.bUpdateBuf = true;
              }
              break;
          }
          break;

        case 0x20: // Read
          if ((mui8Buf[1] == 0) && (mui8Buf[2] == 0)) //Ack Anfrage 1
          {
            if (mui8Buf[1] == 0) // ACK ?
            {
              this.mcDiag_Client.ui32Read_StartAdr = ReadTextDezOrBin(this.TextBox_CommandRead_Adr.Text);
              this.mcDiag_Client.ui32Read_Len = ReadTextDezOrBin(this.TextBox_CommandRead_Len.Text);
              this.mcDiag_Client.ui32Read_BlockSize = ReadTextDezOrBin(this.TextBox_CommandReadBlock_BlSize.Text);

              lcDiag_Client.iCmdMode = 5;
              lcDiag_Client.ui32LastCheckSum = 0;
              lcDiag_Client.ui32CheckSumData = 0;

              if (this.mcDiag_Client.ui32Read_BlockSize > 0)
              {
                UInt32 lui32Len = 0;
                lui32Len = this.mcDiag_Client.ui32Read_Len;

                if (this.mcDiag_Client.bReadBlockIsActive == false)
                {
                  if (lui32Len > this.mcDiag_Client.ui32Read_BlockSize)
                  {
                    this.mcDiag_Client.bReadBlockIsActive = true;
                    lcDiag_Client.iCmdMode = 6;

                    lcDiag_Client.aubBufDataMiso = new Byte[lcDiag_Client.ui32Read_Len];
                    lcDiag_Client.i32BufDataMisoIdx = 0;
                    this.mcDiag_Client.ui32Read_BlockReadCnt = 0;
                  }
                }

                if (lui32Len > this.mcDiag_Client.ui32Read_BlockSize)
                {
                  this.mcDiag_Client.ui32Read_BlockReadCntEnd = this.mcDiag_Client.ui32Read_BlockReadCnt + this.mcDiag_Client.ui32Read_BlockSize;
                  if (this.mcDiag_Client.ui32Read_BlockReadCntEnd > this.mcDiag_Client.ui32Read_Len)
                  {
                    this.mcDiag_Client.ui32Read_BlockReadCntEnd = this.mcDiag_Client.ui32Read_Len;
                  }
                }
              }

              lui32Offset = 16;
              lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                          ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                          ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                          ((UInt32)mui8Buf[lui32Offset + 3] << 24);

              if (!lcDiag_Client.bReadBlockIsActive)
              {
                lcDiag_Client.i32BufDataMisoIdx = 0;
                lcDiag_Client.aubBufDataMiso = new Byte[lui32Temp];
              }
            }
          }
          else
            if ((mui8Buf[1] == 0) && (mui8Buf[2] == 1)) //Ack Ende
            {
              lui32Offset = 20;
              lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                          ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                          ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                          ((UInt32)mui8Buf[lui32Offset + 3] << 24);

              lcDiag_Client.ui32LastCheckSum = lui32Temp;
              lcDiag_Client.ui32CheckSumData++;

              if (lui32Temp != lcDiag_Client.ui32CheckSumData)
              {
                this.BeginInvoke(new SetTextCallback(SetText), new object[] { "CheckSum NOK\r\n" });
              }
              else
              {
                this.BeginInvoke(new SetTextCallback(SetText), new object[] { "CheckSum OK\r\n" });
              }

              if (lcDiag_Client.bReadBlockIsActive)
              {
                if (lcDiag_Client.ui32Read_BlockReadCnt < lcDiag_Client.ui32Read_Len)
                {
                  UInt32 lui32Len;

                  lui32Len = lcDiag_Client.ui32Read_Len - lcDiag_Client.ui32Read_BlockReadCnt;
                  if (lui32Len > lcDiag_Client.ui32Read_BlockSize) lui32Len = lcDiag_Client.ui32Read_BlockSize;

                  lcDiag_Client.ui32Read_BlockReadCntEnd = lcDiag_Client.ui32Read_BlockReadCnt + lui32Len;

                  vBufClear(this.mui8BufOut);
                  this.mui8BufOut[0] = 32;
                  vBufMemCopy(bUint32toByteArray(lcDiag_Client.ui32Read_StartAdr + lcDiag_Client.ui32Read_BlockReadCnt), mui8BufOut, 4, 0, 8);
                  vBufMemCopy(bUint32toByteArray(lui32Len), mui8BufOut, 4, 0, 16);
                  vBufShow(this.mui8BufOut);
                  Send(false);
                }
                else
                {
                  lcDiag_Client.iCmdMode = 0;
                  lcDiag_Client.bReadBlockIsActive = false;
                  lcDiag_Client.bUpdateBuf = true;
                }
              }
              else
              {
                lcDiag_Client.iCmdMode = 0;
                lcDiag_Client.bUpdateBuf = true;
              }
            }
            else
              if ((mui8Buf[1] >= 128) && (mui8Buf[2] <= 255)) //Daten
              {
                int lui32DatatoCpy;
                int li32Index;

                lui32DatatoCpy = lcDiag_Client.aubBufDataMiso.Length - lcDiag_Client.i32BufDataMisoIdx;
                if (lcDiag_Client.bReadBlockIsActive)
                {
                  lui32DatatoCpy = (int)(lcDiag_Client.ui32Read_BlockReadCntEnd - lcDiag_Client.ui32Read_BlockReadCnt);
                }

                if (lui32DatatoCpy > 30) lui32DatatoCpy = 30;

                vBufMemCopy(mui8Buf, lcDiag_Client.aubBufDataMiso, lui32DatatoCpy, 2, lcDiag_Client.i32BufDataMisoIdx);

                //Checksumme
                for (li32Index = 2; li32Index < lui32DatatoCpy + 2; li32Index++)
                {
                  lcDiag_Client.ui32CheckSumData += mui8Buf[li32Index];
                }

                lcDiag_Client.i32BufDataMisoIdx += lui32DatatoCpy;

                if (lcDiag_Client.bReadBlockIsActive)
                {
                  lcDiag_Client.ui32Read_BlockReadCnt += (UInt32)lui32DatatoCpy;
                }
              }
          break;


        case 0x30: // Checksum
          switch (mui8Buf[2])
          {
            case 0x00: //Ack Nachricht
              lcDiag_Client.ui32LastCheckSum = 0;
              break;
            case 0x01: //Ack Response
              lui32Offset = 20;
              lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                          ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                          ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                          ((UInt32)mui8Buf[lui32Offset + 3] << 24);

              lcDiag_Client.ui32LastCheckSum = lui32Temp;
              break;
          }
          break;
      }


    /*
  

    case 0x40: // Call
      {
        uint8* lpui8Adr;
        uint32 lui32Param[4] = {0};
        Diag_vCall* lpvFunc;

        vDiag_MemCopy(&Diag_laui8MOSI_Buf[ 8], &lpui8Adr,  4);
        vDiag_MemCopy(&Diag_laui8MOSI_Buf[12], lui32Param, 4*4);


        vDiag_ClearBuf(Diag_laui8MISO_Buf);
        Diag_laui8MISO_Buf[1] = 0;
        vDiag_MemCopy(&lpui8Adr,  &Diag_laui8MISO_Buf[ 8], 4);
        vDiag_Send(Diag_laui8MISO_Buf);

        lpvFunc = (Diag_vCall*)lpui8Adr;
        lpvFunc(lui32Param[0], lui32Param[1], lui32Param[2], lui32Param[4]);
      }
      break;

    case 0x50: // Exit
      break;
      }
     */
    }


    private void vPorts()
    {
      string[] ArrayComPortsNames = null;
      int index = -1;
      string ComPortName = null;

      //Com Ports
      this.ComboBox_Ports.Items.Clear();
      ArrayComPortsNames = SerialPort.GetPortNames();
      do
      {
        index += 1;
        this.ComboBox_Ports.Items.Add(ArrayComPortsNames[index]);
      } while (!((ArrayComPortsNames[index] == ComPortName) ||
        (index == ArrayComPortsNames.GetUpperBound(0))));
      Array.Sort(ArrayComPortsNames);

      if (index == ArrayComPortsNames.GetUpperBound(0))
      {
        ComPortName = ArrayComPortsNames[0];
      }

      //get first item print in text
      ComboBox_Ports.Text = ArrayComPortsNames[0];

      //Baud Rate
      this.ComboBox_Baud.Items.Clear();
      this.ComboBox_Baud.Items.Add(300);
      this.ComboBox_Baud.Items.Add(600);
      this.ComboBox_Baud.Items.Add(1200);
      this.ComboBox_Baud.Items.Add(2400);
      this.ComboBox_Baud.Items.Add(9600);
      this.ComboBox_Baud.Items.Add(14400);
      this.ComboBox_Baud.Items.Add(19200);
      this.ComboBox_Baud.Items.Add(38400);
      this.ComboBox_Baud.Items.Add(57600);
      this.ComboBox_Baud.Items.Add(115200);
      this.ComboBox_Baud.Text = this.ComboBox_Baud.Items[this.ComboBox_Baud.FindStringExact("9600")].ToString();


      //Data Bits
      this.ComboBox_DataBits.Items.Clear();
      this.ComboBox_DataBits.Items.Add(7);
      this.ComboBox_DataBits.Items.Add(8);
      //get the first item print it in the text 
      this.ComboBox_DataBits.Text = this.ComboBox_DataBits.Items[1].ToString();

      //Stop Bits
      this.ComboBox_StopBits.Items.Clear();
      this.ComboBox_StopBits.Items.Add("One");
      this.ComboBox_StopBits.Items.Add("OnePointFive");
      this.ComboBox_StopBits.Items.Add("Two");
      //get the first item print in the text
      this.ComboBox_StopBits.Text = this.ComboBox_StopBits.Items[0].ToString();

      //Parity 
      this.ComboBox_Parity.Items.Clear();
      this.ComboBox_Parity.Items.Add("None");
      this.ComboBox_Parity.Items.Add("Even");
      this.ComboBox_Parity.Items.Add("Mark");
      this.ComboBox_Parity.Items.Add("Odd");
      this.ComboBox_Parity.Items.Add("Space");

      //get the first item print in the text

      this.ComboBox_Parity.Text = this.ComboBox_Parity.Items[0].ToString();

      //Handshake
      this.ComboBox_HandShaking.Items.Clear();
      this.ComboBox_HandShaking.Items.Add("None");
      this.ComboBox_HandShaking.Items.Add("XOnXOff");
      this.ComboBox_HandShaking.Items.Add("RequestToSend");
      this.ComboBox_HandShaking.Items.Add("RequestToSendXOnXOff");

      //get the first item print it in the text 
      this.ComboBox_HandShaking.Text = this.ComboBox_HandShaking.Items[0].ToString();
    }


    private void Btn_Ports_Click(object sender, EventArgs e)
    {
      vPorts();
    }


    private void ReadComportData()
    {
      Int32 liInputdata;

      if (!ComPort.IsOpen) return;

      if (moTimerDataIn.ElapsedMilliseconds > 50000)
      {
        //Reset Packet index
        mui8BufInIdx = 0;
        mcDiag_Client.iCmdMode = 0;
        this.BeginInvoke(new SetTextCallback(SetText), new object[] { "Reset packet index\r\n" });
      }

      do
      {
        try
        {
          liInputdata = ComPort.ReadByte();
        }
        catch
        {
          liInputdata = -1;
        }

        if (liInputdata >= 0)
        {
          moTimerDataIn.Reset();
          moTimerDataIn.Start();
          //moTimerDataIn.Start();
          mui8BufIn[mui8BufInIdx] = (Byte)liInputdata;
          mui8BufInIdx++;
          this.Lbl_InputCnt.Text = mui8BufInIdx.ToString();
          if (mui8BufInIdx > 31)
          {
            String lszString;

            mui8BufInIdx = 0;
            miFrameIndexAbs++;
            miFrameIndexRel++;

            lszString = mcDiag_Client.iCmdMode.ToString() + vBuf2String(mui8BufIn, true);
            if (lszString != String.Empty)
            {
              this.BeginInvoke(new SetTextCallback(SetText), new object[] { lszString });
            }

            vBufDecode(mui8BufIn, mcDiag_Client);
            if (mcDiag_Client.iCmdMode == 0)
            {
              this.BeginInvoke(new DiagUpdateCallback(UpdateDiagData), new object[] { mcDiag_Client });
            }
          }
        }
      }
      while ((liInputdata >= 0) && (ComPort.BytesToRead > 0));

      if ((mcDiag_Client.iCmdMode == 1) || (mcDiag_Client.iCmdMode == 2))
      {
        this.BeginInvoke(new DiagStartWriteCallback(StartWriteDiagData), new object[] { mcDiag_Client });
      }
    }


    private void port_DataReceived_1(object sender, SerialDataReceivedEventArgs e)
    {
      //ReadComportData();
    }

    private void SetText(string text)
    {
      if (Text_Data.Text == "") Text_Data.Text = text;
      else Text_Data.AppendText("\r\n" + text);
    }

    void StartWriteDiagData(Diag_Client lcDiagData)
    {
      int li32Len = 0;
      li32Len = (int)ReadTextDezOrBin(this.TextBox_CommandWrite_Len.Text);

      if (li32Len > lcDiagData.aubBufDataMosi.Length) li32Len = lcDiagData.aubBufDataMosi.Length;
      if (lcDiagData.bWriteBlockIsActive)
      {
        if (li32Len > (int)lcDiagData.ui32Write_BlockSize) li32Len = (int)lcDiagData.ui32Write_BlockSize;
      }

      if (lcDiagData.bWriteBlockIsActive)
      {
        Send_ByteArray(lcDiagData.aubBufDataMosi.Skip((int)lcDiagData.ui32Write_BlockWriteCnt).Take((Int32)li32Len).ToArray(), false, false, 1);
      }
      else
      {
        Send_ByteArray(lcDiagData.aubBufDataMosi.Take((Int32)li32Len).ToArray(), false, false, 1);
      }
      
      if (lcDiagData.bWriteBlockIsActive)
      {
        lcDiagData.ui32Write_BlockWriteCnt += (UInt32)li32Len;
        if (lcDiagData.ui32Write_BlockWriteCnt >= lcDiagData.ui32Write_Len)
        {
          lcDiagData.bWriteBlockIsActive = false;
        }
      }

      lcDiagData.iCmdMode = 0;
    }

    void UpdateDiagData(Diag_Client lcDiagData)
    {
      if (lcDiagData.iCmdMode == 0)
      {

        TextBox_DiagInfo0.Text = "Interface: " + lcDiagData.szInterfacename + "\r\n";
        TextBox_DiagInfo0.Text += "Device:    " + lcDiagData.szDevicename + "\r\n";
        TextBox_DiagInfo0.Text += "Info:      " + lcDiagData.szInfotext + "\r\n";

        TextBox_DiagInfo1.Text = "Work RAM:   Adr.: " + lcDiagData.ui32WorkMemAdr.ToString() + " 0x" + lcDiagData.ui32WorkMemAdr.ToString("X2") + " " +
                                              "Size: " + lcDiagData.ui32WorkMemSize.ToString() + " 0x" + lcDiagData.ui32WorkMemSize.ToString("X2") + "\r\n";

        vUpdate_TextBoxDecHex(TextBox_DiagWorkRAM_Adr_Dez, TextBox_DiagWorkRAM_Adr_Hex, lcDiagData.ui32WorkMemAdr);
        vUpdate_TextBoxDecHex(TextBox_DiagWorkRAM_Size_Dez, TextBox_DiagWorkRAM_Size_Hex, lcDiagData.ui32WorkMemSize);

        TextBox_DiagInfo1.Text += "Partitions: " + lcDiagData.ui32MemCnt.ToString() + "\r\n";

        for (int lit = 0; lit < lcDiagData.ui32MemCnt; lit++)
        {
          TextBox_DiagInfo1.Text += lcDiagData.MemInfo[lit].toString();
        }

        TextBox_DiagChecksum.Text = "Checksum: " + lcDiagData.ui32LastCheckSum.ToString() + " 0x" + lcDiagData.ui32LastCheckSum.ToString("X2");

        if ((lcDiagData.aubBufDataMiso != null) && (lcDiagData.bUpdateBuf))
        {
          String lszStr;

          lszStr = vBuf2String2(lcDiagData.aubBufDataMiso);
          TextBox_DiagRead.Text = lszStr;
          lcDiagData.bUpdateBuf = false;
        }
      }
    }

    internal void PinChanged(object sender, SerialPinChangedEventArgs e)
    {
      SerialPinChange SerialPinChange1 = 0;
      bool signalState = false;
      SerialPinChange1 = e.EventType;
      this.Label_CTSStatus.BackColor = Color.Green;
      this.Lable_DSRStatus.BackColor = Color.Green;
      this.Label_RIStatus.BackColor = Color.Green;
      this.Label_BreakStatus.BackColor = Color.Green;

      switch (SerialPinChange1)
      {
        case SerialPinChange.Break:
          this.Label_BreakStatus.BackColor = Color.Red;
          //MessageBox.Show("Break is Set");
          break;
        case SerialPinChange.CDChanged:
          signalState = ComPort.CtsHolding;
          //  MessageBox.Show("CD = " + signalState.ToString());
          break;
        case SerialPinChange.CtsChanged:
          signalState = ComPort.CDHolding;
          this.Label_CTSStatus.BackColor = Color.Red;
          //MessageBox.Show("CTS = " + signalState.ToString());
          break;
        case SerialPinChange.DsrChanged:
          signalState = ComPort.DsrHolding;
          this.Lable_DSRStatus.BackColor = Color.Red;
          // MessageBox.Show("DSR = " + signalState.ToString());
          break;
        case SerialPinChange.Ring:
          this.Label_RIStatus.BackColor = Color.Red;
          //MessageBox.Show("Ring Detected");
          break;
      }
    }


    private void Btn_Open_Click(object sender, EventArgs e)
    {
      try
      {
        if (!ComPort.IsOpen)
        {
          this.Btn_PortState.Text = "Close";
          ComPort.PortName = Convert.ToString(ComboBox_Ports.Text);
          ComPort.BaudRate = Convert.ToInt32(this.ComboBox_Baud.Text);
          ComPort.DataBits = Convert.ToInt16(this.ComboBox_DataBits.Text);
          ComPort.StopBits = (StopBits)Enum.Parse(typeof(StopBits), this.ComboBox_StopBits.Text);
          ComPort.Handshake = (Handshake)Enum.Parse(typeof(Handshake), this.ComboBox_HandShaking.Text);
          ComPort.Parity = (Parity)Enum.Parse(typeof(Parity), this.ComboBox_Parity.Text);
          ComPort.Open();
          Timer_Comport.Enabled = true; 
          moTimestampAbs.Reset();
          moTimestampAbs.Start();

          moTimestampRel.Reset();
          moTimestampRel.Start();

          miFrameIndexAbs = 0;
          miFrameIndexRel = 0;
          miFrameCounter  = 0;

          moTimerDataIn.Reset();
          moTimerDataIn.Start();
        }
        else if (ComPort.IsOpen)
        {
          this.Btn_PortState.Text = "Open";
          ComPort.Close();
          Timer_Comport.Enabled = false;

          moTimestampRel.Stop();
          moTimestampAbs.Stop();
          moTimerDataIn.Stop();

        }
      }
      catch (Exception err)
      {
        this.TextError.Text += err.ToString() + "\n";
      }
    }



    private void ComboBox_Ports_SelectedIndexChanged(object sender, EventArgs e)
    {
      ComPort.PortName = Convert.ToString(ComboBox_Ports.Text);
      if (ComPort.IsOpen)
      {
        this.Btn_PortState.Text = "Close";        
      }
      else
      {
        this.Btn_PortState.Text = "Open";
      }
    }

    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      try
      {
        if (ComPort.IsOpen)
        {
          ComPort.Close();
        }
      }
      finally
      {
      }
    }

    private void Send_ByteArray(Byte[] lbBuf, bool lbNewline, bool lbNewCmd, int liMode)
    {
      Byte[] lui8BufOut = new Byte[32];
      

      miFrameIndexAbs++;
      miFrameIndexRel++;

      try
      {
        if (liMode == 0)
        {
          vBufClear(lui8BufOut);
          vBufMemCopy(lbBuf, lui8BufOut, 32, 0, 0);

          this.BeginInvoke(new SetTextCallback(SetText), new object[] { vBufPrint(lui8BufOut, false, lbNewline, lbNewCmd) });

          ComPort.Write(lui8BufOut, 0, 32);

          moTimerDataIn.Reset();
          moTimerDataIn.Start();
        }
        else
        {
          Int32 li32Idx;
          li32Idx = 0;

          miFrameCounter = 0;

          while (li32Idx < lbBuf.Length)
          {
            vBufClear(lui8BufOut);

            lui8BufOut[0] = 0x10;
            lui8BufOut[1] = (byte)(miFrameCounter + 128);

            if ((lbBuf.Length - li32Idx) > 29)
            {
              vBufMemCopy(lbBuf, lui8BufOut, 30, li32Idx, 2);
            }
            else
            {
              lui8BufOut[1] = 0xFF;
              vBufMemCopy(lbBuf, lui8BufOut, lbBuf.Length - li32Idx, li32Idx, 2);
            }

            this.BeginInvoke(new SetTextCallback(SetText), new object[] { vBufPrint(lui8BufOut, false, lbNewline, lbNewCmd) });

            ComPort.Write(lui8BufOut, 0, 32);

            miFrameCounter++;
            miFrameCounter&= 0x7F;

            moTimerDataIn.Reset();
            moTimerDataIn.Start();

            li32Idx += 30;
          }
        }
      }
      catch (Exception err)
      {
        this.TextError.Text += err.ToString() + "\n";
      }
    }

    private void Send(bool lbNewline)
    {      
      moTimestampRel.Reset();
      moTimestampRel.Start();

      moTimerDataIn.Reset();
      moTimerDataIn.Start();

      miFrameIndexRel = 0;

      mui8BufInIdx = 0;

      Send_ByteArray(this.mui8BufOut, true, true, 0);

      if (lbNewline)
      {
        this.BeginInvoke(new SetTextCallback(SetText), new object[] { "\r\n" });
      }
    }


    private void Button_Send_Click(object sender, EventArgs e)
    {
      Send(true);
    }



    private void Form1_Load(object sender, EventArgs e)
    {
      vBufClear(this.mui8BufOut);
      vBufClear(this.mui8BufIn);
      vPorts();
      vUpdateBufShowOut();
    }

    private void vUpdateBufShowOut()
    {
      switch (this.TabControl_Command.SelectedIndex)
      {
        case 0: //Info1
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 0;
          vBufShow(this.mui8BufOut);
          break;
        case 1: //Info2
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 1;
          vBufShow(this.mui8BufOut);
          break;
        case 2: //Write
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 16;
          this.mui8BufOut[1] = 128;

          if (ReadTextDezOrBin(this.TextBox_CommandWriteBlock_BlSize.Text) == 0)
          {
            vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandWrite_Adr.Text)), mui8BufOut, 4, 0, 8);
            vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandWrite_Len.Text)), mui8BufOut, 4, 0, 16);
          }
          else
          {
            int li32Len = 0;
            li32Len = (int)ReadTextDezOrBin(this.TextBox_CommandWrite_Len.Text);
            if (li32Len > (int)ReadTextDezOrBin(this.TextBox_CommandWriteBlock_BlSize.Text))
            {
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandWrite_Adr.Text)), mui8BufOut, 4, 0, 8);
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandWriteBlock_BlSize.Text)), mui8BufOut, 4, 0, 16);
            }
            else
            {
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandWrite_Adr.Text)), mui8BufOut, 4, 0, 8);
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandWrite_Len.Text)), mui8BufOut, 4, 0, 16);
            }
          }
          break;
        case 3: //Read
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 32;
          
          if (ReadTextDezOrBin(this.TextBox_CommandReadBlock_BlSize.Text) == 0)
          {
            vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandRead_Adr.Text)), mui8BufOut, 4, 0, 8);
            vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandRead_Len.Text)), mui8BufOut, 4, 0, 16);
          }
          else
          {
            int li32Len = 0;
            li32Len = (int)ReadTextDezOrBin(this.TextBox_CommandRead_Len.Text);
            if (li32Len > (int)ReadTextDezOrBin(this.TextBox_CommandReadBlock_BlSize.Text))
            {
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandRead_Adr.Text)), mui8BufOut, 4, 0, 8);
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandReadBlock_BlSize.Text)), mui8BufOut, 4, 0, 16);
            }
            else
            {
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandRead_Adr.Text)), mui8BufOut, 4, 0, 8);
              vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandRead_Len.Text)), mui8BufOut, 4, 0, 16);
            }
          }

          
          break;
        case 4: //Checksumme
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 48;
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandChk_Adr.Text)), mui8BufOut, 4, 0, 8);
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandChk_Len.Text)), mui8BufOut, 4, 0, 16);
          break;
        case 5: //Execute
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 64;
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandExecute_Adr.Text)), mui8BufOut, 4, 0, 8);
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandExecute_Param1.Text)), mui8BufOut, 4, 0, 12);
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandExecute_Param2.Text)), mui8BufOut, 4, 0, 16);
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandExecute_Param3.Text)), mui8BufOut, 4, 0, 20);
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandExecute_Param4.Text)), mui8BufOut, 4, 0, 24);
          break;
        case 6: //Exceute
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 65;
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandStart_Adr.Text)), mui8BufOut, 4, 0, 8);
          break;
        case 7: //Exit
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 128;
          break;
        case 8: //Enter
          vBufClear(this.mui8BufOut);
          this.mui8BufOut[0] = 129;
          vBufMemCopy(bUint32toByteArray(0x01020304),                                           mui8BufOut, 4, 0, 4);
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandEnter_ID1.Text)), mui8BufOut, 4, 0, 8);
          vBufMemCopy(bUint32toByteArray(ReadTextDezOrBin(this.TextBox_CommandEnter_ID2.Text)), mui8BufOut, 4, 0,12);
          break;
      }
    }


    private void TabControl_Command_SelectedIndexChanged(object sender, EventArgs e)
    {
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void vUpdate_TextBoxDecHex(TextBox lcDec, TextBox lcHex, UInt32 lui32Value)
    {
      lcDec.Text = lui32Value.ToString();
      lcHex.Text = "0x" + lui32Value.ToString("X2");
    }


    private void TextBox_CommandWrite_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandWrite_Len_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandRead_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandRead_Len_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandChk_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandChk_Len_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandExecute_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandExecute_Param1_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandExecute_Param2_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandExecute_Param3_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandExecute_Param4_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandWrite_FileOffset_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);

      mcDiag_Client.aubBufDataMosi = baFileToByteArray(this.TextBox_CommandWrite_FileName.Text, ReadTextDezOrBin(this.TextBox_CommandWrite_FileOffset.Text));
      this.TextBox_CommandWrite_Len.Text = mcDiag_Client.aubBufDataMosi.Length.ToString();
    }

    private void Button_Clear_TextData_Click(object sender, EventArgs e)
    {
      this.Text_Data.Text = "";
    }


    private void TextBox_CommandWrite_FileName_DragDrop(object sender, DragEventArgs e)
    {
      //Vom Datei Explorer
      if (e.Data.GetDataPresent(DataFormats.FileDrop))
      {
        String[] lszDroppedFiles = (string[])e.Data.GetData(DataFormats.FileDrop);
        ((TextBox)sender).Text = lszDroppedFiles[0];

        mcDiag_Client.aubBufDataMosi = baFileToByteArray(this.TextBox_CommandWrite_FileName.Text, ReadTextDezOrBin(this.TextBox_CommandWrite_FileOffset.Text));
        this.TextBox_CommandWrite_Len.Text = mcDiag_Client.aubBufDataMosi.Length.ToString();
        TextBox_DiagWrite.Text = vBuf2String2(mcDiag_Client.aubBufDataMosi);
      }
    }

    private void TextBox_CommandWrite_FileName_DragEnter(object sender, DragEventArgs e)
    {
      if (e.Data.GetDataPresent(DataFormats.FileDrop))
      {
        e.Effect = DragDropEffects.Copy;
      }
      else
      {
        e.Effect = DragDropEffects.None;
      }
    }

    private void Timer_Comport_Tick(object sender, EventArgs e)
    {
      if (!ComPort.IsOpen) return;

      if (ComPort.BytesToRead > 0)
      {
        ReadComportData();
      }
    }

    private void TextBox_CommandReadBlock_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
    }

    private void TextBox_CommandReadBlock_Len_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
    }


    private void TextBox_CommandReadBlock_BlSize_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }


    private void TextBox_CommandReadBlock_File_DragDrop(object sender, DragEventArgs e)
    {
      //Vom Datei Explorer
      if (e.Data.GetDataPresent(DataFormats.FileDrop))
      {
        String[] lszDroppedFiles = (string[])e.Data.GetData(DataFormats.FileDrop);
        ((TextBox)sender).Text = lszDroppedFiles[0];
      }
    }

    private void TextBox_CommandReadBlock_File_DragEnter(object sender, DragEventArgs e)
    {
      if (e.Data.GetDataPresent(DataFormats.FileDrop))
      {
        e.Effect = DragDropEffects.Copy;
      }
      else
      {
        e.Effect = DragDropEffects.None;
      }
    }

    private void Button_CommandReadBlock_FileSave_Click(object sender, EventArgs e)
    {
      if (this.TextBox_CommandReadBlock_File.Text != "")
      {
        if (this.mcDiag_Client.aubBufDataMiso.Length > 0)
        {
          vFileFromByteArray(this.TextBox_CommandReadBlock_File.Text, this.mcDiag_Client.aubBufDataMiso);
        }
      }
    }

    private void TextBox_CommandWriteBlock_BlSize_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandStart_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }


    private void TextBox_CommandEnter_ID_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }

    private void TextBox_CommandEnter_ID2_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      vUpdateBufShowOut();
      vBufShow(this.mui8BufOut);
    }
  }
}
