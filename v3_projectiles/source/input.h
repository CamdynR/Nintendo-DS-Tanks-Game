#ifndef INPUT_H
#define INPUT_H

#include "Cursor.h"
#include "Position.h"
#include "Stage.h"
#include "Tank.h"

/**
 * @brief Calculates the angle between two points.
 * @param x1 The x-coordinate of the first point.
 * @param y1 The y-coordinate of the first point.
 * @param x2 The x-coordinate of the second point.
 * @param y2 The y-coordinate of the second point.
 * @return The angle in degrees.
 */
float calculateAngle(int x1, int y1, int x2, int y2);

/**
 * @brief Checks if the given position is within the screen bounds.
 * @param pos The position to check.
 * @param tank The tank object, used to determine width and height.
 * @return true if the position is within bounds, false otherwise.
 */
bool isWithinBounds(Position &pos, Tank &tank);

/**
 * @brief Checks if a tank at the given position would collide with any other
 * tank.
 * @param pos The position to check.
 * @param tank The tank being moved (used to determine size).
 * @param stage The stage to check tank collisions on
 * @return true if no collision occurs, false if a collision is detected.
 */
bool noTanksCollided(Position &pos, Tank &tank, Stage *stage);

/**
 * @brief Checks if the tank's position collides with any barriers in the stage.
 * @param pos The position to check.
 * @param tank The tank being moved (used to determine size).
 * @param stage The stage to check barrier collisions on
 * @return true if no collisions with barriers occur, false if a collision is
 * detected.
 */
bool noBarrierCollisions(Position pos, Tank &tank, Stage *stage);

/**
 * @brief Validates if a given position is allowed based on screen bounds and
 * collisions.
 * @param pos The position to validate.
 * @param tank The tank being moved (used to determine size).
 * @param stage The stage to validate input on
 * @return true if the position is valid, false otherwise.
 */
bool validateInput(Position &pos, Tank *tank, Stage *stage);

/**
 * @brief Handles user input to update the tank's position.
 * @param stage The stage to handle direction input on
 */
void handleButtonInput(Stage *stage);

/**
 * @brief Handles touch input to update the tank's turret angle.
 * @param Stage The stage with the current tanks to update.
 * @param cursor The player's cursor sprite.
 */
void handleTouchInput(Stage *stage, Cursor *cursor);

#endif // INPUT_H