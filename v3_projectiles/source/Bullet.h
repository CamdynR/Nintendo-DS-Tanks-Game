#ifndef BULLET_H
#define BULLET_H

#include "Sprite.h"

enum BulletSpeed { B_SPEED_NORMAL = 1, B_SPEED_FAST = 2 };

struct Velocity {
  float x;
  float y;
};
class Bullet : public Sprite {
private:
  Velocity sub_pixel;

public:
  BulletSpeed speed;
  Position position;     // Set upon fire and constantly updated until hit
  Velocity velocity;     // Stores x,y velocity components
  float direction;       // Should awlays match rotation_angle, but included for
                         // readability
  int max_ricochets;     // The max number of ricochets the bullet can do
  int num_ricochets = 0; // The current number of ricochets the bullet has
  int anim_speed = 3;

  bool in_flight = false; // True if has been fired but hasn't been destroyed

  /**
   * @brief Bullet constructor, initialize variables
   */
  Bullet(BulletSpeed speed, int max_ricochets);

  /**
   * @brief Fire the bullet in a given direction
   * @param direction the angle to fire the bullet in
   */
  void fire(Position position, float direction);

  /**
   * @brief If in flight, updates it's position accordingly
   */
  void updatePosition();
};

#endif // BULLET_H