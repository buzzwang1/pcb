
#ifndef _ILI9341_H
#define _ILI9341_H


/* Includes ------------------------------------------------------------------*/
#include "TYPEDEF.h"
#include "ClockInfo.h"
#include "bitmap_Bpp16_5R6G5B.h"


#include "TypeDef.h"
#include "stm32u5xx.h"
#include "stm32u5xx_hal.h"

#include "ili9341_hw.h"


/* LCD Size (Width and Height) */
#define ILI9341_WIDTH 				240
#define ILI9341_HEIGHT				320
#define ILI9341_PIXEL				76800


// SPI set
#define ILI9341_SPI						SPI1


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
  ILI9341_Orientation_Portrait_1,
  ILI9341_Orientation_Portrait_2,
  ILI9341_Orientation_Landscape_1,
  ILI9341_Orientation_Landscape_2
}ILI9341_Orientation_t;

// Orientation, Used private
typedef enum
{
  ILI9341_Landscape,
  ILI9341_Portrait
}ILI9341_Orientation;


class cILI9341 : public cILI9341_HW
{
public:

  DMA_HandleTypeDef mh_GPDMA1;
  LL_DMA_LinkNodeTypeDef mpDmaNode;

  u32 mData_Left;
  u32 mDmaBusy;


  cILI9341()
  {
    mData_Left = 0;
    mDmaBusy = 0;
    vInit();
    ILI9341_InitDMA();
  }

  void vReset(void)
  {
    // Reset display
    mPinCs.vSet1();
    mPinRst.vSet1();
    cClockInfo::Delay_ms(20);
    mPinRst.vSet0();
    cClockInfo::Delay_ms(100);
    mPinRst.vSet1();
    mPinCs.vSet1();
    cClockInfo::Delay_ms(100);
  }


  void vInit()
  {
    vReset();

    mPinCs.vSet0();

    cmd2(ILI9341_RESET);

    cmd2(ILI9341_POWERA);
    data2(0x39);
    data2(0x2C);
    data2(0x00);
    data2(0x34);
    data2(0x02);
    cmd2(ILI9341_POWERB);
    data2(0x00);
    data2(0xC1);
    data2(0x30);
    cmd2(ILI9341_DTCA);
    data2(0x85);
    data2(0x00);
    data2(0x78);
    cmd2(ILI9341_DTCB);
    data2(0x00);
    data2(0x00);
    cmd2(ILI9341_POWER_SEQ);
    data2(0x64);
    data2(0x03);
    data2(0x12);
    data2(0x81);
    cmd2(ILI9341_PRC);
    data2(0x20);
    cmd2(ILI9341_POWER1);
    data2(0x23);
    cmd2(ILI9341_POWER2);
    data2(0x10);
    cmd2(ILI9341_VCOM1);
    data2(0x3E);
    data2(0x28);
    cmd2(ILI9341_VCOM2);
    data2(0x86);
    cmd2(ILI9341_MAC);
    data2(0x20);
    cmd2(ILI9341_PIXEL_FORMAT);
    data2(0x55);
    cmd2(ILI9341_FRC);
    data2(0x00);
    data2(0x18);
    cmd2(ILI9341_DFC);
    data2(0x08);
    data2(0x82);
    data2(0x27);
    cmd2(ILI9341_3GAMMA_EN);
    data2(0x00);
    cmd2(ILI9341_COLUMN_ADDR);
    data2(0x00);
    data2(0x00);
    data2(0x00);
    data2(0xEF);
    cmd2(ILI9341_PAGE_ADDR);
    data2(0x00);
    data2(0x00);
    data2(0x01);
    data2(0x3F);
    cmd2(ILI9341_GAMMA);
    data2(0x01);
    cmd2(ILI9341_PGAMMA);
    data2(0x0F);
    data2(0x31);
    data2(0x2B);
    data2(0x0C);
    data2(0x0E);
    data2(0x08);
    data2(0x4E);
    data2(0xF1);
    data2(0x37);
    data2(0x07);
    data2(0x10);
    data2(0x03);
    data2(0x0E);
    data2(0x09);
    data2(0x00);
    cmd2(ILI9341_NGAMMA);
    data2(0x00);
    data2(0x0E);
    data2(0x14);
    data2(0x03);
    data2(0x11);
    data2(0x07);
    data2(0x31);
    data2(0xC1);
    data2(0x48);
    data2(0x08);
    data2(0x0F);
    data2(0x0C);
    data2(0x31);
    data2(0x36);
    data2(0x0F);
    cmd2(ILI9341_SLEEP_OUT);

    cClockInfo::Delay_ms(100);

    cmd2(ILI9341_DISPLAY_ON);
    cmd2(ILI9341_GRAM);

    cClockInfo::Delay_ms(100);
    cmd2(ILI9341_DISPLAY_ON);

    mPinCs.vSet1();

    //STARTING ROTATION
    vOrientation(ILI9341_Orientation_Landscape_1, 1);
    vTest(0xf800);
    vAddrSet2(0, 0, ILI9341_HEIGHT - 1, ILI9341_WIDTH - 1);
  }



