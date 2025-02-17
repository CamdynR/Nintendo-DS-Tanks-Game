#ifndef BITMAP_SPRITE_H
#define BITMAP_SPRITE_H

#include "Position.h"
#include <gl2d.h>
#include <nds.h>

class BitmapSprite {
protected:
  static const int MAX_COLORS = 4;
  static const int MAX_SIZE = 10;

  Position pos;
  float curr_angle = 0;
  float rotation_angle;
  bool visible;
  int num_colors;

  typedef int ColorsArray[MAX_COLORS];
  ColorsArray *colors;

  typedef int ImageArray[MAX_SIZE][MAX_SIZE];
  ImageArray *orig_img_pixels;
  ImageArray *rot_img_pixels;

public:
  BitmapSprite();
  virtual ~BitmapSprite();

  void setPosition(Position new_pos);
  void setRotation(float angle);
  void setVisible(bool isVisible);
  void setColors(ColorsArray *newColors, int numColors);
  void setImgPixels(int (&imagePixels)[MAX_SIZE][MAX_SIZE]);

  // Virtual methods that can be overridden
  void rotateImage();
  virtual void draw();
};

#endif // BITMAP_SPRITE_H