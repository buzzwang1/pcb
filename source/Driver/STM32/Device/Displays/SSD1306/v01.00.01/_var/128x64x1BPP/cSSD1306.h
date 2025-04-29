
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
#include "cI2C.h"


#define cSSD1306_WIDTH            128
#define cSSD1306_HEIGHT           64

#define cSSD1306_MODEL_A          0
#define cSSD1306_MODEL_B          38
#define cSSD1306_Buffer_SIZE      cSSD1306_WIDTH * cSSD1306_HEIGHT / 8


class cSSD1306 : public cComNode
{
  public:

  /* Private SSD1306 structure */
  typedef struct {
      uint16 CurrentX;
      uint16 CurrentY;
      uint8 Inverted;
      uint8 Initialized;
  } cSSD1306_t;
  cSSD1306_t SSD1306;

  typedef enum
  {
    enIdle  = 0,
    enInit,
    enInit2,
    enInit3,
    enInit4,
    enInit5,
    enInit6,
    enInit7,
    enUpdate,
    enUpdatePage
  }tenSSD1306_Modes;

  uint8 mpaui8BufferTx[cSSD1306_WIDTH * cSSD1306_HEIGHT / 8 + 6];
  uint8 *mpaui8Buffer;

  tenSSD1306_Modes   menMode;
  uint8              mIdx;

  cI2cMaster*           mI2C;

  cComDatMsg mpcWriteCmd;
  cComDatMsg mpcWriteData;

  uint8      mui8WriteCmd[6];
  uint8      mui8Offset;

  bool mbCmdUpdate;
  bool mbCmdInit;

  cSSD1306(cI2cMaster* lpcI2C, uint8 lui8Adr, uint8 lui8Offset);
  cSSD1306(cI2cMaster* lpcI2C, uint8 lui8Adr);
  ~cSSD1306();

  void vComStart(cComNode::tenEvent lenEvent) override;
  void vComDone() override;

  int8 i8Setup();

  void Update(void);
  void vWriteCmd(uint8 lui8Cmd);

  void Clear(uint8 lui8Col);
  void vShowScreen(uint8 *lpaui8GfxBuf);
};


#endif
