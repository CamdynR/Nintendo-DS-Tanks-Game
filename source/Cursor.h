#ifndef CURSOR_H
#define CURSOR_H

#include "Sprite.h"
#include "Tank.h"
#include "calico/types.h"
#include "sprite-sheet.h"
// #include <gl2d.h>
#include <vector>
#include <nds.h>

//---------------------------------------------------------------------------------
//
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

class Cursor : public Sprite {
private:
  int height = 14;
  int width = 14;
  std::vector<Sprite*> tail;
  int numTailSprites = 7;

  /**
   * @brief: Creates the necessary tail sprites for the cursor
   */
   void createTail();

  /**
   * @brief: Set the position of where to be on screen
   */
   void setPosition(Position &pos);

  /**
   * @brief: Draws the dotted line between the cursor and the tank
   */
  void connectToTank(Tank *playerTank);

public:
  /**
   * @brief Initializes the cursor sprite.
   */
  Cursor();

  /**
   * @brief: Shows the cursor and tail sprites
   */
  void showSprites(Position cursorPos, Tank *playerTank);

  /**
   * @brief: Hides the cursor and tail sprites
   */
  void hideSprites();

  /**
   * @brief: Updates the cursor and tail sprites
   */
  void updateOAM();
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