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
#include "Bullet.h"
#include "Sprite.h"
#include "Stage.h"
#include <math.h>
#include <gl2d.h>

#include <stdio.h>

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
// PRIVATE STRUCT FUNCTIONS
//
//---------------------------------------------------------------------------------

bool Tank::isWithinBounds(Position &pos) {
  bool tooFarUp = pos.y < 0;
  bool tooFarLeft = pos.x < 0;
  bool tooFarDown = pos.y + this->height - 1 >= SCREEN_HEIGHT;
  bool tooFarRight = pos.x + this->width - 1 >= SCREEN_WIDTH;
  return !tooFarUp && !tooFarLeft && !tooFarDown && !tooFarRight;
}

bool Tank::noTanksCollided(Position &pos) {
  for (int i = 0; i < stage->num_tanks; i++) {
    if (this == stage->tanks->at(i)) continue; // Skip checking against itself

    // Grab the tank position
    Position tankPos = stage->tanks->at(i)->getPosition();

    // Check for overlap on the x and y axes
    bool xOverlap = !(pos.x + width <= tankPos.x ||
                      tankPos.x + stage->tanks->at(i)->width <= pos.x);
    bool yOverlap = !(pos.y + height <= tankPos.y ||
                      tankPos.y + stage->tanks->at(i)->height <= pos.y);

    if (xOverlap && yOverlap) {
      return false; // Collision detected
    }
  }
  return true; // No collisions found
}

bool Tank::noBarrierCollisions(Position &pos) {
  // Check each corner of the tank for collisions with barriers
  int x1 = pos.x;
  int y1 = pos.y;
  int x2 = pos.x + width - 1;
  int y2 = pos.y + height - 1;

  // Ensure the tank is within the bounds of the stage
  if (x1 < 0 || y1 < 0 || x2 >= SCREEN_WIDTH || y2 >= SCREEN_HEIGHT) {
    return false; // Out of bounds, treat as a collision
  }

  // Check the four corners of the tank for standard barrier collisions
  if (stage->barriers[y1][x1] == 1 || stage->barriers[y1][x2] == 1 ||
      stage->barriers[y2][x1] == 1 || stage->barriers[y2][x2] == 1) {
    return false; // Collision detected
  }

  // Check the four corners of the tank for hole barrier collisions
  if (stage->barriers[y1][x1] == 2 || stage->barriers[y1][x2] == 2 ||
      stage->barriers[y2][x1] == 2 || stage->barriers[y2][x2] == 2) {
    return false; // Collision detected
  }

  return true; // No collisions with barriers
}

bool Tank::validateMove(Position &pos) {
  return isWithinBounds(pos) && noTanksCollided(pos) &&
         noBarrierCollisions(pos);
}

void Tank::addPositionHistory() {
  // Update position history
  if (position_history_counter >= 3) {
    position_history_counter = 0;
    position_history.push_back({getPosition(), direction});
  }
  position_history_counter++;
}

//---------------------------------------------------------------------------------
//
// PUBLIC STRUCT FUNCTIONS
//
//---------------------------------------------------------------------------------

