#ifndef BITMAP_SPRITE_H
#define BITMAP_SPRITE_H

#include "Position.h"
#include <gl2d.h>
#include <nds.h>

class BitmapSprite {
protected:
  glImage sprite_image[1]; // Array of 1 since we're using single sprites
  int texture_id;
  const u16 *palette_data;

public:
  Position pos = {0, 0};
  float curr_angle = 0;
  float rotation_angle = 0;
  bool visible = false;

  BitmapSprite();
  virtual ~BitmapSprite();

  void setBitmapData(const unsigned int *bitmapData, u32 width, u32 height);
  void setPaletteData(const u16 *palette);

  virtual void draw();
};

#endif // BITMAP_SPRITE_H