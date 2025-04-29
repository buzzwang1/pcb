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

      MsgTrace_vInit();
      ComTrace_vInit();
      Session_vInit();
      ComPort_vInit();
      BotNet_vInit();
      IAP_vInit();

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

    private void SetText(string text)
    {
      if (IAP_Text_Data.Text == "") IAP_Text_Data.Text = text;
      else IAP_Text_Data.AppendText("\r\n" + text);
    }

    void StartWriteDiagData(Diag_Client lcDiagData)
    {
      int li32Len = 0;
      li32Len = (int)ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Len.Text);

      if (li32Len > lcDiagData.aubBufDataMosi.Length) li32Len = lcDiagData.aubBufDataMosi.Length;
      if (lcDiagData.bWriteBlockIsActive)
      {
        if (li32Len > (int)lcDiagData.ui32Write_BlockSize) li32Len = (int)lcDiagData.ui32Write_BlockSize;
      }

      if (lcDiagData.bWriteBlockIsActive)
      {
        IAP_Send_ByteArray(lcDiagData.aubBufDataMosi.Skip((int)lcDiagData.ui32Write_BlockWriteCnt).Take((Int32)li32Len).ToArray(), false, false, 1);
      }
      else
      {
        IAP_Send_ByteArray(lcDiagData.aubBufDataMosi.Take((Int32)li32Len).ToArray(), false, false, 1);
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

        IAP_TextBox_DiagInfo0.Text = "Interface: " + lcDiagData.szInterfacename + "\r\n";
        IAP_TextBox_DiagInfo0.Text += "Device:    " + lcDiagData.szDevicename + "\r\n";
        IAP_TextBox_DiagInfo0.Text += "Info:      " + lcDiagData.szInfotext + "\r\n";

        IAP_TextBox_DiagInfo1.Text = "Work RAM:   Adr.: " + lcDiagData.ui32WorkMemAdr.ToString() + " 0x" + lcDiagData.ui32WorkMemAdr.ToString("X2") + " " +
                                              "Size: " + lcDiagData.ui32WorkMemSize.ToString() + " 0x" + lcDiagData.ui32WorkMemSize.ToString("X2") + "\r\n";

        vUpdate_TextBoxDecHex(IAP_TextBox_DiagWorkRAM_Adr_Dez, IAP_TextBox_DiagWorkRAM_Adr_Hex, lcDiagData.ui32WorkMemAdr);
        vUpdate_TextBoxDecHex(IAP_TextBox_DiagWorkRAM_Size_Dez, IAP_TextBox_DiagWorkRAM_Size_Hex, lcDiagData.ui32WorkMemSize);

        IAP_TextBox_DiagInfo1.Text += "Partitions: " + lcDiagData.ui32MemCnt.ToString() + "\r\n";

        for (int lit = 0; lit < lcDiagData.ui32MemCnt; lit++)
        {
          IAP_TextBox_DiagInfo1.Text += lcDiagData.MemInfo[lit].toString();
        }

        IAP_TextBox_DiagChecksum.Text = "Checksum: " + lcDiagData.ui32LastCheckSum.ToString() + " 0x" + lcDiagData.ui32LastCheckSum.ToString("X2");

        if ((lcDiagData.aubBufDataMiso != null) && (lcDiagData.bUpdateBuf))
        {
          String lszStr;

          lszStr = IAP_vBuf2String2(lcDiagData.aubBufDataMiso);
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
          ComPort.WriteBufferSize = 128;
          ComPort.Open();
          IAP_moTimestampAbs.Reset();
          IAP_moTimestampAbs.Start();

          IAP_moTimestampRel.Reset();
          IAP_moTimestampRel.Start();

          IAP_miFrameIndexAbs = 0;
          IAP_miFrameIndexRel = 0;
          IAP_miFrameCounter  = 0;

          IAP_moTimerDataIn.Reset();
          IAP_moTimerDataIn.Start();

          Tab_MainBotnet.Enabled = true;
        }
        else if (ComPort.IsOpen)
        {
          this.Btn_PortState.Text = "Open";
          ComPort.Close();

          IAP_moTimestampRel.Stop();
          IAP_moTimestampAbs.Stop();
          IAP_moTimerDataIn.Stop();

          Tab_MainBotnet.Enabled = false;
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
        Session_vDeInit();
        MsgTrace_vDeInit();
        ComTrace_vDeInit();
        ComPort_vDeInit();
        BotNet_vDeInit();
        IAP_vDeInit();

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


    private void IAP_TextBox_CommandWrite_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandWrite_Len_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandRead_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandRead_Len_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandChk_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandChk_Len_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandExecute_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandExecute_Param1_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandExecute_Param2_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandExecute_Param3_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandExecute_Param4_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandWrite_FileOffset_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);

      mcDiag_Client.aubBufDataMosi = baFileToByteArray(this.IAP_TextBox_CommandWrite_FileName.Text, ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_FileOffset.Text));
      this.IAP_TextBox_CommandWrite_Len.Text = mcDiag_Client.aubBufDataMosi.Length.ToString();
    }

    


    private void IAP_TextBox_CommandWrite_FileName_DragDrop(object sender, DragEventArgs e)
    {
      //Vom Datei Explorer
      if (e.Data.GetDataPresent(DataFormats.FileDrop))
      {
        String[] lszDroppedFiles = (string[])e.Data.GetData(DataFormats.FileDrop);
        ((TextBox)sender).Text = lszDroppedFiles[0];

        mcDiag_Client.aubBufDataMosi = baFileToByteArray(this.IAP_TextBox_CommandWrite_FileName.Text, ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_FileOffset.Text));
        this.IAP_TextBox_CommandWrite_Len.Text = mcDiag_Client.aubBufDataMosi.Length.ToString();
        TextBox_DiagWrite.Text = IAP_vBuf2String2(mcDiag_Client.aubBufDataMosi);
      }
    }

    private void IAP_TextBox_CommandWrite_FileName_DragEnter(object sender, DragEventArgs e)
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


    private void IAP_TextBox_CommandReadBlock_BlSize_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }


    private void IAP_TextBox_CommandReadBlock_File_DragDrop(object sender, DragEventArgs e)
    {
      //Vom Datei Explorer
      if (e.Data.GetDataPresent(DataFormats.FileDrop))
      {
        String[] lszDroppedFiles = (string[])e.Data.GetData(DataFormats.FileDrop);
        ((TextBox)sender).Text = lszDroppedFiles[0];
      }
    }

    private void IAP_TextBox_CommandReadBlock_File_DragEnter(object sender, DragEventArgs e)
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

    private void IAP_Button_CommandReadBlock_FileSave_Click(object sender, EventArgs e)
    {
      if (this.IAP_TextBox_CommandReadBlock_File.Text != "")
      {
        if (this.mcDiag_Client.aubBufDataMiso.Length > 0)
        {
          vFileFromByteArray(this.IAP_TextBox_CommandReadBlock_File.Text, this.mcDiag_Client.aubBufDataMiso);
        }
      }
    }

    private void IAP_TextBox_CommandWriteBlock_BlSize_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandStart_Adr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }


    private void IAP_TextBox_CommandEnter_ID_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_TextBox_CommandEnter_ID2_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void TextBox_SrcDeviceAdr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
    }

    private void TextBox_DestDeviceAdr_TextChanged(object sender, EventArgs e)
    {
      String lszString;
      UInt32 lui32Value;

      lszString = ((TextBox)sender).Text;
      lui32Value = ReadTextDezOrBin(lszString);
      vUpdate_TextBoxDecHex(TextBox_CalcDez, TextBox_CalcHex, lui32Value);
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

    u16 u16GetDestAdress()
    {
      u16 lu16Adr = 0;

      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_DstID1.Text) & 0x0F) << 12);
      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_DstID2.Text) & 0x0F) << 8);
      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_DstID3.Text) & 0x0F) << 4);
      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_DstID4.Text) & 0x0F) << 0);
      
      return lu16Adr;
    }

    u16 u16GetSourceAdress()
    {
      u16 lu16Adr = 0;

      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_SrcID1.Text) & 0x0F) << 12);
      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_SrcID2.Text) & 0x0F) << 8);
      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_SrcID3.Text) & 0x0F) << 4);
      lu16Adr += (u16)((ReadTextDezOrBin(this.TextBox_SrcID4.Text) & 0x0F) << 0);
      
      return lu16Adr;
    }

    /* ----------------------------------- ComPort ---------------------------------------------------- */

    public class cComPort_Entry
    {
      public long mu32TimeStamp;
      public u8[] mData;
      public bool isAcknowledged;
      public bool bRepeat;

      public cComPort_Entry()
      {
        mu32TimeStamp = 0;
        bRepeat = false;
        isAcknowledged = true;
        mData = null;
      }

      public cComPort_Entry(long lu32TimeStamp, u8[] lData)
      {
        mu32TimeStamp = lu32TimeStamp;
        isAcknowledged = false;
        mData = lData;
        bRepeat = false;
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

    public void ComPort_vProcess(Queue<cComPort_Entry> ComPort_lcQueueRx, Queue<cComPort_Entry> ComPort_lcQueueTx)
    {
      bool lbRepeat;
      long lRxTimeStamp = 0;
      List<u8> llstRxData = new List<byte>();
      u8[] lu8RxDataDecoded;
      cComPort_Entry lComTx = new cComPort_Entry();
      long lTxAckTimeStamp = 0;
      
      bool lMsgStarted = false;
      

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

              u8 lu8Data = (u8)ComPort.ReadByte();

              llstRxData.Add(lu8Data);
              lu8RxDataDecoded = MsgTrace_mcBotCom.vDecode(lu8Data);
              
              switch (lu8Data>>5)
              {
                case 3: // BotCom Status message
                {
                  String lszStr = "0x" + lu8Data.ToString("X2");
                  u16  lu16Sleep = 0;
                  if ((lu8Data & 1) == 1) // Acknowledge
                  {
                    lszStr += "|ACK";
                    lComTx.isAcknowledged = true; // Nächste Nachricht kann gesendet werden
                  }
                  else
                  {
                    lszStr += "|NACK";
                    lComTx.bRepeat=true;
                    lu16Sleep = 2;
                  }
                  if ((lu8Data & 2) == 2) // Busy
                  {
                    lszStr += "|Busy";
                    lu16Sleep = 10;
                  }

                  ComTrace_vPutString("-- ", lszStr);
                  if (lu16Sleep > 0) Thread.Sleep(lu16Sleep);
                  }
                break;
              
                case 7: // Botocm Frame Start 111x xxxx
                  lRxTimeStamp = ComPort_mcTimestamp.ElapsedMilliseconds;
                  lMsgStarted = true;
                break;
                case 0:  // Botocm Frame Ende 000x xxxx
                  ComTrace_vPutRx(llstRxData.ToArray());
                  if ((llstRxData.Count > 1) && (lMsgStarted))
                  {
                    MsgTrace_vPutRx(new cBotNetMsg_Base(lu8RxDataDecoded));
                    Session_vPutRx(new cBotNetMsg_Base(lu8RxDataDecoded));
                    BotNet_vPutRx(new cBotNetMsg_Base(lu8RxDataDecoded));
                    IAP_Download_vPutRx(new cBotNetMsg_Base(lu8RxDataDecoded));
                  }
                  lMsgStarted = false;
                  llstRxData.Clear();
                break;
              }
            }

            // Data to Send
            if (ComPort.BytesToWrite < 4)
            {
              if (lComTx.isAcknowledged)
              {
                if (ComPort_lcQueueTx.Count > 0)
                {
                  lComTx = ComPort_lcQueueTx.Dequeue();
                  ComPort.Write(lComTx.mData, 0, lComTx.mData.Length);
                  lComTx.isAcknowledged = false;
                  lTxAckTimeStamp = ComPort_mcTimestamp.ElapsedMilliseconds;
                }
              }
              else
              if (lComTx.bRepeat)
              {
                lComTx.bRepeat = false;
                ComPort.Write(lComTx.mData, 0, lComTx.mData.Length);
                lTxAckTimeStamp = ComPort_mcTimestamp.ElapsedMilliseconds;
              }
            }
            
            if (!lComTx.isAcknowledged)
            {
              // Nach 10s ohne Antwort Ack wieder frei machen
              if ((ComPort_mcTimestamp.ElapsedMilliseconds - lTxAckTimeStamp) > 10000)
              {
                lComTx.isAcknowledged = true;
                ComTrace_vPutString("-- ", "!Force Ack!");
              }
            }
            else
            {
              Thread.Sleep(2);
            }

          } while (lbRepeat);
        }
        else
        {
          Thread.Sleep(500);
        }
      }
    }

    /* ----------------------------------- ComTrace ---------------------------------------------------- */
    public class cComTraceString_Entry
    {
      public String mszString;

      public cComTraceString_Entry(String lszString)
      {
        mszString = lszString;
      }
    }

    Queue<cComTraceString_Entry> ComTrace_mcQueueString;
    Thread ComTrace_mcThread;
    Stopwatch ComTrace_mcTimestamp;

    void ComTrace_vInit()
    {
      ComTrace_mcQueueString = new Queue<cComTraceString_Entry>();
      ComTrace_mcTimestamp = new Stopwatch();

      ComTrace_mcTimestamp.Reset();
      ComTrace_mcTimestamp.Start();

      ComTrace_mcThread = new Thread(delegate()
      {
        ComTrace_vProcess(ComTrace_mcQueueString);
      });
      ComTrace_mcThread.Start();
    }

    void ComTrace_vDeInit()
    {
      ComTrace_mcThread.Abort();
    }

    void ComTrace_vPutTx(u8[] lData)
    {
      ComTrace_vPutString("tx ", lData);
    }

    void ComTrace_vPutRx(u8[] lData)
    {
      ComTrace_vPutString("  rx ", lData);
    }


    void ComTrace_vPutString(String lszPreFix, u8[] lData)
    {
      String lszText = "";
      foreach (u8 lu8PayloadValue in lData)
      {
        lszText += lu8PayloadValue.ToString("X2") + ".";
      }
      ComTrace_vPutString(lszPreFix, lszText);
    }

    void ComTrace_vPutString(String lszPreFix, String lszStr)
    {
      String lszText;
      lszText = lszPreFix + ComTrace_mcTimestamp.ElapsedMilliseconds.ToString("#,###,##0") + " ";
      lszText += lszStr + "\r\n";
      ComTrace_mcQueueString.Enqueue(new cComTraceString_Entry(lszText));
    }

    public void ComTrace_vAppendTextBox(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(ComTrace_vAppendTextBox), new object[] { lszText });
        return;
      }
      
      if (Com_Text_ConIO.Text.Length > 10000)
      {
        Com_Text_ConIO.Text.Remove(0, 1000);
      }
      Com_Text_ConIO.AppendText(lszText);
    }

    public void ComTrace_vProcess(Queue<cComTraceString_Entry> ComTrace_lcQueueString)
    {
      StringBuilder lcSb = new StringBuilder();
      uint16 lu16Tick5ms = 0;
      
      while (true)
      {
        while (ComTrace_lcQueueString.Count > 0)
        {
          cComTraceString_Entry lcString = ComTrace_lcQueueString.Dequeue();

          ComTrace_vAppendTextBox(lcString.mszString);
          lcSb.Append(DateTime.Now.ToString("yyyyMMdd HH:mm:ss") + lcString.mszString);
        }

        lu16Tick5ms++;
        if (lu16Tick5ms == 1000)
        {
          lu16Tick5ms = 0;
          if (lcSb.Length > 0)
          {
            File.AppendAllText(@"1Com_Log.txt", lcSb.ToString());
            lcSb.Length = 0;
          }
        }

        Thread.Sleep(5);
      }
    }


    /* ----------------------------------- MsgTrace ---------------------------------------------------- */

    public class cMsgTrace_Entry
    {
      public long mu32TimeStamp;
      public cBotNetMsg_Base mMsg;

      public cMsgTrace_Entry(long lu32TimeStamp, cBotNetMsg_Base lMsg)
      {
        mu32TimeStamp = lu32TimeStamp;
        mMsg = lMsg;
      }
    }

    Queue<cMsgTrace_Entry> MsgTrace_mcQueueRx;
    Queue<cMsgTrace_Entry> MsgTrace_mcQueueTx;
    Thread                 MsgTrace_mcThread;
    Stopwatch              MsgTrace_mcTimestamp;
    cBotCom                MsgTrace_mcBotCom;

    void MsgTrace_vInit()
    {
      MsgTrace_mcQueueRx = new Queue<cMsgTrace_Entry>();
      MsgTrace_mcQueueTx = new Queue<cMsgTrace_Entry>();
      MsgTrace_mcTimestamp = new Stopwatch();
      MsgTrace_mcBotCom = new cBotCom();

      MsgTrace_mcTimestamp.Reset();
      MsgTrace_mcTimestamp.Start();

      MsgTrace_mcThread = new Thread(delegate()
      {
        MsgTrace_vProcess(MsgTrace_mcQueueRx, MsgTrace_mcQueueTx);
      });
      MsgTrace_mcThread.Start();
    }

    void MsgTrace_vDeInit()
    {
      MsgTrace_mcThread.Abort();
    }

    void MsgTrace_vPutRx(cBotNetMsg_Base lMsg)
    {
      MsgTrace_mcQueueRx.Enqueue(new cMsgTrace_Entry(MsgTrace_mcTimestamp.ElapsedMilliseconds, lMsg));
    }

    void MsgTrace_vPutTx(cBotNetMsg_Base lMsg)
    {
      MsgTrace_mcQueueTx.Enqueue(new cMsgTrace_Entry(MsgTrace_mcTimestamp.ElapsedMilliseconds, lMsg));

      u8[] lu8ComPortRawData = MsgTrace_mcBotCom.vEncode(lMsg.ToByteArray());
      ComTrace_vPutTx(lu8ComPortRawData);
      ComPort_vPutTx(lu8ComPortRawData);
    }

    public void MsgTrace_vAppendTextBox(string lszText)
    {
        if (InvokeRequired)
        {
            this.Invoke(new Action<string>(MsgTrace_vAppendTextBox), new object[] {lszText});
            return;
        }

        if (MsgTrace_Text_ConIO.Text.Length > 10000)
        {
          MsgTrace_Text_ConIO.Text.Remove(0, 1000);
        }
        MsgTrace_Text_ConIO.AppendText(lszText);
    }

    public void MsgTrace_vProcess(Queue<cMsgTrace_Entry> MsgTrace_lcQueueRx, Queue<cMsgTrace_Entry> MsgTrace_lcQueueTx)
    {
      String lszText;
      StringBuilder lcSb = new StringBuilder();
      uint16 lu16Tick5ms = 0;
      
      while (true)
      {
        do
        {
          lszText = "";
          if (MsgTrace_lcQueueRx.Count > 0)
          {
            cMsgTrace_Entry lMsg = MsgTrace_lcQueueRx.Dequeue();
            lszText = "  rx " + lMsg.mu32TimeStamp.ToString("#,###,##0") + " ";
            foreach (u8 lu8PayloadValue in lMsg.mMsg.ToByteArray())
            {
              lszText += lu8PayloadValue.ToString("X2") + ".";
            }
          }

          if (MsgTrace_lcQueueTx.Count > 0)
          {
            cMsgTrace_Entry lMsg = MsgTrace_lcQueueTx.Dequeue();
            lszText = "tx " + lMsg.mu32TimeStamp.ToString("#,###,##0") + " ";
            foreach (u8 lu8PayloadValue in lMsg.mMsg.ToByteArray())
            {
              lszText += lu8PayloadValue.ToString("X2") + ".";
            }
          }

          if (lszText != "")
          {
            MsgTrace_vAppendTextBox(lszText + "\r\n");
            lcSb.Append(DateTime.Now.ToString("yyyyMMdd HH:mm:ss") + " " + lszText + "\r\n");
          }
        } while (lszText != "");

        lu16Tick5ms++;
        if (lu16Tick5ms == 1000)
        {
          lu16Tick5ms = 0;
          if (lcSb.Length > 0)
          {
            File.AppendAllText(@"2MsgTrace_Log.txt", lcSb.ToString());
            lcSb.Length = 0;
          }
        }
        
        Thread.Sleep(1);
      }
    }

    /* ----------------------------------- Session ---------------------------------------------------- */

    public class cSession_Entry
    {
      public long mu32TimeStamp;
      public cBotNetMsg_Base mMsg;

      public cSession_Entry(long lu32TimeStamp, cBotNetMsg_Base lMsg)
      {
        mu32TimeStamp = lu32TimeStamp;
        mMsg = lMsg;
      }
    }

    Queue<cSession_Entry> Session_mcQueueRx;
    Thread Session_mcThread;
    Thread Session_mcThreadBlink;
    Stopwatch Session_mcTimestamp;
    cBotCom Session_mcBotCom;
    bool Session_mbConnected;
    u16 Session_mu16ConnectedDeviceID;
    u16 Session_mu16ConnectedBnAdr;

    void Session_vInit()
    {
      Session_mcQueueRx = new Queue<cSession_Entry>();
      Session_mcTimestamp = new Stopwatch();
      Session_mcBotCom = new cBotCom();
      Session_mbConnected = false;

      Session_mcTimestamp.Reset();
      Session_mcTimestamp.Start();

      Session_mcThread = new Thread(delegate()
      {
        Session_vProcess(Session_mcQueueRx);
      });
      Session_mcThread.Start();

      Session_mcThreadBlink = new Thread(delegate()
      {
        Session_vBlink();
      });
      Session_mcThreadBlink.Start();
    }

    void Session_vDeInit()
    {
      Session_mcThreadBlink.Abort();
      Session_mcThread.Abort();
    }

    void Session_vPutRx(cBotNetMsg_Base lMsg)
    {
      Session_mcQueueRx.Enqueue(new cSession_Entry(Session_mcTimestamp.ElapsedMilliseconds, lMsg));
    }

    public void Session_vAppendTextBox(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(Session_vAppendTextBox), new object[] { lszText });
        return;
      }

      if (Session_Text_ConIO.Text.Length > 20000)
      {
        Session_Text_ConIO.Text.Remove(0, 1000);
      }
      Session_Text_ConIO.AppendText(lszText);
    }

    public void Session_vBlink()
    {
      while (true)
      {
        Thread.Sleep(100);
        Session_Label_Blink.BackColor = Color.Transparent;
        Thread.Sleep(100);
        if (Session_mbConnected)
        {
          Session_Label_Blink.BackColor = Color.Red;
        }
      }
    }

    public void Session_vProcess(Queue<cSession_Entry> Session_lcQueueRx)
    {
      String lszText;
      StringBuilder lcSb = new StringBuilder();
      uint16 lu16Tick5ms = 0;
      
      while (true)
      {
        do
        {
          lszText = "";
          if (Session_lcQueueRx.Count > 0)
          {
            cSession_Entry lMsg = Session_lcQueueRx.Dequeue();
            u8[] lu8Msg = lMsg.mMsg.ToByteArray();

            if (lu8Msg.Length == 25)
            {
              
              if ((lu8Msg[24] == 0x80) &&
                  (lu8Msg[0] == 0x11))
              {
                // Info Nachricht ?
                if (lu8Msg[3] == 0x00)
                {
                  u16 lu16BnAdr =     (u16)(((u16)lu8Msg[22] << 8) + (u16)lu8Msg[23]);
                  u16 lu16DeviceAdr = (u16)(((u16)lu8Msg[1]  << 8) + (u16)lu8Msg[2]);
                  u16 lu16Kanal =     (u16)(((u16)lu8Msg[4]  << 8) + (u16)lu8Msg[5]);

                  cBotNetAdress lcBnAdr = new cBotNetAdress();
                  lcBnAdr.Set(lu16BnAdr);

                  lszText = "  rx " + lMsg.mu32TimeStamp.ToString("#,###,##0") + " ";
                  lszText += "Info:  ";
                  lszText += "DeviceAdr: " + lu16DeviceAdr.ToString() + " ";
                  lszText += "BnAdr: " + lcBnAdr.ToString() + " ";
                  lszText += "Kanal: " + lu16Kanal.ToString();

                  // Info Nachricht vom Clock Master?
                  if (lu16DeviceAdr == 0x08)
                  {
                    i16 li16U_Supply = (i16)(((u16)lu8Msg[6] << 8) + (u16)lu8Msg[7]);
                    i16 li16I_Supply = (i16)(((u16)lu8Msg[8]  << 8) + (u16)lu8Msg[9]);
                    i16 li16U_Bat    = (i16)(((u16)lu8Msg[10]  << 8) + (u16)lu8Msg[11]);
                    i16 li16I_Bat    = (i16)(((u16)lu8Msg[12]  << 8) + (u16)lu8Msg[13]);

                    lszText += " Supply: " + li16U_Supply.ToString()+ "mV ";
                    lszText += li16I_Supply.ToString()+ "mA   ";
                    lszText += "Bat: " + li16U_Bat.ToString()+ "mV ";
                    lszText += li16I_Bat.ToString()+ "mA";

                    if (((u8)lu8Msg[14] & 1) == 1) lszText += " S1";
                    if (((u8)lu8Msg[14] & 2) == 2) lszText += " S2";
                    if (((u8)lu8Msg[14] & 4) == 4) lszText += " S3";
                    
                  }
                }
                else if (lu8Msg[3] == 0xFE) // Connection Acknowledge
                {
                  Session_mu16ConnectedBnAdr = (u16)(((u16)lu8Msg[22] << 8) + (u16)lu8Msg[23]);
                  Session_mu16ConnectedDeviceID = (u16)(((u16)lu8Msg[1] << 8) + (u16)lu8Msg[2]);

                  cBotNetAdress lcBnAdr = new cBotNetAdress();
                  lcBnAdr.Set(Session_mu16ConnectedBnAdr);

                  lszText = "  rx " + lMsg.mu32TimeStamp.ToString("#,###,##0") + " ";
                  lszText += "Bridge: Connection Ack: ";
                  lszText += "DeviceAdr: " + Session_mu16ConnectedDeviceID.ToString() + " ";
                  lszText += "BnAdr: " + lcBnAdr.ToString();
                  Session_mbConnected = true;
                }
                else if (lu8Msg[3] == 0xFF) // Connection Acknowledge
                {
                  Session_mu16ConnectedBnAdr = (u16)(((u16)lu8Msg[22] << 8) + (u16)lu8Msg[23]);
                  Session_mu16ConnectedDeviceID = (u16)(((u16)lu8Msg[1] << 8) + (u16)lu8Msg[2]);

                  cBotNetAdress lcBnAdr = new cBotNetAdress();
                  lcBnAdr.Set(Session_mu16ConnectedBnAdr);

                  lszText = "  rx " + lMsg.mu32TimeStamp.ToString("#,###,##0") + " ";
                  lszText += "Bridge: Dis-Connection Ack: ";
                  lszText += "DeviceAdr: " + Session_mu16ConnectedDeviceID.ToString() + " ";
                  lszText += "BnAdr: " + lcBnAdr.ToString();
                  Session_mbConnected = false;
                }
                // Clock Nachricht ?
                else if (lu8Msg[3] == 0x10)
                {
                  u16 lu16BnAdr     = (u16)(((u16)lu8Msg[22] << 8) + (u16)lu8Msg[23]);
                  u16 lu16DeviceAdr = (u16)(((u16)lu8Msg[1]  << 8) + (u16)lu8Msg[2]);

                  cBotNetAdress lcBnAdr = new cBotNetAdress();
                  lcBnAdr.Set(lu16BnAdr);

                  lszText = "  rx " + lMsg.mu32TimeStamp.ToString("#,###,##0") + " ";
                  lszText += "Clock: ";
                  lszText += "DeviceAdr: " + lu16DeviceAdr.ToString() + " ";
                  lszText += "BnAdr: " + lcBnAdr.ToString() + " ";
                  lszText += "Zeit: " + (((u16)lu8Msg[12] << 8) + (u16)lu8Msg[13]).ToString().PadLeft(4, '0') + "-" + lu8Msg[14].ToString().PadLeft(2, '0') + "-" + lu8Msg[15].ToString().PadLeft(2, '0') + " "
                                      + lu8Msg[16].ToString().PadLeft(2, '0') + "-" + lu8Msg[17].ToString().PadLeft(2, '0') + "-" + lu8Msg[18].ToString().PadLeft(2, '0');
                }
              }
            }
          }

          if (lszText != "")
          {
            Session_vAppendTextBox(lszText + "\r\n");
            lcSb.Append(DateTime.Now.ToString("yyyyMMdd HH:mm:ss") + " " + lszText + "\r\n");
          }
        } while (lszText != "");

        lu16Tick5ms++;
        if (lu16Tick5ms == 1000)
        {
          lu16Tick5ms = 0;
          if (lcSb.Length > 0)
          {
            File.AppendAllText(@"3Session_Log.txt", lcSb.ToString());
            lcSb.Length = 0;
          }
        }

        Thread.Sleep(1);
      }
    }

    cBotNetMsg_Base Session_Get_Start_Message()
    {
      cBotNetMsg_MsgProt lMsg = new cBotNetMsg_MsgProt();
      MsgTest_u8Active = 0;

      u16 lu16DeviceDestID   = (u16)(ReadTextDezOrBin(this.TextBox_DestDeviceAdr.Text));
      u16 lu16DeviceSourceID = (u16)(ReadTextDezOrBin(this.TextBox_SrcDeviceAdr.Text));
      u8[] lu8Start = new u8[] { (u8)(lu16DeviceDestID >> 8), (u8)(lu16DeviceDestID & 0xFF), 0xFE, (u8)(lu16DeviceSourceID >> 8), (u8)(lu16DeviceSourceID & 0xFF), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      // 0x11: Radio_Info
      //lMsg.vPrepare(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), 0x11, 0);
      lMsg.vPrepare(u16GetSourceAdress(), u16GetDestAdress(), 0x11);
      lMsg.vEncode(lu8Start);
      return lMsg;
    }

    cBotNetMsg_Base Session_Get_Stop_Message()
    {
      cBotNetMsg_MsgProt lMsg = new cBotNetMsg_MsgProt();
      MsgTest_u8Active = 0;

      u16 lu16DeviceID = (u16)(ReadTextDezOrBin(this.TextBox_DestDeviceAdr.Text));
      u8[] lu8Start = new u8[] { (u8)(lu16DeviceID >> 8), (u8)(lu16DeviceID & 0xFF), 0xFF, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
      // 0x11: Radio_Info
      //lMsg.vPrepare(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), 0x11, 0);
      lMsg.vPrepare(u16GetSourceAdress(), u16GetDestAdress(), 0x11);
      lMsg.vEncode(lu8Start);
      return lMsg;
    }

    private void Session_Btn_Start_Click(object sender, EventArgs e)
    {
      MsgTrace_vPutTx(Session_Get_Start_Message());
    }

    private void Session_Btn_Stop_Click(object sender, EventArgs e)
    {
      MsgTrace_vPutTx(Session_Get_Stop_Message());
    }

    /* ----------------------------------- MsgTest ---------------------------------------------------- */

    u8 MsgTest_u8Active = 0;

    List<u8> MsgTest_Get_Payload()
    {
      List<u8> llstPayload = new List<byte>();

      if (MsgTest_Text_Payload.Text[0] == '\'')
      {
        String lszPayloadValues = MsgTest_Text_Payload.Text.Substring(1);
        llstPayload.AddRange(Encoding.ASCII.GetBytes(lszPayloadValues + "\r\n"));
      }
      else
      {
        String[] lszPayloadValues = MsgTest_Text_Payload.Text.Split('.');
        foreach (String lszPayloadValue in lszPayloadValues)
        {
          llstPayload.Add((u8)ReadTextDezOrBin(lszPayloadValue));
        }
      }
      
      return llstPayload;
    }

    cBotNetMsg_Base MsgTest_Get_Message()
    {
      if (MsgTest_u8Active == 0)
      {
        cBotNetMsg_MsgProt lMsg = new cBotNetMsg_MsgProt();
        MsgTest_u8Active = 0;

        //lMsg.vPrepare(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), (u16)ReadTextDezOrBin(this.MsgTest_Text_Msg_ID.Text), 0);
        lMsg.vPrepare(u16GetSourceAdress(), u16GetDestAdress(), (u16)ReadTextDezOrBin(this.MsgTest_Text_Msg_ID.Text));
        lMsg.vEncode(MsgTest_Get_Payload());

        return lMsg;
      }
      else
      {
        cBotNetMsg_StreamProt lMsg = new cBotNetMsg_StreamProt();
        MsgTest_u8Active = 1;

        if (MsgTest_RadioButton_Stream_Data.Checked)
        {
          // Daten Nachricht
          lMsg.vPrepareData(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), 0, (u8)ReadTextDezOrBin(this.MsgTest_Text_Stream_Idx.Text));
          lMsg.vEncode(MsgTest_Get_Payload());
        }
        else
        {
          // Commando
          lMsg.vPrepareCmd(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), 0, (u8)(this.MsgTest_Combo_Stream_Cmd.SelectedIndex + 1), (u8)ReadTextDezOrBin(this.MsgTest_Text_Stream_Idx.Text));
          lMsg.vEncode(new u8[] { (u8)ReadTextDezOrBin(this.MsgTest_Text_Stream_Para1.Text), (u8)ReadTextDezOrBin(this.MsgTest_Text_Stream_Para2.Text), (u8)ReadTextDezOrBin(this.MsgTest_Text_Stream_Para3.Text) });
        }
        return lMsg;
      }
    }

    void MsgTest_Set_Message_MsgProt()
    {
      MsgTest_u8Active = 0;
      cBotNetMsg_Base lMsg = MsgTest_Get_Message();

      MsgTest_Text_Data.Text = "";
      foreach (u8 lu8PayloadValue in lMsg.ToByteArray())
      {
        MsgTest_Text_Data.Text += lu8PayloadValue.ToString("X2") + ".";
      }
    }

    void MsgTest_Set_Message_StreamProt()
    {
      MsgTest_u8Active = 1;
      cBotNetMsg_Base lMsg = MsgTest_Get_Message();

      MsgTest_Text_Data.Text = "";
      foreach (u8 lu8PayloadValue in lMsg.ToByteArray())
      {
        MsgTest_Text_Data.Text += lu8PayloadValue.ToString("X2") + ".";
      }
    }

    private void MsgTest_Text_Msg_ID_TextChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_MsgProt();
    }

    private void MsgTest_Text_Stream_Idx_TextChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_RadioButton_Stream_Data_CheckedChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_RadioButton_Stream_Cmd_CheckedChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_Text_Stream_Cmd_TextChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_Combo_Stream_Cmd_SelectedIndexChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_Text_Stream_Para1_TextChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_Text_Stream_Para2_TextChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_Text_Stream_Para3_TextChanged(object sender, EventArgs e)
    {
      MsgTest_Set_Message_StreamProt();
    }

    private void MsgTest_Btn_Send_Click(object sender, EventArgs e)
    {
      MsgTrace_vPutTx(MsgTest_Get_Message());
    }

    private void MsgTest_Text_Payload_TextChanged(object sender, EventArgs e)
    {
      if (MsgTest_u8Active == 0)
      {
        MsgTest_Set_Message_MsgProt();
      }
      else
      {
        MsgTest_Set_Message_StreamProt();
      }
    }

    /* ----------------------------------- Cmd ---------------------------------------------------- */

    void Cmd_Send_Message()
    {
      cBotNetMsg_StreamProt lMsg = new cBotNetMsg_StreamProt();
      List<u8> lu8Data = new List<byte>(Encoding.ASCII.GetBytes(Cmd_Combo_CmdToSend.Text));
      lu8Data.Add(0x0D);

      lMsg.vPrepareData(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), 0, (u8)ReadTextDezOrBin(this.Cmd_Text_StreamID.Text));
      lMsg.vEncode(lu8Data.ToArray());
      MsgTrace_vPutTx(lMsg);
    }

    private void Cmd_Btn_Send_Click(object sender, EventArgs e)
    {
      Cmd_Send_Message();
    }

    private void Cmd_Combo_CmdToSend_KeyUp(object sender, KeyEventArgs e)
    {
      if (e.KeyCode == Keys.Return)
      {
        Cmd_Send_Message();
      }
    }

    /* ----------------------------------- BotNet ---------------------------------------------------- */


    public class cBnStreamSysPortIap : cBotNetStreamPort
    {

      public class cIAP_DataLine
      {
        public u8[] mData;
        public cIAP_DataLine()
        {
          mData = new u8[32];
        }

        public cIAP_DataLine(u8[] lu8Array)
        {
          mData = (u8[])lu8Array.Clone();
        }
      }

      public Queue<cIAP_DataLine> mCmdBufRx;
      List<u8> mLineRx;
      DateTime lcTime;

      public cBnStreamSysPortIap()
      {
        mCmdBufRx = new Queue<cIAP_DataLine>();
        mLineRx = new List<u8>(32);
        lcTime = DateTime.Now;
      }

      public override void vProcess()
      {
        // 5s Timeout to clear the input buffer
        if (((DateTime.Now - lcTime).TotalSeconds) > 5)
        {
          mLineRx.Clear();
        }

        while (mcRxRingBuf.Count > 0)
        {
          mLineRx.Add(mcRxRingBuf.Dequeue());
          if (mLineRx.Count > 31)
          {
            cIAP_DataLine lcData = new cIAP_DataLine(mLineRx.ToArray());
            mCmdBufRx.Enqueue(lcData);
            mLineRx.Clear();
            lcTime = DateTime.Now;
          }
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

    Thread                 BotNet_mcThread;
    cBotNet                BotNet_mcBn;
    cBnStreamSysPortIap    mcBnPortIap;

    void BotNet_vInit()
    {
      mcBnPortIap = new cBnStreamSysPortIap();

      BotNet_mcBn = new cBotNet(u16GetSourceAdress(), new cBotNet_DownLink());
      u16 lu16PortIdx = BotNet_mcBn.mcStreamSys.AddPort(mcBnPortIap);
      IAP_Text_SteamID.Text = lu16PortIdx.ToString();

      BotNet_mcThread = new Thread(delegate()
      {
        BotNet_vProcess();
      });
      BotNet_mcThread.Start();
    }

    void BotNet_vDeInit()
    {
      BotNet_mcThread.Abort();
    }

    void BotNet_vPutRx(cBotNetMsg_Base lMsg)
    {
      BotNet_mcBn.mcDownLinks.mcLinkRx.Enqueue(lMsg);
    }

    public void BotNet_vSetCmdTextBox(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(BotNet_vSetCmdTextBox), new object[] { lszText });
        return;
      }
      Cmd_Text_ConIo.Text = lszText;
      Cmd_Text_ConIo.SelectionStart = Cmd_Text_ConIo.TextLength;
      Cmd_Text_ConIo.ScrollToCaret();
    }

    public void BotNet_vProcess()
    {
      u8 lu8UpdateControls = 20;

      while (true)
      {
        BotNet_mcBn.vProcess();

        while (BotNet_mcBn.mcDownLinks.mcLinkRx.Count > 0)
        {
          MsgTrace_vPutTx(BotNet_mcBn.mcDownLinks.mcLinkRx.Dequeue());
        }

        //Visualisierung
        if (lu8UpdateControls > 0)
        {
          lu8UpdateControls--;
        }
        else
        {
          lu8UpdateControls = 20;

          // Cmd Console
          BotNet_vSetCmdTextBox(BotNet_mcBn.mcStreamSys.mcCmdPort.mcMyPrinter.mpui8TextBufOut);
        }

        Thread.Sleep(1);
      }
    }

    /* ----------------------------------- IAP ---------------------------------------------------- */

    Thread IAP_mcThread;

    Byte[] IAP_mui8BufIn;
    Byte[] IAP_mui8BufOut;

    Stopwatch IAP_moTimestampAbs;
    Stopwatch IAP_moTimestampRel;

    int IAP_miFrameIndexAbs;
    int IAP_miFrameIndexRel;
    int IAP_miFrameCounter;
    bool IAP_mbStartDownload;

    Stopwatch IAP_moTimerDataIn;

    void IAP_vInit()
    {
      IAP_mui8BufIn = new Byte[32];

      IAP_mui8BufOut = new Byte[32];

      IAP_moTimestampAbs = new Stopwatch();
      IAP_moTimestampRel = new Stopwatch();

      IAP_miFrameIndexAbs = 0;
      IAP_miFrameIndexRel = 0;
      IAP_miFrameCounter  = 0;

      IAP_moTimerDataIn = new Stopwatch();

      IAP_vBufClear(this.IAP_mui8BufOut);
      IAP_vBufClear(this.IAP_mui8BufIn);
      IAP_vBufUpdateBufOut();

      IAP_mcThread = new Thread(delegate()
      {
        IAP_vProcess();
      });
      IAP_mcThread.Start();

      IAP_mbStartDownload = false;

      IAP_Download_vInit();

      
    }

    void IAP_vDeInit()
    {
      IAP_Download_vDeInit();
      IAP_mcThread.Abort();
    }

    // ---- IAP Controls

    public void IAP_vSet_IAP_Text_Data(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(IAP_vSet_IAP_Text_Data), new object[] { lszText });
        return;
      }

      if (IAP_Text_Data.Text == "") IAP_Text_Data.Text = lszText;
      else IAP_Text_Data.AppendText(lszText);
    }

    public void IAP_vSet_IAP_TextBox_DiagInfo0(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(IAP_vSet_IAP_TextBox_DiagInfo0), new object[] { lszText });
        return;
      }
      IAP_TextBox_DiagInfo0.Text = lszText;
    }

    private void IAP_TabControl_Command_SelectedIndexChanged(object sender, EventArgs e)
    {
      IAP_vBufUpdateBufOut();
      this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
    }

    private void IAP_Button_Send_Click(object sender, EventArgs e)
    {
      IAP_Send(true);
    }

    private void IAP_Button_Clear_TextData_Click(object sender, EventArgs e)
    {
      this.IAP_Text_Data.Text = "";
    }

    // ---- IAP Buf
    private void IAP_vBufShow(Byte[] mui8Buf)
    {
      UInt32 lui32Temp;
      UInt32 lui32Offset;

      lui32Offset = 0;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_1, lui32Temp.ToString("X8"));

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_2, lui32Temp.ToString("X8"));

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_3, lui32Temp.ToString("X8"));

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_4, lui32Temp.ToString("X8"));

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_5, lui32Temp.ToString("X8"));

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_6, lui32Temp.ToString("X8"));

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_7, lui32Temp.ToString("X8"));

      lui32Offset += 4;
      lui32Temp = (UInt32)mui8Buf[lui32Offset + 0] +
                  ((UInt32)mui8Buf[lui32Offset + 1] << 8) +
                  ((UInt32)mui8Buf[lui32Offset + 2] << 16) +
                  ((UInt32)mui8Buf[lui32Offset + 3] << 24);
      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0_8, lui32Temp.ToString("X8"));

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

      MAIN_vSetInvokeTextBox(IAP_TextBox_Command0, lszString);
    }

    void IAP_vBufUpdateBufOut()
    {
      IAP_vBufUpdateBufOut(this.IAP_TabControl_Command.SelectedIndex);
    }
    
    void IAP_vBufUpdateBufOut(int liIndex)
    {
      switch (liIndex)
      {
        case 1: //Info1
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 0;
          IAP_vBufShow(this.IAP_mui8BufOut);
          break;
        case 2: //Info2
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 1;
          IAP_vBufShow(this.IAP_mui8BufOut);
          break;
        case 3: //Write
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 16;
          this.IAP_mui8BufOut[1] = 128;

          if (ReadTextDezOrBin(this.IAP_TextBox_CommandWriteBlock_BlSize.Text) == 0)
          {
            IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
            IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Len.Text)), IAP_mui8BufOut, 4, 0, 16);
          }
          else
          {
            int li32Len = 0;
            li32Len = (int)ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Len.Text);
            if (li32Len > (int)ReadTextDezOrBin(this.IAP_TextBox_CommandWriteBlock_BlSize.Text))
            {
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandWriteBlock_BlSize.Text)), IAP_mui8BufOut, 4, 0, 16);
            }
            else
            {
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Len.Text)), IAP_mui8BufOut, 4, 0, 16);
            }
          }
          break;
        case 4: //Read
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 32;

          if (ReadTextDezOrBin(this.IAP_TextBox_CommandReadBlock_BlSize.Text) == 0)
          {
            IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
            IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Len.Text)), IAP_mui8BufOut, 4, 0, 16);
          }
          else
          {
            int li32Len = 0;
            li32Len = (int)ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Len.Text);
            if (li32Len > (int)ReadTextDezOrBin(this.IAP_TextBox_CommandReadBlock_BlSize.Text))
            {
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandReadBlock_BlSize.Text)), IAP_mui8BufOut, 4, 0, 16);
            }
            else
            {
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
              IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Len.Text)), IAP_mui8BufOut, 4, 0, 16);
            }
          }
          break;
        case 5: //Checksumme
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 48;
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandChk_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandChk_Len.Text)), IAP_mui8BufOut, 4, 0, 16);
          break;
        case 6: //Execute
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 64;
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandExecute_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandExecute_Param1.Text)), IAP_mui8BufOut, 4, 0, 12);
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandExecute_Param2.Text)), IAP_mui8BufOut, 4, 0, 16);
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandExecute_Param3.Text)), IAP_mui8BufOut, 4, 0, 20);
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandExecute_Param4.Text)), IAP_mui8BufOut, 4, 0, 24);
          break;
        case 7: //Exceute
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 65;
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandStart_Adr.Text)), IAP_mui8BufOut, 4, 0, 8);
          break;
        case 8: //Exit
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 128;
          break;
        case 9: //Enter
          IAP_vBufClear(this.IAP_mui8BufOut);
          this.IAP_mui8BufOut[0] = 129;
          IAP_vBufMemCopy(bU32toByteArray(0x01020304), IAP_mui8BufOut, 4, 0, 4);
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandEnter_ID1.Text)), IAP_mui8BufOut, 4, 0, 8);
          IAP_vBufMemCopy(bU32toByteArray(ReadTextDezOrBin(this.IAP_TextBox_CommandEnter_ID2.Text)), IAP_mui8BufOut, 4, 0, 12);
          IAP_vBufMemCopy(bU32toByteArray(0x00000000), IAP_mui8BufOut, 4, 0, 16);
          break;
      }
    }

    private String IAP_vBuf2String(Byte[] mui8Buf, bool lbisInput)
    {
      String lszString;
      String lszString2;

      lszString = "";

      lszString2 = this.IAP_miFrameIndexAbs.ToString() + " ";
      while (lszString2.Length < 5) lszString2 = " " + lszString2;
      lszString += lszString2;

      lszString2 = this.IAP_miFrameIndexRel.ToString() + " ";
      while (lszString2.Length < 4) lszString2 = " " + lszString2;
      lszString += lszString2;

      lszString2 = this.IAP_moTimestampAbs.ElapsedMilliseconds.ToString("#,###,##0") + " ";
      while (lszString2.Length < 9) lszString2 = " " + lszString2;
      lszString += lszString2;

      lszString2 = this.IAP_moTimestampRel.ElapsedMilliseconds.ToString("#,###,##0") + " ";
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


    private String IAP_vBuf2String2(Byte[] mui8Buf)
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

    private String IAP_vBufPrint(u8[] mui8Buf, bool lbisInput, bool lbNewline, bool lbNewCmd)
    {
      String lszString;

      if (lbNewline) lszString = "\r\n";
      else lszString = "";

      if (lbNewCmd) lszString = "---------------------------------------------------------------------------------------------------------------------------------------------------------------\r\n";

      lszString += IAP_vBuf2String(mui8Buf, lbisInput);

      return lszString;
    }


    private void IAP_vBufClear(Byte[] mui8Buf)
    {
      for (Int32 lit = 0; lit < 32; lit++)
      {
        mui8Buf[lit] = 0x55;
      }
    }

    private void IAP_vBufMemCopy(Byte[] mui8BufScr, Byte[] mui8BufDst, int liLen, int liOffsetScr, int liOffsetDst)
    {
      for (Int32 lit = 0; (lit < 32) && (lit < liLen); lit++)
      {
        mui8BufDst[lit + liOffsetDst] = mui8BufScr[lit + liOffsetScr];
      }
    }

    private void IAP_vBufMemCopy(Byte[] mui8BufScr, Byte[] mui8BufDst, int liLen)
    {
      IAP_vBufMemCopy(mui8BufScr, mui8BufDst, liLen, 0, 0);
    }


    private void IAP_vBufDecode(Byte[] mui8Buf, Diag_Client lcDiag_Client)
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

              IAP_vBufMemCopy(mui8Buf, lszInterfacename, 14, 2, 0);
              IAP_vBufMemCopy(mui8Buf, lszDevicename, 16, 16, 0);

              lcDiag_Client.szInterfacename = szByteArraytoString(lszInterfacename);
              lcDiag_Client.szDevicename = szByteArraytoString(lszDevicename);
              break;
            case 0x01:
              Byte[] lszInfotext = new Byte[30];

              IAP_vBufMemCopy(mui8Buf, lszInfotext, 30, 2, 0);

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
                    IAP_vBufMemCopy(mui8Buf, lszInfo, 24, 8, 0);
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

              this.mcDiag_Client.ui32Write_StartAdr = ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Adr.Text);
              this.mcDiag_Client.ui32Write_Len = ReadTextDezOrBin(this.IAP_TextBox_CommandWrite_Len.Text);
              this.mcDiag_Client.ui32Write_BlockSize = ReadTextDezOrBin(this.IAP_TextBox_CommandWriteBlock_BlSize.Text);

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

              //if (lcDiag_Client.bWriteBlockIsActive)
              {
                if (lui32Temp != lcDiag_Client.ui32CheckSumData)
                {
                  this.BeginInvoke(new IAP_SetTextCallback(SetText), new object[] { "CheckSum NOK (" + lui32Temp.ToString() + "<->" + lcDiag_Client.ui32CheckSumData.ToString() + ")\r\n" });
                }
                else
                {
                  this.BeginInvoke(new IAP_SetTextCallback(SetText), new object[] { "CheckSum OK (" + lui32Temp.ToString() + ")\r\n" });
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

                  IAP_vBufClear(this.IAP_mui8BufOut);
                  this.IAP_mui8BufOut[0] = 0x10;
                  this.IAP_mui8BufOut[1] = 128;
                  IAP_vBufMemCopy(bU32toByteArray(lcDiag_Client.ui32Write_StartAdr + lcDiag_Client.ui32Write_BlockWriteCnt), IAP_mui8BufOut, 4, 0, 8);
                  IAP_vBufMemCopy(bU32toByteArray(lui32Len), IAP_mui8BufOut, 4, 0, 16);
                  this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
                  IAP_Send(false);
                }
                else
                {
                  lcDiag_Client.bWriteBlockIsActive = false;
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
              this.mcDiag_Client.ui32Read_StartAdr = ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Adr.Text);
              this.mcDiag_Client.ui32Read_Len = ReadTextDezOrBin(this.IAP_TextBox_CommandRead_Len.Text);
              this.mcDiag_Client.ui32Read_BlockSize = ReadTextDezOrBin(this.IAP_TextBox_CommandReadBlock_BlSize.Text);

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
                this.BeginInvoke(new IAP_SetTextCallback(SetText), new object[] { "CheckSum NOK\r\n" });
              }
              else
              {
                this.BeginInvoke(new IAP_SetTextCallback(SetText), new object[] { "CheckSum OK\r\n" });
              }

              if (lcDiag_Client.bReadBlockIsActive)
              {
                if (lcDiag_Client.ui32Read_BlockReadCnt < lcDiag_Client.ui32Read_Len)
                {
                  UInt32 lui32Len;

                  lui32Len = lcDiag_Client.ui32Read_Len - lcDiag_Client.ui32Read_BlockReadCnt;
                  if (lui32Len > lcDiag_Client.ui32Read_BlockSize) lui32Len = lcDiag_Client.ui32Read_BlockSize;

                  lcDiag_Client.ui32Read_BlockReadCntEnd = lcDiag_Client.ui32Read_BlockReadCnt + lui32Len;

                  IAP_vBufClear(this.IAP_mui8BufOut);
                  this.IAP_mui8BufOut[0] = 32;
                  IAP_vBufMemCopy(bU32toByteArray(lcDiag_Client.ui32Read_StartAdr + lcDiag_Client.ui32Read_BlockReadCnt), IAP_mui8BufOut, 4, 0, 8);
                  IAP_vBufMemCopy(bU32toByteArray(lui32Len), IAP_mui8BufOut, 4, 0, 16);
                  this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
                  IAP_Send(false);
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

                IAP_vBufMemCopy(mui8Buf, lcDiag_Client.aubBufDataMiso, lui32DatatoCpy, 2, lcDiag_Client.i32BufDataMisoIdx);

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
          vDiag_IAP_Send(Diag_laui8MISO_Buf);

          lpvFunc = (Diag_vCall*)lpui8Adr;
          lpvFunc(lui32Param[0], lui32Param[1], lui32Param[2], lui32Param[4]);
        }
        break;

      case 0x50: // Exit
        break;
        }
       */
    }

    void IAP_Send_ByteArray(u8[] lbBuf, bool lbNewline, bool lbNewCmd, int liMode)
    {
      u8[] lui8BufOut = new Byte[32];

      IAP_miFrameIndexAbs++;
      IAP_miFrameIndexRel++;

      try
      {
        if (liMode == 0) // Einzelne Nachricht
        {
          IAP_vBufClear(lui8BufOut);
          IAP_vBufMemCopy(lbBuf, lui8BufOut, 32, 0, 0);
          IAP_vSet_IAP_Text_Data(IAP_vBufPrint(lui8BufOut, false, lbNewline, lbNewCmd));

          cBotNetMsg_StreamProt lMsg = new cBotNetMsg_StreamProt();

          lMsg.vPrepareData(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), 0, (u8)ReadTextDezOrBin(this.IAP_Text_SteamID.Text));
          lMsg.vEncode(lui8BufOut);

          MsgTrace_vPutTx(lMsg);

          IAP_moTimerDataIn.Reset();
          IAP_moTimerDataIn.Start();
        }
        else  // Write Block Nachricht
        {
          Int32 li32Idx;
          li32Idx = 0;

          IAP_miFrameCounter = 128;

          while (li32Idx < lbBuf.Length)
          {
            if (ComPort_mcQueueTx.Count < 4)
            {
              IAP_vBufClear(lui8BufOut);

              lui8BufOut[0] = 0x10;
              lui8BufOut[1] = (byte)(IAP_miFrameCounter);

              if ((lbBuf.Length - li32Idx) > 29)
              {
                IAP_vBufMemCopy(lbBuf, lui8BufOut, 30, li32Idx, 2);
              }
              else
              {
                lui8BufOut[1] = 0xFF;
                IAP_vBufMemCopy(lbBuf, lui8BufOut, lbBuf.Length - li32Idx, li32Idx, 2);
              }

              IAP_vSet_IAP_Text_Data(IAP_vBufPrint(lui8BufOut, false, lbNewline, lbNewCmd) + "\r\n");

              cBotNetMsg_StreamProt lMsg = new cBotNetMsg_StreamProt();

              lMsg.vPrepareData(false, false, true, u16GetSourceAdress(), u16GetDestAdress(), 0, (u8)ReadTextDezOrBin(this.IAP_Text_SteamID.Text));
              lMsg.vEncode(lui8BufOut);

              MsgTrace_vPutTx(lMsg);

              if (IAP_miFrameCounter == 254)
              {
                IAP_miFrameCounter = 128;
              }
              else
              {
                IAP_miFrameCounter++;
              }

              IAP_moTimerDataIn.Reset();
              IAP_moTimerDataIn.Start();

              li32Idx += 30;
            }
            else
            {
              Thread.Sleep(1);
            }
          }
        }
      }
      catch (Exception err)
      {
        this.TextError.Text += err.ToString() + "\n";
      }
    }

    private void IAP_Send(bool lbNewline)
    {
      IAP_moTimestampRel.Reset();
      IAP_moTimestampRel.Start();

      IAP_moTimerDataIn.Reset();
      IAP_moTimerDataIn.Start();

      IAP_miFrameIndexRel = 0;

      IAP_Send_ByteArray(this.IAP_mui8BufOut, true, true, 0);
      if (lbNewline)
      {
        IAP_vSet_IAP_Text_Data("\r\n");
      }
    }

    public void IAP_vProcess()
    {
      String lszString;
      int liModeOld = mcDiag_Client.iCmdMode;

      while (true)
      {
        while (mcBnPortIap.mCmdBufRx.Count > 0)
        {
          IAP_moTimerDataIn.Reset();
          IAP_moTimerDataIn.Start();

          //this.Lbl_InputCnt.Text = mcBnPortIap.mCmdBufRx.Count.ToString();

          IAP_mui8BufIn = mcBnPortIap.mCmdBufRx.Dequeue().mData;

          IAP_miFrameIndexAbs++;
          IAP_miFrameIndexRel++;

          lszString = mcDiag_Client.iCmdMode.ToString() + IAP_vBuf2String(IAP_mui8BufIn, true);
          if (lszString != String.Empty)
          {
            IAP_vSet_IAP_Text_Data(lszString + "\r\n");
          }

          IAP_vBufDecode(IAP_mui8BufIn, mcDiag_Client);
          if (mcDiag_Client.iCmdMode == 0)
          {
            this.BeginInvoke(new IAP_DiagUpdateCallback(UpdateDiagData), new object[] {mcDiag_Client});
          }
        }

        if (liModeOld != mcDiag_Client.iCmdMode)
        {
          liModeOld = mcDiag_Client.iCmdMode;
          if ((mcDiag_Client.iCmdMode == 1) || (mcDiag_Client.iCmdMode == 2))
          {
            this.BeginInvoke(new IAP_DiagStartWriteCallback(StartWriteDiagData), new object[] { mcDiag_Client });
          }
        }

        Thread.Sleep(1);
      }
    }

    // ----------------------------- IAP Download

    public class IAP_Download_Entry
    {
      public cBotNetMsg_Base mMsg;

      public IAP_Download_Entry(cBotNetMsg_Base lMsg)
      {
        mMsg = lMsg;
      }
    }

    Thread IAP_mcThreadDownload;
    public Byte[] IAP_DL_aubBufData;
    Queue<IAP_Download_Entry> IAP_Download_mcQueueRx;
    u32 IAP_Download_mu32FlashAdr;
    u32 IAP_Download_mu32FlashSize;
    u32 IAP_Download_mu32FileSize;

    void IAP_Download_vInit()
    {
      IAP_Download_mcQueueRx = new Queue<IAP_Download_Entry>();
      IAP_mcThreadDownload = new Thread(delegate()
      {
        IAP_vProcess_DownLoad();
      });
      IAP_mcThreadDownload.Start();

      IAP_Download_mu32FlashAdr = 0;
      IAP_Download_mu32FlashSize =0;
      IAP_Download_mu32FileSize = 0;
    }

    void IAP_Download_vDeInit()
    {
      IAP_mcThreadDownload.Abort();
    }

    void IAP_Download_vPutRx(cBotNetMsg_Base lMsg)
    {
      IAP_Download_mcQueueRx.Enqueue(new IAP_Download_Entry(lMsg));
    }

    public void IAP_vSet_IAP_TextBox_Donwload_Status(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(IAP_vSet_IAP_TextBox_Donwload_Status), new object[] { lszText });
        return;
      }

      if (IAP_TextBox_CommandDl_Status.Text == "") IAP_TextBox_CommandDl_Status.Text = lszText;
      else IAP_TextBox_CommandDl_Status.AppendText("\r\n" + lszText);
    }

    public void IAP_vSet_IAP_TextBox_Donwload_Adr(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(IAP_vSet_IAP_TextBox_Donwload_Adr), new object[] { lszText });
        return;
      }
      IAP_TextBox_CommandDl_Adr.Text = lszText;
    }

    public void IAP_vSet_IAP_TextBox_Donwload_Len(string lszText)
    {
      if (InvokeRequired)
      {
        this.Invoke(new Action<string>(IAP_vSet_IAP_TextBox_Donwload_Len), new object[] { lszText });
        return;
      }
      IAP_TextBox_CommandDl_Len.Text = lszText;
    }


    public void IAP_vProcess_DownLoad()
    {
      while (true)
      {

        while (true)
        {
          IAP_Download_mcQueueRx.Clear();
          IAP_vSet_IAP_TextBox_Donwload_Status("");
          IAP_vSet_IAP_TextBox_Donwload_Status("Ready");
          while (!IAP_mbStartDownload)
          {
            Thread.Sleep(50);
          }
          IAP_mbStartDownload = false;

          IAP_vSet_IAP_TextBox_Donwload_Status("Start Download");

          if (IAP_CheckBox_CommandDl_Connect.Checked)
          {
            if (!Session_mbConnected)
            {
              MsgTrace_vPutTx(Session_Get_Stop_Message());
              Thread.Sleep(250);
            }

            // --- Conntect to Target
            if (!Session_mbConnected)
            {
              u32 lu32Timeout = 0;
              IAP_vSet_IAP_TextBox_Donwload_Status("Try to connect. (3min Timeout)");

              MsgTrace_vPutTx(Session_Get_Start_Message());

              while ((!Session_mbConnected) && (lu32Timeout < 3 * 60 * 1000))
              {
                Thread.Sleep(50);
                lu32Timeout += 50;
              }
            }

            if (Session_mbConnected)
            {
              IAP_vSet_IAP_TextBox_Donwload_Status("  => We are connected");
            }
            else
            {
              IAP_vSet_IAP_TextBox_Donwload_Status("  => Timeout");
              break;
            }
          }

          bool lbSuccess = true;
          u32 lu32ClientBufferSize = 0;
          u32 lu32IapPageSize = 0;

          // --- Enter Diag
          if ((lbSuccess) && (lu32ClientBufferSize == 0))
          {
            u32 lu32Timeout = 0;
            u32 lu32Retrys = 1;
            bool lbAck = false;

            lbSuccess = false;
            lu32Retrys = 0;
            
            while ((lu32Retrys < 5) && (lu32ClientBufferSize == 0))
            {
              lu32Timeout = 0;
              lbAck = false;
              lbSuccess = false;

              IAP_vSet_IAP_TextBox_Donwload_Status((lu32Retrys+1).ToString()+".Try: Enter Diag (1s Timeout)");

              IAP_Download_mcQueueRx.Clear();
              IAP_vBufUpdateBufOut(9);
              this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
              IAP_Send(true);

              // Bei einigen muss man 2 Enter machen
              // Das erste  Mal um der App in den Bootloader zu springen
              // Das zweite Mal um den Bootloader zu aktivieren
              while ((!lbSuccess) && (lu32Timeout < 1 * 1000)) // 2s
              {
                Thread.Sleep(50);
                lu32Timeout += 50;

                while (IAP_Download_mcQueueRx.Count > 0)
                {
                  IAP_Download_Entry lcEntry = IAP_Download_mcQueueRx.Dequeue();
                  //     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37
                  // rx 01.81.00.00.00.04.03.02.01.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.E0.00.10.00.81.
                  if ((lcEntry.mMsg.u8Len() == 38) &&
                      (lcEntry.mMsg.mMsgData[37] == 0x81) &&
                      (lcEntry.mMsg.mMsgData[0] == System.Convert.ToByte(IAP_Text_SteamID.Text)) &&
                      (lcEntry.mMsg.mMsgData[1] == 0x81))
                  {
                    lbSuccess = true;
                    if (lcEntry.mMsg.mMsgData[2] == 0)
                    {
                      lu32ClientBufferSize = (((u32)lcEntry.mMsg.mMsgData[20]) << 24) + (((u32)lcEntry.mMsg.mMsgData[19]) << 16) + (((u32)lcEntry.mMsg.mMsgData[18]) << 8) + (u32)lcEntry.mMsg.mMsgData[17];
                      lbAck = true;
                    }
                  }
                }
              }
              if (lbSuccess)
              {
                if (lbAck)
                {
                  if (lu32ClientBufferSize == 0)
                  {
                    IAP_vSet_IAP_TextBox_Donwload_Status("  => We are in " + lu32Retrys.ToString() + ". Try. No ClientBufferSize. Retry.");
                    Thread.Sleep(50);
                  }
                  else
                  {
                    IAP_vSet_IAP_TextBox_Donwload_Status("  => We are in. ClientBufferSize = " + lu32ClientBufferSize.ToString());
                  }
                }
                else
                {
                  IAP_vSet_IAP_TextBox_Donwload_Status("  => NACK");
                }
              }
              else
              {
                IAP_vSet_IAP_TextBox_Donwload_Status("  => Timeout");
              }
              lu32Retrys++;              
            }
          }

          if (!lbSuccess) break;

          // --- Read Info2 to get IAP Flash Partition
          if (lbSuccess)
          {
            u32 lu32Timeout = 0;
            lbSuccess = false;
            bool lbAck = false;
            u32 lu32Partitions = 0;

            IAP_vSet_IAP_TextBox_Donwload_Status("Read Info2 (3min Timeout)");

            IAP_Download_mcQueueRx.Clear();
            IAP_vBufUpdateBufOut(2);
            this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
            IAP_Send(true);

            while ((!lbSuccess) && (lu32Timeout < 3 * 60 * 1000)) // 5s
            {
              Thread.Sleep(50);
              lu32Timeout += 50;

              while (IAP_Download_mcQueueRx.Count > 0)
              {
                IAP_Download_Entry lcEntry = IAP_Download_mcQueueRx.Dequeue();
                //     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37
                // rx 01.01.01.00.01.05.00.00.00.43.43.4D.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                // rx 01.01.01.00.00.05.00.00.00.00.00.00.00.01.00.00.00.00.00.00.00.00.00.00.10.00.00.00.00.00.40.00.00.E0.00.10.00.81.
                // rx 01.01.01.00.01.04.00.00.00.52.61.6D.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                // rx 01.01.01.00.00.04.00.00.00.00.00.00.00.01.00.00.00.00.00.00.00.00.00.00.20.00.00.00.00.00.C0.00.00.E0.00.10.00.81.
                // rx 01.01.01.00.01.03.00.00.00.46.6C.61.73.68.20.45.45.50.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                // rx 01.01.01.00.00.03.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.F0.03.08.00.00.00.00.00.10.00.00.E0.00.10.00.81.
                // rx 01.01.01.00.01.02.00.00.00.46.6C.61.73.68.20.49.41.50.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                // rx 01.01.01.00.00.02.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.C0.02.08.00.00.00.00.00.30.01.00.E0.00.10.00.81.
                // rx 01.01.01.00.01.01.00.00.00.46.6C.61.73.68.20.41.50.50.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                // rx 01.01.01.00.00.01.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.40.00.08.00.00.00.00.00.80.02.00.E0.00.10.00.81.
                // rx 01.01.01.00.01.00.00.00.00.46.6C.61.73.68.20.42.4C.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                // rx 01.01.01.00.00.00.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.00.00.08.00.00.00.00.00.40.00.00.E0.00.10.00.81.
                // rx 01.01.00.00.00.00.00.00.00.00.00.00.00.06.00.00.00.00.00.00.00.5A.24.00.20.00.00.00.00.00.10.00.00.E0.00.10.00.81.
                // tx 01.01.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.10.00.E0.00.81.
                if ((lcEntry.mMsg.u8Len() == 38) &&
                    (lcEntry.mMsg.mMsgData[37] == 0x81) &&
                    (lcEntry.mMsg.mMsgData[0] == System.Convert.ToByte(IAP_Text_SteamID.Text)) &&
                    (lcEntry.mMsg.mMsgData[1] == 0x1))
                {
                  lbSuccess = true;

                  if (lcEntry.mMsg.mMsgData[2] == 0)
                  {
                    lbAck = true;
                    lu32Partitions = lcEntry.mMsg.mMsgData[13];
                    break;
                  }
                }
              }
            }

            if (lbSuccess)
            {
              if (lbAck)
              {
                bool lbFlashPartitionFound = false;
                IAP_Download_Entry lcEntry_Last;
                IAP_Download_Entry lcEntry = null;

                IAP_vSet_IAP_TextBox_Donwload_Status("  => Info2 Ack");
                IAP_vSet_IAP_TextBox_Donwload_Status("  => Read " + lu32Partitions.ToString() + "Partitions (30s Timeout)");

                lbSuccess = false;
                lu32Timeout = 0;
                while ((!lbSuccess) && (lu32Timeout < 1 * 30 * 1000)) // 5s
                {
                  Thread.Sleep(50);
                  lu32Timeout += 50;

                  while (IAP_Download_mcQueueRx.Count > 0)
                  {
                    lcEntry_Last = lcEntry;
                    lcEntry = IAP_Download_mcQueueRx.Dequeue();
                    //     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37
                    // rx 01.01.01.00.01.05.00.00.00.43.43.4D.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                    // rx 01.01.01.00.00.05.00.00.00.00.00.00.00.01.00.00.00.00.00.00.00.00.00.00.10.00.00.00.00.00.40.00.00.E0.00.10.00.81.
                    // rx 01.01.01.00.01.04.00.00.00.52.61.6D.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                    // rx 01.01.01.00.00.04.00.00.00.00.00.00.00.01.00.00.00.00.00.00.00.00.00.00.20.00.00.00.00.00.C0.00.00.E0.00.10.00.81.
                    // rx 01.01.01.00.01.03.00.00.00.46.6C.61.73.68.20.45.45.50.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                    // rx 01.01.01.00.00.03.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.F0.03.08.00.00.00.00.00.10.00.00.E0.00.10.00.81.
                    // rx 01.01.01.00.01.02.00.00.00.46.6C.61.73.68.20.49.41.50.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                    // rx 01.01.01.00.00.02.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.C0.02.08.00.00.00.00.00.30.01.00.E0.00.10.00.81.
                    // rx 01.01.01.00.01.01.00.00.00.46.6C.61.73.68.20.41.50.50.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                    // rx 01.01.01.00.00.01.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.40.00.08.00.00.00.00.00.80.02.00.E0.00.10.00.81.
                    // rx 01.01.01.00.01.00.00.00.00.46.6C.61.73.68.20.42.4C.00.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.E0.00.10.00.81.
                    // rx 01.01.01.00.00.00.00.00.00.10.00.00.00.00.08.00.00.00.00.00.00.00.00.00.08.00.00.00.00.00.40.00.00.E0.00.10.00.81.
                    // rx 01.01.00.00.00.00.00.00.00.00.00.00.00.06.00.00.00.00.00.00.00.5A.24.00.20.00.00.00.00.00.10.00.00.E0.00.10.00.81.
                    // tx 01.01.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.10.00.E0.00.81.
                    if ((lcEntry.mMsg.u8Len() == 38) &&
                        (lcEntry.mMsg.mMsgData[37] == 0x81) &&
                        (lcEntry.mMsg.mMsgData[0] == System.Convert.ToByte(IAP_Text_SteamID.Text)) &&
                        (lcEntry.mMsg.mMsgData[1] == 0x1) &&
                        (lcEntry.mMsg.mMsgData[2] == 0x1))
                    {
                      if (lcEntry.mMsg.mMsgData[4] == 0x1)
                      {
                        String lszPartitionName;

                        lszPartitionName = System.Text.Encoding.UTF8.GetString(lcEntry.mMsg.ToByteArray(), 9, 24);

                        IAP_vSet_IAP_TextBox_Donwload_Status("  => Process Partition: " + (lcEntry.mMsg.mMsgData[5] + 1).ToString() + ": " + lszPartitionName);

                        // Flash IAP Partition ?
                        if (lszPartitionName.IndexOf("Flash IAP") >= 0)
                        {
                          lbFlashPartitionFound = true;
                          lu32IapPageSize            = (((u32)lcEntry_Last.mMsg.mMsgData[16]) << 24) + (((u32)lcEntry_Last.mMsg.mMsgData[15]) << 16) + (((u32)lcEntry_Last.mMsg.mMsgData[14]) << 8) + (u32)lcEntry_Last.mMsg.mMsgData[13];
                          IAP_Download_mu32FlashAdr  = (((u32)lcEntry_Last.mMsg.mMsgData[24]) << 24) + (((u32)lcEntry_Last.mMsg.mMsgData[23]) << 16) + (((u32)lcEntry_Last.mMsg.mMsgData[22]) << 8) + (u32)lcEntry_Last.mMsg.mMsgData[21];
                          IAP_Download_mu32FlashSize = (((u32)lcEntry_Last.mMsg.mMsgData[32]) << 24) + (((u32)lcEntry_Last.mMsg.mMsgData[31]) << 16) + (((u32)lcEntry_Last.mMsg.mMsgData[30]) << 8) + (u32)lcEntry_Last.mMsg.mMsgData[29];
                          
                          IAP_vSet_IAP_TextBox_Donwload_Status("    => Adr:  " + IAP_Download_mu32FlashAdr.ToString() + " (0x" + IAP_Download_mu32FlashAdr.ToString("X2") + ")");
                          IAP_vSet_IAP_TextBox_Donwload_Status("    => Size: " + IAP_Download_mu32FlashSize.ToString() + " (0x" + IAP_Download_mu32FlashSize.ToString("X2") + ")");
                        }

                        // Letzte Partition ?
                        if (lcEntry.mMsg.mMsgData[5] + 1 == lu32Partitions)
                        {
                          lbSuccess = true;
                        }
                        break;
                      }
                    }
                  }
                }
                if (!lbSuccess)
                {
                  IAP_vSet_IAP_TextBox_Donwload_Status("  => Timeout");
                  break;
                }
                if (!lbFlashPartitionFound)
                {
                  IAP_vSet_IAP_TextBox_Donwload_Status("  => No \"Flash IAP\" found");
                  break;
                }
                if (IAP_Download_mu32FlashAdr == 0)
                {
                  IAP_vSet_IAP_TextBox_Donwload_Status("  => FlashAdr == 0");
                  break;
                }
                if (IAP_Download_mu32FlashSize == 0)
                {
                  IAP_vSet_IAP_TextBox_Donwload_Status("  => FlashSize == 0");
                  break;
                }
                if (IAP_Download_mu32FileSize > IAP_Download_mu32FlashSize)
                {
                  IAP_vSet_IAP_TextBox_Donwload_Status("  => Filesize > Flashsize");
                  break;
                }
              }
              else
              {
                IAP_vSet_IAP_TextBox_Donwload_Status("  => NACK");
                break;
              }
            }
            else
            {
              IAP_vSet_IAP_TextBox_Donwload_Status("  => Timeout");
              break;
            }
          }

          if (!lbSuccess) break;

          //     0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37
          // tx 01.10.80.55.55.55.55.55.55.00.00.02.08.55.55.55.55.00.04.00.00.55.55.55.55.55.55.55.55.55.55.55.55.10.00.E0.00.81.
          // rx 01.10.00.01.00.00.00.00.00.00.C0.02.08.00.00.00.00.00.04.00.00.18.5B.01.00.00.00.00.00.00.00.00.00.E0.00.10.00.81.
          // tx 01.10.FF.65.20.6D.69.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.55.10.00.E0.00.81.
          // tx 01.10.A1.75.6E.67.20.69.6E.20.64.61.73.2C.20.77.61.73.20.69.63.68.20.65.72.6C.65.62.74.20.68.61.62.10.00.E0.00.81.
          // tx 01.10.A0.63.68.20.C3.BC.62.65.72.73.65.74.7A.65.20.6D.69.72.20.64.69.65.73.65.20.45.72.66.61.68.72.10.00.E0.00.81.
          // tx 01.10.9F.72.2C.20.68.69.65.72.20.69.6E.20.4D.69.74.74.65.6C.65.75.72.6F.70.61.2C.20.75.6E.64.20.69.10.00.E0.00.81.
          // tx 01.10.9E.20.6B.65.69.6E.65.20.65.69.67.65.6E.65.20.45.72.66.61.68.72.75.6E.67.20.76.6F.6E.20.6D.69.10.00.E0.00.81.
          // tx 01.10.9D.76.6F.6E.20.46.75.6B.75.73.68.69.6D.61.2C.20.64.61.6E.6E.20.69.73.74.20.64.61.73.20.6A.61.10.00.E0.00.81.
          // tx 01.10.9C.57.65.6E.6E.20.53.69.65.20.64.69.65.20.45.72.66.61.68.72.75.6E.67.20.6E.65.68.6D.65.6E.20.10.00.E0.00.81.
          // tx 01.10.9B.74.20.6A.61.20.6E.75.6E.20.6B.65.69.6E.20.67.72.6F.C3.9F.65.73.20.57.75.6E.64.65.72.21.20.10.00.E0.00.81.
          // tx 01.10.9A.7A.75.20.62.65.74.72.61.63.68.74.65.6E.3F.0D.0A.0D.0A.41.2E.4B.2E.3A.20.44.61.73.20.69.73.10.00.E0.00.81.
          // tx 01.10.99.20.64.61.7A.75.20.61.6E.2C.20.64.69.65.73.65.73.20.54.68.65.6D.61.20.65.72.6E.65.75.74.20.10.00.E0.00.81.
          // tx 01.10.98.6E.6C.69.6E.65.20.67.65.73.74.65.6C.6C.74.2E.20.57.61.73.20.74.72.65.69.62.74.20.53.69.65.10.00.E0.00.81.
          // tx 01.10.97.6D.61.20.54.73.63.68.65.72.6E.6F.62.79.6C.20.65.69.6E.69.67.65.20.56.69.64.65.6F.73.20.6F.10.00.E0.00.81.
          // tx 01.10.96.20.57.65.62.73.65.69.74.65.20.77.77.77.2E.64.63.70.74.76.2E.64.65.20.7A.75.6D.20.54.68.65.10.00.E0.00.81.
          // tx 01.10.95.65.72.72.20.4B.6C.75.67.65.2C.20.53.69.65.20.68.61.62.65.6E.20.61.75.66.20.69.68.72.65.72.10.00.E0.00.81.
          // tx 01.10.94.20.41.6C.65.78.61.6E.64.65.72.20.4B.6C.75.67.65.0D.0A.0D.0A.50.2E.52.2E.2D.52.2E.3A.20.48.10.00.E0.00.81.
          // tx 01.10.93.69.74.67.65.73.63.68.69.63.68.74.65.20.45.69.6E.20.49.6E.74.65.72.76.69.65.77.20.6D.69.74.10.00.E0.00.81.
          // tx 01.10.92.0A.53.6F.6E.64.65.6E.20.69.6E.20.52.61.6E.64.62.65.72.65.69.63.68.65.20.64.65.72.20.5A.65.10.00.E0.00.81.
          // tx 01.10.91.20.7C.20.30.35.2E.30.35.2E.32.30.31.31.20.7C.0D.0A.49.6E.74.65.72.76.69.65.77.73.0D.0A.0D.10.00.E0.00.81.
          // tx 01.10.90.76.6F.6C.76.65.72.20.2D.20.5A.65.69.74.73.63.68.72.69.66.74.20.66.C3.BC.72.20.46.69.6C.6D.10.00.E0.00.81.
          // tx 01.10.8F.52.6F.65.70.73.74.6F.72.66.66.2D.52.6F.62.69.61.6E.6F.20.7C.20.51.75.65.6C.6C.65.20.52.65.10.00.E0.00.81.
          // tx 01.10.8E.56.6F.6E.20.4C.79.64.69.61.20.44.79.6B.69.65.72.20.75.6E.64.20.50.68.69.6C.69.70.70.65.20.10.00.E0.00.81.
          // tx 01.10.8D.20.4C.61.6E.67.65.20.54.65.78.74.65.20.0D.0A.0D.0A.4C.61.6E.67.65.20.54.65.78.74.65.0D.0A.10.00.E0.00.81.
          // tx 01.10.8C.0D.0A.20.20.20.20.48.6F.6D.65.0D.0A.20.20.20.20.41.6B.74.75.65.6C.6C.65.73.0D.0A.20.20.20.10.00.E0.00.81.
          // tx 01.10.8B.0D.0A.20.20.20.20.42.6C.6F.67.0D.0A.0D.0A.53.69.65.20.73.69.6E.64.20.68.69.65.72.3A.0D.0A.10.00.E0.00.81.
          // tx 01.10.8A.20.20.20.20.46.69.6C.6D.65.6D.61.63.68.65.72.0D.0A.20.20.20.20.46.65.72.6E.73.65.68.65.6E.10.00.E0.00.81.
          // tx 01.10.89.65.6C.6C.65.73.0D.0A.20.20.20.20.50.65.72.73.6F.6E.0D.0A.20.20.20.20.41.75.74.6F.72.0D.0A.10.00.E0.00.81.
          // tx 01.10.88.65.72.20.4B.6C.75.67.65.0D.0A.0D.0A.20.20.20.20.48.6F.6D.65.0D.0A.20.20.20.20.41.6B.74.75.10.00.E0.00.81.
          // tx 01.10.87.20.48.61.75.70.74.69.6E.68.61.6C.74.20.73.70.72.69.6E.67.65.6E.0D.0A.41.6C.65.78.61.6E.64.10.00.E0.00.81.
          // tx 01.10.86.20.53.69.65.20.75.6E.74.65.72.20.22.44.61.74.65.6E.73.63.68.75.74.7A.22.2E.0D.0A.5A.75.6D.10.00.E0.00.81.
          // tx 01.10.85.68.72.20.49.6E.66.6F.72.6D.61.74.69.6F.6E.65.6E.20.68.69.65.72.7A.75.20.66.69.6E.64.65.6E.10.00.E0.00.81.
          // tx 01.10.84.64.61.73.73.20.77.69.72.20.43.6F.6F.6B.69.65.73.20.76.65.72.77.65.6E.64.65.6E.2E.20.4D.65.10.00.E0.00.81.
          // tx 01.10.83.C3.A4.72.65.6E.20.53.69.65.20.73.69.63.68.20.65.69.6E.76.65.72.73.74.61.6E.64.65.6E.2C.20.10.00.E0.00.81.
          // tx 01.10.82.72.20.4E.75.74.7A.75.6E.67.20.75.6E.73.65.72.65.72.20.44.69.65.6E.73.74.65.20.65.72.6B.6C.10.00.E0.00.81.
          // tx 01.10.81.65.6C.6C.75.6E.67.20.75.6E.73.65.72.65.72.20.44.69.65.6E.73.74.65.2E.20.4D.69.74.20.64.65.10.00.E0.00.81.
          // tx 01.10.80.0D.0A.43.6F.6F.6B.69.65.73.20.65.72.6C.65.69.63.68.74.65.72.6E.20.64.69.65.20.42.65.73.74.10.00.E0.00.81.
          // rx 01.10.00.00.00.00.00.00.00.00.C0.02.08.00.00.00.00.00.04.00.00.00.00.00.00.F0.03.00.00.00.00.00.00.E0.00.10.00.81.
          // --- Write Data
          {
            u32 lu32Timeout = 0;
            bool lbAck = false;
            bool lbOk = false;
            
            int  liTrys = 0;

            lbSuccess = false;

            u32 lu32Checksum_Send = 0;
            u32 lu32Checksum_Received;
            u32 lu32Index;

            uint32 u32Adress = IAP_Download_mu32FlashAdr;
            uint32 u32DataIdx = 0;
            uint32 u32Data2Copy = lu32IapPageSize;
            
            if (lu32ClientBufferSize > u32Data2Copy) u32Data2Copy = lu32ClientBufferSize;
            MAIN_vSetInvokeTextBox(IAP_TextBox_CommandDl_BlockLen, u32Data2Copy.ToString());
            
            //uint32 u32Data2Copy = 1024 * 2; //  in 2kb Blocks
            //IAP_Download_mu32FlashSize

            MAIN_vSetInvokeTextBox(IAP_TextBox_CommandDl_BlockLen, "");
            MAIN_vSetInvokeTextBox(IAP_TextBox_CommandDl_BlockLen, u32Data2Copy.ToString());


            while ((liTrys < 3) && (u32DataIdx < IAP_Download_mu32FileSize))
            {
              liTrys++;
              lbSuccess = false;
              lbAck = false;
              lbOk = false;
              lu32Timeout = 0;
              lu32Checksum_Received = 0;

              // --- Start Übertragung der Headline

              if (u32Data2Copy + u32DataIdx >= IAP_Download_mu32FileSize)
              {
                u32Data2Copy = IAP_Download_mu32FileSize - u32DataIdx;
              }

              IAP_vSet_IAP_TextBox_Donwload_Status("Write " + IAP_TextBox_CommandDl_BlockLen.Text + "B Block to 0x" + u32Adress.ToString("X2") + " " + (((u32DataIdx * 100) / IAP_Download_mu32FileSize)).ToString() + "% (3min Timeout) " + liTrys.ToString() + ". Try");

              IAP_Download_mcQueueRx.Clear();

              MAIN_vSetInvokeTextBox(IAP_TextBox_CommandWrite_Adr, u32Adress.ToString());
              MAIN_vSetInvokeTextBox(IAP_TextBox_CommandWrite_FileOffset, (0).ToString());
              MAIN_vSetInvokeTextBox(IAP_TextBox_CommandWrite_Len, u32Data2Copy.ToString());
              MAIN_vSetInvokeTextBox(IAP_TextBox_CommandWriteBlock_BlSize, u32Data2Copy.ToString());

              mcDiag_Client.aubBufDataMosi = IAP_DL_aubBufData.Skip((int)u32DataIdx).Take((int)u32Data2Copy).ToArray();

              //Checksumme der gesendeten Daten
              lu32Checksum_Send = 1;
              for (lu32Index = 0; lu32Index < u32Data2Copy; lu32Index++)
              {
                lu32Checksum_Send += mcDiag_Client.aubBufDataMosi[lu32Index];
              }

              IAP_vBufUpdateBufOut(3);
              this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
              IAP_Send(true);
              
              IAP_vSet_IAP_TextBox_Donwload_Status("Wait for Block start (3min Timeout)");
              while ((!lbSuccess) && (lu32Timeout < 1 * 10 * 1000)) // 5s
              {
                Thread.Sleep(5);
                lu32Timeout += 5;

                while (IAP_Download_mcQueueRx.Count > 0)
                {
                  IAP_Download_Entry lcEntry = IAP_Download_mcQueueRx.Dequeue();
                  // --- Warten auf die Antwort für die Headline 16 0 0
                  if ((lcEntry.mMsg.u8Len() == 38) &&
                      (lcEntry.mMsg.mMsgData[37] == 0x81) &&
                      (lcEntry.mMsg.mMsgData[0] == System.Convert.ToByte(IAP_Text_SteamID.Text)) &&
                      (lcEntry.mMsg.mMsgData[1] == 0x10))
                  {
                    lbSuccess = true;
                    if ((lcEntry.mMsg.mMsgData[2] == 0) &&
                        (lcEntry.mMsg.mMsgData[3] == 0))
                    {
                      lbAck = true;
                    }
                    else
                    {
                      IAP_vSet_IAP_TextBox_Donwload_Status("  => NACK");
                    }
                    break;
                  }
                }
              }
              if (!lbAck)
              {
                IAP_vSet_IAP_TextBox_Donwload_Status("  => Timeout");
              }
              
              
              if (lbAck)
              {
                // Warten auf das Ende ds Blocks und Checksumme vergleichen
                lbSuccess = false;
                lbAck = false;
                bool lbExit = false;

                IAP_vSet_IAP_TextBox_Donwload_Status("Wait for Block finished (3min Timeout)");
                while ((!lbSuccess) && (!lbExit) && (lu32Timeout < 1 * 20 * 1000)) // 5s
                {
                  Thread.Sleep(5);
                  lu32Timeout += 5;

                  while (IAP_Download_mcQueueRx.Count > 0)
                  {
                    IAP_Download_Entry lcEntry = IAP_Download_mcQueueRx.Dequeue();
                    // --- Warten auf die Antwort für die Headline 16 0 1
                    if ((lcEntry.mMsg.u8Len() == 38) &&
                        (lcEntry.mMsg.mMsgData[37] == 0x81) &&
                        (lcEntry.mMsg.mMsgData[0] == System.Convert.ToByte(IAP_Text_SteamID.Text)) &&
                        (lcEntry.mMsg.mMsgData[1] == 0x10))
                    {
                      lbSuccess = true;

                      if ((lcEntry.mMsg.mMsgData[2] == 0) &&
                          (lcEntry.mMsg.mMsgData[3] == 1))
                      {
                        lbAck = true;
                        lbExit = true;

                        lu32Checksum_Received = (((u32)lcEntry.mMsg.mMsgData[24]) << 24) + (((u32)lcEntry.mMsg.mMsgData[23]) << 16) + (((u32)lcEntry.mMsg.mMsgData[22]) << 8) + (u32)lcEntry.mMsg.mMsgData[21];
                        if (lu32Checksum_Send == lu32Checksum_Received)
                        {
                          lbOk = true;
                          IAP_vSet_IAP_TextBox_Donwload_Status("  => Checksum ok");
                          u32DataIdx += u32Data2Copy;
                          u32Adress += u32Data2Copy;
                          liTrys = 0;
                        }
                        else
                        {
                          IAP_vSet_IAP_TextBox_Donwload_Status("  => Checksum Error");
                        }
                        break;
                      }
                      
                      if (lcEntry.mMsg.mMsgData[2] == 1)
                      {
                        IAP_vSet_IAP_TextBox_Donwload_Status("  => NACK");
                        lbExit = true;
                        break;
                      }
                    }
                  }
                }
                if (!lbSuccess)
                {
                  IAP_vSet_IAP_TextBox_Donwload_Status("  => Timeout");
                }
              }
            }
            if (!lbOk) break;
          }


          if (IAP_CheckBox_CommandDl_Reset.Checked)
          {
            // --- Reset
            IAP_vSet_IAP_TextBox_Donwload_Status("Send Reset");
            MAIN_vSetInvokeTextBox(IAP_TextBox_CommandStart_Adr, (0).ToString()); // 0 for reset
            IAP_vBufUpdateBufOut(7);
            this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
            IAP_Send(true);
            Thread.Sleep(500);
          }
          else
          {
            // --- Exit Diag
            IAP_vSet_IAP_TextBox_Donwload_Status("Exit Diag");

            IAP_vBufUpdateBufOut(8);
            this.BeginInvoke(new IAP_vBufShow_Callback(IAP_vBufShow), new object[] { this.IAP_mui8BufOut });
            IAP_Send(true);
            Thread.Sleep(500);
          }


          break;
        }

        // --- Disconnect
        MsgTrace_vPutTx(Session_Get_Stop_Message());
      }
    }

    private void IAP_TextBox_CommandDl_File_DragDrop(object sender, DragEventArgs e)
    {
      //Vom Datei Explorer
      if (e.Data.GetDataPresent(DataFormats.FileDrop))
      {
        String[] lszDroppedFiles = (string[])e.Data.GetData(DataFormats.FileDrop);
        ((TextBox)sender).Text = lszDroppedFiles[0];

        IAP_DL_aubBufData = baFileToByteArray(lszDroppedFiles[0], 0);
        this.IAP_TextBox_CommandDl_Len.Text = IAP_DL_aubBufData.Length.ToString();
        IAP_Download_mu32FileSize = (u32)IAP_DL_aubBufData.Length;

        IAP_mbStartDownload = true;
      }
    }

    private void IAP_TextBox_CommandDl_File_DragEnter(object sender, DragEventArgs e)
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

    private void Tab_MainBotnet_SelectedIndexChanged(object sender, EventArgs e)
    {
      switch (Tab_MainBotnet.SelectedIndex)
      {
        case 0: // Com_Trace
          Com_Text_ConIO.SelectionStart = Com_Text_ConIO.TextLength;
          Com_Text_ConIO.ScrollToCaret();
        break;
        case 1: // Msg_Trace
          MsgTrace_Text_ConIO.SelectionStart = MsgTrace_Text_ConIO.TextLength;
          MsgTrace_Text_ConIO.ScrollToCaret();
        break;
        case 2: // Session
          Session_Text_ConIO.SelectionStart = Session_Text_ConIO.TextLength;
          Session_Text_ConIO.ScrollToCaret();
        break; 
        case 3: // Msg Test
        break;
        case 4: // Cmd
          Cmd_Text_ConIo.SelectionStart = Cmd_Text_ConIo.TextLength;
          Cmd_Text_ConIo.ScrollToCaret();
        break;
        case 5: // Iap
        break;
      }
    }
  }
}
