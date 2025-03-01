/*---------------------------------------------------------------------------------

Bullet.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Bullet.h"
#include "Tank.h"
#include "bullet-sprite.h"
#include "math.h"

//---------------------------------------------------------------------------------
//
// PRIVATE STRUCT FUNCTIONS
//
//---------------------------------------------------------------------------------

float Bullet::calculateReflectionDirection(BulletRicochetDir wallDir) {
  switch (wallDir) {
  case B_RIC_DIR_CORNER:
    // For corners, reflect back 180 degrees
    return fmod(direction + 180.0f, 360.0f);
  case B_RIC_DIR_N:
    if (direction < 90 && direction > 0) return 540 - direction;
    else if (direction > 270)
      return 180 - direction;
    else
      return 180;
    break;
  case B_RIC_DIR_S:
    if (direction > 180) return 540 - direction;
    else if (direction < 180)
      return 180 - direction;
    else
      return 0;
    break;
  case B_RIC_DIR_E:
    return 360 - direction;
    break;
  case B_RIC_DIR_W:
    return 360 - direction;
    break;
  default:
    return 0;
  }
}

BulletRicochetDir Bullet::checkForRicochet(Position &pos) {
  // Adjusted position for gap in sprite pixels from edge of tile
  int adjPosX = pos.x + 13;
  int adjPosY = pos.y + 13;

  bool northCollision = false;
  bool southCollision = false;
  bool eastCollision = false;
  bool westCollision = false;

  // Check North and South
  for (int i = 1; i < width - 1; ++i) {
      if (adjPosY <= 0 || stage->barriers[adjPosY][adjPosX + i] == 1) return B_RIC_DIR_N;
      if (adjPosY + height - 1 >= SCREEN_HEIGHT || stage->barriers[adjPosY + height - 1][adjPosX + i] == 1) return B_RIC_DIR_S;
  }
  // Check East and West
  for (int i = 1; i < height - 1; ++i) {
      if (adjPosX <= 0 || stage->barriers[adjPosY + i][adjPosX] == 1) return B_RIC_DIR_W;
      if (adjPosX + width - 1 >= SCREEN_WIDTH || stage->barriers[adjPosY + i][adjPosX + width - 1] == 1) return B_RIC_DIR_E;
  }

  // Check for corner collisions
  if ((northCollision && eastCollision) || 
      (northCollision && westCollision) ||
      (southCollision && eastCollision) || 
      (southCollision && westCollision)) {
      return B_RIC_DIR_CORNER;
  }

  // Regular directional checks
  if (northCollision) return B_RIC_DIR_N;
  if (southCollision) return B_RIC_DIR_S;
  if (eastCollision) return B_RIC_DIR_E;
  if (westCollision) return B_RIC_DIR_W;

  return B_NO_RICOCHET; // No collisions with barriers
}

void Bullet::updateDirection(float direction, BulletRicochetDir wallDir) {
  this->direction = direction;

  // Convert to standard mathematical angle (clockwise from East = 0°)
  float math_angle = 90 - direction;
  float angle_rad = math_angle * (M_PI / 180.0f);

  // Calculate velocity components
  velocity = {-cosf(angle_rad) * speed, -sinf(angle_rad) * speed};
  // Reset sub_pixel
  sub_pixel = {0.0f, 0.0f};
}

void Bullet::reset() {
  // Hide the bullet
  in_flight = false;
  hide = true;
  has_exploded = false;
  // Reset variables back to zero
  direction = 0;
  num_ricochets = 0;
  pos = {0, 0};
  velocity = {0, 0};
  sub_pixel = {0, 0};
}

//-------------------------------------------------------------------------------
//
// PUBLIC STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

Bullet::Bullet(Stage *stage, Tank *tank, BulletSpeed speed, int max_ricochets)
    : stage(stage), tank(tank), speed(speed), max_ricochets(max_ricochets) {

  // Set sprite sheet position
  this->sprite_sheet_pos = {3, 13};

  // Assign an ID
  this->id = Sprite::num_sprites++;
  this->palette_alpha = this->id;
  this->affine_index = -1;
  this->priority = 2;
  // Hide until shown on screen
  this->hide = true;

  // Initialize the ricochet effect graphics
  this->ricochet_effect = new Sprite();
  this->ricochet_effect->id = Sprite::num_sprites++;
  this->ricochet_effect->sprite_sheet_pos = {1, 11};
  this->ricochet_effect->palette_alpha = this->ricochet_effect->id;
  this->ricochet_effect->affine_index = -1;
  this->ricochet_effect->priority = 2;
  this->ricochet_effect->num_anim_frames = 3;
  this->ricochet_effect->anim_speed = 3;
  this->ricochet_effect->hide = true;

  // Initialize graphics and copy to VRAM
  this->initGfx();
  this->copyGfxFrameToVRAM();
  this->ricochet_effect->initGfx();
  this->ricochet_effect->copyGfxFrameToVRAM();
}

void Bullet::fire() {
  this->in_flight = true;
  this->hide = false;

  // Grab initial position from tank
  this->pos = tank->getOffsetPosition();
  // Convert angle to radians and adjust for coordinate system
  float rotation_angle = tank->turret->rotation_angle;
  float angle_rad = rotation_angle * M_PI / 180.0f;
  // Update position based on rotation angle
  this->pos.x -= (int)(12 * sin(angle_rad));
  this->pos.y -= (int)(12 * cos(angle_rad));

  // Update the position of the ricochet effect and show it
  ricochet_effect->pos = this->pos;
  this->ricochet_effect->hide = false;

  // Update direction based on tank turret rotation
  updateDirection(rotation_angle, B_NO_RICOCHET);
}

void Bullet::updatePosition() {
  if (!in_flight) return;

  // Animate the ricochet effect if it is active
  if (!ricochet_effect->hide) {
    ricochet_effect->copyGfxFrameToVRAM();
    ricochet_effect->incrementAnimationFrame(false, false);

    // Check if animation finished (reached last frame)
    if (has_exploded) {
      hide = true;
      if (ricochet_effect->hide) reset();
      return;
    }
  }

  // Keep track of sub-pixel position
  sub_pixel.x += velocity.x;
  sub_pixel.y += velocity.y;

  Position newPos = { pos.x, pos.y };
  Position stepPos = { pos.x, pos.y };
  bool collision = false;

  // Handle X movement one pixel at a time
  while (sub_pixel.x >= 1.0f || sub_pixel.x <= -1.0f) {
    int step = (sub_pixel.x > 0) ? 1 : -1;
    stepPos.x += step;
    
    BulletRicochetDir dir = checkForRicochet(stepPos);
    if (dir != B_NO_RICOCHET) {
      collision = true;
      // Show the ricochet effect
      this->ricochet_effect->hide = false;
      if (num_ricochets < max_ricochets) {
        num_ricochets++;
        float new_dir = calculateReflectionDirection(dir);
        updateDirection(new_dir, dir);
      } else {
        explode();
      }
      break;
    }
    
    newPos.x = stepPos.x;
    sub_pixel.x += (step > 0) ? -1.0f : 1.0f;
  }

  // Only check Y movement if no X collision occurred
  if (!collision) {
    while (sub_pixel.y >= 1.0f || sub_pixel.y <= -1.0f) {
      int step = (sub_pixel.y > 0) ? 1 : -1;
      stepPos.y += step;
      
      BulletRicochetDir dir = checkForRicochet(stepPos);
      if (dir != B_NO_RICOCHET) {
        // Show the ricochet effect
        this->ricochet_effect->hide = false;
        if (num_ricochets < max_ricochets) {
          num_ricochets++;
          float new_dir = calculateReflectionDirection(dir);
          updateDirection(new_dir, dir);
        } else {
          explode();
        }
        break;
      }
      
      newPos.y = stepPos.y;
      sub_pixel.y += (step > 0) ? -1.0f : 1.0f;
    }
  }

  pos = newPos;
  if (ricochet_effect->hide) {
    ricochet_effect->pos = pos;
  }
}

void Bullet::explode() {
  // Show the ricochet effect
  this->ricochet_effect->hide = false;
  // Mark has exploded, let animation finish
  has_exploded = true;
}

void Bullet::updateOAM() {
  Sprite::updateOAM();
  ricochet_effect->updateOAM();
}
