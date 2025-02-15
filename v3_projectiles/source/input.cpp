/*---------------------------------------------------------------------------------

input.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Tank.h"
#include "stage-1.h"
#include <vector>

bool isWithinBounds(Position &pos, Tank &tank) {
  bool tooFarUp = pos.y < 0;
  bool tooFarLeft = pos.x < 0;
  bool tooFarDown = pos.y + tank.height - 1 >= SCREEN_HEIGHT;
  bool tooFarRight = pos.x + tank.width - 1 >= SCREEN_WIDTH;
  return !tooFarUp && !tooFarLeft && !tooFarDown && !tooFarRight;
}

bool noTanksCollided(Position &pos, Tank &tank, std::vector<Tank> tanks) {
  for (int i = 0; i < (int)tanks.size(); i++) {
    if (&tank == &tanks[i]) continue; // Skip checking against itself

    // Grab the tank position
    Position tankPos = tanks[i].getPosition();

    // Check for overlap on the x and y axes
    bool xOverlap = !(pos.x + tank.width <= tankPos.x ||
                      tankPos.x + tanks[i].width <= pos.x);
    bool yOverlap = !(pos.y + tank.height <= tankPos.y ||
                      tankPos.y + tanks[i].height <= pos.y);

    if (xOverlap && yOverlap) {
      return false; // Collision detected
    }
  }
  return true; // No collisions found
}

bool noBarrierCollisions(Position pos, Tank &tank) {
  // Check each corner of the tank for collisions with barriers
  int x1 = pos.x;
  int y1 = pos.y;
  int x2 = pos.x + tank.width - 1;
  int y2 = pos.y + tank.height - 1;

  // Ensure the tank is within the bounds of the stage
  if (x1 < 0 || y1 < 0 || x2 >= SCREEN_WIDTH || y2 >= SCREEN_HEIGHT) {
    return false; // Out of bounds, treat as a collision
  }

  // Check the four corners of the tank for barrier collisions
  if (STAGE_1[y1][x1] == 1 || STAGE_1[y1][x2] == 1 || STAGE_1[y2][x1] == 1 ||
      STAGE_1[y2][x2] == 1) {
    return false; // Collision detected
  }

  return true; // No collisions with barriers
}

bool validateInput(Position &pos, Tank &tank, std::vector<Tank> tanks) {
  return isWithinBounds(pos, tank) && noTanksCollided(pos, tank, tanks) &&
         noBarrierCollisions(pos, tank);
}