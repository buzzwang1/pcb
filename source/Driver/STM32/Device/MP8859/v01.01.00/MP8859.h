#ifndef __MP8859_H__
#define __MP8859_H__

#include "Typedef.h"
#include "cI2C.h"



/*=========================================================================
  I2C ADDRESS/BITS
  -----------------------------------------------------------------------*/

  #define MP8859_I2C_ADDRESS_CONF_0       (0x60)     // ADD Voltage  <25%Vcc 
  #define MP8859_I2C_ADDRESS_CONF_1       (0x62)     // ADD Voltage  25%Vcc-50%Vcc 
  #define MP8859_I2C_ADDRESS_CONF_2       (0x64)     // ADD Voltage  50%Vcc-75%Vcc
  #define MP8859_I2C_ADDRESS_CONF_3       (0x66)     // ADD Voltage  >75%Vcc 
  #define MP8859_I2C_ADDRESS_DEFAULT       MP8859_I2C_ADDRESS_CONF_0
/*=========================================================================*/


  #define MP8859_VOUT_LOW                         (0x00)
  #define MP8859_VOUT_HIGH                        (0x01)
  #define MP8859_VOUT_GO                          (0x02)
  #define MP8859_IOUT_LIM                         (0x03)

  #define MP8859_CTL1                             (0x04)
  #define MP8859_CTL2                             (0x05)

  #define MP8859_STATUS                           (0x09)
  #define MP8859_IRQ                              (0x0A)
  #define MP8859_MASK                             (0x0B)

  #define MP8859_ID1                              (0x0C)
  #define MP8859_MFR_ID                           (0x27)
  #define MP8859_DEV_ID                           (0x28)
  #define MP8859_IC_REV                           (0x29)


class cMP8859 : public cComNode
{
  public:

  struct tstRegVoutGo
  {
    u8 Go_Bit : 1;
    u8 PgDelayEn : 1;
    u8 Res:6;
  };

  struct tstRegCTL1
  {
    u8 Res : 2;
    u8 Freq : 2;
    u8 Mode : 1;
    u8 DischgEn : 1;
    u8 HicCupOcpOvp : 1;
    u8 En : 1;
  };

  struct tstRegCTL2
  {
    u8 Res : 4;
    u8 Ss : 2;
    u8 LineDropComp : 2;
  };

  struct tstRegStatus
  {
    u8 Cc_Cv : 1;
    u8 Otw : 1;
    u8 Otp : 1;
    u8 Pg : 1;
  };

  struct tstRegIrq
  {
    u8 PgRising : 1;
    u8 OtWarnExit : 1;
    u8 OTemppExit : 1;
    u8 UVP_Falling : 1;
    u8 OC_Recover : 1;
    u8 OC_Enter : 1;
    u8 OtWarnEnter : 1;
    u8 OTemppEnter : 1;
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
    enCmdSetCtrl1,
    enCmdSetCtrl12
  }tenMP8859_Cmd;

  u16                mu16Vout;
  tstRegVoutGo       mstVoutGo;
  u8                 mILim;
  tstRegCTL1         mstCTL1;
  tstRegCTL2         mstCTL2;
  tstRegStatus       mstStatus;
  tstRegIrq          mstIrq;

  u8                 mID;
  u8                 mMfrID;
  u8                 mDevID;
  u8                 mIcID;

  uint16             mRegIdx;

  cComNodeMaster*    mI2C;

  cComMsgS<u16, 8> mpcMsgWrite;
  cComMsgS<u16, 8> mpcMsgRead;

  tenMP8859_Cmd     menCmd;

  u16 muRestart_ms;

  bool mbInit;
  bool mbCmdSetVoltage;
  bool mbCmdSetCurrent;
  bool mbCmdSetCtrl1;

  cMP8859(cI2cMaster* lpcI2C, uint8 lui8Adr);

  void vInit(bool lbStart) override
  {
    UNUSED(lbStart);
    vRestart();
  }

  void vRestart()
  {
    menCmd = enCmdIdle;
    mStatus.IsInit = false;
    mStatus.IsError = false;
    //i8Setup(MP8859_Mode_ShuntAndBusVoltageContinuous,
    //        MP8859_ShuntConversionTime_1100uS,
    //        MP8859_BusConversionTime_1100uS,
    //        MP8859_AveragingMode1,
    //        True, True, True);
    i8Setup();

    mControl.StartRequest = mbInit;
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

  int8  i8Setup();

  uint32 SizeOf() {return 0;};
  void  vComStart(cComNode::tenEvent lenEvent) override;
  void  vComDone() override;

  void  vSetReg(u8 lu8Idx, u8 lu8Value);
  u8    u8GetReg(u8 lu8Idx);

  void  vStartWriteReg(u8 lu8Idx);
  void  vStartReadReg(u8 lu8Idx);

  int8  i8SetVoltage_mV(u16 lu16Value, bool lbForce = False);
  int8  i8SetCurrent_mA(u16 lu16Value, bool lbForce = False);

  int8  i8SetCtrl1(u8 lu8Value);
};

#endif // __MP8859_H__
