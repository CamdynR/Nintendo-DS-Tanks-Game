#ifndef INPUT_H
#define INPUT_H

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
 * @return true if no collision occurs, false if a collision is detected.
 */
bool noTanksCollided(Position &pos, Tank &tank);

/**
 * @brief Checks if the tank's position collides with any barriers in the stage.
 * @param pos The position to check.
 * @param tank The tank being moved (used to determine size).
 * @return true if no collisions with barriers occur, false if a collision is
 * detected.
 */
bool noBarrierCollisions(Position pos, Tank &tank);

/**
 * @brief Validates if a given position is allowed based on screen bounds and
 * collisions.
 * @param pos The position to validate.
 * @param tank The tank being moved (used to determine size).
 * @return true if the position is valid, false otherwise.
 */
bool validateInput(Position &pos, Tank &tank);

#endif // INPUT_H