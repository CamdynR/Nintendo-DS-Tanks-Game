/*---------------------------------------------------------------------------------

Tank.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Tank.h"

//---------------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//---------------------------------------------------------------------------------

void initTankBodyGfx(Tank *tank, u8 *gfx, int color) {
  // Allocate 32x32 sprite graphics memory
  tank->body_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  tank->body_frame_gfx = gfx + ((color * 3) * tank->tile_size * tank->tile_size);
}

void initTankTurretGfx(Tank *tank, u8 *gfx, int color) {
  // Allocate 32x32 sprite graphics memory
  tank->turret_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the turret_frame_gfx pointer to the correct position in the sprite
  // sheet Assuming each sprite is 32x32 pixels and gfx is a linear array Blue
  // turret is in row 2, column 1 Red turret is in row 2, column 2
  int row = 3;        // Row 2 (0-indexed)
  int column = color; // Column 1 for blue (0), Column 2 for red (1)
  tank->turret_frame_gfx = gfx + (row * tank->tile_size * tank->tile_size * 2) +
                           (column * tank->tile_size * tank->tile_size);
}

Tank createTank(int x, int y, int color) {
  Tank tank = {{x, y}};
  tank.color = color;

  initTankBodyGfx(&tank, (u8 *)sprite_sheetTiles, color);
  initTankTurretGfx(&tank, (u8 *)sprite_sheetTiles, color);

  return tank;
}