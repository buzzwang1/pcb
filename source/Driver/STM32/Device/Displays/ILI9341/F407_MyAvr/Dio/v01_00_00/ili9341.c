#include "ili9341.h"

// Hinweis  : Das Display benutzt die CPU-Pins :
//             PB9  -> LCD_Backlight

//             PB1  -> LCD_RS
//             PB0  -> LCD_WR
//             PB2  -> LCD_RD
//             PB4  -> LCD_CS
//             PB3  -> LCD_Reset

//             PE0  -> LCD_DB0
//             PE1  -> LCD_DB1
//             PE2  -> LCD_DB2
//             PE3  -> LCD_DB3
//             PE4  -> LCD_DB4
//             PE5  -> LCD_DB5
//             PE6  -> LCD_DB6
//             PE7  -> LCD_DB7
//             PE8  -> LCD_DB8
//             PE9  -> LCD_DB9
//             PE10 -> LCD_DB10
//             PE11 -> LCD_DB11
//             PE12 -> LCD_DB12
//             PE13 -> LCD_DB13
//             PE14 -> LCD_DB14
//             PE15 -> LCD_DB15


#define Wait 20

#define nCS     GPIO_Pin_4
#define RS      GPIO_Pin_1
#define nWR     GPIO_Pin_0
#define nRD     GPIO_Pin_2
#define nReset  GPIO_Pin_3
#define LCD_BK  GPIO_Pin_9


#define ILI9341_Dio_Set_CS() GPIO_SetBits(GPIOB, nCS)
#define ILI9341_Dio_Clr_CS() GPIO_ResetBits(GPIOB,   nCS)

#define ILI9341_Dio_Set_RS() GPIO_SetBits(GPIOB,   RS)
#define ILI9341_Dio_Clr_RS() GPIO_ResetBits(GPIOB, RS)
#define ILI9341_Dio_RS_Data() ILI9341_Dio_Set_RS()
#define ILI9341_Dio_RS_Cmd()  ILI9341_Dio_Clr_RS()

#define ILI9341_Dio_Set_RD() GPIO_SetBits(GPIOB, nRD)
#define ILI9341_Dio_Clr_RD() GPIO_ResetBits(GPIOB,   nRD)

#define ILI9341_Dio_Set_WR() GPIO_SetBits(GPIOB, nWR)
#define ILI9341_Dio_Clr_WR() GPIO_ResetBits(GPIOB,   nWR)

#define ILI9341_Dio_Set_BL() GPIO_SetBits(GPIOB, LCD_BK)
#define ILI9341_Dio_Clr_BL() GPIO_ResetBits(GPIOB,   LCD_BK)

#define ILI9341_Dio_Set_Reset() GPIO_SetBits(GPIOB, nReset)
#define ILI9341_Dio_Clr_Reset() GPIO_ResetBits(GPIOB,   nReset)

#define ILI9341_Dio_Set_Data_Out() GPIOE->MODER |= 0x55555555;
#define ILI9341_Dio_Set_Data_In()  GPIOE->MODER &= 0x00000000;


void ILI9341_Delay(volatile uint32 nCount)
{
  for(; nCount != 0; nCount--);
}


void ILI9341_Set_Dio_Data(uint16 lui16Data)
{
  GPIOE->ODR  = lui16Data;

  ILI9341_Dio_Clr_WR();
  ILI9341_Delay(Wait);
  ILI9341_Dio_Set_WR();
  ILI9341_Delay(Wait);
}

uint16 ILI9341_Dio_Get_Data()
{
  uint16 lui16Data;

  ILI9341_Dio_Clr_RD();
  ILI9341_Delay(Wait);
  lui16Data = GPIO_ReadInputData(GPIOE);
  ILI9341_Dio_Set_RD();
  ILI9341_Delay(Wait);

  return lui16Data;
}

void ILI9341_WriteReg(uint16 lui16Index, uint16 lui16Data)
{
 /************************************************************************
  **                                                                    **
  ** nCS       ----\__________________________________________/-------  **
  ** RS        ------\____________/-----------------------------------  **
  ** nRD       -------------------------------------------------------  **
  ** nWR       --------\_______/--------\_____/-----------------------  **
  ** DB[0:15]  ---------[index]----------[data]-----------------------  **
  **                                                                    **
  ************************************************************************/
  ILI9341_Dio_Set_CS();
    ILI9341_Dio_Set_Data_Out();
      ILI9341_Dio_RS_Cmd();
      ILI9341_Delay(Wait);
      ILI9341_Set_Dio_Data(lui16Index);
      ILI9341_Dio_RS_Data();
      ILI9341_Delay(Wait);
      ILI9341_Set_Dio_Data(lui16Data);
    ILI9341_Dio_Set_Data_In();
  ILI9341_Dio_Clr_CS();
}

