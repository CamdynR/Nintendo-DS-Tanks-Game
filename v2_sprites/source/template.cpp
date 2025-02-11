/*---------------------------------------------------------------------------------

v2 of my Tank Game
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include <nds.h>
#include <unistd.h>

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
  u16 *sprite;
  int height;
  int width;
};

//---------------------------------------------------------------------------------
//
// CONSTANTS & GLOBALS
//
//---------------------------------------------------------------------------------

const int MAX_TANKS = 2;
const int TANK_SIZE = 16;
const int CELL_SIZE = TANK_SIZE;
Tank tanks[MAX_TANKS];

//---------------------------------------------------------------------------------
//
// HELPER FUNCTIONS
//
//---------------------------------------------------------------------------------

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
  return isWithinBounds(pos, tank) && noTanksCollided(pos, tank);
}

/**
 * @brief Handles user input to update the tank's position.
 *
 * @param tank The tank object to update.
 * @param keys The pressed keys bitmask.
 */
void handleInput(Tank &tank, int &keys) {
  Position newPos = tank.pos; // Copy current position

  // Update user's up and down position
  if (keys & KEY_UP) newPos.y -= 1;
  if (keys & KEY_DOWN) newPos.y += 1;
  if (validateInput(newPos, tank)) tank.pos.y = newPos.y;

  newPos = tank.pos; // Reset new position in case validation failed

  // Update user's left and right position
  if (keys & KEY_LEFT) newPos.x -= 1;
  if (keys & KEY_RIGHT) newPos.x += 1;
  if (validateInput(newPos, tank)) tank.pos.x = newPos.x;
};

/**
 * @brief Initializes the graphics system for 2D sprites.
 */
void initGraphics() {
  videoSetMode(MODE_0_2D);
  vramSetBankA(VRAM_A_MAIN_SPRITE);
  oamInit(&oamMain, SpriteMapping_1D_32, false);
}

/**
 * @brief Allocates and initializes sprite graphics.
 * @param paletteIndex The index of the color palette to use
 * @return Pointer to allocated graphics memory.
 */
u16 *createSpriteGfx(int paletteIndex) {
  u16 *gfx =
      oamAllocateGfx(&oamMain, SpriteSize_16x16, SpriteColorFormat_256Color);

  // Assign each pixel the given color index (instead of always using 1)
  for (int i = 0; i < (TANK_SIZE * TANK_SIZE) / 2; i++) {
    gfx[i] = paletteIndex | (paletteIndex << 8);
  }

  return gfx;
}

/**
 * @brief Creates a tank at the specified position.
 * @param x The starting x-coordinate.
 * @param y The starting y-coordinate.
 * @param color The RGB15 color of the tank
 * @param paletteIndex The palette index to use for the tank
 * @return A new Tank instance.
 */
Tank createTank(int x, int y, int color, int paletteIndex) {
  SPRITE_PALETTE[paletteIndex] = color; // Set the correct color
  return {{x, y}, createSpriteGfx(paletteIndex), TANK_SIZE, TANK_SIZE};
}

/**
 * @brief Processes user input and updates tank positions.
 * @param tanks Array of tanks to update.
 * @param numTanks Number of tanks in the array.
 */
void processInput(Tank &tank) {
  scanKeys();
  int keys = keysHeld();

  handleInput(tank, keys);
}

/**
 * @brief Updates sprite attributes for all tanks.
 * @param tanks Array of tanks to update.
 * @param numTanks Number of tanks in the array.
 */
void updateSprites(Tank tanks[], int numTanks) {
  for (int i = 0; i < numTanks; i++) {
    oamSet(&oamMain, i, tanks[i].pos.x, tanks[i].pos.y, // OAM index & position
           0, i + 1, // Priority & palette index
           SpriteSize_16x16, SpriteColorFormat_256Color,
           tanks[i].sprite, // Graphics pointer
           -1, false, false, false, false, false);
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
  tanks[0] = createTank(CELL_SIZE, CELL_SIZE * 5.5, RGB15(0, 31, 0), 1);
  // Create the Enemy Tank
  tanks[1] = createTank(SCREEN_WIDTH - (CELL_SIZE * 2), CELL_SIZE * 5.5,
                        RGB15(31, 0, 0), 2);

  while (pmMainLoop()) {
    processInput(tanks[0]);
    updateSprites(tanks, MAX_TANKS);

    swiWaitForVBlank();
    oamUpdate(&oamMain);
  }

  return 0;
}
