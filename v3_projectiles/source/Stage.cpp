/*---------------------------------------------------------------------------------

Stage.cpp
Camdyn Rasque

---------------------------------------------------------------------------------*/
//---------------------------------------------------------------------------------
//
// IMPORTS
//
//---------------------------------------------------------------------------------

#include "Stage.h"
#include "Tank.h"
#include "stages/stage-1.h"
#include "stages/stage-1_bg.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

int Stage::frame_counter = 0;

Stage::Stage(int stageNum) {
  stage_num = stageNum;

  if (stage_num == 1) {
    num_tanks = STAGE_1_NUM_TANKS;
    tanks = CREATE_STAGE_1_TANKS(this);
    barriers = STAGE_1_BARRIERS;
  }
}

void Stage::initBackground() {
  // Set VRAM bank A for the background
  vramSetBankA(VRAM_A_MAIN_BG);
  // Initialize the tile background to last layer
  int bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
  bgSetPriority(bg, 3);

  if (stage_num == 1) {
    // Copy stage 1 tiles to the background layer
    decompress(stage_1_bgBitmap, BG_GFX, LZ77Vram);
  }
}