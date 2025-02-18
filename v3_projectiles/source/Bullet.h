#ifndef BULLET_H
#define BULLET_H

#include "BitmapSprite.h"
#include "Stage.h"

enum BulletSpeed { B_SPEED_NORMAL = 1, B_SPEED_FAST = 2 };
enum BulletRicochetDir {
  B_NO_RICOCHET = -1,
  B_RIC_DIR_N = 0,
  B_RIC_DIR_E = 1,
  B_RIC_DIR_S = 2,
  B_RIC_DIR_W = 3,
};

struct Velocity {
  float x;
  float y;
};

class Bullet : public BitmapSprite {
private:
  Stage *stage; // Stage bullet is attached to
  Velocity velocity;
  Velocity sub_pixel;
  BulletSpeed speed;
  bool in_flight = false;
  float direction = 0;
  int num_ricochets = 0; // Current number of in-flight ricochets
  int max_ricochets;     // Max number of ricochets allowed

  /**
   * @brief: Checks to see if a wall has been hit
   * @param wallDir the direction of the wall to reflect against
   * @returns The direction angle of reflection
   */
  float calculateReflectionDirection(BulletRicochetDir wallDir);

  /**
   * @brief: Checks to see if a wall has been hit
   * @param pos The new position to check
   * @returns BulletRicochetDir The direction the wall was hit
   */
  BulletRicochetDir checkForRicochet(Position &pos);

  /**
   * @brief: Changes directions (for use after hitting wall)
   */
  void updateDirection(float direction);

public:
  /**
   * @brief: Set initial position and direction for shooting
   * @param stage The stage to attach the bullet to
   * @param speed The speed the bullet should travel
   * @param max_ricochets The max number of ricochets the bullet has
   */
  Bullet(Stage *stage, BulletSpeed speed, int max_ricochets);

  /**
   * @brief: Set initial position and direction for shooting
   * @param position The starting position to fire the bullet from
   * @param direction The starting direction the bullet should fly in
   */
  void fire(Position position, float direction);

  /**
   * @brief: Updates the position given the bullet's velocity
   */
  void updatePosition();

  /**
   * @breif: Draw the bullet on screen
   */
  void draw() override;
};

#endif // BULLET_H