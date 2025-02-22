#ifndef TANK_H
#define TANK_H

#include "Bullet.h"
#include "Sprite.h"
#include "calico/types.h"
#include "sprite-sheet.h"
#include <nds.h>
#include <vector>

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
private:
  struct PositionHistory {
    Position pos;
    TankDirection dir;
  };
  std::vector<PositionHistory> position_history;
  int position_history_counter = 0;

  Stage *stage;

  /**
   * @brief Checks if the given position is within the screen bounds.
   * @param pos The position to check.
   */
  bool isWithinBounds(Position &pos);

  /**
   * @brief Checks if a tank at the given position would collide with any other
   * tank.
   * @param pos The position to check.
   */
  bool noTanksCollided(Position &pos);

  /**
   * @brief Checks if the tank's position collides with any barriers in the
   * stage.
   * @param pos The position to check.
   */
  bool noBarrierCollisions(Position &pos);

  /**
   * @brief Validates if a move is allowed based on screen bounds and
   *        collisions.
   */
  bool validateMove(Position &pos);

  /**
   * @brief Appends current position and direction to history vector
   */
  void addPositionHistory();

  /**
   * @brief Rotates the tank's body to face a certain direction. Used for setting
   *        the initial tank's direction, not for rotating the tank mid match.
   * @param direction The direction to face.
   */
  void faceDirection(TankDirection direction);

public:
  // Tank Component Sprites
  Sprite *body;
  Sprite *turret;
  Sprite *explosion;

  // Tank Attributes
  bool alive = true;       // Is the tank alive
  TankDirection direction = T_DIR_N; // Start facing north
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
  std::vector<Bullet*> bullets;     // Hold the bullet sprites
  int max_bullets = 5;    // Default to player
  int max_bullet_ricochets;

  /**
   * Struct constructor
   * @param stage The stage the tank is on.
    * @param x The x-coordinate of the tank to start at.
    * @param y The y-coordinate of the tank to start at.
    * @param color The color of the tank.
    * @param direction The direction the tank is facing initially
   */
  Tank(Stage *stage, int x, int y, TankColor color, TankDirection direction);

  /**
   * Struct deconstructor
   */
  ~Tank();

  /**
   * @brief Sets the position of one of the axes for the tank.
   * @param axis The axis to set ('x' or 'y').
   * @param value The value to set the axis to.
   */
  void setPosition(char axis, int value);

  /**
   * @brief Sets the position of both axes for the tank.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   */
  void setPosition(int x, int y);

  /**
   * @brief Gets the position of one of the axes for the tank.
   * @param axis The axis to get ('x' or 'y').
   * @return The position value of the specified axis.
   */
  int getPosition(char axis);

  /**
   * @brief Gets the position of both of the axes for the tank.
   * @return The position of the tank.
   */
  Position &getPosition();

  /**
   * @brief Gets the position of both of the axes for the tank offset by the tile offset.
   * @return The offset position of the tank.
   */
  Position getOffsetPosition();

  /**
   * @brief Sets the offset of both of the axes for the tank.
   * @param x The x-coordinate offset.
   * @param y The y-coordinate offset.
   */
  void setOffset(int x, int y);

  /**
   * @brief Uses LERP to smoothly rotate the tank body in between directions.
   */
  void interpolateBodyRotation();

  /**
   * @brief Moves the tank in the specified direction.
   * @param direction The direction to move the tank.
   */
  void move(TankDirection direction);

  /**
   * @brief Rotates the tank's turret towards a point on the screen.
   * @param pos The position to rotate the turret towards.
   */
  void rotateTurret(Position pos);

  /**
   * @brief Rotates the tank's turret to the touch position.
   * @param touch The touch position to rotate the turret towards.
   */
  void rotateTurret(touchPosition &touch);

  /**
   * @brief Rotates the tank's turret a certain number of degrees (starting from +y).
   * @param angle The angle in degrees to rotate the turret.
   */
  void rotateTurret(int angle);

  /**
   * @brief Creates the bullet sprites and stores them in the bullets array.
   */
  void createBullets();

  /**
   * @brief If bullets are available, fires them in the direction pointed.
   */
  void fire();

  /**
   * @brief Updates the positions for any in-flight bullets.
   */
  void updateBulletPositions();

  /**
   * @brief Draws the treadmarks for the tank on screen with gl2d.
   */
  void drawTreadmarks();

  /**
   * @brief Marks the tank for explosion so the necessary animations can be played.
   */
  void explode();

  /**
   * @brief For testing purposes, reset stage back to normal.
   */
  void reset();

  /**
   * @brief Updates the OAM for both the tank body and tank turret.
   */
  void updateOAM();
};

//---------------------------------------------------------------------------------
//
// HELPER FUNCTION DECLARATIONS
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
float calculateAngle(int x1, int y1, int x2, int y2);

#endif // TANK_H