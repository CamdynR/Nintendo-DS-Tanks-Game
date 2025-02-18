#include "BitmapSprite.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------------------------------------

BitmapSprite::BitmapSprite() {}

BitmapSprite::~BitmapSprite() {}

//-------------------------------------------------------------------------------
// Setters / Getters
//-------------------------------------------------------------------------------

void BitmapSprite::setBitmapData(const unsigned int *bitmapData, int bitDepth,
                                 u32 height, u32 width) {
  // Set the height and width of the sprite
  this->height = height;
  this->width = width;

  // Create texture coordinates for single 8x8 sprite
  static const unsigned int texcoords[] = {0, 0, width, height};

  // Make sure palette is set before loading sprite
  if (!palette_data) return;

  // Default to bit depth of 2
  GL_TEXTURE_TYPE_ENUM type = GL_RGB4;
  int paletteWidth = 4;
  // Bit depth of 4
  if (bitDepth == 4) {
    type = GL_RGB16;
    paletteWidth = 16;
  } else if (bitDepth == 8) {
    type = GL_RGB256;
    paletteWidth = 256;
  } else if (bitDepth != 2) {
    return; // Make sure bitDepth is valid
  }

  // Load the sprite using gl2d's sprite loading function
  texture_id = glLoadSpriteSet(sprite_image,   // Pointer to glImage array
                               1,              // Number of sprites
                               texcoords,      // Texture coordinates array
                               type,           // Texture type (2bpp = 4 colors)
                               TEXTURE_SIZE_8, // Texture width
                               TEXTURE_SIZE_8, // Texture height
                               GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T |
                                   TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT,
                               paletteWidth,    // Palette length (4 colors)
                               palette_data,    // Palette data
                               (u8 *)bitmapData // Bitmap data
  );
}

void BitmapSprite::setPaletteData(const u16 *palette) {
  palette_data = palette;
}

//-------------------------------------------------------------------------------
// Render Functions
//-------------------------------------------------------------------------------

void BitmapSprite::draw() {
  if (!visible) return;

  int BULLET_RENDER_PRIORITY = 0;
  int SCALE_FACTOR = 1;

  if (rotation_angle > 0) {
    // Enable alpha blending and anti-aliasing for smoother rotation
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | POLY_FORMAT_LIGHT0 |
              POLY_MODULATION | POLY_ID(BULLET_RENDER_PRIORITY));

    // Calculate center point for rotation
    int centerX = pos.x + (sprite_image[0].width >> 1);
    int centerY = pos.y + (sprite_image[0].height >> 1);

    // Use glSpriteRotateScale for better quality
    glSpriteRotateScale(centerX, centerY, degreesToAngle(360 - rotation_angle),
                        inttof32(SCALE_FACTOR), // Scale = 1.0
                        GL_FLIP_NONE, &sprite_image[0]);
  } else {
    // Set higher priority number to make bullets render underneath tanks
    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE | GL_TEXTURE_COLOR0_TRANSPARENT |
              POLY_ID(BULLET_RENDER_PRIORITY)); // Priority 3 = underneath tanks

    // Use gl2d's sprite drawing function
    glSprite(pos.x + sprite_offset.x, pos.y + sprite_offset.y, GL_FLIP_NONE,
             &sprite_image[0]);
  }
}