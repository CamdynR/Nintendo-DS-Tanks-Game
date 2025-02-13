#ifndef CURSOR_H
#define CURSOR_H

#include <nds.h>
#include "calico/types.h"
#include "sprite-sheet.h"
#include "Sprite.h"

//---------------------------------------------------------------------------------
//
// TYPE DEFINITIONS
//
//---------------------------------------------------------------------------------

struct Cursor: Sprite {
  u16 *sprite_gfx_mem;
  u8 *sprite_frame_gfx;

  int height = 14;
  int width = 14;
};

//---------------------------------------------------------------------------------
//
// FUNCTION DECLARATIONS
//
//---------------------------------------------------------------------------------

/**
 * @brief Initializes the cursor sprite.
 */
void initCursor(Cursor &cursor);

#endif // CURSOR_H