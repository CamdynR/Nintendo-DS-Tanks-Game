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

  // For Testing
  if (keys_down & KEY_START) {
    for (int i = 0; i < stage->num_tanks; i++) {
      stage->tanks[i]->reset();
    }
  } else if (keys_down & KEY_SELECT) {
    for (int i = 0; i < stage->num_tanks; i++) {
      stage->tanks[i]->explode();
    }
  }

  // Grab a reference to the player tank
  Tank *playerTank = stage->tanks[0];
  // Don't perform any inputs if player is dead
  if (!playerTank->alive) return;

  /*******************/
  /* Direction Input */
  /*******************/

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
    playerTank->move(direction);
  }

  /***********************/
  /* Non-Direction Input */
  /***********************/

  // Fire Bullet
  if (keys_down & KEY_L) {
    playerTank->fire();
  }

  // Lay Mine
  // TODO
}

void handleTouchInput(Stage *stage, Cursor *cursor) {
  // Grab a reference to the player tank
  Tank *playerTank = stage->tanks[0];
  // Don't perform any inputs if player is dead
  if (!playerTank->alive) return;

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
    stage->tanks[0]->rotateTurret(touch);    // Rotate the tank turret
  } else {
    cursor->hideSprites(); // Hide the cursor and tail sprites
  }
}
