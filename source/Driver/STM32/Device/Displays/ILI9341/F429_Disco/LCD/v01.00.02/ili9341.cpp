/* Includes ------------------------------------------------------------------*/
#include "ili9341.h"


static void ILI9341_AF_GPIOConfig(void);
static void ILI9341_LayerInit(cBitmap_Bpp16_5R6G5B* lpstBm);

void ILI9341_DeInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* LCD Display Off */
  ILI9341_DisplayOff();

  /* ILI9341_SPI disable */
  SPI_Cmd(ILI9341_SPI, DISABLE);

  /* ILI9341_SPI DeInit */
  SPI_I2S_DeInit(ILI9341_SPI);

  /* Disable SPI clock  */
  RCC_APB2PeriphClockCmd(ILI9341_SPI_CLK, DISABLE);

  /* Configure NCS in Output Push-Pull mode */
  GPIO_InitStructure.GPIO_Pin = ILI9341_NCS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(ILI9341_NCS_GPIO_PORT, &GPIO_InitStructure);

  /* Configure SPI pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = ILI9341_SPI_SCK_PIN;
  GPIO_Init(ILI9341_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = ILI9341_SPI_MISO_PIN;
  GPIO_Init(ILI9341_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = ILI9341_SPI_MOSI_PIN;
  GPIO_Init(ILI9341_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* GPIOA configuration */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_MCO);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 |
                                GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* GPIOB configuration */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_MCO);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_1  | GPIO_Pin_8    |
                                GPIO_Pin_9  |  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* GPIOC configuration */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_MCO);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_MCO);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3  | GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_MCO);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_MCO);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, GPIO_AF_MCO);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6  | GPIO_Pin_7  | GPIO_Pin_10    |
                                GPIO_Pin_11 | GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOG, &GPIO_InitStructure);
}

/**
  * @brief  Initializes the LCD.
  * @param  None
  * @retval None
  */
void ILI9341_Init(cBitmap_Bpp16_5R6G5B* lpstBm)
{
  LTDC_InitTypeDef       LTDC_InitStruct;

  /* Configure the LCD Control pins ------------------------------------------*/
  ILI9341_CtrlLinesConfig();
  ILI9341_ChipSelect(DISABLE);
  ILI9341_ChipSelect(ENABLE);

  /* Configure the ILI9341_SPI interface -----------------------------------------*/
  ILI9341_SPIConfig();

  /* Power on the LCD --------------------------------------------------------*/
  ILI9341_PowerOn();

  /* Enable the LTDC Clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_LTDC, ENABLE);

  /* Enable the DMA2D Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2D, ENABLE);

  /* Configure the LCD Control pins */
  ILI9341_AF_GPIOConfig();

  /* LTDC Configuration *********************************************************/
  /* Polarity configuration */
  /* Initialize the horizontal synchronization polarity as active low */
  LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AL;
  /* Initialize the vertical synchronization polarity as active low */
  LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
  /* Initialize the data enable polarity as active low */
  LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AL;
  /* Initialize the pixel clock polarity as input pixel clock */
  LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

  /* Configure R,G,B component values for LCD background color */
  LTDC_InitStruct.LTDC_BackgroundRedValue = 0;
  LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
  LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;

  /* Configure PLLSAI prescalers for LCD */
  /* Enable Pixel Clock */
  /* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
  /* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
  /* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/4 = 48 Mhz */
  /* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 48/8 = 6 Mhz */
  RCC_PLLSAIConfig(192, 7, 4);
  RCC_LTDCCLKDivConfig(RCC_PLLSAIDivR_Div8);

  /* Enable PLLSAI Clock */
  RCC_PLLSAICmd(ENABLE);
  /* Wait for PLLSAI activation */
  while(RCC_GetFlagStatus(RCC_FLAG_PLLSAIRDY) == RESET)
  {
  }

  /* Timing configuration */
  /* Configure horizontal synchronization width */
  LTDC_InitStruct.LTDC_HorizontalSync = 9;
  /* Configure vertical synchronization height */
  LTDC_InitStruct.LTDC_VerticalSync = 1;
  /* Configure accumulated horizontal back porch */
  LTDC_InitStruct.LTDC_AccumulatedHBP = 29;
  /* Configure accumulated vertical back porch */
  LTDC_InitStruct.LTDC_AccumulatedVBP = 3;
  /* Configure accumulated active width */
  LTDC_InitStruct.LTDC_AccumulatedActiveW = 269;
  /* Configure accumulated active height */
  LTDC_InitStruct.LTDC_AccumulatedActiveH = 323;
  /* Configure total width */
  LTDC_InitStruct.LTDC_TotalWidth = 279;
  /* Configure total height */
  LTDC_InitStruct.LTDC_TotalHeigh = 327;

  LTDC_Init(&LTDC_InitStruct);

  ILI9341_LayerInit(lpstBm);

  LTDC_Cmd(ENABLE);
}

