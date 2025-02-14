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

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

void Sprite::updateOAM() {
  if (id != 0) {
    // Apply rotation
    oamRotateScale(&oamMain, affine_index, degreesToAngle(rotation_angle), 256,
                   256);
  }

  // Update the sprite's position
  oamSet(&oamMain, id, pos.x - tile_offset.x, pos.y - tile_offset.y, priority,
         palette_alpha, sprite_size, color_format, gfx_mem, affine_index,
         size_double, hide, hflip, vflip, mosaic);
}