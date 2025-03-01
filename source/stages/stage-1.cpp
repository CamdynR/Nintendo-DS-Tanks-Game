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
#include <vector>

//---------------------------------------------------------------------------------
//
// FUNCTIONS
//
//---------------------------------------------------------------------------------

std::vector<Tank *> *CREATE_STAGE_1_TANKS(Stage *stage) {
  std::vector<Tank *> *tanks = new std::vector<Tank *>();

  // Create the Player Tank
  tanks->push_back(new Tank(stage, STAGE_1_CELL_SIZE,
               (SCREEN_HEIGHT / 2) - (STAGE_1_CELL_SIZE / 2), T_COLOR_BLUE, T_DIR_E));

  // Create the Enemy Tank
  tanks->push_back(new Tank(stage, SCREEN_WIDTH - (STAGE_1_CELL_SIZE * 2),
               (SCREEN_HEIGHT / 2) - (STAGE_1_CELL_SIZE / 2), T_COLOR_BROWN, T_DIR_W));

  return tanks;
}