
#include "main.h"



// STM32L433
// ARM®-based Cortex®-M7 32b MCU, (280 MHz max)
// Rom 128KB
// Ram 1024KB


u32 TimingDelay = 0;
u32 DCMI_FrameIsReady;

#define FrameWidth 160
#define FrameHeight 120
u16 pic[FrameWidth][FrameHeight];

LED<GPIOE_BASE, 3> lcLedRed;


cST7735 mcDisplay(cST7735::LANDSCAPE_ROT180);

#define DISPLAY_X 160
#define DISPLAY_Y  80

uint8 mDisplayMemory2[DISPLAY_X * DISPLAY_Y * 2];
uint8 mDisplayMemory[DISPLAY_X * DISPLAY_Y * 2];

//cDiffTimer                               mcDiffTimer;

cBitmap_Bpp16_5R6G5B                     mcBm(DISPLAY_X, DISPLAY_Y, mDisplayMemory);
cBitmap_Bpp16_5R6G5B                     mcBm2(DISPLAY_X, DISPLAY_Y, mDisplayMemory2);
cScreen_Bpp16_5R6G5B                     mcScreen1(&mcBm);

cSprite_Res8b_Bpp1_1G                    mc16GSprite;
cRes8b_Bpp1_1G_SpriteEngine_Bpp16_5R6G5B mc16GSpriteEng(Sprite_nModeOr);

cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_5x5Ucase(SPRTMST_FontLut_RFont01_05x05U_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_05x05U_1BPP_1G_Bmp, &mc16GSpriteEng);
cRFont_Res8b_Bpp1_1G                     cRFont_Res8b_Bpp1_1G_Full(SPRTMST_FontLut_RFont01_06x08_1BPP_1G_Bmp, SPRTMST_FontData_RFont01_06x08_1BPP_1G_Bmp, &mc16GSpriteEng);

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}


void Delay(__IO u32 nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  {
    TimingDelay--;
  }
}


