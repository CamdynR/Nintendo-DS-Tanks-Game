#ifndef SPRITE_H
#define SPRITE_H

#include "Position.h"
#include <nds.h>

class Sprite {
private:
  static const int SPRITE_SHEET_COLS = 4;

public:
  static int num_sprites; // The number of sprites on screen

  // Graphics related things
  u16 *gfx_mem;  // Where in VRAM sprite is stored
  u8 *gfx_frame; // The current frame in the sprite sheet

  Position pos = {0, 0};
  int tile_size = 32;

  Position sprite_sheet_pos = {0, 0}; // x,y coord on sprite sheet
  int num_anim_frames = 3;            // Number of frames in animation cycle
  int anim_frame = 0;                 // The animation frame of the sprite
  int anim_speed = 2; // Speed of the animation (higher == slower)

  float rotation_angle = 0; // The rotation angle of the sprite

  Position tile_offset = {0, 0};

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

  /**
   * @brief Allocated memory for the graphics in VRAM and points to the current
   * frame
   */
  void initGfx();

  /**
   * @brief Updates the gfx_frame reference with the new animation frame
   *        of the sprite based on its state.
   */
  void incrementAnimationFrame(bool backwards = false);

  /**
   * @brief Needs to be called once per frame, updates VRAM with
   *        the latest sprite gfx frame
   */
  void copyGfxFrameToVRAM();

  /**
   * @brief Updates the object attribute memory
   */
  void updateOAM();
};

#endif // SPRITE_H