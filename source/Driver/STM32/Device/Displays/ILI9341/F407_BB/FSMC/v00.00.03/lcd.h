#ifndef __ILI9341_H
#define __ILI9341_H

#ifdef __cplusplus
extern "C" {
#endif

#include "TypeDef.h"
#include "misc2.h"
/* LCD Size (Width and Height) */
#define ILI9341_WIDTH 				240
#define ILI9341_HEIGHT				320
#define ILI9341_PIXEL				76800

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_fsmc.h"
#include "ClockInfo.h"
#include "bitmap_Bpp16_5R6G5B.h"


//////////////////////////////////////////////////////////////////////////////////
//-----------------LCD---------------- 

#define	ILI9341_LED PBout(15)  //LCD  PB15

volatile typedef struct
{
  u16 LCD_REG;
  u16 LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE        ((u32)(0x6C000000 | 0x0000007E))
#define LCD             ((LCD_TypeDef *) LCD_BASE)
//////////////////////////////////////////////////////////////////////////////////

// Colors
#define ILI9341_COLOR_WHITE			0xFFFF
#define ILI9341_COLOR_BLACK			0x0000
#define ILI9341_COLOR_RED			0xF800
#define ILI9341_COLOR_GREEN			0x07E0
#define ILI9341_COLOR_GREEN2		0xB723
#define ILI9341_COLOR_BLUE			0x001F
#define ILI9341_COLOR_BLUE2			0x051D
#define ILI9341_COLOR_YELLOW		0xFFE0
#define ILI9341_COLOR_ORANGE		0xFBE4
#define ILI9341_COLOR_CYAN			0x07FF
#define ILI9341_COLOR_MAGENTA		0xA254
#define ILI9341_COLOR_GRAY			0x7BEF //1111 0111 1101 1110
#define ILI9341_COLOR_BROWN			0xBBCA

// Commands
#define ILI9341_RESET					0x01
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_GAMMA					0x26
#define ILI9341_DISPLAY_OFF		0x28
#define ILI9341_DISPLAY_ON		0x29
#define ILI9341_COLUMN_ADDR		0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM					0x2C
#define ILI9341_MAC						0x36
#define ILI9341_PIXEL_FORMAT	0x3A
#define ILI9341_WDB						0x51
#define ILI9341_WCD						0x53
#define ILI9341_RGB_INTERFACE	0xB0
#define ILI9341_FRC						0xB1
#define ILI9341_BPC						0xB5
#define ILI9341_DFC						0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1					0xC5
#define ILI9341_VCOM2					0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA					0xE8
#define ILI9341_DTCB					0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC						0xF7

// Select orientation for LCD
typedef enum {
  ILI9341_Orientation_Portrait_1 = 0x50,
  ILI9341_Orientation_Portrait_2 = 0x80,
  ILI9341_Orientation_Landscape_1 = 0x60,
  ILI9341_Orientation_Landscape_2 = 0xA0
}ILI9341_Orientation_t;

// Orientation, Used private
typedef enum
{
  ILI9341_Landscape,
  ILI9341_Portrait
}ILI9341_Orientation;

void ILI9341_Init(void);
void ILI9341_DisplayOn(void);
void ILI9341_DisplayOff(void);

void ILI9341_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16  ILI9341_ReadReg(u16 LCD_Reg);
void ILI9341_WriteRAM_Prepare(void);
void ILI9341_WriteRAM(u16 RGB_Code);

void ILI9341_Rotate(ILI9341_Orientation_t orientation, uint8 BGR);
void ILI9341_Clear(u16 Color);
void ILI9341_Show(cBitmap_Bpp16_5R6G5B* lpstBm);

#ifdef __cplusplus
  }
#endif

#endif // ILI9341_H
