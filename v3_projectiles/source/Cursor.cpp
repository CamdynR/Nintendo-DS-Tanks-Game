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
    handleCursorInput(cursor, touch, keys, userTank);
  } else {
    cursor.pos.x = -1 * cursor.tile_size;
    cursor.pos.y = -1 * cursor.tile_size;
  }
  glEnd2D();
}