void SysTick_Handler(void)
{
  TimingDelay_Decrement();
  HAL_IncTick();
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(u8 *file, u32 line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


class cCameraDefs
{
public:
  typedef enum
  {
     nOV7670_ADDRESS = 0x42,
     nOV2640_ADDRESS = 0x60,
     nOV7725_ADDRESS = 0x42,
     nOV5640_ADDRESS = 0x78
  } tenI2cAdress;

  typedef enum
  {
    nCamera_OK,
    nCamera_ERROR,
  } tenConsts;

  typedef enum 
  {
    nPIXFORMAT_INVALID = 0,
    nPIXFORMAT_RGB565,    // 2BPP/RGB565
    nPIXFORMAT_JPEG,      // JPEG/COMPRESSED
    nPIXFORMAT_YUV422,
    nPIXFORMAT_GRAYSCALE,
    nPIXFORMAT_BAYER,
  } tenPixFormat;

  typedef enum 
  {
    nFRAMESIZE_INVALID = 0,
    // C/SIF Resolutions
    nFRAMESIZE_QQCIF,    // 88x72 x
    nFRAMESIZE_QCIF,     // 176x144
    nFRAMESIZE_CIF,      // 352x288 x
    nFRAMESIZE_QQSIF,    // 88x60 x
    nFRAMESIZE_QSIF,     // 176x120
    nFRAMESIZE_SIF,      // 352x240 x

    // VGA Resolutions
    nFRAMESIZE_QQQQVGA,  // 40x30 x
    nFRAMESIZE_QQQVGA,   // 80x60 x
    nFRAMESIZE_QQVGA,    // 160x120 x
    nFRAMESIZE_QVGA,     // 320x240 *
    nFRAMESIZE_VGA,      // 640x480 *
    nFRAMESIZE_HQQQVGA,  // 60x40
    nFRAMESIZE_HQQVGA,   // 120x80 x
    nFRAMESIZE_HQVGA,    // 240x160
    nFRAMESIZE_HVGA,     // 480x320

    // FFT Resolutions
    nFRAMESIZE_64X32,    // 64x32 x
    nFRAMESIZE_64X64,    // 64x64 x
    nFRAMESIZE_128X64,   // 128x64 x
    nFRAMESIZE_128X128,  // 128x128

    // Other
    nFRAMESIZE_LCD,      // 128x160
    nFRAMESIZE_QQVGA2,   // 128x160
    nFRAMESIZE_WVGA,     // 720x480 x
    nFRAMESIZE_WVGA2,    // 752x480 x
    nFRAMESIZE_SVGA,     // 800x600 x
    nFRAMESIZE_XGA,      // 1024x768 x
    nFRAMESIZE_SXGA,     // 1280x1024
    nFRAMESIZE_UXGA,     // 1600x1200
    nFRAMESIZE_720P,     // 1280x720 *
    nFRAMESIZE_1080P,    // 1920x1080 *
    nFRAMESIZE_960P,     // 1280x960 *
    nFRAMESIZE_5MPP      // 2592x1944 *
  } tenFramesize;

  static const u16 Resolution[][2];
};

const u16 cCameraDefs::Resolution[][2] = 
{
  {0, 0},

  // C/SIF Resolutions
  { 88,  72},	/* QQCIF     */
  {176, 144}, /* QCIF      */
  {352, 288}, /* CIF       */
  { 88,  60},	/* QQSIF     */
  {176, 120}, /* QSIF      */
  {352, 240}, /* SIF       */

  // VGA Resolutions
  { 40,  30},	/* QQQQVGA   */
  { 80,  60},	/* QQQVGA    */
  {160, 120}, /* QQVGA     */
  {320, 240}, /* QVGA      */
  {640, 480}, /* VGA       */
  { 60,  40},	/* HQQQVGA   */
  {120,  80},	/* HQQVGA    */
  {240, 160}, /* HQVGA     */
  {480, 320}, /* HVGA      */

  // FFT Resolutions
  {64,   32},	 /* 64x32     */
  {64,   64},	 /* 64x64     */
  {128,  64},	 /* 128x64    */
  {128,  128}, /* 128x64    */

  // Other
  {128,  160},	/* LCD       */
  {128,  160},	/* QQVGA2    */
  {720,  480},	/* WVGA      */
  {752,  480},	/* WVGA2     */
  {800,  600},	/* SVGA      */
  {1024, 768},  /* XGA       */
  {1280, 1024}, /* SXGA      */
  {1600, 1200}, /* UXGA      */
  {1280, 720},  /* 720P      */
  {1920, 1080}, /* 1080P     */
  {1280, 960},  /* 960P      */
  {2592, 1944}, /* 5MP       */
};


class cCamera : public cCameraDefs
{
public:

  struct regval_t
  {
    u8 reg_addr;
    u8 value;
  };

  typedef struct 
  {
    I2C_HandleTypeDef* hi2c;
    u8 addr;
    u32 timeout;
    u16 manuf_id;
    u16 device_id;
    tenFramesize framesize;
    tenPixFormat pixformat;
  } Camera_HandleTypeDef;

  DCMI_HandleTypeDef hdcmi;
  I2C_HandleTypeDef hi2c1;
  DMA_HandleTypeDef hdma_dcmi_pssi;
  Camera_HandleTypeDef hcamera;

  cCamera(tenFramesize framesize)
  {
    vInitDma();
    vInitDcmi();
    vInitI2c();

    Camera_Init_Device(&hi2c1, framesize);
  }

  void vInitDma()
  {
    __HAL_RCC_DMA1_CLK_ENABLE();

    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  }

  void vInitDcmi()
  {
    hdcmi.Instance = DCMI;

    DCMI_HandleTypeDef* dcmiHandle = &hdcmi;
    GPIO_InitTypeDef GPIO_InitStruct;

    if (dcmiHandle->Instance == DCMI)
    {
      /* USER CODE BEGIN DCMI_MspInit 0 */

      /* USER CODE END DCMI_MspInit 0 */
        /* DCMI clock enable */
      __HAL_RCC_DCMI_CLK_ENABLE();

      __HAL_RCC_GPIOE_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();
      __HAL_RCC_GPIOC_CLK_ENABLE();
      __HAL_RCC_GPIOD_CLK_ENABLE();
      __HAL_RCC_GPIOB_CLK_ENABLE();
      /**DCMI GPIO Configuration
      PE4     ------> DCMI_D4
      PE5     ------> DCMI_D6
      PE6     ------> DCMI_D7
      PA4     ------> DCMI_HSYNC
      PA6     ------> DCMI_PIXCLK
      PC6     ------> DCMI_D0
      PC7     ------> DCMI_D1
      PD3     ------> DCMI_D5
      PB7     ------> DCMI_VSYNC
      PE0     ------> DCMI_D2
      PE1     ------> DCMI_D3
      */
      GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_0 | GPIO_PIN_1;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
      HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
      HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_3;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
      HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_7;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



      hdma_dcmi_pssi.Instance = DMA1_Stream0;
      hdma_dcmi_pssi.Init.Request = DMA_REQUEST_DCMI_PSSI;
      hdma_dcmi_pssi.Init.Direction = DMA_PERIPH_TO_MEMORY;
      hdma_dcmi_pssi.Init.PeriphInc = DMA_PINC_DISABLE;
      hdma_dcmi_pssi.Init.MemInc = DMA_MINC_ENABLE;
      hdma_dcmi_pssi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
      hdma_dcmi_pssi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
      hdma_dcmi_pssi.Init.Mode = DMA_CIRCULAR;
      hdma_dcmi_pssi.Init.Priority = DMA_PRIORITY_MEDIUM;
      hdma_dcmi_pssi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      HAL_DMA_Init(&hdma_dcmi_pssi);


      __HAL_LINKDMA(dcmiHandle, DMA_Handle, hdma_dcmi_pssi);

      HAL_NVIC_SetPriority(DCMI_PSSI_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DCMI_PSSI_IRQn);


      /*Configure GPIO pin : XCLK: PA8 : 12Mhz*/
      GPIO_InitStruct.Pin = GPIO_PIN_8;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }


    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
    hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
    hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
    hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
    hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
    hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
    HAL_DCMI_Init(&hdcmi);
  }

  void vInitI2c()
  {
    hi2c1.Instance = I2C1;

    I2C_HandleTypeDef* i2cHandle = &hi2c1;
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
    if (i2cHandle->Instance == I2C1)
    {
      /* USER CODE BEGIN I2C1_MspInit 0 */

      /* USER CODE END I2C1_MspInit 0 */

      /** Initializes the peripherals clock
      */
      PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
      PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
      HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

      __HAL_RCC_GPIOB_CLK_ENABLE();
      /**I2C1 GPIO Configuration
      PB8     ------> I2C1_SCL
      PB9     ------> I2C1_SDA
      */
      GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      /* I2C1 clock enable */
      __HAL_RCC_I2C1_CLK_ENABLE();
      /* USER CODE BEGIN I2C1_MspInit 1 */

      /* USER CODE END I2C1_MspInit 1 */
    }


    hi2c1.Init.Timing = 0x20B0CCFF;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);

    HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE);

    HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0);
  }

  int32_t Camera_WriteReg(Camera_HandleTypeDef* hov, u8 regAddr, const u8* pData)
  {
    u8 tt[2];
    tt[0] = regAddr;
    tt[1] = pData[0];
    if (HAL_I2C_Master_Transmit(hov->hi2c, hov->addr, tt, 2, hov->timeout) == HAL_OK)
    {
      return nCamera_OK;
    }
    else
    {
      return nCamera_ERROR;
    }
  }

  int32_t Camera_ReadReg(Camera_HandleTypeDef* hov, u8 regAddr, u8* pData)
  {
    HAL_I2C_Master_Transmit(hov->hi2c, hov->addr + 1, &regAddr, 1, hov->timeout);
    if (HAL_I2C_Master_Receive(hov->hi2c, hov->addr + 1, pData, 1, hov->timeout) == HAL_OK)
    {
      return nCamera_OK;
    }
    else
    {
      return nCamera_ERROR;
    }
  }

  int32_t Camera_WriteRegb2(Camera_HandleTypeDef* hov, u16 reg_addr, u8 reg_data)
  {
    if (HAL_I2C_Mem_Write(hov->hi2c, hov->addr + 1, reg_addr,
      I2C_MEMADD_SIZE_16BIT, &reg_data, 1, hov->timeout) == HAL_OK)
    {
      return nCamera_OK;
    }
    else
    {
      return nCamera_ERROR;
    }
  }

  int32_t Camera_ReadRegb2(Camera_HandleTypeDef* hov, u16 reg_addr, u8* reg_data)
  {
    if (HAL_I2C_Mem_Read(hov->hi2c, hov->addr + 1, reg_addr,
      I2C_MEMADD_SIZE_16BIT, reg_data, 1, hov->timeout) == HAL_OK)
    {
      return nCamera_OK;
    }
    else
    {
      return nCamera_ERROR;
    }
  }

  int32_t Camera_WriteRegList(Camera_HandleTypeDef* hov, const struct regval_t* reg_list)
  {
    const struct regval_t* pReg = reg_list;
    while (pReg->reg_addr != 0xFF && pReg->value != 0xFF)
    {
      int write_result = Camera_WriteReg(hov, pReg->reg_addr, &(pReg->value));
      if (write_result != nCamera_OK)
      {
        return write_result;
      }
      pReg++;
    }
    return nCamera_OK;
  }

  int32_t Camera_read_id(Camera_HandleTypeDef* hov)
  {
    u8 temp[2];
    temp[0] = 0x01;
    if (hov->addr != nOV5640_ADDRESS)
    {
      Camera_WriteReg(hov, 0xFF, temp);
      Camera_ReadReg(hov, 0x1C, &temp[0]);
      Camera_ReadReg(hov, 0x1D, &temp[1]);
      hov->manuf_id = ((u16)temp[0] << 8) | temp[1];
      Camera_ReadReg(hov, 0x0A, &temp[0]);
      Camera_ReadReg(hov, 0x0B, &temp[1]);
    }
    else
    {
      #define OV5640_CHIP_IDH 0x300A
      #define OV5640_CHIP_IDL 0x300B
      Camera_ReadRegb2(&hcamera, OV5640_CHIP_IDH, &temp[0]);
      Camera_ReadRegb2(&hcamera, OV5640_CHIP_IDL, &temp[1]);
      hov->manuf_id = 0;
    }
    hov->device_id = ((u16)temp[0] << 8) | temp[1];
    return 0;
  }

  void Camera_Reset(Camera_HandleTypeDef* hov)
  {
    u8 temp;
    temp = 0x01;
    Camera_WriteReg(hov, 0xFF, &temp);
    temp = 0x80;
    Camera_WriteReg(hov, 0x12, &temp);
    HAL_Delay(100);
  }

  void Camera_Init_Device(I2C_HandleTypeDef* hi2c, tenFramesize framesize)
  {
    hcamera.hi2c = hi2c;
    hcamera.addr = nOV7670_ADDRESS;
    hcamera.timeout = 100;

    Camera_read_id(&hcamera);
    if (hcamera.manuf_id == 0x7fa2 && hcamera.device_id == 0x7673)
    {
      //OV7670_Config();
    }
    else
    {
      hcamera.addr = nOV2640_ADDRESS;
      Camera_read_id(&hcamera);
      if (hcamera.manuf_id == 0x7fa2 && ((hcamera.device_id - 0x2641) <= 2))
      {
        //ov2640_init(framesize);
      }
      else
      {
        hcamera.addr = nOV7725_ADDRESS;
        Camera_read_id(&hcamera);
        if (hcamera.manuf_id == 0x7fa2 && ((hcamera.device_id - 0x7721) <= 2))
        {
          //ov7725_init(framesize);
        }
        else
        {

          hcamera.addr = nOV5640_ADDRESS;
          Camera_read_id(&hcamera);
          if (hcamera.device_id == 0x5640)
          {
            //ov5640_init(framesize);
          }
          else
          {
            hcamera.addr = 0;
            hcamera.device_id = 0;
          }
        }
      }
    }
  }
};


