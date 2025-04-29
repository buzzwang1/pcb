#ifndef _SPRITEDEF
#define _SPRITEDEF

#include "bitmap_base.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum
{
  Sprite_nFormatRaw = 0,
  Sprite_nFormatRle
}Sprite_tenFormat;

typedef enum
{
  Sprite_nModeCopy = 0,
  Sprite_nModeAnd,
  Sprite_nModeOr,
  Sprite_nModeXor,
  Sprite_nModeLastEntry
}Sprite_tenMode;

typedef enum
{
  Sprite_nType_Sprite = 0,
  Sprite_nType_ArrayFont,
  Sprite_nType_BTreeFont,
  Sprite_nType_AnimSprite,
  Sprite_nType_ArraySprite,
}Sprite_tenType;


typedef enum
{
  Sprite_nBPP_BPP32_8R8G8B = BM_BPP32_8R8G8B,
  Sprite_nBPP_BPP8_8G      = BM_BPP8_8G,
  Sprite_nBPP_BPP1_1G      = BM_BPP1_1G,
  Sprite_nBPP16_5R6G5B     = BM_BPP16_5R6G5B
}Sprite_tenBPP;

#ifdef __cplusplus
}
#endif

#endif // _SPRITEDEF
