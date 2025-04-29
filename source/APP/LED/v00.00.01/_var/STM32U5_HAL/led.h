
#ifndef __LED_H__
#define __LED_H__

#include "tGPPin.h"

template <const uintptr_t mpstPort, const uint16 mui16Pin>
class LED : public tcGpPin<mpstPort, mui16Pin>
{
  public:
    bool mInverse;

  LED()
   : tcGpPin<mpstPort, mui16Pin>(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_LOW, 0)
  {
    mInverse = True;
  }

  void On()  { if (mInverse) {this->vSet0();}  else {this->vSet1();}};
  void Off() { if (mInverse) { this->vSet1();} else {this->vSet0();}};
  void Toggle() {this->vToggle();};
};

#endif // __LED_H__