class cOV7725 : public cCamera
{
public:
  typedef enum
  {
    nGAIN                  =  0x00, /* AGC - Gain control gain setting  */
    nBLUE                  =  0x01, /* AWB - Blue channel gain setting  */
    nRED                   =  0x02, /* AWB - Red channel gain setting   */
    nGREEN                 =  0x03, /* AWB - Green channel gain setting */
    nBAVG                  =  0x05, /* U/B Average Level  */
    nGAVG                  =  0x06, /* Y/Gb Average Level */
    nRAVG                  =  0x07, /* V/R Average Level  */
    nAECH                  =  0x08, /* Exposure Value - AEC MSBs */

    nCOM2                  =  0x09, /* Common Control 2 */
    nCOM2_SOFT_SLEEP       =  0x10, /* Soft sleep mode  */
    nCOM2_OUT_DRIVE_1x     =  0x00, /* Output drive capability 1x */
    nCOM2_OUT_DRIVE_2x     =  0x01, /* Output drive capability 2x */
    nCOM2_OUT_DRIVE_3x     =  0x02, /* Output drive capability 3x */
    nCOM2_OUT_DRIVE_4x     =  0x03, /* Output drive capability 4x */

    nPID                   =  0x0A, /* Product ID Number MSB */
    nVER                   =  0x0B, /* Product ID Number LSB */

    nCOM3                  =  0x0C, /* Common Control 3                                       */
    nCOM3_VFLIP            =  0x80, /* Vertical flip image ON/OFF selection                   */
    nCOM3_MIRROR           =  0x40, /* Horizontal mirror image ON/OFF selection               */
    nCOM3_SWAP_BR          =  0x20, /* Swap B/R output sequence in RGB output mode            */
    nCOM3_SWAP_YUV         =  0x10, /* Swap Y/UV output sequence in YUV output mode           */
    nCOM3_SWAP_MSB         =  0x08, /* Swap output MSB/LSB                                    */
    nCOM3_TRI_CLOCK        =  0x04, /* Tri-state option for output clock at power-down period */
    nCOM3_TRI_DATA         =  0x02, /* Tri-state option for output data at power-down period  */
    nCOM3_COLOR_BAR        =  0x01, /* Sensor color bar test pattern output enable            */

    nCOM4                  =  0x0D, /* Common Control 4         */
    nCOM4_PLL_BYPASS       =  0x00, /* Bypass PLL               */
    nCOM4_PLL_4x           =  0x40, /* PLL frequency 4x         */
    nCOM4_PLL_6x           =  0x80, /* PLL frequency 6x         */
    nCOM4_PLL_8x           =  0xC0, /* PLL frequency 8x         */
    nCOM4_AEC_FULL         =  0x00, /* AEC evaluate full window */
    nCOM4_AEC_1_2          =  0x10, /* AEC evaluate 1/2 window  */
    nCOM4_AEC_1_4          =  0x20, /* AEC evaluate 1/4 window  */
    nCOM4_AEC_2_3          =  0x30, /* AEC evaluate 2/3 window  */

    nCOM5                  =  0x0E, /* Common Control 5 */
    nCOM5_AFR              =  0x80, /* Auto frame rate control ON/OFF selection (night mode) */
    nCOM5_AFR_SPEED        =  0x40, /* Auto frame rate control speed selection */
    nCOM5_AFR_0            =  0x00, /* No reduction of frame rate          */
    nCOM5_AFR_1_2          =  0x10, /* Max reduction to 1/2 frame rate     */
    nCOM5_AFR_1_4          =  0x20, /* Max reduction to 1/4 frame rate     */
    nCOM5_AFR_1_8          =  0x30, /* Max reduction to 1/8 frame rate     */
    nCOM5_AFR_4x           =  0x04, /* Add frame when AGC reaches 4x gain  */
    nCOM5_AFR_8x           =  0x08, /* Add frame when AGC reaches 8x gain  */
    nCOM5_AFR_16x          =  0x0c, /* Add frame when AGC reaches 16x gain */
    nCOM5_AEC_NO_LIMIT     =  0x01, /* No limit to AEC increase step       */

    nCOM6                  =  0x0F, /* Common Control 6 */
    nCOM6_AUTO_WINDOW      =  0x01, /* Auto window setting ON/OFF selection when format changes */

    nAEC                   =  0x10, /* AEC[7:0] (see register AECH for AEC[15:8]) */

    nCLKRC                 =  0x11, /* Internal Clock */
    nCLKRC_NO_PRESCALE     =  0x40, /* Use external clock directly */
    nCLKRC_PRESCALER       =  0x3F, /* Internal clock pre-scaler */

    nCOM7                  =  0x12, /* Common Control 7         */
    nCOM7_RESET            =  0x80, /* SCCB Register Reset      */
    nCOM7_RES_VGA          =  0x00, /* Resolution VGA           */
    nCOM7_RES_QVGA         =  0x40, /* Resolution QVGA          */
    nCOM7_BT656            =  0x20, /* BT.656 protocol ON/OFF   */
    nCOM7_SENSOR_RAW       =  0x10, /* Sensor RAW               */
    nCOM7_FMT_GBR422       =  0x00, /* RGB output format GBR422 */
    nCOM7_FMT_RGB565       =  0x04, /* RGB output format RGB565 */
    nCOM7_FMT_RGB555       =  0x08, /* RGB output format RGB555 */
    nCOM7_FMT_RGB444       =  0x0C, /* RGB output format RGB444 */
    nCOM7_FMT_YUV          =  0x00, /* Output format YUV        */
    nCOM7_FMT_P_BAYER      =  0x01, /* Output format Processed Bayer RAW */
    nCOM7_FMT_RGB          =  0x02, /* Output format RGB        */
    nCOM7_FMT_R_BAYER      =  0x03, /* Output format Bayer RAW  */

    nCOM8                  =  0x13, /* Common Control 8              */
    nCOM8_FAST_AUTO        =  0x80, /* Enable fast AGC/AEC algorithm */
    nCOM8_STEP_VSYNC       =  0x00, /* AEC - Step size limited to vertical blank */
    nCOM8_STEP_UNLIMIT     =  0x40, /* AEC - Step size unlimited step size       */
    nCOM8_BANDF_EN         =  0x20, /* Banding filter ON/OFF */
    nCOM8_AEC_BANDF        =  0x10, /* Enable AEC below banding value */
    nCOM8_AEC_FINE_EN      =  0x08, /* Fine AEC ON/OFF control */
    nCOM8_AGC_EN           =  0x04, /* AGC Enable */
    nCOM8_AWB_EN           =  0x02, /* AWB Enable */
    nCOM8_AEC_EN           =  0x01, /* AEC Enable */

    nCOM9                  =  0x14, /* Common Control 9 */
    nCOM9_HISTO_AVG        =  0x80, /* Histogram or average based AEC/AGC selection */
    nCOM9_AGC_GAIN_2x      =  0x00, /* Automatic Gain Ceiling 2x  */
    nCOM9_AGC_GAIN_4x      =  0x10, /* Automatic Gain Ceiling 4x  */
    nCOM9_AGC_GAIN_8x      =  0x20, /* Automatic Gain Ceiling 8x  */
    nCOM9_AGC_GAIN_16x     =  0x30, /* Automatic Gain Ceiling 16x */
    nCOM9_AGC_GAIN_32x     =  0x40, /* Automatic Gain Ceiling 32x */
    nCOM9_DROP_VSYNC       =  0x04, /* Drop VSYNC output of corrupt frame */
    nCOM9_DROP_HREF        =  0x02, /* Drop HREF output of corrupt frame  */

    nCOM10                 =  0x15, /* Common Control 10 */
    nCOM10_NEGATIVE        =  0x80, /* Output negative data */
    nCOM10_HSYNC_EN        =  0x40, /* HREF changes to HSYNC */
    nCOM10_PCLK_FREE       =  0x00, /* PCLK output option: free running PCLK */
    nCOM10_PCLK_MASK       =  0x20, /* PCLK output option: masked during horizontal blank  */
    nCOM10_PCLK_REV        =  0x10, /* PCLK reverse */
    nCOM10_HREF_REV        =  0x08, /* HREF reverse */
    nCOM10_VSYNC_FALLING   =  0x00, /* VSYNC changes on falling edge of PCLK */
    nCOM10_VSYNC_RISING    =  0x04, /* VSYNC changes on rising edge of PCLK */
    nCOM10_VSYNC_NEG       =  0x02, /* VSYNC negative */
    nCOM10_OUT_RANGE_8     =  0x01, /* Output data range: Full range */
    nCOM10_OUT_RANGE_10    =  0x00, /* Output data range: Data from [10] to [F0] (8 MSBs) */

    nREG16                 =  0x16, /* Register 16 */
    nREG16_BIT_SHIFT       =  0x80, /* Bit shift test pattern options */

    nHSTART                =  0x17, /* Horizontal Frame (HREF column) Start 8 MSBs (2 LSBs are at HREF[5:4]) */
    nHSIZE                 =  0x18, /* Horizontal Sensor Size (2 LSBs are at HREF[1:0]) */
    nVSTART                =  0x19, /* Vertical Frame (row) Start 8 MSBs (1 LSB is at HREF[6]) */
    nVSIZE                 =  0x1A, /* Vertical Sensor Size (1 LSB is at HREF[2]) */
    nPSHFT                 =  0x1B, /* Data Format - Pixel Delay Select */
    nMIDH                  =  0x1C, /* Manufacturer ID Byte - High */
    nMIDL                  =  0x1D, /* Manufacturer ID Byte - Low */
    nLAEC                  =  0x1F, /* Fine AEC Value - defines exposure value less than one row period */

    nCOM11                 =  0x20, /* Common Control 11 */
    nCOM11_SNGL_FRAME_EN   =  0x02, /* Single frame ON/OFF selection */
    nCOM11_SNGL_XFR_TRIG   =  0x01, /* Single frame transfer trigger */

    nBDBASE                =  0x22, /* Banding Filter Minimum AEC Value */
    nBDSTEP                =  0x23, /* Banding Filter Maximum Step */
    nAEW                   =  0x24, /* AGC/AEC - Stable Operating Region (Upper Limit) */
    nAEB                   =  0x25, /* AGC/AEC - Stable Operating Region (Lower Limit) */
    nVPT                   =  0x26, /* AGC/AEC Fast Mode Operating Region */
    nREG28                 =  0x28, /* Selection on the number of dummy rows, N */
    nHOUTSIZE              =  0x29, /* Horizontal Data Output Size MSBs (2 LSBs at register EXHCH[1:0]) */
    nEXHCH                 =  0x2A, /* Dummy Pixel Insert MSB */
    nEXHCL                 =  0x2B, /* Dummy Pixel Insert LSB */
    nVOUTSIZE              =  0x2C, /* Vertical Data Output Size MSBs (LSB at register EXHCH[2])      */
    nADVFL                 =  0x2D, /* LSB of Insert Dummy Rows in Vertical Sync (1 bit equals 1 row) */
    nADVFH                 =  0x2E, /* MSB of Insert Dummy Rows in Vertical Sync */
    nYAVE                  =  0x2F, /* Y/G Channel Average Value */
    nLUMHTH                =  0x30, /* Histogram AEC/AGC Luminance High Level Threshold */
    nLUMLTH                =  0x31, /* Histogram AEC/AGC Luminance Low Level Threshold  */
    nHREF                  =  0x32, /* Image Start and Size Control */
    nDM_LNL                =  0x33, /* Dummy Row Low 8 Bits  */
    nDM_LNH                =  0x34, /* Dummy Row High 8 Bits */
    nADOFF_B               =  0x35, /* AD Offset Compensation Value for B Channel  */
    nADOFF_R               =  0x36, /* AD Offset Compensation Value for R Channel  */
    nADOFF_GB              =  0x37, /* AD Offset Compensation Value for GB Channel */
    nADOFF_GR              =  0x38, /* AD Offset Compensation Value for GR Channel */
    nOFF_B                 =  0x39, /* AD Offset Compensation Value for B Channel  */
    nOFF_R                 =  0x3A, /* AD Offset Compensation Value for R Channel  */
    nOFF_GB                =  0x3B, /* AD Offset Compensation Value for GB Channel */
    nOFF_GR                =  0x3C, /* AD Offset Compensation Value for GR Channel */

    nCOM12                 =  0x3D, /* DC offset compensation for analog process */

    nCOM13                 =  0x3E, /* Common Control 13 */
    nCOM13_BLC_EN          =  0x80, /* BLC enable */
    nCOM13_ADC_EN          =  0x40, /* ADC channel BLC ON/OFF control */
    nCOM13_ANALOG_BLC      =  0x20, /* Analog processing channel BLC ON/OFF control */
    nCOM13_ABLC_GAIN_EN    =  0x04, /* ABLC gain trigger enable */

    nCOM14                 =  0x3F, /* Common Control 14 */
    nCOM15                 =  0x40, /* Common Control 15 */
    nCOM16                 =  0x41, /* Common Control 16 */
    nTGT_B                 =  0x42, /* BLC Blue Channel Target Value */
    nTGT_R                 =  0x43, /* BLC Red Channel Target Value  */
    nTGT_GB                =  0x44, /* BLC Gb Channel Target Value   */
    nTGT_GR                =  0x45, /* BLC Gr Channel Target Value   */

    nLC_CTR                =  0x46, /* Lens Correction Control */
    nLC_CTR_RGB_COMP_1     =  0x00, /* R, G, and B channel compensation coefficient is set by LC_COEF (0x49) */
    nLC_CTR_RGB_COMP_3     =  0x04, /* R, G, and B channel compensation coefficient is set by registers
                                       LC_COEFB (0x4B), LC_COEF (0x49), and LC_COEFR (0x4C), respectively */
    nLC_CTR_EN             =  0x01, /* Lens correction enable */
    nLC_XC                 =  0x47, /* X Coordinate of Lens Correction Center Relative to Array Center */
    nLC_YC                 =  0x48, /* Y Coordinate of Lens Correction Center Relative to Array Center */
    nLC_COEF               =  0x49, /* Lens Correction Coefficient */
    nLC_RADI               =  0x4A, /* Lens Correction Radius */
    nLC_COEFB              =  0x4B, /* Lens Correction B Channel Compensation Coefficient */
    nLC_COEFR              =  0x4C, /* Lens Correction R Channel Compensation Coefficient */

    nFIXGAIN               =  0x4D, /* Analog Fix Gain Amplifier */
    nAREF0                 =  0x4E, /* Sensor Reference Control */
    nAREF1                 =  0x4F, /* Sensor Reference Current Control */
    nAREF2                 =  0x50, /* Analog Reference Control */
    nAREF3                 =  0x51, /* ADC Reference Control */
    nAREF4                 =  0x52, /* ADC Reference Control */
    nAREF5                 =  0x53, /* ADC Reference Control */
    nAREF6                 =  0x54, /* Analog Reference Control */
    nAREF7                 =  0x55, /* Analog Reference Control */
    nUFIX                  =  0x60, /* U Channel Fixed Value Output */
    nVFIX                  =  0x61, /* V Channel Fixed Value Output */
    nAWBB_BLK              =  0x62, /* AWB Option for Advanced AWB  */

    nAWB_CTRL0             =  0x63, /* AWB Control Byte 0   */
    nAWB_CTRL0_GAIN_EN     =  0x80, /* AWB gain enable      */
    nAWB_CTRL0_CALC_EN     =  0x40, /* AWB calculate enable */
    nAWB_CTRL0_WBC_MASK    =  0x0F, /* WBC threshold 2      */

    nDSP_CTRL1             =  0x64, /* DSP Control Byte 1                  */
    nDSP_CTRL1_FIFO_EN     =  0x80, /* FIFO enable/disable selection       */
    nDSP_CTRL1_UV_EN       =  0x40, /* UV adjust function ON/OFF selection */
    nDSP_CTRL1_SDE_EN      =  0x20, /* SDE enable                          */
    nDSP_CTRL1_MTRX_EN     =  0x10, /* Color matrix ON/OFF selection       */
    nDSP_CTRL1_INTRP_EN    =  0x08, /* Interpolation ON/OFF selection      */
    nDSP_CTRL1_GAMMA_EN    =  0x04, /* Gamma function ON/OFF selection     */
    nDSP_CTRL1_BLACK_EN    =  0x02, /* Black defect auto correction ON/OFF */
    nDSP_CTRL1_WHITE_EN    =  0x01, /* White defect auto correction ON/OFF */

    nDSP_CTRL2             =  0x65, /* DSP Control Byte 2         */
    nDSP_CTRL2_VDCW_EN     =  0x08, /* Vertical DCW enable        */
    nDSP_CTRL2_HDCW_EN     =  0x04, /* Horizontal DCW enable      */
    nDSP_CTRL2_VZOOM_EN    =  0x02, /* Vertical zoom out enable   */
    nDSP_CTRL2_HZOOM_EN    =  0x01, /* Horizontal zoom out enable */

    nDSP_CTRL3             =  0x66, /* DSP Control Byte 3                      */
    nDSP_CTRL3_UV_EN       =  0x80, /* UV output sequence option               */
    nDSP_CTRL3_CBAR_EN     =  0x20, /* DSP color bar ON/OFF selection          */
    nDSP_CTRL3_FIFO_EN     =  0x08, /* FIFO power down ON/OFF selection        */
    nDSP_CTRL3_SCAL1_PWDN  =  0x04, /* Scaling module power down control 1     */
    nDSP_CTRL3_SCAL2_PWDN  =  0x02, /* Scaling module power down control 2     */
    nDSP_CTRL3_INTRP_PWDN  =  0x01, /* Interpolation module power down control */

    nDSP_CTRL4             =  0x67, /* DSP Control Byte 4          */
    nDSP_CTRL4_YUV_RGB     =  0x00, /* Output selection YUV or RGB */
    nDSP_CTRL4_RAW8        =  0x02, /* Output selection RAW8       */
    nDSP_CTRL4_RAW10       =  0x03, /* Output selection RAW10      */

    nAWB_BIAS              =  0x68, /* AWB BLC Level Clip */
    nAWB_CTRL1             =  0x69, /* AWB Control 1 */
    nAWB_CTRL2             =  0x6A, /* AWB Control 2 */

    nAWB_CTRL3             =  0x6B, /* AWB Control 3 */
    nAWB_CTRL3_ADVANCED    =  0x80, /* AWB mode select - Advanced AWB */
    nAWB_CTRL3_SIMPLE      =  0x00, /* AWB mode select - Simple AWB */

    nAWB_CTRL4             =  0x6C, /* AWB Control 4  */
    nAWB_CTRL5             =  0x6D, /* AWB Control 5  */
    nAWB_CTRL6             =  0x6E, /* AWB Control 6  */
    nAWB_CTRL7             =  0x6F, /* AWB Control 7  */
    nAWB_CTRL8             =  0x70, /* AWB Control 8  */
    nAWB_CTRL9             =  0x71, /* AWB Control 9  */
    nAWB_CTRL10            =  0x72, /* AWB Control 10 */
    nAWB_CTRL11            =  0x73, /* AWB Control 11 */
    nAWB_CTRL12            =  0x74, /* AWB Control 12 */
    nAWB_CTRL13            =  0x75, /* AWB Control 13 */
    nAWB_CTRL14            =  0x76, /* AWB Control 14 */
    nAWB_CTRL15            =  0x77, /* AWB Control 15 */
    nAWB_CTRL16            =  0x78, /* AWB Control 16 */
    nAWB_CTRL17            =  0x79, /* AWB Control 17 */
    nAWB_CTRL18            =  0x7A, /* AWB Control 18 */
    nAWB_CTRL19            =  0x7B, /* AWB Control 19 */
    nAWB_CTRL20            =  0x7C, /* AWB Control 20 */
    nAWB_CTRL21            =  0x7D, /* AWB Control 21 */
    nGAM1                  =  0x7E, /* Gamma Curve 1st Segment Input End Point 0x04 Output Value */
    nGAM2                  =  0x7F, /* Gamma Curve 2nd Segment Input End Point 0x08 Output Value */
    nGAM3                  =  0x80, /* Gamma Curve 3rd Segment Input End Point 0x10 Output Value */
    nGAM4                  =  0x81, /* Gamma Curve 4th Segment Input End Point 0x20 Output Value */
    nGAM5                  =  0x82, /* Gamma Curve 5th Segment Input End Point 0x28 Output Value */
    nGAM6                  =  0x83, /* Gamma Curve 6th Segment Input End Point 0x30 Output Value */
    nGAM7                  =  0x84, /* Gamma Curve 7th Segment Input End Point 0x38 Output Value */
    nGAM8                  =  0x85, /* Gamma Curve 8th Segment Input End Point 0x40 Output Value */
    nGAM9                  =  0x86, /* Gamma Curve 9th Segment Input Enpd Point 0x48 Output Value */
    nGAM10                 =  0x87, /* Gamma Curve 10th Segment Input End Point 0x50 Output Value */
    nGAM11                 =  0x88, /* Gamma Curve 11th Segment Input End Point 0x60 Output Value */
    nGAM12                 =  0x89, /* Gamma Curve 12th Segment Input End Point 0x70 Output Value */
    nGAM13                 =  0x8A, /* Gamma Curve 13th Segment Input End Point 0x90 Output Value */
    nGAM14                 =  0x8B, /* Gamma Curve 14th Segment Input End Point 0xB0 Output Value */
    nGAM15                 =  0x8C, /* Gamma Curve 15th Segment Input End Point 0xD0 Output Value */
    nSLOP                  =  0x8D, /* Gamma Curve Highest Segment Slope */
    nDNSTH                 =  0x8E, /* De-noise Threshold */
    nEDGE0                 =  0x8F, /* Edge Enhancement Strength Control */
    nEDGE1                 =  0x90, /* Edge Enhancement Threshold Control */
    nDNSOFF                =  0x91, /* Auto De-noise Threshold Control */
    nEDGE2                 =  0x92, /* Edge Enhancement Strength Upper Limit */
    nEDGE3                 =  0x93, /* Edge Enhancement Strength Upper Limit */
    nMTX1                  =  0x94, /* Matrix Coefficient 1 */
    nMTX2                  =  0x95, /* Matrix Coefficient 2 */
    nMTX3                  =  0x96, /* Matrix Coefficient 3 */
    nMTX4                  =  0x97, /* Matrix Coefficient 4 */
    nMTX5                  =  0x98, /* Matrix Coefficient 5 */
    nMTX6                  =  0x99, /* Matrix Coefficient 6 */

    nMTX_CTRL              =  0x9A, /* Matrix Control */
    nMTX_CTRL_DBL_EN       =  0x80, /* Matrix double ON/OFF selection */

    nBRIGHTNESS            =  0x9B, /* Brightness Control */
    nCONTRAST              =  0x9C, /* Contrast Gain */
    nUVADJ0                =  0x9E, /* Auto UV Adjust Control 0 */
    nUVADJ1                =  0x9F, /* Auto UV Adjust Control 1 */
    nSCAL0                 =  0xA0, /* DCW Ratio Control */
    nSCAL1                 =  0xA1, /* Horizontal Zoom Out Control */
    nSCAL2                 =  0xA2, /* Vertical Zoom Out Control */
    nFIFODLYM              =  0xA3, /* FIFO Manual Mode Delay Control */
    nFIFODLYA              =  0xA4, /* FIFO Auto Mode Delay Control */

    nSDE                   =  0xA6, /* Special Digital Effect Control */
    nSDE_NEGATIVE_EN       =  0x40, /* Negative image enable          */
    nSDE_GRAYSCALE_EN      =  0x20, /* Gray scale image enable        */
    nSDE_V_FIXED_EN        =  0x10, /* V fixed value enable           */
    nSDE_U_FIXED_EN        =  0x08, /* U fixed value enable           */
    nSDE_CONT_BRIGHT_EN    =  0x04, /* Contrast/Brightness enable     */
    nSDE_SATURATION_EN     =  0x02, /* Saturation enable              */
    nSDE_HUE_EN            =  0x01, /* Hue enable                     */

    nUSAT                  =  0xA7, /* U Component Saturation Gain     */
    nVSAT                  =  0xA8, /* V Component Saturation Gain     */
    nHUECOS                =  0xA9, /* Cosine value ¡Á 0x80             */
    nHUESIN                =  0xAA, /* Sine value ¡Á 0x80               */
    nSIGN_BIT              =  0xAB, /* Sign Bit for Hue and Brightness */

    nDSPAUTO               =  0xAC, /* DSP Auto Function ON/OFF Control */
    nDSPAUTO_AWB_EN        =  0x80, /* AWB auto threshold control */
    nDSPAUTO_DENOISE_EN    =  0x40, /* De-noise auto threshold control */
    nDSPAUTO_EDGE_EN       =  0x20, /* Sharpness (edge enhancement) auto strength control */
    nDSPAUTO_UV_EN         =  0x10, /* UV adjust auto slope control */
    nDSPAUTO_SCAL0_EN      =  0x08, /* Auto scaling factor control (register SCAL0 (0xA0)) */
    nDSPAUTO_SCAL1_EN      =  0x04, /* Auto scaling factor control (registers SCAL1 (0xA1 and SCAL2 (0xA2)) */
  }tenRegs;


