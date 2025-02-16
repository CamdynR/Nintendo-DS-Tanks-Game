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
    tanks = CREATE_STAGE_1_TANKS();
    barriers = STAGE_1_BARRIERS;
  }
}

void Stage::initBackground() {
  // Set VRAM bank A to LCD mode so we can write a bitmap
  // vramSetBankA(VRAM_A_MAIN_BG);
  // int bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
  // bgSetPriority(bg, 3);
  // if (stage_num == 1) {
  //   decompress(stage_1_bgBitmap, BG_GFX, LZ77Vram);
  // }
  vramSetBankA(VRAM_A_MAIN_BG);
  // Use BgType_Text8bpp instead of BgType_Bmp16
  int bg = bgInit(3, BgType_Text8bpp, BgSize_T_256x256, 0, 0);
  bgSetPriority(bg, 3);

  if (stage_num == 1) {
    // Clear any existing data first
    dmaFillWords(0, BG_TILE_RAM(1), 32 * 1024);
    dmaFillWords(0, BG_MAP_RAM(4), 2 * 1024);

    // You'll need to convert your background to tiles
    // and use dmaCopy instead of decompress
    dmaCopy(stage_1_bgTiles, BG_TILE_RAM(0), stage_1_bgTilesLen);
    dmaCopy(stage_1_bgMap, BG_MAP_RAM(0), stage_1_bgMapLen);
    dmaCopy(stage_1_bgPal, BG_PALETTE, stage_1_bgPalLen);
  }
}