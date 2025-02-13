#ifndef SPRITE_H
#define SPRITE_H

#include "Position.h"

struct Sprite {
  Position pos = {-32, -32};
  int tile_size = 32;
};

#endif // SPRITE_H