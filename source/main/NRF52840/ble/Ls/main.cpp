

#include <stdint.h>
#include <string.h>
#include "main.h"
#include "nordic_common.h"
#include "nrf.h"
#include "ble_hci.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "app_timer.h"
#include "ble_nus.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_systick.h"
#include "nrf_drv_ppi.h"
#include "nrf_drv_timer.h"
#include "nrf_delay.h"

//#include "nrf_uarte.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "i2c_devices.h"

// nRF52840
// ARM�-based Cortex�-M4 32b MCU, (64 MHz max)
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

//I2C Adresses
// TMP102:             0x90
// L3GD20:  Gyro:      0xD4
// LSM303D: Acc:       0x32
// LSM303D: Mag:       0x3C


// P0.04 AIN2 BVolt
// 2M - 806k => 0.713 =>
//              3300 / 0.713 => 4630mV
//                    4630mV = > 3300 mV
//                    4200mV = > 3000 mV
//                    3500mV = > 2495 mV
//                    3000mV = > 2140 mV




volatile uint8_t ADC_Finished = 0;

volatile static nrf_saadc_value_t     m_Adc_Bat;

volatile static nrf_saadc_value_t     m_ADC_SEN[2 * 3 * 8]; // 2 ADC channels SEN A/B; 3 measurements per channel, 8 Mux channels
volatile static i16                   m_ADC_SEN2[2][8]; // 2 ADC channels SEN A/B; 3 measurements per channel, 8 Mux channels

u16      mau16SensDataAll[64];


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
  // P0.04 AIN2 BVolt


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

  lChannelCfg.pin_p = (nrf_saadc_input_t)(NRF_SAADC_INPUT_AIN2);
  err_code = nrf_drv_saadc_channel_init(2, &lChannelCfg);
  APP_ERROR_CHECK(err_code);
}


/* Indicates if operation on TWI has ended. */
static volatile bool m_xfer_done = false;

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/* Buffer for samples read from temperature sensor. */
static u8  m_Tmp102_Val;

typedef struct
{
  i16 x;
  i16 y;
  i16 z;
}tstPos3d;


tstPos3d mstLsm303AccVal;
tstPos3d mstL3GD20GyroVal;

void vI2cTest(void)
{

  ret_code_t err_code;
  u8 reg[1] = { 0 };

  for (u8 lu8Idx = 0; lu8Idx < 128; lu8Idx++)
  {
    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, lu8Idx, reg, 1, false);
    if (err_code)
    {
      __asm("nop");
    }
    while (m_xfer_done == false);
  }
}



void LSM303DLHC_ACC_set_mode(void)
{
  u8 lui8Reg[6];
  lui8Reg[0] = (LSM303DLHC_ACC_ODR_50_HZ | LSM303DLHC_ACC_NORMAL_MODE |
                LSM303DLHC_ACC_X_ENABLE  | LSM303DLHC_ACC_Y_ENABLE | LSM303DLHC_ACC_Z_ENABLE);
  lui8Reg[1] = LSM303DLHC_ACC_HPM_NORMAL_MODE | LSM303DLHC_ACC_HPFCF_32;
  lui8Reg[2] = 0;// No Ints
  lui8Reg[3] = LSM303DLHC_ACC_BlockUpdate_Continous | LSM303DLHC_ACC_FULLSCALE_4G;
  lui8Reg[4] = 0;
  lui8Reg[5] = 0;

  ret_code_t err_code;
  u8 reg[2] = { LSM303DLHC_ACC_CTRL_REG1_A, lui8Reg[0] };

  for (u8 lu8Idx = 0; lu8Idx < 6; lu8Idx++)
  {
    reg[1] = lui8Reg[lu8Idx];

    m_xfer_done = false;
    err_code = nrf_drv_twi_tx(&m_twi, LSM303DLHC_ADDRESS_DEFAULT, reg, 2, false);
    if (err_code)
    {
      __asm("nop");
    }
    while (m_xfer_done == false);
    reg[0]++;
  }
}



