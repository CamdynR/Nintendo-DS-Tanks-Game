#ifndef STAGE_H
#define STAGE_H

#include "nds/arm9/video.h"

struct Tank;
struct Stage {
  static int frame_counter; // Keep track of frames

  int stage_num; // The number stage to load
  int num_tanks; // The number of tanks in the stage

  const int (*barriers)[SCREEN_WIDTH];
  Tank **tanks; // Array of tank structs in the stage

  Stage(int stageNum); // Constructor

  void initBackground();
};

#endif // STAGE_H