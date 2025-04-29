#ifndef __ILI9488_H__
#define __ILI9488_H__


#include "ili9488HW.h"

// Screen resolution in normal orientation
#define ILI9488_WIDTH   128
#define ILI9488_HEIGTH  160

class cILI9488: public cILI9488_HW
{
  public:

  typedef enum {
	  scr_normal = 0,
	  scr_CW     = 1,
	  scr_CCW    = 2,
	  scr_180    = 3
  } ScrOrientation_TypeDef;
  /*
  u16 mu16ScrWidth;
  u16 mu16ScrHeight;

  cILI9488() 
  : cILI9488_HW()
  {
    vInit(scr_normal);
  }

  cILI9488(ScrOrientation_TypeDef lenOrientation) 
  : cILI9488_HW()
  {
    vInit(lenOrientation);
  }

  void vInit(ScrOrientation_TypeDef lenOrientation)
  {
    // Reset display
    mPinCs.vSet1();
    mPinRst.vSet1();
    cClockInfo::Delay_ms(5);
    mPinRst.vSet0();
    cClockInfo::Delay_ms(5);
    mPinRst.vSet1();
    mPinCs.vSet1();
    cClockInfo::Delay_ms(5);
    mPinCs.vSet0();

    cmd2(0x11); // Sleep out & booster on
    cClockInfo::Delay_ms(20); // Datasheet says what display wakes about 120ms (may be much faster actually)

    cmd2(0xb1);   // In normal mode (full colors):
    data2(0x05);   // RTNA set 1-line period: RTNA2, RTNA0
    data2(0x3c);   // Front porch: FPA5,FPA4,FPA3,FPA2
    data2(0x3c);   // Back porch: BPA5,BPA4,BPA3,BPA2

    cmd2(0xb2);   // In idle mode (8-colors):
    data2(0x05);   // RTNB set 1-line period: RTNAB, RTNB0
    data2(0x3c);   // Front porch: FPB5,FPB4,FPB3,FPB2
    data2(0x3c);   // Back porch: BPB5,BPB4,BPB3,BPB2

    cmd2(0xb3);   // In partial mode + full colors:
    data2(0x05);   // RTNC set 1-line period: RTNC2, RTNC0
    data2(0x3c);   // Front porch: FPC5,FPC4,FPC3,FPC2
    data2(0x3c);   // Back porch: BPC5,BPC4,BPC3,BPC2
    data2(0x05);   // RTND set 1-line period: RTND2, RTND0
    data2(0x3c);   // Front porch: FPD5,FPD4,FPD3,FPD2
    data2(0x3c);   // Back porch: BPD5,BPD4,BPD3,BPD2

    cmd2(0xB4);   // Display dot inversion control:
    data2(0x03);    // NLB,NLC

    cmd2(0x3a);   // Interface pixel format
    //data(0x03);  // 12-bit/pixel RGB 4-4-4 (4k colors)
    data2(0x05);  // 16-bit/pixel RGB 5-6-5 (65k colors)
    //data(0x06);  // 18-bit/pixel RGB 6-6-6 (256k colors)

    //cmd(0x36);   // Memory data access control:
                // MY MX MV ML RGB MH - -
    //data(0x00);    // Normal: Top to Bottom; Left to Right; RGB
    //data(0x80);    // Y-Mirror: Bottom to top; Left to Right; RGB
    //data(0x40);    // X-Mirror: Top to Bottom; Right to Left; RGB
    //data(0xc0);    // X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
    //data(0x20);    // X-Y Exchange: X and Y changed positions
    //data(0xA0);    // X-Y Exchange,Y-Mirror
    //data(0x60);    // X-Y Exchange,X-Mirror
    //data(0xE0);    // X-Y Exchange,X-Mirror,Y-Mirror

    cmd2(0x20);   // Display inversion off
    //cmd(0x21);   // Display inversion on

    cmd2(0x13);   // Partial mode off

    cmd2(0x26);   // Gamma curve set:
    data2(0x01);    // Gamma curve 1 (G2.2) or (G1.0)
    //data(0x02);    // Gamma curve 2 (G1.8) or (G2.5)
    //data(0x04);    // Gamma curve 3 (G2.5) or (G2.2)
    //data(0x08);    // Gamma curve 4 (G1.0) or (G1.8)

    cmd2(0x29);   // Display on

    mPinCs.vSet1();

    vOrientation(lenOrientation);

    //vClear(u16RGB565(0xFF, 0, 0));
    //vClear(0xFFFF);
    vTest(u16RGB565(0, 0xFF, 0));

    volatile u32 x  = 4000000;
    while (x)
    {
      x--;
      asm("nop");
    };
  }

  u16 u16RGB565(u8 R, u8 G, u8 B) 
  {
    return ((R >> 3) << 11) | ((G >> 2) << 5) | (B >> 3);
  }

  void vOrientation(ScrOrientation_TypeDef orientation) 
  {
    cmd2(0x36); // Memory data access control:
    switch(orientation) 
    {
    case scr_CW:
      mu16ScrWidth  = ILI9488_HEIGTH;
      mu16ScrHeight = ILI9488_WIDTH;
      data2(0xA0); // X-Y Exchange,Y-Mirror
      break;
    case scr_CCW:
      mu16ScrWidth  = ILI9488_HEIGTH;
      mu16ScrHeight = ILI9488_WIDTH;
      data2(0x60); // X-Y Exchange,X-Mirror
      break;
    case scr_180:
      mu16ScrWidth  = ILI9488_WIDTH;
      mu16ScrHeight = ILI9488_HEIGTH;
      data2(0xc0); // X-Mirror,Y-Mirror: Bottom to top; Right to left; RGB
      break;
    default:
      mu16ScrWidth  = ILI9488_WIDTH;
      mu16ScrHeight = ILI9488_HEIGTH;
      data2(0x00); // Normal: Top to Bottom; Left to Right; RGB
      break;
    }
  }

  void vAddrSet(u16 XS, u16 YS, u16 XE, u16 YE) 
  {
    cmd(0x2a); // Column address set
    mPinA0.vSet1();
    write(XS >> 8);
    write(XS);
    write(XE >> 8);
    write(XE);

    cmd(0x2b); // Row address set
    mPinA0.vSet1();
    write(YS >> 8);
    write(YS);
    write(YE >> 8);
    write(YE);

    cmd(0x2c); // Memory write
  }

  void vClear(u16 color) 
  {
    u16 i;
    uint8_t  CH,CL;

    CH = color >> 8;
    CL = (uint8_t)color;

    mPinCs.vSet0();
    vAddrSet(0,0,mu16ScrWidth - 1,mu16ScrHeight - 1);
    mPinA0.vSet1();
    for (i = 0; i < mu16ScrWidth * mu16ScrHeight; i++) 
    {
      write(CH);
      write(CL);
    }
    mPinCs.vSet1();
  }

  void vAddrSet2(u16 XS, u16 YS, u16 XE, u16 YE) 
  {
    cmd2(0x2a); // Column address set
    data2(XS >> 8);
    data2(XS);
    data2(XE >> 8);
    data2(XE);

    cmd2(0x2b); // Row address set
    data2(YS >> 8);
    data2(YS);
    data2(YE >> 8);
    data2(YE);
  }

  void vTest(u16 color) 
  {
    u16 i;
    uint8_t  CH,CL;

    CH = color >> 8;
    CL = (uint8_t)color;

    vAddrSet2(20,20,mu16ScrWidth - 1 - 20,mu16ScrHeight - 1 - 20);
    cmd2(0x2c); // Memory write
    mPinCs.vSet0();
    mPinA0.vSet1();
    for (i = 0; i < (mu16ScrWidth - 40) * (mu16ScrHeight - 40); i++) 
    {
      write(CH);
      write(CL);
    }
    mPinCs.vSet1();
  }

  void vShowScreen(u8 *lpui8Bm) 
  {
    mPinCs.vSet0();
    vAddrSet(0,0,mu16ScrWidth - 1,mu16ScrHeight - 1);
    mPinA0.vSet1();
    write(lpui8Bm, mu16ScrWidth * mu16ScrHeight * 2);
    mPinCs.vSet1();
  }


  void vEvDmaTc(void)
  {
    while (!isReady()) {};
    mPinCs.vSet1();
  }

  void vShowScreenDma(u8 *lpui8Bm) 
  {
    // High und Low Byte order drehen
    u16 i;
    u8  d;

    for (i = 0; i < (mu16ScrWidth * mu16ScrHeight * 2); i+=2) 
    {
      d = lpui8Bm[i];
      lpui8Bm[i] = lpui8Bm[i+1];
      lpui8Bm[i+1] = d;
    }

    vAddrSet2(0,0,mu16ScrWidth - 1,mu16ScrHeight - 1);
    cmd2(0x2c); // Memory write
    mPinCs.vSet0();
    mPinA0.vSet1();
    writeDma(lpui8Bm, mu16ScrWidth * mu16ScrHeight * 2);    
  }*/
};

#endif // __ILI9488_H__
