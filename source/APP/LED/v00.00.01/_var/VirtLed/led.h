
#ifndef __LED_H__
#define __LED_H__


class LED
{
  public:

  enum tenLedState
  {
    nOff = 0,
    nOn
  };

  tenLedState menState;

  LED()         {menState = nOff;}

  void On()     {menState = nOn;};
  void Off()    {menState = nOff;};
  void Toggle() {if (menState == nOn) menState = nOff; else menState = nOn;};

  bool operator==(const tenLedState rhs) { return menState == rhs;};
};

#endif // __LED_H__
