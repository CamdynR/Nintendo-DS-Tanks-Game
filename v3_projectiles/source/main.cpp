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
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

const int CELL_SIZE = TANK_SIZE;
const int ANIMATION_SPEED = 2;
int frameCounter = 0;
int spriteIdCount = 1;
Cursor *cursor = nullptr;
std::vector<Tank> tanks;

//---------------------------------------------------------------------------------
//
// HELPER FUNCTIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Calculates the angle between two points.
 * @param x1 The x-coordinate of the first point.
 * @param y1 The y-coordinate of the first point.
 * @param x2 The x-coordinate of the second point.
 * @param y2 The y-coordinate of the second point.
 * @return The angle in degrees.
 */
float calculateAngle(int x1, int y1, int x2, int y2) {
  float deltaX = x2 - x1;
  float deltaY = y2 - y1;
  float angle = atan2(deltaY, deltaX) * (180.0 / M_PI);
  return angle < 0 ? angle + 360 : angle;
}

/**
 * @brief Checks if the given position is within the screen bounds.
 * @param pos The position to check.
 * @param tank The tank object, used to determine width and height.
 * @return true if the position is within bounds, false otherwise.
 */
bool isWithinBounds(Position &pos, Tank &tank) {
  bool tooFarUp = pos.y < 0;
  bool tooFarLeft = pos.x < 0;
  bool tooFarDown = pos.y + tank.height - 1 >= SCREEN_HEIGHT;
  bool tooFarRight = pos.x + tank.width - 1 >= SCREEN_WIDTH;
  return !tooFarUp && !tooFarLeft && !tooFarDown && !tooFarRight;
}

/**
 * @brief Checks if a tank at the given position would collide with any other
 * tank.
 * @param pos The position to check.
 * @param tank The tank being moved (used to determine size).
 * @return true if no collision occurs, false if a collision is detected.
 */
bool noTanksCollided(Position &pos, Tank &tank) {
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

/**
 * @brief Checks if the tank's position collides with any barriers in the stage.
 * @param pos The position to check.
 * @param tank The tank being moved (used to determine size).
 * @return true if no collisions with barriers occur, false if a collision is
 * detected.
 */
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

/**
 * @brief Validates if a given position is allowed based on screen bounds and
 * collisions.
 * @param pos The position to validate.
 * @param tank The tank being moved (used to determine size).
 * @return true if the position is valid, false otherwise.
 */
bool validateInput(Position &pos, Tank &tank) {
  return isWithinBounds(pos, tank) && noTanksCollided(pos, tank) &&
         noBarrierCollisions(pos, tank);
}

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
  // Copy current position
  Position newPos = {tank.getPosition('x'), tank.getPosition('y')};
  bool hasMoved = false;

  // Update user's up and down position
  if (keys & KEY_UP) {
    newPos.y -= 1;
    tank.direction = 0;
    hasMoved = true;
  }
  if (keys & KEY_DOWN) {
    newPos.y += 1;
    tank.direction = 4; // Down
    hasMoved = true;
  }
  if (validateInput(newPos, tank)) {
    tank.setPosition('y', newPos.y);
  }

  // Reset new position in case validation failed
  newPos = {tank.getPosition('x'), tank.getPosition('y')};

  // Update user's left and right position
  if (keys & KEY_LEFT) {
    newPos.x -= 1;
    if (keys & KEY_UP) {
      tank.direction = 7; // Up-Left
    } else if (keys & KEY_DOWN) {
      tank.direction = 5; // Down-Left
    } else {
      tank.direction = 6; // Left
    }
    hasMoved = true;
  }
  if (keys & KEY_RIGHT) {
    newPos.x += 1;
    if (keys & KEY_UP) {
      tank.direction = 1; // Up-Right
    } else if (keys & KEY_DOWN) {
      tank.direction = 3; // Down-Right
    } else {
      tank.direction = 2; // Right
    }
    hasMoved = true;
  }
  if (validateInput(newPos, tank)) {
    tank.setPosition('x', newPos.x);
  }

  if (hasMoved) {
    // Only update the animation frame when the frame counter reaches the
    // animation speed
    if (frameCounter >= ANIMATION_SPEED) {
      // Animate the tank sprite
      tank.body.anim_frame = (tank.body.anim_frame - 1 + 3) % 3;
      tank.animate();
      // Reset the frame counter
      frameCounter = 0;
    }
  }
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

  // Animate the tanks
  for (int i = 0; i < (int)tanks.size(); i++) {
    tanks[i].animate();
  }

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
