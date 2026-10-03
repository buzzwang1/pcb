#pragma once

#include "cSysDPool.h"

#include "stm32u5xx.h"
#include "stm32u5xx_ll_rcc.h"
#include "stm32u5xx_ll_bus.h"
#include "stm32u5xx_ll_gpio.h"

class cCompComSideCanFd1Online : public cComponent
{
  public:

  cGpPin mCanTx;
  cGpPin mCanRx;

  FDCAN_HandleTypeDef hfdcan1;

  cCompComSideCanFd1Online()
    : cComponent(cDepTreeCfg::cComp::nComSideOnline, { cDepTreeCfg::cComp::nComSideOffline }),
      mCanTx(GPIOD_BASE, 1),
      mCanRx(GPIOD_BASE, 0)
  {
  }

  void MX_FDCAN1_Init(void)
  {
    mCanTx.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);
    mCanRx.vInit(GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_VERY_HIGH, 0);

    mCanTx.vSetAF(GPIO_MODE_AF_PP, GPIO_AF9_FDCAN1);   // SCK
    mCanRx.vSetAF(GPIO_MODE_AF_PP, GPIO_AF9_FDCAN1);  // MISO

    // FDCAN-Taktquelle auf PLL1Q setzen (LL_RCC_FDCAN_CLKSOURCE_PLL1)
    // -------------------------------------------------------------------------
    // Das FDCANSEL-Bitfeld befindet sich im RCC_CCIPR1-Register.
    // Bits 25:24 im CCIPR1: 00 = HSE, 01 = PLL1_Q, 10 = PLL2_Q
    LL_RCC_SetFDCANClockSource(LL_RCC_FDCAN_CLKSOURCE_PLL1);

     // FDCAN1 Clock Enable (im RCC_APB1ENR1)
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_FDCAN1);


    hfdcan1.Instance = FDCAN1;

    // 1. Grundlegende Betriebsmodi
    hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS; // CAN FD mit 64 Byte Payload, KEINE Hochgeschwindigkeitsphase
    hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;            // Normaler Sende-/Empfangsbetrieb
    hfdcan1.Init.AutoRetransmission = ENABLE;         // Automatische Wiederholung bei Fehlern
    hfdcan1.Init.TransmitPause = DISABLE;             // Keine Zwangspause zwischen Übertragungen
    hfdcan1.Init.ProtocolException = DISABLE;

    // 2. Bit-Timing Konfiguration (500 kbit/s bei f_FDCAN = 80 MHz)
    // Formel: Bitrate = f_FDCAN / ( Prescaler * (1 + TimeSeg1 + TimeSeg2) )
    // Hier: 80 MHz / ( 10 * (1 + 13 + 2) ) = 80 MHz / ( 10 * 16 ) = 500 kbit/s
    hfdcan1.Init.NominalPrescaler = 10;
    hfdcan1.Init.NominalSyncJumpWidth = 2;
    hfdcan1.Init.NominalTimeSeg1 = 13;               // Prop_Seg + Phase_Seg1
    hfdcan1.Init.NominalTimeSeg2 = 2;                // Phase_Seg2

    // Data-Timing wird bei NO_BRS nicht für die Umschaltung genutzt, 
    // muss aber gültig initialisiert sein (wird auf Standardwerte gesetzt)
    hfdcan1.Init.DataPrescaler = 10;
    hfdcan1.Init.DataSyncJumpWidth = 2;
    hfdcan1.Init.DataTimeSeg1 = 13;
    hfdcan1.Init.DataTimeSeg2 = 2;

    // 3. Message RAM Konfiguration für 64 Byte Nutzdaten
    //     FDCAN1 Message RAM Basisadresse : 0x4000 6400
    //     FDCAN2 Message RAM Basisadresse : 0x4000 6C00 (falls verwendet)
    hfdcan1.Init.StdFiltersNbr = 1;                  // Anzahl Standard-ID Filter (z. B. 1)
    hfdcan1.Init.ExtFiltersNbr = 0;                  // Keine Extended-ID Filter

    // Tx Puffer Setup
    hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_QUEUE_OPERATION;

    if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
    {
      // Initialisierungsfehler abfangen
      //Error_Handler();
    }

    // 4. FDCAN Modul im Prozessor starten
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
      //Error_Handler();
    }
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


  /**
    * @brief Sendet eine 64-Byte CAN-FD Nachricht mit ID 0x123 über LL-Registerzugriff.
    * @param hfdcan Pointer auf ein konfiguriertes FDCAN_HandleTypeDef (oder direkt FDCAN1)
    * @param pData Pointer auf die 64 Byte Nutzdaten
    * @param bufferIndex Der zu verwendende Tx-Puffer-Index (z.B. 0)
    * @note  Voraussetzung: Message RAM Adresse und Tx-Puffer Offset sind korrekt initialisiert.
    */
  void FDCAN_Send64Bytes_LL(FDCAN_HandleTypeDef *hfdcan, u8 *pData, u8 bufferIndex)
  {
      FDCAN_GlobalTypeDef *fdcan = hfdcan->Instance;

      // 1. Adresse des Tx-Puffers im Message RAM berechnen
      // hfdcan->msgRam.TxBufferSA enthält die Basisadresse des Tx-Puffers im SRAM
      // Ein 64-Byte Payload Element benötigt im RAM: 8 Byte Header + 64 Byte Data = 72 Byte (18 Words)
      u32 elementSizeInWords = 2 + 16; // 2 Header Words + 16 Data Words (64 Bytes)
      volatile u32 *txBufferAddr = (volatile u32 *)(hfdcan->msgRam.TxBufferSA + (bufferIndex * elementSizeInWords * 4));

      // 2. T0 Wort aufbauen:
      // Standard-ID (11 Bit): Bits 28..18 -> 0x123
      // XTD (Extended ID) = 0 (Standard ID)
      // RTR (Remote Frame) = 0 (Data Frame)
      u32 regT0 = (0x123U << 18);

      // 3. T1 Wort aufbauen:
      // DLC = 15 (repräsentiert 64 Bytes in CAN FD)
      // BRS (Bit Rate Switching) = 1 (optional, für schnellere Datenphase)
      // FDF (FD Format) = 1 (CAN FD Format)
      u32 regT1 = (15U << 16) |          // DLC = 15 -> 64 Bytes
                  (1U << 20) |           // BRS = Bit Rate Switch aktivieren
                  (1U << 21);            // FDF = CAN FD Rahmen

      // 4. Header-Wörter in das Message RAM schreiben
      txBufferAddr[0] = regT0;
      txBufferAddr[1] = regT1;

      // 5. 64 Byte Nutzdaten in das Message RAM kopieren (Wortweise schreiben)
      volatile u32 *pTxData = &txBufferAddr[2];
      u32 *pSrcData = (uint32_t *)pData;

      for (u32 i = 0; i < 16; i++) {
          pTxData[i] = pSrcData[i];
      }

      // 6. Sendeaufforderung über das TXBAR-Register setzen
      // Bit 'bufferIndex' im Tx Buffer Add Request Register aktivieren
      fdcan->TXBAR = (1U << bufferIndex);
  }

  bool bRun() override
  {
    // return True to signal finished
    return cComponent::bRun();
  };
};




