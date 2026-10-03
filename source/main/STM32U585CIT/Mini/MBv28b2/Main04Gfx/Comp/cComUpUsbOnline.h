#pragma once

#include "cSysDPool.h"


class cCompComUpUsbOnline : public cComponent
{
  public:

  cBotNet_LinkUsb gcUpLink;

  cCompComUpUsbOnline()
    : cComponent(cDepTreeCfg::cComp::nComUpOnline, { cDepTreeCfg::cComp::nComUpOffline })
  {
  }

  bool bInit() override
  {
    cSysDPool::mCom.mpcUpLinkUsb = &gcUpLink;
    // Add Uplink, nach der hardware initialisierung
    cSysDPool::mCom.mpcBn->bAddLink((cBotNet_LinkBase*)&gcUpLink, 0xE000);

    //cComponentList::mcList1ms.Add(this->mu8Idx);

    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    //cComponentList::mcList1ms.vRemove(this->mu8Idx);

    // return True to signal finished
    return cComponent::bDeInit();
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};

#ifdef __cplusplus
extern "C" {
#endif

//extern void tud_vendor_tx_cb(uint8_t idx, uint32_t xfer_bytes);
//extern void tud_vendor_rx_cb(uint8_t idx, const uint8_t* buffer, uint32_t bufsize);

// Wird von TinyUSB automatisch aufgerufen, wenn die HW das Paket gesendet hat
void tud_vendor_tx_cb(uint8_t idx, uint32_t xfer_bytes)
{
  (void)idx;
  (void)xfer_bytes;

  // Hardware ist wieder frei für das nächste Paket
  cSysDPool::mCom.mpcUpLinkUsb->vDataWriteDone();
}

// Offizieller Callback im Unbuffered-Modus (CFG_TUD_VENDOR_TXRX_BUFFERED = 0)
void tud_vendor_rx_cb(uint8_t idx, const uint8_t* buffer, uint32_t bufsize)
{
  (void)idx; // Verhindert Warnungen zu ungenutzten Variablen

  cSysDPool::mCom.mpcUpLinkUsb->vDataReceived(buffer, bufsize);
}

extern void OTG_FS_IRQHandler(void);

#ifdef __cplusplus
}
#endif


