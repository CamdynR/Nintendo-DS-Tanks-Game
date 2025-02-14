/*---------------------------------------------------------------------------------

Tank.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include <math.h>
#include "Tank.h"
#include "input.h"

//---------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//---------------------------------------------------------------------------------

Tank::Tank(int x, int y, TankColor color, int &spriteIdCount) {
  // Update the position of both sprites
  this->setPosition(x, y);
  this->setOffset(8, 8);
  // Set the tank's color and offsets for body and turret
  this->color = color;

  // Set the oam attributes for the tank body
  this->body.id = spriteIdCount++;
  this->body.palette_alpha = this->body.id;
  this->body.priority = 2;
  this->body.affine_index = this->body.id;

  // Update Sprite ID Count
  // Set the oam attributes for the tank turret
  this->turret.id = spriteIdCount++;
  this->turret.palette_alpha = this->turret.id;
  this->turret.priority = 1;
  this->turret.affine_index = this->turret.id;

  // Initialize the tank body and turret graphics
  initTankBodyGfx(this, (u8 *)sprite_sheetTiles);
  initTankTurretGfx(this, (u8 *)sprite_sheetTiles);

  // Set the initial animation frames
  this->updateAnimationFrames();
}

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

void Tank::setOffset(int x, int y) {
  body.tile_offset = {x, y};
  turret.tile_offset = {x, y};
}

Position &Tank::getPosition() { return body.pos; }

void Tank::updateAnimationFrames() {
  int frame = body.anim_frame + color;
  // Calculate the offset correctly for a 32x32 sprite with 3 frames
  u8 *offset = body.gfx_frame + frame * body.tile_size * body.tile_size;

  dmaCopy(offset, body.gfx_mem, body.tile_size * body.tile_size);
  dmaCopy(turret.gfx_frame, turret.gfx_mem, turret.tile_size * turret.tile_size);
}

void Tank::interpolateBodyRotation() {
  // Normalize both angles to [0, 360) range first
  float target_angle = fmod(fmod(direction, 360.0f) + 360.0f, 360.0f);
  float current_angle = fmod(fmod(body.rotation_angle, 360.0f) + 360.0f, 360.0f);
  
  // Compute the shortest rotation direction
  float angle_diff = target_angle - current_angle;
  
  // Adjust for crossing over 0/360 boundary
  if (angle_diff > 180.0f) {
      angle_diff -= 360.0f;
  } else if (angle_diff < -180.0f) {
      angle_diff += 360.0f;
  }
  
  // Rotate in the shortest direction with smooth interpolation
  if (fabs(angle_diff) > 0.1f) {  // Small threshold to prevent jittering
      if (angle_diff > 0) {
          body.rotation_angle += body_rotation_speed;
      } else {
          body.rotation_angle -= body_rotation_speed;
      }
  } else {
      body.rotation_angle = target_angle;  // Snap to exact angle when very close
  }
  
  // Keep the angle in [0, 360) range
  body.rotation_angle = fmod(fmod(body.rotation_angle, 360.0f) + 360.0f, 360.0f);
}

void Tank::move(TankDirection direction, int frameCounter) {
  bool hasMoved = false;
  int baseSpeed = 1;

  // Accumulate fractional movement
  static float accumulatedX = 0.0f;
  static float accumulatedY = 0.0f;

  bool isDiagonal = (direction == T_NE) || (direction == T_SE) || 
  (direction == T_SW) || (direction == T_NW);

  // Handle Y movement
  if (this->direction == this->body.rotation_angle) {
    Position newPosY = {tank.getPosition('x'), tank.getPosition('y')};
    if (keys & KEY_UP || keys & KEY_DOWN) {
      float yMove = baseSpeed * (isDiagonal ? 0.707f : 1.0f);
      float testY = accumulatedY + yMove;
      int moveAmount = (int)testY;
      
      if (moveAmount != 0) {
        if (keys & KEY_UP) {
          newPosY.y -= moveAmount;
        } else {
          newPosY.y += moveAmount;
        }
        
        if (validateInput(newPosY, tank)) {
          tank.setPosition('y', newPosY.y);
          hasMoved = true;
          accumulatedY = testY - moveAmount;  // Only update accumulator if move was valid
        }
      } else {
        accumulatedY = testY;  // Accumulate small movements
      }
    }
  }

  // Handle X movement
  if (tank.direction == tank.body.rotation_angle) {
    Position newPosX = {tank.getPosition('x'), tank.getPosition('y')};
    if (keys & KEY_LEFT || keys & KEY_RIGHT) {
      float xMove = baseSpeed * (isDiagonal ? 0.707f : 1.0f);
      float testX = accumulatedX + xMove;
      int moveAmount = (int)testX;
      
      if (moveAmount != 0) {
        if (keys & KEY_LEFT) {
          newPosX.x -= moveAmount;
        } else {
          newPosX.x += moveAmount;
        }

        if (validateInput(newPosX, tank)) {
          tank.setPosition('x', newPosX.x);
          hasMoved = true;
          accumulatedX = testX - moveAmount;  // Only update accumulator if move was valid
        }
      } else {
        accumulatedX = testX;  // Accumulate small movements
      }
    }
  }

  if (hasMoved && frameCounter >= ANIMATION_SPEED) {
    tank.body.anim_frame = (tank.body.anim_frame - 1 + 3) % 3;
    tank.updateAnimationFrames();
    frameCounter = 0;
  }
}

void Tank::updateOAM() {
  interpolateBodyRotation();

  // Adjust for rotational miscalculations
  switch (direction) {
  case T_SE: // Tank_Southeast
    body.tile_offset.x += 1;
    break;
  case T_S: // Tank_South
    body.tile_offset.x += 1;
    break;
  case T_SW: // Tank_Southwest
    body.tile_offset.x += 1;
    body.tile_offset.y += 1;
    break;
  case T_W: // Tank_West
    body.tile_offset.y += 1;
    break;
  case T_NW: // Tank_Northwest
    body.tile_offset.y += 1;
    break;
  }

  // Update the OAM
  body.updateOAM();
  turret.updateOAM();

  // Reset rotational adjustment
  setOffset(8, 8);
}

//---------------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//---------------------------------------------------------------------------------

void initTankBodyGfx(Tank *tank, u8 *gfx) {
  // Allocate 32x32 sprite graphics memory
  tank->body.gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  tank->body.gfx_frame =
      gfx + ((tank->color * 3) * tank->body.tile_size * tank->body.tile_size);
}

void initTankTurretGfx(Tank *tank, u8 *gfx) {
  // Allocate 32x32 sprite graphics memory
  tank->turret.gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the turret_frame_gfx pointer to the correct position in the sprite
  // sheet Assuming each sprite is 32x32 pixels and gfx is a linear array Blue
  // turret is in row 2, column 1 Red turret is in row 2, column 2
  int row = 3;             // Row 2 (0-indexed)
  int column = tank->color; // Column 1 for blue (0), Column 2 for red (1)
  tank->turret.gfx_frame =
      gfx + (row * tank->turret.tile_size * tank->turret.tile_size * 2) +
      (column * tank->turret.tile_size * tank->turret.tile_size);
}
