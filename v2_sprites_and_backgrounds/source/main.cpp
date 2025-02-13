/*---------------------------------------------------------------------------------

v2 of my Tank Game
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

#include "all-tanks.h"
#include "calico/types.h"
#include "nds/arm9/background.h"
#include "nds/arm9/video.h"
#include "stage-1.h"
#include "stage-1_bg.h"

//---------------------------------------------------------------------------------
//
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

struct Position {
  int x;
  int y;
};

struct Tank {
  Position pos;

  u16 *body_gfx_mem;
  u8 *body_frame_gfx;

  u16 *turret_gfx_mem;
  u8 *turret_frame_gfx;

  int color;
  int anim_frame = 0;

  int height;
  int width;

  // 0 = N, 1 = NE, 2 = E, 3 = SE, 4 = S, 5 = SW, 6 = W, 7 = NW
  int direction = 0;

  float turret_angle = 0; // Add this field to store the turret's rotation angle
};

//---------------------------------------------------------------------
// The state of the sprite (which way it is walking)
//---------------------------------------------------------------------
enum SpriteColor { T_BLUE = 0, T_RED = 1 };

//---------------------------------------------------------------------------------
//
// CONSTANTS & GLOBALS
//
//---------------------------------------------------------------------------------

const int MAX_TANKS = 2;
const int TANK_SIZE = 16;
const int CELL_SIZE = TANK_SIZE;
Tank tanks[MAX_TANKS];

void animateSprite(Tank *tank);

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
 *
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
 *
 * This function simulates placing the tank at `pos` and checks if it overlaps
 * with any other tank in the global `tanks` array.
 *
 * @param pos The position to check.
 * @param tank The tank being moved (used to determine size).
 * @return true if no collision occurs, false if a collision is detected.
 */
bool noTanksCollided(Position &pos, Tank &tank) {
  for (int i = 0; i < MAX_TANKS; i++) {
    if (&tank == &tanks[i]) continue; // Skip checking against itself

    bool xOverlap = !(pos.x + tank.width <= tanks[i].pos.x ||
                      tanks[i].pos.x + tanks[i].width <= pos.x);
    bool yOverlap = !(pos.y + tank.height <= tanks[i].pos.y ||
                      tanks[i].pos.y + tanks[i].height <= pos.y);

    if (xOverlap && yOverlap) {
      return false; // Collision detected
    }
  }
  return true; // No collisions found
}

/**
 * @brief Checks if the tank's position collides with any barriers in the stage.
 *
 * This function checks the tank's current position against the `STAGE_1` 2D
 * array to determine if it collides with any barriers. The `STAGE_1` array
 * contains 0's for open spaces and 1's for barriers.
 *
 * @param pos The position to check.
 * @param tank The tank being moved (used to determine size).
 * @return true if no collision occurs, false if a collision is detected.
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
 *
 * This function ensures that the position is within screen limits and does not
 * collide with other tanks.
 *
 * @param pos The position to validate.
 * @param tank The tank being moved (used to determine size).
 * @return true if the position is valid, false otherwise.
 */
bool validateInput(Position &pos, Tank &tank) {
  return isWithinBounds(pos, tank) && noTanksCollided(pos, tank) &&
         noBarrierCollisions(pos, tank);
}

/**
 * @brief Handles user input to update the tank's position.
 *
 * @param tank The tank object to update.
 * @param keys The pressed keys bitmask.
 */
void handleInput(Tank &tank, int &keys) {
  Position newPos = tank.pos; // Copy current position
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
  if (validateInput(newPos, tank)) tank.pos.y = newPos.y;

  newPos = tank.pos; // Reset new position in case validation failed

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
  if (validateInput(newPos, tank)) tank.pos.x = newPos.x;

  if (hasMoved) {
    // Animate the tank sprite
    tank.anim_frame = (tank.anim_frame + 1) % 3;
    animateSprite(&tank);
  }
};

/**
 * Initialize and display the stage background for the game.
 */
void initBackground() {
  // enable background 0 in 256 color mode with a 256x256 map
  // BG_TILE_BASE changes the offset where tile data is stored
  // BG_MAP_BASE gives the offset to the map data
  BGCTRL[0] = BG_TILE_BASE(1) | BG_MAP_BASE(0) | BG_COLOR_256 | BG_32x32 |
              BG_PRIORITY_3;

  // Load the correct palette for the background BEFORE sprite palette
  dmaCopy(stage_1_bgPal, BG_PALETTE, stage_1_bgPalLen);

  // Copy tile and map data to VRAM
  // use dma to copy the tile, map and palette data to VRAM
  // CHAR_BASE_BLOCK gives us the actual address of the tile data
  // SCREEN_BASE_BLOCK does the same thing for maps
  // these should match the BG_TILE_BASE and BG_MAP base numbers above
  dmaCopy(stage_1_bgTiles, (void *)CHAR_BASE_BLOCK(1), stage_1_bgTilesLen);
  dmaCopy(stage_1_bgMap, (void *)SCREEN_BASE_BLOCK(0), stage_1_bgMapLen);
}

void initSprites() {
  // Load sprite palette AFTER background palette
  dmaCopy(all_tanksPal, SPRITE_PALETTE, all_tanksPalLen);
}

/**
 * @brief Initializes the graphics system for 2D sprites.
 */
void initGraphics() {
  videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE);
  vramSetBankA(VRAM_A_MAIN_BG); // In Mode 0 2D, BG MUST be in VRAM A
  vramSetBankB(
      VRAM_B_MAIN_SPRITE); // Sprites can be used in VRAM B in this mode
  oamInit(&oamMain, SpriteMapping_1D_32, false);
  initBackground();
  initSprites();
  glScreen2D();
}

