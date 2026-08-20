

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


#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define DEVICE_NAME                     "MyBle"                                     /**< Name of device. Will be included in the advertising data. */
#define NUS_SERVICE_UUID_TYPE           BLE_UUID_TYPE_VENDOR_BEGIN                  /**< UUID type for the Nordic UART Service (vendor specific). */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */

#define APP_ADV_INTERVAL                160                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 100ms). */
#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_ADV_SLOW_INTERVAL           3200                                        /**< The advertising interval (in units of 0.625 ms. This value corresponds to 2s). */
#define APP_ADV_SLOW_DURATION           0                                           /**< The advertising duration */


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


class cBotNet_LinkBle : public cBotNet_LinkBase
{
  public:

  cBotNetMsg_Base mpcMsgDataR;
  cBotNetMsg_Base mpcMsgDataT;

  u8 mu8PoolIdxTx;
  u8 mu8PoolIdxRx;

  bool mbWriteBusy;

  cBotNet_LinkBle()
    : cBotNet_LinkBase(cBotNet_LinkBase::enSideLink)
  {
    mStatus.IsInit    = 1;
    mStatus.IsOnline  = 0;
    mStatus.IsEnabled = 0;

    mu8PoolIdxTx = 0;
    mu8PoolIdxRx = 0;
  }

  virtual bool bAddedToBn(u16 lu16Adr)
  {
    bool lbRet = cBotNet_LinkBase::bAddedToBn(lu16Adr);

    mStatus.IsOnline  = 1;
    mStatus.IsEnabled = 1;

    mbWriteBusy = False;

    return lbRet;
  }

  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);
  }

  void vSetOnline()
  {
    mStatus.IsOnline = 1;
  }

  void vSetOffline()
  {
    mStatus.IsOnline = 0;
  }

  void vSync() override
  {
    if (!mbWriteBusy)
    {
      if ((mu8PoolIdxTx == 0) && (m_conn_handle != BLE_CONN_HANDLE_INVALID))
      {
        mu8PoolIdxTx = mcTxComBuf.get();

        if (mu8PoolIdxTx)
        {
          cBotNetMsg_Base lcMsg; cBnMsgPool::vGetMsg(lcMsg, mu8PoolIdxTx);
          mpcMsgDataT.From(lcMsg);
          u16 lu16Len = mpcMsgDataT.Len();
          ble_nus_data_send(&m_nus, (u8*)mpcMsgDataT.Data(), &lu16Len, m_conn_handle);
          mbWriteBusy = True;
          cBnMsgPool::vReleaseMsg(mu8PoolIdxTx);
          mu8PoolIdxTx = 0;
        }
      }
    }
  }

  void vDataWriteDone()
  {
    mbWriteBusy = False;
  }

  void vDataReceived(const u8* lpu8Buf, u16 lu16BufSize)
  {
    if (mu8PoolIdxRx == 0)
    {
      cBotNetMsg_Base lcMsg; cBnMsgPool::vReqMsg(lcMsg, mu8PoolIdxRx, lu16BufSize);
      if (mu8PoolIdxRx != 0)
      {
        mpcMsgDataR.From(lcMsg);
        mpcMsgDataR.Set(lpu8Buf, lu16BufSize);

        mcRxComBuf.put(mu8PoolIdxRx);
        cBnMsgPool::vPutMsg(mu8PoolIdxRx);
        cBnMsgPool::vSetLen(mu8PoolIdxRx, mpcMsgDataR.Len());

        cBnMsgPool::vReleaseMsg(mu8PoolIdxRx);
        mu8PoolIdxRx = 0;
      }
    }
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError); /* avoid warning */
    UNUSED(lenState); /* avoid warning */
  }

  void vComStart(cComNode::tenEvent lenEvent) override
  {
    UNUSED(lenEvent);
  }

  void vComDone() override
  {
  }

  void vAddChecksum(u8 lu8PoolIdx) override
  {
    UNUSED(lu8PoolIdx);
  }
};



cBotNetCfg mcMyBotNetCfg((const char8*)RomConst_stDevice_Info->szDevice_Name, RomConst_stDevice_Info->u16BnDeviceId, RomConst_stDevice_Info->u16BnNodeAdr);


