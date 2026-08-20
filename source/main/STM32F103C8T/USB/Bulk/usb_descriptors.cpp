#include "tusb.h"
#include "board_api.h"
#include <string.h>

// String-Deskriptoren (Index-Zuweisung)
enum
{
  STRID_LANGID = 0,
  STRID_MANUFACTURER,
  STRID_PRODUCT,
  STRID_SERIAL,
};

// C++ benötigt die exakte Reihenfolge ohne Array-Designatoren
char const* string_desc_arr[] =
{
  "",                  // Index 0 (wird nicht als String-Text genutzt)
  "CustomTech",        // Index 1: STRID_MANUFACTURER
  "STM32 Bulk Device", // Index 2: STRID_PRODUCT
  "123456",            // Index 3: STRID_SERIAL
};

// Gerätedeskriptor ohne Designated Initializers in exakter Reihenfolge der Struktur
tusb_desc_device_t const desc_device =
{
  sizeof(tusb_desc_device_t),   // bLength
  TUSB_DESC_DEVICE,             // bDescriptorType
  0x0200,                       // bcdUSB (USB 2.0)
  0x00,                         // bDeviceClass
  0x00,                         // bDeviceSubClass
  0x00,                         // bDeviceProtocol
  CFG_TUD_ENDPOINT0_SIZE,       // bMaxPacketSize0
  0xCAFE,                       // idVendor
  0x4002,                       // idProduct
  0x0100,                       // bcdDevice
  STRID_MANUFACTURER,           // iManufacturer
  STRID_PRODUCT,                // iProduct
  STRID_SERIAL,                 // iSerialNumber
  1                             // bNumConfigurations
};

#define MS_OS_20_DESC_LEN          0xB2
#define VENDOR_REQUEST_MICROSOFT   0x01

// Berechnung der Gesamtlänge auf Basis der offiziellen Master-Makros
#define BOS_TOTAL_LEN              (TUD_BOS_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

uint8_t const desc_bos[] =
{
  // BOS-Header (Parameter: Gesamt-Länge, Anzahl der Device Capabilities)
  TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 1),

  // Microsoft OS 2.0 Platform Capability Descriptor
  TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT)
};


uint8_t const desc_ms_os_20[] =
{
  // Set Descriptor Set Header: length, type, windows version, total length
  0x0A, 0x00, 0x00, 0x00, 0x02, 0x00, 0xDF, 0x00, U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

  // Configuration Subset Header: length, type, config index, count, total length
  0x08, 0x00, 0x01, 0x00, 0x00, 0x00, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A),

  // Function Subset Header: length, type, first interface, count, total length
  //0x08, 0x00, 0x02, 0x00, 0x00, 0x00, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08),
  0x08, 0x00, 0x00, 0x00, 0x01, 0x00, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08),

  // Compatible ID Descriptor: length, type, compatibleID, subCompatibleID
  0x14, 0x00, 0x03, 0x00, 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

  // Registry Property Descriptor: length, type, propertyType, nameLength, name, dataLength, data
  0x84, 0x00, 0x04, 0x00, 0x07, 0x00, U16_TO_U8S_LE(0x002A),
  'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00,
  'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00, 'r', 0x00, 'f', 0x00,
  'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00,
  'D', 0x00, 's', 0x00, 0x00, 0x00, U16_TO_U8S_LE(0x0050),
  '{', 0x00, '9', 0x00, '7', 0x00, '5', 0x00, 'D', 0x00, '4', 0x00,
  '4', 0x00, '4', 0x00, '9', 0x00, '-', 0x00, '7', 0x00, '5', 0x00,
  '3', 0x00, '4', 0x00, '-', 0x00, '4', 0x00, '0', 0x00, 'A', 0x00,
  'B', 0x00, '-', 0x00, '8', 0x00, 'B', 0x00, '4', 0x00, 'C', 0x00,
  '-', 0x00, '9', 0x00, 'E', 0x00, '6', 0x00, '1', 0x00, '9', 0x00,
  'A', 0x00, '7', 0x00, 'B', 0x00, 'D', 0x00, 'E', 0x00, 'F', 0x00,
  '}', 0x00, 0x00, 0x00, 0x00, 0x00
};


// Extern-C-Block, damit der C-basierte TinyUSB-Stack die Symbole in C++ findet
extern "C"
{
  // Offizieller TinyUSB-Callback für den BOS-Deskriptor
  uint8_t const* tud_descriptor_bos_cb(void)
  {
    board_led_write(1);
    return desc_bos;
  }

  // Ersetzt den dedizierten MS-OS-Callback auf dem aktuellen Master-Stand
  bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const* request)
  {
    // Extrahiert den Request-Typ aus dem bmRequestType-Byte mittels Bitmaske
    uint8_t req_type = request->bmRequestType_bit.type;

    board_led_write(1);

    // Filtert nach Vendor-Anfragen (Wert 2) mit unserem definierten Request-Code 0x01
    if ((req_type == TUSB_REQ_TYPE_VENDOR) && (request->bRequest == VENDOR_REQUEST_MICROSOFT))
    {
      // Index 7 signalisiert der Hardware, dass Windows den MS OS 2.0 Deskriptor anfordert
      if (request->wIndex == 7)
      {
        if (stage == CONTROL_STAGE_SETUP)
        {
          tud_control_xfer(rhport, request, (void*)(uintptr_t)desc_ms_os_20, MS_OS_20_DESC_LEN);
        }
        return true;
      }
    }

    // Gibt false zurück, falls es sich um eine unbekannte Anfrage handelt
    return false;
  }

  uint8_t const * tud_descriptor_device_cb(void)
  {
    return (uint8_t const *) &desc_device;
  }

  // Konfigurationsdeskriptor mit 1 Interface und 2 Bulk-Endpunkten
  #define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)

  uint8_t const desc_configuration[] =
  {
    // Konfigurations-Header
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, CONFIG_TOTAL_LEN, 0x00, 100),

    // Interface-Header für Vendor-Klasse (Interface 0, 2 Endpunkte)
    TUD_VENDOR_DESCRIPTOR(0, 0, 0x01, 0x81, 64)
  };

  uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
  {
    (void) index;
    return desc_configuration;
  }

  uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
  {
    (void) langid;
    static uint16_t _desc_str[32]; // Array-Größe explizit für C++ Speicherplatz

    if (index == STRID_LANGID)
    {
      _desc_str[0] = (TUSB_DESC_STRING << 8) | 4;
      _desc_str[0] = 0x0409; // Englisch
      return _desc_str;
    }

    char const* str = string_desc_arr[index];
    uint8_t chr_count = strlen(str);
    if (chr_count > 31)
    {
      chr_count = 31;
    }

    _desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * chr_count + 2);
    for (uint8_t i = 0; i < chr_count; i++)
    {
      _desc_str[1 + i] = str[i];
    }
    return _desc_str;
  }

} // extern "C"
