#ifndef _MEMPART_H
#define _MEMPART_H

#ifdef __cplusplus
  extern "C" {
#endif

#include "typedef.h"

#define MemPart_Sort_Ram     (0)
#define MemPart_Sort_Flash  (16)
#define MemPart_Sort_Eep    (32)

#define MemPart_Type_Default    (0)

typedef struct
{
  char8  szName[16];
  uint16 ui16Sort;
  uint16 ui16Type;
  uint32 ui32Start;
  uint32 ui32Size;
  uint32 ui32Block_Size;
}tstMemPart_Partition;


#ifdef __cplusplus
 }
#endif

#endif /* _MEMPART_H */
