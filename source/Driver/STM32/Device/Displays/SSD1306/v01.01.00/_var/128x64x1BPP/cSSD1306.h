
#ifndef SSD1306_H
#define SSD1306_H


/**
 * @addtogroup TM_STM32F4xx_Libraries
 * @{
 */

/**
 * @defgroup TM_SSD1306
 * @brief    Library for 128x64 SSD1306 I2C LCD - http://stm32f4-discovery.com/2015/05/library-61-ssd1306-oled-i2c-lcd-for-stm32f4xx
 * @{
 *
 * This SSD1306 LCD uses I2C for communication
 *
 * Library features functions for drawing lines, rectangles and circles.
 *
 * It also allows you to draw texts and characters using appropriate functions provided in library.
 *
 * \par Default pinout
 *
@verbatim
SSD1306    |STM32F4xx    |DESCRIPTION

VCC        |3.3V         |
GND        |GND          |
SCL        |PB6          |Serial clock line
SDA        |PB7          |Serial data line
*/

#include "TYPEDEF.h"
#include "MemTools.h"
#include "cI2C.h"


#define cSSD1306_WIDTH            128
#define cSSD1306_HEIGHT           64

#define cSSD1306_MODEL_A          0
#define cSSD1306_MODEL_B          38
#define cSSD1306_Buffer_SIZE      cSSD1306_WIDTH * cSSD1306_HEIGHT / 8

#define mcSSD1306_vPxl2Byte(li16X, li16Y)  (((li16Y) * cSSD1306_WIDTH) + (li16X))
#define mcSSD1306_vSetPixel(li16X, li16Y)  mpaui8Buffer[mcSSD1306_vPxl2Byte(li16X, li16Y) >> 3] |= mcSSD1306_nPXL[mcSSD1306_vPxl2Byte(li16X, li16Y) & 7]


class cSSD1306 : public cComNode
{
  public:

  /* Private SSD1306 structure */
  typedef struct {
      uint16 CurrentX;
      uint16 CurrentY;
      uint8 Inverted;
  } cSSD1306_t;
  cSSD1306_t SSD1306;

  typedef enum
  {
    enIdle  = 0,
    enInit,
    enInit2,
    enInit3,
    enUpdate,
    enUpdate1,
  }tenSSD1306_Modes;

  uint8 mpaui8BufferTx[cSSD1306_WIDTH * cSSD1306_HEIGHT / 8 + 1];
  uint8 *mpaui8Buffer;

  tenSSD1306_Modes   menMode;

  cComNodeMaster*    mI2C;

  cComMsgS<u16, 8> mCmdMsg;
  cComMsg<u16>     mDataMsg;

  u16        muRestart_ms;
  uint8      mui8Offset;

  bool mbCmdUpdate;
  bool mbCmdInit;

  static const u8 mcSSD1306_nPXL[8];


  cSSD1306(cComNodeMaster* lpcI2C, uint8 lui8Adr, uint8 lui8Offset)
    : mCmdMsg(cComNode::enIsTx),
      mDataMsg(cComNode::enIsTx, NULL, 0, 0)
  {
    mI2C   = lpcI2C;
    mAdr   = lui8Adr;
    mui8Offset = lui8Offset;
    muRestart_ms = 0;
  }

