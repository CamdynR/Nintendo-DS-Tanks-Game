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

#include <stdio.h>

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

int count = 0;
void Stage::checkForBulletCollision() {
  for (int i = 0; i < num_tanks; i++) {
    for (int j = 0; j < tanks[i]->max_bullets; j++) {
      if (!tanks[i]->bullets[j]->in_flight) continue;
      Bullet *bullet1 = tanks[i]->bullets[j];
      
      // Check against all other bullets
      for (int k = 0; k < num_tanks; k++) {
        for (int l = 0; l < tanks[k]->max_bullets; l++) {
          if (!tanks[k]->bullets[l]->in_flight) continue;
          if (bullet1 == tanks[k]->bullets[l]) continue; // Skip self
          
          Bullet *bullet2 = tanks[k]->bullets[l];
          
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
        if (tanks[i] == tanks[k] && bullet1->num_ricochets == 0) continue; // Skip self
        if (!tanks[k]->alive) continue; // Skip destroyed tanks

        // Check for AABB collision
        Position tankPos = tanks[k]->getOffsetPosition();
        bool collision = 
          bullet1->pos.x < tankPos.x + tanks[k]->width &&
          bullet1->pos.x + bullet1->width > tankPos.x &&
          bullet1->pos.y < tankPos.y + tanks[k]->height &&
          bullet1->pos.y + bullet1->height > tankPos.y;

        if (collision) {
          bullet1->explode();
          tanks[k]->explode();
        }
      }
    }
  }
}