static void LSM303DLHC_Read_Acc()
{
  // Write Adress
  ret_code_t err_code;
  u8 reg = { 0x80 | LSM303DLHC_ACC_OUT_X_L_A };
  m_xfer_done = false;
  err_code = nrf_drv_twi_tx(&m_twi, LSM303DLHC_ADDRESS_DEFAULT, &reg, 1, false);
  if (err_code)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);


  m_xfer_done = false;
  err_code = nrf_drv_twi_rx(&m_twi, LSM303DLHC_ADDRESS_DEFAULT, (u8*)&mstLsm303AccVal, 6);
  if (err_code != 0)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);
}


void L3GD20_ACC_set_mode(void)
{
  ret_code_t err_code;
  u8 reg[2];

  /* Configure MEMS: data rate, power mode, full scale and axes */
  u8 ctrl1 = (u8)(L3GD20_MODE_ACTIVE | L3GD20_OUTPUT_DATARATE_2 |
                  L3GD20_AXES_ENABLE | L3GD20_BANDWIDTH_4);
  reg[0] = L3GD20_CTRL_REG1_ADDR;
  reg[1] = ctrl1;
  m_xfer_done = false;
  err_code = nrf_drv_twi_tx(&m_twi, L3GD20_ADDRESS_DEFAULT, reg, 2, false);
  if (err_code)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);

  u8 ctrl4 = (u8)(L3GD20_BlockDataUpdate_Continous | L3GD20_BLE_LSB |
                  L3GD20_FULLSCALE_500);
  reg[0] = L3GD20_CTRL_REG4_ADDR;
  reg[1] = ctrl4;
  m_xfer_done = false;
  err_code = nrf_drv_twi_tx(&m_twi, L3GD20_ADDRESS_DEFAULT, reg, 2, false);
  if (err_code)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);


  u8 ctrl5 = (u8)(L3GD20_HIGHPASSFILTER_ENABLE);
  reg[0] = L3GD20_CTRL_REG5_ADDR;
  reg[1] = ctrl5;
  m_xfer_done = false;
  err_code = nrf_drv_twi_tx(&m_twi, L3GD20_ADDRESS_DEFAULT, reg, 2, false);
  if (err_code)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);
}


static void L3GD20_Read_Gyro()
{
  // Write Adress
  ret_code_t err_code;
  /* In order to read multiple bytes, it is necessary to assert the
     most significant bit of the sub-address field. In other words,
     SUB(7) must be equal to 1 while SUB(6-0) represents the address
     of the first register to be read. */
  u8 reg = { 0x80 | L3GD20_OUT_X_L_ADDR };
  m_xfer_done = false;
  err_code = nrf_drv_twi_tx(&m_twi, L3GD20_ADDRESS_DEFAULT, &reg, 1, false);
  if (err_code)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);


  m_xfer_done = false;
  err_code = nrf_drv_twi_rx(&m_twi, L3GD20_ADDRESS_DEFAULT, (u8*)&mstL3GD20GyroVal, 6);
  if (err_code != 0)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);
}


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
  if (err_code != 0)
  {
    __asm("nop");
  }
  while (m_xfer_done == false);
}


/**
 * @brief TWI events handler.
 */
void twi_handler(nrf_drv_twi_evt_t const * p_event, void * p_context)
{
  switch (p_event->type)
  {
    case NRF_DRV_TWI_EVT_DONE:
      m_xfer_done = true;
      break;
    case NRF_DRV_TWI_EVT_ADDRESS_NACK:
      m_xfer_done = true;
      break;
    case NRF_DRV_TWI_EVT_DATA_NACK:
      m_xfer_done = true;
      break;
    default:
      break;
  }
}


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