  cSSD1306(cComNodeMaster* lpcI2C, uint8 lui8Adr)
   : cSSD1306(lpcI2C, lui8Adr, 0)
  {
  }

  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);
    mpaui8BufferTx[0] = 0x40; // Data Mode
    mpaui8Buffer      = mpaui8BufferTx + 1;

    /* Set default values */
    SSD1306.Inverted = 0;
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;

    vRestart();
  }

  void vRestart()
  {
    menMode = enIdle;
    mStatus.IsInit = false;
    mStatus.IsError = false;
    mbCmdInit = true;
    mControl.StartRequest = true;
  }

  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override
  {
    UNUSED(lenError);
    UNUSED(lenState);
    cComNode::vComError(lenError, lenState);
    muRestart_ms = 100;
  };

  void vTick(u16 lu16Time_ms) override
  {
    if (muRestart_ms)
    {
      if (muRestart_ms > lu16Time_ms)
      {
        muRestart_ms -= lu16Time_ms;
      }
      else
      {
        muRestart_ms = 0;
        vRestart();
      }
    }
  };

  void vComStart(cComNode::tenEvent lenEvent)
  {
    UNUSED(lenEvent);

    if (menMode == enIdle)
    {
      if (mbCmdInit)
      {
        mbCmdInit = false;
        menMode = enInit;
        vComDone();
      }
      else if (mbCmdUpdate)
      {
        mbCmdUpdate = false;
        menMode = enUpdate;
        vComDone();
      }
    }
    mControl.StartRequest = (uint8)(mbCmdInit | mbCmdUpdate);
  }

  void vComDone()
  {
    static const u8 mpaui8InitSequence[] =
    {
      0x00, //Command Data Stream
      0xAE, //display off

      0xA8, //--set multiplex ratio(1 to 64)
        0x3F, //

      0x20, //Set Memory Addressing Mode
        0x00, //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET,11,Invalid

      0x40, //--set start line address
      0xD3, //-set display offset
        0x00, //-not offset
      
      0xA1, //--set segment re-map 0 to 127
      0xC8, //Set COM Output Scan Direction

      //0xA0, 0xC0, 180° gedreht zu A1/C8

      0xDA, //--set com pins hardware configuration
        0x12,

      0x81, //--set contrast control register
       0x7F,
      0xA4, //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
      0xA6, //--set normal display A6 inverse color A7

      0xD5, //--set display clock divide ratio/oscillator frequency
        0x80, //--set divide ratio

      0xD9, //--set pre-charge period
        0xC2, //

      0xDB, //--set vcomh
        0x20, //0x20,0.77xVcc
      0x8D, // Enable charge pump regulator
       0x14,

      0x2E,   // DEACT_SCROLL

      0xD3, //Set vertical shift by COM from 0d~63d
        0x00,

      0x21, 0, 127, //Specifies column start address and end address / accord. mARTi-null #16
      0x22, 0,   7, //Specifies page start address and end address / accord. mARTi-null #16

     0xAF
    };

    bool lbLoop = True;
    while (lbLoop)
    {
      lbLoop = False;
      switch (menMode)
      {
        case enIdle:
          break;

        case enInit:
          menMode = enInit2;
          Clear(0xFF);

          mDataMsg.UseMem((u8*)mpaui8InitSequence, sizeof(mpaui8InitSequence));
          mI2C->vStartMsg(&mDataMsg);
          break;
        case enInit2:
          // Locken, damit es ohne Unterbrechung weitergeht
          mI2C->vLock();
          menMode = enInit3;
          mCmdMsg.Set(0x80); //Command Mode
          mCmdMsg.Add(0x40 + mui8Offset);
          mI2C->vStartMsg(&mCmdMsg);
          break;
        case enInit3:
          // Locken, damit es ohne Unterbrechung weitergeht
          mI2C->vLock();
          menMode = enUpdate;
          lbLoop = True;
          break;

        case enUpdate:
          menMode = enUpdate1;
          mI2C->vLock();
          mDataMsg.UseMem(mpaui8BufferTx, cSSD1306_WIDTH * cSSD1306_HEIGHT / 8 + 1);
          mI2C->vStartMsg(&mDataMsg);
          break;
        case enUpdate1:
          menMode = enIdle;
          break;
      }
    }
  }


  int8 i8Setup()
  {
    int li8Error = FRET_OK;

    mbCmdInit   = true;
    mControl.StartRequest = 1;

    return li8Error;
  }


  void Update(void)
  {
    mbCmdUpdate = true;
    mControl.StartRequest = 1;
  }

  void Clear(uint8 lui8Col)
  {
    cMemTools::vMemSet(mpaui8Buffer, lui8Col, cSSD1306_WIDTH * cSSD1306_HEIGHT / 8);
  }

  void vShowScreen(uint8 *lpaui8GfxBuf)
  {
    uint16 lui16SrcIdx;
    uint8  lui8SrcValue;
    uint16 lui16DstIdx1;
    uint16 lui16DstIdx2;
    uint16 lui16DstIdx3;
    uint16 lui16DstIdx4;

    lui16DstIdx1 = 0;
    lui16DstIdx2 = 7;
    lui16DstIdx3 = 7;
    lui16DstIdx4 = 0;

    this->Clear(0);

    for (lui16SrcIdx = 0; lui16SrcIdx < cSSD1306_WIDTH * cSSD1306_HEIGHT / 8; lui16SrcIdx++)
    {
      lui8SrcValue = lpaui8GfxBuf[lui16SrcIdx];

      if (lui8SrcValue)
      {
        if (lui8SrcValue & 128) mcSSD1306_vSetPixel(lui16DstIdx2 +  0, lui16DstIdx4);
        if (lui8SrcValue &  64) mcSSD1306_vSetPixel(lui16DstIdx2 +  8, lui16DstIdx4);
        if (lui8SrcValue &  32) mcSSD1306_vSetPixel(lui16DstIdx2 + 16, lui16DstIdx4);
        if (lui8SrcValue &  16) mcSSD1306_vSetPixel(lui16DstIdx2 + 24, lui16DstIdx4);
        if (lui8SrcValue &   8) mcSSD1306_vSetPixel(lui16DstIdx2 + 32, lui16DstIdx4);
        if (lui8SrcValue &   4) mcSSD1306_vSetPixel(lui16DstIdx2 + 40, lui16DstIdx4);
        if (lui8SrcValue &   2) mcSSD1306_vSetPixel(lui16DstIdx2 + 48, lui16DstIdx4);
        if (lui8SrcValue &   1) mcSSD1306_vSetPixel(lui16DstIdx2 + 56, lui16DstIdx4);
      }

      lui16DstIdx2 += 64;
      lui16DstIdx1++;

      if (lui16DstIdx2 >= cSSD1306_WIDTH)
      {
        if ((lui16DstIdx4 & 7) == 7)
        {
          lui16DstIdx4 -= 8;

          if (lui16DstIdx3 == 0)
          {
            lui16DstIdx1  = 0;
            lui16DstIdx2  = 7;
            lui16DstIdx3  = 7;
            lui16DstIdx4 += 8;
          }
          else
          {
            lui16DstIdx3 -= 1;
          }

        }
        lui16DstIdx4 += 1;
        lui16DstIdx2  = lui16DstIdx3;
      }
    }
  }
};


#endif