void ili9341_WriteReg(uint16 lui16Data)
{
  ILI9341_Dio_Clr_CS();
    ILI9341_Dio_Set_RD();
    ILI9341_Dio_Set_Data_Out();
      ILI9341_Dio_RS_Cmd();
      ILI9341_Delay(Wait);
      ILI9341_Set_Dio_Data(lui16Data);
}

void ili9341_WriteData(uint16 lui16Data)
{
  ILI9341_Dio_Clr_CS();
    ILI9341_Dio_Set_RD();
    ILI9341_Dio_Set_Data_Out();
      ILI9341_Dio_RS_Data();
      ILI9341_Delay(Wait);
      ILI9341_Set_Dio_Data(lui16Data);
}

uint16 ILI9341_ReadReg(uint16 lui16Index)
{
 /************************************************************************
  **                                                                    **
  ** nCS       ----\__________________________________________/-------  **
  ** RS        ------\____________/-----------------------------------  **
  ** nRD       -------------------------\______/----------------------  **
  ** nWR       --------\_______/--------------------------------------  **
  ** DB[0:15]  ---------[index]----------[data]-----------------------  **
  **                                                                    **
  ************************************************************************/
  uint16 lui16Data;

  ILI9341_Dio_Clr_CS();
    ILI9341_Dio_Set_Data_Out();
      ILI9341_Dio_RS_Cmd();
      ILI9341_Delay(Wait);
        ILI9341_Set_Dio_Data(lui16Index);
        ILI9341_Delay(Wait);
      ILI9341_Dio_RS_Data();
      ILI9341_Delay(Wait);

    ILI9341_Dio_Set_Data_In();
      lui16Data = ILI9341_Dio_Get_Data();
  ILI9341_Dio_Set_CS();

  return lui16Data;
}

uint32 ILI9341_DisplayStatus(void)
{
 /************************************************************************
  **                                                                    **
  ** nCS       ----\__________________________________________/-------  **
  ** RS        ------\____________/-----------------------------------  **
  ** nRD       -------------------------\______/----------------------  **
  ** nWR       --------\_______/--------------------------------------  **
  ** DB[0:15]  ---------[index]----------[data]-----------------------  **
  **                                                                    **
  ************************************************************************/
  uint32 lui32Data;

  ILI9341_Dio_Clr_CS();
    ILI9341_Dio_Set_Data_Out();
      ILI9341_Dio_Clr_RS();
        ILI9341_Set_Dio_Data(4);
      ILI9341_Dio_Set_RS();

    ILI9341_Dio_Set_Data_In();

      lui32Data = 0;
      lui32Data += ILI9341_Dio_Get_Data() & 0xFF;
      lui32Data <<= 8;

      lui32Data += ILI9341_Dio_Get_Data() & 0xFF;
      lui32Data <<= 8;

      lui32Data += ILI9341_Dio_Get_Data() & 0xFF;
      lui32Data <<= 8;

      lui32Data += ILI9341_Dio_Get_Data() & 0xFF;

  ILI9341_Dio_Set_CS();

  return lui32Data;
}


void ILI9341_InitIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);

  GPIO_InitStructure.GPIO_Pin   = LCD_BK | nCS | nWR | RS | nRD | nReset;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  // Structur fuer Port-E
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_2  | GPIO_Pin_3 |
                                GPIO_Pin_4  | GPIO_Pin_5  | GPIO_Pin_6  | GPIO_Pin_7 |
                                GPIO_Pin_8  | GPIO_Pin_9  | GPIO_Pin_10 | GPIO_Pin_11 |
                                GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  // Config von Port-E
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}


