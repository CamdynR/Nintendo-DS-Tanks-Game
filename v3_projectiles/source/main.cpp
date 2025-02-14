/*---------------------------------------------------------------------------------

v3 of my Tank Game
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include <gl2d.h>
#include <math.h>
#include <nds.h>
#include <unistd.h>
#include <vector>

#include "Cursor.h"
#include "Position.h"
#include "Tank.h"
#include "nds/arm9/background.h"
#include "nds/arm9/video.h"
#include "sprite-sheet.h"
#include "stage-1.h"
#include "stage-1_bg.h"

//---------------------------------------------------------------------------------
//
// GLOBALS
//
//---------------------------------------------------------------------------------

// Constants
const int CELL_SIZE = TANK_SIZE;
const int ANIMATION_SPEED = 2;
// Counters
int frameCounter = 0;
int spriteIdCount = 1;
// Global Refs
Cursor *cursor = nullptr; // Player cursor
std::vector<Tank> tanks; // All tanks on screen

//---------------------------------------------------------------------------------
//
// HELPER FUNCTIONS
//
//---------------------------------------------------------------------------------


//---------------------------------------------------------------------------------
//
// INPUT HANDLING FUNCTIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Handles user input to update the tank's position.
 * @param tank The tank object to update.
 * @param keys The pressed keys bitmask.
 */
void handleDirectionInput(Tank &tank, int &keys) {
  // Set initial direction based on combined key presses
  TankDirection direction;
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

  tank.move(direction, frameCounter);
}

/**
 * @brief Handles touch input to update the tank's turret angle.
 * @param tank The tank object to update.
 * @param touch The touch position data.
 */
void handleTurretInput(Tank &tank, touchPosition &touch) {
  // Grab the tank's position
  Position tankPos = tank.getPosition();
  // Calculate the angle between the tank and the touch position
  float angle =
      calculateAngle(tankPos.x, tankPos.y, cursor->pos.x, cursor->pos.y);
  tank.turret.rotation_angle = 270 - angle;
}

/**
 * @brief Processes user input and updates tank positions.
 * @param tank The tank object to update.
 */
void processSpriteInput(Tank &tank) {
  // Button Input
  scanKeys();
  int keys = keysHeld();
  // Handle directional button input
  handleDirectionInput(tank, keys);
  // Touch Input
  touchPosition touch;
  touchRead(&touch);
  // Handle touch input
  if (keys & KEY_TOUCH) {
    handleTurretInput(tank, touch);
  }
}

//---------------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Initialize and display the stage background for the game.
 */
void initBackground() {
  // Set VRAM bank A to LCD mode so we can write a bitmap
  vramSetBankA(VRAM_A_MAIN_BG);
  int bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
  bgSetPriority(bg, 3);
  decompress(stage_1_bgBitmap, BG_GFX, LZ77Vram);
}

/**
 * @brief Initializes the sprite palette.
 */
void initSprites() {
  vramSetBankB(VRAM_B_MAIN_SPRITE);
  oamInit(&oamMain, SpriteMapping_1D_32, false);
  // Load sprite palette AFTER background palette
  dmaCopy(sprite_sheetPal, SPRITE_PALETTE, sprite_sheetPalLen);
}

/**
 * @brief Initializes the graphics system for 2D sprites.
 */
void initGraphics() {
  videoSetMode(MODE_5_3D);
  initBackground();
  initSprites();
  glScreen2D();

  // Prevent GL2D from drawing a black frame
  glClearColor(31, 31, 31, 0);
  glClearPolyID(63);
}

//---------------------------------------------------------------------------------
//
// ANIMATION FUNCTIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Updates sprite attributes for all tanks.
 */
void updateSprites() {
  // Update the cursor first and foremost
  cursor->updateOAM();
  // Update all the tank sprite positions
  for (int i = 0; i < (int)tanks.size(); i++) {
    // tanks[i].body.rotation_angle = angle;
    tanks[i].updateOAM();
  }
}

//---------------------------------------------------------------------------------
//
// MAIN
//
//---------------------------------------------------------------------------------

int main(void) {
  initGraphics();

  // Create player cursor
  cursor = new Cursor();

  // Create the Player Tank
  tanks.push_back(Tank(CELL_SIZE, CELL_SIZE * 5.5, T_BLUE, spriteIdCount));
  // Create the Enemy Tank
  tanks.push_back(Tank(SCREEN_WIDTH - (CELL_SIZE * 2), CELL_SIZE * 5.5, T_RED,
                        spriteIdCount));

  while (pmMainLoop()) {
    processSpriteInput(tanks[0]);
    updateSprites();
    processCursorInput(*cursor, tanks[0]);

    frameCounter++;

    glFlush(0); // Make sure frame has finished rendering
    swiWaitForVBlank();
    oamUpdate(&oamMain);
  }

  return 0;
}
