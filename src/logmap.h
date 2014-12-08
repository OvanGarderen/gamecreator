#pragma once
#include <glib.h>

#define LOGMAP_W_MIN 16.0
#define LOGMAP_H_MIN 16.0

#define MAP_NW 0
#define MAP_NE 1
#define MAP_SE 2
#define MAP_SW 3

#include "shared.h"

object(Logmap);
object(Child);

struct Child {
  void * value;
  Rect area;
};

struct Logmap {
  float w, h;

  Logmap * submaps[4];
  GList * children;
};

Logmap * Logmap_new(float w, float h);
Logmap * Logmap_split(Logmap * lm);
Logmap * Logmap_add(Logmap * lm, void * obj, Rect rect);
GList  * Logmap_get(Logmap * lm, Rect rect);
void Logmap_draw(Logmap * lm, ALLEGRO_COLOR c, float thickness, float ofx, float ofy);

Logmap * Logmap_remove(Logmap * lm, void * obj, Rect at);
Logmap * Logmap_move(Logmap * lm, void * obj, Rect oldrect, Rect newrect);
