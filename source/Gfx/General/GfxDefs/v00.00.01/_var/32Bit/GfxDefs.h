#ifndef _GFXDEFS_H
#define _GFXDEFS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "TypeDef.h"

#define GFX_INVALID            null
#define GFX_FAIL               FRET_ERR_GENERAL
#define GFX_OK                 FRET_OK
#define GFX_INLINE             inline


// Max image size
// e.g. if img < 256x256 pixel uint16 is suffient
typedef uint32  GfxImgSize;
typedef int16   GfxInt;
typedef uint16  GfxUInt;

typedef struct {GfxInt x; GfxInt y;}  tstGfxPos2D; // Pixel

#ifdef __cplusplus
}
#endif

#endif // _GFXDEFS_H
