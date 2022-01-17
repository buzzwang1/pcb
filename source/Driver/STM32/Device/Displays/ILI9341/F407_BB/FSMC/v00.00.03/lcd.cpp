#include "lcd.h"

void ILI9341_WR_REG(vu16 regval)
{
  LCD->LCD_REG = regval;
}

void ILI9341_WR_DATA(vu16 lu16Data)
{
  LCD->LCD_RAM = lu16Data;
}

u16 ILI9341_RD_DATA(void)
{
  vu16 ram;
  ram = LCD->LCD_RAM;
  return ram;
}

void ILI9341_WriteReg(vu16 lu16Reg, vu16 lu16RegValue)
{
  LCD->LCD_REG = lu16Reg;
  LCD->LCD_RAM = lu16RegValue;
}

u16 ILI9341_ReadReg(vu16 lu16Reg)
{
  ILI9341_WR_REG(lu16Reg);
  cClockInfo::Delay_us(5);
  return ILI9341_RD_DATA();
}

void ILI9341_WriteRAM_Prepare(void)
{
  LCD->LCD_REG = ILI9341_GRAM;
}


void ILI9341_WriteRAM(u16 lu16Code)
{
  LCD->LCD_RAM = lu16Code;
}


u16 ILI9341_BGR2RGB(u16 c)
{
  u16  r, g, b, rgb;
  b = (c >> 0) & 0x1f;
  g = (c >> 5) & 0x3f;
  r = (c >> 11) & 0x1f;
  rgb = (b << 11) + (g << 5) + (r << 0);
  return(rgb);
}


void ILI9341_SetArea(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
  ILI9341_WR_REG(ILI9341_COLUMN_ADDR);
  ILI9341_WR_DATA(x1 >> 8);
  ILI9341_WR_DATA(x1 & 0xFF);
  ILI9341_WR_DATA(x2 >> 8);
  ILI9341_WR_DATA(x2 & 0xFF);

  ILI9341_WR_REG(ILI9341_PAGE_ADDR);
  ILI9341_WR_DATA(y1 >> 8);
  ILI9341_WR_DATA(y1 & 0xFF);
  ILI9341_WR_DATA(y2 >> 8);
  ILI9341_WR_DATA(y2 & 0xFF);
}


void ILI9341_Rotate(ILI9341_Orientation_t orientation, uint8 BGR)
{
  uint8 lui8Data;

  lui8Data = (uint8)orientation;

  if (lui8Data)
  {
    ILI9341_WR_REG(ILI9341_MAC);
    if (BGR) lui8Data |= 0x08;
    ILI9341_WR_DATA(lui8Data);
  }
}


void ILI9341_DisplayOn(void)
{
  ILI9341_WR_REG(0X29);
}

void ILI9341_DisplayOff(void)
{
  ILI9341_WR_REG(0X28);
}




