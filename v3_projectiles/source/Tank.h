#ifndef TANK_H
#define TANK_H

#include "Bullet.h"
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
// ENUMS
//
//---------------------------------------------------------------------------------

enum TankColor {
  // Player Colors
  T_COLOR_BLUE = 0, // Player 1
  T_COLOR_RED = 1,  // Player 2
  // Computer Colors
  T_COLOR_BROWN = 2,
  T_COLOR_ASH = 3,
  T_COLOR_MARINE = 4,
  T_COLOR_YELLOW = 5,
  T_COLOR_PINK = 6,
  T_COLOR_GREEN = 7,
  T_COLOR_VIOLET = 8,
  T_COLOR_WHITE = 9,
  T_COLOR_BLACK = 10,
};

enum TankDirection {
  T_DIR_N = 0,    // North
  T_DIR_NE = 315, // Northeast
  T_DIR_E = 270,  // East
  T_DIR_SE = 225, // Southeast
  T_DIR_S = 180,  // South
  T_DIR_SW = 135, // Southwest
  T_DIR_W = 90,   // West
  T_DIR_NW = 45   // Northwest
};

enum TankMovement {
  T_MOVEMENT_NORMAL = 0,
  T_MOVEMENT_STATIONARY = 1,
  T_MOVEMENT_SLOW = 2,
  T_MOVEMENT_FAST = 3
};

enum TankFireRateCooldown {
  // Player has no cooldown, self controlled
  T_COOLDOWN_CONTROLLED = 0,
  // Computer fire rate cooldowns
  T_COOLDOWN_SLOW = 1,
  T_COOLDOWN_FAST = 2
};

enum TankBehavior {
  // Player behavior is controlled by them
  T_BEHAVIOR_CONTROLLED = 0,
  // Computer behavior
  T_BEHAVIOR_PASSIVE = 1,
  T_BEHAVIOR_DEFENSIVE = 2,
  T_BEHAVIOR_INCAUTIOUS = 3,
  T_BEHAVIOR_OFFENSIVE = 4,
  T_BEHAVIOR_ACTIVE = 5,
  T_BEHAVIOR_DYNAMIC = 6,
};

//---------------------------------------------------------------------------------
//
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

class Stage; // Avoids circular dependencies
class Tank {
public:
  // Tank Component Sprites
  Sprite *body = new Sprite();
  Sprite *turret = new Sprite();

  // Tank Attributes
  TankDirection direction; // Start facing north
  TankMovement movement;   // How fast the tank moves
  TankColor color;         // Color of the tank - Default to player
  TankFireRateCooldown fire_rate_cooldown; // Default to player
  TankBehavior behavior;                   // Default to player

  // Sprite Attributes
  int body_rotation_speed = 5;
  int height = TANK_SIZE; // Visual height of the tank in px within the Tile
  int width = TANK_SIZE;  // Visual height of the tank in px within the Tile

  // Bullet related attributes
  BulletSpeed bullet_speed;                  // Speed of the bullets
  static const int MAX_POSSIBLE_BULLETS = 5; // Max any one tank can have
  Bullet *bullets[MAX_POSSIBLE_BULLETS];     // Hold the bullet sprites
  int max_bullets = MAX_POSSIBLE_BULLETS;    // Default to player
  int max_bullet_ricochets;
  int active_bullets = 0;

  /**
   * Struct constructor
   */
  Tank(int x, int y, TankColor color);

  /**
   * Struct deconstructor
   */
  ~Tank();

  /**
   * @brief Sets the position of one of the axes for the tank
   * @param axis The axis to set (x or y)
   * @param value The value to set the axis to
   */
  void setPosition(char axis, int value);

  /**
   * @brief Sets the position of both axes for the tank
   * @param x The x-coordinate
   * @param y The y-coordinate
   */
  void setPosition(int x, int y);

  /**
   * @brief Gets the position of one of the axes for the tank
   * @param axis The axis to get (x or y)
   */
  int getPosition(char axis);

  /**
   * @brief Gets the position of both of the axes for the tank
   */
  Position &getPosition();

  /**
   * @brief Sets the offset of both of the axes for the tank
   */
  void setOffset(int x, int y);

  /**
   * @brief Uses LERP to smoothly rotate the tank body->in between
   *        directions
   */
  void interpolateBodyRotation();

  /**
   * Moves the tank in the specified direction.
   */
  void move(TankDirection direction, Stage *stage);

  /**
   * Rotates the tank's turret to the touch position
   */
  void rotateTurret(touchPosition &touch);

  /**
   * @brief Updates the OAM for both the tank body->and tank turret
   */
  void updateOAM();

  /**
   * @brief Creates the bullet sprites and stores them in the bullets arrays
   */
  void createBullets();

  /**
   * @brief If bullets are available, fires them in the direction pointed
   */
  void fire();

  /**
   * @brief Updates the positions for any in-flight bullets
   */
  void updateBulletPositions();
};

//---------------------------------------------------------------------------------
//
// FUNCTION DECLARATIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Allocates and initializes sprite graphics for the tank body->
 * @param tank The tank object.
 * @param gfx The graphics data.
 */
void initTankBodyGfx(Tank *tank, u8 *gfx);

/**
 * @brief Allocates and initializes sprite graphics for the tank turret->
 * @param tank The tank object.
 * @param gfx The graphics data.
 */
void initTankTurretGfx(Tank *tank, u8 *gfx);

#endif // TANK_H