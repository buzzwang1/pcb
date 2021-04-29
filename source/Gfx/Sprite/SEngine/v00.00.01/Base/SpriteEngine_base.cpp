
#include "spriteengine_base.h"


const tmp_i8SpriteEngineBitx cSpriteEngine::i8Bitx[Sprite_nModeLastEntry * 2] =
{
  &cSpriteEngine::i8BitCpyRaw,
  &cSpriteEngine::i8BitAndRaw,
  &cSpriteEngine::i8BitOrRaw,
  &cSpriteEngine::i8BitXorRaw,
  &cSpriteEngine::i8BitCpyRle,
  &cSpriteEngine::i8BitAndRle,
  &cSpriteEngine::i8BitOrRle,
  &cSpriteEngine::i8BitXorRle,
};


