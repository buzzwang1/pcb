
#include "main.h"

/*#include <cstdint>
#include <array>
#include <span>
#include <concepts>
#include <iostream>
#include <iterator>
#include <string>*/


// STM32F303
// ARM®-based Cortex®-M4 32b MCU, (72 MHz max)
// Rom 256KB
// Ram 48KB

void NMI_Handler(void)
{
  while (1)
  {
  }
}

void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}


void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}


void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}


void SVC_Handler(void)
{
  while (1)
  {
  }
}

void DebugMon_Handler(void)
{
  while (1)
  {
  }
}


void PendSV_Handler(void)
{
  while (1)
  {
  }
}


void SysTick_Handler(void)
{
}

/*
using GPIODEF = \
struct IODefStruct
{
  GPIO_TypeDef* GPIO;
  u32 PinNumber;
  enum IOFUNCTION :u32 {INPUT = 0, OUTPUT = 1, ALT = 2, ANALOG = 3} Functions;
  u32 ALTFunc;
  enum IOTYPE : u32 { NORMAL = 0, OPENDRAIN = 1} Type;
  enum IOSPEED: u32 { LOW = 0, MEDIUM = 1, HIGH = 2, VERYHIGH = 3} Speed;
  enum IOPULL:  u32 { NONE = 0, PULLUP = 1, PULLDOWN = 2 } Bias;
  enum IOSTATE: u32 { LOGIC_LOW, LOGIC_HIGH, DONT_CARE} InitialState;
};


static const std::array<GPIODEF, 1> gpiodefs =
{
  {
    GPIOB, 0,
    GPIODEF::IOFUNCTION::OUTPUT, 1,
    GPIODEF::IOTYPE::NORMAL,
    GPIODEF::IOSPEED::LOW,
    GPIODEF::IOPULL::NONE,
    GPIODEF::IOSTATE::LOGIC_LOW
  }
}*/




template <typename tySizeType, const tySizeType mSize>
#ifdef __GNUC__
class __attribute__((packed)) cBAryT
#else
class cBAryT
#endif
{
public:
  u8         mu8Data[mSize];
  tySizeType mLen;

  cBAryT()
  {
    mLen = 0;
  }

  void vSet(tySizeType lLen, u8 lu8Value)
  {
    tySizeType lIdx;

    mLen = lLen;

    if (mLen > mSize) mLen = mSize;

    for (lIdx = 0; lIdx < mLen; lIdx++)
    {
      mu8Data[lIdx] = lu8Value;
    }
  }

  void vAdd(u8 lu8Value)
  {
    if (mLen < mSize)
    {
      mu8Data[mLen] = lu8Value;
      mLen++;
    }
  }

  tySizeType Size() { return mSize; }
  u8*        Data() { return mu8Data; }
};



#ifdef __GNUC__
class __attribute__((packed)) cBAry
#else
class cBAry
#endif
{
public:
  u16 mSize;
  u16 mLen;
  u8  *mu8Data;

  cBAry(u8* lu8Data, u16 lSize)
  {
    mu8Data = lu8Data;
    mSize   = lSize;
    mLen = 0;
  }

  void vSet(u16 lLen, u8 lu8Value)
  {
    u16 lIdx;

    mLen = lLen;

    if (mLen > mSize) mLen = mSize;

    for (lIdx = 0; lIdx < mLen; lIdx++)
    {
      mu8Data[lIdx] = lu8Value;
    }
  }

  void vAdd(u8 lu8Value)
  {
    if (mLen < mSize)
    {
      mu8Data[mLen] = lu8Value;
      mLen++;
    }
  }
};


                               //   988, 104, 28

cBAryT<u16, 5> lcTmplt1;     //  1032, 108, 35
cBAryT<u16, 6> lcTmplt2;     //  1056, 108, 43
cBAryT<u16, 7> lcTmplt3;     //  1080, 108, 52
cBAryT<u16, 8> lcTmplt4;     //  1104, 108, 62 

//u8 lu8Test1_Buf[5]; cBAry lcTest1(lu8Test1_Buf, sizeof(lu8Test1_Buf)); // 1088, 108, 41
//u8 lu8Test2_Buf[6]; cBAry lcTest2(lu8Test2_Buf, sizeof(lu8Test2_Buf)); // 1180, 108, 55
//u8 lu8Test3_Buf[7]; cBAry lcTest3(lu8Test3_Buf, sizeof(lu8Test3_Buf)); // 1192, 108, 70
//u8 lu8Test4_Buf[8]; cBAry lcTest4(lu8Test4_Buf, sizeof(lu8Test4_Buf)); // 1236, 108, 86

int main(void)
{

  while (1)
  {
    lcTmplt1.vSet(2, 1); lcTmplt1.vAdd(2);
    lcTmplt2.vSet(2, 1); lcTmplt2.vAdd(2);
    lcTmplt3.vSet(2, 1); lcTmplt3.vAdd(2);
    lcTmplt4.vSet(2, 1); lcTmplt4.vAdd(2);


    //lcTest1.vSet(2, 1); lcTest1.vAdd(2);
    //lcTest2.vSet(2, 1); lcTest2.vAdd(2);
    //lcTest3.vSet(2, 1); lcTest3.vAdd(2);
    //lcTest4.vSet(2, 1); lcTest4.vAdd(2);

    asm( "nop" );
  }
}