static void TMP102_Read_Temp()
{
  m_xfer_done = false;

  /* Read 1 byte from the specified address - skip 3 bits dedicated for fractional part of temperature. */
  ret_code_t err_code = nrf_drv_twi_rx(&m_twi, TMP102_ADDR, &m_Tmp102_Val, sizeof(m_Tmp102_Val));
  while (m_xfer_done == false);
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




#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define DEVICE_NAME                     "Nordic_UART"                               /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */

#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */


BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */

static uint16_t   m_conn_handle          = BLE_CONN_HANDLE_INVALID;                 /**< Handle of the current connection. */
static uint16_t   m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;            /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */
static ble_uuid_t m_adv_uuids[]          =                                          /**< Universally unique service identifier. */
{
    {BLE_UUID_NUS_SERVICE, NUS_SERVICE_UUID_TYPE}
};


/**@brief Function for assert macro callback.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyse
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num    Line number of the failing ASSERT call.
 * @param[in] p_file_name File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for initializing the timer module.
 */
static void timers_init(void)
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the GAP initialization.
 *
 * @details This function will set up all the necessary GAP (Generic Access Profile) parameters of
 *          the device. It also sets the permissions and appearance.
 */
static void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *) DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 */
/**@snippet [Handling the data received over BLE] */
static void nus_data_handler(ble_nus_evt_t * p_evt)
{

    if (p_evt->type == BLE_NUS_EVT_RX_DATA)
    {
        uint32_t err_code;

        NRF_LOG_DEBUG("Received data from BLE NUS. Writing data on UART.");
        NRF_LOG_HEXDUMP_DEBUG(p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);

        err_code = ble_nus_data_send(&m_nus, (u8*) p_evt->params.rx_data.p_data, &p_evt->params.rx_data.length, m_conn_handle);

        /*for (uint32_t i = 0; i < p_evt->params.rx_data.length; i++)
        {
            do
            {
                err_code = app_uart_put(p_evt->params.rx_data.p_data[i]);
                if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
                {
                    NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
                    APP_ERROR_CHECK(err_code);
                }
            } while (err_code == NRF_ERROR_BUSY);
        }
        if (p_evt->params.rx_data.p_data[p_evt->params.rx_data.length - 1] == '\r')
        {
            while (app_uart_put('\n') == NRF_ERROR_BUSY);
        }*/
    }

}
/**@snippet [Handling the data received over BLE] */


/**@brief Function for initializing services that will be used by the application.
 */
static void services_init(void)
{
    uint32_t           err_code;
    ble_nus_init_t     nus_init;
    nrf_ble_qwr_init_t qwr_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    // Initialize NUS.
    memset(&nus_init, 0, sizeof(nus_init));

    nus_init.data_handler = nus_data_handler;

    err_code = ble_nus_init(&m_nus, &nus_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling an event from the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module
 *          which are passed to the application.
 *
 * @note All this function does is to disconnect. This could have been done by simply setting
 *       the disconnect_on_fail config parameter, but instead we use the event handler
 *       mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling errors from the Connection Parameters module.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
static void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

    // Prepare wakeup buttons.
    err_code = bsp_btn_ble_sleep_mode_prepare();
    APP_ERROR_CHECK(err_code);

    // Go to system-off mode (this function will not return; wakeup will cause a reset).
    err_code = sd_power_system_off();
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;
        case BLE_ADV_EVT_IDLE:
            {
              uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
              APP_ERROR_CHECK(err_code);
              //sleep_mode_enter();
            }
            break;
        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("Connected");
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
            // LED indication will be changed when advertising starts.
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_AUTO,
             };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
        } break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            // Pairing not supported
            err_code = sd_ble_gap_sec_params_reply(m_conn_handle, BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // No system attributes have been stored.
            err_code = sd_ble_gatts_sys_attr_set(m_conn_handle, NULL, 0, 0);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for the SoftDevice initialization.
 *
 * @details This function initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for handling events from the GATT library. */
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
        m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
        NRF_LOG_INFO("Data len is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
    }
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
}


/**@brief Function for initializing the GATT library. */
void gatt_init(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated by button press.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;
    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break;

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}


/**@brief   Function for handling app_uart events.
 *
 * @details This function will receive a single character from the app_uart module and append it to
 *          a string. The string will be be sent over BLE when the last character received was a
 *          'new line' '\n' (hex 0x0A) or if the string has reached the maximum data length.
 */
