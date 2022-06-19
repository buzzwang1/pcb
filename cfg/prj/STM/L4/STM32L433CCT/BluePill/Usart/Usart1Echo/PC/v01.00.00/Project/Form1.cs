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

namespace Project
{
  public partial class Form1 : Form
  {
    SerialPort ComPort = new SerialPort();

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
    
    delegate void IAP_SetTextCallback(string text);
    delegate void IAP_DiagUpdateCallback(Diag_Client lcDiagData);
    delegate void IAP_DiagStartWriteCallback(Diag_Client lcDiagData);
    delegate void IAP_vBufShow_Callback(Byte[] mui8Buf);


    public Form1()
    {
      InitializeComponent();

      ComPort_vInit();

      SerialPinChangedEventHandler1 = new SerialPinChangedEventHandler(PinChanged);
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
          ComPort.WriteBufferSize = 128;
          ComPort.Open();
        }
        else if (ComPort.IsOpen)
        {
          this.Btn_PortState.Text = "Open";
          ComPort.Close();
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
        ComPort_vDeInit();
        if (ComPort.IsOpen)
        {
          ComPort.Close();
        }
      }
      finally
      {
      }
    }

    private void Form1_Load(object sender, EventArgs e)
    {
      vPorts();
    }

    public void MAIN_vSetInvokeTextBox(TextBox lcTextBox, string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<TextBox, string>(MAIN_vSetInvokeTextBox), new object[] { lcTextBox, lszText });
        return;
      }
      lcTextBox.Text = lszText;
    }


    private void vUpdate_TextBoxDecHex(TextBox lcDec, TextBox lcHex, UInt32 lui32Value)
    {
      lcDec.Text = lui32Value.ToString();
      lcHex.Text = "0x" + lui32Value.ToString("X2");
    }


    /* ----------------------------------- General ---------------------------------------------------- */

    String szByteArraytoString(u8[] mui8Buf)
    {
      String lszStr;
      int liIndex0;

      lszStr = System.Text.Encoding.Default.GetString(mui8Buf);

      // '/0'-entfernen
      liIndex0 = lszStr.IndexOf('\0');
      if (liIndex0 == 0) lszStr = "";
      else
        if (liIndex0 > 0)
        {
          lszStr = lszStr.Substring(0, liIndex0);
        }
      return lszStr;
    }


    u8[] bU32toByteArray(u32 lui32Value)
    {
      u8[] mui8Buf = new u8[4];

      mui8Buf[0] = (Byte)(lui32Value & 0xFF);
      mui8Buf[1] = (Byte)((lui32Value >> 8) & 0xFF);
      mui8Buf[2] = (Byte)((lui32Value >> 16) & 0xFF);
      mui8Buf[3] = (Byte)((lui32Value >> 24) & 0xFF);

      return mui8Buf;
    }

    u32 ReadTextDezOrBin(String lszInput)
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


    u8[] baFileToByteArray(String lszFile, UInt32 lui32Offset)
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


    int vFileFromByteArray(String lszFile, Byte[] lui8Buf)
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


    /* ----------------------------------- ComPort ---------------------------------------------------- */

    public class cComPort_Entry
    {
      public long mu32TimeStamp;
      public u8[] mData;

      public cComPort_Entry(long lu32TimeStamp, u8[] lData)
      {
        mu32TimeStamp = lu32TimeStamp;
        mData = lData;
      }
    }

    Queue<cComPort_Entry> ComPort_mcQueueRx;
    Queue<cComPort_Entry> ComPort_mcQueueTx;
    Thread ComPort_mcThread;
    Stopwatch ComPort_mcTimestamp;

    void ComPort_vInit()
    {
      ComPort_mcQueueRx = new Queue<cComPort_Entry>();
      ComPort_mcQueueTx = new Queue<cComPort_Entry>();
      ComPort_mcTimestamp = new Stopwatch();

      ComPort_mcTimestamp.Reset();
      ComPort_mcTimestamp.Start();

      ComPort_mcThread = new Thread(delegate()
      {
        ComPort_vProcess(ComPort_mcQueueRx, ComPort_mcQueueTx);
      });
      ComPort_mcThread.Start();
    }

    void ComPort_vDeInit()
    {
      ComPort_mcThread.Abort();
    }

    void ComPort_vPutTx(u8[] lData)
    {
      ComPort_mcQueueTx.Enqueue(new cComPort_Entry(ComPort_mcTimestamp.ElapsedMilliseconds, lData));
    }

    public void ComPort_vAppendTextBox(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(ComPort_vAppendTextBox), new object[] { lszText });
        return;
      }

      Com_Text_ConIO.Text = lszText;
    }

    public void ComPort_vProcess(Queue<cComPort_Entry> ComPort_lcQueueRx, Queue<cComPort_Entry> ComPort_lcQueueTx)
    {
      bool lbRepeat;
      long lRxTimeStamp = 0;
      List<u8> llstRxData = new List<byte>();

      while (true)
      {
        if (ComPort.IsOpen)
        {
          do
          {
            lbRepeat = false;

            // Data to get
            int liDataToRead = ComPort.BytesToRead;
            for (int lit = 0; lit < liDataToRead; lit++)
            {
              if (llstRxData.Count == 0)
              {
                lRxTimeStamp = ComPort_mcTimestamp.ElapsedMilliseconds;
              }

              u8 lu8Data = (u8)ComPort.ReadByte();
              llstRxData.Add(lu8Data);
            }

            if (llstRxData.Count > 0)
            {
              byte[] lbyTemp = llstRxData.ToArray();
              lbyTemp[0] += 2;

              ComPort_vAppendTextBox(System.Text.Encoding.Default.GetString(lbyTemp));
              if (ComPort.IsOpen) ComPort.Write(lbyTemp, 0, 1);
              llstRxData.Clear();
            }
          } while (ComPort.IsOpen);
        }
        else
        {
          Thread.Sleep(500);
        }
      }
    }
   }
}
