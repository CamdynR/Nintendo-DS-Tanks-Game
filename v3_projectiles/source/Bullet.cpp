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

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

Bullet::Bullet(BulletSpeed speed, int max_ricochets)
    : BitmapSprite(), speed(speed), max_ricochets(max_ricochets) {

  // Set the bitmap data from bullet sprite
  setPaletteData(bullet_spritePal);
  setBitmapData(bullet_spriteBitmap, 8,
                8); // 8x8 is the size defined in bullet-sprite.h
}

void Bullet::fire(Position position, float direction) {
  this->in_flight = true;
  this->visible = true;
  this->pos = position;
  this->rotation_angle = direction;

  // Convert to standard mathematical angle (clockwise from East = 0°)
  float math_angle = 90 - direction;
  float angle_rad = math_angle * (M_PI / 180.0f);

  // Calculate velocity components
  velocity = {-cosf(angle_rad) * speed, -sinf(angle_rad) * speed};
  // Reset sub_pixel
  sub_pixel = {0.0f, 0.0f};
}

void Bullet::updatePosition() {
  if (!in_flight) return;

  // Keep track of sub-pixel position
  sub_pixel.x += velocity.x;
  sub_pixel.y += velocity.y;

  // Update position when accumulated change >= 1 pixel
  while (sub_pixel.x >= 1.0f) {
    pos.x += 1;
    sub_pixel.x -= 1.0f;
  }
  while (sub_pixel.x <= -1.0f) {
    pos.x -= 1;
    sub_pixel.x += 1.0f;
  }

  while (sub_pixel.y >= 1.0f) {
    pos.y += 1;
    sub_pixel.y -= 1.0f;
  }
  while (sub_pixel.y <= -1.0f) {
    pos.y -= 1;
    sub_pixel.y += 1.0f;
  }
}

void Bullet::draw() {
  if (!in_flight) return;
  BitmapSprite::draw(); // Draw the red square
}