  void vAddrSet2(u16 XS, u16 YS, u16 XE, u16 YE)
  {
    cmd2(0x2a); // Column address set
    data2(XS >> 8);
    data2(XS);
    data2(XE >> 8);
    data2(XE);

    cmd2(0x2b); // Row address set
    data2(YS >> 8);
    data2(YS);
    data2(YE >> 8);
    data2(YE);
  }

  void vOrientation(ILI9341_Orientation_t orientation, uint8 BGR)
  {
    uint8 lui8Data;

    lui8Data = 0;
    if (orientation == ILI9341_Orientation_Portrait_1)
    {
      lui8Data = 0x50;
    }
    else if (orientation == ILI9341_Orientation_Portrait_2)
    {
      lui8Data = 0x80;
    }
    else if (orientation == ILI9341_Orientation_Landscape_1)
    {
      lui8Data = 0x20;
    }
    else if (orientation == ILI9341_Orientation_Landscape_2)
    {
      lui8Data = 0xE0;
    }

    if (lui8Data)
    {
      cmd2(ILI9341_MAC);
      if (BGR) lui8Data |= 0x08;
      data2(lui8Data);
    }
  }

  void vTest(u16 color)
  {
    vAddrSet2(0, 0, 320 - 1, 240 - 1);
    cmd2(0x2c); // Memory write
    mPinCs.vSet0();
    mPinA0.vSet1();

    fill(color, (320 * 240));

    mPinCs.vSet1();
  }

