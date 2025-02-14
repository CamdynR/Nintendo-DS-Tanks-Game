#ifndef SPRITE_H
#define SPRITE_H

#include "Position.h"
#include <nds.h>

struct Sprite {
  Position pos = {-32, -32};
  int tile_size = 32;

  u16 *gfx_mem;  // Where in VRAM sprite is stored
  u8 *gfx_frame; // The current frame in the sprite sheet

  int anim_frame = 0; // The animation frame of the sprite

  float rotation_angle = 0; // The rotation angle of the sprite
};

#endif // SPRITE_H