void ILI9341_Init(void)
{
  u16  lu16LcdId;

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
 	ILI9341_WriteReg(0x0000,0x0001);
  cClockInfo::Delay_ms(50); // delay 50 ms

  lu16LcdId = ILI9341_ReadReg(0x0000);

  if (lu16LcdId < 0XFF || lu16LcdId == 0XFFFF || lu16LcdId == 0X9300)
  {
    ILI9341_WR_REG(0XD3);
    lu16LcdId = ILI9341_RD_DATA();   //dummy read
    lu16LcdId = ILI9341_RD_DATA();   //0X00
    lu16LcdId = ILI9341_RD_DATA();   //93
    lu16LcdId <<= 8;
    lu16LcdId |= ILI9341_RD_DATA();
  }

  if (lu16LcdId == 0X9341)
  {
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 0);
    FSMC_Bank1E->BWTR[6] &= ~(0XF << 8);
    FSMC_Bank1E->BWTR[6] |= 3 << 0;
    FSMC_Bank1E->BWTR[6] |= 2 << 8;
  }

  if (lu16LcdId == 0X9341) //9341
  {
    ILI9341_WR_REG(0xCF);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0xC1);
    ILI9341_WR_DATA(0X30);
    ILI9341_WR_REG(0xED);
    ILI9341_WR_DATA(0x64);
    ILI9341_WR_DATA(0x03);
    ILI9341_WR_DATA(0X12);
    ILI9341_WR_DATA(0X81);
    ILI9341_WR_REG(0xE8);
    ILI9341_WR_DATA(0x85);
    ILI9341_WR_DATA(0x10);
    ILI9341_WR_DATA(0x7A);
    ILI9341_WR_REG(0xCB);
    ILI9341_WR_DATA(0x39);
    ILI9341_WR_DATA(0x2C);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x34);
    ILI9341_WR_DATA(0x02);
    ILI9341_WR_REG(0xF7);
    ILI9341_WR_DATA(0x20);
    ILI9341_WR_REG(0xEA);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_REG(0xC0);    //Power control
    ILI9341_WR_DATA(0x1B);   //VRH[5:0]
    ILI9341_WR_REG(0xC1);    //Power control
    ILI9341_WR_DATA(0x01);   //SAP[2:0];BT[3:0]
    ILI9341_WR_REG(0xC5);    //VCM control
    ILI9341_WR_DATA(0x30);   //3F
    ILI9341_WR_DATA(0x30);   //3C
    ILI9341_WR_REG(0xC7);    //VCM control2
    ILI9341_WR_DATA(0XB7);
    ILI9341_WR_REG(0x36);    // Memory Access Control
    ILI9341_WR_DATA(0x04);   // 0 0 0 0 | 1 1 00 : My Mx Mv Ml : BRG Mh 00
    ILI9341_WR_REG(0x3A);
    ILI9341_WR_DATA(0x55);
    ILI9341_WR_REG(0xB1);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x1A);
    ILI9341_WR_REG(0xB6);    // Display Function Control
    ILI9341_WR_DATA(0x0A);
    ILI9341_WR_DATA(0xA2);
    ILI9341_WR_REG(0xF2);    // 3Gamma Function Disable
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_REG(0x26);    //Gamma curve selected
    ILI9341_WR_DATA(0x01);
    ILI9341_WR_REG(0xE0);    //Set Gamma
    ILI9341_WR_DATA(0x0F);
    ILI9341_WR_DATA(0x2A);
    ILI9341_WR_DATA(0x28);
    ILI9341_WR_DATA(0x08);
    ILI9341_WR_DATA(0x0E);
    ILI9341_WR_DATA(0x08);
    ILI9341_WR_DATA(0x54);
    ILI9341_WR_DATA(0XA9);
    ILI9341_WR_DATA(0x43);
    ILI9341_WR_DATA(0x0A);
    ILI9341_WR_DATA(0x0F);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_REG(0XE1);    //Set Gamma
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x15);
    ILI9341_WR_DATA(0x17);
    ILI9341_WR_DATA(0x07);
    ILI9341_WR_DATA(0x11);
    ILI9341_WR_DATA(0x06);
    ILI9341_WR_DATA(0x2B);
    ILI9341_WR_DATA(0x56);
    ILI9341_WR_DATA(0x3C);
    ILI9341_WR_DATA(0x05);
    ILI9341_WR_DATA(0x10);
    ILI9341_WR_DATA(0x0F);
    ILI9341_WR_DATA(0x3F);
    ILI9341_WR_DATA(0x3F);
    ILI9341_WR_DATA(0x0F);
    ILI9341_WR_REG(0x2B);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x01);
    ILI9341_WR_DATA(0x3f);
    ILI9341_WR_REG(0x2A);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0x00);
    ILI9341_WR_DATA(0xef);
    ILI9341_WR_REG(0x11); //Exit Sleep
    cClockInfo::Delay_ms(120);

    ILI9341_WR_REG(0x29); //display on
  }

  ILI9341_Rotate(ILI9341_Orientation_Landscape_2, 1);
  ILI9341_LED=1;
  ILI9341_Clear(ILI9341_COLOR_MAGENTA);
}

void ILI9341_Clear(u16 color)
{
  u32 index = 0;
  ILI9341_SetArea(0, 0, ILI9341_HEIGHT - 1, ILI9341_WIDTH - 1);
  ILI9341_WriteRAM_Prepare();

  for (index = 0; index < ILI9341_PIXEL; index++)
  {
    LCD->LCD_RAM = color;
  }
}

void ILI9341_Show(cBitmap_Bpp16_5R6G5B* lpstBm)
{
  u32 index = 0;

  ILI9341_SetArea(0, 0, ILI9341_HEIGHT - 1, ILI9341_WIDTH - 1);
  ILI9341_WriteRAM_Prepare();

  for (index = 0; index < ILI9341_PIXEL; index++)
  {
    LCD->LCD_RAM = ((uint16*)lpstBm->mpui8Data)[index];
  }
}