/**
  * @brief  Initializes the LCD Layers.
  * @param  None
  * @retval None
  */
void ILI9341_LayerInit(cBitmap_Bpp16_5R6G5B* lpstBm)
{
  LTDC_Layer_InitTypeDef LTDC_Layer_InitStruct;

  /* Windowing configuration */
  /* In this case all the active display area is used to display a picture then :
  Horizontal start = horizontal synchronization + Horizontal back porch = 30
  Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
  Vertical start   = vertical synchronization + vertical back porch     = 4
  Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */
  LTDC_Layer_InitStruct.LTDC_HorizontalStart = 30;
  LTDC_Layer_InitStruct.LTDC_HorizontalStop = (ILI9341_WIDTH + 30 - 1);
  LTDC_Layer_InitStruct.LTDC_VerticalStart = 4;
  LTDC_Layer_InitStruct.LTDC_VerticalStop = (ILI9341_HEIGHT + 4 - 1);

  /* Pixel Format configuration*/
  LTDC_Layer_InitStruct.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
  /* Alpha constant (255 totally opaque) */
  LTDC_Layer_InitStruct.LTDC_ConstantAlpha = 255;
  /* Default Color configuration (configure A,R,G,B component values) */
  LTDC_Layer_InitStruct.LTDC_DefaultColorBlue = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorGreen = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorRed = 0;
  LTDC_Layer_InitStruct.LTDC_DefaultColorAlpha = 0;
  /* Configure blending factors */
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
  LTDC_Layer_InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

  /* the length of one line of pixels in bytes + 3 then :
  Line Lenth = Active high width x number of bytes per pixel + 3
  Active high width         = ILI9341_WIDTH
  number of bytes per pixel = 2    (pixel_format : RGB565)
  */
  LTDC_Layer_InitStruct.LTDC_CFBLineLength = ((ILI9341_WIDTH * 2) + 3);
  /* the pitch is the increment from the start of one line of pixels to the
  start of the next line in bytes, then :
  Pitch = Active high width x number of bytes per pixel */
  LTDC_Layer_InitStruct.LTDC_CFBPitch = (ILI9341_WIDTH * 2);

  /* Configure the number of lines */
  LTDC_Layer_InitStruct.LTDC_CFBLineNumber = ILI9341_HEIGHT;

  /* Start Address configuration : the LCD Frame buffer is defined on SDRAM */
  LTDC_Layer_InitStruct.LTDC_CFBStartAdress = (uint32)lpstBm->mpui8Data;

  /* Initialize LTDC layer 1 */
  LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_InitStruct);

  /* LTDC configuration reload */
  LTDC_ReloadConfig(LTDC_IMReload);

  /* Enable only background Layers */
  LTDC_LayerCmd(LTDC_Layer1, ENABLE);

  /* LTDC configuration reload */
  LTDC_ReloadConfig(LTDC_IMReload);

  /* dithering activation */
  LTDC_DitherCmd(ENABLE);
}

/**
  * @brief  Controls LCD Chip Select (CS) pin
  * @param  NewState CS pin state
  * @retval None
  */
