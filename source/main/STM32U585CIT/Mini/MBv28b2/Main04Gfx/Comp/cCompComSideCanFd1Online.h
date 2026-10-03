#pragma once

#include "cSysDPool.h"

#include "stm32u5xx.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_gpio.h"

class cCompComSideCanFd1Online : public cComponent
{
  public:

  FDCAN_HandleTypeDef hfdcan1;

  cCompComSideCanFd1Online()
    : cComponent(cDepTreeCfg::cComp::nComSideOnline, { cDepTreeCfg::cComp::nComSideOffline })
  {
  }

  void MX_FDCAN1_Init(void)
  {
    //hfdcan1.Instance = FDCAN1;
    //
    //// 1. Grundlegende Betriebsmodi
    //hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS; // CAN FD mit 64 Byte Payload, KEINE Hochgeschwindigkeitsphase
    //hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;            // Normaler Sende-/Empfangsbetrieb
    //hfdcan1.Init.AutoRetransmission = ENABLE;         // Automatische Wiederholung bei Fehlern
    //hfdcan1.Init.TransmitPause = DISABLE;             // Keine Zwangspause zwischen Übertragungen
    //hfdcan1.Init.ProtocolException = DISABLE;
    //
    //// 2. Bit-Timing Konfiguration (500 kbit/s bei f_FDCAN = 80 MHz)
    //// Formel: Bitrate = f_FDCAN / ( Prescaler * (1 + TimeSeg1 + TimeSeg2) )
    //// Hier: 80 MHz / ( 10 * (1 + 13 + 2) ) = 80 MHz / ( 10 * 16 ) = 500 kbit/s
    //hfdcan1.Init.NominalPrescaler = 10;
    //hfdcan1.Init.NominalSyncJumpWidth = 2;
    //hfdcan1.Init.NominalTimeSeg1 = 13;               // Prop_Seg + Phase_Seg1
    //hfdcan1.Init.NominalTimeSeg2 = 2;                // Phase_Seg2
    //
    //// Data-Timing wird bei NO_BRS nicht für die Umschaltung genutzt, 
    //// muss aber gültig initialisiert sein (wird auf Standardwerte gesetzt)
    //hfdcan1.Init.DataPrescaler = 10;
    //hfdcan1.Init.DataSyncJumpWidth = 2;
    //hfdcan1.Init.DataTimeSeg1 = 13;
    //hfdcan1.Init.DataTimeSeg2 = 2;
    //
    //// 3. Message RAM Konfiguration für 64 Byte Nutzdaten
    ////     FDCAN1 Message RAM Basisadresse : 0x4000 6400
    ////     FDCAN2 Message RAM Basisadresse : 0x4000 6C00 (falls verwendet)
    //hfdcan1.Init.StdFiltersNbr = 1;                  // Anzahl Standard-ID Filter (z. B. 1)
    //hfdcan1.Init.ExtFiltersNbr = 0;                  // Keine Extended-ID Filter
    //
    //// Tx Puffer Setup
    //hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_QUEUE_OPERATION;
    //
    //if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
    //{
    //  // Initialisierungsfehler abfangen
    //  //Error_Handler();
    //}
    //
    //// 4. FDCAN Modul im Prozessor starten
    //if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    //{
    //  //Error_Handler();
    //}
  }


  bool bInit() override
  {
    // return True to signal finished
    return cComponent::bInit();
  }

  bool bDeInit() override
  {
    // return True to signal finished
    return cComponent::bDeInit();
  }


  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




