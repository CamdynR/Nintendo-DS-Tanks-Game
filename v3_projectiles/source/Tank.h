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

//---------------------------------------------------------------------------------
//
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

struct Tank : Sprite {
  // Tank body sprite
  u16 *body_gfx_mem;
  u8 *body_frame_gfx;

  // Turret sprite
  u16 *turret_gfx_mem;
  u8 *turret_frame_gfx;

  int color;          // Color of the tank (0 = blue, 1 = red)
  int anim_frame = 0; // Animation frame state (0-2)

  int height = TANK_SIZE; // Visual height of the tank in px within the Tile
  int width = TANK_SIZE;  // Visual height of the tank in px within the Tile

  // 0 = N, 1 = NE, 2 = E, 3 = SE, 4 = S, 5 = SW, 6 = W, 7 = NW
  int direction = 0;

  float turret_angle = 0; // Turret's rotational angle

  /**
   * @brief Animates the tank sprite based on its state.
   */
  void animate();
};

//---------------------------------------------------------------------
// The state of the sprite (which way it is walking)
//---------------------------------------------------------------------
enum TankColor { T_BLUE = 0, T_RED = 1 };

//---------------------------------------------------------------------------------
//
// FUNCTION DECLARATIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Allocates and initializes sprite graphics for the tank body.
 * @param tank The tank object.
 * @param gfx The graphics data.
 * @param color The color of the tank.
 */
void initTankBodyGfx(Tank *tank, u8 *gfx, int color);

/**
 * @brief Allocates and initializes sprite graphics for the tank turret.
 * @param tank The tank object.
 * @param gfx The graphics data.
 * @param color The color of the tank.
 */
void initTankTurretGfx(Tank *tank, u8 *gfx, int color);

/**
 * @brief Creates a tank at the specified position.
 * @param x The starting x-coordinate.
 * @param y The starting y-coordinate.
 * @param color The color of the tank (0 = blue, 1 = red)
 * @return A new Tank instance.
 */
Tank createTank(int x, int y, int color);

#endif // TANK_H