      u8 COM3_SET_CBAR(u8 r, u8 x)   {return ((r & 0xFE) | ((x & 1) << 0));}
      u8 COM3_SET_MIRROR(u8 r, u8 x) {return ((r & 0xBF) | ((x & 1) << 6));}
      u8 COM3_SET_FLIP(u8 r, u8 x)   {return ((r & 0x7F) | ((x & 1) << 7));}

      u8 COM3_GET_CBAR(u8 r)   {return ((r >> 0) & 1);}
      u8 COM3_GET_MIRROR(u8 r) {return ((r >> 6) & 1);}
      u8 COM3_GET_FLIP(u8 r)   {return ((r >> 7) & 1);}
      
      u8 COM4_GET_PLL(u8 r)    { return (r & 0xC0); }

      u8 COM7_SET_FMT(u8 r, u8 x) {return ((r & 0xFC) | ((x & 0x3) << 0)); }
      u8 COM7_SET_RES(u8 r, u8 x) {return ((r & 0xBF) | (x)); }
      u8 COM7_GET_FMT(u8 r)       {return (r & 0x03); }

      u8 COM8_SET_AGC(u8 r, u8 x) {return ((r & 0xFB) | ((x & 0x1) << 2)); }
      u8 COM8_SET_AWB(u8 r, u8 x) {return ((r & 0xFD) | ((x & 0x1) << 1)); }
      u8 COM8_SET_AEC(u8 r, u8 x) {return ((r & 0x7E) | ((x & 0x1) << 7) | ((x & 0x1) << 0)); }

