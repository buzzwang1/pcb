#include "lcd.h"

//////////////////////////////////////////////////////////////////////////////////

//2.4��/2.8��/3.5��/4.3�� TFTҺ������
//֧������IC�ͺŰ���:ILI9341/ILI9325/RM68042/RM68021/ILI9320/ILI9328/LGDP4531/LGDP4535/
//                   SPFD5408/1505/B505/C505/NT35310/NT35510��
//STM32F4����-�⺯���汾
//********************************************************************************

//////////////////////////////////////////////////////////////////////////////////

_lcd_dev lcddev;

void LCD_WR_REG(vu16 regval)
{
  regval = regval;
  LCD->LCD_REG = regval;
}

void LCD_WR_DATA(vu16 data)
{
  data = data;
  LCD->LCD_RAM = data;
}

u16 LCD_RD_DATA(void)
{
  vu16 ram;
  ram = LCD->LCD_RAM;
  return ram;
}

void LCD_WriteReg(vu16 LCD_Reg, vu16 LCD_RegValue)
{
  LCD->LCD_REG = LCD_Reg;
  LCD->LCD_RAM = LCD_RegValue;
}

u16 LCD_ReadReg(vu16 LCD_Reg)
{
  LCD_WR_REG(LCD_Reg);
  cClockInfo::Delay_us(5);
  return LCD_RD_DATA();
}

void LCD_WriteRAM_Prepare(void)
{
  LCD->LCD_REG = lcddev.wramcmd;
}


void LCD_WriteRAM(u16 RGB_Code)
{
  LCD->LCD_RAM = RGB_Code;
}


u16 LCD_BGR2RGB(u16 c)
{
  u16  r, g, b, rgb;
  b = (c >> 0) & 0x1f;
  g = (c >> 5) & 0x3f;
  r = (c >> 11) & 0x1f;
  rgb = (b << 11) + (g << 5) + (r << 0);
  return(rgb);
}



void LCD_DisplayOn(void)
{
  if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310)LCD_WR_REG(0X29);
  else if (lcddev.id == 0X5510)LCD_WR_REG(0X2900);
  else LCD_WriteReg(R7, 0x0173);
}

void LCD_DisplayOff(void)
{
  if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310)LCD_WR_REG(0X28);
  else if (lcddev.id == 0X5510)LCD_WR_REG(0X2800);
  else LCD_WriteReg(R7, 0x0);
}


void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
  if (lcddev.id == 0X9341 || lcddev.id == 0X5310)
  {
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0XFF);
  }
  else if (lcddev.id == 0X6804)
  {
    if (lcddev.dir == 1)Xpos = lcddev.width - 1 - Xpos;
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_DATA(Ypos & 0XFF);
  }
  else if (lcddev.id == 0X5510)
  {
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(Xpos >> 8);
    LCD_WR_REG(lcddev.setxcmd + 1);
    LCD_WR_DATA(Xpos & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(Ypos >> 8);
    LCD_WR_REG(lcddev.setycmd + 1);
    LCD_WR_DATA(Ypos & 0XFF);
  }
  else
  {
    if (lcddev.dir == 1)Xpos = lcddev.width - 1 - Xpos;
    LCD_WriteReg(lcddev.setxcmd, Xpos);
    LCD_WriteReg(lcddev.setycmd, Ypos);
  }
}

void LCD_Display_Dir(u8 dir)
{
  if (dir == 0)			//����
  {
    lcddev.dir = 0;	//����
    lcddev.width = 240;
    lcddev.height = 320;
    if (lcddev.id == 0X9341 || lcddev.id == 0X6804 || lcddev.id == 0X5310)
    {
      lcddev.wramcmd = 0X2C;
      lcddev.setxcmd = 0X2A;
      lcddev.setycmd = 0X2B;
      if (lcddev.id == 0X6804 || lcddev.id == 0X5310)
      {
        lcddev.width = 320;
        lcddev.height = 480;
      }
    }
    else if (lcddev.id == 0x5510)
    {
      lcddev.wramcmd = 0X2C00;
      lcddev.setxcmd = 0X2A00;
      lcddev.setycmd = 0X2B00;
      lcddev.width = 480;
      lcddev.height = 800;
    }
    else
    {
      lcddev.wramcmd = R34;
      lcddev.setxcmd = R32;
      lcddev.setycmd = R33;
    }
  }
  else 				//����
  {
    lcddev.dir = 1;	//����
    lcddev.width = 320;
    lcddev.height = 240;
    if (lcddev.id == 0X9341 || lcddev.id == 0X5310)
    {
      lcddev.wramcmd = 0X2C;
      lcddev.setxcmd = 0X2A;
      lcddev.setycmd = 0X2B;
    }
    else if (lcddev.id == 0X6804)
    {
      lcddev.wramcmd = 0X2C;
      lcddev.setxcmd = 0X2B;
      lcddev.setycmd = 0X2A;
    }
    else if (lcddev.id == 0x5510)
    {
      lcddev.wramcmd = 0X2C00;
      lcddev.setxcmd = 0X2A00;
      lcddev.setycmd = 0X2B00;
      lcddev.width = 800;
      lcddev.height = 480;
    }
    else
    {
      lcddev.wramcmd = R34;
      lcddev.setxcmd = R33;
      lcddev.setycmd = R32;
    }
    if (lcddev.id == 0X6804 || lcddev.id == 0X5310)
    {
      lcddev.width = 480;
      lcddev.height = 320;
    }
  }
  //LCD_Scan_Dir(DFT_SCAN_DIR);	//Ĭ��ɨ�跽��
}

