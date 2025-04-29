#ifndef __HTU21D_H__
#define __HTU21D_H__

#include "Typedef.h"
#include "cI2C.h"

/*
Der Sensor braucht etwa 50ms bei Temp und 20ms bei Hum um einen Wert zu liefern

Bei Hold:
Dann wird von Sensor der I2C für diese Zeit blockiert

Bei NoHold:
Gibt der Sensor NACK zurück, bis der Wert zur Verfügung steht.
Der Wert muss gepollt werden
*/



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define HTU21D_DEFAULT_ADDRESS       (0x40 << 1)
  #define HTU21D_I2C_ADDRESS_DEFAULT   HTU21D_DEFAULT_ADDRESS
/*=========================================================================*/

  #define HTU21D_RA_TEMPERATURE_HOLD      0xE3
  #define HTU21D_RA_HUMIDITY_HOLD         0xE5
  #define HTU21D_RA_TEMPERATURE_NOHOLD    0xF3
  #define HTU21D_RA_HUMIDITY_NOHOLD       0xF5
  #define HTU21D_RESET                    0xFE
  #define HTU21D_WRITE_USER_REGISTER      0xE6
  #define HTU21D_READ_USER_REGISTER       0xE7


class cHTU21D : public cComNode
{
  public:


  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdWriteReset,
    enCmdWriteReset2,
    enCmdReadUser,
    enCmdReadUser2,
    enCmdReadTemperatureHold,
    enCmdReadTemperatureHold2,
    enCmdReadTemperatureNoHold,
    enCmdReadTemperatureNoHold2,
    enCmdReadTemperatureNoHold3,
    enCmdReadHumidityHold,
    enCmdReadHumidityHold2,
    enCmdReadHumidityNoHold,
    enCmdReadHumidityNoHold2,
    enCmdReadHumidityNoHold3
  }tenHTU21D_Cmd;

  uint16        mui16Temperature_digit;
  uint16        mui16Humiditiy_digit;
  uint8         mui8User_digit;

  cI2cMaster*   mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenHTU21D_Cmd      menCmd;
  uint8              mIdx;


  bool mbInit;
  bool mbWriteReset;
  bool mbReadUser;
  bool mbReadTemperatureHold;
  bool mbReadTemperatureNoHold;
  bool mbReadTemperatureNoHoldRepeat;
  bool mbReadHumidityHold;
  bool mbReadHumidityNoHold;
  bool mbReadHumidityNoHoldRepeat;


  cHTU21D(cI2cMaster* lpcI2C, uint8 lui8Adr);
  ~cHTU21D();

  int8  i8Setup();

  void vComStart(cComNode::tenEvent lenEvent) override;
  void vComDone() override;
  void vComError(cComNode::tenError lenError, cComNode::tenState lenState) override;

  int8  i8ReadUser_digit();
  int8  i8ReadTemperatureHold_digit();
  int8  i8ReadHumidityHold_digit();

  //For NoHold the vTick10ms must be called
  void  vTick10ms(void);
  int8  i8ReadTemperatureNoHold_digit();
  int8  i8ReadHumidityNoHold_digit();

  /* in 0.01 °C*/
  int32 i32CalcTemperature();

  /* in 0.01 %*/
  int32 i32CalcHumidity();
};

#endif // __INA291_H__
