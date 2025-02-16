#ifndef BULLET_H
#define BULLET_H

#include "Sprite.h"

enum BulletSpeed { B_SPEED_NORMAL = 1, B_SPEED_FAST = 2 };

struct Bullet : Sprite {
  BulletSpeed speed = B_SPEED_NORMAL;
  float direction;       // Should awlays match rotation_angle, but included for
                         // readability
  int max_ricochets;     // The max number of ricochets the bullet can do
  int num_ricochets = 0; // The current number of ricochets the bullet has

  Position sprite_sheet_pos = {12, 1};

  /**
   * @brief Bullet constructor, initialize variables
   */
  Bullet(float direction, int max_ricochets);

  /*
   * @brief Updates the selected animation frame of the tank
   *        sprites based on its state.
   */
  // void updateAnimationFrames();
};

#endif // BULLET_H