      u8 COM9_SET_AGC(u8 r, u8 x)       {return ((r & 0x8F) | ((x & 0x07) << 4)); }
      u8 DSP_CTRL3_SET_CBAR(u8 r, u8 x) { return  ((r & 0xDF) | ((x & 1) << 5)); }


  static const u8 ov7725_default_regs[][2];

  u8 ov7725_WR_Reg(u8 reg, u8 data)
  {
    Camera_WriteReg(&hcamera, reg, &data);
    return 0;
  }

  u8 ov7725_RD_Reg(u8 reg, u8* data)
  {
    return Camera_ReadReg(&hcamera, reg, data);
  }

  int ov7725_reset(void)
  {
    // Reset all registers
    int ret = ov7725_WR_Reg(nCOM7, nCOM7_RESET);

    // Delay 2 ms
    cClockInfo::Delay_ms(2);

    // Write default regsiters
    for (int i = 0; ov7725_default_regs[i][0]; i++) 
    {
      ret |= ov7725_WR_Reg(ov7725_default_regs[i][0], ov7725_default_regs[i][1]);
    }

    // Delay 300 ms
    cClockInfo::Delay_ms(300);

    return ret;
  }

  int ov7725_set_pixformat(tenPixFormat pixformat)
  {
    u8 reg;
    int ret = ov7725_RD_Reg(nCOM7, &reg);

    switch (pixformat) 
    {
      case nPIXFORMAT_RGB565:
        reg = COM7_SET_FMT(reg, nCOM7_FMT_RGB);
        ret |= ov7725_WR_Reg(nDSP_CTRL4, nDSP_CTRL4_YUV_RGB);
        break;
      case nPIXFORMAT_YUV422:
      case nPIXFORMAT_GRAYSCALE:
        reg = COM7_SET_FMT(reg, nCOM7_FMT_YUV);
        ret |= ov7725_WR_Reg(nDSP_CTRL4, nDSP_CTRL4_YUV_RGB);
        break;
      case nPIXFORMAT_BAYER:
        reg = COM7_SET_FMT(reg, nCOM7_FMT_P_BAYER);
        ret |= ov7725_WR_Reg(nDSP_CTRL4, nDSP_CTRL4_RAW8);
        break;
      default:
        return -1;
    }

    // Write back register
    return ov7725_WR_Reg(nCOM7, reg) | ret;
  }

