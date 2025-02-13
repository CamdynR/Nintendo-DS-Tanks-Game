/*---------------------------------------------------------------------------------

Cursor.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Cursor.h"

//---------------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//---------------------------------------------------------------------------------

void initCursor(Cursor &cursor) {
  // Allocate 32x32 sprite graphics memory
  cursor.sprite_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  cursor.sprite_frame_gfx =
      (u8 *)sprite_sheetTiles + ((2 * 4) * cursor.tile_size * cursor.tile_size);
  dmaCopy(cursor.sprite_frame_gfx, cursor.sprite_gfx_mem,
    cursor.tile_size * cursor.tile_size);
}