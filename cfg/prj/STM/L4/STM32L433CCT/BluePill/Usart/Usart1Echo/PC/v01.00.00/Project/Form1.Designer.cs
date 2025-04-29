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
      this.Lbl_InputCnt = new System.Windows.Forms.Label();
      this.Com_Text_ConIO = new System.Windows.Forms.TextBox();
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
      this.Lable_DSRStatus.Location = new System.Drawing.Point(514, 26);
      this.Lable_DSRStatus.Name = "Lable_DSRStatus";
      this.Lable_DSRStatus.Size = new System.Drawing.Size(30, 13);
      this.Lable_DSRStatus.TabIndex = 3;
      this.Lable_DSRStatus.Text = "DSR";
      // 
      // Label_RIStatus
      // 
      this.Label_RIStatus.AutoSize = true;
      this.Label_RIStatus.Location = new System.Drawing.Point(514, 39);
      this.Label_RIStatus.Name = "Label_RIStatus";
      this.Label_RIStatus.Size = new System.Drawing.Size(18, 13);
      this.Label_RIStatus.TabIndex = 3;
      this.Label_RIStatus.Text = "RI";
      // 
      // Label_BreakStatus
      // 
      this.Label_BreakStatus.AutoSize = true;
      this.Label_BreakStatus.Location = new System.Drawing.Point(515, 52);
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
      // Lbl_InputCnt
      // 
      this.Lbl_InputCnt.AutoSize = true;
      this.Lbl_InputCnt.Location = new System.Drawing.Point(515, 65);
      this.Lbl_InputCnt.Name = "Lbl_InputCnt";
      this.Lbl_InputCnt.Size = new System.Drawing.Size(13, 13);
      this.Lbl_InputCnt.TabIndex = 12;
      this.Lbl_InputCnt.Text = "0";
      // 
      // Com_Text_ConIO
      // 
      this.Com_Text_ConIO.Font = new System.Drawing.Font("Lucida Console", 6.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.Com_Text_ConIO.Location = new System.Drawing.Point(12, 125);
      this.Com_Text_ConIO.MaxLength = 0;
      this.Com_Text_ConIO.Multiline = true;
      this.Com_Text_ConIO.Name = "Com_Text_ConIO";
      this.Com_Text_ConIO.Size = new System.Drawing.Size(496, 62);
      this.Com_Text_ConIO.TabIndex = 25;
      this.Com_Text_ConIO.WordWrap = false;
      // 
      // Form1
      // 
      this.AllowDrop = true;
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(567, 204);
      this.Controls.Add(this.Com_Text_ConIO);
      this.Controls.Add(this.Lbl_InputCnt);
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
    private System.Windows.Forms.Label Lbl_InputCnt;
    private System.Windows.Forms.TextBox Com_Text_ConIO;
  }
}

