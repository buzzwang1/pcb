
#include "main.h"

class cAchseHwX: public cCncAchseHW
{
public:

  bool isRefSwitch() override 
  { 
    return False; 
  };

  // 0 = ; 1 = 
  void vSetDir(u8 lu8Dir) override 
  {
  };

  // 0..50000Hz
  void vSetSpeed_Hz(u16 lu16Speed) override 
  {
  };
};

class cAchseHwY : public cCncAchseHW
{
public:

  bool isRefSwitch() override
  {
    return False;
  };

  // 0 = ; 1 = 
  void vSetDir(u8 lu8Dir) override
  {
  };

  // 0..50000Hz
  void vSetSpeed_Hz(u16 lu16Speed) override
  {
  };
};

class cAchseHwZ : public cCncAchseHW
{
public:

  bool isRefSwitch() override
  {
    return False;
  };

  // 0 = ; 1 = 
  void vSetDir(u8 lu8Dir) override
  {
  };

  // 0..50000Hz
  void vSetSpeed_Hz(u16 lu16Speed) override
  {
  };
};


cRingBufDynT<u8, u16> mcUartDataIn(128);
cRingBufDynT<u8, u16> mcUartDataOut(128);


cAchseHwX mcAchseHwX;
cAchseHwY mcAchseHwY;
cAchseHwZ mcAchseHwZ;

cCncAchse mcAX(mcAchseHwX);
cCncAchse mcAY(mcAchseHwY);
cCncAchse mcAZ(mcAchseHwZ);

cCnc mcCnc(mcAchseHwX, mcAchseHwY, mcAchseHwZ, &mcUartDataOut);
cPal mcPal(&mcCnc, &mcUartDataIn, &mcUartDataOut);

void main(void)
{      
  printf("Test \n");

  mcUartDataIn.put((u8*)"@07\r", cStrTools::uixStrLen("@07\r"));
  mcUartDataIn.put((u8*)"@0D1000,2000,3000\r", cStrTools::uixStrLen("@0D1000,2000,3000\r"));
  mcUartDataIn.put((u8*)"@0R7\r", cStrTools::uixStrLen("@0R7\r"));

  bool lbFinished;

  do
  {
    lbFinished = False;
    if (mcCnc.menProcessedCmd == mcCnc.enCmdNop)
    {
      lbFinished = mcPal.bDoProcess();
    }

    mcCnc.DoProcess1ms();

  } while (!lbFinished);
  
}


