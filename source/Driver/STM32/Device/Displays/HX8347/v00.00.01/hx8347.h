
#ifndef _HX8347_H
#define _HX8347_H


#include "HX8347HW.h"

#define HX8347A_WIDTH            (120)
#define HX8347A_HEIGHT           (176)

class cHx8347 : public cHx8347_HW
{
public:

  typedef enum {
    nSTART_ID = 0x70,
    nSTART_BS0 = 0x00,
    nSTART_RS = 0x02,
    nSTART_RW = 0x01,

    nID = 0,
    nSTART_WRITEIDX = (nSTART_ID | nSTART_BS0),
    nSTART_WRITEREG = (nSTART_ID | nSTART_BS0 | nSTART_RS),
    nDATA           = (nSTART_ID | nSTART_BS0 | nSTART_RS),
    nREGISTER       = (nSTART_ID | nSTART_BS0),

  } tenConsts;

  uint16          mui16PosX1;
  uint16          mui16PosX2;
  uint16          mui16PosY1;
  uint16          mui16PosY2;

  cHx8347()
  {
    vInit();
  }

  cHx8347(bool bDisplayOn)
  {
    vInit(bDisplayOn);
  }

  void vInit(bool bDisplayOn)
  {
    mui16PosX1 = UINT16MAX;
    mui16PosX2 = UINT16MAX;
    mui16PosY1 = UINT16MAX;
    mui16PosY2 = UINT16MAX;

    vPwrEnable();

    vSerial();

    /*Reset*/
    vRstDisable();
    cClockInfo::Delay_ms(50);
    vRstEnable();
    cClockInfo::Delay_ms(50);

    //Gamma for CMO 2.4"
    vCmd(0x46, 0x91);
    vCmd(0x47, 0x11);
    vCmd(0x48, 0x01);
    vCmd(0x49, 0x67);
    vCmd(0x4a, 0x02);
    vCmd(0x4b, 0x57);
    vCmd(0x4c, 0x01);
    vCmd(0x4d, 0x67);
    vCmd(0x4e, 0x82);
    vCmd(0x4f, 0x40);
    vCmd(0x50, 0x20);
    vCmd(0x51, 0x84);

    //120x176 window setting
    /*vCmd(0x02,0x00);       // Column address start2
    vCmd(0x03,0x00);       // Column address start1
    vCmd(0x04,0x00);       // Column address end2
    vCmd(0x05,0xEF);       // Column address end1
    vCmd(0x06,0x00);       // Row address start2
    vCmd(0x07,0x00);       // Row address start1
    vCmd(0x08,0x01);       // Row address end2
    vCmd(0x09,0x3F);       // Row address end1*/

    cClockInfo::Delay_ms(1);

    // Display Setting
    vCmd(0x01, 0x02); // IDMON=0, INVON=0, NORON=1, PTLON=0
    vCmd(0x16, 0x00); // MY=0, MX=0, MV=0, ML=1, BGR=0, TEON=0   0048

    vCmd(0x72, 0x00); // - - - - - - TRI[0:1]
    vCmd(0x70, 0x0E); // TEMODE=0,TE=1,CSEL[2:0]=110

    vCmd(0x23, 0x95); // N_DC=1001 0101
    vCmd(0x24, 0x95); // PI_DC=1001 0101
    vCmd(0x25, 0xFF); // I_DC=1111 1111

    vCmd(0x27, 0x02); // N_BP=0000 0010
    vCmd(0x28, 0x02); // N_FP=0000 0010
    vCmd(0x29, 0x02); // PI_BP=0000 0010
    vCmd(0x2a, 0x02); // PI_FP=0000 0010
    vCmd(0x2C, 0x02); // I_BP=0000 0010
    vCmd(0x2d, 0x02); // I_FP=0000 0010

    vCmd(0x3a, 0x01); // N_RTN=0000, N_NW=001    0001
    vCmd(0x3b, 0x01); // P_RTN=0000, P_NW=001
    vCmd(0x3c, 0xf0); // I_RTN=1111, I_NW=000
    vCmd(0x3d, 0x00); // DIV=00
    cClockInfo::Delay_ms(1);

    vCmd(0x35, 0x38); // EQS=38h
    vCmd(0x36, 0x78); // EQP=78h
    vCmd(0x3E, 0x38); // SON=38h
    vCmd(0x40, 0x0F); // GDON=0Fh
    vCmd(0x41, 0xF0); // GDOFF

    cClockInfo::Delay_ms(1);

    // Power Supply Setting
    vCmd(0x19, 0x87); // CADJ=0110,CUADJ=011(FR:60Hz), OSD_EN=1
    vCmd(0x93, 0x0F); // RADJ=1010, 125%, Frame Rate = 125%  60Hz = 75Hz

    vCmd(0x57, 0x02); // Test_Mode Enable
    vCmd(0x95, 0x01); // SET DISPLAY CLOCK AND PUMPING CLOCK TO SYNCHRONIZE
    vCmd(0x57, 0x00); // Test_Mode Disable
    cClockInfo::Delay_ms(1);


    vCmd(0x20, 0x40); // BT=0100
    vCmd(0x1D, 0x01); // VC1=001   0007
    vCmd(0x1E, 0x00); // VC3=000
    vCmd(0x1F, 0x0A); // VRH=1010


    //VCOM SETTING
    vCmd(0x44, 0x6B); // VCM=1101111
    vCmd(0x45, 0x12); // VDV=10000

    cClockInfo::Delay_ms(1);
    vCmd(0x1C, 0x04); // AP=100
    cClockInfo::Delay_ms(1);

    vCmd(0x1B, 0x08); // GASENB=0, PON=0, DK=1, XDK=0, VgLCD_TRI=0, STB=0
    cClockInfo::Delay_ms(1);
    vCmd(0x1B, 0x12); // GASENB=0, PON=1, DK=0, XDK=0, VgLCD_TRI=0, STB=0
    cClockInfo::Delay_ms(1);
    vCmd(0x43, 0x80); //set VCOMG=0
    cClockInfo::Delay_ms(3);


    //
    /*vCmd(0x02, 0x00); //set x0
    vCmd(0x03, 0x3c); //set x0
    vCmd(0x04, 0x00); //set x1
    vCmd(0x05, 0xb3); //set x1
    vCmd(0x06, 0x00); //set y0
    vCmd(0x07, 0x90); //set y0
    vCmd(0x08, 0x01); //set y1
    vCmd(0x09, 0x3f); //set y1*/

    if (bDisplayOn)
    {
      // Display ON Setting
      vCmd(0x90, 0x7F); // SAP=0111 1111
      vCmd(0x26, 0x04); //GON=0, DTE=0, D=01
      cClockInfo::Delay_ms(4);
      vCmd(0x26, 0x24); //GON=1, DTE=0, D=01
      vCmd(0x26, 0x2C); //GON=1, DTE=0, D=11
      cClockInfo::Delay_ms(4);
      vCmd(0x26, 0x3C); //GON=1, DTE=1, D=11

      vClear(0xF);
    }
  }

