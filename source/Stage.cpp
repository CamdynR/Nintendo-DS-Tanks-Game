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
#include "stages/stage-4.h"
#include "stages/stage-4_bg.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

int Stage::frame_counter = 0;

Stage::Stage(int stageNum) {
  stage_num = stageNum;

  if (stage_num == 1) {
    tanks = CREATE_STAGE_1_TANKS(this);
    barriers = STAGE_1_BARRIERS;
  } else if (stage_num == 4) {
    tanks = CREATE_STAGE_4_TANKS(this);
    barriers = STAGE_4_BARRIERS;
  }

  // Set tanks size
  if (tanks != nullptr) num_tanks = tanks->size();
  else num_tanks = 0;
}

void Stage::initBackground() {
  // Set VRAM bank A for the background
  vramSetBankA(VRAM_A_MAIN_BG);
  // Initialize the tile background to last layer
  int bg = bgInit(3, BgType_Text8bpp, BgSize_T_256x256, 31, 0);
  bgSetPriority(bg, 3);

  if (stage_num == 1) {
    // Copy stage 1 tiles to the background layer
    dmaCopy(stage_1_bgTiles, bgGetGfxPtr(bg), stage_1_bgTilesLen);
    dmaCopy(stage_1_bgMap, bgGetMapPtr(bg), stage_1_bgMapLen);
    dmaCopy(stage_1_bgPal, BG_PALETTE, stage_1_bgPalLen);
  } else if (stage_num == 4) {
    // Copy stage 4 tiles to the background layer
    dmaCopy(stage_4_bgTiles, bgGetGfxPtr(bg), stage_4_bgTilesLen);
    dmaCopy(stage_4_bgMap, bgGetMapPtr(bg), stage_4_bgMapLen);
    dmaCopy(stage_4_bgPal, BG_PALETTE, stage_4_bgPalLen);
  }
}

int count = 0;
void Stage::checkForBulletCollision() {
  for (int i = 0; i < num_tanks; i++) {
    for (int j = 0; j < tanks->at(i)->max_bullets; j++) {
      if (!tanks->at(i)->bullets[j]->in_flight) continue;
      Bullet *bullet1 = tanks->at(i)->bullets[j];
      
      // Check against all other bullets
      for (int k = 0; k < num_tanks; k++) {
        for (int l = 0; l < tanks->at(k)->max_bullets; l++) {
          if (!tanks->at(k)->bullets[l]->in_flight) continue;
          if (bullet1 == tanks->at(k)->bullets[l]) continue; // Skip self
          
          Bullet *bullet2 = tanks->at(k)->bullets[l];
          
          // Check for AABB collision
          bool collision = 
            bullet1->pos.x < bullet2->pos.x + bullet2->width &&
            bullet1->pos.x + bullet1->width > bullet2->pos.x &&
            bullet1->pos.y < bullet2->pos.y + bullet2->height &&
            bullet1->pos.y + bullet1->height > bullet2->pos.y;
            
          if (collision) {
            bullet1->explode();
            bullet2->explode();
          }
        }
      }

      // Check against all other tanks
      for (int k = 0; k < num_tanks; k++) {
        if (tanks->at(i) == tanks->at(k) && bullet1->num_ricochets == 0) continue; // Skip self
        if (!tanks->at(k)->alive) continue; // Skip destroyed tanks

        // Check for AABB collision
        Position tankPos = tanks->at(k)->getOffsetPosition();
        bool collision = 
          bullet1->pos.x < tankPos.x + tanks->at(k)->width &&
          bullet1->pos.x + bullet1->width > tankPos.x &&
          bullet1->pos.y < tankPos.y + tanks->at(k)->height &&
          bullet1->pos.y + bullet1->height > tankPos.y;

        if (collision) {
          bullet1->explode();
          tanks->at(k)->explode();
        }
      }
    }
  }
}