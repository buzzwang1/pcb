
#ifndef __LED_H__
#define __LED_H__

#include "tGPPin.h"

template <const uintptr_t mpstPort, const uint16 mui16Pin>
class LED : public tcGpPin<mpstPort, mui16Pin>
{
  public:

  LED()
   : tcGpPin<mpstPort, mui16Pin>(GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_FREQ_MEDIUM, 0) {};

  void On()     {this->vSet0();};
  void Off()    {this->vSet1();};
  void Toggle() {this->vToggle();};
};

#endif // __LED_H__