Tank::Tank(Stage *stage, int x, int y, TankColor color, TankDirection direction)
    : stage(stage), color(color) {
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

  // Create the sprite objects
  this->body = new Sprite();
  this->turret = new Sprite();
  this->explosion = new Sprite();

  // Update the position of both sprites
  this->setPosition(x, y);
  this->setOffset(8, 8);

  // Set the oam attributes for the tank body
  this->body->id = Sprite::num_sprites++;
  this->body->palette_alpha = this->body->id;
  this->body->affine_index = this->body->id;
  this->body->priority = 3;

  // Update Sprite ID Count
  // Set the oam attributes for the tank turret
  this->turret->id = Sprite::num_sprites++;
  this->turret->palette_alpha = this->turret->id;
  this->turret->affine_index = this->turret->id;
  this->turret->priority = 1;
  this->turret->tile_offset = { 8, 8 };

  // Initialize the explosion animation
  this->explosion->id = Sprite::num_sprites++;
  this->explosion->palette_alpha = this->explosion->id;
  this->explosion->affine_index = -1;
  this->explosion->priority = 1;
  this->explosion->hide = true;
  this->explosion->num_anim_frames = 6;
  this->explosion->anim_speed = 3;

  // Set the sprite sheet positions for the tank's sprites
  this->body->sprite_sheet_pos = {0, 0 + color};
  this->turret->sprite_sheet_pos = {3, 0 + color};
  this->explosion->sprite_sheet_pos = {1, 12};

  // Initialize the graphics for the sprites
  this->body->initGfx();
  this->turret->initGfx();
  this->explosion->initGfx();

  // Display the initial graphics frames on screen
  this->body->copyGfxFrameToVRAM();
  this->turret->copyGfxFrameToVRAM();
  this->explosion->copyGfxFrameToVRAM();

  // Create and initialize the bullet sprites for this tank
  createBullets();

  // Face tank in initial direction
  faceDirection(direction);
}

Tank::~Tank() {
  // Clean up any remaining bullets
  for (int i = (int)bullets.size() - 1; i >= 0; i--) {
    if (bullets[i] != nullptr) {
      delete bullets[i];
      bullets[i] = nullptr;
    }
  }

  // Clean up tank body and turret sprites
  if (body != nullptr) {
    delete body;
    body = nullptr;
  }

  if (turret != nullptr) {
    delete turret;
    turret = nullptr;
  }
}

void Tank::setPosition(char axis, int value) {
  int x = getPosition('x');
  int y = getPosition('y');

  if (axis == 'x') x = value;
  else if (axis == 'y') y = value;

  // Update sprite positions
  body->pos = { x, y };
  turret->pos = { x, y };
  explosion->pos = { x, y };

  addPositionHistory();
}

void Tank::setPosition(int x, int y) {
  // Update sprite positions
  body->pos = {x, y};
  turret->pos = {x, y};
  explosion->pos = {x, y};

  addPositionHistory();
}

int Tank::getPosition(char axis) {
  if (axis == 'x') {
    return body->pos.x;
  } else if (axis == 'y') {
    return body->pos.y;
  }
  return -1;
}

void Tank::setOffset(int x, int y) {
  body->tile_offset = {x, y};
  explosion->tile_offset = {x, y};
}

Position &Tank::getPosition() { return body->pos; }

Position Tank::getOffsetPosition() {
  return { body->pos.x - body->tile_offset.x,
           body->pos.y - body->tile_offset.y };
}

void Tank::interpolateBodyRotation() {
  // Normalize both angles to [0, 360) range first
  float target_angle = direction;
  float current_angle = body->rotation_angle;

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
      body->rotation_angle += body_rotation_speed;
    } else {
      body->rotation_angle -= body_rotation_speed;
    }
  } else {
    body->rotation_angle = target_angle; // Snap to exact angle when very close
  }

  // Keep the angle in [0, 360) range
  body->rotation_angle =
      fmod(fmod(body->rotation_angle, 360.0f) + 360.0f, 360.0f);
}

void Tank::move(TankDirection direction) {
  this->direction = direction; // Save tank direction for linear interpolation
  if (direction != body->rotation_angle) return;

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

      if (validateMove(newPosY)) {
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

      if (validateMove(newPosX)) {
        setPosition('x', newPosX.x);
        hasMoved = true;
        accumulatedX =
            testX - moveAmount; // Only update accumulator if move was valid
      }
    } else {
      accumulatedX = testX; // Accumulate small movements
    }
  }

  if (hasMoved) {
    body->incrementAnimationFrame(true);
    body->copyGfxFrameToVRAM();
  }
}

