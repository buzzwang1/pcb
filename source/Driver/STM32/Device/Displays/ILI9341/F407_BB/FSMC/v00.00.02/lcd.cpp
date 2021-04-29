#include "lcd.h"

//////////////////////////////////////////////////////////////////////////////////

//2.4寸/2.8寸/3.5寸/4.3寸 TFT液晶驱动
//支持驱动IC型号包括:ILI9341/ILI9325/RM68042/RM68021/ILI9320/ILI9328/LGDP4531/LGDP4535/
//                   SPFD5408/1505/B505/C505/NT35310/NT35510等
//STM32F4工程-库函数版本
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
  LCD_WR_REG(0X29);
}

void LCD_DisplayOff(void)
{
  LCD_WR_REG(0X28);
}


void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
  LCD_WR_REG(lcddev.setxcmd);
  LCD_WR_DATA(Xpos >> 8);
  LCD_WR_DATA(Xpos & 0XFF);
  LCD_WR_REG(lcddev.setycmd);
  LCD_WR_DATA(Ypos >> 8);
  LCD_WR_DATA(Ypos & 0XFF);
}


void LCD_Scan_Dir(u8 dir)
{
  u16 regval = 0;
  u16 dirreg = 0;
  u16 temp;
  u16 xsize, ysize;

  if (lcddev.id == 0x9341)
  {
    switch (dir)
    {
      case L2R_U2D:
        regval |= (0 << 7) | (0 << 6) | (0 << 5);
        break;
      case L2R_D2U:
        regval |= (1 << 7) | (0 << 6) | (0 << 5);
        break;
      case R2L_U2D:
        regval |= (0 << 7) | (1 << 6) | (0 << 5);
        break;
      case R2L_D2U:
        regval |= (1 << 7) | (1 << 6) | (0 << 5);
        break;
      case U2D_L2R:
        regval |= (0 << 7) | (0 << 6) | (1 << 5);
        break;
      case U2D_R2L:
        regval |= (0 << 7) | (1 << 6) | (1 << 5);
        break;
      case D2U_L2R:
        regval |= (1 << 7) | (0 << 6) | (1 << 5);
        break;
      case D2U_R2L:
        regval |= (1 << 7) | (1 << 6) | (1 << 5);
        break;
    }
    dirreg = 0X36;

    LCD_WriteReg(dirreg, regval);
    if ((regval & 0X20) || lcddev.dir == 1)
    {
      if (lcddev.width < lcddev.height)
      {
        temp = lcddev.width;
        lcddev.width = lcddev.height;
        lcddev.height = temp;
      }
    }
    else
    {
      if (lcddev.width > lcddev.height)
      {
        temp = lcddev.width;
        lcddev.width = lcddev.height;
        lcddev.height = temp;
      }
    }
    if (lcddev.dir == 1)
    {
      xsize = lcddev.height;
      ysize = lcddev.width;
    }

    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(0); LCD_WR_DATA(0);
    LCD_WR_DATA((xsize - 1) >> 8); LCD_WR_DATA((xsize - 1) & 0XFF);
    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(0); LCD_WR_DATA(0);
    LCD_WR_DATA((ysize - 1) >> 8); LCD_WR_DATA((ysize - 1) & 0XFF);
  }
}

void LCD_Display_Dir(u8 dir)
{
  if (dir == 0)
  {
    lcddev.dir = 0;
    lcddev.width = 240;
    lcddev.height = 320;
  }
  else
  {
    lcddev.dir = 1;
    lcddev.width = 320;
    lcddev.height = 240;
  }
  lcddev.wramcmd = 0X2C;
  lcddev.setxcmd = 0X2A;
  lcddev.setycmd = 0X2B;

  LCD_Scan_Dir(L2R_U2D);
}

