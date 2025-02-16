#ifndef CURSOR_H
#define CURSOR_H

#include "Sprite.h"
#include "Tank.h"
#include "calico/types.h"
#include "sprite-sheet.h"
#include <gl2d.h>
#include <nds.h>

//---------------------------------------------------------------------------------
//
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

struct Cursor : Sprite {
  int height = 14;
  int width = 14;

  /**
   * @brief Initializes the cursor sprite.
   */
  Cursor();

  /**
   * Set the position of where to be on screen
   */
  void setPosition(int x, int y);

  /**
   * Draws the dotted line between the cursor and the tank
   */
  void connectToTank(Tank *playerTank);
};

//---------------------------------------------------------------------------------
//
// FUNCTION DECLARATIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Draws a dotted line with exactly 8 small circles connecting two
 * points.
 * @param x1 The x-coordinate of the first point.
 * @param y1 The y-coordinate of the first point.
 * @param x2 The x-coordinate of the second point.
 * @param y2 The y-coordinate of the second point.
 */
void drawDottedLine(int x1, int y1, int x2, int y2);

/**
 * @brief Handles touch input to update the cursor position.
 * @param cursor The cursor object to update.
 * @param touch The touch position data.
 * @param keys The pressed keys bitmask.
 * @param playerTank The tank object to draw line to.
 */
void handleCursorInput(Cursor *cursor, touchPosition &touch, int keys,
                       Tank &playerTank);

/**
 * @brief Processes user input to update the cursor position.
 * @param cursor The cursor object to update.
 * @param playerTank The tank object to draw line to.
 */
void processCursorInput(Cursor *cursor, Tank *playerTank);

#endif // CURSOR_H