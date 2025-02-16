/*---------------------------------------------------------------------------------

Bullet.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Bullet.h"
#include "sprite-sheet.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

Bullet::Bullet(float direction, int num_ricochets)
    : direction(direction), num_ricochets(num_ricochets) {}

// void Bullet::updateAnimationFrames() {
//   // Calculate base position for this tank color's row
//   int row = 1 + color;
//   // Get current animation frame (0-2)
//   int col = (body.anim_frame % 3) + 1;

//   // Calculate the sprite sheet index
//   int sprite_index = ((row - 1) * Sprite::SPRITE_SHEET_COLS + (col - 1));
//   u8 *offset = (u8 *)sprite_sheetTiles +
//                (sprite_index * body.tile_size * body.tile_size);

//   dmaCopy(offset, body.gfx_mem, body.tile_size * body.tile_size);
//   dmaCopy(turret.gfx_frame, turret.gfx_mem,
//           turret.tile_size * turret.tile_size);
// }