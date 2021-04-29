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
      this.components = new System.ComponentModel.Container();
      this.Btn_Ports = new System.Windows.Forms.Button();
      this.Btn_PortState = new System.Windows.Forms.Button();
      this.ComboBox_Ports = new System.Windows.Forms.ComboBox();
      this.ComboBox_Baud = new System.Windows.Forms.ComboBox();
      this.ComboBox_DataBits = new System.Windows.Forms.ComboBox();
      this.ComboBox_StopBits = new System.Windows.Forms.ComboBox();
      this.ComboBox_Parity = new System.Windows.Forms.ComboBox();
      this.ComboBox_HandShaking = new System.Windows.Forms.ComboBox();
      this.Text_Data = new System.Windows.Forms.TextBox();
      this.Label_CTSStatus = new System.Windows.Forms.Label();
      this.Lable_DSRStatus = new System.Windows.Forms.Label();
      this.Label_RIStatus = new System.Windows.Forms.Label();
      this.Label_BreakStatus = new System.Windows.Forms.Label();
      this.TextError = new System.Windows.Forms.TextBox();
      this.Button_Send = new System.Windows.Forms.Button();
      this.TextBox_Command0_1 = new System.Windows.Forms.TextBox();
      this.TextBox_Command0_2 = new System.Windows.Forms.TextBox();
      this.TextBox_Command0_3 = new System.Windows.Forms.TextBox();
      this.TextBox_Command0_4 = new System.Windows.Forms.TextBox();
      this.TextBox_Command0_5 = new System.Windows.Forms.TextBox();
      this.TextBox_Command0_6 = new System.Windows.Forms.TextBox();
      this.TextBox_Command0_8 = new System.Windows.Forms.TextBox();
      this.TextBox_Command0_7 = new System.Windows.Forms.TextBox();
      this.TabControl_Command = new System.Windows.Forms.TabControl();
      this.TabPage_Command_Info1 = new System.Windows.Forms.TabPage();
      this.TextBox_DiagInfo0 = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Info2 = new System.Windows.Forms.TabPage();
      this.TextBox_DiagInfo1 = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Write = new System.Windows.Forms.TabPage();
      this.label17 = new System.Windows.Forms.Label();
      this.TextBox_CommandWriteBlock_BlSize = new System.Windows.Forms.TextBox();
      this.Button_WriteBlock_Start = new System.Windows.Forms.Button();
      this.TextBox_DiagWrite = new System.Windows.Forms.TextBox();
      this.label10 = new System.Windows.Forms.Label();
      this.label11 = new System.Windows.Forms.Label();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.TextBox_CommandWrite_FileName = new System.Windows.Forms.TextBox();
      this.TextBox_CommandWrite_FileOffset = new System.Windows.Forms.TextBox();
      this.TextBox_CommandWrite_Len = new System.Windows.Forms.TextBox();
      this.TextBox_CommandWrite_Adr = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Read = new System.Windows.Forms.TabPage();
      this.label20 = new System.Windows.Forms.Label();
      this.TextBox_DiagRead = new System.Windows.Forms.TextBox();
      this.label3 = new System.Windows.Forms.Label();
      this.TextBox_CommandReadBlock_File = new System.Windows.Forms.TextBox();
      this.label4 = new System.Windows.Forms.Label();
      this.TextBox_CommandRead_Len = new System.Windows.Forms.TextBox();
      this.TextBox_CommandRead_Adr = new System.Windows.Forms.TextBox();
      this.label19 = new System.Windows.Forms.Label();
      this.TextBox_CommandReadBlock_BlSize = new System.Windows.Forms.TextBox();
      this.Button_ReadBlock_Start = new System.Windows.Forms.Button();
      this.Button_CommandReadBlock_FileSave = new System.Windows.Forms.Button();
      this.TabPage_Command_Checksum = new System.Windows.Forms.TabPage();
      this.TextBox_DiagChecksum = new System.Windows.Forms.TextBox();
      this.label12 = new System.Windows.Forms.Label();
      this.label16 = new System.Windows.Forms.Label();
      this.label13 = new System.Windows.Forms.Label();
      this.TextBox_CommandChk_Len = new System.Windows.Forms.TextBox();
      this.TextBox_CommandChk_Adr = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Execute = new System.Windows.Forms.TabPage();
      this.label9 = new System.Windows.Forms.Label();
      this.label7 = new System.Windows.Forms.Label();
      this.TextBox_CommandExecute_Param4 = new System.Windows.Forms.TextBox();
      this.TextBox_CommandExecute_Param2 = new System.Windows.Forms.TextBox();
      this.label8 = new System.Windows.Forms.Label();
      this.label5 = new System.Windows.Forms.Label();
      this.label6 = new System.Windows.Forms.Label();
      this.TextBox_CommandExecute_Param3 = new System.Windows.Forms.TextBox();
      this.TextBox_CommandExecute_Param1 = new System.Windows.Forms.TextBox();
      this.TextBox_CommandExecute_Adr = new System.Windows.Forms.TextBox();
      this.TabPage_Command_Exit = new System.Windows.Forms.TabPage();
      this.TextBox_CalcHex = new System.Windows.Forms.TextBox();
      this.TextBox_CalcDez = new System.Windows.Forms.TextBox();
      this.TextBox_Command0 = new System.Windows.Forms.TextBox();
      this.Button_Clear_TextData = new System.Windows.Forms.Button();
      this.TextBox_DiagWorkRAM_Adr_Dez = new System.Windows.Forms.TextBox();
      this.TextBox_DiagWorkRAM_Adr_Hex = new System.Windows.Forms.TextBox();
      this.TextBox_DiagWorkRAM_Size_Dez = new System.Windows.Forms.TextBox();
      this.TextBox_DiagWorkRAM_Size_Hex = new System.Windows.Forms.TextBox();
      this.label14 = new System.Windows.Forms.Label();
      this.label15 = new System.Windows.Forms.Label();
      this.Timer_Comport = new System.Windows.Forms.Timer(this.components);
      this.Lbl_InputCnt = new System.Windows.Forms.Label();
      this.TabPage_Command_Start = new System.Windows.Forms.TabPage();
      this.label18 = new System.Windows.Forms.Label();
      this.TextBox_CommandStart_Adr = new System.Windows.Forms.TextBox();
      this.TabControl_Command.SuspendLayout();
      this.TabPage_Command_Info1.SuspendLayout();
      this.TabPage_Command_Info2.SuspendLayout();
      this.TabPage_Command_Write.SuspendLayout();
      this.TabPage_Command_Read.SuspendLayout();
      this.TabPage_Command_Checksum.SuspendLayout();
      this.TabPage_Command_Execute.SuspendLayout();
      this.TabPage_Command_Start.SuspendLayout();
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
      // Text_Data
      // 
      this.Text_Data.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Text_Data.Location = new System.Drawing.Point(10, 433);
      this.Text_Data.Multiline = true;
      this.Text_Data.Name = "Text_Data";
      this.Text_Data.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.Text_Data.Size = new System.Drawing.Size(832, 191);
      this.Text_Data.TabIndex = 2;
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
      // Button_Send
      // 
      this.Button_Send.Location = new System.Drawing.Point(634, 402);
      this.Button_Send.Name = "Button_Send";
      this.Button_Send.Size = new System.Drawing.Size(81, 25);
      this.Button_Send.TabIndex = 0;
      this.Button_Send.Text = "Send";
      this.Button_Send.UseVisualStyleBackColor = true;
      this.Button_Send.Click += new System.EventHandler(this.Button_Send_Click);
      // 
      // TextBox_Command0_1
      // 
      this.TextBox_Command0_1.Location = new System.Drawing.Point(10, 402);
      this.TextBox_Command0_1.Name = "TextBox_Command0_1";
      this.TextBox_Command0_1.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_1.TabIndex = 4;
      // 
      // TextBox_Command0_2
      // 
      this.TextBox_Command0_2.Location = new System.Drawing.Point(105, 402);
      this.TextBox_Command0_2.Name = "TextBox_Command0_2";
      this.TextBox_Command0_2.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_2.TabIndex = 5;
      // 
      // TextBox_Command0_3
      // 
      this.TextBox_Command0_3.Location = new System.Drawing.Point(179, 402);
      this.TextBox_Command0_3.Name = "TextBox_Command0_3";
      this.TextBox_Command0_3.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_3.TabIndex = 5;
      // 
      // TextBox_Command0_4
      // 
      this.TextBox_Command0_4.Location = new System.Drawing.Point(253, 402);
      this.TextBox_Command0_4.Name = "TextBox_Command0_4";
      this.TextBox_Command0_4.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_4.TabIndex = 5;
      // 
      // TextBox_Command0_5
      // 
      this.TextBox_Command0_5.Location = new System.Drawing.Point(327, 402);
      this.TextBox_Command0_5.Name = "TextBox_Command0_5";
      this.TextBox_Command0_5.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_5.TabIndex = 5;
      // 
      // TextBox_Command0_6
      // 
      this.TextBox_Command0_6.Location = new System.Drawing.Point(399, 402);
      this.TextBox_Command0_6.Name = "TextBox_Command0_6";
      this.TextBox_Command0_6.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_6.TabIndex = 5;
      // 
      // TextBox_Command0_8
      // 
      this.TextBox_Command0_8.Location = new System.Drawing.Point(547, 402);
      this.TextBox_Command0_8.Name = "TextBox_Command0_8";
      this.TextBox_Command0_8.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_8.TabIndex = 5;
      // 
      // TextBox_Command0_7
      // 
      this.TextBox_Command0_7.Location = new System.Drawing.Point(473, 402);
      this.TextBox_Command0_7.Name = "TextBox_Command0_7";
      this.TextBox_Command0_7.Size = new System.Drawing.Size(68, 20);
      this.TextBox_Command0_7.TabIndex = 5;
      // 
      // TabControl_Command
      // 
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Info1);
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Info2);
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Write);
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Read);
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Checksum);
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Execute);
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Start);
      this.TabControl_Command.Controls.Add(this.TabPage_Command_Exit);
      this.TabControl_Command.Location = new System.Drawing.Point(9, 125);
      this.TabControl_Command.Name = "TabControl_Command";
      this.TabControl_Command.SelectedIndex = 0;
      this.TabControl_Command.Size = new System.Drawing.Size(656, 248);
      this.TabControl_Command.TabIndex = 6;
      this.TabControl_Command.SelectedIndexChanged += new System.EventHandler(this.TabControl_Command_SelectedIndexChanged);
      // 
      // TabPage_Command_Info1
      // 
      this.TabPage_Command_Info1.Controls.Add(this.TextBox_DiagInfo0);
      this.TabPage_Command_Info1.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Info1.Name = "TabPage_Command_Info1";
      this.TabPage_Command_Info1.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Info1.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Info1.TabIndex = 0;
      this.TabPage_Command_Info1.Text = "Info";
      this.TabPage_Command_Info1.UseVisualStyleBackColor = true;
      // 
      // TextBox_DiagInfo0
      // 
      this.TextBox_DiagInfo0.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TextBox_DiagInfo0.Location = new System.Drawing.Point(6, 6);
      this.TextBox_DiagInfo0.Multiline = true;
      this.TextBox_DiagInfo0.Name = "TextBox_DiagInfo0";
      this.TextBox_DiagInfo0.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.TextBox_DiagInfo0.Size = new System.Drawing.Size(636, 210);
      this.TextBox_DiagInfo0.TabIndex = 0;
      // 
      // TabPage_Command_Info2
      // 
      this.TabPage_Command_Info2.Controls.Add(this.TextBox_DiagInfo1);
      this.TabPage_Command_Info2.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Info2.Name = "TabPage_Command_Info2";
      this.TabPage_Command_Info2.Padding = new System.Windows.Forms.Padding(3);
      this.TabPage_Command_Info2.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Info2.TabIndex = 1;
      this.TabPage_Command_Info2.Text = "Info2";
      this.TabPage_Command_Info2.UseVisualStyleBackColor = true;
      // 
      // TextBox_DiagInfo1
      // 
      this.TextBox_DiagInfo1.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TextBox_DiagInfo1.Location = new System.Drawing.Point(6, 6);
      this.TextBox_DiagInfo1.Multiline = true;
      this.TextBox_DiagInfo1.Name = "TextBox_DiagInfo1";
      this.TextBox_DiagInfo1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.TextBox_DiagInfo1.Size = new System.Drawing.Size(636, 210);
      this.TextBox_DiagInfo1.TabIndex = 1;
      // 
      // TabPage_Command_Write
      // 
      this.TabPage_Command_Write.Controls.Add(this.label17);
      this.TabPage_Command_Write.Controls.Add(this.TextBox_CommandWriteBlock_BlSize);
      this.TabPage_Command_Write.Controls.Add(this.Button_WriteBlock_Start);
      this.TabPage_Command_Write.Controls.Add(this.TextBox_DiagWrite);
      this.TabPage_Command_Write.Controls.Add(this.label10);
      this.TabPage_Command_Write.Controls.Add(this.label11);
      this.TabPage_Command_Write.Controls.Add(this.label2);
      this.TabPage_Command_Write.Controls.Add(this.label1);
      this.TabPage_Command_Write.Controls.Add(this.TextBox_CommandWrite_FileName);
      this.TabPage_Command_Write.Controls.Add(this.TextBox_CommandWrite_FileOffset);
      this.TabPage_Command_Write.Controls.Add(this.TextBox_CommandWrite_Len);
      this.TabPage_Command_Write.Controls.Add(this.TextBox_CommandWrite_Adr);
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
      // TextBox_CommandWriteBlock_BlSize
      // 
      this.TextBox_CommandWriteBlock_BlSize.Location = new System.Drawing.Point(54, 76);
      this.TextBox_CommandWriteBlock_BlSize.Name = "TextBox_CommandWriteBlock_BlSize";
      this.TextBox_CommandWriteBlock_BlSize.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandWriteBlock_BlSize.TabIndex = 11;
      this.TextBox_CommandWriteBlock_BlSize.Text = "0";
      this.TextBox_CommandWriteBlock_BlSize.TextChanged += new System.EventHandler(this.TextBox_CommandWriteBlock_BlSize_TextChanged);
      // 
      // Button_WriteBlock_Start
      // 
      this.Button_WriteBlock_Start.Location = new System.Drawing.Point(6, 102);
      this.Button_WriteBlock_Start.Name = "Button_WriteBlock_Start";
      this.Button_WriteBlock_Start.Size = new System.Drawing.Size(115, 30);
      this.Button_WriteBlock_Start.TabIndex = 10;
      this.Button_WriteBlock_Start.Text = "Start Block";
      this.Button_WriteBlock_Start.UseVisualStyleBackColor = true;
      this.Button_WriteBlock_Start.Click += new System.EventHandler(this.Button_WriteBlock_Start_Click);
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
      // TextBox_CommandWrite_FileName
      // 
      this.TextBox_CommandWrite_FileName.AllowDrop = true;
      this.TextBox_CommandWrite_FileName.Location = new System.Drawing.Point(30, 199);
      this.TextBox_CommandWrite_FileName.Name = "TextBox_CommandWrite_FileName";
      this.TextBox_CommandWrite_FileName.Size = new System.Drawing.Size(508, 20);
      this.TextBox_CommandWrite_FileName.TabIndex = 0;
      this.TextBox_CommandWrite_FileName.DragDrop += new System.Windows.Forms.DragEventHandler(this.TextBox_CommandWrite_FileName_DragDrop);
      this.TextBox_CommandWrite_FileName.DragEnter += new System.Windows.Forms.DragEventHandler(this.TextBox_CommandWrite_FileName_DragEnter);
      // 
      // TextBox_CommandWrite_FileOffset
      // 
      this.TextBox_CommandWrite_FileOffset.Location = new System.Drawing.Point(585, 199);
      this.TextBox_CommandWrite_FileOffset.Name = "TextBox_CommandWrite_FileOffset";
      this.TextBox_CommandWrite_FileOffset.Size = new System.Drawing.Size(58, 20);
      this.TextBox_CommandWrite_FileOffset.TabIndex = 0;
      this.TextBox_CommandWrite_FileOffset.Text = "0";
      this.TextBox_CommandWrite_FileOffset.TextChanged += new System.EventHandler(this.TextBox_CommandWrite_FileOffset_TextChanged);
      // 
      // TextBox_CommandWrite_Len
      // 
      this.TextBox_CommandWrite_Len.Location = new System.Drawing.Point(54, 32);
      this.TextBox_CommandWrite_Len.Name = "TextBox_CommandWrite_Len";
      this.TextBox_CommandWrite_Len.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandWrite_Len.TabIndex = 0;
      this.TextBox_CommandWrite_Len.Text = "0";
      this.TextBox_CommandWrite_Len.TextChanged += new System.EventHandler(this.TextBox_CommandWrite_Len_TextChanged);
      // 
      // TextBox_CommandWrite_Adr
      // 
      this.TextBox_CommandWrite_Adr.Location = new System.Drawing.Point(54, 6);
      this.TextBox_CommandWrite_Adr.Name = "TextBox_CommandWrite_Adr";
      this.TextBox_CommandWrite_Adr.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandWrite_Adr.TabIndex = 0;
      this.TextBox_CommandWrite_Adr.Text = "0";
      this.TextBox_CommandWrite_Adr.TextChanged += new System.EventHandler(this.TextBox_CommandWrite_Adr_TextChanged);
      // 
      // TabPage_Command_Read
      // 
      this.TabPage_Command_Read.Controls.Add(this.label20);
      this.TabPage_Command_Read.Controls.Add(this.TextBox_DiagRead);
      this.TabPage_Command_Read.Controls.Add(this.label3);
      this.TabPage_Command_Read.Controls.Add(this.TextBox_CommandReadBlock_File);
      this.TabPage_Command_Read.Controls.Add(this.label4);
      this.TabPage_Command_Read.Controls.Add(this.TextBox_CommandRead_Len);
      this.TabPage_Command_Read.Controls.Add(this.TextBox_CommandRead_Adr);
      this.TabPage_Command_Read.Controls.Add(this.label19);
      this.TabPage_Command_Read.Controls.Add(this.TextBox_CommandReadBlock_BlSize);
      this.TabPage_Command_Read.Controls.Add(this.Button_ReadBlock_Start);
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
      // TextBox_CommandReadBlock_File
      // 
      this.TextBox_CommandReadBlock_File.AllowDrop = true;
      this.TextBox_CommandReadBlock_File.Location = new System.Drawing.Point(31, 202);
      this.TextBox_CommandReadBlock_File.Name = "TextBox_CommandReadBlock_File";
      this.TextBox_CommandReadBlock_File.Size = new System.Drawing.Size(571, 20);
      this.TextBox_CommandReadBlock_File.TabIndex = 12;
      this.TextBox_CommandReadBlock_File.DragDrop += new System.Windows.Forms.DragEventHandler(this.TextBox_CommandReadBlock_File_DragDrop);
      this.TextBox_CommandReadBlock_File.DragEnter += new System.Windows.Forms.DragEventHandler(this.TextBox_CommandReadBlock_File_DragEnter);
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
      // TextBox_CommandRead_Len
      // 
      this.TextBox_CommandRead_Len.Location = new System.Drawing.Point(54, 32);
      this.TextBox_CommandRead_Len.Name = "TextBox_CommandRead_Len";
      this.TextBox_CommandRead_Len.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandRead_Len.TabIndex = 2;
      this.TextBox_CommandRead_Len.Text = "0";
      this.TextBox_CommandRead_Len.TextChanged += new System.EventHandler(this.TextBox_CommandRead_Len_TextChanged);
      // 
      // TextBox_CommandRead_Adr
      // 
      this.TextBox_CommandRead_Adr.Location = new System.Drawing.Point(54, 6);
      this.TextBox_CommandRead_Adr.Name = "TextBox_CommandRead_Adr";
      this.TextBox_CommandRead_Adr.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandRead_Adr.TabIndex = 3;
      this.TextBox_CommandRead_Adr.Text = "0";
      this.TextBox_CommandRead_Adr.TextChanged += new System.EventHandler(this.TextBox_CommandRead_Adr_TextChanged);
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
      // TextBox_CommandReadBlock_BlSize
      // 
      this.TextBox_CommandReadBlock_BlSize.Location = new System.Drawing.Point(54, 73);
      this.TextBox_CommandReadBlock_BlSize.Name = "TextBox_CommandReadBlock_BlSize";
      this.TextBox_CommandReadBlock_BlSize.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandReadBlock_BlSize.TabIndex = 7;
      this.TextBox_CommandReadBlock_BlSize.Text = "0";
      this.TextBox_CommandReadBlock_BlSize.TextChanged += new System.EventHandler(this.TextBox_CommandReadBlock_BlSize_TextChanged);
      // 
      // Button_ReadBlock_Start
      // 
      this.Button_ReadBlock_Start.Location = new System.Drawing.Point(6, 99);
      this.Button_ReadBlock_Start.Name = "Button_ReadBlock_Start";
      this.Button_ReadBlock_Start.Size = new System.Drawing.Size(115, 30);
      this.Button_ReadBlock_Start.TabIndex = 0;
      this.Button_ReadBlock_Start.Text = "Read Block";
      this.Button_ReadBlock_Start.UseVisualStyleBackColor = true;
      this.Button_ReadBlock_Start.Click += new System.EventHandler(this.Button_ReadBlock_Start_Click);
      // 
      // Button_CommandReadBlock_FileSave
      // 
      this.Button_CommandReadBlock_FileSave.Location = new System.Drawing.Point(608, 202);
      this.Button_CommandReadBlock_FileSave.Name = "Button_CommandReadBlock_FileSave";
      this.Button_CommandReadBlock_FileSave.Size = new System.Drawing.Size(40, 20);
      this.Button_CommandReadBlock_FileSave.TabIndex = 0;
      this.Button_CommandReadBlock_FileSave.Text = "Save";
      this.Button_CommandReadBlock_FileSave.UseVisualStyleBackColor = true;
      this.Button_CommandReadBlock_FileSave.Click += new System.EventHandler(this.Button_CommandReadBlock_FileSave_Click);
      // 
      // TabPage_Command_Checksum
      // 
      this.TabPage_Command_Checksum.Controls.Add(this.TextBox_DiagChecksum);
      this.TabPage_Command_Checksum.Controls.Add(this.label12);
      this.TabPage_Command_Checksum.Controls.Add(this.label16);
      this.TabPage_Command_Checksum.Controls.Add(this.label13);
      this.TabPage_Command_Checksum.Controls.Add(this.TextBox_CommandChk_Len);
      this.TabPage_Command_Checksum.Controls.Add(this.TextBox_CommandChk_Adr);
      this.TabPage_Command_Checksum.Location = new System.Drawing.Point(4, 22);
      this.TabPage_Command_Checksum.Name = "TabPage_Command_Checksum";
      this.TabPage_Command_Checksum.Size = new System.Drawing.Size(648, 222);
      this.TabPage_Command_Checksum.TabIndex = 6;
      this.TabPage_Command_Checksum.Text = "CheckSum";
      this.TabPage_Command_Checksum.UseVisualStyleBackColor = true;
      // 
      // TextBox_DiagChecksum
      // 
      this.TextBox_DiagChecksum.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TextBox_DiagChecksum.Location = new System.Drawing.Point(428, 6);
      this.TextBox_DiagChecksum.Name = "TextBox_DiagChecksum";
      this.TextBox_DiagChecksum.Size = new System.Drawing.Size(217, 18);
      this.TextBox_DiagChecksum.TabIndex = 10;
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
      // TextBox_CommandChk_Len
      // 
      this.TextBox_CommandChk_Len.Location = new System.Drawing.Point(53, 32);
      this.TextBox_CommandChk_Len.Name = "TextBox_CommandChk_Len";
      this.TextBox_CommandChk_Len.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandChk_Len.TabIndex = 6;
      this.TextBox_CommandChk_Len.Text = "0";
      this.TextBox_CommandChk_Len.TextChanged += new System.EventHandler(this.TextBox_CommandChk_Len_TextChanged);
      // 
      // TextBox_CommandChk_Adr
      // 
      this.TextBox_CommandChk_Adr.Location = new System.Drawing.Point(53, 6);
      this.TextBox_CommandChk_Adr.Name = "TextBox_CommandChk_Adr";
      this.TextBox_CommandChk_Adr.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandChk_Adr.TabIndex = 7;
      this.TextBox_CommandChk_Adr.Text = "0";
      this.TextBox_CommandChk_Adr.TextChanged += new System.EventHandler(this.TextBox_CommandChk_Adr_TextChanged);
      // 
      // TabPage_Command_Execute
      // 
      this.TabPage_Command_Execute.Controls.Add(this.label9);
      this.TabPage_Command_Execute.Controls.Add(this.label7);
      this.TabPage_Command_Execute.Controls.Add(this.TextBox_CommandExecute_Param4);
      this.TabPage_Command_Execute.Controls.Add(this.TextBox_CommandExecute_Param2);
      this.TabPage_Command_Execute.Controls.Add(this.label8);
      this.TabPage_Command_Execute.Controls.Add(this.label5);
      this.TabPage_Command_Execute.Controls.Add(this.label6);
      this.TabPage_Command_Execute.Controls.Add(this.TextBox_CommandExecute_Param3);
      this.TabPage_Command_Execute.Controls.Add(this.TextBox_CommandExecute_Param1);
      this.TabPage_Command_Execute.Controls.Add(this.TextBox_CommandExecute_Adr);
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
      // TextBox_CommandExecute_Param4
      // 
      this.TextBox_CommandExecute_Param4.Location = new System.Drawing.Point(314, 32);
      this.TextBox_CommandExecute_Param4.Name = "TextBox_CommandExecute_Param4";
      this.TextBox_CommandExecute_Param4.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandExecute_Param4.TabIndex = 10;
      this.TextBox_CommandExecute_Param4.Text = "0";
      this.TextBox_CommandExecute_Param4.TextChanged += new System.EventHandler(this.TextBox_CommandExecute_Param4_TextChanged);
      // 
      // TextBox_CommandExecute_Param2
      // 
      this.TextBox_CommandExecute_Param2.Location = new System.Drawing.Point(185, 32);
      this.TextBox_CommandExecute_Param2.Name = "TextBox_CommandExecute_Param2";
      this.TextBox_CommandExecute_Param2.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandExecute_Param2.TabIndex = 10;
      this.TextBox_CommandExecute_Param2.Text = "0";
      this.TextBox_CommandExecute_Param2.TextChanged += new System.EventHandler(this.TextBox_CommandExecute_Param2_TextChanged);
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
      // TextBox_CommandExecute_Param3
      // 
      this.TextBox_CommandExecute_Param3.Location = new System.Drawing.Point(314, 6);
      this.TextBox_CommandExecute_Param3.Name = "TextBox_CommandExecute_Param3";
      this.TextBox_CommandExecute_Param3.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandExecute_Param3.TabIndex = 6;
      this.TextBox_CommandExecute_Param3.Text = "0";
      this.TextBox_CommandExecute_Param3.TextChanged += new System.EventHandler(this.TextBox_CommandExecute_Param3_TextChanged);
      // 
      // TextBox_CommandExecute_Param1
      // 
      this.TextBox_CommandExecute_Param1.Location = new System.Drawing.Point(185, 6);
      this.TextBox_CommandExecute_Param1.Name = "TextBox_CommandExecute_Param1";
      this.TextBox_CommandExecute_Param1.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandExecute_Param1.TabIndex = 6;
      this.TextBox_CommandExecute_Param1.Text = "0";
      this.TextBox_CommandExecute_Param1.TextChanged += new System.EventHandler(this.TextBox_CommandExecute_Param1_TextChanged);
      // 
      // TextBox_CommandExecute_Adr
      // 
      this.TextBox_CommandExecute_Adr.Location = new System.Drawing.Point(57, 6);
      this.TextBox_CommandExecute_Adr.Name = "TextBox_CommandExecute_Adr";
      this.TextBox_CommandExecute_Adr.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandExecute_Adr.TabIndex = 7;
      this.TextBox_CommandExecute_Adr.Text = "0";
      this.TextBox_CommandExecute_Adr.TextChanged += new System.EventHandler(this.TextBox_CommandExecute_Adr_TextChanged);
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
      // TextBox_Command0
      // 
      this.TextBox_Command0.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.TextBox_Command0.Location = new System.Drawing.Point(9, 379);
      this.TextBox_Command0.Multiline = true;
      this.TextBox_Command0.Name = "TextBox_Command0";
      this.TextBox_Command0.Size = new System.Drawing.Size(833, 20);
      this.TextBox_Command0.TabIndex = 5;
      // 
      // Button_Clear_TextData
      // 
      this.Button_Clear_TextData.Location = new System.Drawing.Point(806, 408);
      this.Button_Clear_TextData.Name = "Button_Clear_TextData";
      this.Button_Clear_TextData.Size = new System.Drawing.Size(36, 19);
      this.Button_Clear_TextData.TabIndex = 9;
      this.Button_Clear_TextData.Text = "Clr";
      this.Button_Clear_TextData.UseVisualStyleBackColor = true;
      this.Button_Clear_TextData.Click += new System.EventHandler(this.Button_Clear_TextData_Click);
      // 
      // TextBox_DiagWorkRAM_Adr_Dez
      // 
      this.TextBox_DiagWorkRAM_Adr_Dez.Location = new System.Drawing.Point(676, 323);
      this.TextBox_DiagWorkRAM_Adr_Dez.Name = "TextBox_DiagWorkRAM_Adr_Dez";
      this.TextBox_DiagWorkRAM_Adr_Dez.Size = new System.Drawing.Size(80, 20);
      this.TextBox_DiagWorkRAM_Adr_Dez.TabIndex = 7;
      this.TextBox_DiagWorkRAM_Adr_Dez.Text = "---";
      // 
      // TextBox_DiagWorkRAM_Adr_Hex
      // 
      this.TextBox_DiagWorkRAM_Adr_Hex.Location = new System.Drawing.Point(676, 349);
      this.TextBox_DiagWorkRAM_Adr_Hex.Name = "TextBox_DiagWorkRAM_Adr_Hex";
      this.TextBox_DiagWorkRAM_Adr_Hex.Size = new System.Drawing.Size(80, 20);
      this.TextBox_DiagWorkRAM_Adr_Hex.TabIndex = 8;
      this.TextBox_DiagWorkRAM_Adr_Hex.Text = "---";
      // 
      // TextBox_DiagWorkRAM_Size_Dez
      // 
      this.TextBox_DiagWorkRAM_Size_Dez.Location = new System.Drawing.Point(762, 323);
      this.TextBox_DiagWorkRAM_Size_Dez.Name = "TextBox_DiagWorkRAM_Size_Dez";
      this.TextBox_DiagWorkRAM_Size_Dez.Size = new System.Drawing.Size(80, 20);
      this.TextBox_DiagWorkRAM_Size_Dez.TabIndex = 7;
      this.TextBox_DiagWorkRAM_Size_Dez.Text = "---";
      // 
      // TextBox_DiagWorkRAM_Size_Hex
      // 
      this.TextBox_DiagWorkRAM_Size_Hex.Location = new System.Drawing.Point(762, 349);
      this.TextBox_DiagWorkRAM_Size_Hex.Name = "TextBox_DiagWorkRAM_Size_Hex";
      this.TextBox_DiagWorkRAM_Size_Hex.Size = new System.Drawing.Size(80, 20);
      this.TextBox_DiagWorkRAM_Size_Hex.TabIndex = 8;
      this.TextBox_DiagWorkRAM_Size_Hex.Text = "---";
      // 
      // label14
      // 
      this.label14.AutoSize = true;
      this.label14.Location = new System.Drawing.Point(673, 307);
      this.label14.Name = "label14";
      this.label14.Size = new System.Drawing.Size(79, 13);
      this.label14.TabIndex = 10;
      this.label14.Text = "Work RAM Adr";
      // 
      // label15
      // 
      this.label15.AutoSize = true;
      this.label15.Location = new System.Drawing.Point(759, 307);
      this.label15.Name = "label15";
      this.label15.Size = new System.Drawing.Size(83, 13);
      this.label15.TabIndex = 11;
      this.label15.Text = "Work RAM Size";
      // 
      // Timer_Comport
      // 
      this.Timer_Comport.Interval = 10;
      this.Timer_Comport.Tick += new System.EventHandler(this.Timer_Comport_Tick);
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
      // TabPage_Command_Start
      // 
      this.TabPage_Command_Start.Controls.Add(this.label18);
      this.TabPage_Command_Start.Controls.Add(this.TextBox_CommandStart_Adr);
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
      // TextBox_CommandStart_Adr
      // 
      this.TextBox_CommandStart_Adr.Location = new System.Drawing.Point(57, 10);
      this.TextBox_CommandStart_Adr.Name = "TextBox_CommandStart_Adr";
      this.TextBox_CommandStart_Adr.Size = new System.Drawing.Size(67, 20);
      this.TextBox_CommandStart_Adr.TabIndex = 10;
      this.TextBox_CommandStart_Adr.Text = "0";
      this.TextBox_CommandStart_Adr.TextChanged += new System.EventHandler(this.TextBox_CommandStart_Adr_TextChanged);
      // 
      // Form1
      // 
      this.AllowDrop = true;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(848, 636);
      this.Controls.Add(this.Lbl_InputCnt);
      this.Controls.Add(this.label15);
      this.Controls.Add(this.label14);
      this.Controls.Add(this.Button_Clear_TextData);
      this.Controls.Add(this.TextBox_DiagWorkRAM_Size_Hex);
      this.Controls.Add(this.TextBox_DiagWorkRAM_Adr_Hex);
      this.Controls.Add(this.TextBox_CalcHex);
      this.Controls.Add(this.TextBox_DiagWorkRAM_Size_Dez);
      this.Controls.Add(this.TextBox_DiagWorkRAM_Adr_Dez);
      this.Controls.Add(this.TextBox_CalcDez);
      this.Controls.Add(this.TabControl_Command);
      this.Controls.Add(this.TextBox_Command0_7);
      this.Controls.Add(this.TextBox_Command0_5);
      this.Controls.Add(this.TextBox_Command0);
      this.Controls.Add(this.TextBox_Command0_8);
      this.Controls.Add(this.TextBox_Command0_3);
      this.Controls.Add(this.TextBox_Command0_6);
      this.Controls.Add(this.TextBox_Command0_4);
      this.Controls.Add(this.TextBox_Command0_2);
      this.Controls.Add(this.TextBox_Command0_1);
      this.Controls.Add(this.Label_BreakStatus);
      this.Controls.Add(this.Label_RIStatus);
      this.Controls.Add(this.Lable_DSRStatus);
      this.Controls.Add(this.Label_CTSStatus);
      this.Controls.Add(this.TextError);
      this.Controls.Add(this.Text_Data);
      this.Controls.Add(this.ComboBox_HandShaking);
      this.Controls.Add(this.ComboBox_Parity);
      this.Controls.Add(this.ComboBox_StopBits);
      this.Controls.Add(this.ComboBox_DataBits);
      this.Controls.Add(this.ComboBox_Baud);
      this.Controls.Add(this.ComboBox_Ports);
      this.Controls.Add(this.Button_Send);
      this.Controls.Add(this.Btn_PortState);
      this.Controls.Add(this.Btn_Ports);
      this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Name = "Form1";
      this.Text = "Form1";
      this.Load += new System.EventHandler(this.Form1_Load);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
      this.TabControl_Command.ResumeLayout(false);
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
    private System.Windows.Forms.TextBox Text_Data;
    private System.Windows.Forms.Label Label_CTSStatus;
    private System.Windows.Forms.Label Lable_DSRStatus;
    private System.Windows.Forms.Label Label_RIStatus;
    private System.Windows.Forms.Label Label_BreakStatus;
    private System.Windows.Forms.TextBox TextError;
    private System.Windows.Forms.Button Button_Send;
    private System.Windows.Forms.TextBox TextBox_Command0_1;
    private System.Windows.Forms.TextBox TextBox_Command0_2;
    private System.Windows.Forms.TextBox TextBox_Command0_3;
    private System.Windows.Forms.TextBox TextBox_Command0_4;
    private System.Windows.Forms.TextBox TextBox_Command0_5;
    private System.Windows.Forms.TextBox TextBox_Command0_6;
    private System.Windows.Forms.TextBox TextBox_Command0_8;
    private System.Windows.Forms.TextBox TextBox_Command0_7;
    private System.Windows.Forms.TabControl TabControl_Command;
    private System.Windows.Forms.TabPage TabPage_Command_Info1;
    private System.Windows.Forms.TabPage TabPage_Command_Info2;
    private System.Windows.Forms.TabPage TabPage_Command_Write;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.TextBox TextBox_CommandWrite_Len;
    private System.Windows.Forms.TextBox TextBox_CommandWrite_Adr;
    private System.Windows.Forms.TabPage TabPage_Command_Read;
    private System.Windows.Forms.TabPage TabPage_Command_Execute;
    private System.Windows.Forms.TabPage TabPage_Command_Exit;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.TextBox TextBox_CommandRead_Len;
    private System.Windows.Forms.TextBox TextBox_CommandRead_Adr;
    private System.Windows.Forms.Label label9;
    private System.Windows.Forms.Label label7;
    private System.Windows.Forms.TextBox TextBox_CommandExecute_Param4;
    private System.Windows.Forms.TextBox TextBox_CommandExecute_Param2;
    private System.Windows.Forms.Label label8;
    private System.Windows.Forms.Label label5;
    private System.Windows.Forms.Label label6;
    private System.Windows.Forms.TextBox TextBox_CommandExecute_Param3;
    private System.Windows.Forms.TextBox TextBox_CommandExecute_Param1;
    private System.Windows.Forms.TextBox TextBox_CommandExecute_Adr;
    private System.Windows.Forms.TextBox TextBox_CalcHex;
    private System.Windows.Forms.TextBox TextBox_CalcDez;
    private System.Windows.Forms.TextBox TextBox_Command0;
    private System.Windows.Forms.Button Button_Clear_TextData;
    private System.Windows.Forms.Label label10;
    private System.Windows.Forms.Label label11;
    private System.Windows.Forms.TextBox TextBox_CommandWrite_FileName;
    private System.Windows.Forms.TextBox TextBox_CommandWrite_FileOffset;
    private System.Windows.Forms.TabPage TabPage_Command_Checksum;
    private System.Windows.Forms.Label label12;
    private System.Windows.Forms.Label label13;
    private System.Windows.Forms.TextBox TextBox_CommandChk_Len;
    private System.Windows.Forms.TextBox TextBox_CommandChk_Adr;
    private System.Windows.Forms.TextBox TextBox_DiagInfo0;
    private System.Windows.Forms.TextBox TextBox_DiagInfo1;
    private System.Windows.Forms.TextBox TextBox_DiagWorkRAM_Adr_Dez;
    private System.Windows.Forms.TextBox TextBox_DiagWorkRAM_Adr_Hex;
    private System.Windows.Forms.TextBox TextBox_DiagWorkRAM_Size_Dez;
    private System.Windows.Forms.TextBox TextBox_DiagWorkRAM_Size_Hex;
    private System.Windows.Forms.Label label14;
    private System.Windows.Forms.Label label15;
    private System.Windows.Forms.TextBox TextBox_DiagChecksum;
    private System.Windows.Forms.TextBox TextBox_DiagRead;
    private System.Windows.Forms.TextBox TextBox_DiagWrite;
    private System.Windows.Forms.Label label16;
    private System.Windows.Forms.Timer Timer_Comport;
    private System.Windows.Forms.Label label20;
    private System.Windows.Forms.TextBox TextBox_CommandReadBlock_File;
    private System.Windows.Forms.Label label19;
    private System.Windows.Forms.TextBox TextBox_CommandReadBlock_BlSize;
    private System.Windows.Forms.Button Button_ReadBlock_Start;
    private System.Windows.Forms.Button Button_CommandReadBlock_FileSave;
    private System.Windows.Forms.Label label17;
    private System.Windows.Forms.TextBox TextBox_CommandWriteBlock_BlSize;
    private System.Windows.Forms.Button Button_WriteBlock_Start;
    private System.Windows.Forms.Label Lbl_InputCnt;
    private System.Windows.Forms.TabPage TabPage_Command_Start;
    private System.Windows.Forms.Label label18;
    private System.Windows.Forms.TextBox TextBox_CommandStart_Adr;
  }
}

