#ifndef BULLET_H
#define BULLET_H

#include "Sprite.h"
#include "Stage.h"

enum BulletSpeed { B_SPEED_NORMAL = 2, B_SPEED_FAST = 3 };
enum BulletRicochetDir {
  B_NO_RICOCHET,
  B_RIC_DIR_N,
  B_RIC_DIR_E,
  B_RIC_DIR_S,
  B_RIC_DIR_W,
  B_RIC_DIR_CORNER
};

struct Velocity {
  float x;
  float y;
};

class Bullet : public Sprite {
private:
  Stage *stage; // Stage bullet is attached to
  Tank *tank;   // The tank in the stage the bullet is attached to
  Velocity velocity;
  Velocity sub_pixel;
  BulletSpeed speed;
  float direction = 0;
  int max_ricochets;     // Max number of ricochets allowed

  Sprite *ricochet_effect;

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
  void updateDirection(float direction, BulletRicochetDir wallDir);

  /**
   * @brief: After bullet has finished firing, reset all
   *         affected variables.
   */
  void reset();

public:
  bool in_flight = false;
  bool has_exploded = false; // True when bullet has hit a tank / wall
  int num_ricochets = 0; // Current number of in-flight ricochets
  const int height = 6;
  const int width = 6;

  /**
   * @brief: Set initial position and direction for shooting
   * @param stage The stage to attach the bullet to
   * @param tank The tank in the stage to attach the bullet to
   * @param speed The speed the bullet should travel
   * @param max_ricochets The max number of ricochets the bullet has
   */
  Bullet(Stage *stage, Tank *tank, BulletSpeed speed, int max_ricochets);

  /**
   * @brief: Set initial position and direction for shooting
   */
  void fire();

  /**
   * @brief: Updates the position given the bullet's velocity
   */
  void updatePosition();

  /**
   * @brief: Marks the bullet for explosion so the necessary
   *         animations can be played and the position resets
   */
  void explode();

  /**
   * @brief: Updates the object attribute memory with new data
   */
  void updateOAM();
};

#endif // BULLET_H