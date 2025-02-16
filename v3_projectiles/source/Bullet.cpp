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
  sprite_sheet_pos = {0, 11};
  // Make sure the bullet is centered
  tile_offset = {8, 14};
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

  // Convert angle to match standard coordinate system
  float adjusted_angle;
  if (direction == 270) {
    // East case (270° -> 0°)
    adjusted_angle = 0;
  } else if (direction >= 0) {
    // Convert 0-180° range
    adjusted_angle = 90 - direction; // This inverts the direction properly
  } else {
    // Handle negative angles (-89.99° to 0°)
    adjusted_angle = 90 - direction; // Same formula works for negative angles
  }

  int scaled_direction = (int)(direction * 100);
  iprintf("direction: %d.%02d\n", scaled_direction / 100,
          scaled_direction % 100);
  int adjusted_angle2 = (int)(adjusted_angle * 100);
  iprintf("adjusted_angle: %d.%02d\n", adjusted_angle2 / 100,
          adjusted_angle2 % 100);

  // Convert to radians
  float angle_rad = adjusted_angle * (M_PI / 180.0f);

  // Calculate velocity components based on direction and speed
  float vel_x = cos(angle_rad) * speed;
  float vel_y = sin(angle_rad) * speed;

  // Store velocity for use in update logic
  velocity = {vel_x, vel_y};
}

void Bullet::updatePosition() {
  // Exit early if not in-flight
  if (!in_flight) return;

  // Update position based on velocity
  pos.x += velocity.x;
  pos.y += velocity.y;

  if (Stage::frame_counter % anim_speed == 0) {
    incrementAnimationFrame();
    copyGfxFrameToVRAM();
  }
}