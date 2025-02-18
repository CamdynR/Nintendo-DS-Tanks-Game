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
#include "Stage.h"
#include "sprite-sheet.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

int Sprite::num_sprites = 0; // Initialize the total number of sprites

Sprite::~Sprite() {
  // Free the sprite's graphics memory from VRAM if it exists
  if (gfx_mem != nullptr) {
    oamFreeGfx(&oamMain, gfx_mem);
    gfx_mem = nullptr;
  }

  // Decrement the total sprite count
  num_sprites--;
}

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

void Sprite::incrementAnimationFrame(bool backwards, bool loop) {
  if (hide || Stage::frame_counter % anim_speed != 0) return;

  if (backwards) {
    anim_frame = (anim_frame - 1 + num_anim_frames) % num_anim_frames;
    // If we've looped and loop is false, hide sprite and set to last frame
    if (anim_frame == num_anim_frames - 1 && !loop) {
      hide = true;
    }
  } else {
    anim_frame = (anim_frame + 1) % num_anim_frames;
    // If we've looped and loop is false, hide sprite and reset to first frame
    if (anim_frame == 0 && !loop) {
      hide = true;
    }
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