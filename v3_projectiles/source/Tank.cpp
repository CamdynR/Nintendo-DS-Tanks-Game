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
#include "Input.h"
#include "Sprite.h"
#include "Stage.h"
#include <math.h>

//---------------------------------------------------------------------------------
//
// HELPER FUNCTIONS
//
//---------------------------------------------------------------------------------

float calculateAngle(int x1, int y1, int x2, int y2) {
  float deltaX = x2 - x1;
  float deltaY = y2 - y1;
  float angle = atan2(deltaY, deltaX) * (180.0 / M_PI);
  return angle < 0 ? angle + 360 : angle;
}

//---------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//---------------------------------------------------------------------------------

Tank::Tank(int x, int y, TankColor color) : color(color) {
  // Set tank attributes base on the tank color
  switch (color) {
  case T_COLOR_BROWN:
    movement = T_MOVEMENT_STATIONARY;
    bullet_speed = B_SPEED_NORMAL;
    fire_rate_cooldown = T_COOLDOWN_SLOW;
    max_bullet_ricochets = 1;
    max_bullets = 1;
    behavior = T_BEHAVIOR_PASSIVE;
    break;
  case T_COLOR_ASH:
    movement = T_MOVEMENT_SLOW;
    bullet_speed = B_SPEED_NORMAL;
    fire_rate_cooldown = T_COOLDOWN_SLOW;
    max_bullet_ricochets = 1;
    max_bullets = 1;
    behavior = T_BEHAVIOR_DEFENSIVE;
    break;
  case T_COLOR_MARINE:
    movement = T_MOVEMENT_SLOW;
    bullet_speed = B_SPEED_FAST;
    fire_rate_cooldown = T_COOLDOWN_SLOW;
    max_bullet_ricochets = 0;
    max_bullets = 1;
    behavior = T_BEHAVIOR_DEFENSIVE;
    break;
  case T_COLOR_YELLOW:
    movement = T_MOVEMENT_NORMAL;
    bullet_speed = B_SPEED_NORMAL;
    fire_rate_cooldown = T_COOLDOWN_SLOW;
    max_bullet_ricochets = 1;
    max_bullets = 1;
    behavior = T_BEHAVIOR_INCAUTIOUS;
    break;
  case T_COLOR_PINK:
    movement = T_MOVEMENT_SLOW;
    bullet_speed = B_SPEED_NORMAL;
    fire_rate_cooldown = T_COOLDOWN_FAST;
    max_bullet_ricochets = 1;
    max_bullets = 3;
    behavior = T_BEHAVIOR_OFFENSIVE;
    break;
  case T_COLOR_GREEN:
    movement = T_MOVEMENT_STATIONARY;
    bullet_speed = B_SPEED_FAST;
    fire_rate_cooldown = T_COOLDOWN_FAST;
    max_bullet_ricochets = 2;
    max_bullets = 2;
    behavior = T_BEHAVIOR_ACTIVE;
    break;
  case T_COLOR_VIOLET:
    movement = T_MOVEMENT_NORMAL;
    bullet_speed = B_SPEED_NORMAL;
    fire_rate_cooldown = T_COOLDOWN_FAST;
    max_bullet_ricochets = 1;
    max_bullets = 5;
    behavior = T_BEHAVIOR_OFFENSIVE;
    break;
  case T_COLOR_WHITE:
    movement = T_MOVEMENT_SLOW;
    bullet_speed = B_SPEED_NORMAL;
    fire_rate_cooldown = T_COOLDOWN_FAST;
    max_bullet_ricochets = 1;
    max_bullets = 5;
    behavior = T_BEHAVIOR_OFFENSIVE;
    break;
  case T_COLOR_BLACK:
    movement = T_MOVEMENT_FAST;
    bullet_speed = B_SPEED_FAST;
    fire_rate_cooldown = T_COOLDOWN_FAST;
    max_bullet_ricochets = 0;
    max_bullets = 3;
    behavior = T_BEHAVIOR_DYNAMIC;
    break;
  default:
    movement = T_MOVEMENT_NORMAL;
    bullet_speed = B_SPEED_NORMAL;
    fire_rate_cooldown = T_COOLDOWN_CONTROLLED;
    max_bullet_ricochets = 1;
    max_bullets = 5;
    behavior = T_BEHAVIOR_CONTROLLED;
  }

  // Update the position of both sprites
  this->setPosition(x, y);
  this->setOffset(8, 8);

  // Set the oam attributes for the tank body
  this->body.id = Sprite::num_sprites++;
  this->body.palette_alpha = this->body.id;
  this->body.priority = 2;
  this->body.affine_index = this->body.id;

  // Update Sprite ID Count
  // Set the oam attributes for the tank turret
  this->turret.id = Sprite::num_sprites++;
  this->turret.palette_alpha = this->turret.id;
  this->turret.priority = 1;
  this->turret.affine_index = this->turret.id;

  // Initialize the tank body and turret graphics
  this->body.sprite_sheet_pos = {0, 0 + color};
  this->turret.sprite_sheet_pos = {3, 0 + color};
  this->body.initGfx();
  this->turret.initGfx();

  // Set the initial animation frames
  this->body.copyGfxFrameToVRAM();
  this->turret.copyGfxFrameToVRAM();
}