void ILI9341_ChipSelect(FunctionalState NewState)
{
  if (NewState == DISABLE)
  {
    GPIO_ResetBits(ILI9341_NCS_GPIO_PORT, ILI9341_NCS_PIN); /* CS pin low: LCD disabled */
  }
  else
  {
    GPIO_SetBits(ILI9341_NCS_GPIO_PORT, ILI9341_NCS_PIN); /* CS pin high: LCD enabled */
  }
}

#define  ILI9341_WIDTH    ((uint16_t)240)
#define  ILI9341_HEIGHT   ((uint16_t)320)

void ILI9341_ClearScreen(u16 u16Col)
{
  uint32      lui32t;

  for (lui32t = 0; lui32t < ILI9341_WIDTH*ILI9341_HEIGHT/2; lui32t++)
  {
    *(__IO uint16_t*)(ILI9341_FRAME_BUFFER) = u16Col;
  }
}

/**
  * @brief  Displays a mono-color picture.
  * @param  Pict: pointer to the picture array.
  * @retval None
  */
void ILI9341_Show(cBitmap_Bpp16_5R6G5B* lpstBm)
{
  uint32      lui32t;
  uint16*     lpui16ScreenData;

  //BM_CHK_INIT(*lpstBm);

  lpui16ScreenData = (uint16*)lpstBm->mpui8Data;

  for (lui32t = 0; lui32t < ILI9341_WIDTH*ILI9341_HEIGHT/2; lui32t++)
  {
    *(__IO uint16_t*)(ILI9341_FRAME_BUFFER) = *lpui16ScreenData;
    lpui16ScreenData++;
  }
}




/**
  * @brief  Writes command to select the LCD register.
  * @param  ILI9341_Reg: address of the selected register.
  * @retval None
  */
void ILI9341_WriteCommand(uint8_t ILI9341_Reg)
{
    /* Reset WRX to send command */
  ILI9341_CtrlLinesWrite(ILI9341_WRX_GPIO_PORT, ILI9341_WRX_PIN, Bit_RESET);

  /* Reset LCD control line(/CS) and Send command */
  ILI9341_ChipSelect(DISABLE);
  SPI_I2S_SendData(ILI9341_SPI, ILI9341_Reg);

  /* Wait until a data is sent(not busy), before config /CS HIGH */

  while(SPI_I2S_GetFlagStatus(ILI9341_SPI, SPI_I2S_FLAG_TXE) == RESET) ;

  while(SPI_I2S_GetFlagStatus(ILI9341_SPI, SPI_I2S_FLAG_BSY) != RESET);

  ILI9341_ChipSelect(ENABLE);
}

/**
  * @brief  Writes data to select the LCD register.
  *         This function must be used after ILI9341_WriteCommand() function
  * @param  value: data to write to the selected register.
  * @retval None
  */
void ILI9341_WriteData(uint8_t value)
{
    /* Set WRX to send data */
  ILI9341_CtrlLinesWrite(ILI9341_WRX_GPIO_PORT, ILI9341_WRX_PIN, Bit_SET);

  /* Reset LCD control line(/CS) and Send data */
  ILI9341_ChipSelect(DISABLE);
  SPI_I2S_SendData(ILI9341_SPI, value);

  /* Wait until a data is sent(not busy), before config /CS HIGH */

  while(SPI_I2S_GetFlagStatus(ILI9341_SPI, SPI_I2S_FLAG_TXE) == RESET) ;

  while(SPI_I2S_GetFlagStatus(ILI9341_SPI, SPI_I2S_FLAG_BSY) != RESET);

  ILI9341_ChipSelect(ENABLE);
}

/**
  * @brief  Configure the LCD controller (Power On sequence as described in ILI9341 Datasheet)
  * @param  None
  * @retval None
  */
