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
// INITIALIZATION FUNCTIONS - PRIVATE
//
//---------------------------------------------------------------------------------

void Cursor::createTail() {
  for (int i = 0; i < numTailSprites; i++) {
    // Initialize sprite
    Sprite *tailSegment = new Sprite();

    // Set initial sprite data
    tailSegment->sprite_sheet_pos = {0, 12};
    tailSegment->id = Sprite::num_sprites++;
    tailSegment->palette_alpha = tailSegment->id;
    tailSegment->affine_index = -1;
    // Hide until shown on screen
    tailSegment->hide = true;
    tailSegment->tile_offset = {15, 15};

    // Update the gfx
    tailSegment->initGfx();
    tailSegment->copyGfxFrameToVRAM();

    // Add to tail
    tail.push_back(tailSegment);
  }
}

void Cursor::setPosition(Position &pos) { this->pos = pos; }

void Cursor::connectToTank(Tank *playerTank) {
  // Grab the center of the player tank
  Position tankPos = playerTank->getPosition();
  int tankCenterX = tankPos.x + playerTank->body->tile_offset.x;
  int tankCenterY = tankPos.y + playerTank->body->tile_offset.y;

  // Math to connect the two points
  int dx = pos.x - tankCenterX;
  int dy = pos.y - tankCenterY;
  float xIncrement = dx / (float)(numTailSprites + 2);
  float yIncrement = dy / (float)(numTailSprites + 2);
  float x = tankCenterX;
  float y = tankCenterY;

  // Position each tail sprite along the line
  for (int i = 0; i < numTailSprites + 1; i++) {
    x += xIncrement;
    y += yIncrement;
    if (i < 1) continue;
    // Update the position of each tail sprite
    tail[i - 1]->pos = { (int)x, (int)y };
    tail[i - 1]->hide = false;  // Make sure the sprite is visible
  }
}

//---------------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS - PUBLIC
//
//---------------------------------------------------------------------------------

Cursor::Cursor() {
  // Set sprite sheet position
  this->sprite_sheet_pos = {0, 11};

  // Assign an ID
  this->id = Sprite::num_sprites++;
  this->palette_alpha = this->id;
  this->affine_index = -1;
  // Hide until shown on screen
  this->hide = true;
  this->tile_offset = {16, 16};

  // Create all of the tail sprites
  createTail();

  // Initialize graphics and copy to VRAM
  initGfx();
  copyGfxFrameToVRAM();
}

void Cursor::showSprites(Position cursorPos, Tank *playerTank) {
  // Update the position first
  setPosition(cursorPos);
  // Then show the sprites
  hide = false;
  connectToTank(playerTank);
}

void Cursor::hideSprites() {
  this->hide = true;
  for (int i = 0; i < numTailSprites; i++) {
    tail[i]->hide = true;
  }
}

void Cursor::updateOAM() {
  Sprite::updateOAM();
  for (int i = 0; i < numTailSprites; i++) {
    tail[i]->updateOAM();
  }
}