void Tank::rotateTurret(Position pos) {
  // Calculate the angle between the tank and the touch position
  Position center = getPosition();
  center.x += TANK_SIZE / 2;
  center.y += TANK_SIZE / 2;
  float angle = calculateAngle(center.x, center.y, pos.x, pos.y);
  turret->rotation_angle = fmod(270 - angle, 360.0f);
  if (turret->rotation_angle < 0) turret->rotation_angle += 360.0f;
}

void Tank::rotateTurret(touchPosition &touch) {
  rotateTurret({ touch.px, touch.py });
}

void Tank::rotateTurret(int angle) {
  turret->rotation_angle = angle;
}

void Tank::faceDirection(TankDirection direction) {
  // Set the tank's direction
  this->direction = direction;
  // Rotate the turret
  turret->rotation_angle = direction;
  // Rotate the tank body
  body->rotation_angle = direction;
}

void Tank::createBullets() {
  // Initialize all bullet pointers
  for (int i = 0; i < max_bullets; i++) {
    // Create new Bullet with this tank as owner
    bullets.push_back(new Bullet(stage, this, bullet_speed, max_bullet_ricochets));
  }
}

void Tank::fire() {
  // Fire the next available bullet
  for (int i = 0; i < max_bullets; i++) {
    if (bullets[i]->in_flight) continue;
    bullets[i]->fire();
    break;
  }
}

void Tank::updateBulletPositions() {
  for (int i = 0; i < max_bullets; i++) {
    bullets[i]->updatePosition();
  }
}

void Tank::drawTreadmarks() {
  for (int i = 0; i < (int)position_history.size(); i++) {
    // Treadmark color
    int color = RGB15(24, 21, 18);
    int treadWidth = 3;
    // Left treadmark
    Position leftMarkStart = { position_history[i].pos };
    Position leftMarkEnd = { position_history[i].pos };
    // Right treadmark
    Position rightMarkStart = { position_history[i].pos };
    Position rightMarkEnd = { position_history[i].pos };
    switch(position_history[i].dir) {
      case T_DIR_N:
        // Left - Start
        leftMarkStart.y += TANK_SIZE;
        // Left - End
        leftMarkEnd.x += treadWidth;
        leftMarkEnd.y = leftMarkStart.y;
        // Right - Start
        rightMarkStart.x += TANK_SIZE - treadWidth - 1;
        rightMarkStart.y = leftMarkStart.y;
        // Right - End
        rightMarkEnd.x = rightMarkStart.x + treadWidth;
        rightMarkEnd.y = rightMarkStart.y;
        break;
      case T_DIR_S:
        // Left - Start (no change needed)
        // Left - End
        leftMarkEnd.x += treadWidth;
        // Right - Start
        rightMarkStart.x += TANK_SIZE - treadWidth - 1;
        // Right - End
        rightMarkEnd.x = rightMarkStart.x + treadWidth;
        break;
      case T_DIR_E:
        // Left - Start (no change needed)
        // Left - End
        leftMarkEnd.y += treadWidth;
        // Right - Start
        rightMarkStart.y += TANK_SIZE - treadWidth - 1;
        // Right - End
        rightMarkEnd.y = rightMarkStart.y + treadWidth;
        break;
      case T_DIR_W:
        // Left - Start
        leftMarkStart.x += TANK_SIZE;
        // Left - End
        leftMarkEnd.x = leftMarkStart.x;
        leftMarkEnd.y += treadWidth;
        // Right - Start
        rightMarkStart.x = leftMarkStart.x;
        rightMarkStart.y += TANK_SIZE - treadWidth - 1;
        // Right - End
        rightMarkEnd.x = rightMarkStart.x;
        rightMarkEnd.y = rightMarkStart.y + treadWidth;
        break;
      case T_DIR_NE:
        // Left - Start
        leftMarkStart.x -= 2;
        leftMarkStart.y += 8;
        // Left - End
        leftMarkEnd.x = leftMarkStart.x + 2;
        leftMarkEnd.y = leftMarkStart.y + 2;
        // Right - Start
        rightMarkStart.x = leftMarkStart.x + 9;
        rightMarkStart.y = leftMarkStart.y + 9;
        // Right - End
        rightMarkEnd.x = leftMarkStart.x + 11;
        rightMarkEnd.y = leftMarkStart.y + 11;
        break;
      case T_DIR_NW:
        // Left - Start
        leftMarkStart.x += 8;
        leftMarkStart.y += TANK_SIZE + 3;
        // Left - End
        leftMarkEnd.x = leftMarkStart.x + 2;
        leftMarkEnd.y = leftMarkStart.y - 4;
        // Right - Start
        rightMarkStart.x = leftMarkStart.x + 8;
        rightMarkStart.y = leftMarkStart.y - 8;
        // Right - End
        rightMarkEnd.x = rightMarkStart.x + 2;
        rightMarkEnd.y = rightMarkStart.y - 4;
        break;
      case T_DIR_SE:
        // Left - Start
        leftMarkStart.x -= 3;
        leftMarkStart.y += 9;
        // Left - End
        leftMarkEnd.x = leftMarkStart.x + 2;
        leftMarkEnd.y = leftMarkStart.y - 4;
        // Right - Start
        rightMarkStart.x = leftMarkStart.x + 10;
        rightMarkStart.y = leftMarkStart.y - 12;
        // Right - End
        rightMarkEnd.x = rightMarkStart.x - 4;
        rightMarkEnd.y = rightMarkStart.y + 2;
        break;
      case T_DIR_SW:
        // Left - Start
        leftMarkStart.x += 8;
        leftMarkStart.y -= 4;
        // Left - End
        leftMarkEnd.x = leftMarkStart.x + 2;
        leftMarkEnd.y = leftMarkStart.y + 2;
        // Right - Start
        rightMarkStart.x = leftMarkStart.x + 11;
        rightMarkStart.y = leftMarkStart.y + 11;
        // Right - End
        rightMarkEnd.x = rightMarkStart.x - 4;
        rightMarkEnd.y = rightMarkStart.y - 4;
        break;
    };
    glLine(leftMarkStart.x, leftMarkStart.y, leftMarkEnd.x, leftMarkEnd.y, color);
    glLine(rightMarkStart.x, rightMarkStart.y, rightMarkEnd.x, rightMarkEnd.y, color);
  }
}

