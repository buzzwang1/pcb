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

    internal delegate void SerialDataReceivedEventHandlerDelegate(
             object sender, SerialDataReceivedEventArgs e);

    internal delegate void SerialPinChangedEventHandlerDelegate(
             object sender, SerialPinChangedEventArgs e);
    private SerialPinChangedEventHandler SerialPinChangedEventHandler1;
    

    public Form1()
    {
      InitializeComponent();

      ComPort_vInit();
      ComTrace_vInit();

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
      this.ComboBox_Baud.Text = this.ComboBox_Baud.Items[this.ComboBox_Baud.FindStringExact("115200")].ToString();


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
        ComTrace_vDeInit();
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

      public cComPort_Entry()
      {
        mu32TimeStamp = 0;
        mData = null;
      }

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

      Com_Text_ConIO.Text += lszText;
    }

    public static string ByteArrayToString(byte[] ba)
    {
      StringBuilder hex = new StringBuilder(ba.Length * 2);
      foreach (byte b in ba)
        hex.AppendFormat("{0:x2} ", b);
      return hex.ToString().ToUpper();
    }

    public void ComPort_vProcess(Queue<cComPort_Entry> ComPort_lcQueueRx, Queue<cComPort_Entry> ComPort_lcQueueTx)
    {
      bool lbRepeat;
      long lRxTimeStamp = 0;
      List<u8> llstRxData = new List<byte>();
      cComPort_Entry lComTx = new cComPort_Entry();
      long lTxAckTimeStamp = 0;
      bool lbAnswer = false;
      cComPort_Entry lComRx = new cComPort_Entry();


      while (true)
      {
        if (ComPort.IsOpen)
        {
          do
          {
            lbRepeat = true;
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

              if (lu8Data == 0xED)
              {
                lbAnswer = true;

                lComRx.mu32TimeStamp = lRxTimeStamp;
                lComRx.mData = llstRxData.ToArray();

                //ComPort_mcQueueRx.Enqueue(lComRx);
                ComPort_vAppendTextBox("<= " + ByteArrayToString(lComRx.mData) + "\r\n");
                llstRxData.Clear();
              }
              
            }
            

            // Data to Send
            //if (ComPort_lcQueueTx.Count > 0)
            /*if (lbAnswer)
            {
              lbAnswer =false;
              
              lComTx = lComRx;
              
              if ((lComTx.mData[0] == 0xFC) &&
                  (lComTx.mData[1] == 0xCF) &&
                  (lComTx.mData[2] == 0x02) &&
                  (lComTx.mData[3] == 0x01) &&
                  (lComTx.mData[4] == 0x00) &&
                  (lComTx.mData[5] == 0x00) &&
                  (lComTx.mData[6] == 0x00) &&
                  (lComTx.mData[7] == 0x00) &&
                  (lComTx.mData[8] == 0x03) &&
                  (lComTx.mData[9] == 0xED))
              {
                lComTx.mData[0] = 0xFC;
                lComTx.mData[1] = 0xCF;
                lComTx.mData[2] = 0x02;
                lComTx.mData[3] = 0xAA;
                lComTx.mData[4] = 0x41;
                lComTx.mData[5] = 0x16;
                lComTx.mData[6] = 0x13;
                lComTx.mData[7] = 0x01;
                lComTx.mData[8] = (u8)(lComTx.mData[2] + lComTx.mData[3] + lComTx.mData[4] + lComTx.mData[5] + lComTx.mData[6] + lComTx.mData[7]);
                lComTx.mData[9] = 0xED;

                ComPort.Write(lComTx.mData, 0, lComTx.mData.Length);
                lTxAckTimeStamp = ComPort_mcTimestamp.ElapsedMilliseconds;
                //ComPort_vAppendTextBox("=> " + ByteArrayToString(lComTx.mData) + "\r\n");
              }
            }*/

          } while (lbRepeat);
        }
        else
        {
          Thread.Sleep(500);
        }
      }
    }



    private void Btn_Test1_Click(object sender, EventArgs e)
    {
      u8 []lu8TxData = {0xFC, 0xCF, 0x02, 0x01, 0x00 ,0x00, 0x00, 0x00, 0x03, 0xED};
      
      for (int id = 0; id < 256; id++)
      {
        lu8TxData[2] = (u8)id;
        lu8TxData[8] = (u8)(id + 1);
        u8 []lu8TxData2 = new u8[10];
        lu8TxData.CopyTo(lu8TxData2, 0);
        ComPort_vPutTx(lu8TxData2);
      }
    }

    private void Btn_Test2_Click(object sender, EventArgs e)
    {
      /*u8[] lu8TxData = { 0xAA, 0xAA, 0x02, 0x03, 14, 0x19};

      for (int id = 0; id < 256; id++)
      {
        lu8TxData[2] = (u8)id;
        lu8TxData[5] = (u8)(lu8TxData[2] + lu8TxData[3] + lu8TxData[4]);
        u8[] lu8TxData2 = new u8[6];
        lu8TxData.CopyTo(lu8TxData2, 0);
        ComPort_vPutTx(lu8TxData2);
      }*/

      u8[] lu8TxData = { 0xFC, 0xCF, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x03, 0xED };

      for (int preamble = 0; preamble < 256; preamble++)
      {
        lu8TxData[0] = (u8)preamble;
        lu8TxData[1] = (u8)((preamble >> 4) + ((preamble & 15) << 4));
        for (int id = 0; id < 256; id++)
        {
          lu8TxData[2] = (u8)id;
          lu8TxData[8] = (u8)(id + 1);
          u8[] lu8TxData2 = new u8[10];
          lu8TxData.CopyTo(lu8TxData2, 0);
          ComPort_vPutTx(lu8TxData2);
        }

        lu8TxData[0] = (u8)preamble;
        lu8TxData[1] = (u8)(preamble);
        for (int id = 0; id < 256; id++)
        {
          lu8TxData[2] = (u8)id;
          lu8TxData[8] = (u8)(id + 1);
          u8[] lu8TxData2 = new u8[10];
          lu8TxData.CopyTo(lu8TxData2, 0);
          ComPort_vPutTx(lu8TxData2);
        }
      }
      
    }

    private static byte[] StringToByteArray(String hex)
    {
      int NumberChars = hex.Length;
      byte[] bytes = new byte[NumberChars / 2];
      for (int i = 0; i < NumberChars; i += 2)
        bytes[i / 2] = Convert.ToByte(hex.Substring(i, 2), 16);
      return bytes;
    }


    private void Btn_Test3_Click(object sender, EventArgs e)
    {
      String lszString = this.textBox1.Text;
      lszString = lszString.Replace(".", "");
      lszString = lszString.Replace(" ", "");
      ComPort_vPutTx(StringToByteArray(lszString));
    }


    /* ----------------------------------- ComTrace ---------------------------------------------------- */

    public class cComTrace_Entry
    {
      public long mu32TimeStamp;
      public u8[] mData;

      public cComTrace_Entry(long lu32TimeStamp, u8[] lData)
      {
        mu32TimeStamp = lu32TimeStamp;
        mData = lData;
      }
    }

    public class cComTraceString_Entry
    {
      public long mu32TimeStamp;
      public String mszString;

      public cComTraceString_Entry(long lu32TimeStamp, String lszString)
      {
        mu32TimeStamp = lu32TimeStamp;
        mszString = lszString;
      }
    }



    Thread ComTrace_mcThread;

    void ComTrace_vInit()
    {
    }

    void ComTrace_vDeInit()
    {
      ComTrace_mcThread.Abort();
    }

    private void Btn_Test4_Click(object sender, EventArgs e)
    {
      ComTrace_mcThread = new Thread(delegate()
      {
        ComTrace_vProcess();
      });
      ComTrace_mcThread.Start();
    }

    public void ComTrace_vProcess()
    {
      var random = new Random();
      while (true)
      {

        if (ComPort_mcQueueTx.Count == 0)
        {
          int randomnumber = random.Next() % 12;

          int preamble = random.Next() & 0xff;

          u8[] lu8TxData = new u8[randomnumber + 2];


          for (int idx = 0; idx < randomnumber; idx++)
          {
            lu8TxData[2+idx] = (u8)(random.Next());
          }

          lu8TxData[0] = (u8)preamble;
          lu8TxData[1] = (u8)((preamble >> 4) + ((preamble & 15) << 4));
          u8[] lu8TxData2 = new u8[lu8TxData.Length];
          lu8TxData.CopyTo(lu8TxData2, 0);
          ComPort_vPutTx(lu8TxData2);

          lu8TxData[0] = (u8)preamble;
          lu8TxData[1] = (u8)preamble;
          u8[] lu8TxData3 = new u8[lu8TxData.Length];
          lu8TxData.CopyTo(lu8TxData3, 0);
          ComPort_vPutTx(lu8TxData3);
        }
        Thread.Sleep(40);
      }
    }


    
   }
}
