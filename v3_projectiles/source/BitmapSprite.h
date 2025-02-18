#ifndef BITMAP_SPRITE_H
#define BITMAP_SPRITE_H

#include "Position.h"
#include <gl2d.h>
#include <nds.h>

class BitmapSprite {
protected:
  glImage sprite_image[1]; // Array of 1 since we're using single sprites
  int texture_id;
  const u16 *palette_data = nullptr;

public:
  Position pos = {0, 0};
  Position sprite_offset = {0, 0};
  float rotation_angle = 0;
  bool visible = false;
  int height;
  int width;

  BitmapSprite();
  virtual ~BitmapSprite();

  void setBitmapData(const unsigned int *bitmapData, int bitDepth, u32 height,
                     u32 width);
  void setPaletteData(const u16 *palette);

  virtual void draw();
};

#endif // BITMAP_SPRITE_H