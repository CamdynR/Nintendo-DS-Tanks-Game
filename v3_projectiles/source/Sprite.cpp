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

void Sprite::initGfx(int col, int row) {
  // Allocate 32x32 sprite graphics memory
  this->gfx_mem =
      oamAllocateGfx(&oamMain, this->sprite_size, this->color_format);

  // Set the frame_gfx pointer to the right position in the sprite sheet
  int sprite_index = ((row - 1) * Sprite::SPRITE_SHEET_COLS + (col - 1));
  this->gfx_frame = (u8 *)sprite_sheetTiles +
                    (sprite_index * this->tile_size * this->tile_size);

  // Copy the frame graphics into memory
  dmaCopy(this->gfx_frame, this->gfx_mem, this->tile_size * this->tile_size);
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