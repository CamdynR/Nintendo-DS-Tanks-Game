/*---------------------------------------------------------------------------------

        My First Demo :-)

---------------------------------------------------------------------------------*/
#include "nds/arm9/video.h"
#include <gl2d.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h> // For rand() and srand()

#define FRAMES_PER_ANIMATION 3

// some useful defines

#define HALF_WIDTH (SCREEN_WIDTH / 2)
#define HALF_HEIGHT (SCREEN_HEIGHT / 2)

// Data Types
struct Pos {
  int x;
  int y;
  int dir;
};

// Declarations
const int CELL_SIZE = 16;
void drawFrame(Pos &userPos, Pos &enemyPos, int keys, touchPosition &touchXY,
               int &frame);

void drawStage(int stageNum);

bool checkCollision(Pos &userPos);

//---------------------------------------------------------------------------------
int main(void) {
  srand(TIMER0_DATA);
  //---------------------------------------------------------------------------------
  // Set it to my favorite mode
  videoSetMode(MODE_5_3D);

  oamInit(&oamMain, SpriteMapping_1D_128, false);

  consoleDemoInit();

  // Initialize GL in 3d mode
  glScreen2D();

  iprintf("\x1b[1;1HPrimitives Test");

  // User input
  touchPosition touchXY; // touch screen
  int keys;              // keys pressed

  Pos userPos, enemyPos;                       // Entity positions on screen
  userPos.x = CELL_SIZE;                       // User x-coord
  userPos.y = CELL_SIZE * 5.5;                 // User y-coord
  enemyPos.x = SCREEN_WIDTH - (CELL_SIZE * 2); // Enemy x-coord
  enemyPos.y = CELL_SIZE * 5.5;                // Enemy y-coord

  int frame = 0;

  while (pmMainLoop()) {
    frame++;

    scanKeys();          // Look for what keys are pressed
    keys = keysHeld();   // Grab those key values
    touchRead(&touchXY); // Grab touch screen input

    if (frame > 60 && frame % 30 == 0) {
      enemyPos.dir = rand() % 9;
    }

    drawFrame(userPos, enemyPos, keys, touchXY,
              frame); // Render frame on screen

    glFlush(1);         // Make sure frame has finished rendering
    swiWaitForVBlank(); // Wait for the next frame upt

    iprintf("\x1b[2;1HFrame: %i", frame);
  }

  return 0;
}

// Render a box on screen
void drawFrame(Pos &userPos, Pos &enemyPos, int keys, touchPosition &touchXY,
               int &frame) {
  const int moveVal = 1;
  Pos newPosX = userPos; // Position if only moving in X direction
  Pos newPosY = userPos; // Position if only moving in Y direction

  // Try moving horizontally first
  if (keys & KEY_RIGHT)
    newPosX.x += moveVal;
  if (keys & KEY_LEFT)
    newPosX.x -= moveVal;
  if (!checkCollision(newPosX)) {
    userPos.x = newPosX.x;
  }

  // Then try moving vertically
  if (keys & KEY_DOWN)
    newPosY.y += moveVal;
  if (keys & KEY_UP)
    newPosY.y -= moveVal;
  if (!checkCollision(newPosY)) {
    userPos.y = newPosY.y;
  }

  Pos newEnemyPosY = enemyPos;
  Pos newEnemyPosX = enemyPos;

  if (enemyPos.dir == 1 || enemyPos.dir == 2 || enemyPos.dir == 8)
    newEnemyPosY.y += moveVal;
  if (enemyPos.dir == 4 || enemyPos.dir == 5 || enemyPos.dir == 6)
    newEnemyPosY.y -= moveVal;
  if (!checkCollision(newEnemyPosY))
    enemyPos.y = newEnemyPosY.y;

  if (enemyPos.dir == 2 || enemyPos.dir == 3 || enemyPos.dir == 4)
    newEnemyPosX.x += moveVal;
  if (enemyPos.dir == 6 || enemyPos.dir == 7 || enemyPos.dir == 8)
    newEnemyPosX.x -= moveVal;
  if (!checkCollision(newEnemyPosX))
    enemyPos.x = newEnemyPosX.x;

  // set up GL2D for 2d mode
  glBegin2D();

  // Create a black background
  glClearColor(0, 0, 0, 0); // Black background
  glClearPolyID(63);        // Ensure objects render properly
  glClearDepth(0x7FFF);

  // Draw the stage
  drawStage(1);

  // Create a green box on screen for the user
  glBoxFilled(userPos.x, userPos.y, userPos.x + (CELL_SIZE - 1),
              userPos.y + (CELL_SIZE - 1), RGB15(0, 31, 0));

  // Create a red box for the enemy
  glBoxFilled(enemyPos.x, enemyPos.y, enemyPos.x + (CELL_SIZE - 1),
              enemyPos.y + (CELL_SIZE - 1), RGB15(31, 0, 0));

  if (touchXY.rawx != 0 && touchXY.rawy != 0) {
    iprintf("\x1b[3;1HTouch Pos: %04X, %04X", touchXY.px, touchXY.py);

    if (keys & KEY_L) {
      // Connect the user to the cursor
      glLine(userPos.x + (CELL_SIZE * 0.5), userPos.y + (CELL_SIZE * 0.5),
             touchXY.px, touchXY.py, RGB15(31, 0, 31));

      bool left = touchXY.px >= enemyPos.x;
      bool right = touchXY.px < enemyPos.x + CELL_SIZE;
      bool top = touchXY.py >= enemyPos.y;
      bool bottom = touchXY.py < enemyPos.y + CELL_SIZE;
      if (left && right && top && bottom) {
        userPos.x = CELL_SIZE;                       // User x-coord
        userPos.y = CELL_SIZE * 5.5;                 // User y-coord
        enemyPos.x = SCREEN_WIDTH - (CELL_SIZE * 2); // Enemy x-coord
        enemyPos.y = CELL_SIZE * 5.5;                // Enemy y-coord
        frame = 0;
      }
    }

    // Show the user's touch on screen
    // Horizontal Line
    glLine(touchXY.px - 2, touchXY.py, touchXY.px + 2, touchXY.py,
           RGB15(16, 25, 28));
    // Vertical Line
    glLine(touchXY.px, touchXY.py - 2, touchXY.px, touchXY.py + 2,
           RGB15(16, 25, 28));
  }

  // Wrap up GL2D rendering
  glEnd2D();
}

