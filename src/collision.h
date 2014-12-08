#pragma once

#include <allegro5/allegro.h>

typedef struct Rect Rect;

struct Rect {
  double x, y; //left upper corner
  double w,h;
};

Rect rectnone;

char rect_isnone(Rect r);
Rect rect_4point(double xl, double yl, double xr, double yr);
Rect rect_move(Rect r, double dx, double dy);

Rect collide_rects(Rect r1, Rect r2);

void drawrect(Rect r, ALLEGRO_COLOR c, double thickness);
void drawrect_at(double x, double y, Rect r, ALLEGRO_COLOR c, double thickness);