void TFT_SetCurPos(uint16_t ulStartX, uint16_t ulEndX, uint16_t ulStartY, uint16_t ulEndY)
{

	ili9341_WriteReg(0x002a);
	ili9341_WriteData((ulStartX>>8) & 0x00ff);
	ili9341_WriteData(ulStartX & 0x00ff);
	ili9341_WriteData((ulEndX>>8) & 0x00ff);
	ili9341_WriteData((ulEndX) & 0x00ff);

     ili9341_WriteReg(0x002b);
     ili9341_WriteData((ulStartY>>8) & 0x00ff);
     ili9341_WriteData((ulStartY) & 0x00ff);
     ili9341_WriteData((ulEndY>>8) & 0x00ff);
     ili9341_WriteData(ulEndY & 0x00ff);
     ili9341_WriteReg(0x002c);
 }

 void TFT_Clear(uint16 ulColor)
 {
     unsigned long ulTemp;

  //
// Clear LCD
     //
     TFT_SetCurPos(0, 239, 0, 319);

     for(ulTemp = 0; ulTemp <= 239 * 319 / 2; ulTemp++)
     {
    	 ili9341_WriteData(ulColor);
     }
 }


uint8 ILI9341_Init(void)
{
  uint8 ret_wert = 1;
  uint16 t;
  volatile uint8  myarray[256];
  uint32 lui32UID;

  // IO-Lines initialisieren
  ILI9341_InitIO();

  ILI9341_Dio_Set_CS();
  ILI9341_Dio_Set_RS();
  ILI9341_Dio_Set_RD();
  ILI9341_Dio_Set_WR();

  ILI9341_Dio_Clr_BL();

  ILI9341_Dio_Clr_Reset();
  ILI9341_Delay(5000000);
  ILI9341_Dio_Set_Reset();
  ILI9341_Delay(500000);

  ILI9341_Dio_Set_CS();
    ILI9341_Dio_Set_Data_Out();
      ILI9341_Dio_RS_Cmd();
        ILI9341_Set_Dio_Data(1);
    ILI9341_Dio_Set_Data_In();
  ILI9341_Dio_Clr_CS();

  ILI9341_Delay(500000);

  for (t = 0; t < 256; t++)
  {
    myarray[t] = ILI9341_ReadReg(t);
  }

  // LCD-ID auslesen
  //lui32UID = ILI9341_DisplayStatus();
  lui32UID += ILI9341_ReadReg(4);
  lui32UID <<= 8;
  lui32UID += ILI9341_Dio_Get_Data();
  lui32UID <<= 8;
  lui32UID += ILI9341_Dio_Get_Data();


  if(lui32UID == ILI9341_ID)
  {
    // Display gefunden
    ret_wert = 0;

    // LCD-Controller initialisieren (Mode=Portrait)
    //P_LCD9325_InitChip(LCD_ILI9325_PORTRAIT);
    // Backlight einschalten
    ILI9341_Dio_Set_BL();
  }


  /* Configure LCD */
  ili9341_WriteReg(0xCA);
  ili9341_WriteData(0xC3);
  ili9341_WriteData(0x08);
  ili9341_WriteData(0x50);
  ili9341_WriteReg(LCD_POWERB);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0xC1);
  ili9341_WriteData(0x30);
  ili9341_WriteReg(LCD_POWER_SEQ);
  ili9341_WriteData(0x64);
  ili9341_WriteData(0x03);
  ili9341_WriteData(0x12);
  ili9341_WriteData(0x81);
  ili9341_WriteReg(LCD_DTCA);
  ili9341_WriteData(0x85);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x78);
  ili9341_WriteReg(LCD_POWERA);
  ili9341_WriteData(0x39);
  ili9341_WriteData(0x2C);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x34);
  ili9341_WriteData(0x02);
  ili9341_WriteReg(LCD_PRC);
  ili9341_WriteData(0x20);
  ili9341_WriteReg(LCD_DTCB);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_FRMCTR1);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x1B);
  ili9341_WriteReg(LCD_DFC);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0xA2);
  ili9341_WriteReg(LCD_POWER1);
  ili9341_WriteData(0x10);
  ili9341_WriteReg(LCD_POWER2);
  ili9341_WriteData(0x10);
  ili9341_WriteReg(LCD_VCOM1);
  ili9341_WriteData(0x45);
  ili9341_WriteData(0x15);
  ili9341_WriteReg(LCD_VCOM2);
  ili9341_WriteData(0x90);
  ili9341_WriteReg(LCD_MAC);
  ili9341_WriteData(0xC8);
  ili9341_WriteReg(LCD_3GAMMA_EN);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_RGB_INTERFACE);
  ili9341_WriteData(0xC2);
  ili9341_WriteReg(LCD_DFC);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0xA7);
  ili9341_WriteData(0x27);
  ili9341_WriteData(0x04);

  /* Colomn address set */
  ili9341_WriteReg(LCD_COLUMN_ADDR);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0xEF);
  /* Page address set */
  ili9341_WriteReg(LCD_PAGE_ADDR);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x01);
  ili9341_WriteData(0x3F);
  ili9341_WriteReg(LCD_INTERFACE);
  ili9341_WriteData(0x01);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x06);

  ili9341_WriteReg(LCD_GRAM);
  ILI9341_Delay(5000000);

  ili9341_WriteReg(LCD_GAMMA);
  ili9341_WriteData(0x01);

  ili9341_WriteReg(LCD_PGAMMA);
  ili9341_WriteData(0x0F);
  ili9341_WriteData(0x29);
  ili9341_WriteData(0x24);
  ili9341_WriteData(0x0C);
  ili9341_WriteData(0x0E);
  ili9341_WriteData(0x09);
  ili9341_WriteData(0x4E);
  ili9341_WriteData(0x78);
  ili9341_WriteData(0x3C);
  ili9341_WriteData(0x09);
  ili9341_WriteData(0x13);
  ili9341_WriteData(0x05);
  ili9341_WriteData(0x17);
  ili9341_WriteData(0x11);
  ili9341_WriteData(0x00);
  ili9341_WriteReg(LCD_NGAMMA);
  ili9341_WriteData(0x00);
  ili9341_WriteData(0x16);
  ili9341_WriteData(0x1B);
  ili9341_WriteData(0x04);
  ili9341_WriteData(0x11);
  ili9341_WriteData(0x07);
  ili9341_WriteData(0x31);
  ili9341_WriteData(0x33);
  ili9341_WriteData(0x42);
  ili9341_WriteData(0x05);
  ili9341_WriteData(0x0C);
  ili9341_WriteData(0x0A);
  ili9341_WriteData(0x28);
  ili9341_WriteData(0x2F);
  ili9341_WriteData(0x0F);

  ili9341_WriteReg(LCD_SLEEP_OUT);
  ILI9341_Delay(5000000);
  ili9341_WriteReg(LCD_DISPLAY_ON);
  /* GRAM start writing */
  ili9341_WriteReg(LCD_GRAM);

  TFT_Clear(0x5555);

  return(ret_wert);
}




