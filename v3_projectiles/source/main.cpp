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
  videoSetMode(MODE_5_3D);
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
 * @param cursor the player's cursor sprite
 * @param stage the stage to update the sprites of
 */
void updateSprites(Cursor *cursor, Stage *stage) {
  // Update the cursor first and foremost
  cursor->updateOAM();
  // Update all the tank sprite positions
  for (int i = 0; i < stage->num_tanks; i++) {
    stage->tanks[i]->updateOAM();
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

  // Initialize the first stage
  Stage *stage = new Stage(1);
  stage->initBackground();

  // Create a player cursor
  Cursor *cursor = new Cursor();

  while (pmMainLoop()) {
    handleDirectionInput(stage->tanks[0], stage);
    handleTouchInput(stage->tanks[0], cursor);

    updateSprites(cursor, stage);

    Stage::frame_counter++;

    glFlush(0); // Make sure frame has finished rendering
    swiWaitForVBlank();
    oamUpdate(&oamMain);
  }

  return 0;
}
