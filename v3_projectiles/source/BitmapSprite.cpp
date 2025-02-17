#include "BitmapSprite.h"

//-------------------------------------------------------------------------------
//
// STRUCT FUNCTIONS
//
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Constructor / Destructor
//-------------------------------------------------------------------------------

BitmapSprite::BitmapSprite() : palette_data(nullptr) {}

BitmapSprite::~BitmapSprite() {}

//-------------------------------------------------------------------------------
// Setters / Getters
//-------------------------------------------------------------------------------

void BitmapSprite::setBitmapData(const unsigned int *bitmapData, u32 width,
                                 u32 height) {
  // Load the sprite using gl2d's built-in functions
  texture_id = glLoadTileSet(sprite_image,   // Pointer to glImage array
                             width,          // Sprite width
                             height,         // Sprite height
                             width,          // Bitmap width
                             height,         // Bitmap height
                             GL_RGB256,      // Texture type (2bpp = 16 colors)
                             TEXTURE_SIZE_8, // Size setting based on 8x8 sprite
                             TEXTURE_SIZE_8, // Size setting based on 8x8 sprite
                             GL_TEXTURE_WRAP_S | GL_TEXTURE_WRAP_T |
                                 TEXGEN_OFF | GL_TEXTURE_COLOR0_TRANSPARENT,
                             256,             // Palette length (16 colors)
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

  // Use gl2d's sprite drawing function
  glSprite(pos.x, pos.y, GL_FLIP_NONE, &sprite_image[0]);
}