  int ov7725_set_framesize(tenFramesize framesize)
  {
    u8 reg;
    int ret = 0;
    u16 w = Resolution[framesize][0];
    u16 h = Resolution[framesize][1];
    bool vflip;

    if ((w > 640) || (h > 480)) 
    {
      return -1;
    }

    // Write MSBs
    ret |= ov7725_WR_Reg(nHOUTSIZE, w >> 2);
    ret |= ov7725_WR_Reg(nVOUTSIZE, h >> 1);

    // Write LSBs
    ret |= ov7725_WR_Reg(nEXHCH, ((w & 0x3) | ((h & 0x1) << 2)));

    // Sample VFLIP
    ret |= ov7725_RD_Reg(nCOM3, &reg);
    vflip = reg & nCOM3_VFLIP;
    ret |= ov7725_RD_Reg(nHREF, &reg);
    ret |= ov7725_WR_Reg(nHREF, (reg & 0xBF) | (vflip ? 0x40 : 0x00));

    if ((w <= 320) && (h <= 240)) 
    {
      // Set QVGA Resolution
      u8 reg;
      int ret = ov7725_RD_Reg(nCOM7, &reg);
      reg = COM7_SET_RES(reg, nCOM7_RES_QVGA);
      ret |= ov7725_WR_Reg(nCOM7, reg);

      // Set QVGA Window Size
      ret |= ov7725_WR_Reg(nHSTART, 0x3F);
      ret |= ov7725_WR_Reg(nHSIZE, 0x50);
      ret |= ov7725_WR_Reg(nVSTART, 0x03 - vflip);
      ret |= ov7725_WR_Reg(nVSIZE, 0x78);

      // Enable auto-scaling/zooming factors
      ret |= ov7725_WR_Reg(nDSPAUTO, 0xFF);
    }
    else 
    {
      // Set VGA Resolution
      u8 reg;
      int ret = ov7725_RD_Reg(nCOM7, &reg);
      reg = COM7_SET_RES(reg, nCOM7_RES_VGA);
      ret |= ov7725_WR_Reg(nCOM7, reg);

      // Set VGA Window Size
      ret |= ov7725_WR_Reg(nHSTART, 0x23);
      ret |= ov7725_WR_Reg(nHSIZE, 0xA0);
      ret |= ov7725_WR_Reg(nVSTART, 0x07 - vflip);
      ret |= ov7725_WR_Reg(nVSIZE, 0xF0);

      // Disable auto-scaling/zooming factors
      ret |= ov7725_WR_Reg(nDSPAUTO, 0xF3);

      // Clear auto-scaling/zooming factors
      ret |= ov7725_WR_Reg(nSCAL0, 0x00);
      ret |= ov7725_WR_Reg(nSCAL1, 0x40);
      ret |= ov7725_WR_Reg(nSCAL2, 0x40);
    }

    return ret;
  }

