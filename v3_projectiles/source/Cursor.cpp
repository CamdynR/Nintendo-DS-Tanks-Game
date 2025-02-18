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

Cursor::Cursor() {
  // Set sprite sheet position
  this->sprite_sheet_pos = {0, 11};
  // Initialize graphics and copy to VRAM
  initGfx();
  copyGfxFrameToVRAM();

  // Assign an ID
  this->id = Sprite::num_sprites++;
  this->palette_alpha = this->id;
  this->affine_index = this->id;
  // Hide until shown on screen
  this->hide = true;
  this->tile_offset = {16, 16};
}

void Cursor::setPosition(int x, int y) { pos = {x, y}; }

void Cursor::connectToTank(Tank *playerTank) {
  // Grab the center of the player tank
  Position tankPos = playerTank->getPosition();
  int tankCenterX = tankPos.x + playerTank->body->tile_offset.x;
  int tankCenterY = tankPos.y + playerTank->body->tile_offset.y;

  // Math to connect the two points
  int dx = pos.x - tankCenterX;
  int dy = pos.y - tankCenterY;
  int steps = 8; // Always have exactly 8 dots
  float xIncrement = dx / (float)steps;
  float yIncrement = dy / (float)steps;
  float x = tankCenterX;
  float y = tankCenterY;

  // Draw the line
  for (int i = 0; i <= steps; i++) {
    glBoxFilled(x - 1, y - 1, x + 1, y + 1,
                RGB15(7, 23, 31)); // Draw smaller circle
    x += xIncrement;
    y += yIncrement;
  }
}
