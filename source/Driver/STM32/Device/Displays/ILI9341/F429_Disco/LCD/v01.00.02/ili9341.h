
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F429I_DISCOVERY_ILI9341_H
#define __STM32F429I_DISCOVERY_ILI9341_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "TYPEDEF.h"
#include "ClockInfo.h"

#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_dma2d.h"
#include "stm32f4xx_ltdc.h"
#include "misc.h"

#include "bitmap_Bpp16_5R6G5B.h"
//#include "stm32f429i_discovery_sdram.h"


/* LCD Size (Width and Height) */
#define  ILI9341_WIDTH    ((uint16_t)240)
#define  ILI9341_HEIGHT   ((uint16_t)320)

#define ILI9341_FRAME_BUFFER       ((uint32_t)0xD0000000)
#define ILI9341_BUFFER_OFFSET      ((uint32_t)0x50000)



/**
  * @brief  LCD Control pin
  */
#define ILI9341_NCS_PIN             GPIO_Pin_2
#define ILI9341_NCS_GPIO_PORT       GPIOC
#define ILI9341_NCS_GPIO_CLK        RCC_AHB1Periph_GPIOC

/**
  * @brief  LCD Command/data pin
  */
#define ILI9341_WRX_PIN             GPIO_Pin_13
#define ILI9341_WRX_GPIO_PORT       GPIOD
#define ILI9341_WRX_GPIO_CLK        RCC_AHB1Periph_GPIOD

/**
  * @brief  LCD SPI Interface pins
  */
#define ILI9341_SPI_SCK_PIN               GPIO_Pin_7                     /* PF.07 */
#define ILI9341_SPI_SCK_GPIO_PORT         GPIOF                          /* GPIOF */
#define ILI9341_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOF
#define ILI9341_SPI_SCK_SOURCE            GPIO_PinSource7
#define ILI9341_SPI_SCK_AF                GPIO_AF_SPI5
#define ILI9341_SPI_MISO_PIN              GPIO_Pin_8                     /* PF.08 */
#define ILI9341_SPI_MISO_GPIO_PORT        GPIOF                          /* GPIOF */
#define ILI9341_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define ILI9341_SPI_MISO_SOURCE           GPIO_PinSource8
#define ILI9341_SPI_MISO_AF               GPIO_AF_SPI5
#define ILI9341_SPI_MOSI_PIN              GPIO_Pin_9                     /* PF.09 */
#define ILI9341_SPI_MOSI_GPIO_PORT        GPIOF                          /* GPIOF */
#define ILI9341_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOF
#define ILI9341_SPI_MOSI_SOURCE           GPIO_PinSource9
#define ILI9341_SPI_MOSI_AF               GPIO_AF_SPI5
#define ILI9341_SPI                       SPI5
#define ILI9341_SPI_CLK                   RCC_APB2Periph_SPI5

/**
  * @brief  LCD Registers
  */
#define ILI9341_SLEEP_OUT            0x11   /* Sleep out register */
#define ILI9341_GAMMA                0x26   /* Gamma register */
#define ILI9341_DISPLAY_OFF          0x28   /* Display off register */
#define ILI9341_DISPLAY_ON           0x29   /* Display on register */
#define ILI9341_COLUMN_ADDR          0x2A   /* Colomn address register */
#define ILI9341_PAGE_ADDR            0x2B   /* Page address register */
#define ILI9341_GRAM                 0x2C   /* GRAM register */
#define ILI9341_MAC                  0x36   /* Memory Access Control register*/
#define ILI9341_PIXEL_FORMAT         0x3A   /* Pixel Format register */
#define ILI9341_WDB                  0x51   /* Write Brightness Display register */
#define ILI9341_WCD                  0x53   /* Write Control Display register*/
#define ILI9341_RGB_INTERFACE        0xB0   /* RGB Interface Signal Control */
#define ILI9341_FRC                  0xB1   /* Frame Rate Control register */
#define ILI9341_BPC                  0xB5   /* Blanking Porch Control register*/
#define ILI9341_DFC                  0xB6   /* Display Function Control register*/
#define ILI9341_POWER1               0xC0   /* Power Control 1 register */
#define ILI9341_POWER2               0xC1   /* Power Control 2 register */
#define ILI9341_VCOM1                0xC5   /* VCOM Control 1 register */
#define ILI9341_VCOM2                0xC7   /* VCOM Control 2 register */
#define ILI9341_POWERA               0xCB   /* Power control A register */
#define ILI9341_POWERB               0xCF   /* Power control B register */
#define ILI9341_PGAMMA               0xE0   /* Positive Gamma Correction register*/
#define ILI9341_NGAMMA               0xE1   /* Negative Gamma Correction register*/
#define ILI9341_DTCA                 0xE8   /* Driver timing control A */
#define ILI9341_DTCB                 0xEA   /* Driver timing control B */
#define ILI9341_POWER_SEQ            0xED   /* Power on sequence register */
#define ILI9341_3GAMMA_EN            0xF2   /* 3 Gamma enable register */
#define ILI9341_INTERFACE            0xF6   /* Interface control register */
#define ILI9341_PRC                  0xF7   /* Pump ratio control register */

/**
  * @brief  LCD color
  */
#define ILI9341_COLOR_WHITE          0xFFFF
#define ILI9341_COLOR_BLACK          0x0000
#define ILI9341_COLOR_GREY           0xF7DE
#define ILI9341_COLOR_BLUE           0x001F
#define ILI9341_COLOR_BLUE2          0x051F
#define ILI9341_COLOR_RED            0xF800
#define ILI9341_COLOR_MAGENTA        0xF81F
#define ILI9341_COLOR_GREEN          0x07E0
#define ILI9341_COLOR_CYAN           0x7FFF
#define ILI9341_COLOR_YELLOW         0xFFE0




/**
  * @brief  LCD Direction
  */
#define ILI9341_DIR_HORIZONTAL       0x0000
#define ILI9341_DIR_VERTICAL         0x0001

/**
  * @}
  */

/**
  * @brief  LCD Layer
  */
#define ILI9341_BACKGROUND_LAYER     0x0000
#define ILI9341_FOREGROUND_LAYER     0x0001



/** @defgroup STM32F429I_DISCOVERY_ILI9341_Exported_Functions
  * @{
  */
void     ILI9341_DeInit(void);
void     ILI9341_Init(cBitmap_Bpp16_5R6G5B* lpstBm);
void     ILI9341_ChipSelect(FunctionalState NewState);

void     ILI9341_ClearScreen(u16 u16Col);
void     ILI9341_Show(cBitmap_Bpp16_5R6G5B* lpstBm);

void     ILI9341_WriteCommand(uint8_t ILI9341_Reg);
void     ILI9341_WriteData(uint8_t value);
void     ILI9341_PowerOn(void);
void     ILI9341_DisplayOn(void);
void     ILI9341_DisplayOff(void);
void     ILI9341_CtrlLinesConfig(void);
void     ILI9341_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal);
void     ILI9341_SPIConfig(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F429I_DISCOVERY_ILI9341_H */
