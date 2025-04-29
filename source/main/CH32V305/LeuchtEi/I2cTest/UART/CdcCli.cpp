/********************************** (C) COPYRIGHT *******************************
* File Name          : UART.C
* Author             : WCH
* Version            : V1.01
* Date               : 2022/12/13
* Description        : uart serial port related initialization and processing
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "CdcCli.h"

/*******************************************************************************/
/* Variable Definition */
/* Global */

/* The following are serial port transmit and receive related variables and buffers */
volatile tstCdcCliCntr Uart;

__attribute__ ((aligned(4))) uint8_t  CdcCli_Rx_Buf[DEF_CLI_RX_BUF_LEN];
__attribute__ ((aligned(4))) uint8_t  CdcCli_Tx_Buf[DEF_CLI_TX_BUF_LEN];


void CdcCli_vParaInit()
{
    uint8_t i;

    Uart.Rx.LoadNum   = 0x00;
    Uart.Rx.DealNum   = 0x00;
    Uart.Rx.PgkCnt    = 0;

    for( i = 0; i < DEF_CLI_RX_BUF_NUM_MAX; i++ )
    {
       Uart.Rx.PkgLen[ i ] = 0x00;
    }
    Uart.Rx.UsbDownStopFlag = 0x00;

    Uart.Tx.USB_Up_Pack0_Flag = 0x00;
    Uart.Tx.USB_Up_IngFlag = 0x00;
}




/*********************************************************************
 * @fn      UART2_USB_Init
 *
 * @brief   Uart2 initialization in usb interrupt
 *
 * @return  none
 */
void CdcCli_vInit()
{

    CdcCli_vParaInit();

    /* restart usb receive  */
    USBHSD->UEP2_RX_DMA = (uint32_t)(uint8_t *)&CdcCli_Rx_Buf[0];
    USBHSD->UEP2_RX_CTRL &= ~USBFS_UEP_R_RES_MASK;
    USBHSD->UEP2_RX_CTRL |= USBFS_UEP_R_RES_ACK;
}

/*********************************************************************
 * @fn      UART2_DataTx_Deal
 *
 * @brief   Uart2 data transmission processing
 *
 * @return  none
 */
void CdcCli_vUsbRx_Deal(cRingBufT<uint8, uint16>* lcStreamIn)
{
  u16 count;


  if (Uart.Rx.PgkCnt)
  {  
    count = Uart.Rx.PkgLen[Uart.Rx.DealNum];
    if (lcStreamIn->space_left() > count)
    {
      NVIC_DisableIRQ( USBHS_IRQn );

      //put
      lcStreamIn->put((u8*)&CdcCli_Rx_Buf[Uart.Rx.DealNum * DEF_USB_HS_PACK_LEN], count);

      Uart.Rx.DealNum++;
      if( Uart.Rx.DealNum >= DEF_CLI_RX_BUF_NUM_MAX)
      {
        Uart.Rx.DealNum = 0x00;
      }
        
      if (Uart.Rx.PgkCnt > 0)
      {
        Uart.Rx.PgkCnt--;
      }
        
      // If the current serial port has suspended the downlink, restart the driver downlink
      if((Uart.Rx.UsbDownStopFlag == 0x01))
      {
        USBHSD->UEP2_RX_CTRL &= ~USBFS_UEP_R_RES_MASK;
        USBHSD->UEP2_RX_CTRL |= USBFS_UEP_R_RES_ACK;
        Uart.Rx.UsbDownStopFlag = 0x00;
      }
      NVIC_EnableIRQ( USBHS_IRQn );
    }
  }
}

/*********************************************************************
 * @fn      UART2_DataRx_Deal
 *
 * @brief   Uart2 data receiving processing
 *
 * @return  none
 */
void CdcCli_vUsbTx_Deal(cRingBufT<uint8, uint16>* lcStreamOut)
{
    u16 cnt;

    cnt = 0;

    /*****************************************************************/
    /* Serial port 1 data processing via USB upload and reception */
    if (Uart.Tx.USB_Up_IngFlag == 0x00)
    {
      cnt = lcStreamOut->cnt();

      if (cnt > DEF_CLI_TX_BUF_LEN / 2)
      {
        cnt = DEF_CLI_TX_BUF_LEN / 2;
      }

      if (cnt)
      {
        // get CdcCli_Tx_Buf //
        lcStreamOut->get(CdcCli_Tx_Buf, cnt);

        Uart.Tx.USB_Up_IngFlag = 0x01;
        NVIC_DisableIRQ( USBHS_IRQn );
        USBHSD->UEP2_TX_DMA = (uint32_t)(uint8_t *)&CdcCli_Tx_Buf;
        USBHSD->UEP2_TX_LEN  = cnt;
        USBHSD->UEP2_TX_CTRL &= ~USBHS_UEP_T_RES_MASK;
        USBHSD->UEP2_TX_CTRL |= USBHS_UEP_T_RES_ACK;
        NVIC_EnableIRQ( USBHS_IRQn );
      }
    }
}