void LCD_Init(void)
{
	vu32 i=0;

  GPIO_InitTypeDef  GPIO_InitStructure;
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  readWriteTiming;
	FSMC_NORSRAMTimingInitTypeDef  writeTiming;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG, ENABLE);//使能PD,PE,PF,PG时钟
  RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC,ENABLE);//使能FSMC时钟


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;//PB15 推挽输出,控制背光
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化 //PB15 推挽输出,控制背光

  GPIO_InitStructure.GPIO_Pin = (3<<0)|(3<<4)|(7<<8)|(3<<14);//PD0,1,4,5,8,9,10,14,15 AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化

  GPIO_InitStructure.GPIO_Pin = (0X1FF<<7);//PE7~15,AF OUT
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;//PF12,FSMC_A6
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOG, &GPIO_InitStructure);//初始化

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


  readWriteTiming.FSMC_AddressSetupTime = 0XF;	 //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
  readWriteTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（ADDHLD）模式A未用到
  readWriteTiming.FSMC_DataSetupTime = 24;			//数据保存时间为25个HCLK	=6*25=150ns
  readWriteTiming.FSMC_BusTurnAroundDuration = 0x00;
  readWriteTiming.FSMC_CLKDivision = 0x00;
  readWriteTiming.FSMC_DataLatency = 0x00;
  readWriteTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A


	writeTiming.FSMC_AddressSetupTime =8;	      //地址建立时间（ADDSET）为8个HCLK =48ns
  writeTiming.FSMC_AddressHoldTime = 0x00;	 //地址保持时间（A
  writeTiming.FSMC_DataSetupTime = 8;		 //数据保存时间为6ns*9个HCLK=54ns
  writeTiming.FSMC_BusTurnAroundDuration = 0x00;
  writeTiming.FSMC_CLKDivision = 0x00;
  writeTiming.FSMC_DataLatency = 0x00;
  writeTiming.FSMC_AccessMode = FSMC_AccessMode_A;	 //模式A


  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;//  这里我们使用NE4 ，也就对应BTCR[6],[7]。
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable; // 不复用数据地址
  FSMC_NORSRAMInitStructure.FSMC_MemoryType =FSMC_MemoryType_SRAM;// FSMC_MemoryType_SRAM;  //SRAM
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;//存储器数据宽度为16bit
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode =FSMC_BurstAccessMode_Disable;// FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait=FSMC_AsynchronousWait_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;	//  存储器写使能
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Enable; // 读写使用不同的时序
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &readWriteTiming; //读写时序
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &writeTiming;  //写时序

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  //初始化FSMC配置

  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);  // 使能BANK1


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
    lcddev.id |= LCD_RD_DATA();
  }

  if (lcddev.id == 0X9341)
  {
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 0);
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 8);
    FSMC_Bank1E->BWTR[6] |= 3 << 0;
    FSMC_Bank1E->BWTR[6] |= 2 << 8;
  }

  if (lcddev.id == 0X9341) //9341
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

  LCD_Display_Dir(0);
  LCD_LED=1;
  LCD_Clear(MAGENTA);
}

void LCD_Clear(u16 color)
{
  u32 index = 0;
  u32 totalpoint = lcddev.width;

  totalpoint *= lcddev.height;
  LCD_SetCursor(0x00, 0x0000);
  LCD_WriteRAM_Prepare();

  for (index = 0; index < totalpoint; index++)
  {
    LCD->LCD_RAM = color;
  }

  LCD_SetCursor(0, 0); LCD_WriteRAM_Prepare(); LCD->LCD_RAM = 31;
  LCD_SetCursor(7, 0); LCD_WriteRAM_Prepare(); LCD->LCD_RAM = 31;
  LCD_SetCursor(7, 2); LCD_WriteRAM_Prepare(); LCD->LCD_RAM = 31;
  LCD_SetCursor(7, 4); LCD_WriteRAM_Prepare(); LCD->LCD_RAM = 31;
  LCD_SetCursor(7, 6); LCD_WriteRAM_Prepare(); LCD->LCD_RAM = 31;

}

void LCD_Show(cBitmap_Bpp16_5R6G5B* lpstBm)
{
  u32 index = 0;
  u32 totalpoint = lcddev.width * lcddev.height;

  LCD_SetCursor(0x00, 0x0000);
  LCD_WriteRAM_Prepare();

  for (index = 0; index < totalpoint; index++)
  {
    LCD->LCD_RAM = ((uint16*)lpstBm->mpui8Data)[index];
  }
}
