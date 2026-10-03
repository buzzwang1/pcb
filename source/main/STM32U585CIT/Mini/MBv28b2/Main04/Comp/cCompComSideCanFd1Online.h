#pragma once

#include "cSysDPool.h"

#include "stm32u5xx.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_gpio.h"

class cCompComSideCanFd1Online : public cComponent
{
  public:

  FDCAN_HandleTypeDef mhfdcan1;
  FDCAN_FilterTypeDef mstFilterConfig;

  FDCAN_TxHeaderTypeDef mstTxHeader;
  FDCAN_RxHeaderTypeDef mstRxHeader;
  u8 mTxData0[12] = { 0x10, 0x32, 0x54, 0x76, 0x98, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
  u8 mRxData[12];

  cCompComSideCanFd1Online()
    : cComponent(cDepTreeCfg::cComp::nComSideOnline, { cDepTreeCfg::cComp::nComSideOffline })
  {
    cSysDPool::mCom.mu32TestRxCnt = 0;
  }

  void vInit_FDCAN1(void)
  {
    mhfdcan1.Instance = FDCAN1;

    HAL_FDCAN_MspInit(&mhfdcan1);

    mhfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_BRS;
    mhfdcan1.Init.Mode = FDCAN_MODE_NORMAL; // FDCAN_MODE_NORMAL / FDCAN_MODE_INTERNAL_LOOPBACK / FDCAN_MODE_EXTERNAL_LOOPBACK
    mhfdcan1.Init.AutoRetransmission = DISABLE;
    mhfdcan1.Init.TransmitPause = ENABLE;
    mhfdcan1.Init.ProtocolException = DISABLE;

    // Bit-Timing Konfiguration (500 kbit/s bei f_FDCAN = 80 MHz)
    // Formel: Bitrate = f_FDCAN / ( (Prescaler + 1) * (TimeSeg1 + TimeSeg2 + 3) )
    // Hier: 80 MHz / ( (9 + 1) * (10 + 3 + 3) ) =
    //       80 MHz /   ( 10    *     16)        = 500 kbit/s
    mhfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
    mhfdcan1.Init.NominalPrescaler = 10;
    mhfdcan1.Init.NominalSyncJumpWidth = 2;
    mhfdcan1.Init.NominalTimeSeg1 = 10;               // Prop_Seg + Phase_Seg1
    mhfdcan1.Init.NominalTimeSeg2 = 3;                // Phase_Seg2

    // Bit-Timing Konfiguration (500 kbit/s bei f_FDCAN = 80 MHz)
    // Formel: Bitrate = f_FDCAN / ( (Prescaler + 1) * (TimeSeg1 + TimeSeg2 + 3) )
    // Hier: 80 MHz / ( (0 + 1) * (10 + 3 + 3) ) =
    //       80 MHz /   ( 1    *     16)        = 5000 kbit/s
    mhfdcan1.Init.DataPrescaler = 1;
    mhfdcan1.Init.DataSyncJumpWidth = 2;
    mhfdcan1.Init.DataTimeSeg1 = 10;
    mhfdcan1.Init.DataTimeSeg2 = 3;


    mhfdcan1.Init.StdFiltersNbr = 1;
    mhfdcan1.Init.ExtFiltersNbr = 1;
    mhfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    if (HAL_FDCAN_Init(&mhfdcan1) != HAL_OK)
    {
      __asm("nop");
    }

    // Configure standard ID reception filter to Rx FIFO 0
    mstFilterConfig.IdType = FDCAN_STANDARD_ID;
    mstFilterConfig.FilterIndex = 0;
    mstFilterConfig.FilterType = FDCAN_FILTER_DUAL;
    mstFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    mstFilterConfig.FilterID1 = 0x444;
    mstFilterConfig.FilterID2 = 0x555;
    if (HAL_FDCAN_ConfigFilter(&mhfdcan1, &mstFilterConfig) != HAL_OK)
    {
      __asm("nop");
    }

    // Configure global filter:
    // Filter all remote frames with STD and EXT ID
    // Reject non matching frames with STD ID and EXT ID
    if (HAL_FDCAN_ConfigGlobalFilter(&mhfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
    {
      __asm("nop");
    }

    // Start FDCAN controller (continuous listening CAN bus)
    if (HAL_FDCAN_Start(&mhfdcan1) != HAL_OK)
    {
      __asm("nop");
    }
  }


  bool bInit() override
  {
    vInit_FDCAN1();

    cComponentList::mcList128ms.Add(this->mu8Idx);
    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    cComponentList::mcList128ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {

    /* Add message to Tx FIFO */
    mstTxHeader.Identifier = 0x444;
    mstTxHeader.IdType = FDCAN_STANDARD_ID;
    mstTxHeader.TxFrameType = FDCAN_DATA_FRAME;
    mstTxHeader.DataLength = FDCAN_DLC_BYTES_12;
    mstTxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    mstTxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    mstTxHeader.FDFormat = FDCAN_FD_CAN;
    mstTxHeader.TxEventFifoControl = FDCAN_STORE_TX_EVENTS;
    mstTxHeader.MessageMarker = 0x52;
    if (HAL_FDCAN_AddMessageToTxFifoQ(&mhfdcan1, &mstTxHeader, mTxData0) != HAL_OK)
    {
      __asm("nop");
    }

    while (HAL_FDCAN_GetRxFifoFillLevel(&mhfdcan1, FDCAN_RX_FIFO0))
    {
      HAL_FDCAN_GetRxMessage(&mhfdcan1, FDCAN_RX_FIFO0, &mstRxHeader, mRxData);
      cSysDPool::mCom.mu32TestRxCnt++;
    }

    // return True to signal finished
    return cComponent::bRun();
  };
};




