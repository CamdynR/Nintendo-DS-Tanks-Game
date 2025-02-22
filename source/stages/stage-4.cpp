/*---------------------------------------------------------------------------------

stage-4.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "stage-4.h"
#include <vector>

//---------------------------------------------------------------------------------
//
// FUNCTIONS
//
//---------------------------------------------------------------------------------

std::vector<Tank *> *CREATE_STAGE_4_TANKS(Stage *stage) {
  std::vector<Tank *> *tanks = new std::vector<Tank *>();

  // Create the Player Tank
  tanks->push_back(new Tank(stage, STAGE_4_CELL_SIZE * 2, STAGE_4_CELL_SIZE * 9.5, T_COLOR_BLUE, T_DIR_N));

  // Create the Enemy Tanks
  tanks->push_back(new Tank(stage, STAGE_4_CELL_SIZE * 8, STAGE_4_CELL_SIZE, T_COLOR_BROWN, T_DIR_S));

  tanks->push_back(new Tank(stage, STAGE_4_CELL_SIZE * 13.5, STAGE_4_CELL_SIZE, T_COLOR_ASH, T_DIR_S));

  // tanks->push_back(new Tank(stage, STAGE_4_CELL_SIZE * 8, STAGE_4_CELL_SIZE * 5, T_COLOR_ASH, T_DIR_E));

  // tanks->push_back(new Tank(stage, STAGE_4_CELL_SIZE * 13.5, STAGE_4_CELL_SIZE * 5, T_COLOR_BROWN, T_DIR_W));

  return tanks;
}