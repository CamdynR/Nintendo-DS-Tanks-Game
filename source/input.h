#ifndef INPUT_H
#define INPUT_H

#include "Cursor.h"
#include "Position.h"
#include "Stage.h"
#include "Tank.h"

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