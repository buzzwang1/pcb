namespace Project
{
  partial class Form1
  {
    /// <summary>
    /// Erforderliche Designervariable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Verwendete Ressourcen bereinigen.
    /// </summary>
    /// <param name="disposing">True, wenn verwaltete Ressourcen gelöscht werden sollen; andernfalls False.</param>
    protected override void Dispose(bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose();
      }
      base.Dispose(disposing);
    }

    #region Vom Windows Form-Designer generierter Code

    /// <summary>
    /// Erforderliche Methode für die Designerunterstützung.
    /// Der Inhalt der Methode darf nicht mit dem Code-Editor geändert werden.
    /// </summary>
    private void InitializeComponent()
    {
      this.Btn_Ports = new System.Windows.Forms.Button();
      this.Btn_PortState = new System.Windows.Forms.Button();
      this.ComboBox_Ports = new System.Windows.Forms.ComboBox();
      this.ComboBox_Baud = new System.Windows.Forms.ComboBox();
      this.ComboBox_DataBits = new System.Windows.Forms.ComboBox();
      this.ComboBox_StopBits = new System.Windows.Forms.ComboBox();
      this.ComboBox_Parity = new System.Windows.Forms.ComboBox();
      this.ComboBox_HandShaking = new System.Windows.Forms.ComboBox();
      this.Label_CTSStatus = new System.Windows.Forms.Label();
      this.Lable_DSRStatus = new System.Windows.Forms.Label();
      this.Label_RIStatus = new System.Windows.Forms.Label();
      this.Label_BreakStatus = new System.Windows.Forms.Label();
      this.TextError = new System.Windows.Forms.TextBox();
      this.TextBox_CalcHex = new System.Windows.Forms.TextBox();
      this.TextBox_CalcDez = new System.Windows.Forms.TextBox();
      this.Lbl_InputCnt = new System.Windows.Forms.Label();
      this.Tab_MainBotnet = new System.Windows.Forms.TabControl();
      this.tabPage1 = new System.Windows.Forms.TabPage();
      this.Com_Text_ConIO = new System.Windows.Forms.TextBox();
      this.tabPage2 = new System.Windows.Forms.TabPage();
      this.MsgTrace_Text_ConIO = new System.Windows.Forms.TextBox();
      this.tabPage3 = new System.Windows.Forms.TabPage();
      this.Session_Text_ConIO = new System.Windows.Forms.TextBox();
      this.Session_Btn_Stop = new System.Windows.Forms.Button();
      this.Session_Btn_Start = new System.Windows.Forms.Button();
      this.tabPage4 = new System.Windows.Forms.TabPage();
      this.label31 = new System.Windows.Forms.Label();
      this.MsgTest_Text_Payload = new System.Windows.Forms.TextBox();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.MsgTest_Combo_Stream_Cmd = new System.Windows.Forms.ComboBox();
      this.MsgTest_Text_Stream_Para3 = new System.Windows.Forms.TextBox();
      this.MsgTest_Text_Stream_Para2 = new System.Windows.Forms.TextBox();
      this.MsgTest_Text_Stream_Para1 = new System.Windows.Forms.TextBox();
      this.MsgTest_RadioButton_Stream_Cmd = new System.Windows.Forms.RadioButton();
      this.MsgTest_RadioButton_Stream_Data = new System.Windows.Forms.RadioButton();
      this.label29 = new System.Windows.Forms.Label();
      this.MsgTest_Text_Stream_Idx = new System.Windows.Forms.TextBox();
      this.MsgTest_Btn_Send = new System.Windows.Forms.Button();
      this.MsgTest_Text_Data = new System.Windows.Forms.TextBox();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.label28 = new System.Windows.Forms.Label();
      this.MsgTest_Text_Msg_ID = new System.Windows.Forms.TextBox();
      this.tabPage5 = new System.Windows.Forms.TabPage();
      this.Cmd_Combo_CmdToSend = new System.Windows.Forms.ComboBox();
      this.Cmd_Btn_Send = new System.Windows.Forms.Button();
      this.label30 = new System.Windows.Forms.Label();
      this.label25 = new System.Windows.Forms.Label();
      this.Cmd_Text_StreamID = new System.Windows.Forms.TextBox();
      this.Cmd_Text_ConIo = new System.Windows.Forms.TextBox();
      this.tabPage6 = new System.Windows.Forms.TabPage();
      this.label26 = new System.Windows.Forms.Label();
      this.IAP_Text_SteamID = new System.Windows.Forms.TextBox();
      this.label15 = new System.Windows.Forms.Label();
      this.label14 = new System.Windows.Forms.Label();
      this.IAP_Button_Clear_TextData = new System.Windows.Forms.Button();
      this.IAP_TextBox_DiagWorkRAM_Size_Hex = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_DiagWorkRAM_Adr_Hex = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_DiagWorkRAM_Size_Dez = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_DiagWorkRAM_Adr_Dez = new System.Windows.Forms.TextBox();
      this.IAP_TabControl_Command = new System.Windows.Forms.TabControl();
      this.TabPage_Command_Download = new System.Windows.Forms.TabPage();
      this.IAP_CheckBox_CommandDl_Reset = new System.Windows.Forms.CheckBox();
      this.label38 = new System.Windows.Forms.Label();
      this.label34 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandDl_Status = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandDl_BlockLen = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandDl_Adr = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandDl_Len = new System.Windows.Forms.TextBox();
      this.label36 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandDl_File = new System.Windows.Forms.TextBox();
      this.label37 = new System.Windows.Forms.Label();
      this.TabPage_Command_Info1 = new System.Windows.Forms.TabPage();
      this.IAP_TextBox_DiagInfo0 = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Info2 = new System.Windows.Forms.TabPage();
      this.IAP_TextBox_DiagInfo1 = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Write = new System.Windows.Forms.TabPage();
      this.label17 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandWriteBlock_BlSize = new System.Windows.Forms.TextBox();
      this.TextBox_DiagWrite = new System.Windows.Forms.TextBox();
      this.label10 = new System.Windows.Forms.Label();
      this.label11 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandWrite_FileName = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandWrite_FileOffset = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandWrite_Len = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandWrite_Adr = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Read = new System.Windows.Forms.TabPage();
      this.label20 = new System.Windows.Forms.Label();
      this.TextBox_DiagRead = new System.Windows.Forms.TextBox();
      this.label3 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandReadBlock_File = new System.Windows.Forms.TextBox();
      this.label4 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandRead_Len = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandRead_Adr = new System.Windows.Forms.TextBox();
      this.label19 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandReadBlock_BlSize = new System.Windows.Forms.TextBox();
      this.Button_CommandReadBlock_FileSave = new System.Windows.Forms.Button();
      this.TabPage_Command_Checksum = new System.Windows.Forms.TabPage();
      this.IAP_TextBox_DiagChecksum = new System.Windows.Forms.TextBox();
      this.label12 = new System.Windows.Forms.Label();
      this.label16 = new System.Windows.Forms.Label();
      this.label13 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandChk_Len = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandChk_Adr = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Execute = new System.Windows.Forms.TabPage();
      this.label9 = new System.Windows.Forms.Label();
      this.label7 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandExecute_Param4 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandExecute_Param2 = new System.Windows.Forms.TextBox();
      this.label8 = new System.Windows.Forms.Label();
      this.label5 = new System.Windows.Forms.Label();
      this.label6 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandExecute_Param3 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandExecute_Param1 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_CommandExecute_Adr = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Start = new System.Windows.Forms.TabPage();
      this.label18 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandStart_Adr = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Exit = new System.Windows.Forms.TabPage();
      this.TabPage_Command_Enter = new System.Windows.Forms.TabPage();
      this.label22 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandEnter_ID2 = new System.Windows.Forms.TextBox();
      this.label21 = new System.Windows.Forms.Label();
      this.IAP_TextBox_CommandEnter_ID1 = new System.Windows.Forms.TextBox();
      this.IAP_Button_Send = new System.Windows.Forms.Button();
      this.IAP_TextBox_Command0_7 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0_5 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0_8 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0_3 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0_6 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0_4 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0_2 = new System.Windows.Forms.TextBox();
      this.IAP_TextBox_Command0_1 = new System.Windows.Forms.TextBox();
      this.IAP_Text_Data = new System.Windows.Forms.TextBox();
      this.tabPage7 = new System.Windows.Forms.TabPage();
      this.label27 = new System.Windows.Forms.Label();
      this.ComPort_Text_StreamID = new System.Windows.Forms.TextBox();
      this.ComPort_Text_ConIO = new System.Windows.Forms.TextBox();
      this.TextBox_SrcID1 = new System.Windows.Forms.TextBox();
      this.TextBox_SrcID2 = new System.Windows.Forms.TextBox();
      this.TextBox_SrcID3 = new System.Windows.Forms.TextBox();
      this.TextBox_SrcID4 = new System.Windows.Forms.TextBox();
      this.TextBox_DstID4 = new System.Windows.Forms.TextBox();
      this.TextBox_DstID3 = new System.Windows.Forms.TextBox();
      this.TextBox_DstID2 = new System.Windows.Forms.TextBox();
      this.TextBox_DstID1 = new System.Windows.Forms.TextBox();
      this.label23 = new System.Windows.Forms.Label();
      this.label24 = new System.Windows.Forms.Label();
      this.label32 = new System.Windows.Forms.Label();
      this.label33 = new System.Windows.Forms.Label();
      this.TextBox_SrcDeviceAdr = new System.Windows.Forms.TextBox();
      this.TextBox_DestDeviceAdr = new System.Windows.Forms.TextBox();
      this.Session_Label_Blink = new System.Windows.Forms.Label();
      this.IAP_CheckBox_CommandDl_Connect = new System.Windows.Forms.CheckBox();
      this.Tab_MainBotnet.SuspendLayout();
      this.tabPage1.SuspendLayout();
      this.tabPage2.SuspendLayout();
      this.tabPage3.SuspendLayout();
      this.tabPage4.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.groupBox1.SuspendLayout();
      this.tabPage5.SuspendLayout();
      this.tabPage6.SuspendLayout();
      this.IAP_TabControl_Command.SuspendLayout();
      this.TabPage_Command_Download.SuspendLayout();
      this.TabPage_Command_Info1.SuspendLayout();
      this.TabPage_Command_Info2.SuspendLayout();
      this.TabPage_Command_Write.SuspendLayout();
      this.TabPage_Command_Read.SuspendLayout();
      this.TabPage_Command_Checksum.SuspendLayout();
      this.TabPage_Command_Execute.SuspendLayout();
      this.TabPage_Command_Start.SuspendLayout();
      this.TabPage_Command_Enter.SuspendLayout();
      this.tabPage7.SuspendLayout();
      this.SuspendLayout();
      // 
      // Btn_Ports
      // 
      this.Btn_Ports.Location = new System.Drawing.Point(12, 12);
      this.Btn_Ports.Name = "Btn_Ports";
      this.Btn_Ports.Size = new System.Drawing.Size(81, 25);
      this.Btn_Ports.TabIndex = 0;
      this.Btn_Ports.Text = "Ports";
      this.Btn_Ports.UseVisualStyleBackColor = true;
      this.Btn_Ports.Click += new System.EventHandler(this.Btn_Ports_Click);
      // 
      // Btn_PortState
      // 
      this.Btn_PortState.Location = new System.Drawing.Point(99, 12);
      this.Btn_PortState.Name = "Btn_PortState";
      this.Btn_PortState.Size = new System.Drawing.Size(81, 25);
      this.Btn_PortState.TabIndex = 0;
      this.Btn_PortState.Text = "---";
      this.Btn_PortState.UseVisualStyleBackColor = true;
      this.Btn_PortState.Click += new System.EventHandler(this.Btn_Open_Click);
      // 
      // ComboBox_Ports
      // 
      this.ComboBox_Ports.FormattingEnabled = true;
      this.ComboBox_Ports.Location = new System.Drawing.Point(13, 43);
      this.ComboBox_Ports.Name = "ComboBox_Ports";
      this.ComboBox_Ports.Size = new System.Drawing.Size(80, 21);
      this.ComboBox_Ports.TabIndex = 1;
      this.ComboBox_Ports.SelectedIndexChanged += new System.EventHandler(this.ComboBox_Ports_SelectedIndexChanged);
      // 
      // ComboBox_Baud
      // 
      this.ComboBox_Baud.FormattingEnabled = true;
      this.ComboBox_Baud.Location = new System.Drawing.Point(13, 72);
      this.ComboBox_Baud.Name = "ComboBox_Baud";
      this.ComboBox_Baud.Size = new System.Drawing.Size(80, 21);
      this.ComboBox_Baud.TabIndex = 1;
      // 
      // ComboBox_DataBits
      // 
      this.ComboBox_DataBits.FormattingEnabled = true;
      this.ComboBox_DataBits.Location = new System.Drawing.Point(13, 98);
      this.ComboBox_DataBits.Name = "ComboBox_DataBits";
      this.ComboBox_DataBits.Size = new System.Drawing.Size(80, 21);
      this.ComboBox_DataBits.TabIndex = 1;
      // 
      // ComboBox_StopBits
      // 
      this.ComboBox_StopBits.FormattingEnabled = true;
      this.ComboBox_StopBits.Location = new System.Drawing.Point(99, 44);
      this.ComboBox_StopBits.Name = "ComboBox_StopBits";
      this.ComboBox_StopBits.Size = new System.Drawing.Size(80, 21);
      this.ComboBox_StopBits.TabIndex = 1;
      // 
      // ComboBox_Parity
      // 
      this.ComboBox_Parity.FormattingEnabled = true;
      this.ComboBox_Parity.Location = new System.Drawing.Point(99, 72);
      this.ComboBox_Parity.Name = "ComboBox_Parity";
      this.ComboBox_Parity.Size = new System.Drawing.Size(80, 21);
      this.ComboBox_Parity.TabIndex = 1;
      // 
      // ComboBox_HandShaking
      // 
      this.ComboBox_HandShaking.FormattingEnabled = true;
      this.ComboBox_HandShaking.Location = new System.Drawing.Point(99, 98);
      this.ComboBox_HandShaking.Name = "ComboBox_HandShaking";
      this.ComboBox_HandShaking.Size = new System.Drawing.Size(80, 21);
      this.ComboBox_HandShaking.TabIndex = 1;
      // 
      // Label_CTSStatus
      // 
      this.Label_CTSStatus.AutoSize = true;
      this.Label_CTSStatus.Location = new System.Drawing.Point(514, 13);
      this.Label_CTSStatus.Name = "Label_CTSStatus";
      this.Label_CTSStatus.Size = new System.Drawing.Size(28, 13);
      this.Label_CTSStatus.TabIndex = 3;
      this.Label_CTSStatus.Text = "CTS";
      // 
      // Lable_DSRStatus
      // 
      this.Lable_DSRStatus.AutoSize = true;
      this.Lable_DSRStatus.Location = new System.Drawing.Point(564, 13);
      this.Lable_DSRStatus.Name = "Lable_DSRStatus";
      this.Lable_DSRStatus.Size = new System.Drawing.Size(30, 13);
      this.Lable_DSRStatus.TabIndex = 3;
      this.Lable_DSRStatus.Text = "DSR";
      // 
      // Label_RIStatus
      // 
      this.Label_RIStatus.AutoSize = true;
      this.Label_RIStatus.Location = new System.Drawing.Point(605, 13);
      this.Label_RIStatus.Name = "Label_RIStatus";
      this.Label_RIStatus.Size = new System.Drawing.Size(18, 13);
      this.Label_RIStatus.TabIndex = 3;
      this.Label_RIStatus.Text = "RI";
      // 
      // Label_BreakStatus
      // 
      this.Label_BreakStatus.AutoSize = true;
      this.Label_BreakStatus.Location = new System.Drawing.Point(646, 13);
      this.Label_BreakStatus.Name = "Label_BreakStatus";
      this.Label_BreakStatus.Size = new System.Drawing.Size(29, 13);
      this.Label_BreakStatus.TabIndex = 3;
      this.Label_BreakStatus.Text = "BRK";
      // 
      // TextError
      // 
      this.TextError.Location = new System.Drawing.Point(186, 10);
      this.TextError.Multiline = true;
      this.TextError.Name = "TextError";
      this.TextError.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.TextError.Size = new System.Drawing.Size(322, 109);
      this.TextError.TabIndex = 2;
      // 
      // TextBox_CalcHex
      // 
      this.TextBox_CalcHex.Location = new System.Drawing.Point(515, 99);
      this.TextBox_CalcHex.Name = "TextBox_CalcHex";
      this.TextBox_CalcHex.Size = new System.Drawing.Size(100, 20);
      this.TextBox_CalcHex.TabIndex = 8;
      this.TextBox_CalcHex.Text = "0";
      // 
      // TextBox_CalcDez
      // 
      this.TextBox_CalcDez.Location = new System.Drawing.Point(515, 73);
      this.TextBox_CalcDez.Name = "TextBox_CalcDez";
      this.TextBox_CalcDez.Size = new System.Drawing.Size(100, 20);
      this.TextBox_CalcDez.TabIndex = 7;
      this.TextBox_CalcDez.Text = "0";
      // 
      // Lbl_InputCnt
      // 
      this.Lbl_InputCnt.AutoSize = true;
      this.Lbl_InputCnt.Location = new System.Drawing.Point(681, 13);
      this.Lbl_InputCnt.Name = "Lbl_InputCnt";
      this.Lbl_InputCnt.Size = new System.Drawing.Size(13, 13);
      this.Lbl_InputCnt.TabIndex = 12;
      this.Lbl_InputCnt.Text = "0";
      // 
      // Tab_MainBotnet
      // 
      this.Tab_MainBotnet.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.Tab_MainBotnet.Controls.Add(this.tabPage1);
      this.Tab_MainBotnet.Controls.Add(this.tabPage2);
      this.Tab_MainBotnet.Controls.Add(this.tabPage3);
      this.Tab_MainBotnet.Controls.Add(this.tabPage4);
      this.Tab_MainBotnet.Controls.Add(this.tabPage5);
      this.Tab_MainBotnet.Controls.Add(this.tabPage6);
      this.Tab_MainBotnet.Controls.Add(this.tabPage7);
      this.Tab_MainBotnet.Enabled = false;
      this.Tab_MainBotnet.Location = new System.Drawing.Point(13, 125);
      this.Tab_MainBotnet.Name = "Tab_MainBotnet";
      this.Tab_MainBotnet.SelectedIndex = 0;
      this.Tab_MainBotnet.Size = new System.Drawing.Size(863, 548);
      this.Tab_MainBotnet.TabIndex = 13;
      this.Tab_MainBotnet.SelectedIndexChanged += new System.EventHandler(this.Tab_MainBotnet_SelectedIndexChanged);
      // 
      // tabPage1
      // 
      this.tabPage1.Controls.Add(this.Com_Text_ConIO);
      this.tabPage1.Location = new System.Drawing.Point(4, 22);
      this.tabPage1.Name = "tabPage1";
      this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage1.Size = new System.Drawing.Size(855, 522);
      this.tabPage1.TabIndex = 0;
      this.tabPage1.Text = "Com";
      this.tabPage1.UseVisualStyleBackColor = true;
      // 
      // Com_Text_ConIO
      // 
      this.Com_Text_ConIO.Dock = System.Windows.Forms.DockStyle.Fill;
      this.Com_Text_ConIO.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Com_Text_ConIO.Location = new System.Drawing.Point(3, 3);
      this.Com_Text_ConIO.MaxLength = 0;
      this.Com_Text_ConIO.Multiline = true;
      this.Com_Text_ConIO.Name = "Com_Text_ConIO";
      this.Com_Text_ConIO.ScrollBars = System.Windows.Forms.ScrollBars.Both;
      this.Com_Text_ConIO.Size = new System.Drawing.Size(849, 516);
      this.Com_Text_ConIO.TabIndex = 15;
      this.Com_Text_ConIO.WordWrap = false;
      // 
      // tabPage2
      // 
      this.tabPage2.Controls.Add(this.MsgTrace_Text_ConIO);
      this.tabPage2.Location = new System.Drawing.Point(4, 22);
      this.tabPage2.Name = "tabPage2";
      this.tabPage2.Size = new System.Drawing.Size(855, 522);
      this.tabPage2.TabIndex = 5;
      this.tabPage2.Text = "MsgTrace";
      this.tabPage2.UseVisualStyleBackColor = true;
      // 
      // MsgTrace_Text_ConIO
      // 
      this.MsgTrace_Text_ConIO.Dock = System.Windows.Forms.DockStyle.Fill;
      this.MsgTrace_Text_ConIO.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.MsgTrace_Text_ConIO.Location = new System.Drawing.Point(0, 0);
      this.MsgTrace_Text_ConIO.MaxLength = 1000;
      this.MsgTrace_Text_ConIO.Multiline = true;
      this.MsgTrace_Text_ConIO.Name = "MsgTrace_Text_ConIO";
      this.MsgTrace_Text_ConIO.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.MsgTrace_Text_ConIO.Size = new System.Drawing.Size(855, 522);
      this.MsgTrace_Text_ConIO.TabIndex = 16;
      // 
      // tabPage3
      // 
      this.tabPage3.Controls.Add(this.Session_Text_ConIO);
      this.tabPage3.Controls.Add(this.Session_Btn_Stop);
      this.tabPage3.Controls.Add(this.Session_Btn_Start);
      this.tabPage3.Location = new System.Drawing.Point(4, 22);
      this.tabPage3.Name = "tabPage3";
      this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage3.Size = new System.Drawing.Size(855, 522);
      this.tabPage3.TabIndex = 6;
      this.tabPage3.Text = "Session";
      this.tabPage3.UseVisualStyleBackColor = true;
      // 
      // Session_Text_ConIO
      // 
      this.Session_Text_ConIO.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.Session_Text_ConIO.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Session_Text_ConIO.Location = new System.Drawing.Point(127, 3);
      this.Session_Text_ConIO.MaxLength = 1000;
      this.Session_Text_ConIO.Multiline = true;
      this.Session_Text_ConIO.Name = "Session_Text_ConIO";
      this.Session_Text_ConIO.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.Session_Text_ConIO.Size = new System.Drawing.Size(728, 418);
      this.Session_Text_ConIO.TabIndex = 17;
      // 
      // Session_Btn_Stop
      // 
      this.Session_Btn_Stop.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.Session_Btn_Stop.Location = new System.Drawing.Point(6, 32);
      this.Session_Btn_Stop.Name = "Session_Btn_Stop";
      this.Session_Btn_Stop.Size = new System.Drawing.Size(102, 20);
      this.Session_Btn_Stop.TabIndex = 15;
      this.Session_Btn_Stop.Text = "Stop";
      this.Session_Btn_Stop.UseVisualStyleBackColor = true;
      this.Session_Btn_Stop.Click += new System.EventHandler(this.Session_Btn_Stop_Click);
      // 
      // Session_Btn_Start
      // 
      this.Session_Btn_Start.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.Session_Btn_Start.Location = new System.Drawing.Point(6, 6);
      this.Session_Btn_Start.Name = "Session_Btn_Start";
      this.Session_Btn_Start.Size = new System.Drawing.Size(102, 20);
      this.Session_Btn_Start.TabIndex = 14;
      this.Session_Btn_Start.Text = "Start";
      this.Session_Btn_Start.UseVisualStyleBackColor = true;
      this.Session_Btn_Start.Click += new System.EventHandler(this.Session_Btn_Start_Click);
      // 
      // tabPage4
      // 
      this.tabPage4.Controls.Add(this.label31);
      this.tabPage4.Controls.Add(this.MsgTest_Text_Payload);
      this.tabPage4.Controls.Add(this.groupBox2);
      this.tabPage4.Controls.Add(this.MsgTest_Btn_Send);
      this.tabPage4.Controls.Add(this.MsgTest_Text_Data);
      this.tabPage4.Controls.Add(this.groupBox1);
      this.tabPage4.Location = new System.Drawing.Point(4, 22);
      this.tabPage4.Name = "tabPage4";
      this.tabPage4.Size = new System.Drawing.Size(855, 522);
      this.tabPage4.TabIndex = 4;
      this.tabPage4.Text = "MsgTest";
      this.tabPage4.UseVisualStyleBackColor = true;
      // 
      // label31
      // 
      this.label31.AutoSize = true;
      this.label31.Location = new System.Drawing.Point(-3, 107);
      this.label31.Name = "label31";
      this.label31.Size = new System.Drawing.Size(45, 13);
      this.label31.TabIndex = 23;
      this.label31.Text = "Payload";
      // 
      // MsgTest_Text_Payload
      // 
      this.MsgTest_Text_Payload.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.MsgTest_Text_Payload.Location = new System.Drawing.Point(0, 123);
      this.MsgTest_Text_Payload.Name = "MsgTest_Text_Payload";
      this.MsgTest_Text_Payload.Size = new System.Drawing.Size(765, 20);
      this.MsgTest_Text_Payload.TabIndex = 15;
      this.MsgTest_Text_Payload.Text = "0";
      this.MsgTest_Text_Payload.TextChanged += new System.EventHandler(this.MsgTest_Text_Payload_TextChanged);
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.MsgTest_Combo_Stream_Cmd);
      this.groupBox2.Controls.Add(this.MsgTest_Text_Stream_Para3);
      this.groupBox2.Controls.Add(this.MsgTest_Text_Stream_Para2);
      this.groupBox2.Controls.Add(this.MsgTest_Text_Stream_Para1);
      this.groupBox2.Controls.Add(this.MsgTest_RadioButton_Stream_Cmd);
      this.groupBox2.Controls.Add(this.MsgTest_RadioButton_Stream_Data);
      this.groupBox2.Controls.Add(this.label29);
      this.groupBox2.Controls.Add(this.MsgTest_Text_Stream_Idx);
      this.groupBox2.Location = new System.Drawing.Point(146, 3);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(452, 99);
      this.groupBox2.TabIndex = 14;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Stream_Msg";
      // 
      // MsgTest_Combo_Stream_Cmd
      // 
      this.MsgTest_Combo_Stream_Cmd.FormattingEnabled = true;
      this.MsgTest_Combo_Stream_Cmd.Items.AddRange(new object[] {
            "1: Re-Init Buffer",
            "2: Connect",
            "3: Disconnect"});
      this.MsgTest_Combo_Stream_Cmd.Location = new System.Drawing.Point(94, 69);
      this.MsgTest_Combo_Stream_Cmd.Name = "MsgTest_Combo_Stream_Cmd";
      this.MsgTest_Combo_Stream_Cmd.Size = new System.Drawing.Size(114, 21);
      this.MsgTest_Combo_Stream_Cmd.TabIndex = 30;
      this.MsgTest_Combo_Stream_Cmd.SelectedIndexChanged += new System.EventHandler(this.MsgTest_Combo_Stream_Cmd_SelectedIndexChanged);
      // 
      // MsgTest_Text_Stream_Para3
      // 
      this.MsgTest_Text_Stream_Para3.Location = new System.Drawing.Point(322, 70);
      this.MsgTest_Text_Stream_Para3.Name = "MsgTest_Text_Stream_Para3";
      this.MsgTest_Text_Stream_Para3.Size = new System.Drawing.Size(48, 20);
      this.MsgTest_Text_Stream_Para3.TabIndex = 29;
      this.MsgTest_Text_Stream_Para3.Text = "0";
      this.MsgTest_Text_Stream_Para3.TextChanged += new System.EventHandler(this.MsgTest_Text_Stream_Para3_TextChanged);
      // 
      // MsgTest_Text_Stream_Para2
      // 
      this.MsgTest_Text_Stream_Para2.Location = new System.Drawing.Point(268, 70);
      this.MsgTest_Text_Stream_Para2.Name = "MsgTest_Text_Stream_Para2";
      this.MsgTest_Text_Stream_Para2.Size = new System.Drawing.Size(48, 20);
      this.MsgTest_Text_Stream_Para2.TabIndex = 28;
      this.MsgTest_Text_Stream_Para2.Text = "0";
      this.MsgTest_Text_Stream_Para2.TextChanged += new System.EventHandler(this.MsgTest_Text_Stream_Para2_TextChanged);
      // 
      // MsgTest_Text_Stream_Para1
      // 
      this.MsgTest_Text_Stream_Para1.Location = new System.Drawing.Point(214, 69);
      this.MsgTest_Text_Stream_Para1.Name = "MsgTest_Text_Stream_Para1";
      this.MsgTest_Text_Stream_Para1.Size = new System.Drawing.Size(48, 20);
      this.MsgTest_Text_Stream_Para1.TabIndex = 27;
      this.MsgTest_Text_Stream_Para1.Text = "0";
      this.MsgTest_Text_Stream_Para1.TextChanged += new System.EventHandler(this.MsgTest_Text_Stream_Para1_TextChanged);
      // 
      // MsgTest_RadioButton_Stream_Cmd
      // 
      this.MsgTest_RadioButton_Stream_Cmd.AutoSize = true;
      this.MsgTest_RadioButton_Stream_Cmd.Location = new System.Drawing.Point(16, 71);
      this.MsgTest_RadioButton_Stream_Cmd.Name = "MsgTest_RadioButton_Stream_Cmd";
      this.MsgTest_RadioButton_Stream_Cmd.Size = new System.Drawing.Size(72, 17);
      this.MsgTest_RadioButton_Stream_Cmd.TabIndex = 26;
      this.MsgTest_RadioButton_Stream_Cmd.Text = "Command";
      this.MsgTest_RadioButton_Stream_Cmd.UseVisualStyleBackColor = true;
      this.MsgTest_RadioButton_Stream_Cmd.CheckedChanged += new System.EventHandler(this.MsgTest_RadioButton_Stream_Cmd_CheckedChanged);
      // 
      // MsgTest_RadioButton_Stream_Data
      // 
      this.MsgTest_RadioButton_Stream_Data.AutoSize = true;
      this.MsgTest_RadioButton_Stream_Data.Checked = true;
      this.MsgTest_RadioButton_Stream_Data.Location = new System.Drawing.Point(16, 48);
      this.MsgTest_RadioButton_Stream_Data.Name = "MsgTest_RadioButton_Stream_Data";
      this.MsgTest_RadioButton_Stream_Data.Size = new System.Drawing.Size(48, 17);
      this.MsgTest_RadioButton_Stream_Data.TabIndex = 25;
      this.MsgTest_RadioButton_Stream_Data.TabStop = true;
      this.MsgTest_RadioButton_Stream_Data.Text = "Data";
      this.MsgTest_RadioButton_Stream_Data.UseVisualStyleBackColor = true;
      this.MsgTest_RadioButton_Stream_Data.CheckedChanged += new System.EventHandler(this.MsgTest_RadioButton_Stream_Data_CheckedChanged);
      // 
      // label29
      // 
      this.label29.AutoSize = true;
      this.label29.Location = new System.Drawing.Point(10, 23);
      this.label29.Name = "label29";
      this.label29.Size = new System.Drawing.Size(54, 13);
      this.label29.TabIndex = 22;
      this.label29.Text = "StreamIdx";
      // 
      // MsgTest_Text_Stream_Idx
      // 
      this.MsgTest_Text_Stream_Idx.Location = new System.Drawing.Point(70, 20);
      this.MsgTest_Text_Stream_Idx.Name = "MsgTest_Text_Stream_Idx";
      this.MsgTest_Text_Stream_Idx.Size = new System.Drawing.Size(37, 20);
      this.MsgTest_Text_Stream_Idx.TabIndex = 21;
      this.MsgTest_Text_Stream_Idx.Text = "0";
      this.MsgTest_Text_Stream_Idx.TextChanged += new System.EventHandler(this.MsgTest_Text_Stream_Idx_TextChanged);
      // 
      // MsgTest_Btn_Send
      // 
      this.MsgTest_Btn_Send.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.MsgTest_Btn_Send.Location = new System.Drawing.Point(771, 499);
      this.MsgTest_Btn_Send.Name = "MsgTest_Btn_Send";
      this.MsgTest_Btn_Send.Size = new System.Drawing.Size(81, 20);
      this.MsgTest_Btn_Send.TabIndex = 13;
      this.MsgTest_Btn_Send.Text = "Send";
      this.MsgTest_Btn_Send.UseVisualStyleBackColor = true;
      this.MsgTest_Btn_Send.Click += new System.EventHandler(this.MsgTest_Btn_Send_Click);
      // 
      // MsgTest_Text_Data
      // 
      this.MsgTest_Text_Data.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.MsgTest_Text_Data.Location = new System.Drawing.Point(3, 499);
      this.MsgTest_Text_Data.Name = "MsgTest_Text_Data";
      this.MsgTest_Text_Data.Size = new System.Drawing.Size(765, 20);
      this.MsgTest_Text_Data.TabIndex = 9;
      this.MsgTest_Text_Data.Text = "0";
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.label28);
      this.groupBox1.Controls.Add(this.MsgTest_Text_Msg_ID);
      this.groupBox1.Location = new System.Drawing.Point(2, 3);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(145, 99);
      this.groupBox1.TabIndex = 0;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Msg_Prot";
      // 
      // label28
      // 
      this.label28.AutoSize = true;
      this.label28.Location = new System.Drawing.Point(17, 26);
      this.label28.Name = "label28";
      this.label28.Size = new System.Drawing.Size(18, 13);
      this.label28.TabIndex = 22;
      this.label28.Text = "ID";
      // 
      // MsgTest_Text_Msg_ID
      // 
      this.MsgTest_Text_Msg_ID.Location = new System.Drawing.Point(41, 23);
      this.MsgTest_Text_Msg_ID.Name = "MsgTest_Text_Msg_ID";
      this.MsgTest_Text_Msg_ID.Size = new System.Drawing.Size(50, 20);
      this.MsgTest_Text_Msg_ID.TabIndex = 21;
      this.MsgTest_Text_Msg_ID.Text = "0";
      this.MsgTest_Text_Msg_ID.TextChanged += new System.EventHandler(this.MsgTest_Text_Msg_ID_TextChanged);
      // 
      // tabPage5
      // 
      this.tabPage5.Controls.Add(this.Cmd_Combo_CmdToSend);
      this.tabPage5.Controls.Add(this.Cmd_Btn_Send);
      this.tabPage5.Controls.Add(this.label30);
      this.tabPage5.Controls.Add(this.label25);
      this.tabPage5.Controls.Add(this.Cmd_Text_StreamID);
      this.tabPage5.Controls.Add(this.Cmd_Text_ConIo);
      this.tabPage5.Location = new System.Drawing.Point(4, 22);
      this.tabPage5.Name = "tabPage5";
      this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage5.Size = new System.Drawing.Size(855, 522);
      this.tabPage5.TabIndex = 2;
      this.tabPage5.Text = "Cmd";
      this.tabPage5.UseVisualStyleBackColor = true;
      // 
      // Cmd_Combo_CmdToSend
      // 
      this.Cmd_Combo_CmdToSend.FormattingEnabled = true;
      this.Cmd_Combo_CmdToSend.Items.AddRange(new object[] {
            "help",
            "ver",
            "ports",
            "links"});
      this.Cmd_Combo_CmdToSend.Location = new System.Drawing.Point(40, 3);
      this.Cmd_Combo_CmdToSend.Name = "Cmd_Combo_CmdToSend";
      this.Cmd_Combo_CmdToSend.Size = new System.Drawing.Size(399, 21);
      this.Cmd_Combo_CmdToSend.TabIndex = 26;
      this.Cmd_Combo_CmdToSend.Text = "help";
      this.Cmd_Combo_CmdToSend.KeyUp += new System.Windows.Forms.KeyEventHandler(this.Cmd_Combo_CmdToSend_KeyUp);
      // 
      // Cmd_Btn_Send
      // 
      this.Cmd_Btn_Send.Location = new System.Drawing.Point(445, 1);
      this.Cmd_Btn_Send.Name = "Cmd_Btn_Send";
      this.Cmd_Btn_Send.Size = new System.Drawing.Size(66, 23);
      this.Cmd_Btn_Send.TabIndex = 25;
      this.Cmd_Btn_Send.Text = "Send";
      this.Cmd_Btn_Send.UseVisualStyleBackColor = true;
      this.Cmd_Btn_Send.Click += new System.EventHandler(this.Cmd_Btn_Send_Click);
      // 
      // label30
      // 
      this.label30.AutoSize = true;
      this.label30.Location = new System.Drawing.Point(6, 7);
      this.label30.Name = "label30";
      this.label30.Size = new System.Drawing.Size(28, 13);
      this.label30.TabIndex = 24;
      this.label30.Text = "Cmd";
      // 
      // label25
      // 
      this.label25.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label25.AutoSize = true;
      this.label25.Location = new System.Drawing.Point(695, 3);
      this.label25.Name = "label25";
      this.label25.Size = new System.Drawing.Size(54, 13);
      this.label25.TabIndex = 21;
      this.label25.Text = "Stream ID";
      // 
      // Cmd_Text_StreamID
      // 
      this.Cmd_Text_StreamID.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.Cmd_Text_StreamID.Enabled = false;
      this.Cmd_Text_StreamID.Location = new System.Drawing.Point(755, 0);
      this.Cmd_Text_StreamID.Name = "Cmd_Text_StreamID";
      this.Cmd_Text_StreamID.Size = new System.Drawing.Size(100, 20);
      this.Cmd_Text_StreamID.TabIndex = 17;
      this.Cmd_Text_StreamID.Text = "0";
      // 
      // Cmd_Text_ConIo
      // 
      this.Cmd_Text_ConIo.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.Cmd_Text_ConIo.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Cmd_Text_ConIo.Location = new System.Drawing.Point(3, 30);
      this.Cmd_Text_ConIo.Multiline = true;
      this.Cmd_Text_ConIo.Name = "Cmd_Text_ConIo";
      this.Cmd_Text_ConIo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.Cmd_Text_ConIo.Size = new System.Drawing.Size(852, 489);
      this.Cmd_Text_ConIo.TabIndex = 16;
      // 
      // tabPage6
      // 
      this.tabPage6.Controls.Add(this.label26);
      this.tabPage6.Controls.Add(this.IAP_Text_SteamID);
      this.tabPage6.Controls.Add(this.label15);
      this.tabPage6.Controls.Add(this.label14);
      this.tabPage6.Controls.Add(this.IAP_Button_Clear_TextData);
      this.tabPage6.Controls.Add(this.IAP_TextBox_DiagWorkRAM_Size_Hex);
      this.tabPage6.Controls.Add(this.IAP_TextBox_DiagWorkRAM_Adr_Hex);
      this.tabPage6.Controls.Add(this.IAP_TextBox_DiagWorkRAM_Size_Dez);
      this.tabPage6.Controls.Add(this.IAP_TextBox_DiagWorkRAM_Adr_Dez);
      this.tabPage6.Controls.Add(this.IAP_TabControl_Command);
      this.tabPage6.Controls.Add(this.IAP_Button_Send);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_7);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_5);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_8);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_3);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_6);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_4);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_2);
      this.tabPage6.Controls.Add(this.IAP_TextBox_Command0_1);
      this.tabPage6.Controls.Add(this.IAP_Text_Data);
      this.tabPage6.Location = new System.Drawing.Point(4, 22);
      this.tabPage6.Name = "tabPage6";
      this.tabPage6.Padding = new System.Windows.Forms.Padding(3);
      this.tabPage6.Size = new System.Drawing.Size(855, 522);
      this.tabPage6.TabIndex = 1;
      this.tabPage6.Text = "IAP";
      this.tabPage6.UseVisualStyleBackColor = true;
      // 
      // label26
      // 
      this.label26.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label26.AutoSize = true;
      this.label26.Location = new System.Drawing.Point(695, 3);
      this.label26.Name = "label26";
      this.label26.Size = new System.Drawing.Size(54, 13);
      this.label26.TabIndex = 32;
      this.label26.Text = "Stream ID";
      // 
      // IAP_Text_SteamID
      // 
      this.IAP_Text_SteamID.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_Text_SteamID.Location = new System.Drawing.Point(755, 0);
      this.IAP_Text_SteamID.Name = "IAP_Text_SteamID";
      this.IAP_Text_SteamID.Size = new System.Drawing.Size(100, 20);
      this.IAP_Text_SteamID.TabIndex = 31;
      this.IAP_Text_SteamID.Text = "0";
      // 
      // label15
      // 
      this.label15.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label15.AutoSize = true;
      this.label15.Location = new System.Drawing.Point(766, 188);
      this.label15.Name = "label15";
      this.label15.Size = new System.Drawing.Size(83, 13);
      this.label15.TabIndex = 30;
      this.label15.Text = "Work RAM Size";
      // 
      // label14
      // 
      this.label14.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label14.AutoSize = true;
      this.label14.Location = new System.Drawing.Point(680, 188);
      this.label14.Name = "label14";
      this.label14.Size = new System.Drawing.Size(79, 13);
      this.label14.TabIndex = 29;
      this.label14.Text = "Work RAM Adr";
      // 
      // IAP_Button_Clear_TextData
      // 
      this.IAP_Button_Clear_TextData.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_Button_Clear_TextData.Location = new System.Drawing.Point(813, 289);
      this.IAP_Button_Clear_TextData.Name = "IAP_Button_Clear_TextData";
      this.IAP_Button_Clear_TextData.Size = new System.Drawing.Size(36, 19);
      this.IAP_Button_Clear_TextData.TabIndex = 28;
      this.IAP_Button_Clear_TextData.Text = "Clr";
      this.IAP_Button_Clear_TextData.UseVisualStyleBackColor = true;
      this.IAP_Button_Clear_TextData.Click += new System.EventHandler(this.IAP_Button_Clear_TextData_Click);
      // 
      // IAP_TextBox_DiagWorkRAM_Size_Hex
      // 
      this.IAP_TextBox_DiagWorkRAM_Size_Hex.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_DiagWorkRAM_Size_Hex.Location = new System.Drawing.Point(769, 230);
      this.IAP_TextBox_DiagWorkRAM_Size_Hex.Name = "IAP_TextBox_DiagWorkRAM_Size_Hex";
      this.IAP_TextBox_DiagWorkRAM_Size_Hex.Size = new System.Drawing.Size(80, 20);
      this.IAP_TextBox_DiagWorkRAM_Size_Hex.TabIndex = 26;
      this.IAP_TextBox_DiagWorkRAM_Size_Hex.Text = "---";
      // 
      // IAP_TextBox_DiagWorkRAM_Adr_Hex
      // 
      this.IAP_TextBox_DiagWorkRAM_Adr_Hex.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_DiagWorkRAM_Adr_Hex.Location = new System.Drawing.Point(683, 230);
      this.IAP_TextBox_DiagWorkRAM_Adr_Hex.Name = "IAP_TextBox_DiagWorkRAM_Adr_Hex";
      this.IAP_TextBox_DiagWorkRAM_Adr_Hex.Size = new System.Drawing.Size(80, 20);
      this.IAP_TextBox_DiagWorkRAM_Adr_Hex.TabIndex = 27;
      this.IAP_TextBox_DiagWorkRAM_Adr_Hex.Text = "---";
      // 
      // IAP_TextBox_DiagWorkRAM_Size_Dez
      // 
      this.IAP_TextBox_DiagWorkRAM_Size_Dez.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_DiagWorkRAM_Size_Dez.Location = new System.Drawing.Point(769, 204);
      this.IAP_TextBox_DiagWorkRAM_Size_Dez.Name = "IAP_TextBox_DiagWorkRAM_Size_Dez";
      this.IAP_TextBox_DiagWorkRAM_Size_Dez.Size = new System.Drawing.Size(80, 20);
      this.IAP_TextBox_DiagWorkRAM_Size_Dez.TabIndex = 24;
      this.IAP_TextBox_DiagWorkRAM_Size_Dez.Text = "---";
      // 
      // IAP_TextBox_DiagWorkRAM_Adr_Dez
      // 
      this.IAP_TextBox_DiagWorkRAM_Adr_Dez.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_DiagWorkRAM_Adr_Dez.Location = new System.Drawing.Point(683, 204);
      this.IAP_TextBox_DiagWorkRAM_Adr_Dez.Name = "IAP_TextBox_DiagWorkRAM_Adr_Dez";
      this.IAP_TextBox_DiagWorkRAM_Adr_Dez.Size = new System.Drawing.Size(80, 20);
      this.IAP_TextBox_DiagWorkRAM_Adr_Dez.TabIndex = 25;
      this.IAP_TextBox_DiagWorkRAM_Adr_Dez.Text = "---";
      // 
      // IAP_TabControl_Command
      // 
      this.IAP_TabControl_Command.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Download);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Info1);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Info2);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Write);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Read);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Checksum);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Execute);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Start);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Exit);
      this.IAP_TabControl_Command.Controls.Add(this.TabPage_Command_Enter);
      this.IAP_TabControl_Command.Location = new System.Drawing.Point(6, 6);
      this.IAP_TabControl_Command.Name = "IAP_TabControl_Command";
      this.IAP_TabControl_Command.SelectedIndex = 0;
      this.IAP_TabControl_Command.Size = new System.Drawing.Size(656, 248);
      this.IAP_TabControl_Command.TabIndex = 23;
      this.IAP_TabControl_Command.SelectedIndexChanged += new System.EventHandler(this.IAP_TabControl_Command_SelectedIndexChanged);
      // 
      // TabPage_Command_Download
      // 
      this.TabPage_Command_Download.Controls.Add(this.IAP_CheckBox_CommandDl_Connect);
      this.TabPage_Command_Download.Controls.Add(this.IAP_CheckBox_CommandDl_Reset);
      this.TabPage_Command_Download.Controls.Add(this.label38);
      this.TabPage_Command_Download.Controls.Add(this.label34);
      this.TabPage_Command_Download.Controls.Add(this.IAP_TextBox_CommandDl_Status);
      this.TabPage_Command_Download.Controls.Add(this.IAP_TextBox_CommandDl_BlockLen);
      this.TabPage_Command_Download.Controls.Add(this.IAP_TextBox_CommandDl_Adr);
      this.TabPage_Command_Download.Controls.Add(this.IAP_TextBox_CommandDl_Len);
      this.TabPage_Command_Download.Controls.Add(this.label36);
      this.TabPage_Command_Download.Controls.Add(this.IAP_TextBox_CommandDl_File);
      this.TabPage_Command_Download.Controls.Add(this.label37);
      this.TabPage_Command_Download.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Download.Name = "TabPage_Command_Download";
      this.TabPage_Command_Download.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Download.TabIndex = 9;
      this.TabPage_Command_Download.Text = "DL";
      this.TabPage_Command_Download.UseVisualStyleBackColor = true;
      // 
      // IAP_CheckBox_CommandDl_Reset
      // 
      this.IAP_CheckBox_CommandDl_Reset.AutoSize = true;
      this.IAP_CheckBox_CommandDl_Reset.Checked = true;
      this.IAP_CheckBox_CommandDl_Reset.CheckState = System.Windows.Forms.CheckState.Checked;
      this.IAP_CheckBox_CommandDl_Reset.Location = new System.Drawing.Point(6, 133);
      this.IAP_CheckBox_CommandDl_Reset.Name = "IAP_CheckBox_CommandDl_Reset";
      this.IAP_CheckBox_CommandDl_Reset.Size = new System.Drawing.Size(95, 17);
      this.IAP_CheckBox_CommandDl_Reset.TabIndex = 37;
      this.IAP_CheckBox_CommandDl_Reset.Text = "Reset after DL";
      this.IAP_CheckBox_CommandDl_Reset.UseVisualStyleBackColor = true;
      // 
      // label38
      // 
      this.label38.AutoSize = true;
      this.label38.Location = new System.Drawing.Point(3, 199);
      this.label38.Name = "label38";
      this.label38.Size = new System.Drawing.Size(23, 13);
      this.label38.TabIndex = 36;
      this.label38.Text = "File";
      // 
      // label34
      // 
      this.label34.AutoSize = true;
      this.label34.Location = new System.Drawing.Point(3, 76);
      this.label34.Name = "label34";
      this.label34.Size = new System.Drawing.Size(42, 13);
      this.label34.TabIndex = 35;
      this.label34.Text = "Bl. Size";
      // 
      // IAP_TextBox_CommandDl_Status
      // 
      this.IAP_TextBox_CommandDl_Status.Font = new System.Drawing.Font("Lucida Console", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IAP_TextBox_CommandDl_Status.Location = new System.Drawing.Point(127, 3);
      this.IAP_TextBox_CommandDl_Status.Multiline = true;
      this.IAP_TextBox_CommandDl_Status.Name = "IAP_TextBox_CommandDl_Status";
      this.IAP_TextBox_CommandDl_Status.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.IAP_TextBox_CommandDl_Status.Size = new System.Drawing.Size(518, 187);
      this.IAP_TextBox_CommandDl_Status.TabIndex = 33;
      // 
      // IAP_TextBox_CommandDl_BlockLen
      // 
      this.IAP_TextBox_CommandDl_BlockLen.Location = new System.Drawing.Point(54, 73);
      this.IAP_TextBox_CommandDl_BlockLen.Name = "IAP_TextBox_CommandDl_BlockLen";
      this.IAP_TextBox_CommandDl_BlockLen.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandDl_BlockLen.TabIndex = 34;
      this.IAP_TextBox_CommandDl_BlockLen.Text = "1024";
      // 
      // IAP_TextBox_CommandDl_Adr
      // 
      this.IAP_TextBox_CommandDl_Adr.Location = new System.Drawing.Point(54, 3);
      this.IAP_TextBox_CommandDl_Adr.Name = "IAP_TextBox_CommandDl_Adr";
      this.IAP_TextBox_CommandDl_Adr.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandDl_Adr.TabIndex = 28;
      this.IAP_TextBox_CommandDl_Adr.Text = "0";
      // 
      // IAP_TextBox_CommandDl_Len
      // 
      this.IAP_TextBox_CommandDl_Len.Location = new System.Drawing.Point(54, 29);
      this.IAP_TextBox_CommandDl_Len.Name = "IAP_TextBox_CommandDl_Len";
      this.IAP_TextBox_CommandDl_Len.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandDl_Len.TabIndex = 29;
      this.IAP_TextBox_CommandDl_Len.Text = "0";
      // 
      // label36
      // 
      this.label36.AutoSize = true;
      this.label36.Location = new System.Drawing.Point(3, 32);
      this.label36.Name = "label36";
      this.label36.Size = new System.Drawing.Size(25, 13);
      this.label36.TabIndex = 32;
      this.label36.Text = "Len";
      // 
      // IAP_TextBox_CommandDl_File
      // 
      this.IAP_TextBox_CommandDl_File.AllowDrop = true;
      this.IAP_TextBox_CommandDl_File.Location = new System.Drawing.Point(30, 196);
      this.IAP_TextBox_CommandDl_File.Name = "IAP_TextBox_CommandDl_File";
      this.IAP_TextBox_CommandDl_File.Size = new System.Drawing.Size(615, 20);
      this.IAP_TextBox_CommandDl_File.TabIndex = 27;
      this.IAP_TextBox_CommandDl_File.DragDrop += new System.Windows.Forms.DragEventHandler(this.IAP_TextBox_CommandDl_File_DragDrop);
      this.IAP_TextBox_CommandDl_File.DragEnter += new System.Windows.Forms.DragEventHandler(this.IAP_TextBox_CommandDl_File_DragEnter);
      // 
      // label37
      // 
      this.label37.AutoSize = true;
      this.label37.Location = new System.Drawing.Point(3, 6);
      this.label37.Name = "label37";
      this.label37.Size = new System.Drawing.Size(45, 13);
      this.label37.TabIndex = 30;
      this.label37.Text = "Adresse";
      // 
      // TabPage_Command_Info1
      // 
      this.TabPage_Command_Info1.Controls.Add(this.IAP_TextBox_DiagInfo0);
      this.TabPage_Command_Info1.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Info1.Name = "TabPage_Command_Info1";
      this.TabPage_Command_Info1.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Info1.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Info1.TabIndex = 0;
      this.TabPage_Command_Info1.Text = "Info";
      this.TabPage_Command_Info1.UseVisualStyleBackColor = true;
      // 
      // IAP_TextBox_DiagInfo0
      // 
      this.IAP_TextBox_DiagInfo0.Dock = System.Windows.Forms.DockStyle.Fill;
      this.IAP_TextBox_DiagInfo0.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IAP_TextBox_DiagInfo0.Location = new System.Drawing.Point(3, 3);
      this.IAP_TextBox_DiagInfo0.Multiline = true;
      this.IAP_TextBox_DiagInfo0.Name = "IAP_TextBox_DiagInfo0";
      this.IAP_TextBox_DiagInfo0.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.IAP_TextBox_DiagInfo0.Size = new System.Drawing.Size(642, 216);
      this.IAP_TextBox_DiagInfo0.TabIndex = 0;
      // 
      // TabPage_Command_Info2
      // 
      this.TabPage_Command_Info2.Controls.Add(this.IAP_TextBox_DiagInfo1);
      this.TabPage_Command_Info2.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Info2.Name = "TabPage_Command_Info2";
      this.TabPage_Command_Info2.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Info2.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Info2.TabIndex = 1;
      this.TabPage_Command_Info2.Text = "Info2";
      this.TabPage_Command_Info2.UseVisualStyleBackColor = true;
      // 
      // IAP_TextBox_DiagInfo1
      // 
      this.IAP_TextBox_DiagInfo1.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IAP_TextBox_DiagInfo1.Location = new System.Drawing.Point(6, 6);
      this.IAP_TextBox_DiagInfo1.Multiline = true;
      this.IAP_TextBox_DiagInfo1.Name = "IAP_TextBox_DiagInfo1";
      this.IAP_TextBox_DiagInfo1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.IAP_TextBox_DiagInfo1.Size = new System.Drawing.Size(636, 210);
      this.IAP_TextBox_DiagInfo1.TabIndex = 1;
      // 
      // TabPage_Command_Write
      // 
      this.TabPage_Command_Write.Controls.Add(this.label17);
      this.TabPage_Command_Write.Controls.Add(this.IAP_TextBox_CommandWriteBlock_BlSize);
      this.TabPage_Command_Write.Controls.Add(this.TextBox_DiagWrite);
      this.TabPage_Command_Write.Controls.Add(this.label10);
      this.TabPage_Command_Write.Controls.Add(this.label11);
      this.TabPage_Command_Write.Controls.Add(this.label2);
      this.TabPage_Command_Write.Controls.Add(this.label1);
      this.TabPage_Command_Write.Controls.Add(this.IAP_TextBox_CommandWrite_FileName);
      this.TabPage_Command_Write.Controls.Add(this.IAP_TextBox_CommandWrite_FileOffset);
      this.TabPage_Command_Write.Controls.Add(this.IAP_TextBox_CommandWrite_Len);
      this.TabPage_Command_Write.Controls.Add(this.IAP_TextBox_CommandWrite_Adr);
      this.TabPage_Command_Write.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Write.Name = "TabPage_Command_Write";
      this.TabPage_Command_Write.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Write.TabIndex = 2;
      this.TabPage_Command_Write.Text = "Write";
      this.TabPage_Command_Write.UseVisualStyleBackColor = true;
      // 
      // label17
      // 
      this.label17.AutoSize = true;
      this.label17.Location = new System.Drawing.Point(3, 79);
      this.label17.Name = "label17";
      this.label17.Size = new System.Drawing.Size(42, 13);
      this.label17.TabIndex = 12;
      this.label17.Text = "Bl. Size";
      // 
      // IAP_TextBox_CommandWriteBlock_BlSize
      // 
      this.IAP_TextBox_CommandWriteBlock_BlSize.Location = new System.Drawing.Point(54, 76);
      this.IAP_TextBox_CommandWriteBlock_BlSize.Name = "IAP_TextBox_CommandWriteBlock_BlSize";
      this.IAP_TextBox_CommandWriteBlock_BlSize.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandWriteBlock_BlSize.TabIndex = 11;
      this.IAP_TextBox_CommandWriteBlock_BlSize.Text = "1024";
      this.IAP_TextBox_CommandWriteBlock_BlSize.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandWriteBlock_BlSize_TextChanged);
      // 
      // TextBox_DiagWrite
      // 
      this.TextBox_DiagWrite.Font = new System.Drawing.Font("Lucida Console", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TextBox_DiagWrite.Location = new System.Drawing.Point(127, 6);
      this.TextBox_DiagWrite.Multiline = true;
      this.TextBox_DiagWrite.Name = "TextBox_DiagWrite";
      this.TextBox_DiagWrite.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.TextBox_DiagWrite.Size = new System.Drawing.Size(518, 187);
      this.TextBox_DiagWrite.TabIndex = 7;
      // 
      // label10
      // 
      this.label10.AutoSize = true;
      this.label10.Location = new System.Drawing.Point(1, 202);
      this.label10.Name = "label10";
      this.label10.Size = new System.Drawing.Size(23, 13);
      this.label10.TabIndex = 1;
      this.label10.Text = "File";
      // 
      // label11
      // 
      this.label11.AutoSize = true;
      this.label11.Location = new System.Drawing.Point(544, 202);
      this.label11.Name = "label11";
      this.label11.Size = new System.Drawing.Size(35, 13);
      this.label11.TabIndex = 1;
      this.label11.Text = "Offset";
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(3, 35);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(25, 13);
      this.label2.TabIndex = 1;
      this.label2.Text = "Len";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Location = new System.Drawing.Point(3, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(45, 13);
      this.label1.TabIndex = 1;
      this.label1.Text = "Adresse";
      // 
      // IAP_TextBox_CommandWrite_FileName
      // 
      this.IAP_TextBox_CommandWrite_FileName.AllowDrop = true;
      this.IAP_TextBox_CommandWrite_FileName.Location = new System.Drawing.Point(30, 199);
      this.IAP_TextBox_CommandWrite_FileName.Name = "IAP_TextBox_CommandWrite_FileName";
      this.IAP_TextBox_CommandWrite_FileName.Size = new System.Drawing.Size(508, 20);
      this.IAP_TextBox_CommandWrite_FileName.TabIndex = 0;
      this.IAP_TextBox_CommandWrite_FileName.DragDrop += new System.Windows.Forms.DragEventHandler(this.IAP_TextBox_CommandWrite_FileName_DragDrop);
      this.IAP_TextBox_CommandWrite_FileName.DragEnter += new System.Windows.Forms.DragEventHandler(this.IAP_TextBox_CommandWrite_FileName_DragEnter);
      // 
      // IAP_TextBox_CommandWrite_FileOffset
      // 
      this.IAP_TextBox_CommandWrite_FileOffset.Location = new System.Drawing.Point(585, 199);
      this.IAP_TextBox_CommandWrite_FileOffset.Name = "IAP_TextBox_CommandWrite_FileOffset";
      this.IAP_TextBox_CommandWrite_FileOffset.Size = new System.Drawing.Size(58, 20);
      this.IAP_TextBox_CommandWrite_FileOffset.TabIndex = 0;
      this.IAP_TextBox_CommandWrite_FileOffset.Text = "0";
      this.IAP_TextBox_CommandWrite_FileOffset.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandWrite_FileOffset_TextChanged);
      // 
      // IAP_TextBox_CommandWrite_Len
      // 
      this.IAP_TextBox_CommandWrite_Len.Location = new System.Drawing.Point(54, 32);
      this.IAP_TextBox_CommandWrite_Len.Name = "IAP_TextBox_CommandWrite_Len";
      this.IAP_TextBox_CommandWrite_Len.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandWrite_Len.TabIndex = 0;
      this.IAP_TextBox_CommandWrite_Len.Text = "0";
      this.IAP_TextBox_CommandWrite_Len.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandWrite_Len_TextChanged);
      // 
      // IAP_TextBox_CommandWrite_Adr
      // 
      this.IAP_TextBox_CommandWrite_Adr.Location = new System.Drawing.Point(54, 6);
      this.IAP_TextBox_CommandWrite_Adr.Name = "IAP_TextBox_CommandWrite_Adr";
      this.IAP_TextBox_CommandWrite_Adr.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandWrite_Adr.TabIndex = 0;
      this.IAP_TextBox_CommandWrite_Adr.Text = "0";
      this.IAP_TextBox_CommandWrite_Adr.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandWrite_Adr_TextChanged);
      // 
      // TabPage_Command_Read
      // 
      this.TabPage_Command_Read.Controls.Add(this.label20);
      this.TabPage_Command_Read.Controls.Add(this.TextBox_DiagRead);
      this.TabPage_Command_Read.Controls.Add(this.label3);
      this.TabPage_Command_Read.Controls.Add(this.IAP_TextBox_CommandReadBlock_File);
      this.TabPage_Command_Read.Controls.Add(this.label4);
      this.TabPage_Command_Read.Controls.Add(this.IAP_TextBox_CommandRead_Len);
      this.TabPage_Command_Read.Controls.Add(this.IAP_TextBox_CommandRead_Adr);
      this.TabPage_Command_Read.Controls.Add(this.label19);
      this.TabPage_Command_Read.Controls.Add(this.IAP_TextBox_CommandReadBlock_BlSize);
      this.TabPage_Command_Read.Controls.Add(this.Button_CommandReadBlock_FileSave);
      this.TabPage_Command_Read.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Read.Name = "TabPage_Command_Read";
      this.TabPage_Command_Read.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Read.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Read.TabIndex = 3;
      this.TabPage_Command_Read.Text = "Read";
      this.TabPage_Command_Read.UseVisualStyleBackColor = true;
      // 
      // label20
      // 
      this.label20.AutoSize = true;
      this.label20.Location = new System.Drawing.Point(2, 205);
      this.label20.Name = "label20";
      this.label20.Size = new System.Drawing.Size(23, 13);
      this.label20.TabIndex = 13;
      this.label20.Text = "File";
      // 
      // TextBox_DiagRead
      // 
      this.TextBox_DiagRead.Font = new System.Drawing.Font("Lucida Console", 6F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TextBox_DiagRead.Location = new System.Drawing.Point(127, 6);
      this.TextBox_DiagRead.Multiline = true;
      this.TextBox_DiagRead.Name = "TextBox_DiagRead";
      this.TextBox_DiagRead.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.TextBox_DiagRead.Size = new System.Drawing.Size(521, 190);
      this.TextBox_DiagRead.TabIndex = 6;
      this.TextBox_DiagRead.DragDrop += new System.Windows.Forms.DragEventHandler(this.IAP_TextBox_CommandReadBlock_File_DragDrop);
      this.TextBox_DiagRead.DragEnter += new System.Windows.Forms.DragEventHandler(this.IAP_TextBox_CommandReadBlock_File_DragEnter);
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(3, 35);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(25, 13);
      this.label3.TabIndex = 4;
      this.label3.Text = "Len";
      // 
      // IAP_TextBox_CommandReadBlock_File
      // 
      this.IAP_TextBox_CommandReadBlock_File.AllowDrop = true;
      this.IAP_TextBox_CommandReadBlock_File.Location = new System.Drawing.Point(31, 202);
      this.IAP_TextBox_CommandReadBlock_File.Name = "IAP_TextBox_CommandReadBlock_File";
      this.IAP_TextBox_CommandReadBlock_File.Size = new System.Drawing.Size(571, 20);
      this.IAP_TextBox_CommandReadBlock_File.TabIndex = 12;
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(3, 9);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(45, 13);
      this.label4.TabIndex = 5;
      this.label4.Text = "Adresse";
      // 
      // IAP_TextBox_CommandRead_Len
      // 
      this.IAP_TextBox_CommandRead_Len.Location = new System.Drawing.Point(54, 32);
      this.IAP_TextBox_CommandRead_Len.Name = "IAP_TextBox_CommandRead_Len";
      this.IAP_TextBox_CommandRead_Len.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandRead_Len.TabIndex = 2;
      this.IAP_TextBox_CommandRead_Len.Text = "2048";
      this.IAP_TextBox_CommandRead_Len.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandRead_Len_TextChanged);
      // 
      // IAP_TextBox_CommandRead_Adr
      // 
      this.IAP_TextBox_CommandRead_Adr.Location = new System.Drawing.Point(54, 6);
      this.IAP_TextBox_CommandRead_Adr.Name = "IAP_TextBox_CommandRead_Adr";
      this.IAP_TextBox_CommandRead_Adr.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandRead_Adr.TabIndex = 3;
      this.IAP_TextBox_CommandRead_Adr.Text = "0x20000000";
      this.IAP_TextBox_CommandRead_Adr.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandRead_Adr_TextChanged);
      // 
      // label19
      // 
      this.label19.AutoSize = true;
      this.label19.Location = new System.Drawing.Point(3, 76);
      this.label19.Name = "label19";
      this.label19.Size = new System.Drawing.Size(42, 13);
      this.label19.TabIndex = 9;
      this.label19.Text = "Bl. Size";
      // 
      // IAP_TextBox_CommandReadBlock_BlSize
      // 
      this.IAP_TextBox_CommandReadBlock_BlSize.Location = new System.Drawing.Point(54, 73);
      this.IAP_TextBox_CommandReadBlock_BlSize.Name = "IAP_TextBox_CommandReadBlock_BlSize";
      this.IAP_TextBox_CommandReadBlock_BlSize.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandReadBlock_BlSize.TabIndex = 7;
      this.IAP_TextBox_CommandReadBlock_BlSize.Text = "2048";
      this.IAP_TextBox_CommandReadBlock_BlSize.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandReadBlock_BlSize_TextChanged);
      // 
      // Button_CommandReadBlock_FileSave
      // 
      this.Button_CommandReadBlock_FileSave.Location = new System.Drawing.Point(608, 202);
      this.Button_CommandReadBlock_FileSave.Name = "Button_CommandReadBlock_FileSave";
      this.Button_CommandReadBlock_FileSave.Size = new System.Drawing.Size(40, 20);
      this.Button_CommandReadBlock_FileSave.TabIndex = 0;
      this.Button_CommandReadBlock_FileSave.Text = "Save";
      this.Button_CommandReadBlock_FileSave.UseVisualStyleBackColor = true;
      this.Button_CommandReadBlock_FileSave.Click += new System.EventHandler(this.IAP_Button_CommandReadBlock_FileSave_Click);
      // 
      // TabPage_Command_Checksum
      // 
      this.TabPage_Command_Checksum.Controls.Add(this.IAP_TextBox_DiagChecksum);
      this.TabPage_Command_Checksum.Controls.Add(this.label12);
      this.TabPage_Command_Checksum.Controls.Add(this.label16);
      this.TabPage_Command_Checksum.Controls.Add(this.label13);
      this.TabPage_Command_Checksum.Controls.Add(this.IAP_TextBox_CommandChk_Len);
      this.TabPage_Command_Checksum.Controls.Add(this.IAP_TextBox_CommandChk_Adr);
      this.TabPage_Command_Checksum.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Checksum.Name = "TabPage_Command_Checksum";
      this.TabPage_Command_Checksum.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Checksum.TabIndex = 6;
      this.TabPage_Command_Checksum.Text = "CheckSum";
      this.TabPage_Command_Checksum.UseVisualStyleBackColor = true;
      // 
      // IAP_TextBox_DiagChecksum
      // 
      this.IAP_TextBox_DiagChecksum.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IAP_TextBox_DiagChecksum.Location = new System.Drawing.Point(428, 6);
      this.IAP_TextBox_DiagChecksum.Name = "IAP_TextBox_DiagChecksum";
      this.IAP_TextBox_DiagChecksum.Size = new System.Drawing.Size(217, 18);
      this.IAP_TextBox_DiagChecksum.TabIndex = 10;
      // 
      // label12
      // 
      this.label12.AutoSize = true;
      this.label12.Location = new System.Drawing.Point(2, 35);
      this.label12.Name = "label12";
      this.label12.Size = new System.Drawing.Size(25, 13);
      this.label12.TabIndex = 8;
      this.label12.Text = "Len";
      // 
      // label16
      // 
      this.label16.AutoSize = true;
      this.label16.Location = new System.Drawing.Point(351, 10);
      this.label16.Name = "label16";
      this.label16.Size = new System.Drawing.Size(71, 13);
      this.label16.TabIndex = 9;
      this.label16.Text = "Checksumme";
      // 
      // label13
      // 
      this.label13.AutoSize = true;
      this.label13.Location = new System.Drawing.Point(2, 9);
      this.label13.Name = "label13";
      this.label13.Size = new System.Drawing.Size(45, 13);
      this.label13.TabIndex = 9;
      this.label13.Text = "Adresse";
      // 
      // IAP_TextBox_CommandChk_Len
      // 
      this.IAP_TextBox_CommandChk_Len.Location = new System.Drawing.Point(53, 32);
      this.IAP_TextBox_CommandChk_Len.Name = "IAP_TextBox_CommandChk_Len";
      this.IAP_TextBox_CommandChk_Len.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandChk_Len.TabIndex = 6;
      this.IAP_TextBox_CommandChk_Len.Text = "0";
      this.IAP_TextBox_CommandChk_Len.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandChk_Len_TextChanged);
      // 
      // IAP_TextBox_CommandChk_Adr
      // 
      this.IAP_TextBox_CommandChk_Adr.Location = new System.Drawing.Point(53, 6);
      this.IAP_TextBox_CommandChk_Adr.Name = "IAP_TextBox_CommandChk_Adr";
      this.IAP_TextBox_CommandChk_Adr.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandChk_Adr.TabIndex = 7;
      this.IAP_TextBox_CommandChk_Adr.Text = "0";
      this.IAP_TextBox_CommandChk_Adr.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandChk_Adr_TextChanged);
      // 
      // TabPage_Command_Execute
      // 
      this.TabPage_Command_Execute.Controls.Add(this.label9);
      this.TabPage_Command_Execute.Controls.Add(this.label7);
      this.TabPage_Command_Execute.Controls.Add(this.IAP_TextBox_CommandExecute_Param4);
      this.TabPage_Command_Execute.Controls.Add(this.IAP_TextBox_CommandExecute_Param2);
      this.TabPage_Command_Execute.Controls.Add(this.label8);
      this.TabPage_Command_Execute.Controls.Add(this.label5);
      this.TabPage_Command_Execute.Controls.Add(this.label6);
      this.TabPage_Command_Execute.Controls.Add(this.IAP_TextBox_CommandExecute_Param3);
      this.TabPage_Command_Execute.Controls.Add(this.IAP_TextBox_CommandExecute_Param1);
      this.TabPage_Command_Execute.Controls.Add(this.IAP_TextBox_CommandExecute_Adr);
      this.TabPage_Command_Execute.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Execute.Name = "TabPage_Command_Execute";
      this.TabPage_Command_Execute.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Execute.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Execute.TabIndex = 4;
      this.TabPage_Command_Execute.Text = "Execute";
      this.TabPage_Command_Execute.UseVisualStyleBackColor = true;
      // 
      // label9
      // 
      this.label9.AutoSize = true;
      this.label9.Location = new System.Drawing.Point(265, 35);
      this.label9.Name = "label9";
      this.label9.Size = new System.Drawing.Size(43, 13);
      this.label9.TabIndex = 11;
      this.label9.Text = "Param4";
      // 
      // label7
      // 
      this.label7.AutoSize = true;
      this.label7.Location = new System.Drawing.Point(136, 35);
      this.label7.Name = "label7";
      this.label7.Size = new System.Drawing.Size(43, 13);
      this.label7.TabIndex = 11;
      this.label7.Text = "Param2";
      // 
      // IAP_TextBox_CommandExecute_Param4
      // 
      this.IAP_TextBox_CommandExecute_Param4.Location = new System.Drawing.Point(314, 32);
      this.IAP_TextBox_CommandExecute_Param4.Name = "IAP_TextBox_CommandExecute_Param4";
      this.IAP_TextBox_CommandExecute_Param4.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandExecute_Param4.TabIndex = 10;
      this.IAP_TextBox_CommandExecute_Param4.Text = "0";
      this.IAP_TextBox_CommandExecute_Param4.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandExecute_Param4_TextChanged);
      // 
      // IAP_TextBox_CommandExecute_Param2
      // 
      this.IAP_TextBox_CommandExecute_Param2.Location = new System.Drawing.Point(185, 32);
      this.IAP_TextBox_CommandExecute_Param2.Name = "IAP_TextBox_CommandExecute_Param2";
      this.IAP_TextBox_CommandExecute_Param2.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandExecute_Param2.TabIndex = 10;
      this.IAP_TextBox_CommandExecute_Param2.Text = "0";
      this.IAP_TextBox_CommandExecute_Param2.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandExecute_Param2_TextChanged);
      // 
      // label8
      // 
      this.label8.AutoSize = true;
      this.label8.Location = new System.Drawing.Point(265, 9);
      this.label8.Name = "label8";
      this.label8.Size = new System.Drawing.Size(43, 13);
      this.label8.TabIndex = 8;
      this.label8.Text = "Param3";
      // 
      // label5
      // 
      this.label5.AutoSize = true;
      this.label5.Location = new System.Drawing.Point(136, 9);
      this.label5.Name = "label5";
      this.label5.Size = new System.Drawing.Size(43, 13);
      this.label5.TabIndex = 8;
      this.label5.Text = "Param1";
      // 
      // label6
      // 
      this.label6.AutoSize = true;
      this.label6.Location = new System.Drawing.Point(6, 9);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size(45, 13);
      this.label6.TabIndex = 9;
      this.label6.Text = "Adresse";
      // 
      // IAP_TextBox_CommandExecute_Param3
      // 
      this.IAP_TextBox_CommandExecute_Param3.Location = new System.Drawing.Point(314, 6);
      this.IAP_TextBox_CommandExecute_Param3.Name = "IAP_TextBox_CommandExecute_Param3";
      this.IAP_TextBox_CommandExecute_Param3.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandExecute_Param3.TabIndex = 6;
      this.IAP_TextBox_CommandExecute_Param3.Text = "0";
      this.IAP_TextBox_CommandExecute_Param3.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandExecute_Param3_TextChanged);
      // 
      // IAP_TextBox_CommandExecute_Param1
      // 
      this.IAP_TextBox_CommandExecute_Param1.Location = new System.Drawing.Point(185, 6);
      this.IAP_TextBox_CommandExecute_Param1.Name = "IAP_TextBox_CommandExecute_Param1";
      this.IAP_TextBox_CommandExecute_Param1.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandExecute_Param1.TabIndex = 6;
      this.IAP_TextBox_CommandExecute_Param1.Text = "0";
      this.IAP_TextBox_CommandExecute_Param1.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandExecute_Param1_TextChanged);
      // 
      // IAP_TextBox_CommandExecute_Adr
      // 
      this.IAP_TextBox_CommandExecute_Adr.Location = new System.Drawing.Point(57, 6);
      this.IAP_TextBox_CommandExecute_Adr.Name = "IAP_TextBox_CommandExecute_Adr";
      this.IAP_TextBox_CommandExecute_Adr.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandExecute_Adr.TabIndex = 7;
      this.IAP_TextBox_CommandExecute_Adr.Text = "0";
      this.IAP_TextBox_CommandExecute_Adr.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandExecute_Adr_TextChanged);
      // 
      // TabPage_Command_Start
      // 
      this.TabPage_Command_Start.Controls.Add(this.label18);
      this.TabPage_Command_Start.Controls.Add(this.IAP_TextBox_CommandStart_Adr);
      this.TabPage_Command_Start.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Start.Name = "TabPage_Command_Start";
      this.TabPage_Command_Start.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Start.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Start.TabIndex = 7;
      this.TabPage_Command_Start.Text = "Start";
      this.TabPage_Command_Start.UseVisualStyleBackColor = true;
      // 
      // label18
      // 
      this.label18.AutoSize = true;
      this.label18.Location = new System.Drawing.Point(6, 13);
      this.label18.Name = "label18";
      this.label18.Size = new System.Drawing.Size(45, 13);
      this.label18.TabIndex = 11;
      this.label18.Text = "Adresse";
      // 
      // IAP_TextBox_CommandStart_Adr
      // 
      this.IAP_TextBox_CommandStart_Adr.Location = new System.Drawing.Point(57, 10);
      this.IAP_TextBox_CommandStart_Adr.Name = "IAP_TextBox_CommandStart_Adr";
      this.IAP_TextBox_CommandStart_Adr.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandStart_Adr.TabIndex = 10;
      this.IAP_TextBox_CommandStart_Adr.Text = "0";
      this.IAP_TextBox_CommandStart_Adr.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandStart_Adr_TextChanged);
      // 
      // TabPage_Command_Exit
      // 
      this.TabPage_Command_Exit.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Exit.Name = "TabPage_Command_Exit";
      this.TabPage_Command_Exit.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Exit.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Exit.TabIndex = 5;
      this.TabPage_Command_Exit.Text = "Exit";
      this.TabPage_Command_Exit.UseVisualStyleBackColor = true;
      // 
      // TabPage_Command_Enter
      // 
      this.TabPage_Command_Enter.Controls.Add(this.label22);
      this.TabPage_Command_Enter.Controls.Add(this.IAP_TextBox_CommandEnter_ID2);
      this.TabPage_Command_Enter.Controls.Add(this.label21);
      this.TabPage_Command_Enter.Controls.Add(this.IAP_TextBox_CommandEnter_ID1);
      this.TabPage_Command_Enter.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Enter.Name = "TabPage_Command_Enter";
      this.TabPage_Command_Enter.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Enter.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Enter.TabIndex = 8;
      this.TabPage_Command_Enter.Text = "Enter";
      this.TabPage_Command_Enter.UseVisualStyleBackColor = true;
      // 
      // label22
      // 
      this.label22.AutoSize = true;
      this.label22.Location = new System.Drawing.Point(7, 35);
      this.label22.Name = "label22";
      this.label22.Size = new System.Drawing.Size(24, 13);
      this.label22.TabIndex = 15;
      this.label22.Text = "ID2";
      // 
      // IAP_TextBox_CommandEnter_ID2
      // 
      this.IAP_TextBox_CommandEnter_ID2.Location = new System.Drawing.Point(31, 32);
      this.IAP_TextBox_CommandEnter_ID2.Name = "IAP_TextBox_CommandEnter_ID2";
      this.IAP_TextBox_CommandEnter_ID2.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandEnter_ID2.TabIndex = 14;
      this.IAP_TextBox_CommandEnter_ID2.Text = "0";
      this.IAP_TextBox_CommandEnter_ID2.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandEnter_ID2_TextChanged);
      // 
      // label21
      // 
      this.label21.AutoSize = true;
      this.label21.Location = new System.Drawing.Point(7, 9);
      this.label21.Name = "label21";
      this.label21.Size = new System.Drawing.Size(24, 13);
      this.label21.TabIndex = 13;
      this.label21.Text = "ID1";
      // 
      // IAP_TextBox_CommandEnter_ID1
      // 
      this.IAP_TextBox_CommandEnter_ID1.Location = new System.Drawing.Point(31, 6);
      this.IAP_TextBox_CommandEnter_ID1.Name = "IAP_TextBox_CommandEnter_ID1";
      this.IAP_TextBox_CommandEnter_ID1.Size = new System.Drawing.Size(67, 20);
      this.IAP_TextBox_CommandEnter_ID1.TabIndex = 12;
      this.IAP_TextBox_CommandEnter_ID1.Text = "0";
      this.IAP_TextBox_CommandEnter_ID1.TextChanged += new System.EventHandler(this.IAP_TextBox_CommandEnter_ID_TextChanged);
      // 
      // IAP_Button_Send
      // 
      this.IAP_Button_Send.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_Button_Send.Location = new System.Drawing.Point(618, 280);
      this.IAP_Button_Send.Name = "IAP_Button_Send";
      this.IAP_Button_Send.Size = new System.Drawing.Size(81, 25);
      this.IAP_Button_Send.TabIndex = 12;
      this.IAP_Button_Send.Text = "Send";
      this.IAP_Button_Send.UseVisualStyleBackColor = true;
      this.IAP_Button_Send.Click += new System.EventHandler(this.IAP_Button_Send_Click);
      // 
      // IAP_TextBox_Command0_7
      // 
      this.IAP_TextBox_Command0_7.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_7.Location = new System.Drawing.Point(470, 283);
      this.IAP_TextBox_Command0_7.Name = "IAP_TextBox_Command0_7";
      this.IAP_TextBox_Command0_7.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_7.TabIndex = 22;
      // 
      // IAP_TextBox_Command0_5
      // 
      this.IAP_TextBox_Command0_5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_5.Location = new System.Drawing.Point(324, 283);
      this.IAP_TextBox_Command0_5.Name = "IAP_TextBox_Command0_5";
      this.IAP_TextBox_Command0_5.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_5.TabIndex = 21;
      // 
      // IAP_TextBox_Command0
      // 
      this.IAP_TextBox_Command0.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IAP_TextBox_Command0.Location = new System.Drawing.Point(6, 260);
      this.IAP_TextBox_Command0.Multiline = true;
      this.IAP_TextBox_Command0.Name = "IAP_TextBox_Command0";
      this.IAP_TextBox_Command0.Size = new System.Drawing.Size(843, 20);
      this.IAP_TextBox_Command0.TabIndex = 17;
      // 
      // IAP_TextBox_Command0_8
      // 
      this.IAP_TextBox_Command0_8.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_8.Location = new System.Drawing.Point(544, 283);
      this.IAP_TextBox_Command0_8.Name = "IAP_TextBox_Command0_8";
      this.IAP_TextBox_Command0_8.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_8.TabIndex = 16;
      // 
      // IAP_TextBox_Command0_3
      // 
      this.IAP_TextBox_Command0_3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_3.Location = new System.Drawing.Point(176, 283);
      this.IAP_TextBox_Command0_3.Name = "IAP_TextBox_Command0_3";
      this.IAP_TextBox_Command0_3.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_3.TabIndex = 15;
      // 
      // IAP_TextBox_Command0_6
      // 
      this.IAP_TextBox_Command0_6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_6.Location = new System.Drawing.Point(396, 283);
      this.IAP_TextBox_Command0_6.Name = "IAP_TextBox_Command0_6";
      this.IAP_TextBox_Command0_6.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_6.TabIndex = 20;
      // 
      // IAP_TextBox_Command0_4
      // 
      this.IAP_TextBox_Command0_4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_4.Location = new System.Drawing.Point(250, 283);
      this.IAP_TextBox_Command0_4.Name = "IAP_TextBox_Command0_4";
      this.IAP_TextBox_Command0_4.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_4.TabIndex = 19;
      // 
      // IAP_TextBox_Command0_2
      // 
      this.IAP_TextBox_Command0_2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_2.Location = new System.Drawing.Point(102, 283);
      this.IAP_TextBox_Command0_2.Name = "IAP_TextBox_Command0_2";
      this.IAP_TextBox_Command0_2.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_2.TabIndex = 18;
      // 
      // IAP_TextBox_Command0_1
      // 
      this.IAP_TextBox_Command0_1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.IAP_TextBox_Command0_1.Location = new System.Drawing.Point(7, 283);
      this.IAP_TextBox_Command0_1.Name = "IAP_TextBox_Command0_1";
      this.IAP_TextBox_Command0_1.Size = new System.Drawing.Size(68, 20);
      this.IAP_TextBox_Command0_1.TabIndex = 14;
      // 
      // IAP_Text_Data
      // 
      this.IAP_Text_Data.Dock = System.Windows.Forms.DockStyle.Bottom;
      this.IAP_Text_Data.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.IAP_Text_Data.Location = new System.Drawing.Point(3, 317);
      this.IAP_Text_Data.Multiline = true;
      this.IAP_Text_Data.Name = "IAP_Text_Data";
      this.IAP_Text_Data.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.IAP_Text_Data.Size = new System.Drawing.Size(849, 202);
      this.IAP_Text_Data.TabIndex = 13;
      // 
      // tabPage7
      // 
      this.tabPage7.Controls.Add(this.label27);
      this.tabPage7.Controls.Add(this.ComPort_Text_StreamID);
      this.tabPage7.Controls.Add(this.ComPort_Text_ConIO);
      this.tabPage7.Location = new System.Drawing.Point(4, 22);
      this.tabPage7.Name = "tabPage7";
      this.tabPage7.Size = new System.Drawing.Size(855, 522);
      this.tabPage7.TabIndex = 3;
      this.tabPage7.Text = "ComPort";
      this.tabPage7.UseVisualStyleBackColor = true;
      // 
      // label27
      // 
      this.label27.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.label27.AutoSize = true;
      this.label27.Location = new System.Drawing.Point(695, 3);
      this.label27.Name = "label27";
      this.label27.Size = new System.Drawing.Size(54, 13);
      this.label27.TabIndex = 24;
      this.label27.Text = "Stream ID";
      // 
      // ComPort_Text_StreamID
      // 
      this.ComPort_Text_StreamID.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this.ComPort_Text_StreamID.Location = new System.Drawing.Point(755, 0);
      this.ComPort_Text_StreamID.Name = "ComPort_Text_StreamID";
      this.ComPort_Text_StreamID.Size = new System.Drawing.Size(100, 20);
      this.ComPort_Text_StreamID.TabIndex = 23;
      this.ComPort_Text_StreamID.Text = "0";
      // 
      // ComPort_Text_ConIO
      // 
      this.ComPort_Text_ConIO.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.ComPort_Text_ConIO.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.ComPort_Text_ConIO.Location = new System.Drawing.Point(1, 33);
      this.ComPort_Text_ConIO.Multiline = true;
      this.ComPort_Text_ConIO.Name = "ComPort_Text_ConIO";
      this.ComPort_Text_ConIO.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.ComPort_Text_ConIO.Size = new System.Drawing.Size(852, 488);
      this.ComPort_Text_ConIO.TabIndex = 22;
      // 
      // TextBox_SrcID1
      // 
      this.TextBox_SrcID1.Location = new System.Drawing.Point(783, 73);
      this.TextBox_SrcID1.Name = "TextBox_SrcID1";
      this.TextBox_SrcID1.Size = new System.Drawing.Size(24, 20);
      this.TextBox_SrcID1.TabIndex = 14;
      this.TextBox_SrcID1.Text = "14";
      // 
      // TextBox_SrcID2
      // 
      this.TextBox_SrcID2.Location = new System.Drawing.Point(804, 73);
      this.TextBox_SrcID2.Name = "TextBox_SrcID2";
      this.TextBox_SrcID2.Size = new System.Drawing.Size(24, 20);
      this.TextBox_SrcID2.TabIndex = 14;
      this.TextBox_SrcID2.Text = "0";
      // 
      // TextBox_SrcID3
      // 
      this.TextBox_SrcID3.Location = new System.Drawing.Point(825, 73);
      this.TextBox_SrcID3.Name = "TextBox_SrcID3";
      this.TextBox_SrcID3.Size = new System.Drawing.Size(24, 20);
      this.TextBox_SrcID3.TabIndex = 14;
      this.TextBox_SrcID3.Text = "0";
      // 
      // TextBox_SrcID4
      // 
      this.TextBox_SrcID4.Location = new System.Drawing.Point(845, 73);
      this.TextBox_SrcID4.Name = "TextBox_SrcID4";
      this.TextBox_SrcID4.Size = new System.Drawing.Size(24, 20);
      this.TextBox_SrcID4.TabIndex = 14;
      this.TextBox_SrcID4.Text = "0";
      // 
      // TextBox_DstID4
      // 
      this.TextBox_DstID4.Location = new System.Drawing.Point(845, 98);
      this.TextBox_DstID4.Name = "TextBox_DstID4";
      this.TextBox_DstID4.Size = new System.Drawing.Size(24, 20);
      this.TextBox_DstID4.TabIndex = 17;
      this.TextBox_DstID4.Text = "0";
      // 
      // TextBox_DstID3
      // 
      this.TextBox_DstID3.Location = new System.Drawing.Point(825, 98);
      this.TextBox_DstID3.Name = "TextBox_DstID3";
      this.TextBox_DstID3.Size = new System.Drawing.Size(24, 20);
      this.TextBox_DstID3.TabIndex = 18;
      this.TextBox_DstID3.Text = "0";
      // 
      // TextBox_DstID2
      // 
      this.TextBox_DstID2.Location = new System.Drawing.Point(804, 98);
      this.TextBox_DstID2.Name = "TextBox_DstID2";
      this.TextBox_DstID2.Size = new System.Drawing.Size(24, 20);
      this.TextBox_DstID2.TabIndex = 15;
      this.TextBox_DstID2.Text = "0";
      // 
      // TextBox_DstID1
      // 
      this.TextBox_DstID1.Location = new System.Drawing.Point(783, 98);
      this.TextBox_DstID1.Name = "TextBox_DstID1";
      this.TextBox_DstID1.Size = new System.Drawing.Size(24, 20);
      this.TextBox_DstID1.TabIndex = 16;
      this.TextBox_DstID1.Text = "1";
      // 
      // label23
      // 
      this.label23.AutoSize = true;
      this.label23.Location = new System.Drawing.Point(665, 101);
      this.label23.Name = "label23";
      this.label23.Size = new System.Drawing.Size(29, 13);
      this.label23.TabIndex = 19;
      this.label23.Text = "Dest";
      // 
      // label24
      // 
      this.label24.AutoSize = true;
      this.label24.Location = new System.Drawing.Point(653, 75);
      this.label24.Name = "label24";
      this.label24.Size = new System.Drawing.Size(41, 13);
      this.label24.TabIndex = 20;
      this.label24.Text = "Source";
      // 
      // label32
      // 
      this.label32.AutoSize = true;
      this.label32.Location = new System.Drawing.Point(780, 52);
      this.label32.Name = "label32";
      this.label32.Size = new System.Drawing.Size(36, 13);
      this.label32.TabIndex = 21;
      this.label32.Text = "BnAdr";
      // 
      // label33
      // 
      this.label33.AutoSize = true;
      this.label33.Location = new System.Drawing.Point(698, 51);
      this.label33.Name = "label33";
      this.label33.Size = new System.Drawing.Size(57, 13);
      this.label33.TabIndex = 22;
      this.label33.Text = "DeviceAdr";
      // 
      // TextBox_SrcDeviceAdr
      // 
      this.TextBox_SrcDeviceAdr.Location = new System.Drawing.Point(701, 72);
      this.TextBox_SrcDeviceAdr.Name = "TextBox_SrcDeviceAdr";
      this.TextBox_SrcDeviceAdr.Size = new System.Drawing.Size(76, 20);
      this.TextBox_SrcDeviceAdr.TabIndex = 23;
      this.TextBox_SrcDeviceAdr.Text = "7";
      this.TextBox_SrcDeviceAdr.TextChanged += new System.EventHandler(this.TextBox_SrcDeviceAdr_TextChanged);
      // 
      // TextBox_DestDeviceAdr
      // 
      this.TextBox_DestDeviceAdr.Location = new System.Drawing.Point(701, 98);
      this.TextBox_DestDeviceAdr.Name = "TextBox_DestDeviceAdr";
      this.TextBox_DestDeviceAdr.Size = new System.Drawing.Size(76, 20);
      this.TextBox_DestDeviceAdr.TabIndex = 24;
      this.TextBox_DestDeviceAdr.Text = "16";
      this.TextBox_DestDeviceAdr.TextChanged += new System.EventHandler(this.TextBox_DestDeviceAdr_TextChanged);
      // 
      // Session_Label_Blink
      // 
      this.Session_Label_Blink.AutoSize = true;
      this.Session_Label_Blink.Location = new System.Drawing.Point(831, 9);
      this.Session_Label_Blink.Name = "Session_Label_Blink";
      this.Session_Label_Blink.Size = new System.Drawing.Size(25, 13);
      this.Session_Label_Blink.TabIndex = 25;
      this.Session_Label_Blink.Text = "      ";
      // 
      // IAP_CheckBox_CommandDl_Connect
      // 
      this.IAP_CheckBox_CommandDl_Connect.AutoSize = true;
      this.IAP_CheckBox_CommandDl_Connect.Checked = true;
      this.IAP_CheckBox_CommandDl_Connect.CheckState = System.Windows.Forms.CheckState.Checked;
      this.IAP_CheckBox_CommandDl_Connect.Location = new System.Drawing.Point(6, 110);
      this.IAP_CheckBox_CommandDl_Connect.Name = "IAP_CheckBox_CommandDl_Connect";
      this.IAP_CheckBox_CommandDl_Connect.Size = new System.Drawing.Size(116, 17);
      this.IAP_CheckBox_CommandDl_Connect.TabIndex = 38;
      this.IAP_CheckBox_CommandDl_Connect.Text = "Connect before DL";
      this.IAP_CheckBox_CommandDl_Connect.UseVisualStyleBackColor = true;
      // 
      // Form1
      // 
      this.AllowDrop = true;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(892, 693);
      this.Controls.Add(this.Session_Label_Blink);
      this.Controls.Add(this.TextBox_DestDeviceAdr);
      this.Controls.Add(this.TextBox_SrcDeviceAdr);
      this.Controls.Add(this.label33);
      this.Controls.Add(this.label32);
      this.Controls.Add(this.label24);
      this.Controls.Add(this.label23);
      this.Controls.Add(this.TextBox_DstID4);
      this.Controls.Add(this.TextBox_DstID3);
      this.Controls.Add(this.TextBox_DstID2);
      this.Controls.Add(this.TextBox_DstID1);
      this.Controls.Add(this.TextBox_SrcID4);
      this.Controls.Add(this.TextBox_SrcID3);
      this.Controls.Add(this.TextBox_SrcID2);
      this.Controls.Add(this.TextBox_SrcID1);
      this.Controls.Add(this.Tab_MainBotnet);
      this.Controls.Add(this.Lbl_InputCnt);
      this.Controls.Add(this.TextBox_CalcHex);
      this.Controls.Add(this.TextBox_CalcDez);
      this.Controls.Add(this.Label_BreakStatus);
      this.Controls.Add(this.Label_RIStatus);
      this.Controls.Add(this.Lable_DSRStatus);
      this.Controls.Add(this.Label_CTSStatus);
      this.Controls.Add(this.TextError);
      this.Controls.Add(this.ComboBox_HandShaking);
      this.Controls.Add(this.ComboBox_Parity);
      this.Controls.Add(this.ComboBox_StopBits);
      this.Controls.Add(this.ComboBox_DataBits);
      this.Controls.Add(this.ComboBox_Baud);
      this.Controls.Add(this.ComboBox_Ports);
      this.Controls.Add(this.Btn_PortState);
      this.Controls.Add(this.Btn_Ports);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Name = "Form1";
      this.Text = "Form1";
      this.Load += new System.EventHandler(this.Form1_Load);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.Tab_MainBotnet.ResumeLayout(false);
      this.tabPage1.ResumeLayout(false);
      this.tabPage1.PerformLayout();
      this.tabPage2.ResumeLayout(false);
      this.tabPage2.PerformLayout();
      this.tabPage3.ResumeLayout(false);
      this.tabPage3.PerformLayout();
      this.tabPage4.ResumeLayout(false);
      this.tabPage4.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.tabPage5.ResumeLayout(false);
      this.tabPage5.PerformLayout();
      this.tabPage6.ResumeLayout(false);
      this.tabPage6.PerformLayout();
      this.IAP_TabControl_Command.ResumeLayout(false);
      this.TabPage_Command_Download.ResumeLayout(false);
      this.TabPage_Command_Download.PerformLayout();
      this.TabPage_Command_Info1.ResumeLayout(false);
      this.TabPage_Command_Info1.PerformLayout();
      this.TabPage_Command_Info2.ResumeLayout(false);
      this.TabPage_Command_Info2.PerformLayout();
      this.TabPage_Command_Write.ResumeLayout(false);
      this.TabPage_Command_Write.PerformLayout();
      this.TabPage_Command_Read.ResumeLayout(false);
      this.TabPage_Command_Read.PerformLayout();
      this.TabPage_Command_Checksum.ResumeLayout(false);
      this.TabPage_Command_Checksum.PerformLayout();
      this.TabPage_Command_Execute.ResumeLayout(false);
      this.TabPage_Command_Execute.PerformLayout();
      this.TabPage_Command_Start.ResumeLayout(false);
      this.TabPage_Command_Start.PerformLayout();
      this.TabPage_Command_Enter.ResumeLayout(false);
      this.TabPage_Command_Enter.PerformLayout();
      this.tabPage7.ResumeLayout(false);
      this.tabPage7.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }

    #endregion

    private System.Windows.Forms.Button Btn_Ports;
    private System.Windows.Forms.Button Btn_PortState;
    private System.Windows.Forms.ComboBox ComboBox_Ports;
    private System.Windows.Forms.ComboBox ComboBox_Baud;
    private System.Windows.Forms.ComboBox ComboBox_DataBits;
    private System.Windows.Forms.ComboBox ComboBox_StopBits;
    private System.Windows.Forms.ComboBox ComboBox_Parity;
    private System.Windows.Forms.ComboBox ComboBox_HandShaking;
    private System.Windows.Forms.Label Label_CTSStatus;
    private System.Windows.Forms.Label Lable_DSRStatus;
    private System.Windows.Forms.Label Label_RIStatus;
    private System.Windows.Forms.Label Label_BreakStatus;
    private System.Windows.Forms.TextBox TextError;
    private System.Windows.Forms.TextBox TextBox_CalcHex;
    private System.Windows.Forms.TextBox TextBox_CalcDez;
    private System.Windows.Forms.Label Lbl_InputCnt;
    private System.Windows.Forms.TabControl Tab_MainBotnet;
    private System.Windows.Forms.TabPage tabPage1;
    private System.Windows.Forms.TabPage tabPage6;
    private System.Windows.Forms.Label label15;
    private System.Windows.Forms.Label label14;
    private System.Windows.Forms.Button IAP_Button_Clear_TextData;
    private System.Windows.Forms.TextBox IAP_TextBox_DiagWorkRAM_Size_Hex;
    private System.Windows.Forms.TextBox IAP_TextBox_DiagWorkRAM_Adr_Hex;
    private System.Windows.Forms.TextBox IAP_TextBox_DiagWorkRAM_Size_Dez;
    private System.Windows.Forms.TextBox IAP_TextBox_DiagWorkRAM_Adr_Dez;
    private System.Windows.Forms.TabControl IAP_TabControl_Command;
    private System.Windows.Forms.TabPage TabPage_Command_Info1;
    private System.Windows.Forms.TextBox IAP_TextBox_DiagInfo0;
    private System.Windows.Forms.TabPage TabPage_Command_Info2;
    private System.Windows.Forms.TextBox IAP_TextBox_DiagInfo1;
    private System.Windows.Forms.TabPage TabPage_Command_Write;
    private System.Windows.Forms.Label label17;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandWriteBlock_BlSize;
    private System.Windows.Forms.TextBox TextBox_DiagWrite;
    private System.Windows.Forms.Label label10;
    private System.Windows.Forms.Label label11;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandWrite_FileName;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandWrite_FileOffset;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandWrite_Len;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandWrite_Adr;
    private System.Windows.Forms.TabPage TabPage_Command_Read;
    private System.Windows.Forms.Label label20;
    private System.Windows.Forms.TextBox TextBox_DiagRead;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandReadBlock_File;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandRead_Len;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandRead_Adr;
    private System.Windows.Forms.Label label19;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandReadBlock_BlSize;
    private System.Windows.Forms.Button Button_CommandReadBlock_FileSave;
    private System.Windows.Forms.TabPage TabPage_Command_Checksum;
    private System.Windows.Forms.TextBox IAP_TextBox_DiagChecksum;
    private System.Windows.Forms.Label label12;
    private System.Windows.Forms.Label label16;
    private System.Windows.Forms.Label label13;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandChk_Len;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandChk_Adr;
    private System.Windows.Forms.TabPage TabPage_Command_Execute;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandExecute_Param4;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandExecute_Param2;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandExecute_Param3;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandExecute_Param1;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandExecute_Adr;
    private System.Windows.Forms.TabPage TabPage_Command_Start;
    private System.Windows.Forms.Label label18;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandStart_Adr;
    private System.Windows.Forms.TabPage TabPage_Command_Exit;
    private System.Windows.Forms.TabPage TabPage_Command_Enter;
    private System.Windows.Forms.Label label22;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandEnter_ID2;
    private System.Windows.Forms.Label label21;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandEnter_ID1;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_7;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_5;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_8;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_3;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_6;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_4;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_2;
    private System.Windows.Forms.TextBox IAP_TextBox_Command0_1;
    private System.Windows.Forms.TextBox IAP_Text_Data;
    private System.Windows.Forms.Button IAP_Button_Send;
    private System.Windows.Forms.TextBox Com_Text_ConIO;
    private System.Windows.Forms.TextBox TextBox_SrcID1;
    private System.Windows.Forms.TextBox TextBox_SrcID2;
    private System.Windows.Forms.TextBox TextBox_SrcID3;
    private System.Windows.Forms.TextBox TextBox_SrcID4;
    private System.Windows.Forms.TextBox TextBox_DstID4;
    private System.Windows.Forms.TextBox TextBox_DstID3;
    private System.Windows.Forms.TextBox TextBox_DstID2;
    private System.Windows.Forms.TextBox TextBox_DstID1;
    private System.Windows.Forms.Label label23;
    private System.Windows.Forms.Label label24;
    private System.Windows.Forms.TabPage tabPage5;
    private System.Windows.Forms.TabPage tabPage7;
    private System.Windows.Forms.TextBox Cmd_Text_ConIo;
    private System.Windows.Forms.Label label25;
    private System.Windows.Forms.TextBox Cmd_Text_StreamID;
    private System.Windows.Forms.Label label26;
    private System.Windows.Forms.TextBox IAP_Text_SteamID;
    private System.Windows.Forms.Label label27;
    private System.Windows.Forms.TextBox ComPort_Text_StreamID;
    private System.Windows.Forms.TextBox ComPort_Text_ConIO;
    private System.Windows.Forms.TabPage tabPage4;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.Label label29;
    private System.Windows.Forms.TextBox MsgTest_Text_Stream_Idx;
    private System.Windows.Forms.Button MsgTest_Btn_Send;
    private System.Windows.Forms.TextBox MsgTest_Text_Data;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.Label label28;
    private System.Windows.Forms.TextBox MsgTest_Text_Msg_ID;
    private System.Windows.Forms.Label label31;
    private System.Windows.Forms.TextBox MsgTest_Text_Payload;
    private System.Windows.Forms.RadioButton MsgTest_RadioButton_Stream_Cmd;
    private System.Windows.Forms.RadioButton MsgTest_RadioButton_Stream_Data;
    private System.Windows.Forms.TabPage tabPage2;
    private System.Windows.Forms.TextBox MsgTrace_Text_ConIO;
    private System.Windows.Forms.ComboBox MsgTest_Combo_Stream_Cmd;
    private System.Windows.Forms.TextBox MsgTest_Text_Stream_Para3;
    private System.Windows.Forms.TextBox MsgTest_Text_Stream_Para2;
    private System.Windows.Forms.TextBox MsgTest_Text_Stream_Para1;
    private System.Windows.Forms.Label label30;
    private System.Windows.Forms.Button Cmd_Btn_Send;
    private System.Windows.Forms.ComboBox Cmd_Combo_CmdToSend;
    private System.Windows.Forms.Label label32;
    private System.Windows.Forms.Label label33;
    private System.Windows.Forms.TextBox TextBox_SrcDeviceAdr;
    private System.Windows.Forms.TextBox TextBox_DestDeviceAdr;
    private System.Windows.Forms.TabPage tabPage3;
    private System.Windows.Forms.TextBox Session_Text_ConIO;
    private System.Windows.Forms.Button Session_Btn_Stop;
    private System.Windows.Forms.Button Session_Btn_Start;
    private System.Windows.Forms.Label Session_Label_Blink;
    private System.Windows.Forms.TabPage TabPage_Command_Download;
    private System.Windows.Forms.CheckBox IAP_CheckBox_CommandDl_Reset;
    private System.Windows.Forms.Label label38;
    private System.Windows.Forms.Label label34;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandDl_Status;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandDl_BlockLen;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandDl_Adr;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandDl_Len;
    private System.Windows.Forms.Label label36;
    private System.Windows.Forms.TextBox IAP_TextBox_CommandDl_File;
    private System.Windows.Forms.Label label37;
    private System.Windows.Forms.CheckBox IAP_CheckBox_CommandDl_Connect;
  }
}

