#include "BitmapSprite.h"
#include "math.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------------------------------------

BitmapSprite::BitmapSprite() : rotation_angle(0), visible(true) {
  // Initialize image arrays
  orig_img_pixels = (ImageArray *)new int[MAX_SIZE * MAX_SIZE];
  rot_img_pixels = (ImageArray *)new int[MAX_SIZE * MAX_SIZE];
}

BitmapSprite::~BitmapSprite() {
  // Clean up memory - use delete[] since we allocated arrays
  delete[] (int *)orig_img_pixels;
  delete[] (int *)rot_img_pixels;
}

//-------------------------------------------------------------------------------
// Setters
//-------------------------------------------------------------------------------

void BitmapSprite::setPosition(Position new_pos) { pos = new_pos; }
void BitmapSprite::setRotation(float angle) { rotation_angle = angle; }
void BitmapSprite::setVisible(bool isVisible) { visible = isVisible; }
void BitmapSprite::setImgPixels(int (&imgPixels)[MAX_SIZE][MAX_SIZE]) {
  for (int i = 0; i < MAX_SIZE; i++) {
    for (int j = 0; j < MAX_SIZE; j++) {
      (*orig_img_pixels)[i][j] = imgPixels[i][j];
      (*rot_img_pixels)[i][j] = imgPixels[i][j];
    }
  }
}
void BitmapSprite::setColors(ColorsArray *newColors, int numColors) {
  num_colors = numColors;
  colors = newColors;
}

//-------------------------------------------------------------------------------
// Render Functions
//-------------------------------------------------------------------------------

void BitmapSprite::rotateImage() {
  if (curr_angle == rotation_angle) return;

  for (int y = 0; y < MAX_SIZE; y++) {
    for (int x = 0; x < MAX_SIZE; x++) {
      (*rot_img_pixels)[y][x] = (*orig_img_pixels)[y][x];
    }
  }

  // Calculate rotation
  const float radians = rotation_angle * M_PI / 180.0f;
  const float cos_angle = cos(radians);
  const float sin_angle = sin(radians);
  const int center = MAX_SIZE / 2;
  const int inner_size = 6;
  const int offset = 2; // Gap size from edges

  // Rotate the inner 6x6 square
  for (int y = offset; y < offset + inner_size; y++) {
    for (int x = offset; x < offset + inner_size; x++) {
      // Convert to relative coordinates from center
      float rel_x = x - center;
      float rel_y = y - center;

      // Rotate point
      float rot_x = rel_x * cos_angle + rel_y * sin_angle;
      float rot_y = -rel_x * sin_angle + rel_y * cos_angle;

      // Convert back to array coordinates
      int new_x = round(rot_x + center);
      int new_y = round(rot_y + center);

      // Only copy if within bounds
      if (new_x >= 0 && new_x < MAX_SIZE && new_y >= 0 && new_y < MAX_SIZE) {
        (*rot_img_pixels)[new_y][new_x] = (*orig_img_pixels)[y][x];
      }
    }
  }

  curr_angle = rotation_angle;
}

void BitmapSprite::draw() {
  if (!visible) return;

  // Rotate the image if necessary
  rotateImage();
  // Draw the final image on screen
  for (int y = 0; y < MAX_SIZE; y++) {
    for (int x = 0; x < MAX_SIZE; x++) {
      // Ignore blank spaces
      if ((*rot_img_pixels)[y][x] < 0) continue;
      // Place the pixel in the corresponding spot
      glPutPixel(pos.x + x, pos.y + y, (*colors)[(*rot_img_pixels)[y][x]]);
    }
  }
}