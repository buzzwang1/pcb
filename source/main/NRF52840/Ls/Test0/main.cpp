

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "nordic_common.h"
#include "boards.h"
#include "nrf.h"
#include "app_error.h"
#include "app_util_platform.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_delay.h"
#include "nrf_uarte.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


// nRF52840
// ARM®-based Cortex®-M4 32b MCU, (64 MHz max)
// Rom 1024KB
// Ram 256KB

// pca10056 = > nRF52840 eval board
// pca10059 = > nRF52840 usb dongle


// P0.13 Ext_Power_Pin
//
// P1.11 FEN3
// P1.13 FEN2
// P1.15 FEN1
//
// P0.31 AIN7 SENA
// P0.29 AIN5 SENB
//
// P0.06 SCL
// P0.08 SDA
//
// P0.10 TX
// P0.09 Rx


volatile uint8_t ADC_Finished = 0;

volatile static nrf_saadc_value_t     m_ADC_SEN[2 * 3 * 8]; // 2 ADC channels SEN A/B; 3 measurements per channel, 8 Mux channels
volatile static i16                   m_ADC_SEN2[2][8]; // 2 ADC channels SEN A/B; 3 measurements per channel, 8 Mux channels




void saadc_callback(nrf_drv_saadc_evt_t const* p_event)
{
  if (p_event->type == NRF_DRV_SAADC_EVT_DONE)
  {
    ADC_Finished = 1;
  }
}


void saadc_init(void)
{
  // P0.31 AIN7 SENA
  // P0.29 AIN5 SENB


  ret_code_t err_code;

  nrf_saadc_channel_config_t lChannelCfg;

  lChannelCfg.resistor_p = NRF_SAADC_RESISTOR_DISABLED;
  lChannelCfg.resistor_n = NRF_SAADC_RESISTOR_DISABLED;
  lChannelCfg.gain       = NRF_SAADC_GAIN1;
  lChannelCfg.reference  = NRF_SAADC_REFERENCE_INTERNAL;
  lChannelCfg.acq_time   = NRF_SAADC_ACQTIME_5US;
  lChannelCfg.mode       = NRF_SAADC_MODE_SINGLE_ENDED;
  lChannelCfg.burst      = NRF_SAADC_BURST_DISABLED;
  lChannelCfg.pin_n      = NRF_SAADC_INPUT_DISABLED;

  err_code = nrf_drv_saadc_init(NULL, saadc_callback);
  APP_ERROR_CHECK(err_code);

  lChannelCfg.pin_p = (nrf_saadc_input_t)(NRF_SAADC_INPUT_AIN7);
  err_code = nrf_drv_saadc_channel_init(0, &lChannelCfg);
  APP_ERROR_CHECK(err_code);

  lChannelCfg.pin_p = (nrf_saadc_input_t)(NRF_SAADC_INPUT_AIN5);
  err_code = nrf_drv_saadc_channel_init(1, &lChannelCfg);
  APP_ERROR_CHECK(err_code);
}




/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

/* Common addresses definition for temperature sensor. */
#define TMP102_ADDR          (0x90U >> 1)

#define TMP102_REG_TEMP      0x00U
#define TMP102_REG_CONF      0x01U
#define TMP102_REG_THYST     0x02U
#define TMP102_REG_TOS       0x03U

/* Mode for TMP102. */
#define NORMAL_MODE 0U

/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read from temperature sensor. */
static uint8_t m_sample;


void TMP102_set_mode(void)
{
  ret_code_t err_code;

  /* Writing to TMP102_REG_CONF "0" set temperature sensor in NORMAL mode. */
  uint8_t reg[3] = { TMP102_REG_CONF, NORMAL_MODE };
  // err_code = nrf_drv_twi_tx(&m_twi, TMP102_ADDR, reg, sizeof(reg), false);
  // APP_ERROR_CHECK(err_code);
  // while (m_xfer_done == false);

  /* Writing to pointer byte. */
  reg[0] = TMP102_REG_TEMP;
  m_xfer_done = false;
  err_code = nrf_drv_twi_tx(&m_twi, TMP102_ADDR, reg, 1, false);
  APP_ERROR_CHECK(err_code);
  while (m_xfer_done == false);
}


/**
 * @brief Function for handling data from temperature sensor.
 *
 * @param[in] temp          Temperature in Celsius degrees read from sensor.
 */
static void data_handler(uint8_t temp)
{
  //NRF_LOG_INFO("Temperature: %d Celsius degrees.", temp);
}


/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
  switch (p_event->type)
  {
    case NRF_DRV_TWI_EVT_DONE:
      if (p_event->xfer_desc.type == NRF_DRV_TWI_XFER_RX)
      {
        data_handler(m_sample);
      }
      m_xfer_done = true;
      break;
    default:
      break;
  }
}

/**
 * @brief UART initialization.
 */
void twi_init (void)
{
  // P0.06 SCL
  // P0.08 SDA

  ret_code_t err_code;

  const nrf_drv_twi_config_t twi_TMP102_config = {
      .scl                = NRF_GPIO_PIN_MAP(0, 6),
      .sda                = NRF_GPIO_PIN_MAP(0, 8),
      .frequency          = NRF_DRV_TWI_FREQ_400K,
      .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
      .clear_bus_init     = false
  };

  err_code = nrf_drv_twi_init(&m_twi, &twi_TMP102_config, twi_handler, NULL);
  APP_ERROR_CHECK(err_code);

  nrf_drv_twi_enable(&m_twi);
}