// Draw the stage on screen
void drawStage(int stageNum) {
  int stageColor = RGB15(31, 31, 31);

  if (stageNum == 1) {
    // Upper Wall
    glBoxFilled(0, 0, SCREEN_WIDTH, CELL_SIZE - 1, stageColor);
    // Lower Wall
    glBoxFilled(0, SCREEN_HEIGHT - CELL_SIZE, SCREEN_WIDTH, SCREEN_HEIGHT,
                stageColor);

    // Barrier 1
    glBoxFilled(CELL_SIZE * 4, CELL_SIZE * 3, CELL_SIZE * 5 - 1,
                CELL_SIZE * 5 - 1, stageColor);
    // Barrier 2
    glBoxFilled(CELL_SIZE * 4, CELL_SIZE * 7, CELL_SIZE * 5 - 1,
                CELL_SIZE * 9 - 1, stageColor);
    // Barrier 3
    glBoxFilled(CELL_SIZE * 8, CELL_SIZE * 3, CELL_SIZE * 9 - 1,
                CELL_SIZE * 9 - 1, stageColor);
  } else if (stageNum == 2) {
    for (int i = 0; i < SCREEN_HEIGHT / CELL_SIZE; i++) {
      glBoxFilled(0, CELL_SIZE * i, (CELL_SIZE - 1), CELL_SIZE * (i + 1) - 1,
                  RGB15(i * 2, i * 2, i * 2));
    }
  }
}

bool checkCollision(Pos &userPos) {
  // Ensure the entire player stays within the screen boundaries
  if (userPos.x < 0 || userPos.x + CELL_SIZE > SCREEN_WIDTH || userPos.y < 0 ||
      userPos.y + CELL_SIZE > SCREEN_HEIGHT) {
    return true;
  }

  // Ensure the player respects the walls on the top and bottom
  if (userPos.y < CELL_SIZE ||
      userPos.y + CELL_SIZE > SCREEN_HEIGHT - CELL_SIZE) {
    return true;
  }

  // Define wall and barrier positions
  if ((userPos.x < CELL_SIZE * 5 && userPos.x + CELL_SIZE > CELL_SIZE * 4 &&
       userPos.y < CELL_SIZE * 5 && userPos.y + CELL_SIZE > CELL_SIZE * 3) ||
      (userPos.x < CELL_SIZE * 5 && userPos.x + CELL_SIZE > CELL_SIZE * 4 &&
       userPos.y < CELL_SIZE * 9 && userPos.y + CELL_SIZE > CELL_SIZE * 7) ||
      (userPos.x < CELL_SIZE * 9 && userPos.x + CELL_SIZE > CELL_SIZE * 8 &&
       userPos.y < CELL_SIZE * 9 && userPos.y + CELL_SIZE > CELL_SIZE * 3)) {
    return true;
  }

  return false;
}