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
  // Allocate 32x32 sprite graphics memory
  this->gfx_mem =
      oamAllocateGfx(&oamMain, this->sprite_size, this->color_format);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  this->gfx_frame =
      (u8 *)sprite_sheetTiles + ((2 * 4) * this->tile_size * this->tile_size);
  dmaCopy(this->gfx_frame, this->gfx_mem,
          this->tile_size * this->tile_size);

  // Hide until shown on screen
  this->hide = true;
  this->tile_offset = {0, 8};
}

//---------------------------------------------------------------------------------
//
// HELPER FUNCTIONS
//
//---------------------------------------------------------------------------------

void drawDottedLine(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  int steps = 8; // Always have exactly 8 dots
  float xIncrement = dx / (float)steps;
  float yIncrement = dy / (float)steps;
  float x = x1;
  float y = y1;

  for (int i = 0; i <= steps; i++) {
    glBoxFilled(x - 1, y - 1, x + 1, y + 1,
                RGB15(7, 23, 31)); // Draw smaller circle
    x += xIncrement;
    y += yIncrement;
  }
}

void handleCursorInput(Cursor &cursor, touchPosition &touch, int keys,
                       Tank &userTank) {
  if (touch.rawx != 0 && touch.rawy != 0) {
    cursor.pos.x = touch.px - 16;
    cursor.pos.y = touch.py - 7;
    // Show the user's touch on screen
    Position tankPos = userTank.getPosition();
    drawDottedLine(tankPos.x + 8, tankPos.y + 8, cursor.pos.x + 15,
                   cursor.pos.y + 8);
  }
}

void processCursorInput(Cursor &cursor, Tank &userTank) {
  glBegin2D();
  // Button Input
  scanKeys();
  int keys = keysHeld();
  // Touch Input
  touchPosition touch;
  touchRead(&touch);
  // Handle touch input
  if (keys & KEY_TOUCH) {
    cursor.hide = false;
    handleCursorInput(cursor, touch, keys, userTank);
  } else {
    cursor.hide = true;
  }
  glEnd2D();
}