  int ov7725_set_hmirror(int enable)
  {
    u8 reg;
    int ret = ov7725_RD_Reg(nCOM3, &reg);
    ret |= ov7725_WR_Reg(nCOM3, COM3_SET_MIRROR(reg, enable));

    return ret;
  }

  int ov7725_set_vflip(int enable)
  {
    u8 reg;
    int ret = ov7725_RD_Reg(nCOM3, &reg);
    ret |= ov7725_WR_Reg(nCOM3, COM3_SET_FLIP(reg, enable));
    // Apply new vertical flip setting.
    ret |= ov7725_set_framesize(hcamera.framesize);

    return ret;
  }

  int ov7725_init(tenFramesize framesize)
  {
    ov7725_reset();
    hcamera.framesize = framesize;
    hcamera.pixformat = nPIXFORMAT_RGB565;
    ov7725_set_pixformat(hcamera.pixformat);
    ov7725_set_framesize(hcamera.framesize);
    ov7725_set_hmirror(1);
    ov7725_set_vflip(1);

    return 1;
  }
};

const u8 cOV7725::ov7725_default_regs[][2] = {

  // From App Note.

      {nCOM12,         0x03},
      {nHSTART,        0x22},
      {nHSIZE,         0xa4},
      {nVSTART,        0x07},
      {nVSIZE,         0xf0},
      {nHREF,          0x00},
      {nHOUTSIZE,      0xa0},
      {nVOUTSIZE,      0xf0},
      {nEXHCH,         0x00},
      {nCLKRC,         0x81}, // {CLKRC, 0xc0}, 12Mhz Input / div 2 = 6Mhz

      {nTGT_B,         0x7f},
      {nFIXGAIN,       0x09},
      {nAWB_CTRL0,     0xe0},
      {nDSP_CTRL1,     0xff},
      {nDSP_CTRL2,     0x20 | nDSP_CTRL2_VDCW_EN | nDSP_CTRL2_HDCW_EN | nDSP_CTRL2_VZOOM_EN | nDSP_CTRL2_HZOOM_EN}, // {DSP_CTRL2, 0x20},
      {nDSP_CTRL3,     0x00},
      {nDSP_CTRL4,     0x48},

      {nCOM10,         nCOM10_VSYNC_NEG},
      {nCOM8,          0xf0},
      {nCOM4,          nCOM4_PLL_4x}, // 6Mhz x 6 = 36Mhz, Max FPS: 113FPS QQVGA
      {nCOM6,          0xc5},
      {nCOM9,          0x11},
      {nBDBASE,        0x7f},
      {nBDSTEP,        0x03},
      {nAEW,           0x40},
      {nAEB,           0x30},
      {nVPT,           0xa1},
      {nEXHCL,         0x00},
      {nAWB_CTRL3,     0xaa},
      {nCOM8,          0xff},

      {nEDGE1,         0x05},
      {nDNSOFF,        0x01},
      {nEDGE2,         0x03},
      {nEDGE3,         0x00},
      {nMTX1,          0xb0},
      {nMTX2,          0x9d},
      {nMTX3,          0x13},
      {nMTX4,          0x16},
      {nMTX5,          0x7b},
      {nMTX6,          0x91},
      {nMTX_CTRL,      0x1e},
      {nBRIGHTNESS,    0x08},
      {nCONTRAST,      0x20},
      {nUVADJ0,        0x81},
      {nSDE,           nSDE_CONT_BRIGHT_EN | nSDE_SATURATION_EN},

      {nGAM1,          0x0c},
      {nGAM2,          0x16},
      {nGAM3,          0x2a},
      {nGAM4,          0x4e},
      {nGAM5,          0x61},
      {nGAM6,          0x6f},
      {nGAM7,          0x7b},
      {nGAM8,          0x86},
      {nGAM9,          0x8e},
      {nGAM10,         0x97},
      {nGAM11,         0xa4},
      {nGAM12,         0xaf},
      {nGAM13,         0xc5},
      {nGAM14,         0xd7},
      {nGAM15,         0xe8},
      {nSLOP,          0x20},

      {nDM_LNL,        0x00},
      {nBDBASE,        0x7f}, // {BDBASE, 0x7f}
      {nBDSTEP,        0x03},

      {nLC_RADI,       0x10},
      {nLC_COEF,       0x10},
      {nLC_COEFB,      0x14},
      {nLC_COEFR,      0x17},
      {nLC_CTR,        0x01}, // {LC_CTR, 0x05},

      {nCOM5,          0xf5}, // {COM5, 0x65},

  // OpenMV Custom.

      {nCOM7,          nCOM7_FMT_RGB565},

      // End.

      {0x00,          0x00},
};



