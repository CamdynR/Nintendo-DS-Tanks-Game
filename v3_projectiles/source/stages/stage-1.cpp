/*---------------------------------------------------------------------------------

stage-1.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "stage-1.h"

//---------------------------------------------------------------------------------
//
// FUNCTIONS
//
//---------------------------------------------------------------------------------

Tank **CREATE_STAGE_1_TANKS() {
  Tank **tanks = new Tank *[STAGE_1_NUM_TANKS];

  // Create the Player Tank
  tanks[0] = new Tank(STAGE_1_CELL_SIZE,
                      (SCREEN_HEIGHT / 2) - (STAGE_1_CELL_SIZE / 2), T_BLUE);

  // Create the Enemy Tank
  tanks[1] = new Tank(SCREEN_WIDTH - (STAGE_1_CELL_SIZE * 2),
                      (SCREEN_HEIGHT / 2) - (STAGE_1_CELL_SIZE / 2), T_RED);

  return tanks;
}