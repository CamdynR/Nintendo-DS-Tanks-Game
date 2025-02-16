/*---------------------------------------------------------------------------------

Sprite.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Sprite.h"
#include "sprite-sheet.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

int Sprite::num_sprites = 0; // Initialize the total number of sprites

void Sprite::initGfx() {
  // Allocate 32x32 sprite graphics memory
  gfx_mem = oamAllocateGfx(&oamMain, sprite_size, color_format);

  // Set the frame_gfx pointer to the right position in the sprite sheet
  int sprite_index =
      sprite_sheet_pos.y * Sprite::SPRITE_SHEET_COLS + sprite_sheet_pos.x;
  gfx_frame = (u8 *)sprite_sheetTiles + (sprite_index * tile_size * tile_size);
}

void Sprite::copyGfxFrameToVRAM() {
  dmaCopy(gfx_frame, gfx_mem, tile_size * tile_size);
}

void Sprite::incrementAnimationFrame(bool backwards) {
  if (backwards) {
    anim_frame = (anim_frame - 1 + num_anim_frames) % num_anim_frames;
  } else {
    anim_frame = (anim_frame + 1) % num_anim_frames;
  }
  // Match initGfx formula
  int sprite_index = ((sprite_sheet_pos.y) * Sprite::SPRITE_SHEET_COLS +
                      (sprite_sheet_pos.x + anim_frame));
  gfx_frame = (u8 *)sprite_sheetTiles + (sprite_index * tile_size * tile_size);
}

void Sprite::updateOAM() {
  // Apply rotation
  oamRotateScale(&oamMain, affine_index, degreesToAngle(rotation_angle), 256,
                 256);

  // Update the sprite's position
  oamSet(&oamMain, id, pos.x - tile_offset.x, pos.y - tile_offset.y, priority,
         palette_alpha, sprite_size, color_format, gfx_mem, affine_index,
         size_double, hide, hflip, vflip, mosaic);
}