void Tank::explode() {
  // Play the explosion animation
  alive = false;
  body->hide = true;
  turret->hide = true;
  explosion->hide = false;

  body->updateOAM();
  turret->updateOAM();
}

void Tank::reset() {
  // Reset the tank to its initial state
  alive = true;
  body->hide = false;
  turret->hide = false;
  explosion->hide = true;

  setPosition(position_history[0].pos.x, position_history[0].pos.y);
  direction = T_DIR_N;
  body->rotation_angle = 0;
  turret->rotation_angle = 0;

  body->updateOAM();
  turret->updateOAM();
  explosion->updateOAM();
}

void Tank::updateOAM() {
  // Handle explosion on death
  if (body->hide == true) {
    explosion->copyGfxFrameToVRAM();
    explosion->incrementAnimationFrame(false, false);
    explosion->updateOAM();
    return;
  }

  interpolateBodyRotation();

  // Adjust for rotational miscalculations
  switch (direction) {
  case T_DIR_SE: // Tank_Southeast
    body->tile_offset.x += 1;
    break;
  case T_DIR_S: // Tank_South
    body->tile_offset.x += 1;
    break;
  case T_DIR_SW: // Tank_Southwest
    body->tile_offset.x += 1;
    body->tile_offset.y += 1;
    break;
  case T_DIR_W: // Tank_West
    body->tile_offset.y += 1;
    break;
  case T_DIR_NW: // Tank_Northwest
    body->tile_offset.y += 1;
    break;
  default:
    body->tile_offset.x = 8;
    body->tile_offset.y = 8;
  }

  // Update the OAM
  body->updateOAM();
  turret->updateOAM();

  // Reset rotational adjustment
  setOffset(8, 8);
}