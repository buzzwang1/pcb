/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2015
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */

#include "cssd1306.h"
#include "MemTools.h"

/* Absolute value */
#define ABS(x)   ((x) > 0 ? (x) : -(x))

cSSD1306::cSSD1306(cI2cMaster* lpcI2C, uint8 lui8Adr, uint8 lui8Offset)
{
  mI2C   = lpcI2C;
  mAdr   = lui8Adr;

  mpcWriteCmd.vMemAlloc(40, 0);

  menMode     = enIdle;
  mStatus.IsInit    = false;
  mbCmdUpdate = true;
  mbCmdInit   = true;
  mStatus.IsStartRequested = (uint8)(mbCmdInit & mbCmdUpdate);

  mpaui8BufferTx[0] = 0x40; // Data Mode
  mpaui8Buffer      = mpaui8BufferTx + 1;
  mui8Offset        = lui8Offset;

  /* Set default values */
  SSD1306.Inverted = 0;
  SSD1306.CurrentX = 0;
  SSD1306.CurrentY = 0;

  /* Initialized OK */
  SSD1306.Initialized = 1;

  mI2C->vAddSlave((cComNode*)this);
}

cSSD1306::cSSD1306(cI2cMaster* lpcI2C, uint8 lui8Adr)
 : cSSD1306(lpcI2C, lui8Adr, 0)
{
}

cSSD1306::~cSSD1306()
{
  //while (menMode != enIdle) {};
  //mpcWriteCmd.vMemFree();
  //mI2C->vRemoveSlave((cComNode*)this);
}


void cSSD1306::vComStart(cComNode::tenEvent lenEvent)
{
  UNUSED(lenEvent);

  if (IsError())
  {
    mStatus.IsError  = false;
    menMode = enIdle;
    mStatus.IsInit  = false;
    mbCmdInit = true;
  }

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
  mStatus.IsStartRequested = (uint8)(mbCmdInit | mbCmdUpdate);
}

void cSSD1306::vComDone()
{
  static const uint8 mpaui8InitSequence[] =
  {
    0x00, //Command Data
    0xAE, //display off

    0x20, //Set Memory Addressing Mode
      0x00, //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET,11,Invalid

    0xB0, //Set Page Start Address for Page Addressing Mode,0-7
    0xC8, //Set COM Output Scan Direction

    0x00, //---set low column address
    0x10, //---set high column address
    0x40, //--set start line address
    0x81, //--set contrast control register
     0x7F,

    0xA1, //--set segment re-map 0 to 127
    0xA6, //--set normal display A6 inverse color A7
    0xA8, //--set multiplex ratio(1 to 64)
      0x3F, //

    0xA4, //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    0xD3, //-set display offset
      0x00, //-not offset
    0xD5, //--set display clock divide ratio/oscillator frequency
      0x80, //--set divide ratio

    0xD9, //--set pre-charge period
      0x22, //

    0xDA, //--set com pins hardware configuration
      0x12,

    0xDB, //--set vcomh
      0x20, //0x20,0.77xVcc

    0xD3, //Set vertical shift by COM from 0d~63d
      0x00,

    0x8D, // Enable charge pump regulator
     0x14,

   0xAF
  };

  switch (menMode)
  {
    case enIdle:
      break;

    case enInit:
      mIdx    = 0;
      menMode = enInit2;
      Clear(0xFF);

      mpcWriteCmd.vSetAll((uint8*)mpaui8InitSequence, sizeof(mpaui8InitSequence), NULL, 0);
      mpcWriteCmd.vStart();
      mI2C->vStartMsg(&mpcWriteCmd);
      break;

    case enInit2:
      menMode     = enIdle;
      mbCmdUpdate = true;
      vComStart(cComNode::tenEvent::enEvPrepareToSendData);
      mStatus.IsInit      = true;
      break;

    case enUpdate:
      menMode = enUpdatePage;
      mIdx   = 0;
      mpcWriteCmd.vSetAll(mui8WriteCmd, 2, NULL, 0);
      mpcWriteCmd.cTxData.mpu8Data[0] = 0x00; //Command Mode
      mpcWriteCmd.cTxData.mpu8Data[1] = 0x40 + mui8Offset;
      mpcWriteCmd.vStart();
      mI2C->vStartMsg(&mpcWriteCmd);
      break;
    case enUpdatePage:
      if (mIdx == 0)
      {
        mIdx++;
        mpcWriteData.vSetAll(mpaui8BufferTx, cSSD1306_WIDTH * cSSD1306_HEIGHT / 8 + 1, NULL, 0);
        mpcWriteData.vStart();
        mI2C->vStartMsg(&mpcWriteData);
      }
      else
      {
        menMode = enIdle;
      }

      break;
  }
}


int8 cSSD1306::i8Setup()
{
  int li8Error = FRET_OK;

  mbCmdInit   = true;
  mStatus.IsStartRequested = 1;

  return li8Error;
}


void cSSD1306::Update(void)
{
  mbCmdUpdate = true;
  mStatus.IsStartRequested = 1;
}


void cSSD1306::vWriteCmd(uint8 lui8Cmd)
{
  mpcWriteCmd.vSetAll(mui8WriteCmd, 2, NULL, 0);
  mpcWriteCmd.cTxData.mpu8Data[0] = 0x00;
  mpcWriteCmd.cTxData.mpu8Data[1] = lui8Cmd;
  mpcWriteCmd.vStart();
  mI2C->vStartMsg(&mpcWriteCmd);
}

void cSSD1306::Clear(uint8 lui8Col)
{
  cMemTools::vMemSet(mpaui8Buffer, lui8Col, cSSD1306_WIDTH * cSSD1306_HEIGHT / 8);
}


const uint8 mcSSD1306_nPXL[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

#define mcSSD1306_vPxl2Byte(li16X, li16Y)  (((li16Y) * cSSD1306_WIDTH) + (li16X))
#define mcSSD1306_vSetPixel(li16X, li16Y)  mpaui8Buffer[mcSSD1306_vPxl2Byte(li16X, li16Y) >> 3] |= mcSSD1306_nPXL[mcSSD1306_vPxl2Byte(li16X, li16Y) & 7]


void cSSD1306::vShowScreen(uint8 *lpaui8GfxBuf)
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

