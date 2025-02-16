/*---------------------------------------------------------------------------------

input.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "input.h"

//---------------------------------------------------------------------------------
//
// HELPER FUNTIONS
//
//---------------------------------------------------------------------------------

bool isWithinBounds(Position &pos, Tank *tank) {
  bool tooFarUp = pos.y < 0;
  bool tooFarLeft = pos.x < 0;
  bool tooFarDown = pos.y + tank->height - 1 >= SCREEN_HEIGHT;
  bool tooFarRight = pos.x + tank->width - 1 >= SCREEN_WIDTH;
  return !tooFarUp && !tooFarLeft && !tooFarDown && !tooFarRight;
}

bool noTanksCollided(Position &pos, Tank *tank, Stage *stage) {
  for (int i = 0; i < stage->num_tanks; i++) {
    if (tank == stage->tanks[i]) continue; // Skip checking against itself

    // Grab the tank position
    Position tankPos = stage->tanks[i]->getPosition();

    // Check for overlap on the x and y axes
    bool xOverlap = !(pos.x + tank->width <= tankPos.x ||
                      tankPos.x + stage->tanks[i]->width <= pos.x);
    bool yOverlap = !(pos.y + tank->height <= tankPos.y ||
                      tankPos.y + stage->tanks[i]->height <= pos.y);

    if (xOverlap && yOverlap) {
      return false; // Collision detected
    }
  }
  return true; // No collisions found
}

bool noBarrierCollisions(Position pos, Tank *tank, Stage *stage) {
  // Check each corner of the tank for collisions with barriers
  int x1 = pos.x;
  int y1 = pos.y;
  int x2 = pos.x + tank->width - 1;
  int y2 = pos.y + tank->height - 1;

  // Ensure the tank is within the bounds of the stage
  if (x1 < 0 || y1 < 0 || x2 >= SCREEN_WIDTH || y2 >= SCREEN_HEIGHT) {
    return false; // Out of bounds, treat as a collision
  }

  // Check the four corners of the tank for barrier collisions
  if (stage->barriers[y1][x1] == 1 || stage->barriers[y1][x2] == 1 ||
      stage->barriers[y2][x1] == 1 || stage->barriers[y2][x2] == 1) {
    return false; // Collision detected
  }

  return true; // No collisions with barriers
}

bool validateInput(Position &pos, Tank *tank, Stage *stage) {
  return isWithinBounds(pos, tank) && noTanksCollided(pos, tank, stage) &&
         noBarrierCollisions(pos, tank, stage);
}

//---------------------------------------------------------------------------------
//
// MAIN INPUT HANDLERS
//
//---------------------------------------------------------------------------------

/**
 * @brief Handles user input to update the tank's position.
 * @param tank The tank object to update.
 * @param stage The stage to handle direction input on
 */
void handleDirectionInput(Tank *tank, Stage *stage) {
  // Scan for keys
  scanKeys();
  int keys = keysHeld();

  // Set initial direction based on combined key presses
  TankDirection direction = tank->direction;
  if (keys & KEY_LEFT) {
    if (keys & KEY_UP) {
      direction = T_NW;
    } else if (keys & KEY_DOWN) {
      direction = T_SW;
    } else {
      direction = T_W;
    }
  } else if (keys & KEY_RIGHT) {
    if (keys & KEY_UP) {
      direction = T_NE;
    } else if (keys & KEY_DOWN) {
      direction = T_SE;
    } else {
      direction = T_E;
    }
  } else if (keys & KEY_UP) {
    direction = T_N;
  } else if (keys & KEY_DOWN) {
    direction = T_S;
  }

  // If any direction was pressed
  if (keys & KEY_UP || keys & KEY_RIGHT || keys & KEY_DOWN || keys & KEY_LEFT) {
    tank->move(direction, stage);
  }
}

/**
 * @brief Handles touch input to update the tank's turret angle.
 * @param playerTank The tank object to update.
 * @param cursor The player's cursor sprite.
 */
void handleTouchInput(Tank *playerTank, Cursor *cursor) {
  // Scan for keys
  scanKeys();
  int keys = keysHeld();
  // Touch Input
  touchPosition touch;
  touchRead(&touch);
  // Handle touch input
  if (keys & KEY_TOUCH) {
    cursor->hide = false;                    // Show the cursor
    cursor->setPosition(touch.px, touch.py); // Update the cursor position
    cursor->connectToTank(playerTank);       // Draw the dotted line
    playerTank->rotateTurret(touch);         // Rotate the tank turret
  } else {
    cursor->hide = true;
  }
}