/**
 * @brief Allocates and initializes sprite graphics.
 * @return Pointer to allocated graphics memory.
 */
void initTankBodyGfx(Tank *tank, u8 *gfx, int color) {
  // Allocate 16x16 sprite graphics memory
  tank->body_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  tank->body_frame_gfx = gfx + (color * 32 * 32);
}

/**
 * @brief Allocates and initializes sprite graphics.
 * @return Pointer to allocated graphics memory.
 */
void initTankTurretGfx(Tank *tank, u8 *gfx, int color) {
  // Allocate 16x16 sprite graphics memory
  tank->turret_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the turret_frame_gfx pointer to the correct position in the sprite
  // sheet Assuming each sprite is 32x32 pixels and gfx is a linear array Blue
  // turret is in row 2, column 1 Red turret is in row 2, column 2
  int row = 3;        // Row 2 (0-indexed)
  int column = color; // Column 1 for blue (0), Column 2 for red (1)
  tank->turret_frame_gfx = gfx + (row * 32 * 32 * 2) + (column * 32 * 32);
}

/**
 * @brief Creates a tank at the specified position.
 * @param x The starting x-coordinate.
 * @param y The starting y-coordinate.
 * @param color The color of the tank (0 = blue, 1 = red)
 * @return A new Tank instance.
 */
Tank createTank(int x, int y, int color) {
  Tank tank = {{x, y}};
  tank.height = TANK_SIZE;
  tank.width = TANK_SIZE;
  tank.color = color;

  initTankBodyGfx(&tank, (u8 *)all_tanksTiles, color);
  initTankTurretGfx(&tank, (u8 *)all_tanksTiles, color);

  return tank;
}

/**
 * @brief Processes user input and updates tank positions.
 * @param tanks Array of tanks to update.
 * @param numTanks Number of tanks in the array.
 */
void processInput(Tank &tank) {
  // Button Input
  scanKeys();
  int keys = keysHeld();
  // Handle directional button input
  handleInput(tank, keys);

  // Touch Input
  touchPosition touch;
  touchRead(&touch);
  // Handle touch input
  if (keys & KEY_TOUCH) {
    float angle =
        calculateAngle(tank.pos.x - 16, tank.pos.y - 16, touch.px, touch.py);
    tank.turret_angle = 270 - angle; // Update turret angle based on touch input

    if (touch.rawx != 0 && touch.rawy != 0) {
      // Show the user's touch on screen
      // Horizontal Line
      glBegin2D();
      glLine(touch.px - 2, touch.py, touch.px + 2, touch.py, RGB15(16, 25, 28));
      // Vertical Line
      glLine(touch.px, touch.py - 2, touch.px, touch.py + 2, RGB15(16, 25, 28));
      glEnd2D();
    }
  }
}

/**
 * @brief Animates the tank sprite based on its state.
 * @param tank The tank to animate.
 */
void animateSprite(Tank *tank) {
  int frame = tank->anim_frame + tank->color;
  // Calculate the offset correctly for a 48x16 sprite with 3 frames
  u8 *offset = tank->body_frame_gfx + frame * 32 * 32;

  dmaCopy(offset, tank->body_gfx_mem, 32 * 32);
  dmaCopy(tank->turret_frame_gfx, tank->turret_gfx_mem, 32 * 32);
}

/**
 * @brief Updates sprite attributes for all tanks.
 * @param tanks Array of tanks to update.
 * @param numTanks Number of tanks in the array.
 */
void updateSprites(Tank tanks[], int numTanks) {
  for (int i = 0; i < numTanks; i++) {
    int angle = 0;
    switch (tanks[i].direction) {
    case 0:
      angle = 0;
      break; // Up
    case 1:
      angle = 315;
      break; // Up-Right
    case 2:
      angle = 270;
      break; // Right
    case 3:
      angle = 225;
      break; // Down-Right
    case 4:
      angle = 180;
      break; // Down
    case 5:
      angle = 135;
      break; // Down-Left
    case 6:
      angle = 90;
      break; // Left
    case 7:
      angle = 45;
      break; // Up-Left
    }

    // Apply rotation
    oamRotateScale(&oamMain, i, degreesToAngle(angle), 256, 256);

    // Update the tank sprite's position
    oamSet(&oamMain, i, tanks[i].pos.x - 8, tanks[i].pos.y - 8, 1, i,
           SpriteSize_32x32, SpriteColorFormat_256Color,
           tanks[i].body_gfx_mem, // Graphics pointer
           i, false, false, false, false, false);

    // Apply rotation to the turret
    oamRotateScale(&oamMain, i + MAX_TANKS,
                   degreesToAngle(tanks[i].turret_angle), 256, 256);

    // Update the turret sprite's position
    oamSet(&oamMain, i + MAX_TANKS, tanks[i].pos.x - 8, tanks[i].pos.y - 8, 0,
           i + 1, SpriteSize_32x32, SpriteColorFormat_256Color,
           tanks[i].turret_gfx_mem, // Graphics pointer
           i + MAX_TANKS, false, false, false, false, false);
  }
}

//---------------------------------------------------------------------------------
//
// MAIN
//
//---------------------------------------------------------------------------------

int main(void) {
  initGraphics();

  // Create the Player Tank
  tanks[0] = createTank(CELL_SIZE, CELL_SIZE * 5.5, T_BLUE);
  // Create the Enemy Tank
  tanks[1] = createTank(SCREEN_WIDTH - (CELL_SIZE * 2), CELL_SIZE * 5.5, T_RED);

  animateSprite(&tanks[0]);
  animateSprite(&tanks[1]);

  while (pmMainLoop()) {
    processInput(tanks[0]);
    updateSprites(tanks, 2);

    swiWaitForVBlank();
    oamUpdate(&oamMain);
  }

  return 0;
}
