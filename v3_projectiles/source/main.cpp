/*---------------------------------------------------------------------------------

v3 of my Tank Game
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

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
 * @brief Initializes the graphics system for 2D sprites.
 */
void initGraphics() {
  videoSetMode(MODE_0_3D);
  glScreen2D();
  initSprites();

  // Prevent GL2D from drawing a black frame
  glClearColor(31, 31, 31, 0);
  glClearPolyID(63);
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
  cursor->updateOAM();
  // Update all the tank sprite positions
  for (int i = 0; i < stage->num_tanks; i++) {
    stage->tanks[i]->updateOAM();
    // Update positions of any active bullet sprites for each tank
    for (int j = 0; j < stage->tanks[i]->active_bullets; j++) {
      stage->tanks[i]->bullets[j]->updatePosition();
      stage->tanks[i]->bullets[j]->updateOAM();
    }
  }
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
  Stage *stage = new Stage(1);
  stage->initBackground();

  while (pmMainLoop()) {
    // Handle all inputs
    handleButtonInput(stage);
    handleTouchInput(stage, cursor);
    // Update sprites in the Object Attribute Model
    updateSprites(stage, cursor);

    // All GL drawing happens here after sprites have been moved
    glBegin2D();
    // Draw dotted line to connect to tank
    if (!cursor->hide) {
      cursor->connectToTank(stage->tanks[0]); // Draw the dotted line
    }
    // Draw bullets using GL2D
    // for (int i = 0; i < stage->num_tanks; i++) {
    //   for (int j = 0; j < stage->tanks[i]->active_bullets; j++) {
    //     stage->tanks[i]->bullets[j]->draw();
    //   }
    // }
    glEnd2D();

    // Increment the frame counter
    Stage::frame_counter++;

    glFlush(0); // Make sure frame has finished rendering
    swiWaitForVBlank();
    oamUpdate(&oamMain);
  }

  return 0;
}