  void ILI9341_InitDMA(void)
  {
    __HAL_RCC_GPDMA1_CLK_ENABLE();

    /* DMA node configuration declaration */
    LL_DMA_InitNodeTypeDef       DMA_InitNodeStruct;
    LL_DMA_InitLinkedListTypeDef DMA_InitLinkedListStruct;


    /* Set node 1 configuration ################################################*/
    /* Set node type */
    DMA_InitNodeStruct.NodeType = DMA_GPDMA_LINEAR_NODE;
    /* Set node configuration */
    DMA_InitNodeStruct.BlkHWRequest = LL_DMA_HWREQUEST_SINGLEBURST;
    DMA_InitNodeStruct.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    DMA_InitNodeStruct.DestAllocatedPort = LL_DMA_DEST_ALLOCATED_PORT1;
    DMA_InitNodeStruct.DestBurstLength = 1U;
    DMA_InitNodeStruct.DestIncMode = LL_DMA_DEST_FIXED;
    DMA_InitNodeStruct.DestDataWidth = LL_DMA_DEST_DATAWIDTH_BYTE;
    DMA_InitNodeStruct.SrcAllocatedPort = LL_DMA_SRC_ALLOCATED_PORT0;
    DMA_InitNodeStruct.SrcBurstLength = 1U;
    DMA_InitNodeStruct.SrcIncMode = LL_DMA_SRC_INCREMENT;
    DMA_InitNodeStruct.SrcDataWidth = LL_DMA_SRC_DATAWIDTH_BYTE;
    DMA_InitNodeStruct.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;


    /* Set node data handling parameters */
    DMA_InitNodeStruct.DataAlignment = LL_DMA_DATA_ALIGN_ZEROPADD;
    DMA_InitNodeStruct.DestHWordExchange = LL_DMA_DEST_HALFWORD_PRESERVE;
    DMA_InitNodeStruct.DestByteExchange = LL_DMA_DEST_BYTE_PRESERVE;
    DMA_InitNodeStruct.SrcByteExchange = LL_DMA_SRC_BYTE_PRESERVE;

    /* Set node trigger parameters */
    DMA_InitNodeStruct.TriggerPolarity = LL_DMA_TRIG_POLARITY_MASKED;

    /* Set repeated block parameters */
    DMA_InitNodeStruct.BlkRptDestAddrUpdateMode = LL_DMA_BLKRPT_DEST_ADDR_INCREMENT;
    DMA_InitNodeStruct.BlkRptSrcAddrUpdateMode = LL_DMA_BLKRPT_SRC_ADDR_INCREMENT;
    DMA_InitNodeStruct.DestAddrUpdateMode = LL_DMA_BURST_DEST_ADDR_INCREMENT;
    DMA_InitNodeStruct.SrcAddrUpdateMode = LL_DMA_BURST_SRC_ADDR_INCREMENT;
    DMA_InitNodeStruct.BlkRptCount = 0U;
    DMA_InitNodeStruct.DestAddrOffset = 0U;
    DMA_InitNodeStruct.SrcAddrOffset = 0U;
    DMA_InitNodeStruct.BlkRptDestAddrOffset = 0U;
    DMA_InitNodeStruct.BlkRptSrcAddrOffset = 0U;

    /* Set registers to be updated */
    DMA_InitNodeStruct.UpdateRegisters = LL_DMA_UPDATE_CTR1 | LL_DMA_UPDATE_CTR2 | \
                                         LL_DMA_UPDATE_CBR1 | LL_DMA_UPDATE_CSAR | \
                                         LL_DMA_UPDATE_CDAR | LL_DMA_UPDATE_CLLR;

    /* Set transfer parameters */
    DMA_InitNodeStruct.SrcAddress = 0;
    DMA_InitNodeStruct.DestAddress = 0;
    DMA_InitNodeStruct.BlkDataLength = 0;

    /* Initializes DMA linked list node */
    LL_DMA_CreateLinkNode(&DMA_InitNodeStruct, &mpDmaNode);



    /* Initialize DMA channel ##################################################*/
    /* Enable DMA clock */
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ALL);

    /* Set DMA channel parameter to be configured */
    DMA_InitLinkedListStruct.Priority = LL_DMA_HIGH_PRIORITY;
    DMA_InitLinkedListStruct.TransferEventMode = LL_DMA_TCEM_BLK_TRANSFER;
    DMA_InitLinkedListStruct.LinkStepMode = LL_DMA_LSM_FULL_EXECUTION;
    DMA_InitLinkedListStruct.LinkAllocatedPort = LL_DMA_LINK_ALLOCATED_PORT1;

    /* Initialize the DMA linked list */
    LL_DMA_List_Init(GPDMA1, LL_DMA_CHANNEL_6, &DMA_InitLinkedListStruct);


    /* Link created queue to DMA channel #######################################*/
    LL_DMA_SetLinkedListBaseAddr(GPDMA1, LL_DMA_CHANNEL_6, (uint32_t)&mpDmaNode);
    LL_DMA_ConfigLinkUpdate(GPDMA1, LL_DMA_CHANNEL_6, DMA_InitNodeStruct.UpdateRegisters, (uint32_t)&mpDmaNode);

