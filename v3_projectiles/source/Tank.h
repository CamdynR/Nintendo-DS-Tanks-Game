#ifndef TANK_H
#define TANK_H

#include "Sprite.h"
#include "calico/types.h"
#include "sprite-sheet.h"
#include <nds.h>

//---------------------------------------------------------------------------------
//
// CONSTANTS & GLOBALS
//
//---------------------------------------------------------------------------------

const int TANK_SIZE = 16;

//---------------------------------------------------------------------
// The color for the tank to use
//---------------------------------------------------------------------
enum TankColor { T_BLUE = 0, T_RED = 1 };

//---------------------------------------------------------------------------------
//
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

struct Tank {
  // Tank body sprite
  Sprite body;

  // Turret sprite
  Sprite turret;

  int color; // Color of the tank (0 = blue, 1 = red)

  int height = TANK_SIZE; // Visual height of the tank in px within the Tile
  int width = TANK_SIZE;  // Visual height of the tank in px within the Tile

  // 0 = N, 1 = NE, 2 = E, 3 = SE, 4 = S, 5 = SW, 6 = W, 7 = NW
  int direction = 0;

  /*
   * Struct constructor
   */
  Tank(int x, int y, TankColor color, int &spriteIdCount);

  /*
   * @brief Sets the position of one of the axes for the tank
   * @param axis The axis to set (x or y)
   * @param value The value to set the axis to
   */
  void setPosition(char axis, int value);

  /*
   * @brief Sets the position of both axes for the tank
   * @param x The x-coordinate
   * @param y The y-coordinate
   */
  void setPosition(int x, int y);

  /*
   * @brief Gets the position of one of the axes for the tank
   * @param axis The axis to get (x or y)
   */
  int getPosition(char axis);

  /*
   * @brief Gets the position of both of the axes for the tank
   */
  Position &getPosition();

  /*
   * @brief Sets the offset of both of the axes for the tank
   */
  void setOffset(int x, int y);

  /*
   * @brief Animates the tank sprite based on its state.
   */
  void animate();

  /*
   * @brief Updates the OAM for both the tank body and tank turret
   */
  void updateOAM();
};

//---------------------------------------------------------------------------------
//
// FUNCTION DECLARATIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Allocates and initializes sprite graphics for the tank body.
 * @param tank The tank object.
 * @param gfx The graphics data.
 */
void initTankBodyGfx(Tank *tank, u8 *gfx);

/**
 * @brief Allocates and initializes sprite graphics for the tank turret.
 * @param tank The tank object.
 * @param gfx The graphics data.
 */
void initTankTurretGfx(Tank *tank, u8 *gfx);

#endif // TANK_H