cBotNet_LinkBle gcUpLink;


cBotNet gcBn(&mcMyBotNetCfg);

cBotNetMsgPortBtr   gcBtr(&gcBn);
cBotNetMsgPortSpop  gcSpop(&gcBn, &gcBtr);
cBotNetMsgPortRRpt  gcRRpt(&gcBn);
cBotNetMsgPortMView gcMView(&gcBn);

class cBn_MsgProcess : public cBotNet_MsgSysProcess
{
  public:
    cBn_MsgProcess()
      : cBotNet_MsgSysProcess(&gcBn)
    {
    }

  bool bMsg(cBotNetMsg_MsgProt& lcMsg)
  {
    u8* lu8RxPayload = lcMsg.GetPayload().mpu8Data;
    switch (lcMsg.u16GetIdx())
    {
      // System
      case 8: // Request message
        switch (lu8RxPayload[0])
        {
          case 0: // System: Identification
            // TX 00 | 00 | 10 | RR RR DI DI HW HW RR RR BT : RR: Reserve = 0; DI Device Idx; HW: Hardware Version; BT Board Type
            if ((lu8RxPayload[1] == 0) && (lu8RxPayload[2] == 10))
            {
              u8 lu8Data[12];

              lu8Data[ 0] =  0; // R1
              lu8Data[ 1] =  0; // S1
              lu8Data[ 2] = 10; // S2

              // Reserve
              lu8Data[ 3] = 0;
              lu8Data[ 4] = 0;
              // HV: HW Version
              lu8Data[ 5] = RomConst_stDevice_Info->u16BnDeviceId >> 8;
              lu8Data[ 6] = RomConst_stDevice_Info->u16BnDeviceId & 0xFF;;

              // HV: HW Version
              lu8Data[ 7] = RomConst_stDevice_Info->u8HwInfo >> 8;
              lu8Data[ 8] = RomConst_stDevice_Info->u8HwInfo & 0xFF;;
              // SV: SW Version
              lu8Data[ 9] = 0;
              lu8Data[10] = 0;
              // BT Board Type
              lu8Data[11] = 0; //u8GetRomConstBoardType();

              u8PutInt(lcMsg.cGetDAdr(), lcMsg.cGetSAdr(), 0x09, lu8Data, sizeof(lu8Data));

              return True;
            }
            break;
         }
         break;

      default:
        break;
    }
    return False;
  }
};

cBn_MsgProcess mcBn_MsgProcess;


class cCliCmd_SetPwm: public cCliCmd
{
  public:

  cCliCmd_SetPwm():cCliCmd((const char*)"s", (const char*)"pmw a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    lszStr.Setf((const char8*)"Test\r\n");
    lcCli->bPrintLn(lszStr);
      return True;
  }
};

class cCliCmd_SetPos: public cCliCmd
{
  public:

  cCliCmd_SetPos():cCliCmd((const char*)"p", (const char*)"pos a") {}

  bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
  {
    char8  lszStrBuf[32];
    cStr  lszStr(lszStrBuf, 32);

    UNUSED(lcParam);
    UNUSED(lbFirstCall);
    UNUSED(lCallerAdr);

    lszStr.Setf((const char8*)"Test\r\n");
    lcCli->bPrintLn(lszStr);

      return True;
  }
};

class cCliCmd_Status: public cCliCmd
{
  public:
    cCliCmd_Status():cCliCmd((const char*)"?", (const char*)"status")
    {}

    bool bProzessCmd(cStr &lcParam, cCli *lcCli, bool lbFirstCall, void* lCallerAdr) override
    {
      char8  lszStrBuf[32];
      cStr  lszStr(lszStrBuf, 32);

      UNUSED(lcParam);
      UNUSED(lbFirstCall);
      UNUSED(lCallerAdr);

      lszStr.Setf((const char8*)"Test\r\n");
      lcCli->bPrintLn(lszStr);

      return True;
    }
};


class cBotNetMotCli
{
  public:
  cCliCmd_SetPwm  mcCliCmd_SetPwm;
  cCliCmd_SetPos  mcCliCmd_SetPos;
  cCliCmd_Status  mcCliCmd_Status;

