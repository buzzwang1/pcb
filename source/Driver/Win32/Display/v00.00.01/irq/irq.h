#ifndef __IRQ_H
#define __IRQ_H

#ifdef __cplusplus
  extern "C" {
#endif

#ifndef _WIN32
  #include <sys.h>
#endif

#include "TYPEDEF.h"

void IRQ_vDi(void);
void IRQ_vEn(void);


#ifdef __cplusplus
  }
#endif

#endif





