/*

void ili9320_SetCursor(u16 x,u16 y)
{
  //LCD_WriteReg(0,0x02,x);
  //LCD_WriteReg(1,0x03,y);


  LCD_WriteReg(32, y);
  LCD_WriteReg(33, 319-x);
  //*(__IO uint16_t *) (Bank1_LCD_C)= 34;
  //LCD_WriteIndex(34);
}



void ili9320_SetWindows(u16 StartX,u16 StartY,u16 EndX,u16 EndY)
{
  LCD_WriteReg(0x0050, StartY); // Horizontal GRAM Start Address
  LCD_WriteReg(0x0051, EndX); // Horizontal GRAM End Address
  LCD_WriteReg(0x0052, 319-StartX); // Vertical GRAM Start Address
  LCD_WriteReg(0x0053, EndY); // Vertical GRAM Start Address

  //LCD_WriteIndex(34);
}


void ili9320_Clear(u16 dat)
{
  u32 i;
  LCD_WriteReg(0x0050, 0); // Horizontal GRAM Start Address
  LCD_WriteReg(0x0051, 239); // Horizontal GRAM End Address
  LCD_WriteReg(0x0052, 0); // Vertical GRAM Start Address
  LCD_WriteReg(0x0053, 319); // Vertical GRAM Start Address
  LCD_WriteReg(32, 0);
  LCD_WriteReg(33, 0);
		//*(__IO uint16_t *) (Bank1_LCD_C)= 34;
  LCD_WriteIndex(34);
  for(i=0;i<76800;i++) LCD_WriteRAM(dat);
}


u16 ili9320_GetPoint(u16 x,u16 y)
{
	//u16 temp;
  	ili9320_SetCursor(x,y);
  	LCD_WriteIndex(34);
  	return (ili9320_BGR2RGB(ili9320_ReadData()));

}

void ili9320_SetPoint(u16 x,u16 y,u16 point)
{

  LCD_WriteReg(32, y);
  LCD_WriteReg(33, 319-x);
  LCD_WriteIndex(34);
  LCD_WriteRAM(point);
}*/