  cBotNetMotCli()
  {
    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPwm);
    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_SetPos);
    gcBn.mcStreamSys.mcCmdPort.bAddCmd(&mcCliCmd_Status);
  }
};

cBotNetMotCli mcBnMotCli;


cCliCmd_SetPwm    mcCliCmd_SetPwm;
cCliCmd_SetPos    mcCliCmd_SetPos;
cCliCmd_Status    mcCliCmd_Status;


cCliCmdList mcCliCmdListApp((cCliCmd* []) {&mcCliCmd_SetPwm,  &mcCliCmd_SetPos,
                                           &mcCliCmd_Status},  3);


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
  switch (p_evt->type)
  {
    case BLE_NUS_EVT_RX_DATA:
      {
        gcUpLink.vDataReceived((u8*)p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);
      }
      break;

    case BLE_NUS_EVT_TX_RDY:
      gcUpLink.vDataWriteDone();
      break;

    case BLE_NUS_EVT_COMM_STARTED:
      // Benachrichtigungen (Notifications) wurden vom PC aktiviert
      break;

    case BLE_NUS_EVT_COMM_STOPPED:
      // Benachrichtigungen wurden deaktiviert
      break;

    default:
      break;
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
            gcUpLink.vSetOnline();
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("Disconnected");
            gcUpLink.vSetOffline();
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

static uint8_t m_custom_data[] = { 0x01, 0x02, 0x03, 0x04, 0xAA, 0xBB };
/**@brief Function for initializing the Advertising functionality.
 */
static void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;
    ble_advdata_manuf_data_t manuf_data;

    memset(&init, 0, sizeof(init));

    // --- Custom Daten definieren ---
    manuf_data.company_identifier = 0x0059; // 0x0059 ist die offizielle ID für Nordic Semiconductor
                                            // Für eigene Testzwecke frei wählbar (2 Byte)
    manuf_data.data.p_data = m_custom_data;
    manuf_data.data.size = sizeof(m_custom_data);

    init.advdata.name_type          = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = false;
    init.advdata.flags              = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE;
    init.advdata.p_manuf_specific_data = &manuf_data;

    init.srdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.srdata.uuids_complete.p_uuids  = m_adv_uuids;

    // Fast Advertising (z. B. 100 ms für 3 Minuten)
    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_DURATION;


    //// Slow Advertising
    //init.config.ble_adv_slow_enabled = true;                  // <--- Hier aktivieren!
    //init.config.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL; // z.B. 3200 (2000 ms = 2 Sek)
    //init.config.ble_adv_slow_timeout = APP_ADV_SLOW_DURATION; // z.B. 36000 (360 Sek) oder 0 (unendlich)


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
      gcBn.vProcess(1000);
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


void MAIN_vInitSystem(void)
{
  /* SysTick end of count event each 10ms */

  cBnMsgPool::vInit();
  
  // Add MsgSys
  gcBtr.vAddMsgSys();
  gcSpop.vAddMsgSys();
  gcRRpt.vAddMsgSys();
  gcMView.vAddMsgSys();
  
  mcBn_MsgProcess.vAddMsgSys();
  
  gcBn.mcStreamSys.mcCmdPort.bAddCmdList(&mcCliCmdListApp);
  
  // Add Uplink, nach der hardware initialisierung
  gcBn.bAddLink((cBotNet_LinkBase*)&gcUpLink);
}

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
    vExt3v3_On();  //Einschalten für I2c Init

    nrf_drv_systick_init();

    MAIN_vInitSystem();

    Counter_init();

    // Start execution.
    printf("\r\nUART started.\r\n");
    NRF_LOG_INFO("Debug logging for UART over RTT started.");
    advertising_start();

    // Enter main loop.
    for (;;)
    {
      //idle_state_handle();

      //if (m_conn_handle != BLE_CONN_HANDLE_INVALID)
      //{
      //
      //  ble_nus_data_send(&m_nus, (u8*)mau16SensDataAll, &lu16Strlen, m_conn_handle);
      //}
    }
}


// This is called from the Startup Code, before the c++ contructors
void MainSystemInit()
{
}