void ILI9341_PowerOn(void)
{
  ILI9341_WriteCommand(0xCA);
  ILI9341_WriteData(0xC3);
  ILI9341_WriteData(0x08);
  ILI9341_WriteData(0x50);
  ILI9341_WriteCommand(ILI9341_POWERB);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0xC1);
  ILI9341_WriteData(0x30);
  ILI9341_WriteCommand(ILI9341_POWER_SEQ);
  ILI9341_WriteData(0x64);
  ILI9341_WriteData(0x03);
  ILI9341_WriteData(0x12);
  ILI9341_WriteData(0x81);
  ILI9341_WriteCommand(ILI9341_DTCA);
  ILI9341_WriteData(0x85);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x78);
  ILI9341_WriteCommand(ILI9341_POWERA);
  ILI9341_WriteData(0x39);
  ILI9341_WriteData(0x2C);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x34);
  ILI9341_WriteData(0x02);
  ILI9341_WriteCommand(ILI9341_PRC);
  ILI9341_WriteData(0x20);
  ILI9341_WriteCommand(ILI9341_DTCB);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteCommand(ILI9341_FRC);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x1B);
  ILI9341_WriteCommand(ILI9341_DFC);
  ILI9341_WriteData(0x0A);
  ILI9341_WriteData(0xA2);
  ILI9341_WriteCommand(ILI9341_POWER1);
  ILI9341_WriteData(0x10);
  ILI9341_WriteCommand(ILI9341_POWER2);
  ILI9341_WriteData(0x10);
  ILI9341_WriteCommand(ILI9341_VCOM1);
  ILI9341_WriteData(0x45);
  ILI9341_WriteData(0x15);
  ILI9341_WriteCommand(ILI9341_VCOM2);
  ILI9341_WriteData(0x90);
  ILI9341_WriteCommand(ILI9341_MAC);
  ILI9341_WriteData(0xC8);
  ILI9341_WriteCommand(ILI9341_3GAMMA_EN);
  ILI9341_WriteData(0x00);
  ILI9341_WriteCommand(ILI9341_RGB_INTERFACE);
  ILI9341_WriteData(0xC2);
  ILI9341_WriteCommand(ILI9341_DFC);
  ILI9341_WriteData(0x0A);
  ILI9341_WriteData(0xA7);
  ILI9341_WriteData(0x27);
  ILI9341_WriteData(0x04);

  /* colomn address set */
  ILI9341_WriteCommand(ILI9341_COLUMN_ADDR);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0xEF);
  /* Page Address Set */
  ILI9341_WriteCommand(ILI9341_PAGE_ADDR);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x01);
  ILI9341_WriteData(0x3F);
  ILI9341_WriteCommand(ILI9341_INTERFACE);
  ILI9341_WriteData(0x01);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x06);

  ILI9341_WriteCommand(ILI9341_GRAM);
  cClockInfo::Delay_ms(200);

  ILI9341_WriteCommand(ILI9341_GAMMA);
  ILI9341_WriteData(0x01);

  ILI9341_WriteCommand(ILI9341_PGAMMA);
  ILI9341_WriteData(0x0F);
  ILI9341_WriteData(0x29);
  ILI9341_WriteData(0x24);
  ILI9341_WriteData(0x0C);
  ILI9341_WriteData(0x0E);
  ILI9341_WriteData(0x09);
  ILI9341_WriteData(0x4E);
  ILI9341_WriteData(0x78);
  ILI9341_WriteData(0x3C);
  ILI9341_WriteData(0x09);
  ILI9341_WriteData(0x13);
  ILI9341_WriteData(0x05);
  ILI9341_WriteData(0x17);
  ILI9341_WriteData(0x11);
  ILI9341_WriteData(0x00);
  ILI9341_WriteCommand(ILI9341_NGAMMA);
  ILI9341_WriteData(0x00);
  ILI9341_WriteData(0x16);
  ILI9341_WriteData(0x1B);
  ILI9341_WriteData(0x04);
  ILI9341_WriteData(0x11);
  ILI9341_WriteData(0x07);
  ILI9341_WriteData(0x31);
  ILI9341_WriteData(0x33);
  ILI9341_WriteData(0x42);
  ILI9341_WriteData(0x05);
  ILI9341_WriteData(0x0C);
  ILI9341_WriteData(0x0A);
  ILI9341_WriteData(0x28);
  ILI9341_WriteData(0x2F);
  ILI9341_WriteData(0x0F);

  ILI9341_WriteCommand(ILI9341_SLEEP_OUT);
  cClockInfo::Delay_ms(200);
  ILI9341_WriteCommand(ILI9341_DISPLAY_ON);
  /* GRAM start writing */
  ILI9341_WriteCommand(ILI9341_GRAM);
 }

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void ILI9341_DisplayOn(void)
{
  ILI9341_WriteCommand(ILI9341_DISPLAY_ON);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void ILI9341_DisplayOff(void)
{
    /* Display Off */
    ILI9341_WriteCommand(ILI9341_DISPLAY_OFF);
}

/**
  * @brief  Configures LCD control lines in Output Push-Pull mode.
  * @note   The ILI9341_NCS line can be configured in Open Drain mode
  *         when VDDIO is lower than required LCD supply.
  * @param  None
  * @retval None
  */
void ILI9341_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clock*/
  RCC_AHB1PeriphClockCmd(ILI9341_NCS_GPIO_CLK | ILI9341_WRX_GPIO_CLK, ENABLE);

  /* Configure NCS in Output Push-Pull mode */
  GPIO_InitStructure.GPIO_Pin = ILI9341_NCS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(ILI9341_NCS_GPIO_PORT, &GPIO_InitStructure);

  /* Configure WRX in Output Push-Pull mode */
  GPIO_InitStructure.GPIO_Pin = ILI9341_WRX_PIN;
  GPIO_Init(ILI9341_WRX_GPIO_PORT, &GPIO_InitStructure);

  /* Set chip select pin high */
  ILI9341_CtrlLinesWrite(ILI9341_NCS_GPIO_PORT, ILI9341_NCS_PIN, Bit_SET);
}

