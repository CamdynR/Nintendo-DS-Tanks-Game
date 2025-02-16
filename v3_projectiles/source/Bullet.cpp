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
#include "Math.h"
#include "Stage.h"

#include <stdio.h>

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

Bullet::Bullet(BulletSpeed speed, int max_ricochets)
    : speed(speed), max_ricochets(max_ricochets) {
  // Update sprite sheet position
  sprite_sheet_pos = {0, 12};
  // Make sure the bullet is centered
  tile_offset = {8, 8};
  // Initialize graphics and copy to VRAM
  initGfx();
  copyGfxFrameToVRAM();
}

void Bullet::fire(Position position, float direction) {
  // Update initial variables
  this->position = position;
  this->direction = direction;
  this->in_flight = true;
  this->hide = false;

  // Set initial sprite position
  this->pos = position;
  this->rotation_angle = direction;

  // Convert to standard mathematical angle (clockwise from East = 0°)
  // Your system: 0° = North, increasing CCW
  // Math system: 0° = East, increasing CCW
  float math_angle = 90 - direction;

  // Convert to radians
  float angle_rad = math_angle * (M_PI / 180.0f);

  // Calculate velocity components
  float vel_x = -cos(angle_rad) * speed;
  float vel_y = -sin(angle_rad) * speed;

  int velX = vel_x * 100;
  int velY = vel_y * 100;
  iprintf("vel_x: %d, vel_y: %d\n", velX, velY);

  // Store velocity for use in update logic
  velocity = {vel_x, vel_y};

  for (int i = 0; i < 5; i++) {
    updatePosition();
  }
}

void Bullet::updatePosition() {
  // Exit early if not in-flight
  if (!in_flight) return;

  // Keep track of sub-pixel position using member variables
  sub_pixel.x += velocity.x;
  sub_pixel.y += velocity.y;

  // Only update actual position when accumulated change >= 1 pixel
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

  if (Stage::frame_counter % anim_speed == 0) {
    incrementAnimationFrame();
    copyGfxFrameToVRAM();
  }
}