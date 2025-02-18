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
#include "bullet-sprite.h"
#include "math.h"

//---------------------------------------------------------------------------------
//
// PRIVATE STRUCT FUNCTIONS
//
//---------------------------------------------------------------------------------

float Bullet::calculateReflectionDirection(BulletRicochetDir wallDir) {
  switch (wallDir) {
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
  // Top left corner of bullet
  Position bTopLeft = {pos.x, pos.y};
  Position bBotRight = {pos.x + width - 1 + (2 * sprite_offset.x),
                        pos.y + height - 1 + (2 * sprite_offset.y)};

  // Check for barriers or screen bounds to the north and south
  for (int i = bTopLeft.x + 1; i < bBotRight.x; i++) {
    if (bTopLeft.y < 0 || stage->barriers[bTopLeft.y][i] == 1) {
      return B_RIC_DIR_N;
    } else if (bBotRight.y >= SCREEN_HEIGHT ||
               stage->barriers[bBotRight.y][i] == 1) {
      return B_RIC_DIR_S;
    }
  }

  // Check for barriers or screen bounds to the west and east
  for (int i = bTopLeft.y + 1; i < bBotRight.y; i++) {
    if (bTopLeft.x < 0 || stage->barriers[i][bTopLeft.x] == 1) {
      return B_RIC_DIR_W;
    } else if (bBotRight.x >= SCREEN_WIDTH ||
               stage->barriers[i][bBotRight.x] == 1) {
      return B_RIC_DIR_E;
    }
  }

  return B_NO_RICOCHET; // No collisions with barriers
}

void Bullet::updateDirection(float direction) {
  this->direction = direction;

  // Convert to standard mathematical angle (clockwise from East = 0°)
  float math_angle = 90 - direction;
  float angle_rad = math_angle * (M_PI / 180.0f);

  // Calculate velocity components
  velocity = {-cosf(angle_rad) * speed, -sinf(angle_rad) * speed};
  // Reset sub_pixel
  sub_pixel = {0.0f, 0.0f};
}

//-------------------------------------------------------------------------------
//
// PUBLIC STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

Bullet::Bullet(Stage *stage, BulletSpeed speed, int max_ricochets)
    : stage(stage), speed(speed), max_ricochets(max_ricochets) {

  // Set the bitmap data from bullet sprite
  sprite_offset = {-1, -1};
  setPaletteData(bullet_spritePal);
  setBitmapData(bullet_spriteBitmap, 4, 8, 8);
}

void Bullet::fire(Position position, float direction) {
  this->in_flight = true;
  this->visible = true;
  this->pos = {position.x + 8, position.y - 10};
  updateDirection(direction);
}

void Bullet::updatePosition() {
  if (!in_flight) return;

  // Keep track of sub-pixel position
  sub_pixel.x += velocity.x;
  sub_pixel.y += velocity.y;

  // New Position
  Position newPos = {pos.x, pos.y};

  // Update position when accumulated change >= 1 pixel
  while (sub_pixel.x >= 1.0f) {
    newPos.x += 1;
    sub_pixel.x -= 1.0f;
  }
  while (sub_pixel.x <= -1.0f) {
    newPos.x -= 1;
    sub_pixel.x += 1.0f;
  }

  while (sub_pixel.y >= 1.0f) {
    newPos.y += 1;
    sub_pixel.y -= 1.0f;
  }
  while (sub_pixel.y <= -1.0f) {
    newPos.y -= 1;
    sub_pixel.y += 1.0f;
  }

  BulletRicochetDir dir = checkForRicochet(newPos);
  if (dir != B_NO_RICOCHET) {
    float new_dir = calculateReflectionDirection(dir);
    updateDirection(new_dir);
  } else {
    pos = {newPos.x, newPos.y};
  }
}

void Bullet::draw() {
  if (!in_flight) return;
  BitmapSprite::draw(); // Draw the red square
}