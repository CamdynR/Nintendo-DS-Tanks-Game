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

#include "calico/types.h"
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

struct Position {
  int x;
  int y;
};

struct Cursor {
  Position pos = {100, 100};

  u16 *sprite_gfx_mem;
  u8 *sprite_frame_gfx;

  int height = 14;
  int width = 14;
};

struct Tank {
  Position pos;

  u16 *body_gfx_mem;
  u8 *body_frame_gfx;

  u16 *turret_gfx_mem;
  u8 *turret_frame_gfx;

  int color;
  int anim_frame = 0;

  int height = 16;
  int width = 16;

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
const int SPRITE_SIZE = 32;
const int ANIMATION_SPEED = 2;
int frameCounter = 0;
Cursor cursor;
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
    // Only update the animation frame when the frame counter reaches the
    // animation speed
    if (frameCounter >= ANIMATION_SPEED) {
      // Animate the tank sprite
      tank.anim_frame = (tank.anim_frame - 1 + 3) % 3;
      animateSprite(&tank);
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
  float angle =
      calculateAngle(tank.pos.x, tank.pos.y, cursor.pos.x, cursor.pos.y);
  tank.turret_angle = 270 - angle; // Update turret angle based on touch input
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

/**
 * @brief Draws a dotted line with exactly 8 small circles connecting two
 * points.
 * @param x1 The x-coordinate of the first point.
 * @param y1 The y-coordinate of the first point.
 * @param x2 The x-coordinate of the second point.
 * @param y2 The y-coordinate of the second point.
 */
void drawDottedLine(int x1, int y1, int x2, int y2) {
  int dx = x2 - x1;
  int dy = y2 - y1;
  int steps = 8; // Always have exactly 8 dots
  float xIncrement = dx / (float)steps;
  float yIncrement = dy / (float)steps;
  float x = x1;
  float y = y1;

  glBegin2D();
  for (int i = 0; i <= steps; i++) {
    glBoxFilled(x - 1, y - 1, x + 1, y + 1,
                RGB15(7, 23, 31)); // Draw smaller circle
    x += xIncrement;
    y += yIncrement;
  }
  glEnd2D();
}

/**
 * @brief Handles touch input to update the cursor position.
 * @param touch The touch position data.
 * @param keys The pressed keys bitmask.
 */
void handleCursorInput(touchPosition &touch, int keys) {
  if (touch.rawx != 0 && touch.rawy != 0) {
    cursor.pos.x = touch.px - 16;
    cursor.pos.y = touch.py - 7;
    // Show the user's touch on screen
    drawDottedLine(tanks[0].pos.x + 8, tanks[0].pos.y + 8, cursor.pos.x + 15,
                   cursor.pos.y + 8);
  }
}

/**
 * @brief Processes user input to update the cursor position.
 */
void processCursorInput() {
  // Button Input
  scanKeys();
  int keys = keysHeld();
  // Touch Input
  touchPosition touch;
  touchRead(&touch);
  // Handle touch input
  if (keys & KEY_TOUCH) {
    handleCursorInput(touch, keys);
  } else {
    cursor.pos.x = -1 * SPRITE_SIZE;
    cursor.pos.y = -1 * SPRITE_SIZE;
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
  vramSetBankB(
      VRAM_B_MAIN_SPRITE); // Sprites can be used in VRAM B in this mode
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
  // glClearDepth(0);
}

/**
 * @brief Initializes the cursor sprite.
 */
void initCursor() {
  // Allocate 32x32 sprite graphics memory
  cursor.sprite_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  cursor.sprite_frame_gfx =
      (u8 *)sprite_sheetTiles + ((2 * 4) * SPRITE_SIZE * SPRITE_SIZE);
  dmaCopy(cursor.sprite_frame_gfx, cursor.sprite_gfx_mem,
          SPRITE_SIZE * SPRITE_SIZE);
}

/**
 * @brief Allocates and initializes sprite graphics for the tank body.
 * @param tank The tank object.
 * @param gfx The graphics data.
 * @param color The color of the tank.
 */
void initTankBodyGfx(Tank *tank, u8 *gfx, int color) {
  // Allocate 32x32 sprite graphics memory
  tank->body_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the body_frame_gfx pointer to the start of the sprite sheet
  tank->body_frame_gfx = gfx + ((color * 3) * SPRITE_SIZE * SPRITE_SIZE);
}

/**
 * @brief Allocates and initializes sprite graphics for the tank turret.
 * @param tank The tank object.
 * @param gfx The graphics data.
 * @param color The color of the tank.
 */
void initTankTurretGfx(Tank *tank, u8 *gfx, int color) {
  // Allocate 32x32 sprite graphics memory
  tank->turret_gfx_mem =
      oamAllocateGfx(&oamMain, SpriteSize_32x32, SpriteColorFormat_256Color);
  // Set the turret_frame_gfx pointer to the correct position in the sprite
  // sheet Assuming each sprite is 32x32 pixels and gfx is a linear array Blue
  // turret is in row 2, column 1 Red turret is in row 2, column 2
  int row = 3;        // Row 2 (0-indexed)
  int column = color; // Column 1 for blue (0), Column 2 for red (1)
  tank->turret_frame_gfx = gfx + (row * SPRITE_SIZE * SPRITE_SIZE * 2) +
                           (column * SPRITE_SIZE * SPRITE_SIZE);
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
  tank.color = color;

  initTankBodyGfx(&tank, (u8 *)sprite_sheetTiles, color);
  initTankTurretGfx(&tank, (u8 *)sprite_sheetTiles, color);

  return tank;
}

//---------------------------------------------------------------------------------
//
// ANIMATION FUNCTIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Animates the tank sprite based on its state.
 * @param tank The tank to animate.
 */
void animateSprite(Tank *tank) {
  int frame = tank->anim_frame + tank->color;
  // Calculate the offset correctly for a 32x32 sprite with 3 frames
  u8 *offset = tank->body_frame_gfx + frame * SPRITE_SIZE * SPRITE_SIZE;

  dmaCopy(offset, tank->body_gfx_mem, SPRITE_SIZE * SPRITE_SIZE);
  dmaCopy(tank->turret_frame_gfx, tank->turret_gfx_mem,
          SPRITE_SIZE * SPRITE_SIZE);
}

/**
 * @brief Updates sprite attributes for all tanks.
 * @param tanks Array of tanks to update.
 * @param numTanks Number of tanks in the array.
 */
void updateSprites(Tank tanks[], int numTanks) {
  // Update the cursor sprite's position
  oamSet(&oamMain, 0, cursor.pos.x, cursor.pos.y - 8, 1, 0, SpriteSize_32x32,
         SpriteColorFormat_256Color,
         cursor.sprite_gfx_mem, // Graphics pointer
         -1, false, false, false, false, false);

  // Update all the tank sprite positions
  for (int i = 0; i < numTanks; i++) {
    int id = i + 1;
    int rotAdjX = 0;
    int rotAdjY = 0;

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
      rotAdjX -= 1;
      break; // Down-Right
    case 4:
      angle = 180;
      rotAdjX -= 1;
      break; // Down
    case 5:
      angle = 135;
      rotAdjX -= 1;
      rotAdjY -= 1;
      break; // Down-Left
    case 6:
      angle = 90;
      rotAdjY -= 1;
      break; // Left
    case 7:
      angle = 45;
      rotAdjY -= 1;
      break; // Up-Left
    }

    // Apply rotation
    oamRotateScale(&oamMain, id, degreesToAngle(angle), 256, 256);

    // Update the tank sprite's position
    oamSet(&oamMain, id, tanks[i].pos.x - 8 + rotAdjX,
           tanks[i].pos.y - 8 + rotAdjY, 2, id, SpriteSize_32x32,
           SpriteColorFormat_256Color,
           tanks[i].body_gfx_mem, // Graphics pointer
           id, false, false, false, false, false);

    // Apply rotation to the turret
    oamRotateScale(&oamMain, id + MAX_TANKS,
                   degreesToAngle(tanks[i].turret_angle), 256, 256);

    // Update the turret sprite's position
    oamSet(&oamMain, id + MAX_TANKS, tanks[i].pos.x - 8, tanks[i].pos.y - 8, 1,
           id + 1, SpriteSize_32x32, SpriteColorFormat_256Color,
           tanks[i].turret_gfx_mem, // Graphics pointer
           id + MAX_TANKS, false, false, false, false, false);
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
  initCursor();
  // Create the Player Tank
  tanks[0] = createTank(CELL_SIZE, CELL_SIZE * 5.5, T_BLUE);
  // Create the Enemy Tank
  tanks[1] = createTank(SCREEN_WIDTH - (CELL_SIZE * 2), CELL_SIZE * 5.5, T_RED);

  animateSprite(&tanks[0]);
  animateSprite(&tanks[1]);

  while (pmMainLoop()) {
    processSpriteInput(tanks[0]);
    updateSprites(tanks, 2);
    processCursorInput();

    frameCounter++;

    glFlush(0); // Make sure frame has finished rendering
    swiWaitForVBlank();
    oamUpdate(&oamMain);
  }

  return 0;
}