/**@snippet [Handling the data received over UART] */
////void uart_event_handle(app_uart_evt_t * p_event)
////{
////    static uint8_t data_array[BLE_NUS_MAX_DATA_LEN];
////    static uint8_t index = 0;
////    uint32_t       err_code;
////
////    switch (p_event->evt_type)
////    {
////        case APP_UART_DATA_READY:
////            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
////            index++;
////
////            if ((data_array[index - 1] == '\n') ||
////                (data_array[index - 1] == '\r') ||
////                (index >= m_ble_nus_max_data_len))
////            {
////                if (index > 1)
////                {
////                    NRF_LOG_DEBUG("Ready to send data over BLE NUS");
////                    NRF_LOG_HEXDUMP_DEBUG(data_array, index);
////
////                    do
////                    {
////                        uint16_t length = (uint16_t)index;
////                        err_code = ble_nus_data_send(&m_nus, data_array, &length, m_conn_handle);
////                        if ((err_code != NRF_ERROR_INVALID_STATE) &&
////                            (err_code != NRF_ERROR_RESOURCES) &&
////                            (err_code != NRF_ERROR_NOT_FOUND))
////                        {
////                            APP_ERROR_CHECK(err_code);
////                        }
////                    } while (err_code == NRF_ERROR_RESOURCES);
////                }
////
////                index = 0;
////            }
////            break;
////
////        case APP_UART_COMMUNICATION_ERROR:
////            APP_ERROR_HANDLER(p_event->data.error_communication);
////            break;
////
////        case APP_UART_FIFO_ERROR:
////            APP_ERROR_HANDLER(p_event->data.error_code);
////            break;
////
////        default:
////            break;
////    }
////}
/**@snippet [Handling the data received over UART] */


/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
////static void uart_init(void)
////{
////    uint32_t                     err_code;
////    app_uart_comm_params_t const comm_params =
////    {
////        .rx_pin_no    = RX_PIN_NUMBER,
////        .tx_pin_no    = TX_PIN_NUMBER,
////        .rts_pin_no   = RTS_PIN_NUMBER,
////        .cts_pin_no   = CTS_PIN_NUMBER,
////        .flow_control = APP_UART_FLOW_CONTROL_DISABLED,
////        .use_parity   = false,
////#if defined (UART_PRESENT)
////        .baud_rate    = NRF_UART_BAUDRATE_115200
////#else
////        .baud_rate    = NRF_UARTE_BAUDRATE_115200
////#endif
////    };
////
////    APP_UART_FIFO_INIT(&comm_params,
////                       UART_RX_BUF_SIZE,
////                       UART_TX_BUF_SIZE,
////                       uart_event_handle,
////                       APP_IRQ_PRIORITY_LOWEST,
////                       err_code);
////    APP_ERROR_CHECK(err_code);
////}
/**@snippet [UART Initialization] */


/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;

    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;
    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
