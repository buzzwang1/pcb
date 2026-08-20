#ifndef _TUSB_CONFIG_H_
#define _TUSB_CONFIG_H_

#define CFG_TUSB_MCU             OPT_MCU_STM32F1
#define CFG_TUD_ENABLED          1

// Definiert Port 0 als USB-Device mit Full-Speed (behebt den Assertion-Fehler)
#define CFG_TUSB_RHPORT0_MODE    OPT_MODE_DEVICE

#ifndef BOARD_TUD_MAX_SPEED
  #define BOARD_TUD_MAX_SPEED   OPT_MODE_DEFAULT_SPEED
#endif
#define CFG_TUD_MAX_SPEED         BOARD_TUD_MAX_SPEED

// Aktiviert die herstellerspezifische Klasse (Vendor Class) für Bulk-Transfers
#define CFG_TUD_VENDOR           1

// FIFO-Puffergrößen für die Bulk-Endpunkte (max. Paketgröße bei Full-Speed ist 64 Bytes)
#define CFG_TUD_VENDOR_RX_BUFSIZE 64
#define CFG_TUD_VENDOR_TX_BUFSIZE 64

#define CFG_TUD_ENDPOINT0_SIZE   64

#endif