/**
  * @brief  Sets or reset LCD control lines.
  * @param  GPIOx: where x can be B or D to select the GPIO peripheral.
  * @param  CtrlPins: the Control line.
  *   This parameter can be:
  *     @arg ILI9341_NCS_PIN: Chip Select pin
  *     @arg ILI9341_NWR_PIN: Read/Write Selection pin
  *     @arg ILI9341_RS_PIN: Register/RAM Selection pin
  * @param  BitVal: specifies the value to be written to the selected bit.
  *   This parameter can be:
  *     @arg Bit_RESET: to clear the port pin
  *     @arg Bit_SET: to set the port pin
  * @retval None
  */
void ILI9341_CtrlLinesWrite(GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal)
{
  /* Set or Reset the control line */
  GPIO_WriteBit(GPIOx, (uint16_t)CtrlPins, (BitAction)BitVal);
}

/**
  * @brief  Configures the ILI9341_SPI interface.
  * @param  None
  * @retval None
  */
void ILI9341_SPIConfig(void)
{
  SPI_InitTypeDef    SPI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable ILI9341_SPI_SCK_GPIO_CLK, ILI9341_SPI_MISO_GPIO_CLK and ILI9341_SPI_MOSI_GPIO_CLK clock */
  RCC_AHB1PeriphClockCmd(ILI9341_SPI_SCK_GPIO_CLK | ILI9341_SPI_MISO_GPIO_CLK | ILI9341_SPI_MOSI_GPIO_CLK, ENABLE);

  /* Enable ILI9341_SPI and SYSCFG clock  */
  RCC_APB2PeriphClockCmd(ILI9341_SPI_CLK, ENABLE);

  /* Configure ILI9341_SPI SCK pin */
  GPIO_InitStructure.GPIO_Pin = ILI9341_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(ILI9341_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* Configure ILI9341_SPI MISO pin */
  GPIO_InitStructure.GPIO_Pin = ILI9341_SPI_MISO_PIN;
  GPIO_Init(ILI9341_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* Configure ILI9341_SPI MOSI pin */
  GPIO_InitStructure.GPIO_Pin = ILI9341_SPI_MOSI_PIN;
  GPIO_Init(ILI9341_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* Connect SPI SCK */
  GPIO_PinAFConfig(ILI9341_SPI_SCK_GPIO_PORT, ILI9341_SPI_SCK_SOURCE, ILI9341_SPI_SCK_AF);

  /* Connect SPI MISO */
  GPIO_PinAFConfig(ILI9341_SPI_MISO_GPIO_PORT, ILI9341_SPI_MISO_SOURCE, ILI9341_SPI_MISO_AF);

  /* Connect SPI MOSI */
  GPIO_PinAFConfig(ILI9341_SPI_MOSI_GPIO_PORT, ILI9341_SPI_MOSI_SOURCE, ILI9341_SPI_MOSI_AF);

  SPI_I2S_DeInit(ILI9341_SPI);

  /* SPI configuration -------------------------------------------------------*/
  /* If the SPI peripheral is already enabled, don't reconfigure it */
  if ((ILI9341_SPI->CR1 & SPI_CR1_SPE) == 0)
  {
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    /* SPI baudrate is set to 5.6 MHz (PCLK2/SPI_BaudRatePrescaler = 90/16 = 5.625 MHz)
       to verify these constraints:
          - ILI9341 LCD SPI interface max baudrate is 10MHz for write and 6.66MHz for read
          - l3gd20 SPI interface max baudrate is 10MHz for write/read
          - PCLK2 frequency is set to 90 MHz
       */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(ILI9341_SPI, &SPI_InitStructure);

    /* Enable L3GD20_SPI  */
    SPI_Cmd(ILI9341_SPI, ENABLE);
  }
}

/**
  * @brief  GPIO config for LTDC.
  * @param  None
  * @retval None
  */
static void ILI9341_AF_GPIOConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOF, GPIOG AHB Clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | \
                         RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD | \
                         RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE);

/* GPIOs Configuration */
/*
 +----------------------------+---------------------------+--------------------------------+
 +                                  LCD pins assignment                                    +
 +----------------------------+---------------------------+--------------------------------+
 |  ILI9341_TFT R2 <-> PC.10  |  ILI9341_TFT G2 <-> PA.06 |  ILI9341_TFT B2 <-> PD.06      |
 |  ILI9341_TFT R3 <-> PB.00  |  ILI9341_TFT G3 <-> PG.10 |  ILI9341_TFT B3 <-> PG.11      |
 |  ILI9341_TFT R4 <-> PA.11  |  ILI9341_TFT G4 <-> PB.10 |  ILI9341_TFT B4 <-> PG.12      |
 |  ILI9341_TFT R5 <-> PA.12  |  ILI9341_TFT G5 <-> PB.11 |  ILI9341_TFT B5 <-> PA.03      |
 |  ILI9341_TFT R6 <-> PB.01  |  ILI9341_TFT G6 <-> PC.07 |  ILI9341_TFT B6 <-> PB.08      |
 |  ILI9341_TFT R7 <-> PG.06  |  ILI9341_TFT G7 <-> PD.03 |  ILI9341_TFT B7 <-> PB.09      |
 -------------------------------------------------------------------------------
          |  ILI9341_TFT HSYNC <-> PC.06  | ILI9341_TFT VSYNC <->  PA.04 |
          |  ILI9341_TFT CLK   <-> PG.07  | ILI9341_TFT DE    <->  PF.10 |
           --------------------------------------------------------------

*/

 /* GPIOA configuration */
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource4, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource12, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | \
                             GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

 /* GPIOB configuration */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, 0x09);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource1, 0x09);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | \
                             GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;

  GPIO_Init(GPIOB, &GPIO_InitStruct);

 /* GPIOC configuration */
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10;

  GPIO_Init(GPIOC, &GPIO_InitStruct);

 /* GPIOD configuration */
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource3, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;

  GPIO_Init(GPIOD, &GPIO_InitStruct);

 /* GPIOF configuration */
  GPIO_PinAFConfig(GPIOF, GPIO_PinSource10, GPIO_AF_LTDC);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;

  GPIO_Init(GPIOF, &GPIO_InitStruct);

 /* GPIOG configuration */
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource6, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource7, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource10, 0x09);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource11, GPIO_AF_LTDC);
  GPIO_PinAFConfig(GPIOG, GPIO_PinSource12, 0x09);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | \
                             GPIO_Pin_11 | GPIO_Pin_12;

  GPIO_Init(GPIOG, &GPIO_InitStruct);
}

