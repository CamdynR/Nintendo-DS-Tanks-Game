#ifndef SPRITE_H
#define SPRITE_H

#include "Position.h"
#include <nds.h>

struct Sprite {
  static const int SPRITE_SHEET_COLS = 3;
  static int num_sprites; // The number of sprites on screen

  // Graphics related things
  u16 *gfx_mem;  // Where in VRAM sprite is stored
  u8 *gfx_frame; // The current frame in the sprite sheet

  Position pos = {0, 0};
  int tile_size = 32;

  int anim_frame = 0; // The animation frame of the sprite
  int anim_speed = 2; // Speed of the animation (higher == slower)

  float rotation_angle = 0; // The rotation angle of the sprite

  Position tile_offset = {0, 0};

  /**
   * @brief Initialize the sprite graphics, sets gfx_mem and gfx_frame
   * @param col The column in the sprite sheet (1 indexed)
   * @param row The row in the sprite sheet (1 indexed)
   */
  void initGfx(int col, int row);

  /*
   * @brief Updates the object attribute memory
   */
  void updateOAM();

  // Object-Attribute Memory Properties
  int id = 0;       // The ID of the sprite in the OAM
  int priority = 0; // The priority of the sprite
  int palette_alpha = 0;
  SpriteSize sprite_size = SpriteSize_32x32;
  SpriteColorFormat color_format = SpriteColorFormat_256Color;
  int affine_index = 0;
  bool size_double = false;
  bool hide = false;
  bool hflip = false;
  bool vflip = false;
  bool mosaic = false;
};

#endif // SPRITE_H