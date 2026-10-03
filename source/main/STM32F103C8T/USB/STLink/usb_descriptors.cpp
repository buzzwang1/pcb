#include "tusb.h"
#include "board_api.h"

extern "C"
{

// USB Device Descriptor
tusb_desc_device_t const desc_device = 
{
  .bLength            = sizeof(tusb_desc_device_t),
  .bDescriptorType    = TUSB_DESC_DEVICE,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = TUSB_CLASS_VENDOR_SPECIFIC,
  .bDeviceSubClass    = 0x00,
  .bDeviceProtocol    = 0x00,
  .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

  .idVendor           = 0x0483, // STMicroelectronics
  .idProduct          = 0x3748, // ST-LINK/V2 PID
  .bcdDevice          = 0x0100,

  .iManufacturer      = 0x01,
  .iProduct           = 0x02,
  .iSerialNumber      = 0x03,

  .bNumConfigurations = 0x01
};

uint8_t const* tud_descriptor_device_cb(void) 
{
  return (uint8_t const*)&desc_device;
}

// USB Configuration Descriptor
#define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_VENDOR_DESC_LEN)

#define EPNUM_VENDOR_OUT  0x01
#define EPNUM_VENDOR_IN   0x81

uint8_t const desc_configuration[] = 
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, 1, 0, CONFIG_TOTAL_LEN, 0x00, 100),

  // Interface number, string index, EP Out & EP In address, EP size
  TUD_VENDOR_DESCRIPTOR(0, 0, EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, 64)
};

uint8_t const* tud_descriptor_configuration_cb(uint8_t index) 
{
  (void)index;
  return desc_configuration;
}

// String Descriptors
char const* string_desc_arr[] = 
{
  (const char[]) {0x09, 0x04}, // 0: Supported language (English)
  "STMicroelectronics",        // 1: Manufacturer
  "ST-LINK/V2",                // 2: Product
  "000000000001",              // 3: Serial
};

static uint16_t _desc_str[32];

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) 
{
  (void)langid;
  uint8_t chr_count;

  if (index == 0) 
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }
  else 
  {
    if (index >= sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) return NULL;

    const char* str = string_desc_arr[index];
    chr_count = (uint8_t)strlen(str);
    if (chr_count > 31) chr_count = 31;

    for (uint8_t i = 0; i < chr_count; i++) {
      _desc_str[1 + i] = str[i];
    }
  }

  _desc_str[0] = (uint16_t)((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));
  return _desc_str;
}

} // extern "C"
