#pragma once

#include "shared.h"
#include "typedefs.h"
#include "collision.h"

struct Sprite{
  int type;
  char * name;
  Image * image;

  // dimensions
  int fw,fh;

  Rect bbox;

  // animation
  int maxframe, curframe;
  int animdir, animdelay, ticks;
};

void sprite_draw(Sprite *s, float x, float y);
void sprite_tick(Sprite *s);
Sprite * sprite_copy(Sprite * spr);
