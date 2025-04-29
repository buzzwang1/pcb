/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.H
* Author             : WCH
* Version            : V1.01
* Date               : 2022/12/13
* Description        : UART communication-related headers
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#ifndef __CDC_CLI_H__
#define __CDC_CLI_H__

#include "cRingBufT.h"

#ifdef __cplusplus
  extern "C" {
#endif

#include "ch32v30x_usbhs_device.h"
#include "ch32v30x_conf.h"



/******************************************************************************/
/* Related macro definitions */
/* Serial buffer related definitions */
#define DEF_USB_HS_PACK_LEN      DEF_USBD_HS_PACK_SIZE                        /* USB high speed mode packet size for serial x data */
#define DEF_CLI_RX_BUF_LEN       ( 2 * DEF_USBD_HS_PACK_SIZE )                /* Serial x receive buffer size */
#define DEF_CLI_TX_BUF_LEN       ( 1 * DEF_USBD_HS_PACK_SIZE )                /* Serial x transmit buffer size */
#define DEF_CLI_RX_BUF_NUM_MAX   ( DEF_CLI_RX_BUF_LEN / DEF_USB_HS_PACK_LEN ) /* Serial x transmit buffer size */


typedef struct __attribute__((packed))
{
  volatile u16 LoadNum;                                                /* Serial x data send buffer load number */
  volatile u16 DealNum;                                                /* Serial x data send buffer processing number */
  volatile u16 PgkCnt;                                                 /* Serial x data send buffer remaining unprocessed number */
  volatile u16 PkgLen[DEF_CLI_RX_BUF_NUM_MAX];                         /* The current packet length of the serial x data send buffer */
  u8  UsbDownStopFlag;                                                 /* Serial xUSB packet stop down flag */
}tstCdcCliCntrRx;

typedef struct __attribute__((packed))
{
  u8  USB_Up_IngFlag;                                                  /* Serial xUSB packet being uploaded flag */
  u8  USB_Up_Pack0_Flag;                                               /* Serial xUSB data needs to upload 0-length packet flag */
}tstCdcCliCntrTx;

/************************************************************/
/* Serial port X related structure definition */
typedef struct __attribute__((packed))
{
    tstCdcCliCntrRx Rx;
    tstCdcCliCntrTx Tx;
    uint8_t  USB_Desciptor;
}tstCdcCliCntr;

/***********************************************************************************************************************/
extern volatile tstCdcCliCntr Uart;                                                 /* Serial x control related structure */
extern __attribute__((aligned(4))) uint8_t CdcCli_Rx_Buf[DEF_CLI_RX_BUF_LEN];
extern __attribute__((aligned(4))) uint8_t CdcCli_Tx_Buf[DEF_CLI_TX_BUF_LEN];

/***********************************************************************************************************************/
/* Function extensibility */
extern void CdcCli_vParaInit();
extern void CdcCli_vUsbRx_Deal(cRingBufT<uint8, uint16>* lcStreamIn);
extern void CdcCli_vUsbTx_Deal(cRingBufT<uint8, uint16>* lcStreamOut);
extern void CdcCli_vInit();

#ifdef __cplusplus
}
#endif

#endif

/***********************************************************************************************************************/


