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
#include "calico/gba/keypad.h"

#include <stdio.h>

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

void handleButtonInput(Stage *stage) {
  // Scan for keys
  scanKeys();
  int keys_held = keysHeld();
  int keys_down = keysDown();

  /*******************/
  /* Direction Input */
  /*******************/

  // Grab a reference to the player tank
  Tank *playerTank = stage->tanks[0];

  // Set initial direction based on combined key presses
  TankDirection direction = playerTank->direction;
  if (keys_held & KEY_LEFT) {
    if (keys_held & KEY_UP) {
      direction = T_DIR_NW;
    } else if (keys_held & KEY_DOWN) {
      direction = T_DIR_SW;
    } else {
      direction = T_DIR_W;
    }
  } else if (keys_held & KEY_RIGHT) {
    if (keys_held & KEY_UP) {
      direction = T_DIR_NE;
    } else if (keys_held & KEY_DOWN) {
      direction = T_DIR_SE;
    } else {
      direction = T_DIR_E;
    }
  } else if (keys_held & KEY_UP) {
    direction = T_DIR_N;
  } else if (keys_held & KEY_DOWN) {
    direction = T_DIR_S;
  }

  // If any direction was pressed
  if (keys_held & KEY_UP || keys_held & KEY_RIGHT || keys_held & KEY_DOWN ||
      keys_held & KEY_LEFT) {
    playerTank->move(direction, stage);
  }

  /***********************/
  /* Non-Direction Input */
  /***********************/

  // Fire Bullet
  if (keys_down & KEY_L) {
    playerTank->fire();
  }

  // Lay Mine
}

void handleTouchInput(Stage *stage, Cursor *cursor) {
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
    cursor->connectToTank(stage->tanks[0]);  // Draw the dotted line
    stage->tanks[0]->rotateTurret(touch);    // Rotate the tank turret
  } else {
    cursor->hide = true;
  }
}