    /* Enable DMA interrupts */
    //LL_DMA_EnableIT_USE(GPDMA1, LL_DMA_CHANNEL_6);
    //LL_DMA_EnableIT_ULE(GPDMA1, LL_DMA_CHANNEL_6);
    //LL_DMA_EnableIT_DTE(GPDMA1, LL_DMA_CHANNEL_6);
    //LL_DMA_EnableIT_TC(GPDMA1, LL_DMA_CHANNEL_6);

    /* DMA channel 13 interrupt init */
    //NVIC_SetPriority(GPDMA1_Channel6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
    //NVIC_EnableIRQ(GPDMA1_Channel6_IRQn);
  }

  /*
  uint8 ILI9341_u8DMA_TX_Complete(void)
  {
    uint8 lui8Result;

    lui8Result = 1;
    //At the end of the transfer, the Transfer Complete Flag (TCIF) is set
    //This bit is set by hardware. It is cleared by software writing 1 to the corresponding bit in the DMA_IFCR register.

    if (!mui8FirstTx)
    {
      lui8Result = DMA_GetFlagStatus(DMA2_Stream6, DMA_FLAG_TCIF6);
    }
    return lui8Result;
  }
*/


  bool bDmaTcHandler(void)
  {
    // Clear Flag register
    GPDMA1_Channel6->CFCR = 0xffff;
    SPI1->IFCR = 0xFFFF;
    if (mData_Left > 0)
    {
      if (mData_Left > 64000)
      {
        GPDMA1_Channel6->CBR1  = 64000;
        mData_Left -= 64000;
      }
      else
      {
        GPDMA1_Channel6->CBR1  = mData_Left;
        mData_Left = 0;
      }
      LL_DMA_EnableChannel(GPDMA1, LL_DMA_CHANNEL_6);
      SET_BIT(SPI1->CR1, SPI_CR1_CSTART);
    }
    else
    {
      LL_SPI_DisableDMAReq_TX(SPI1);
      LL_DMA_DisableChannel(GPDMA1, LL_DMA_CHANNEL_6);

      while (!LL_SPI_IsActiveFlag_TXC(SPI1)); // Wait for fifo
      CLEAR_BIT(SPI1->CR1, SPI_CR1_CSTART);
      LL_SPI_SetTransferSize(SPI1, 1);

      LL_DMA_SetSrcDataWidth( GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_SRC_DATAWIDTH_BYTE);
      LL_DMA_SetDestDataWidth(GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_SRC_DATAWIDTH_BYTE);

      LL_SPI_Disable(SPI1);
      LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_8BIT);
      LL_SPI_Enable(SPI1);

      mPinCs.vSet1();
      mDmaBusy = 0;
      return 1;
    }
    return 0;
  }


  void DMA_Start(uint32* lpui8SourceAdr, uint32 lui16SizeofSource)
  {

    if (mDmaBusy == 0)
    {
      mDmaBusy   = 1;
      mData_Left = lui16SizeofSource;

      LL_SPI_Disable(SPI1);
      LL_SPI_SetDataWidth(SPI1, LL_SPI_DATAWIDTH_16BIT);
      LL_SPI_SetTransferSize(SPI1, 0);
      LL_SPI_Enable(SPI1);

      //Control register
      //GPDMA1_Channel6->CCR;
      // Clear Flag register
      GPDMA1_Channel6->CFCR = 0xffff;

      //
      GPDMA1_Channel6->CLBAR = 0;
      GPDMA1_Channel6->CLLR  = 0;

      GPDMA1_Channel6->CDAR = (u32)&SPI1->TXDR;
      GPDMA1_Channel6->CSAR = (u32)lpui8SourceAdr;

      if (mData_Left > 64000)
      {
        GPDMA1_Channel6->CBR1  = 64000;
        mData_Left -= 64000;
      }
      else
      {
        GPDMA1_Channel6->CBR1  = mData_Left;
        mData_Left = 0;
      }


      SPI1->CR2             = 0;

      LL_DMA_SetTransferEventMode(    GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_TCEM_BLK_TRANSFER);
      LL_DMA_SetSrcIncMode(           GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_SRC_INCREMENT);
      LL_DMA_SetDestIncMode(          GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_DEST_FIXED);
      LL_DMA_SetTriggerMode(          GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_TRIGM_BLK_TRANSFER);
      LL_DMA_SetDataTransferDirection(GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
      LL_DMA_SetPeriphRequest(        GPDMA1, LL_DMA_CHANNEL_6, LL_GPDMA1_REQUEST_SPI1_TX);

      LL_DMA_SetSrcDataWidth(         GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_SRC_DATAWIDTH_HALFWORD);
      LL_DMA_SetDestDataWidth(        GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_DEST_DATAWIDTH_HALFWORD);

      LL_DMA_SetDestAllocatedPort(    GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_DEST_ALLOCATED_PORT0);
      LL_DMA_SetSrcAllocatedPort(     GPDMA1, LL_DMA_CHANNEL_6, LL_DMA_SRC_ALLOCATED_PORT1);

      LL_DMA_EnableIT_TC(             GPDMA1, LL_DMA_CHANNEL_6);
      HAL_NVIC_SetPriority(GPDMA1_Channel6_IRQn, 8, 8);
      HAL_NVIC_EnableIRQ(GPDMA1_Channel6_IRQn);

      LL_SPI_EnableDMAReq_TX(SPI1);
      LL_DMA_EnableChannel(GPDMA1, LL_DMA_CHANNEL_6);
      SET_BIT(SPI1->CR1, SPI_CR1_CSTART);
    }
  }


  void vShow1(cBitmap_Bpp16_5R6G5B* lpstBm)
  {
    LL_SPI_Disable(SPI1);
    LL_SPI_SetTransferSize(SPI1, 1); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(SPI1);
    vAddrSet2(0, 0, ILI9341_HEIGHT - 1, ILI9341_WIDTH - 1);
    cmd2(0x2c); // Memory write
    mPinCs.vSet0();
    mPinA0.vSet1();

    u32 i;
    u8  data;

    LL_SPI_Disable(SPI1);
    LL_SPI_SetTransferSize(SPI1, 0); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(SPI1);
    LL_SPI_StartMasterTransfer(SPI1);
    SPI1->IFCR = 0x10;

    for (i = 0; i < (u32)(lpstBm->miGfxHeight * lpstBm->miGfxWidth * 2); i++)
    {
      data = lpstBm->mpui8Data[i];

      while (!LL_SPI_IsActiveFlag_TXP(SPI1)); // Tx-Packet space available
      LL_SPI_TransmitData8(SPI1, data);
    }
    while (!LL_SPI_IsActiveFlag_TXC(SPI1)); // Wait for fifo
    CLEAR_BIT(SPI1->CR1, SPI_CR1_CSTART);

    LL_SPI_Disable(SPI1);
    LL_SPI_SetTransferSize(SPI1, 1); // When these bits are changed by software, the SPI must be disabled.
    LL_SPI_Enable(SPI1);

    mPinCs.vSet1();
  }


  void vShow(cBitmap_Bpp16_5R6G5B* lpstBm)
  {
    if (mDmaBusy == 0)
    {
      LL_SPI_Disable(SPI1);
      LL_SPI_SetTransferSize(SPI1, 1); // When these bits are changed by software, the SPI must be disabled.
      LL_SPI_Enable(SPI1);
      cmd2(0x2c); // Memory write
      mPinCs.vSet0();
      mPinA0.vSet1();

      SPI1->IFCR = 0xFFFF;

      DMA_Start((uint32*)lpstBm->mpui8Data, (lpstBm->miGfxHeight * lpstBm->miGfxWidth * 2));
    }
  }

};



#endif // _ILI9341_H