cCamera mcCamera(cCamera::nFRAMESIZE_QQVGA);


void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef* hdcmi)
{
  DCMI_FrameIsReady = 1;
}

void DMA1_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&mcCamera.hdma_dcmi_pssi);
}


void DCMI_PSSI_IRQHandler(void)
{
  HAL_DCMI_IRQHandler(&mcCamera.hdcmi);
}


void SPI4_IRQHandler(void)
{
  mcDisplay.vEvTc();
}


void MAIN_vTick1000msLp(void)
{
  static u16 lu16_5s = 0;
  char lszValue[16] = "";

  lu16_5s++;

  lcLedRed.Toggle();

  if (mcDisplay.isReady())
  {
    mcScreen1.vFill(BM_BPP32_8R8G8B_GREY2);

    cRFont_Res8b_Bpp1_1G_Full.mpcSpriteEng->menMode = Sprite_nModeCopy;

    cRFont_Res8b_Bpp1_1G_Full.mui32Col = mcScreen1.u32GetCol(0xFFFFFF);
    cStrTools::uixItoa(lu16_5s, lszValue, 10);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(40, 40, lszValue, &mcScreen1);

    
    cStrTools::uixItoa(mcCamera.hcamera.device_id, lszValue, 10);
    cRFont_Res8b_Bpp1_1G_Full.i8PutStringXY(40, 50, lszValue, &mcScreen1);

    cPaint::vRectFull( 0, 0, 20, 20, 0x0000FF, &mcScreen1);
    cPaint::vRectFull(20, 0, 20, 20, 0x00FF00, &mcScreen1);
    cPaint::vRectFull(40, 0, 20, 20, 0xFF0000, &mcScreen1);
    mcScreen1.vLine(0,             0, DISPLAY_X - 1, DISPLAY_Y - 1, 0xFFFFFF);
    mcScreen1.vLine(0, DISPLAY_Y - 1, DISPLAY_X - 1, 0,             0x00FF00);

    cPaint::vRect(0, 0, 160, 80, 0xFF0000, &mcScreen1);

    //mcDisplay.vShowScreen(mcScreen1.mpcBm->mpui8Data);
    mcDisplay.vShowScreenDma(mcScreen1.mpcBm->mpui8Data);
  }
}


void MAIN_vInitSystem(void)
{
  cClockInfo::Update();
  SysTick_Config(cClockInfo::mstClocks.HCLK_Frequency / 100);

  /* STM32L4xx HAL library initialization:
       - Configure the Flash prefetch
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  HAL_Init();

  CycCall_Start(NULL /*1ms_HP*/,
                NULL /*10ms_HP*/,
                NULL /*100ms_HP*/,
                NULL /*1s_HP*/,

                NULL               /*1ms_LP*/,
                NULL               /*10ms_LP*/,
                NULL               /*100ms_LP*/,
                MAIN_vTick1000msLp /*1s_LP*/);

  HAL_DCMI_Start_DMA(&mcCamera.hdcmi, DCMI_MODE_CONTINUOUS, (u32)&pic, FrameWidth * FrameHeight * 2 / 4);

}


int main(void)
{
  MAIN_vInitSystem();

  while (1)
  {
    CycCall_vIdle();
    //__asm("wfi");
  }
}


void SysError_Handler()
{
  while (1)
  {};
}

void SystemClock_Config_HSI_64MHZ(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    SysError_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                                       |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                                       |RCC_PERIPHCLK_I2C3;
  PeriphClkInit.Usart16ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart234578ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c123ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    SysError_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    SysError_Handler();
  }
}


void SystemClock_Config_HSE_240Mhz(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  /*AXI clock gating */
  RCC->CKGAENR = 0xFFFFFFFF;

  /** Supply configuration update enable
   */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  /** Configure LSE Drive Capability
   */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 112;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    //Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    //Error_Handler();
  }

  // 48Mhz / 4 = 12Mhz
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI48, RCC_MCODIV_4);
}

// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
  SystemInit();
  SystemClock_Config_HSE_240Mhz();
  SystemCoreClockUpdate();
}

