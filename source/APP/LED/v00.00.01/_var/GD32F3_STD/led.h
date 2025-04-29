
#ifndef __LED_H__
#define __LED_H__

#include "tGPPin.h"

template <const uintptr_t mpstPort, const uint16 mui16Pin>
class LED : public tcGpPin<mpstPort, mui16Pin>
{
  public:

  LED()
   : tcGpPin<mpstPort, mui16Pin>(GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, 0) {};

  void On()     {this->vSet0();};
  void Off()    {this->vSet1();};
  void Toggle() {this->vToggle();};
};

#endif // __LED_H__