static void buttons_leds_init(bool * p_erase_bonds)
{
    bsp_event_t startup_event;

    uint32_t err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
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



/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


/**@brief Function for starting advertising.
 */
static void advertising_start(void)
{
    uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
    APP_ERROR_CHECK(err_code);
}


int16 i16Swap(i16 liVal)
{
  u8* lu8Val;
  u8  lu8Temp;

  lu8Val = (u8*)&liVal;

  lu8Temp = lu8Val[0];
  lu8Val[0] = lu8Val[1];
  lu8Val[1] = lu8Temp;

  return liVal;
}

const nrf_drv_timer_t TIMER_CNT = NRF_DRV_TIMER_INSTANCE(1);
u32 mu32Counter;

void Counter_event_handler(nrf_timer_event_t event_type, void* p_context)
{
  switch (event_type)
  {
  case NRF_TIMER_EVENT_COMPARE0:
    mu32Counter++;
    break;

  default:
    break;
  }
}

void Counter_init(void)
{
  uint32_t time_ms = 1; //Time(in miliseconds) between consecutive compare events.
  uint32_t time_ticks;
  uint32_t err_code = NRF_SUCCESS;

  //Configure TIMER_LED for generating simple light effect - leds on board will invert his state one after the other.
  nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
  err_code = nrf_drv_timer_init(&TIMER_CNT, &timer_cfg, Counter_event_handler);
  APP_ERROR_CHECK(err_code);

  time_ticks = nrf_drv_timer_ms_to_ticks(&TIMER_CNT, time_ms);

  nrf_drv_timer_extended_compare(&TIMER_CNT, NRF_TIMER_CC_CHANNEL0, time_ticks, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, true);

  nrf_drv_timer_enable(&TIMER_CNT);
}



/**@brief Application main function.
 */
int main(void)
{
    u8 lu8SenIdx = 0;
    u16 lu16SendCnt = 0;
    bool erase_bonds;

    // Initialize.
    ////uart_init();
    log_init();
    timers_init();
    buttons_leds_init(&erase_bonds);
    power_management_init();
    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    advertising_init();
    conn_params_init();

    vInitExt3v3();
    vExt3v3_On();  //Einschalten f�r I2c Init
    vInitAnaMux();
    twi_init();
    saadc_init();
    //vI2cTest();
    TMP102_set_mode();
    LSM303DLHC_ACC_set_mode();
    L3GD20_ACC_set_mode();
    nrf_drv_systick_init();
    Counter_init();

    // Start execution.
    printf("\r\nUART started.\r\n");
    NRF_LOG_INFO("Debug logging for UART over RTT started.");
    advertising_start();

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
          nrfx_saadc_sample_convert(2, (nrf_saadc_value_t*)&m_Adc_Bat);
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
        // Ohne Gyro
          // Wird ca. alle 1.5ms aufgerufen
          // und dauert 300us
        // Mit Gyro
          // Wird ca. alle 1.65ms aufgerufen
          // und dauert 560us
        lu8SenIdx = 0;
        TMP102_Read_Temp();
        LSM303DLHC_Read_Acc();
        L3GD20_Read_Gyro();

        if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
        {
          if (lu16SendCnt == 0)
          {
            //char lcMyText[64];
            //u16 lu16Strlen;
            //sprintf(lcMyText, "%d %d %d %d", m_ADC_SEN2[0][0], m_ADC_SEN2[0][1], m_ADC_SEN2[0][2], m_ADC_SEN2[0][3]);
            //lu16Strlen = strlen(lcMyText);
            //mau16SensDataAll
            //ble_nus_data_send(&m_nus, (u8*)mau16SensDataAll, &lu16Strlen, m_conn_handle);

            for (u8 lu8t = 0; lu8t < 8; lu8t++)
            {
              if ((m_ADC_SEN2[0][lu8t]) < 0)
              {
                __asm ("NOP");
                m_ADC_SEN2[0][lu8t] = 0;
              }
              if ((m_ADC_SEN2[0][lu8t]) > 4095)
              {
                __asm ("NOP");
                m_ADC_SEN2[0][lu8t] = 4095;
              }
              if ((m_ADC_SEN2[1][lu8t]) < 0)
              {
                __asm ("NOP");
                m_ADC_SEN2[1][lu8t] = 0;
              }
              if ((m_ADC_SEN2[1][lu8t]) > 4095)
              {
                __asm ("NOP");
                m_ADC_SEN2[1][lu8t] = 4095;
              }

              mau16SensDataAll[0 + lu8t] = i16Swap(m_ADC_SEN2[0][lu8t]);
              mau16SensDataAll[8 + lu8t] = i16Swap(m_ADC_SEN2[1][lu8t]);
            }

            // Temp
            mau16SensDataAll[16] = i16Swap(m_Tmp102_Val);

            // Acc
            mau16SensDataAll[17] = i16Swap(mstLsm303AccVal.x);
            mau16SensDataAll[18] = i16Swap(mstLsm303AccVal.y);
            mau16SensDataAll[19] = i16Swap(mstLsm303AccVal.z);

            // Gyro
            mau16SensDataAll[20] = i16Swap(mstL3GD20GyroVal.x);
            mau16SensDataAll[21] = i16Swap(mstL3GD20GyroVal.y);
            mau16SensDataAll[22] = i16Swap(mstL3GD20GyroVal.z);

            // Zeitstempel
            u8* mpu8Time = (u8*)&mau16SensDataAll[23];
            mpu8Time[0] = ((mu32Counter >> 24) & 0xFF);
            mpu8Time[1] = ((mu32Counter >> 16) & 0xFF);
            mpu8Time[2] = ((mu32Counter >>  8) & 0xFF);
            mpu8Time[3] = ((mu32Counter >>  0) & 0xFF);

            mau16SensDataAll[25] = i16Swap(m_Adc_Bat);

                            //Fuss    Temp    Acc     Gyro    Tick   Bat
            u16 lu16Strlen = 16 * 2 + 1 * 2 + 3 * 2 + 3 * 2 +   4 +  1 * 2; // 50

            ble_nus_data_send(&m_nus, (u8*)mau16SensDataAll, &lu16Strlen, m_conn_handle);

            lu16SendCnt = 54;
          }
          else
          {
            lu16SendCnt--;
          }
        }
        else
        {
          lu16SendCnt = 0;
        }
      }
    }
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
}