static void read_sensor_data()
{
  m_xfer_done = false;

  /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
  ret_code_t err_code = nrf_drv_twi_rx(&m_twi, TMP102_ADDR, &m_sample, sizeof(m_sample));
  if (err_code != 0)
  {
    //APP_ERROR_CHECK(err_code);
  }
}


void NMI_Handler(void)
{
  while (1) {}
}

void HardFault_Handler(void)
{
  while (1) {}
}


void MemManage_Handler(void)
{
  while (1) {}
}


void BusFault_Handler(void)
{
  while (1) {}
}


void UsageFault_Handler(void)
{
  while (1) {}
}


void SVC_Handler(void)
{
  while (1) {}
}

void DebugMon_Handler(void)
{
  while (1) {}
}


void PendSV_Handler(void)
{
  while (1) {}
}


void SysTick_Handler(void)
{
}

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */




/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{
  //ret_code_t err_code = app_timer_init();
  //APP_ERROR_CHECK(err_code);
}






/**@brief Function for initializing the nrf log module.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for initializing power management.
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}


static void vAnaMuxWait(u16 lu16Delay)
{
  for (u16 lu16t = 0; lu16t < lu16Delay; lu16t++)
  {
    __asm("nop");
  }
}

static void vAnaMuxAdr(u32 lu32Adr)
{
  lu32Adr &= 7;
  volatile u32 lu32Port = NRF_P1->OUTSET;

  lu32Port &= ~((1 << 15) | (1 << 13) | (1 << 11));

  if (lu32Adr & 1) lu32Port |= (1 << 15);
  if (lu32Adr & 2) lu32Port |= (1 << 13);
  if (lu32Adr & 4) lu32Port |= (1 << 11);

  NRF_P1->OUT = lu32Port;
}


static void vInitAnaMux()
{
   //P1.15 FEN1
   //P1.13 FEN2
   //P1.11 FEN3

  nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1, 15));
  nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1, 13));
  nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(1, 11));

  vAnaMuxAdr(0);
}

static void vExt3v3_On()
{
  nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0, 13), 1);
}

static void vExt3v3_Off()
{
  nrf_gpio_pin_write(NRF_GPIO_PIN_MAP(0, 13), 0);
}

static void vInitExt3v3()
{
  //P0.13 Ext_Power_Pin
  vExt3v3_Off();
  nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0, 13));
  vExt3v3_Off();
}




/**@brief Application main function.
 */
int main(void)
{
    u8 lu8SenIdx = 0;

    // Initialize.
    //uart_init();
    log_init();

    vInitExt3v3();
    vInitAnaMux();
    twi_init();
    saadc_init();
    TMP102_set_mode();

    // Enter main loop.
    for (;;)
    {
      //idle_state_handle();

      vExt3v3_On();

      vAnaMuxAdr(lu8SenIdx);
      vAnaMuxWait(250); // 250 entspr. 25us

      //nrf_drv_saadc_buffer_convert((nrf_saadc_value_t*)m_ADC_SEN, 6);
      ADC_Finished = 0;

      nrfx_saadc_sample_convert(0, (nrf_saadc_value_t*) &m_ADC_SEN[lu8SenIdx * 6 + 0]);
      nrfx_saadc_sample_convert(0, (nrf_saadc_value_t*) &m_ADC_SEN[lu8SenIdx * 6 + 2]);
      nrfx_saadc_sample_convert(0, (nrf_saadc_value_t*) &m_ADC_SEN[lu8SenIdx * 6 + 4]);

      nrfx_saadc_sample_convert(1, (nrf_saadc_value_t*) &m_ADC_SEN[lu8SenIdx * 6 + 1]);
      nrfx_saadc_sample_convert(1, (nrf_saadc_value_t*) &m_ADC_SEN[lu8SenIdx * 6 + 3]);
      nrfx_saadc_sample_convert(1, (nrf_saadc_value_t*) &m_ADC_SEN[lu8SenIdx * 6 + 5]);

      switch (lu8SenIdx)
      {
        case 0:
          m_ADC_SEN2[0][2] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][2] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
        case 1:
          m_ADC_SEN2[0][1] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][1] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
        case 2:      
          m_ADC_SEN2[0][0] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][0] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
        case 3:
          m_ADC_SEN2[0][3] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][3] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
        case 4:
          m_ADC_SEN2[0][4] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][4] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
        case 5:
          m_ADC_SEN2[0][7] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][7] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
        case 6:
          m_ADC_SEN2[0][5] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][5] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
        case 7:
          m_ADC_SEN2[0][6] = (m_ADC_SEN[lu8SenIdx * 6 + 0] + m_ADC_SEN[lu8SenIdx * 6 + 2] + m_ADC_SEN[lu8SenIdx * 6 + 4]) / 3;
          m_ADC_SEN2[1][6] = (m_ADC_SEN[lu8SenIdx * 6 + 1] + m_ADC_SEN[lu8SenIdx * 6 + 3] + m_ADC_SEN[lu8SenIdx * 6 + 5]) / 3;
          break;
      }

      lu8SenIdx++;
      
      if (lu8SenIdx > 7)
      {
        lu8SenIdx = 0;
        read_sensor_data();
      }
    }
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
}

