#ifndef BULLET_H
#define BULLET_H

#include "BitmapSprite.h"

enum BulletSpeed { B_SPEED_NORMAL = 1, B_SPEED_FAST = 2 };

struct Velocity {
  float x;
  float y;
};

class Bullet : public BitmapSprite {
private:
  Velocity velocity;
  Velocity sub_pixel;
  BulletSpeed speed;
  bool in_flight = false;

public:
  int max_ricochets;     // Max number of ricochets allowed
  int num_ricochets = 0; // Current number of in-flight ricochets

  Bullet(BulletSpeed speed, int max_ricochets);
  void fire(Position position, float direction);
  void updatePosition();
  void draw() override;
};

#endif // BULLET_H