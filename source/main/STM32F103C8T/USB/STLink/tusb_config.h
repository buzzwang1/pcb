#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define CFG_TUSB_MCU             OPT_MCU_STM32F1
#define CFG_TUSB_RHPORT0_MODE    OPT_MODE_DEVICE

// Class Driver aktivieren
#define CFG_TUD_VENDOR           1

// Buffer-Größen für den Vendor-Treiber
#define CFG_TUD_VENDOR_RX_BUFSIZE 128
#define CFG_TUD_VENDOR_TX_BUFSIZE 128

#endif