void LCD_Init(void)
{
	vu32 i=0;

  GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��PD,PE,PF,PGʱ��
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//ʹ��FSMCʱ��


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB15 �������,���Ʊ���
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ�� //PB15 �������,���Ʊ���

  GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(7<<8)|(3<<14);//PD0,1,4,5,8,9,10,14,15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��

  GPIO_InitStructure.GPIO_Pin = (0X1FF<<7);//PE7~15,AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//�������
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOG, &GPIO_InitStructure);//��ʼ��

  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);//PD0,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);//PD1,AF12
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);//PD15,AF12

  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);//PE7,AF12
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);//PE15,AF12

  GPIO_PinAFConfig(GPIOF,GPIO_PinSource12,GPIO_AF_FSMC);//PF12,AF12
  GPIO_PinAFConfig(GPIOG,GPIO_PinSource12,GPIO_AF_FSMC);


  readWriteTiming.FSMC_AddressSetupTime = 0XF;	 //��ַ����ʱ�䣨ADDSET��Ϊ16��HCLK 1/168M=6ns*16=96ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨ADDHLD��ģʽAδ�õ�
  readWriteTiming.FSMC_DataSetupTime = 24;			//���ݱ���ʱ��Ϊ25��HCLK	=6*25=150ns
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA


	writeTiming.FSMC_AddressSetupTime =8;	      //��ַ����ʱ�䣨ADDSET��Ϊ8��HCLK =48ns
  writeTiming.FSMC_AddressHoldTime = 0x00;	 //��ַ����ʱ�䣨A
  writeTiming.FSMC_DataSetupTime = 8;		 //���ݱ���ʱ��Ϊ6ns*9��HCLK=54ns
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //ģʽA


  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  ��������ʹ��NE4 ��Ҳ�Ͷ�ӦBTCR[6],[7]��
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // ���������ݵ�ַ
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//�洢�����ݿ��Ϊ16bit
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  �洢��дʹ��
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // ��дʹ�ò�ͬ��ʱ��
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //��дʱ��
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //дʱ��

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //��ʼ��FSMC����

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // ʹ��BANK1


  cClockInfo::Delay_ms(50); // delay 50 ms
 	LCD_WriteReg(0x0000,0x0001);
  cClockInfo::Delay_ms(50); // delay 50 ms

  lcddev.id = LCD_ReadReg(0x0000);

  if (lcddev.id < 0XFF || lcddev.id == 0XFFFF || lcddev.id == 0X9300)
  {
    LCD_WR_REG(0XD3);
    lcddev.id = LCD_RD_DATA();   //dummy read
    lcddev.id = LCD_RD_DATA();   //0X00
    lcddev.id = LCD_RD_DATA();   //93
    lcddev.id <<= 8;
    lcddev.id |= LCD_RD_DATA();  //
    if (lcddev.id != 0X9341)     //�9341
    {
      LCD_WR_REG(0XBF);
      lcddev.id = LCD_RD_DATA();//dummy read
      lcddev.id = LCD_RD_DATA();//�0X01
      lcddev.id = LCD_RD_DATA();//�0XD0
      lcddev.id = LCD_RD_DATA();//�0X68
      lcddev.id <<= 8;
      lcddev.id |= LCD_RD_DATA();//�������0X04
      if (lcddev.id != 0X6804)	//Ҳ����6804,���Կ����ǲ���NT35310
      {
        LCD_WR_REG(0XD4);
        lcddev.id = LCD_RD_DATA();	//dummy read
        lcddev.id = LCD_RD_DATA();	//����0X01
        lcddev.id = LCD_RD_DATA();	//����0X53
        lcddev.id <<= 8;
        lcddev.id |= LCD_RD_DATA();	//�������0X10
        if (lcddev.id != 0X5310)		//Ҳ����NT35310,���Կ����ǲ���NT35510
        {
          LCD_WR_REG(0XDA00);
          lcddev.id = LCD_RD_DATA();//����0X00
          LCD_WR_REG(0XDB00);
          lcddev.id = LCD_RD_DATA();//����0X80
          lcddev.id <<= 8;
          LCD_WR_REG(0XDC00);
          lcddev.id |= LCD_RD_DATA();//����0X00
          if (lcddev.id == 0x8000)lcddev.id = 0x5510;//NT35510���ص�ID��8000H,Ϊ��������,����ǿ������Ϊ5510
        }
      }
    }
  }

  if (lcddev.id == 0X9341 || lcddev.id == 0X5310 || lcddev.id == 0X5510)//�����������IC,������WRʱ��Ϊ���
  {
    //��������дʱ����ƼĴ�����ʱ��
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 0);//��ַ����ʱ�䣨ADDSET������
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 8);//���ݱ���ʱ������
    FSMC_Bank1E->BWTR[6] |= 3 << 0;		//��ַ����ʱ�䣨ADDSET��Ϊ3��HCLK =18ns
    FSMC_Bank1E->BWTR[6] |= 2 << 8; 	//���ݱ���ʱ��Ϊ6ns*3��HCLK=18ns
  }
  else if (lcddev.id == 0X6804 || lcddev.id == 0XC505)	//6804/C505�ٶ��ϲ�ȥ,�ý���
  {
    //��������дʱ����ƼĴ�����ʱ��
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 0);//��ַ����ʱ�䣨ADDSET������
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 8);//���ݱ���ʱ������
    FSMC_Bank1E->BWTR[6] |= 10 << 0;	//��ַ����ʱ�䣨ADDSET��Ϊ10��HCLK =60ns
    FSMC_Bank1E->BWTR[6] |= 12 << 8; 	//���ݱ���ʱ��Ϊ6ns*13��HCLK=96ns
  }

  if (lcddev.id == 0X9341)	//9341��ʼ��
  {
    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    i++;
    LCD_WR_DATA(0xC1);
    LCD_WR_DATA(0X30);
    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0X12);
    LCD_WR_DATA(0X81);
    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x85);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x7A);
    LCD_WR_REG(0xCB);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x02);
    LCD_WR_REG(0xF7);
    LCD_WR_DATA(0x20);
    LCD_WR_REG(0xEA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xC0);    //Power control
    LCD_WR_DATA(0x1B);   //VRH[5:0]
    LCD_WR_REG(0xC1);    //Power control
    LCD_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
    LCD_WR_REG(0xC5);    //VCM control
    LCD_WR_DATA(0x30); 	 //3F
    LCD_WR_DATA(0x30); 	 //3C
    LCD_WR_REG(0xC7);    //VCM control2
    LCD_WR_DATA(0XB7);
    LCD_WR_REG(0x36);    // Memory Access Control
    LCD_WR_DATA(0x48);
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1A);
    LCD_WR_REG(0xB6);    // Display Function Control
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0xA2);
    LCD_WR_REG(0xF2);    // 3Gamma Function Disable
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x26);    //Gamma curve selected
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xE0);    //Set Gamma
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x28);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x08);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0XA9);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0XE1);    //Set Gamma
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x15);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x2B);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x0F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x2B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x3f);
    LCD_WR_REG(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xef);
    LCD_WR_REG(0x11); //Exit Sleep
    cClockInfo::Delay_ms(120);
    LCD_WR_REG(0x29); //display on
  }
  else if (lcddev.id == 0x6804) //6804��ʼ��
  {
    LCD_WR_REG(0X11);
    cClockInfo::Delay_ms(20);
    LCD_WR_REG(0XD0);//VCI1  VCL  VGH  VGL DDVDH VREG1OUT power amplitude setting
    LCD_WR_DATA(0X07);
    LCD_WR_DATA(0X42);
    LCD_WR_DATA(0X1D);
    LCD_WR_REG(0XD1);//VCOMH VCOM_AC amplitude setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X1a);
    LCD_WR_DATA(0X09);
    LCD_WR_REG(0XD2);//Operational Amplifier Circuit Constant Current Adjust , charge pump frequency setting
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X22);
    LCD_WR_REG(0XC0);//REV SM GS
    LCD_WR_DATA(0X10);
    LCD_WR_DATA(0X3B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);
    LCD_WR_DATA(0X11);

    LCD_WR_REG(0XC5);// Frame rate setting = 72HZ  when setting 0x03
    LCD_WR_DATA(0X03);

    LCD_WR_REG(0XC8);//Gamma setting
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X25);
    LCD_WR_DATA(0X21);
    LCD_WR_DATA(0X05);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X0a);
    LCD_WR_DATA(0X65);
    LCD_WR_DATA(0X25);
    LCD_WR_DATA(0X77);
    LCD_WR_DATA(0X50);
    LCD_WR_DATA(0X0f);
    LCD_WR_DATA(0X00);

    LCD_WR_REG(0XF8);
    LCD_WR_DATA(0X01);

    LCD_WR_REG(0XFE);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X02);

    LCD_WR_REG(0X20);//Exit invert mode

    LCD_WR_REG(0X36);
    LCD_WR_DATA(0X08);//ԭ����a

    LCD_WR_REG(0X3A);
    LCD_WR_DATA(0X55);//16λģʽ
    LCD_WR_REG(0X2B);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0X3F);

    LCD_WR_REG(0X2A);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X00);
    LCD_WR_DATA(0X01);
    LCD_WR_DATA(0XDF);
    cClockInfo::Delay_ms(120);
    LCD_WR_REG(0X29);
  }
  else if (lcddev.id == 0x5310)
  {
    LCD_WR_REG(0xED);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0xFE);

    LCD_WR_REG(0xEE);
    LCD_WR_DATA(0xDE);
    LCD_WR_DATA(0x21);

    LCD_WR_REG(0xF1);
    LCD_WR_DATA(0x01);
    LCD_WR_REG(0xDF);
    LCD_WR_DATA(0x10);

    //VCOMvoltage//
    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x8F);	  //5f

    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE2);
    LCD_WR_DATA(0xE2);
    LCD_WR_DATA(0xE2);
    LCD_WR_REG(0xBF);
    LCD_WR_DATA(0xAA);

    LCD_WR_REG(0xB0);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x19);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0x80);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x96);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x96);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA1);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB5);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x03);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5E);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xAC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x70);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x90);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xEB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xB8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xBA);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x54);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xFF);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC2);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC3);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x39);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x37);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x26);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x24);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC4);
    LCD_WR_DATA(0x62);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x95);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE6);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC5);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x65);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC6);
    LCD_WR_DATA(0x20);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xC9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x21);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x46);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x52);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x7A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE1);
    LCD_WR_DATA(0x16);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x22);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x52);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x64);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x7A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x8B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB9);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE2);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4F);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x61);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x79);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x97);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE3);
    LCD_WR_DATA(0x05);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x1C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x62);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x78);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x97);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA6);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC7);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE4);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x01);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x2A);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x74);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x93);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0xE5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x3C);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x5D);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x74);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x84);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x93);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xA2);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xB3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xC4);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xCD);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xD3);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xDC);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE6);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x34);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE7);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x76);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x67);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x67);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x87);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x56);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x23);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x33);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE8);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x87);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x77);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x88);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xAA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0xBB);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x99);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x66);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x44);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x55);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0xAA);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x00);
    LCD_WR_DATA(0xAA);

    LCD_WR_REG(0xCF);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x50);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF3);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0xF9);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x29);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x55);	//66

    LCD_WR_REG(0x11);
    cClockInfo::Delay_ms(100);
    LCD_WR_REG(0x29);
    LCD_WR_REG(0x35);
    LCD_WR_DATA(0x00);

    LCD_WR_REG(0x51);
    LCD_WR_DATA(0xFF);
    LCD_WR_REG(0x53);
    LCD_WR_DATA(0x2C);
    LCD_WR_REG(0x55);
    LCD_WR_DATA(0x82);
    LCD_WR_REG(0x2c);
  }
  else if (lcddev.id == 0x5510)
  {
  LCD_WriteReg(0xF000, 0x55);
  LCD_WriteReg(0xF001, 0xAA);
  LCD_WriteReg(0xF002, 0x52);
  LCD_WriteReg(0xF003, 0x08);
  LCD_WriteReg(0xF004, 0x01);
  //AVDD Set AVDD 5.2V
  LCD_WriteReg(0xB000, 0x0D);
  LCD_WriteReg(0xB001, 0x0D);
  LCD_WriteReg(0xB002, 0x0D);
  //AVDD ratio
  LCD_WriteReg(0xB600, 0x34);
  LCD_WriteReg(0xB601, 0x34);
  LCD_WriteReg(0xB602, 0x34);
  //AVEE -5.2V
  LCD_WriteReg(0xB100, 0x0D);
  LCD_WriteReg(0xB101, 0x0D);
  LCD_WriteReg(0xB102, 0x0D);
  //AVEE ratio
  LCD_WriteReg(0xB700, 0x34);
  LCD_WriteReg(0xB701, 0x34);
  LCD_WriteReg(0xB702, 0x34);
  //VCL -2.5V
  LCD_WriteReg(0xB200, 0x00);
  LCD_WriteReg(0xB201, 0x00);
  LCD_WriteReg(0xB202, 0x00);
  //VCL ratio
  LCD_WriteReg(0xB800, 0x24);
  LCD_WriteReg(0xB801, 0x24);
  LCD_WriteReg(0xB802, 0x24);
  //VGH 15V (Free pump)
  LCD_WriteReg(0xBF00, 0x01);
  LCD_WriteReg(0xB300, 0x0F);
  LCD_WriteReg(0xB301, 0x0F);
  LCD_WriteReg(0xB302, 0x0F);
  //VGH ratio
  LCD_WriteReg(0xB900, 0x34);
  LCD_WriteReg(0xB901, 0x34);
  LCD_WriteReg(0xB902, 0x34);
  //VGL_REG -10V
  LCD_WriteReg(0xB500, 0x08);
  LCD_WriteReg(0xB501, 0x08);
  LCD_WriteReg(0xB502, 0x08);
  LCD_WriteReg(0xC200, 0x03);
  //VGLX ratio
  LCD_WriteReg(0xBA00, 0x24);
  LCD_WriteReg(0xBA01, 0x24);
  LCD_WriteReg(0xBA02, 0x24);
  //VGMP/VGSP 4.5V/0V
  LCD_WriteReg(0xBC00, 0x00);
  LCD_WriteReg(0xBC01, 0x78);
  LCD_WriteReg(0xBC02, 0x00);
  //VGMN/VGSN -4.5V/0V
  LCD_WriteReg(0xBD00, 0x00);
  LCD_WriteReg(0xBD01, 0x78);
  LCD_WriteReg(0xBD02, 0x00);
  //VCOM
  LCD_WriteReg(0xBE00, 0x00);
  LCD_WriteReg(0xBE01, 0x64);
  //Gamma Setting
  LCD_WriteReg(0xD100, 0x00);
  LCD_WriteReg(0xD101, 0x33);
  LCD_WriteReg(0xD102, 0x00);
  LCD_WriteReg(0xD103, 0x34);
  LCD_WriteReg(0xD104, 0x00);
  LCD_WriteReg(0xD105, 0x3A);
  LCD_WriteReg(0xD106, 0x00);
  LCD_WriteReg(0xD107, 0x4A);
  LCD_WriteReg(0xD108, 0x00);
  LCD_WriteReg(0xD109, 0x5C);
  LCD_WriteReg(0xD10A, 0x00);
  LCD_WriteReg(0xD10B, 0x81);
  LCD_WriteReg(0xD10C, 0x00);
  LCD_WriteReg(0xD10D, 0xA6);
  LCD_WriteReg(0xD10E, 0x00);
  LCD_WriteReg(0xD10F, 0xE5);
  LCD_WriteReg(0xD110, 0x01);
  LCD_WriteReg(0xD111, 0x13);
  LCD_WriteReg(0xD112, 0x01);
  LCD_WriteReg(0xD113, 0x54);
  LCD_WriteReg(0xD114, 0x01);
  LCD_WriteReg(0xD115, 0x82);
  LCD_WriteReg(0xD116, 0x01);
  LCD_WriteReg(0xD117, 0xCA);
  LCD_WriteReg(0xD118, 0x02);
  LCD_WriteReg(0xD119, 0x00);
  LCD_WriteReg(0xD11A, 0x02);
  LCD_WriteReg(0xD11B, 0x01);
  LCD_WriteReg(0xD11C, 0x02);
  LCD_WriteReg(0xD11D, 0x34);
  LCD_WriteReg(0xD11E, 0x02);
  LCD_WriteReg(0xD11F, 0x67);
  LCD_WriteReg(0xD120, 0x02);
  LCD_WriteReg(0xD121, 0x84);
  LCD_WriteReg(0xD122, 0x02);
  LCD_WriteReg(0xD123, 0xA4);
  LCD_WriteReg(0xD124, 0x02);
  LCD_WriteReg(0xD125, 0xB7);
  LCD_WriteReg(0xD126, 0x02);
  LCD_WriteReg(0xD127, 0xCF);
  LCD_WriteReg(0xD128, 0x02);
  LCD_WriteReg(0xD129, 0xDE);
  LCD_WriteReg(0xD12A, 0x02);
  LCD_WriteReg(0xD12B, 0xF2);
  LCD_WriteReg(0xD12C, 0x02);
  LCD_WriteReg(0xD12D, 0xFE);
  LCD_WriteReg(0xD12E, 0x03);
  LCD_WriteReg(0xD12F, 0x10);
  LCD_WriteReg(0xD130, 0x03);
  LCD_WriteReg(0xD131, 0x33);
  LCD_WriteReg(0xD132, 0x03);
  LCD_WriteReg(0xD133, 0x6D);
  LCD_WriteReg(0xD200, 0x00);
  LCD_WriteReg(0xD201, 0x33);
  LCD_WriteReg(0xD202, 0x00);
  LCD_WriteReg(0xD203, 0x34);
  LCD_WriteReg(0xD204, 0x00);
  LCD_WriteReg(0xD205, 0x3A);
  LCD_WriteReg(0xD206, 0x00);
  LCD_WriteReg(0xD207, 0x4A);
  LCD_WriteReg(0xD208, 0x00);
  LCD_WriteReg(0xD209, 0x5C);
  LCD_WriteReg(0xD20A, 0x00);

  LCD_WriteReg(0xD20B, 0x81);
  LCD_WriteReg(0xD20C, 0x00);
  LCD_WriteReg(0xD20D, 0xA6);
  LCD_WriteReg(0xD20E, 0x00);
  LCD_WriteReg(0xD20F, 0xE5);
  LCD_WriteReg(0xD210, 0x01);
  LCD_WriteReg(0xD211, 0x13);
  LCD_WriteReg(0xD212, 0x01);
  LCD_WriteReg(0xD213, 0x54);
  LCD_WriteReg(0xD214, 0x01);
  LCD_WriteReg(0xD215, 0x82);
  LCD_WriteReg(0xD216, 0x01);
  LCD_WriteReg(0xD217, 0xCA);
  LCD_WriteReg(0xD218, 0x02);
  LCD_WriteReg(0xD219, 0x00);
  LCD_WriteReg(0xD21A, 0x02);
  LCD_WriteReg(0xD21B, 0x01);
  LCD_WriteReg(0xD21C, 0x02);
  LCD_WriteReg(0xD21D, 0x34);
  LCD_WriteReg(0xD21E, 0x02);
  LCD_WriteReg(0xD21F, 0x67);
  LCD_WriteReg(0xD220, 0x02);
  LCD_WriteReg(0xD221, 0x84);
  LCD_WriteReg(0xD222, 0x02);
  LCD_WriteReg(0xD223, 0xA4);
  LCD_WriteReg(0xD224, 0x02);
  LCD_WriteReg(0xD225, 0xB7);
  LCD_WriteReg(0xD226, 0x02);
  LCD_WriteReg(0xD227, 0xCF);
  LCD_WriteReg(0xD228, 0x02);
  LCD_WriteReg(0xD229, 0xDE);
  LCD_WriteReg(0xD22A, 0x02);
  LCD_WriteReg(0xD22B, 0xF2);
  LCD_WriteReg(0xD22C, 0x02);
  LCD_WriteReg(0xD22D, 0xFE);
  LCD_WriteReg(0xD22E, 0x03);
  LCD_WriteReg(0xD22F, 0x10);
  LCD_WriteReg(0xD230, 0x03);
  LCD_WriteReg(0xD231, 0x33);
  LCD_WriteReg(0xD232, 0x03);
  LCD_WriteReg(0xD233, 0x6D);
  LCD_WriteReg(0xD300, 0x00);
  LCD_WriteReg(0xD301, 0x33);
  LCD_WriteReg(0xD302, 0x00);
  LCD_WriteReg(0xD303, 0x34);
  LCD_WriteReg(0xD304, 0x00);
  LCD_WriteReg(0xD305, 0x3A);
  LCD_WriteReg(0xD306, 0x00);
  LCD_WriteReg(0xD307, 0x4A);
  LCD_WriteReg(0xD308, 0x00);
  LCD_WriteReg(0xD309, 0x5C);
  LCD_WriteReg(0xD30A, 0x00);

  LCD_WriteReg(0xD30B, 0x81);
  LCD_WriteReg(0xD30C, 0x00);
  LCD_WriteReg(0xD30D, 0xA6);
  LCD_WriteReg(0xD30E, 0x00);
  LCD_WriteReg(0xD30F, 0xE5);
  LCD_WriteReg(0xD310, 0x01);
  LCD_WriteReg(0xD311, 0x13);
  LCD_WriteReg(0xD312, 0x01);
  LCD_WriteReg(0xD313, 0x54);
  LCD_WriteReg(0xD314, 0x01);
  LCD_WriteReg(0xD315, 0x82);
  LCD_WriteReg(0xD316, 0x01);
  LCD_WriteReg(0xD317, 0xCA);
  LCD_WriteReg(0xD318, 0x02);
  LCD_WriteReg(0xD319, 0x00);
  LCD_WriteReg(0xD31A, 0x02);
  LCD_WriteReg(0xD31B, 0x01);
  LCD_WriteReg(0xD31C, 0x02);
  LCD_WriteReg(0xD31D, 0x34);
  LCD_WriteReg(0xD31E, 0x02);
  LCD_WriteReg(0xD31F, 0x67);
  LCD_WriteReg(0xD320, 0x02);
  LCD_WriteReg(0xD321, 0x84);
  LCD_WriteReg(0xD322, 0x02);
  LCD_WriteReg(0xD323, 0xA4);
  LCD_WriteReg(0xD324, 0x02);
  LCD_WriteReg(0xD325, 0xB7);
  LCD_WriteReg(0xD326, 0x02);
  LCD_WriteReg(0xD327, 0xCF);
  LCD_WriteReg(0xD328, 0x02);
  LCD_WriteReg(0xD329, 0xDE);
  LCD_WriteReg(0xD32A, 0x02);
  LCD_WriteReg(0xD32B, 0xF2);
  LCD_WriteReg(0xD32C, 0x02);
  LCD_WriteReg(0xD32D, 0xFE);
  LCD_WriteReg(0xD32E, 0x03);
  LCD_WriteReg(0xD32F, 0x10);
  LCD_WriteReg(0xD330, 0x03);
  LCD_WriteReg(0xD331, 0x33);
  LCD_WriteReg(0xD332, 0x03);
  LCD_WriteReg(0xD333, 0x6D);
  LCD_WriteReg(0xD400, 0x00);
  LCD_WriteReg(0xD401, 0x33);
  LCD_WriteReg(0xD402, 0x00);
  LCD_WriteReg(0xD403, 0x34);
  LCD_WriteReg(0xD404, 0x00);
  LCD_WriteReg(0xD405, 0x3A);
  LCD_WriteReg(0xD406, 0x00);
  LCD_WriteReg(0xD407, 0x4A);
  LCD_WriteReg(0xD408, 0x00);
  LCD_WriteReg(0xD409, 0x5C);
  LCD_WriteReg(0xD40A, 0x00);
  LCD_WriteReg(0xD40B, 0x81);

  LCD_WriteReg(0xD40C, 0x00);
  LCD_WriteReg(0xD40D, 0xA6);
  LCD_WriteReg(0xD40E, 0x00);
  LCD_WriteReg(0xD40F, 0xE5);
  LCD_WriteReg(0xD410, 0x01);
  LCD_WriteReg(0xD411, 0x13);
  LCD_WriteReg(0xD412, 0x01);
  LCD_WriteReg(0xD413, 0x54);
  LCD_WriteReg(0xD414, 0x01);
  LCD_WriteReg(0xD415, 0x82);
  LCD_WriteReg(0xD416, 0x01);
  LCD_WriteReg(0xD417, 0xCA);
  LCD_WriteReg(0xD418, 0x02);
  LCD_WriteReg(0xD419, 0x00);
  LCD_WriteReg(0xD41A, 0x02);
  LCD_WriteReg(0xD41B, 0x01);
  LCD_WriteReg(0xD41C, 0x02);
  LCD_WriteReg(0xD41D, 0x34);
  LCD_WriteReg(0xD41E, 0x02);
  LCD_WriteReg(0xD41F, 0x67);
  LCD_WriteReg(0xD420, 0x02);
  LCD_WriteReg(0xD421, 0x84);
  LCD_WriteReg(0xD422, 0x02);
  LCD_WriteReg(0xD423, 0xA4);
  LCD_WriteReg(0xD424, 0x02);
  LCD_WriteReg(0xD425, 0xB7);
  LCD_WriteReg(0xD426, 0x02);
  LCD_WriteReg(0xD427, 0xCF);
  LCD_WriteReg(0xD428, 0x02);
  LCD_WriteReg(0xD429, 0xDE);
  LCD_WriteReg(0xD42A, 0x02);
  LCD_WriteReg(0xD42B, 0xF2);
  LCD_WriteReg(0xD42C, 0x02);
  LCD_WriteReg(0xD42D, 0xFE);
  LCD_WriteReg(0xD42E, 0x03);
  LCD_WriteReg(0xD42F, 0x10);
  LCD_WriteReg(0xD430, 0x03);
  LCD_WriteReg(0xD431, 0x33);
  LCD_WriteReg(0xD432, 0x03);
  LCD_WriteReg(0xD433, 0x6D);
  LCD_WriteReg(0xD500, 0x00);
  LCD_WriteReg(0xD501, 0x33);
  LCD_WriteReg(0xD502, 0x00);
  LCD_WriteReg(0xD503, 0x34);
  LCD_WriteReg(0xD504, 0x00);
  LCD_WriteReg(0xD505, 0x3A);
  LCD_WriteReg(0xD506, 0x00);
  LCD_WriteReg(0xD507, 0x4A);
  LCD_WriteReg(0xD508, 0x00);
  LCD_WriteReg(0xD509, 0x5C);
  LCD_WriteReg(0xD50A, 0x00);
  LCD_WriteReg(0xD50B, 0x81);

  LCD_WriteReg(0xD50C, 0x00);
  LCD_WriteReg(0xD50D, 0xA6);
  LCD_WriteReg(0xD50E, 0x00);
  LCD_WriteReg(0xD50F, 0xE5);
  LCD_WriteReg(0xD510, 0x01);
  LCD_WriteReg(0xD511, 0x13);
  LCD_WriteReg(0xD512, 0x01);
  LCD_WriteReg(0xD513, 0x54);
  LCD_WriteReg(0xD514, 0x01);
  LCD_WriteReg(0xD515, 0x82);
  LCD_WriteReg(0xD516, 0x01);
  LCD_WriteReg(0xD517, 0xCA);
  LCD_WriteReg(0xD518, 0x02);
  LCD_WriteReg(0xD519, 0x00);
  LCD_WriteReg(0xD51A, 0x02);
  LCD_WriteReg(0xD51B, 0x01);
  LCD_WriteReg(0xD51C, 0x02);
  LCD_WriteReg(0xD51D, 0x34);
  LCD_WriteReg(0xD51E, 0x02);
  LCD_WriteReg(0xD51F, 0x67);
  LCD_WriteReg(0xD520, 0x02);
  LCD_WriteReg(0xD521, 0x84);
  LCD_WriteReg(0xD522, 0x02);
  LCD_WriteReg(0xD523, 0xA4);
  LCD_WriteReg(0xD524, 0x02);
  LCD_WriteReg(0xD525, 0xB7);
  LCD_WriteReg(0xD526, 0x02);
  LCD_WriteReg(0xD527, 0xCF);
  LCD_WriteReg(0xD528, 0x02);
  LCD_WriteReg(0xD529, 0xDE);
  LCD_WriteReg(0xD52A, 0x02);
  LCD_WriteReg(0xD52B, 0xF2);
  LCD_WriteReg(0xD52C, 0x02);
  LCD_WriteReg(0xD52D, 0xFE);
  LCD_WriteReg(0xD52E, 0x03);
  LCD_WriteReg(0xD52F, 0x10);
  LCD_WriteReg(0xD530, 0x03);
  LCD_WriteReg(0xD531, 0x33);
  LCD_WriteReg(0xD532, 0x03);
  LCD_WriteReg(0xD533, 0x6D);
  LCD_WriteReg(0xD600, 0x00);
  LCD_WriteReg(0xD601, 0x33);
  LCD_WriteReg(0xD602, 0x00);
  LCD_WriteReg(0xD603, 0x34);
  LCD_WriteReg(0xD604, 0x00);
  LCD_WriteReg(0xD605, 0x3A);
  LCD_WriteReg(0xD606, 0x00);
  LCD_WriteReg(0xD607, 0x4A);
  LCD_WriteReg(0xD608, 0x00);
  LCD_WriteReg(0xD609, 0x5C);
  LCD_WriteReg(0xD60A, 0x00);
  LCD_WriteReg(0xD60B, 0x81);

  LCD_WriteReg(0xD60C, 0x00);
  LCD_WriteReg(0xD60D, 0xA6);
  LCD_WriteReg(0xD60E, 0x00);
  LCD_WriteReg(0xD60F, 0xE5);
  LCD_WriteReg(0xD610, 0x01);
  LCD_WriteReg(0xD611, 0x13);
  LCD_WriteReg(0xD612, 0x01);
  LCD_WriteReg(0xD613, 0x54);
  LCD_WriteReg(0xD614, 0x01);
  LCD_WriteReg(0xD615, 0x82);
  LCD_WriteReg(0xD616, 0x01);
  LCD_WriteReg(0xD617, 0xCA);
  LCD_WriteReg(0xD618, 0x02);
  LCD_WriteReg(0xD619, 0x00);
  LCD_WriteReg(0xD61A, 0x02);
  LCD_WriteReg(0xD61B, 0x01);
  LCD_WriteReg(0xD61C, 0x02);
  LCD_WriteReg(0xD61D, 0x34);
  LCD_WriteReg(0xD61E, 0x02);
  LCD_WriteReg(0xD61F, 0x67);
  LCD_WriteReg(0xD620, 0x02);
  LCD_WriteReg(0xD621, 0x84);
  LCD_WriteReg(0xD622, 0x02);
  LCD_WriteReg(0xD623, 0xA4);
  LCD_WriteReg(0xD624, 0x02);
  LCD_WriteReg(0xD625, 0xB7);
  LCD_WriteReg(0xD626, 0x02);
  LCD_WriteReg(0xD627, 0xCF);
  LCD_WriteReg(0xD628, 0x02);
  LCD_WriteReg(0xD629, 0xDE);
  LCD_WriteReg(0xD62A, 0x02);
  LCD_WriteReg(0xD62B, 0xF2);
  LCD_WriteReg(0xD62C, 0x02);
  LCD_WriteReg(0xD62D, 0xFE);
  LCD_WriteReg(0xD62E, 0x03);
  LCD_WriteReg(0xD62F, 0x10);
  LCD_WriteReg(0xD630, 0x03);
  LCD_WriteReg(0xD631, 0x33);
  LCD_WriteReg(0xD632, 0x03);
  LCD_WriteReg(0xD633, 0x6D);
  //LV2 Page 0 enable
  LCD_WriteReg(0xF000, 0x55);
  LCD_WriteReg(0xF001, 0xAA);
  LCD_WriteReg(0xF002, 0x52);
  LCD_WriteReg(0xF003, 0x08);
  LCD_WriteReg(0xF004, 0x00);
  //Display control
  LCD_WriteReg(0xB100, 0xCC);
  LCD_WriteReg(0xB101, 0x00);
  //Source hold time
  LCD_WriteReg(0xB600, 0x05);
  //Gate EQ control
  LCD_WriteReg(0xB700, 0x70);
  LCD_WriteReg(0xB701, 0x70);
  //Source EQ control (Mode 2)
  LCD_WriteReg(0xB800, 0x01);
  LCD_WriteReg(0xB801, 0x03);
  LCD_WriteReg(0xB802, 0x03);
  LCD_WriteReg(0xB803, 0x03);
  //Inversion mode (2-dot)
  LCD_WriteReg(0xBC00, 0x02);
  LCD_WriteReg(0xBC01, 0x00);
  LCD_WriteReg(0xBC02, 0x00);
  //Timing control 4H w/ 4-delay
  LCD_WriteReg(0xC900, 0xD0);
  LCD_WriteReg(0xC901, 0x02);
  LCD_WriteReg(0xC902, 0x50);
  LCD_WriteReg(0xC903, 0x50);
  LCD_WriteReg(0xC904, 0x50);
  LCD_WriteReg(0x3500, 0x00);
  LCD_WriteReg(0x3A00, 0x55);  //16-bit/pixel
  LCD_WR_REG(0x1100);
  cClockInfo::Delay_ms(120);
  LCD_WR_REG(0x2900);
  }
  else if (lcddev.id == 0x9325)//9325
  {
  LCD_WriteReg(0x00E5, 0x78F0);
  LCD_WriteReg(0x0001, 0x0100);
  LCD_WriteReg(0x0002, 0x0700);
  LCD_WriteReg(0x0003, 0x1030);
  LCD_WriteReg(0x0004, 0x0000);
  LCD_WriteReg(0x0008, 0x0202);
  LCD_WriteReg(0x0009, 0x0000);
  LCD_WriteReg(0x000A, 0x0000);
  LCD_WriteReg(0x000C, 0x0000);
  LCD_WriteReg(0x000D, 0x0000);
  LCD_WriteReg(0x000F, 0x0000);
  //power on sequence VGHVGL
  LCD_WriteReg(0x0010, 0x0000);
  LCD_WriteReg(0x0011, 0x0007);
  LCD_WriteReg(0x0012, 0x0000);
  LCD_WriteReg(0x0013, 0x0000);
  LCD_WriteReg(0x0007, 0x0000);
  //vgh
  LCD_WriteReg(0x0010, 0x1690);
  LCD_WriteReg(0x0011, 0x0227);
  //cClockInfo::Delay_ms(100);
  //vregiout
  LCD_WriteReg(0x0012, 0x009D); //0x001b
  //cClockInfo::Delay_ms(100);
  //vom amplitude
  LCD_WriteReg(0x0013, 0x1900);
  //cClockInfo::Delay_ms(100);
  //vom H
  LCD_WriteReg(0x0029, 0x0025);
  LCD_WriteReg(0x002B, 0x000D);
  //gamma
  LCD_WriteReg(0x0030, 0x0007);
  LCD_WriteReg(0x0031, 0x0303);
  LCD_WriteReg(0x0032, 0x0003);// 0006
  LCD_WriteReg(0x0035, 0x0206);
  LCD_WriteReg(0x0036, 0x0008);
  LCD_WriteReg(0x0037, 0x0406);
  LCD_WriteReg(0x0038, 0x0304);//0200
  LCD_WriteReg(0x0039, 0x0007);
  LCD_WriteReg(0x003C, 0x0602);// 0504
  LCD_WriteReg(0x003D, 0x0008);
  //ram
  LCD_WriteReg(0x0050, 0x0000);
  LCD_WriteReg(0x0051, 0x00EF);
  LCD_WriteReg(0x0052, 0x0000);
  LCD_WriteReg(0x0053, 0x013F);
  LCD_WriteReg(0x0060, 0xA700);
  LCD_WriteReg(0x0061, 0x0001);
  LCD_WriteReg(0x006A, 0x0000);
  //
  LCD_WriteReg(0x0080, 0x0000);
  LCD_WriteReg(0x0081, 0x0000);
  LCD_WriteReg(0x0082, 0x0000);
  LCD_WriteReg(0x0083, 0x0000);
  LCD_WriteReg(0x0084, 0x0000);
  LCD_WriteReg(0x0085, 0x0000);
  //
  LCD_WriteReg(0x0090, 0x0010);
  LCD_WriteReg(0x0092, 0x0600);

  LCD_WriteReg(0x0007, 0x0133);
  LCD_WriteReg(0x00, 0x0022);//
  }
  else if (lcddev.id == 0x9328)//ILI9328   OK
  {
  LCD_WriteReg(0x00EC, 0x108F);// internal timeing
  LCD_WriteReg(0x00EF, 0x1234);// ADD
  //LCD_WriteReg(0x00e7,0x0010);
      //LCD_WriteReg(0x0000,0x0001);//�����ڲ�ʱ��
  LCD_WriteReg(0x0001, 0x0100);
  LCD_WriteReg(0x0002, 0x0700);//��Դ����
//LCD_WriteReg(0x0003,(1<<3)|(1<<4) ); 	//65K  RGB
//DRIVE TABLE(�Ĵ��� 03H)
//BIT3=AM BIT4:5=ID0:1
//AM ID0 ID1   FUNCATION
// 0  0   0	   R->L D->U
// 1  0   0	   D->U	R->L
// 0  1   0	   L->R D->U
// 1  1   0    D->U	L->R
// 0  0   1	   R->L U->D
// 1  0   1    U->D	R->L
// 0  1   1    L->R U->D �����������.
// 1  1   1	   U->D	L->R
  LCD_WriteReg(0x0003, (1 << 12) | (3 << 4) | (0 << 3));//65K
  LCD_WriteReg(0x0004, 0x0000);
  LCD_WriteReg(0x0008, 0x0202);
  LCD_WriteReg(0x0009, 0x0000);
  LCD_WriteReg(0x000a, 0x0000);//display setting
  LCD_WriteReg(0x000c, 0x0001);//display setting
  LCD_WriteReg(0x000d, 0x0000);//0f3c
  LCD_WriteReg(0x000f, 0x0000);
  //��Դ����
  LCD_WriteReg(0x0010, 0x0000);
  LCD_WriteReg(0x0011, 0x0007);
  LCD_WriteReg(0x0012, 0x0000);
  LCD_WriteReg(0x0013, 0x0000);
  LCD_WriteReg(0x0007, 0x0001);
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x0010, 0x1490);
  LCD_WriteReg(0x0011, 0x0227);
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x0012, 0x008A);
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x0013, 0x1a00);
  LCD_WriteReg(0x0029, 0x0006);
  LCD_WriteReg(0x002b, 0x000d);
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x0020, 0x0000);
  LCD_WriteReg(0x0021, 0x0000);
  cClockInfo::Delay_ms(50);
  //٤��У��
  LCD_WriteReg(0x0030, 0x0000);
  LCD_WriteReg(0x0031, 0x0604);
  LCD_WriteReg(0x0032, 0x0305);
  LCD_WriteReg(0x0035, 0x0000);
  LCD_WriteReg(0x0036, 0x0C09);
  LCD_WriteReg(0x0037, 0x0204);
  LCD_WriteReg(0x0038, 0x0301);
  LCD_WriteReg(0x0039, 0x0707);
  LCD_WriteReg(0x003c, 0x0000);
  LCD_WriteReg(0x003d, 0x0a0a);
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x0050, 0x0000);
  LCD_WriteReg(0x0051, 0x00ef);
  LCD_WriteReg(0x0052, 0x0000);
  LCD_WriteReg(0x0053, 0x013f);

  LCD_WriteReg(0x0060, 0xa700);
  LCD_WriteReg(0x0061, 0x0001);
  LCD_WriteReg(0x006a, 0x0000);
  LCD_WriteReg(0x0080, 0x0000);
  LCD_WriteReg(0x0081, 0x0000);
  LCD_WriteReg(0x0082, 0x0000);
  LCD_WriteReg(0x0083, 0x0000);
  LCD_WriteReg(0x0084, 0x0000);
  LCD_WriteReg(0x0085, 0x0000);

  LCD_WriteReg(0x0090, 0x0010);
  LCD_WriteReg(0x0092, 0x0600);
  //������ʾ����
  LCD_WriteReg(0x0007, 0x0133);
  }
  else if (lcddev.id == 0x9320)//����OK.
  {
  LCD_WriteReg(0x00, 0x0000);
  LCD_WriteReg(0x01, 0x0100);	//Driver Output Contral.
  LCD_WriteReg(0x02, 0x0700);	//LCD Driver Waveform Contral.
  LCD_WriteReg(0x03, 0x1030);//Entry Mode Set.
  //LCD_WriteReg(0x03,0x1018);	//Entry Mode Set.

  LCD_WriteReg(0x04, 0x0000);	//Scalling Contral.
  LCD_WriteReg(0x08, 0x0202);	//Display Contral 2.(0x0207)
  LCD_WriteReg(0x09, 0x0000);	//Display Contral 3.(0x0000)
  LCD_WriteReg(0x0a, 0x0000);	//Frame Cycle Contal.(0x0000)
  LCD_WriteReg(0x0c, (1 << 0));	//Extern Display Interface Contral 1.(0x0000)
  LCD_WriteReg(0x0d, 0x0000);	//Frame Maker Position.
  LCD_WriteReg(0x0f, 0x0000);	//Extern Display Interface Contral 2.
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x07, 0x0101);	//Display Contral.
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x10, (1 << 12) | (0 << 8) | (1 << 7) | (1 << 6) | (0 << 4));	//Power Control 1.(0x16b0)
  LCD_WriteReg(0x11, 0x0007);								//Power Control 2.(0x0001)
  LCD_WriteReg(0x12, (1 << 8) | (1 << 4) | (0 << 0));				//Power Control 3.(0x0138)
  LCD_WriteReg(0x13, 0x0b00);								//Power Control 4.
  LCD_WriteReg(0x29, 0x0000);								//Power Control 7.

  LCD_WriteReg(0x2b, (1 << 14) | (1 << 4));
  LCD_WriteReg(0x50, 0);	//Set X Star
  //ˮƽGRAM��ֹλ��Set X End.
  LCD_WriteReg(0x51, 239);	//Set Y Star
  LCD_WriteReg(0x52, 0);	//Set Y End.t.
  LCD_WriteReg(0x53, 319);	//

  LCD_WriteReg(0x60, 0x2700);	//Driver Output Control.
  LCD_WriteReg(0x61, 0x0001);	//Driver Output Control.
  LCD_WriteReg(0x6a, 0x0000);	//Vertical Srcoll Control.

  LCD_WriteReg(0x80, 0x0000);	//Display Position? Partial Display 1.
  LCD_WriteReg(0x81, 0x0000);	//RAM Address Start? Partial Display 1.
  LCD_WriteReg(0x82, 0x0000);	//RAM Address End-Partial Display 1.
  LCD_WriteReg(0x83, 0x0000);	//Displsy Position? Partial Display 2.
  LCD_WriteReg(0x84, 0x0000);	//RAM Address Start? Partial Display 2.
  LCD_WriteReg(0x85, 0x0000);	//RAM Address End? Partial Display 2.

  LCD_WriteReg(0x90, (0 << 7) | (16 << 0));	//Frame Cycle Contral.(0x0013)
  LCD_WriteReg(0x92, 0x0000);	//Panel Interface Contral 2.(0x0000)
  LCD_WriteReg(0x93, 0x0001);	//Panel Interface Contral 3.
  LCD_WriteReg(0x95, 0x0110);	//Frame Cycle Contral.(0x0110)
  LCD_WriteReg(0x97, (0 << 8));	//
  LCD_WriteReg(0x98, 0x0000);	//Frame Cycle Contral.
  LCD_WriteReg(0x07, 0x0173);	//(0x0173)
  }
  else if (lcddev.id == 0X9331)//OK |/|/|
  {
  LCD_WriteReg(0x00E7, 0x1014);
  LCD_WriteReg(0x0001, 0x0100); // set SS and SM bit
  LCD_WriteReg(0x0002, 0x0200); // set 1 line inversion
  LCD_WriteReg(0x0003, (1 << 12) | (3 << 4) | (1 << 3));//65K
//LCD_WriteReg(0x0003, 0x1030); // set GRAM write direction and BGR=1.
  LCD_WriteReg(0x0008, 0x0202); // set the back porch and front porch
  LCD_WriteReg(0x0009, 0x0000); // set non-display area refresh cycle ISC[3:0]
  LCD_WriteReg(0x000A, 0x0000); // FMARK function
  LCD_WriteReg(0x000C, 0x0000); // RGB interface setting
  LCD_WriteReg(0x000D, 0x0000); // Frame marker Position
  LCD_WriteReg(0x000F, 0x0000); // RGB interface polarity
  // -------------------Power On sequence ------------------//
  LCD_WriteReg(0x0010, 0x0000); // SAP, BT[3:0], AP, DSTB, SLP, STB
  LCD_WriteReg(0x0011, 0x0007); // DC1[2:0], DC0[2:0], VC[2:0]
  LCD_WriteReg(0x0012, 0x0000); // VREG1OUT voltage
  LCD_WriteReg(0x0013, 0x0000); // VDV[4:0] for VCOM amplitude
  cClockInfo::Delay_ms(200); // Dis-charge capacitor power voltage
  LCD_WriteReg(0x0010, 0x1690); // SAP, BT[3:0], AP, DSTB, SLP, STB
  LCD_WriteReg(0x0011, 0x0227); // DC1[2:0], DC0[2:0], VC[2:0]
  cClockInfo::Delay_ms(50); // Delay 50ms
  LCD_WriteReg(0x0012, 0x000C); // Internal reference voltage= Vci;
  cClockInfo::Delay_ms(50); // Delay 50ms
  LCD_WriteReg(0x0013, 0x0800); // Set VDV[4:0] for VCOM amplitude
  LCD_WriteReg(0x0029, 0x0011); // Set VCM[5:0] for VCOMH
  LCD_WriteReg(0x002B, 0x000B); // Set Frame Rate
  cClockInfo::Delay_ms(50); // Delay 50ms
  LCD_WriteReg(0x0020, 0x0000); // GRAM horizontal Address
  LCD_WriteReg(0x0021, 0x013f); // GRAM Vertical Address
  // ----------- Adjust the Gamma Curve ----------//
  LCD_WriteReg(0x0030, 0x0000);
  LCD_WriteReg(0x0031, 0x0106);
  LCD_WriteReg(0x0032, 0x0000);
  LCD_WriteReg(0x0035, 0x0204);
  LCD_WriteReg(0x0036, 0x160A);
  LCD_WriteReg(0x0037, 0x0707);
  LCD_WriteReg(0x0038, 0x0106);
  LCD_WriteReg(0x0039, 0x0707);
  LCD_WriteReg(0x003C, 0x0402);
  LCD_WriteReg(0x003D, 0x0C0F);
  //------------------ Set GRAM area ---------------//
  LCD_WriteReg(0x0050, 0x0000); // Horizontal GRAM Start Address
  LCD_WriteReg(0x0051, 0x00EF); // Horizontal GRAM End Address
  LCD_WriteReg(0x0052, 0x0000); // Vertical GRAM Start Address
  LCD_WriteReg(0x0053, 0x013F); // Vertical GRAM Start Address
  LCD_WriteReg(0x0060, 0x2700); // Gate Scan Line
  LCD_WriteReg(0x0061, 0x0001); // NDL,VLE, REV
  LCD_WriteReg(0x006A, 0x0000); // set scrolling line
  //-------------- Partial Display Control ---------//
  LCD_WriteReg(0x0080, 0x0000);
  LCD_WriteReg(0x0081, 0x0000);
  LCD_WriteReg(0x0082, 0x0000);
  LCD_WriteReg(0x0083, 0x0000);
  LCD_WriteReg(0x0084, 0x0000);
  LCD_WriteReg(0x0085, 0x0000);
  //-------------- Panel Control -------------------//
  LCD_WriteReg(0x0090, 0x0010);
  LCD_WriteReg(0x0092, 0x0600);
  LCD_WriteReg(0x0007, 0x0133); // 262K color and display ON
  }
  else if (lcddev.id == 0x5408)
  {
  LCD_WriteReg(0x01, 0x0100);
  LCD_WriteReg(0x02, 0x0700);//LCD Driving Waveform Contral
  LCD_WriteReg(0x03, 0x1030);//Entry Mode����
  //ָ������������϶��µ��Զ���ģʽ
  //Normal Mode(Window Mode disable)
  //RGB��ʽ
  //16λ����2�δ����8��������
  LCD_WriteReg(0x04, 0x0000); //Scalling Control register
  LCD_WriteReg(0x08, 0x0207); //Display Control 2
  LCD_WriteReg(0x09, 0x0000); //Display Control 3
  LCD_WriteReg(0x0A, 0x0000); //Frame Cycle Control
  LCD_WriteReg(0x0C, 0x0000); //External Display Interface Control 1
  LCD_WriteReg(0x0D, 0x0000); //Frame Maker Position
  LCD_WriteReg(0x0F, 0x0000); //External Display Interface Control 2
  cClockInfo::Delay_ms(20);
  //TFT Һ����ɫͼ����ʾ����14
  LCD_WriteReg(0x10, 0x16B0); //0x14B0 //Power Control 1
  LCD_WriteReg(0x11, 0x0001); //0x0007 //Power Control 2
  LCD_WriteReg(0x17, 0x0001); //0x0000 //Power Control 3
  LCD_WriteReg(0x12, 0x0138); //0x013B //Power Control 4
  LCD_WriteReg(0x13, 0x0800); //0x0800 //Power Control 5
  LCD_WriteReg(0x29, 0x0009); //NVM read data 2
  LCD_WriteReg(0x2a, 0x0009); //NVM read data 3
  LCD_WriteReg(0xa4, 0x0000);
  LCD_WriteReg(0x50, 0x0000); //���ò������ڵ�X�Ὺʼ��
  LCD_WriteReg(0x51, 0x00EF); //���ò������ڵ�X�������
  LCD_WriteReg(0x52, 0x0000); //���ò������ڵ�Y�Ὺʼ��
  LCD_WriteReg(0x53, 0x013F); //���ò������ڵ�Y�������
  LCD_WriteReg(0x60, 0x2700); //Driver Output Control
  //������Ļ�ĵ����Լ�ɨ�����ʼ��
  LCD_WriteReg(0x61, 0x0001); //Driver Output Control
  LCD_WriteReg(0x6A, 0x0000); //Vertical Scroll Control
  LCD_WriteReg(0x80, 0x0000); //Display Position �C Partial Display 1
  LCD_WriteReg(0x81, 0x0000); //RAM Address Start �C Partial Display 1
  LCD_WriteReg(0x82, 0x0000); //RAM address End - Partial Display 1
  LCD_WriteReg(0x83, 0x0000); //Display Position �C Partial Display 2
  LCD_WriteReg(0x84, 0x0000); //RAM Address Start �C Partial Display 2
  LCD_WriteReg(0x85, 0x0000); //RAM address End �C Partail Display2
  LCD_WriteReg(0x90, 0x0013); //Frame Cycle Control
  LCD_WriteReg(0x92, 0x0000);  //Panel Interface Control 2
  LCD_WriteReg(0x93, 0x0003); //Panel Interface control 3
  LCD_WriteReg(0x95, 0x0110);  //Frame Cycle Control
  LCD_WriteReg(0x07, 0x0173);
  cClockInfo::Delay_ms(50);
  }
  else if (lcddev.id == 0x1505)//OK
  {
  // second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
  LCD_WriteReg(0x0007, 0x0000);
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x0012, 0x011C);//0x011A   why need to set several times?
  LCD_WriteReg(0x00A4, 0x0001);//NVM
  LCD_WriteReg(0x0008, 0x000F);
  LCD_WriteReg(0x000A, 0x0008);
  LCD_WriteReg(0x000D, 0x0008);
  //٤��У��
  LCD_WriteReg(0x0030, 0x0707);
  LCD_WriteReg(0x0031, 0x0007); //0x0707
  LCD_WriteReg(0x0032, 0x0603);
  LCD_WriteReg(0x0033, 0x0700);
  LCD_WriteReg(0x0034, 0x0202);
  LCD_WriteReg(0x0035, 0x0002); //?0x0606
  LCD_WriteReg(0x0036, 0x1F0F);
  LCD_WriteReg(0x0037, 0x0707); //0x0f0f  0x0105
  LCD_WriteReg(0x0038, 0x0000);
  LCD_WriteReg(0x0039, 0x0000);
  LCD_WriteReg(0x003A, 0x0707);
  LCD_WriteReg(0x003B, 0x0000); //0x0303
  LCD_WriteReg(0x003C, 0x0007); //?0x0707
  LCD_WriteReg(0x003D, 0x0000); //0x1313//0x1f08
  cClockInfo::Delay_ms(50);
  LCD_WriteReg(0x0007, 0x0001);
  LCD_WriteReg(0x0017, 0x0001);//������Դ
  cClockInfo::Delay_ms(50);
  //��Դ����
  LCD_WriteReg(0x0010, 0x17A0);
  LCD_WriteReg(0x0011, 0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
  LCD_WriteReg(0x0012, 0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
  LCD_WriteReg(0x0013, 0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
  LCD_WriteReg(0x002A, 0x0000);
  LCD_WriteReg(0x0029, 0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
  LCD_WriteReg(0x0012, 0x013E);// 0x013C  power supply on
  //Coordinates Control//
  LCD_WriteReg(0x0050, 0x0000);//0x0e00
  LCD_WriteReg(0x0051, 0x00EF);
  LCD_WriteReg(0x0052, 0x0000);
  LCD_WriteReg(0x0053, 0x013F);
  //Pannel Image Control//
  LCD_WriteReg(0x0060, 0x2700);
  LCD_WriteReg(0x0061, 0x0001);
  LCD_WriteReg(0x006A, 0x0000);
  LCD_WriteReg(0x0080, 0x0000);
  //Partial Image Control//
  LCD_WriteReg(0x0081, 0x0000);
  LCD_WriteReg(0x0082, 0x0000);
  LCD_WriteReg(0x0083, 0x0000);
  LCD_WriteReg(0x0084, 0x0000);
  LCD_WriteReg(0x0085, 0x0000);
  //Panel Interface Control//
  LCD_WriteReg(0x0090, 0x0013);//0x0010 frenqucy
  LCD_WriteReg(0x0092, 0x0300);
  LCD_WriteReg(0x0093, 0x0005);
  LCD_WriteReg(0x0095, 0x0000);
  LCD_WriteReg(0x0097, 0x0000);
  LCD_WriteReg(0x0098, 0x0000);

  LCD_WriteReg(0x0001, 0x0100);
  LCD_WriteReg(0x0002, 0x0700);
  LCD_WriteReg(0x0003, 0x1038);//ɨ�跽�� ��->��  ��->��
  LCD_WriteReg(0x0004, 0x0000);
  LCD_WriteReg(0x000C, 0x0000);
  LCD_WriteReg(0x000F, 0x0000);
  LCD_WriteReg(0x0020, 0x0000);
  LCD_WriteReg(0x0021, 0x0000);
  LCD_WriteReg(0x0007, 0x0021);
  cClockInfo::Delay_ms(20);
  LCD_WriteReg(0x0007, 0x0061);
  cClockInfo::Delay_ms(20);
  LCD_WriteReg(0x0007, 0x0173);
  cClockInfo::Delay_ms(20);
  }
  else if (lcddev.id == 0xB505)
  {
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x0000, 0x0000);

  LCD_WriteReg(0x00a4, 0x0001);
  cClockInfo::Delay_ms(20);
  LCD_WriteReg(0x0060, 0x2700);
  LCD_WriteReg(0x0008, 0x0202);

  LCD_WriteReg(0x0030, 0x0214);
  LCD_WriteReg(0x0031, 0x3715);
  LCD_WriteReg(0x0032, 0x0604);
  LCD_WriteReg(0x0033, 0x0e16);
  LCD_WriteReg(0x0034, 0x2211);
  LCD_WriteReg(0x0035, 0x1500);
  LCD_WriteReg(0x0036, 0x8507);
  LCD_WriteReg(0x0037, 0x1407);
  LCD_WriteReg(0x0038, 0x1403);
  LCD_WriteReg(0x0039, 0x0020);

  LCD_WriteReg(0x0090, 0x001a);
  LCD_WriteReg(0x0010, 0x0000);
  LCD_WriteReg(0x0011, 0x0007);
  LCD_WriteReg(0x0012, 0x0000);
  LCD_WriteReg(0x0013, 0x0000);
  cClockInfo::Delay_ms(20);

  LCD_WriteReg(0x0010, 0x0730);
  LCD_WriteReg(0x0011, 0x0137);
  cClockInfo::Delay_ms(20);

  LCD_WriteReg(0x0012, 0x01b8);
  cClockInfo::Delay_ms(20);

  LCD_WriteReg(0x0013, 0x0f00);
  LCD_WriteReg(0x002a, 0x0080);
  LCD_WriteReg(0x0029, 0x0048);
  cClockInfo::Delay_ms(20);

  LCD_WriteReg(0x0001, 0x0100);
  LCD_WriteReg(0x0002, 0x0700);
  LCD_WriteReg(0x0003, 0x1038);//ɨ�跽�� ��->��  ��->��
  LCD_WriteReg(0x0008, 0x0202);
  LCD_WriteReg(0x000a, 0x0000);
  LCD_WriteReg(0x000c, 0x0000);
  LCD_WriteReg(0x000d, 0x0000);
  LCD_WriteReg(0x000e, 0x0030);
  LCD_WriteReg(0x0050, 0x0000);
  LCD_WriteReg(0x0051, 0x00ef);
  LCD_WriteReg(0x0052, 0x0000);
  LCD_WriteReg(0x0053, 0x013f);
  LCD_WriteReg(0x0060, 0x2700);
  LCD_WriteReg(0x0061, 0x0001);
  LCD_WriteReg(0x006a, 0x0000);
  //LCD_WriteReg(0x0080,0x0000);
  //LCD_WriteReg(0x0081,0x0000);
  LCD_WriteReg(0x0090, 0X0011);
  LCD_WriteReg(0x0092, 0x0600);
  LCD_WriteReg(0x0093, 0x0402);
  LCD_WriteReg(0x0094, 0x0002);
  cClockInfo::Delay_ms(20);

  LCD_WriteReg(0x0007, 0x0001);
  cClockInfo::Delay_ms(20);
  LCD_WriteReg(0x0007, 0x0061);
  LCD_WriteReg(0x0007, 0x0173);

  LCD_WriteReg(0x0020, 0x0000);
  LCD_WriteReg(0x0021, 0x0000);
  LCD_WriteReg(0x00, 0x22);
  }
  else if (lcddev.id == 0xC505)
  {
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x0000, 0x0000);
  cClockInfo::Delay_ms(20);
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x0000, 0x0000);
  LCD_WriteReg(0x00a4, 0x0001);
  cClockInfo::Delay_ms(20);
  LCD_WriteReg(0x0060, 0x2700);
  LCD_WriteReg(0x0008, 0x0806);

  LCD_WriteReg(0x0030, 0x0703);//gamma setting
  LCD_WriteReg(0x0031, 0x0001);
  LCD_WriteReg(0x0032, 0x0004);
  LCD_WriteReg(0x0033, 0x0102);
  LCD_WriteReg(0x0034, 0x0300);
  LCD_WriteReg(0x0035, 0x0103);
  LCD_WriteReg(0x0036, 0x001F);
  LCD_WriteReg(0x0037, 0x0703);
  LCD_WriteReg(0x0038, 0x0001);
  LCD_WriteReg(0x0039, 0x0004);



  LCD_WriteReg(0x0090, 0x0015);	//80Hz
  LCD_WriteReg(0x0010, 0X0410);	//BT,AP
  LCD_WriteReg(0x0011, 0x0247);	//DC1,DC0,VC
  LCD_WriteReg(0x0012, 0x01BC);
  LCD_WriteReg(0x0013, 0x0e00);
  cClockInfo::Delay_ms(120);
  LCD_WriteReg(0x0001, 0x0100);
  LCD_WriteReg(0x0002, 0x0200);
  LCD_WriteReg(0x0003, 0x1030);

  LCD_WriteReg(0x000A, 0x0008);
  LCD_WriteReg(0x000C, 0x0000);

  LCD_WriteReg(0x000E, 0x0020);
  LCD_WriteReg(0x000F, 0x0000);
  LCD_WriteReg(0x0020, 0x0000);	//H Start
  LCD_WriteReg(0x0021, 0x0000);	//V Start
  LCD_WriteReg(0x002A, 0x003D);	//vcom2
  cClockInfo::Delay_ms(20);
  LCD_WriteReg(0x0029, 0x002d);
  LCD_WriteReg(0x0050, 0x0000);
  LCD_WriteReg(0x0051, 0xD0EF);
  LCD_WriteReg(0x0052, 0x0000);
  LCD_WriteReg(0x0053, 0x013F);
  LCD_WriteReg(0x0061, 0x0000);
  LCD_WriteReg(0x006A, 0x0000);
  LCD_WriteReg(0x0092, 0x0300);

  LCD_WriteReg(0x0093, 0x0005);
  LCD_WriteReg(0x0007, 0x0100);
  }
  else if (lcddev.id == 0x4531)//OK |/|/|
  {
  LCD_WriteReg(0X00, 0X0001);
  cClockInfo::Delay_ms(10);
  LCD_WriteReg(0X10, 0X1628);
  LCD_WriteReg(0X12, 0X000e);//0x0006
  LCD_WriteReg(0X13, 0X0A39);
  cClockInfo::Delay_ms(10);
  LCD_WriteReg(0X11, 0X0040);
  LCD_WriteReg(0X15, 0X0050);
  cClockInfo::Delay_ms(10);
  LCD_WriteReg(0X12, 0X001e);//16
  cClockInfo::Delay_ms(10);
  LCD_WriteReg(0X10, 0X1620);
  LCD_WriteReg(0X13, 0X2A39);
  cClockInfo::Delay_ms(10);
  LCD_WriteReg(0X01, 0X0100);
  LCD_WriteReg(0X02, 0X0300);
  LCD_WriteReg(0X03, 0X1038);//�ı䷽���
  LCD_WriteReg(0X08, 0X0202);
  LCD_WriteReg(0X0A, 0X0008);
  LCD_WriteReg(0X30, 0X0000);
  LCD_WriteReg(0X31, 0X0402);
  LCD_WriteReg(0X32, 0X0106);
  LCD_WriteReg(0X33, 0X0503);
  LCD_WriteReg(0X34, 0X0104);
  LCD_WriteReg(0X35, 0X0301);
  LCD_WriteReg(0X36, 0X0707);
  LCD_WriteReg(0X37, 0X0305);
  LCD_WriteReg(0X38, 0X0208);
  LCD_WriteReg(0X39, 0X0F0B);
  LCD_WriteReg(0X41, 0X0002);
  LCD_WriteReg(0X60, 0X2700);
  LCD_WriteReg(0X61, 0X0001);
  LCD_WriteReg(0X90, 0X0210);
  LCD_WriteReg(0X92, 0X010A);
  LCD_WriteReg(0X93, 0X0004);
  LCD_WriteReg(0XA0, 0X0100);
  LCD_WriteReg(0X07, 0X0001);
  LCD_WriteReg(0X07, 0X0021);
  LCD_WriteReg(0X07, 0X0023);
  LCD_WriteReg(0X07, 0X0033);
  LCD_WriteReg(0X07, 0X0133);
  LCD_WriteReg(0XA0, 0X0000);
  }
  else if (lcddev.id == 0x4535)
  {
  LCD_WriteReg(0X15, 0X0030);
  LCD_WriteReg(0X9A, 0X0010);
  LCD_WriteReg(0X11, 0X0020);
  LCD_WriteReg(0X10, 0X3428);
  LCD_WriteReg(0X12, 0X0002);//16
  LCD_WriteReg(0X13, 0X1038);
  cClockInfo::Delay_ms(40);
  LCD_WriteReg(0X12, 0X0012);//16
  cClockInfo::Delay_ms(40);
  LCD_WriteReg(0X10, 0X3420);
  LCD_WriteReg(0X13, 0X3038);
  cClockInfo::Delay_ms(70);
  LCD_WriteReg(0X30, 0X0000);
  LCD_WriteReg(0X31, 0X0402);
  LCD_WriteReg(0X32, 0X0307);
  LCD_WriteReg(0X33, 0X0304);
  LCD_WriteReg(0X34, 0X0004);
  LCD_WriteReg(0X35, 0X0401);
  LCD_WriteReg(0X36, 0X0707);
  LCD_WriteReg(0X37, 0X0305);
  LCD_WriteReg(0X38, 0X0610);
  LCD_WriteReg(0X39, 0X0610);

  LCD_WriteReg(0X01, 0X0100);
  LCD_WriteReg(0X02, 0X0300);
  LCD_WriteReg(0X03, 0X1030);//�ı䷽���
  LCD_WriteReg(0X08, 0X0808);
  LCD_WriteReg(0X0A, 0X0008);
  LCD_WriteReg(0X60, 0X2700);
  LCD_WriteReg(0X61, 0X0001);
  LCD_WriteReg(0X90, 0X013E);
  LCD_WriteReg(0X92, 0X0100);
  LCD_WriteReg(0X93, 0X0100);
  LCD_WriteReg(0XA0, 0X3000);
  LCD_WriteReg(0XA3, 0X0010);
  LCD_WriteReg(0X07, 0X0001);
  LCD_WriteReg(0X07, 0X0021);
  LCD_WriteReg(0X07, 0X0023);
  LCD_WriteReg(0X07, 0X0033);
  LCD_WriteReg(0X07, 0X0133);
  }

  LCD_Display_Dir(0);
  LCD_LED=1;
  LCD_Clear(MAGENTA);
}


void LCD_Clear(u16 color)
{
  u32 index = 0;
  u32 totalpoint = lcddev.width;

  totalpoint *= lcddev.height;
  if ((lcddev.id == 0X6804) && (lcddev.dir == 1))
  {
    lcddev.dir = 0;
    lcddev.setxcmd = 0X2A;
    lcddev.setycmd = 0X2B;
    LCD_SetCursor(0x00, 0x0000);
    lcddev.dir = 1;
    lcddev.setxcmd = 0X2B;
    lcddev.setycmd = 0X2A;
  }
  else
  {
    LCD_SetCursor(0x00, 0x0000);
  }

  LCD_WriteRAM_Prepare();
  for (index = 0; index < totalpoint; index++)
  {
    LCD->LCD_RAM = color;
  }
}


