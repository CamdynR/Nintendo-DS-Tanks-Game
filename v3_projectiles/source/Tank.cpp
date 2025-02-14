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
// STRUCT FUNCTIONS
//
//---------------------------------------------------------------------------------

void Tank::setPosition(char axis, int value) {
  if (axis == 'x') {
    body.pos.x = value;
    turret.pos.x = value;
  } else if (axis == 'y') {
    body.pos.y = value;
    turret.pos.y = value;
  }
}

void Tank::setPosition(int x, int y) {
  body.pos = {x, y};
  turret.pos = {x, y};
}

int Tank::getPosition(char axis) {
  if (axis == 'x') {
    return body.pos.x;
  } else if (axis == 'y') {
    return body.pos.y;
  }
  return -1;
}

Position &Tank::getPosition() { return body.pos; }

void Tank::animate() {
  int frame = body.anim_frame + color;
  // Calculate the offset correctly for a 32x32 sprite with 3 frames
  u8 *offset = body.gfx_frame + frame * body.tile_size * body.tile_size;

  dmaCopy(offset, body.gfx_mem, body.tile_size * body.tile_size);
  dmaCopy(turret.gfx_frame, turret.gfx_mem,
          turret.tile_size * turret.tile_size);
}

//---------------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//---------------------------------------------------------------------------------

void initTankBodyGfx(Tank &tank, u8 *gfx) {
  // Allocate 32x32 sprite graphics memory
  tank.body.gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  tank.body.gfx_frame =
      gfx + ((tank.color * 3) * tank.body.tile_size * tank.body.tile_size);
}

void initTankTurretGfx(Tank &tank, u8 *gfx) {
  // Allocate 32x32 sprite graphics memory
  tank.turret.gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the turret_frame_gfx pointer to the correct position in the sprite
  // sheet Assuming each sprite is 32x32 pixels and gfx is a linear array Blue
  // turret is in row 2, column 1 Red turret is in row 2, column 2
  int row = 3;             // Row 2 (0-indexed)
  int column = tank.color; // Column 1 for blue (0), Column 2 for red (1)
  tank.turret.gfx_frame =
      gfx + (row * tank.turret.tile_size * tank.turret.tile_size * 2) +
      (column * tank.turret.tile_size * tank.turret.tile_size);
}

Tank createTank(int x, int y, int color) {
  Tank tank = {{x, y}};
  tank.color = color;

  initTankBodyGfx(tank, (u8 *)sprite_sheetTiles);
  initTankTurretGfx(tank, (u8 *)sprite_sheetTiles);

  return tank;
}
