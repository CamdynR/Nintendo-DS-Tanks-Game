/*---------------------------------------------------------------------------------

v3 of my Tank Game
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Bullet.h"
#include "Cursor.h"
#include "Stage.h"
#include "Tank.h"
#include "input.h"
#include "nds/arm9/video.h"
#include "sprite-sheet.h"
#include <gl2d.h>
#include <nds.h>
#include <unistd.h>

//---------------------------------------------------------------------------------
//
// INITIALIZATION FUNCTIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Initializes the sprite palette.
 */
void initSprites() {
  vramSetBankB(VRAM_B_MAIN_SPRITE);
  oamInit(&oamMain, SpriteMapping_1D_32, false);
  dmaCopy(sprite_sheetPal, SPRITE_PALETTE, sprite_sheetPalLen);
}

/**
 * @brief Initializes the graphics system for 2D bitmap drawing
 */
void initGL2D() {
  // Enable gl2d
  glScreen2D();
  // Prevent GL2D from drawing a black frame
  glClearColor(31, 31, 31, 0);
  glClearPolyID(63);
  // Set up texture parameters
  glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE);
}

/**
 * @brief Initializes the graphics system for 2D sprites.
 */
void initGraphics() {
  videoSetMode(MODE_0_3D);

  // Set up background for tread marks
  REG_BG0CNT = BG_PRIORITY(3);  // Lower priority (appears behind)

  initSprites();
  initGL2D();
}

//---------------------------------------------------------------------------------
//
// ANIMATION FUNCTIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Updates sprite attributes for all tanks.
 * @param stage the stage to update the sprites of
 * @param cursor the player's cursor sprite
 */
void updateSprites(Stage *stage, Cursor *cursor) {
  // Update the cursor first and foremost
  if (!stage->tanks->at(0)->alive) cursor->hideSprites();
  if (!cursor->hide) cursor->connectToTank(stage->tanks->at(0));
  cursor->updateOAM();

  // Update all the tank sprite positions
  for (int i = 0; i < stage->num_tanks; i++) {
    stage->tanks->at(i)->updateOAM();

    // Update positions of any active bullet sprites for each tank
    for (int j = 0; j < stage->tanks->at(i)->max_bullets; j++) {
      stage->tanks->at(i)->bullets[j]->updatePosition();
      stage->tanks->at(i)->bullets[j]->updateOAM();
    }
  }

  // Checks to see if any bullets have collided
  stage->checkForBulletCollision();
}

/**
 * @brief Renders all bitmap drawings for the treadmarks in OpenGL
 * @param stage the stage to update the drawings of
 */
void updateTreadBitmapGfx(Stage *stage) {
  // Update all the tank sprite positions
  glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_ID(2));
  for (int i = 0; i < stage->num_tanks; i++) {
    stage->tanks->at(i)->drawTreadmarks();
  }
}

/**
 * @brief Renders all bitmap drawings in OpenGL
 * @param stage the stage to update the drawings of
 * @param cursor the player's cursor sprite
 */
void updateGl2dGfx(Stage *stage, Cursor *cursor) {
  // Begin 2D drawing
  glBegin2D();
  // Draw treads FIRST
  updateTreadBitmapGfx(stage);
  // End 2D drawing
  glEnd2D();
}

//---------------------------------------------------------------------------------
//
// MAIN
//
//---------------------------------------------------------------------------------

int main(void) {
  consoleDemoInit();

  // Initialize the graphics (set video mode, set VRAM banks, etc)
  initGraphics();

  // Create a player cursor
  Cursor *cursor = new Cursor();

  // Initialize the first stage
  Stage *stage = new Stage(4);
  stage->initBackground();

  while (pmMainLoop()) {
    // Handle all inputs
    handleButtonInput(stage);
    handleTouchInput(stage, cursor);
    // Update sprites in the Object Attribute Model
    updateSprites(stage, cursor);
    // Update the OpenGL 2D graphics
    updateGl2dGfx(stage, cursor);

    // Increment the frame counter
    Stage::frame_counter++;

    glFlush(0); // Make sure frame has finished rendering
    swiWaitForVBlank();
    oamUpdate(&oamMain);
  }

  return 0;
}
