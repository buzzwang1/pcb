
#ifndef _IRQ_H
#define _IRQ_H

#include <inttypes.h>

#ifdef __cplusplus
 extern "C" {
#endif

//enable all interrupts
extern void _eai(void);

//disable all interrupts
extern void _dai(void);

//#define _eai()
//#define _dai()

#ifdef __cplusplus
}
#endif


#endif // _IRQ_H