  void vInit(void)
  {
    vInit(True);
  }

  void vDeInit(void)
  {
    vPwrDisable();
    vRstEnable();
    cClockInfo::Delay_ms(1);
    vRstDisable();
  }


  /*
  void HX8347A_StartDMA(uint32* lpui8SourceAdr, uint16 lui16SizeofSource)
  {

    while (!HX8347A_u8DMA_TX_Complete()) {}

    DMA_Cmd(DMA1_Channel5, DISABLE);

    mui8FirstTx = 0;

    DMA_ClearFlag(DMA1_FLAG_GL5);

    //2. Set the memory address in the DMA_CMARx register. The data will be written to or read from this memory after the peripheral event.
    //Variable from which data will be transmitted
    mstDMA.DMA_MemoryBaseAddr = (uint32)lpui8SourceAdr;

    //3. Configure the total number of data to be transferred in the DMA_CNDTRx register. After each peripheral event, this value will be decremented.
    //Buffer size
    mstDMA.DMA_BufferSize = lui16SizeofSource;

    DMA_Init(DMA1_Channel5, &mstDMA); //Initialise the DMA
    DMA_Cmd(DMA1_Channel5, ENABLE); //Enable the DMA1 – Channel5
  }*/


  void vDraw(uint8 color)
  {
    vCsEnable();
    write(color);
    vCsDisable();
    return;
  }


  void vDrawStop(void)
  {
    vWait();
    vCsDisable();
    vSerial();
    return;
  }


  void vDrawStart(void)
  {
    vCsEnable();
    write(nREGISTER);
    write(0x22);
    vWait();
    vCsDisable();

    vCsEnable();
    write(nDATA);

    vWait();

    vParallel();

    return;
  }


  void vClear(u8 lui8Col)
  {
    uint8 lu8y, lu8x;
    vSetArea(0, 0, (HX8347A_WIDTH - 1), (HX8347A_HEIGHT - 1));
    vDrawStart();
    for (lu8y = 0; lu8y < HX8347A_HEIGHT; lu8y++)
    {
      for (lu8x = 0; lu8x < HX8347A_WIDTH / 8; lu8x++)
      {
        write(lui8Col);
      }
    }
    vDrawStop();
  }


  void vSetArea(uint16 lui16x1, uint16 lui16y1, uint16 lui16x2, uint16 lui16y2)
  {
    lui16x1 += 60;
    lui16x2 += 60;

    vDrawStop();

    if (mui16PosX1 != lui16x1)
    {
      mui16PosX1 = lui16x1;
      vCmd(0x03, (lui16x1 >> 0)); //set x0
      vCmd(0x02, (lui16x1 >> 8)); //set x0
    }

    if (mui16PosX2 != lui16x2)
    {
      mui16PosX2 = lui16x2;
      vCmd(0x05, (lui16x2 >> 0)); //set x1
      vCmd(0x04, (lui16x2 >> 8)); //set x1
    }

    if (mui16PosY1 != lui16y1)
    {
      mui16PosY1 = lui16y1;
      vCmd(0x07, (lui16y1 >> 0)); //set y0
      vCmd(0x06, (lui16y1 >> 8)); //set y0
    }

    if (mui16PosY2 != lui16y2)
    {
      mui16PosY2 = lui16y2;
      vCmd(0x09, (lui16y2 >> 0)); //set y1
      vCmd(0x08, (lui16y2 >> 8)); //set y1
    }
  }


  void vCmd(unsigned int reg, unsigned int param)
  {
    vCsEnable();
    write(nREGISTER);
    write(reg);
    vWait();
    vCsDisable();

    vCsEnable();
    write(nDATA);
    write(param);
    vWait();
    vCsDisable();
  }

  void vEvDmaTc(void)
  {
    while (!isReady()) {};
    vDrawStop();
  }

  void vShowScreenDma(u8* lpui8Bm)
  {
    //uint16 Idx;
    vSetArea(0, 0, (HX8347A_WIDTH - 1), (HX8347A_HEIGHT - 1));
    vDrawStart();

    writeDma(lpui8Bm, HX8347A_WIDTH * HX8347A_HEIGHT / 8);
  }
};

#endif // _HX8347_H
