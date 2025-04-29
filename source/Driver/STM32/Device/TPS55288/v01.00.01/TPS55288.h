#ifndef __TPS55288_H__
#define __TPS55288_H__

#include "Typedef.h"
#include "cI2C.h"



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define TPS55288_I2C_ADDRESS_CONF_0       (0x74 << 1)
  #define TPS55288_I2C_ADDRESS_CONF_1       (0x75 << 1)
  #define TPS55288_I2C_ADDRESS_DEFAULT       TPS55288_I2C_ADDRESS_CONF_0
/*=========================================================================*/


  #define TPS55288_VOUT_LOW                         (0x00) // Reference Voltage
  #define TPS55288_VOUT_HIGH                        (0x01) // Reference Voltage
  #define TPS55288_IOUT_LIMIT                       (0x02) // IOUT_LIMIT
  #define TPS55288_VOUT_SR                          (0x03) // Slew Rate
  #define TPS55288_VOUT_FS                          (0x04) // Feedback voltage

  #define TPS55288_CDC                              (0x05) // Cable Compensation
  #define TPS55288_MODE                             (0x06) // Mode

  #define TPS55288_STATUS                           (0x07)


class cTPS55288 : public cComNode
{
  public:

  struct tstRegIOutLimit
  {
    u8 Limit : 7;
    u8 En    : 1;
  };

  struct tstRegSR
  {
    u8 Sr : 2;
    u8 Res2 : 2;
    u8 OcpDelay : 2;
    u8 Res1 : 2;
  };

  struct tstRegFS
  {
    u8 IntFb : 2;
    u8 Res   : 5;
    u8 Fb    : 1;
  };

  struct tstRegCDC
  {
    u8 Cdc : 3;       // Compensation for voltage droop over the cable
    u8 CdcOption : 1; // Select the cable voltage droop compensation approach.
    u8 Res : 1;       // 
    u8 OvpMask : 1;   // Overvoltage mask
    u8 OcpMask : 1;   // Overcurrent mask
    u8 ScMask : 1;    // Short circuit mask
  };

  struct tstRegMODE
  {
    u8 MODE : 1;   // Mode control approach
    u8 PFM : 1;    // Select operating mode at light load condition
    u8 I2CADD : 1; // I2C address
    u8 VCC : 1;    // VCC option
    u8 DISCHG : 1; // Output discharge
    u8 HICCUP : 1; // Hiccup mode
    u8 FSWDBL : 1; // Switching frequency doubling in buck-boost mode
    u8 OE : 1;     // Output enable
  };

  struct tstRegSTATUS
  {
    u8 Status : 2; // Operating status
    u8 Res : 3;
    u8 OVP : 1;    // Overvoltage protection
    u8 OCP : 1;    // Overcurrent protection
    u8 SCP : 1;    // Short circuit protection
  };

  struct tstRegs
  {
    u8              u8VoutL;
    u8              u8VoutH;
    tstRegIOutLimit stIOutLimit;
    tstRegSR        stRegSR;
    tstRegFS        stRegFS;
    tstRegCDC       stRegCDC;
    tstRegMODE      stRegMODE;
    tstRegSTATUS    stRegSTATUS;
  };

  union tunRegs
  {
    u8      u8Regs[sizeof(tstRegs)];
    tstRegs stRegs;
  };

  typedef enum
  {
    enCmdIdle  = 0,
    enCmdInit,
    enCmdInit2,
    enCmdInit3,
    enCmdInit4,
    enCmdInit5,
    enCmdSetVoltage,
    enCmdSetVoltage2,
    enCmdSetVoltage3,
    enCmdSetVoltage4,
    enCmdSetCurrent,
    enCmdSetCurrent2,
    enCmdSetMode1,
    enCmdSetMode2
  }tenTPS55288_Cmd;


  tunRegs      mRegs;
  uint16       mRegIdx;

  u16          mu16Vout_mV;
  u16          mu16ILim_mA;

  cI2cMaster*  mI2C;

  cComDatMsg mpcMsgWrite;
  cComDatMsg mpcMsgRead;

  tenTPS55288_Cmd     menCmd;

  bool mbInit;
  bool mbCmdSetVoltage;
  bool mbCmdSetCurrent;
  bool mbCmdSetMode1;



  cTPS55288(cI2cMaster* lpcI2C, uint8 lui8Adr);
  #if _WIN32
    ~cTPS55288();
  #endif // _WIN32

  int8  i8Setup();

  uint32 SizeOf() {return 0;};
  void  vComStart(cComNode::tenEvent lenEvent) override;
  void  vComDone() override;

  void  vStartWriteReg(u8 lu8Idx);
  void  vStartReadReg(u8 lu8Idx);

  int8  i8SetVoltage_mV(u16 lu16Value, bool lbForce = False);
  int8  i8SetCurrent_mA(u16 lu16Value, bool lbForce = False);

  int8  i8SetMode(u8 lu8Value);
};

#endif // __TPS55288_H__