Tank::~Tank() {
  // Clean up any remaining bullets
  for (int i = 0; i < MAX_POSSIBLE_BULLETS; i++) {
    if (bullets[i] != nullptr) {
      delete bullets[i];
      bullets[i] = nullptr;
    }
  }
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

void Tank::interpolateBodyRotation() {
  // Normalize both angles to [0, 360) range first
  float target_angle = fmod(fmod(direction, 360.0f) + 360.0f, 360.0f);
  float current_angle =
      fmod(fmod(body.rotation_angle, 360.0f) + 360.0f, 360.0f);

  // Compute the shortest rotation direction
  float angle_diff = target_angle - current_angle;

  // Adjust for crossing over 0/360 boundary
  if (angle_diff > 180.0f) {
    angle_diff -= 360.0f;
  } else if (angle_diff < -180.0f) {
    angle_diff += 360.0f;
  }

  // Rotate in the shortest direction with smooth interpolation
  if (fabs(angle_diff) > 0.1f) { // Small threshold to prevent jittering
    if (angle_diff > 0) {
      body.rotation_angle += body_rotation_speed;
    } else {
      body.rotation_angle -= body_rotation_speed;
    }
  } else {
    body.rotation_angle = target_angle; // Snap to exact angle when very close
  }

  // Keep the angle in [0, 360) range
  body.rotation_angle =
      fmod(fmod(body.rotation_angle, 360.0f) + 360.0f, 360.0f);
}

void Tank::move(TankDirection direction, Stage *stage) {
  this->direction = direction; // Save tank direction for linear interpolation
  if (direction != body.rotation_angle) return;

  bool hasMoved = false; // For checking
  int baseSpeed = 1;     // For use with slightly slowing down diagonal speed

  // Accumulate fractional movement
  static float accumulatedX = 0.0f;
  static float accumulatedY = 0.0f;

  bool isDiagonal = (direction == T_DIR_NE) || (direction == T_DIR_SE) ||
                    (direction == T_DIR_SW) || (direction == T_DIR_NW);

  // Handle Y movement
  Position newPosY = {getPosition('x'), getPosition('y')};
  bool hasPosY =
      direction == T_DIR_NW || direction == T_DIR_N || direction == T_DIR_NE;
  bool hasNegY =
      direction == T_DIR_SW || direction == T_DIR_S || direction == T_DIR_SE;
  if (hasPosY || hasNegY) {
    float yMove = baseSpeed * (isDiagonal ? 0.707f : 1.0f);
    float testY = accumulatedY + yMove;
    int moveAmount = (int)testY;

    if (moveAmount != 0) {
      if (hasPosY) {
        newPosY.y -= moveAmount;
      } else {
        newPosY.y += moveAmount;
      }

      if (validateInput(newPosY, this, stage)) {
        setPosition('y', newPosY.y);
        hasMoved = true;
        accumulatedY =
            testY - moveAmount; // Only update accumulator if move was valid
      }
    } else {
      accumulatedY = testY; // Accumulate small movements
    }
  }

  // Handle X movement
  Position newPosX = {getPosition('x'), getPosition('y')};
  bool hasPosX =
      direction == T_DIR_NE || direction == T_DIR_E || direction == T_DIR_SE;
  bool hasNegX =
      direction == T_DIR_NW || direction == T_DIR_W || direction == T_DIR_SW;
  if (hasPosX || hasNegX) {
    float xMove = baseSpeed * (isDiagonal ? 0.707f : 1.0f);
    float testX = accumulatedX + xMove;
    int moveAmount = (int)testX;

    if (moveAmount != 0) {
      if (hasPosX) {
        newPosX.x += moveAmount;
      } else {
        newPosX.x -= moveAmount;
      }

      if (validateInput(newPosX, this, stage)) {
        setPosition('x', newPosX.x);
        hasMoved = true;
        accumulatedX =
            testX - moveAmount; // Only update accumulator if move was valid
      }
    } else {
      accumulatedX = testX; // Accumulate small movements
    }
  }

  if (hasMoved && Stage::frame_counter >= body.anim_speed) {
    body.incrementAnimationFrame(true);
    body.copyGfxFrameToVRAM();
    Stage::frame_counter = 0;
  }
}

void Tank::rotateTurret(touchPosition &touch) {
  Position tankPos = getPosition();
  int tankCenterX = tankPos.x + body.tile_offset.x;
  int tankCenterY = tankPos.y + body.tile_offset.y;
  // Calculate the angle between the tank and the touch position
  float angle = calculateAngle(tankCenterX, tankCenterY, touch.px, touch.py);
  turret.rotation_angle = 270 - angle;
}

void Tank::updateOAM() {
  interpolateBodyRotation();

  // Adjust for rotational miscalculations
  switch (direction) {
  case T_DIR_SE: // Tank_Southeast
    body.tile_offset.x += 1;
    break;
  case T_DIR_S: // Tank_South
    body.tile_offset.x += 1;
    break;
  case T_DIR_SW: // Tank_Southwest
    body.tile_offset.x += 1;
    body.tile_offset.y += 1;
    break;
  case T_DIR_W: // Tank_West
    body.tile_offset.y += 1;
    break;
  case T_DIR_NW: // Tank_Northwest
    body.tile_offset.y += 1;
    break;
  default:
    body.tile_offset.x = 8;
    body.tile_offset.y = 8;
  }

  // Update the OAM
  body.updateOAM();
  turret.updateOAM();

  // Reset rotational adjustment
  setOffset(8, 8);
}
