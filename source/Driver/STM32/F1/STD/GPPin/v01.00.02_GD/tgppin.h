#ifndef __TGPPIN_H__
#define __TGPPIN_H__

#include "TypeDef.h"
#include "gd32f1x0.h"
#include "gd32f1x0_rcu.h"
#include "gd32f1x0_gpio.h"


template <const u32 mu32Port_BaseAdr>
class tcPort
{
public:
  tcPort()
  {
    /* GPIO Periph clock enable */
    /* Enable USART clock */
    switch (mu32Port_BaseAdr)
    {
      case GPIOA: rcu_periph_clock_enable(RCU_GPIOA);break;
      case GPIOB: rcu_periph_clock_enable(RCU_GPIOB);break;
      case GPIOC: rcu_periph_clock_enable(RCU_GPIOC);break;
      case GPIOD: rcu_periph_clock_enable(RCU_GPIOD);break;
      case GPIOF: rcu_periph_clock_enable(RCU_GPIOF);break;
    }
  }

  inline u32 Port()
  {
    return mu32Port_BaseAdr;
  }
};

template <const u32 mu32Port_BaseAdr, const uint16 mui16Pin>
class tcGpPin : public tcPort<mu32Port_BaseAdr>
{
public:
  tcGpPin(u32 lu32Mode,
          u32 lu32PupPdwn,
          u32 lu32oType,
          u32 lu32Speed,
          u8  lu8InitValue)
    : tcPort<mu32Port_BaseAdr>()
  {
    vSet(lu8InitValue);

    gpio_mode_set(this->Port(), lu32Mode, lu32PupPdwn, PinBitMask());
    gpio_output_options_set(this->Port(), lu32oType, lu32Speed, PinBitMask());
  }

  // Mode:    GPIO_MODE_INPUT,  GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG
  // PupPdwn: GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, GPIO_PUPD_PULLDOWN
  void vSetMode(u32 lu32Mode, u32 lu32PupPdwn)
  {
    gpio_mode_set(this->Port(), lu32Mode, lu32PupPdwn, PinBitMask());
  }

  // oType: GPIO_OTYPE_PP,    GPIO_OTYPE_OD
  // Speed: GPIO_OSPEED_2MHZ, GPIO_OSPEED_10MHZ, GPIO_OSPEED_50MHZ
  void vSetOption(u32 lu32oType, u32 lu32Speed)
  {
    gpio_output_options_set(this->Port(), lu32oType, lu32Speed, PinBitMask());
  }

  // lu32AFx: GPIO_AF_0 ..
  void vSetAF(u32 lu32AFx)
  {
    gpio_af_set(this->Port(), lu32AFx, PinBitMask());
  }
	
  inline uint8 ui8Get()
  {
    return gpio_input_bit_get(this->Port(), PinBitMask());
  }

  inline void vSet(uint8 lui8Value)
  {
    gpio_bit_write(this->Port(), PinBitMask(), (bit_status)lui8Value);
  }

  inline void vSet1(void)
  {
    gpio_bit_set(this->Port(), PinBitMask());
  }

  inline void vSet0(void)
  {
    gpio_bit_reset(this->Port(), PinBitMask());
  }

  inline void vToggle(void)
  {
    if (gpio_input_bit_get(this->Port(), PinBitMask()))
    {
      vSet0();
    }
    else
    {
      vSet1();
    }
  }

private:
  inline constexpr u32 PinBitMask()
  {
    return (1 << mui16Pin);
  }
};


class cGpPin
{
public:
  u32 mu32PortBase;
  u16 mui16Pin;

  cGpPin() {};

  cGpPin(u32 lu32PortBase,
         u16 lui16Pin,
         u32 lu32Mode,
         u32 lu32PupPdwn,
         u32 lu32oType,
         u32 lu32Speed,
         u8  lu8InitValue)
  {
    vInit(lu32PortBase, lui16Pin, lu32Mode, lu32PupPdwn, lu32oType, lu32Speed, lu8InitValue);
  }

  void vInit(u32 lu32PortBase,
             u16 lui16Pin,
             u32 lu32Mode,
             u32 lu32PupPdwn,
             u32 lu32oType,
             u32 lu32Speed,
             u8  lu8InitValue)
  {
    mu32PortBase = lu32PortBase;
    mui16Pin     = lui16Pin;

    switch (lu32PortBase)
    {
      case GPIOA: rcu_periph_clock_enable(RCU_GPIOA);break;
      case GPIOB: rcu_periph_clock_enable(RCU_GPIOB);break;
      case GPIOC: rcu_periph_clock_enable(RCU_GPIOC);break;
      case GPIOD: rcu_periph_clock_enable(RCU_GPIOD);break;
      case GPIOF: rcu_periph_clock_enable(RCU_GPIOF);break;
    }

    vSet(lu8InitValue);

    gpio_mode_set(lu32PortBase, lu32Mode, lu32PupPdwn, PinBitMask());
    gpio_output_options_set(lu32PortBase, lu32oType, lu32Speed, PinBitMask());
  }


  // Mode:    GPIO_MODE_INPUT,  GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG
  // PupPdwn: GPIO_PUPD_NONE, GPIO_PUPD_PULLUP, GPIO_PUPD_PULLDOWN
  void vSetMode(u32 lu32Mode, u32 lu32PupPdwn)
  {
    gpio_mode_set(mu32PortBase, lu32Mode, lu32PupPdwn, PinBitMask());
  }

  // oType: GPIO_OTYPE_PP,    GPIO_OTYPE_OD
  // Speed: GPIO_OSPEED_2MHZ, GPIO_OSPEED_10MHZ, GPIO_OSPEED_50MHZ
  void vSetOption(u32 lu32oType, u32 lu32Speed)
  {
    gpio_output_options_set(mu32PortBase, lu32oType, lu32Speed, PinBitMask());
  }


  // lu32AFx: GPIO_AF_0 ..
  void vSetAF(u32 lu32AFx)
  {
    gpio_af_set(mu32PortBase, lu32AFx, PinBitMask());
  }

  inline uint8 ui8Get()
  {
    return gpio_output_bit_get(mu32PortBase, PinBitMask());
  }

  inline void vSet(uint8 lui8Value)
  {
    gpio_bit_write(mu32PortBase, PinBitMask(), (bit_status)lui8Value);
  }

  inline void vSet1(void)
  {
    gpio_bit_set(mu32PortBase, PinBitMask());
  }

  inline void vSet0(void)
  {
    gpio_bit_reset(mu32PortBase, PinBitMask());
  }

  inline void vToggle(void)
  {
    if (ui8Get()) vSet0();
             else vSet1();
  }

private:
  inline constexpr u32 PinBitMask()
  {
    return (1 << mui16Pin);
  }
};

#endif // __TGPPIN_H__
