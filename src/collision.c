#include "shared.h"
#include "collision.h"
#include "drawtools.h"

Rect rectnone = {0.0,0.0,0.0,0.0};

char rect_isnone(Rect r){
  if(r.w == rectnone.w && r.h == rectnone.h)
    return 1;
  return 0;     
}

Rect rect_4point(double xl, double yl, double xr, double yr) {
  return (Rect) {xl,yl,xr-xl,yr-yl};
}

Rect rect_move(Rect r, double dx, double dy){
  return (Rect) {r.x+dx, r.y+dy, r.w, r.h};
}

Rect collide_rects(Rect r1, Rect r2) {
  if(r1.x + r1.w < r2.x || r1.x > r2.x + r2.w ||
     r1.y + r1.h < r2.y || r1.y > r2.y + r2.h)
    return rectnone;
  
  int xl = _MAX(r1.x,r2.x);
  int yl = _MAX(r1.y,r2.y);
  int xr = _MIN(r1.x + r1.w, r2.x + r2.w);
  int yr = _MIN(r1.y + r1.h, r2.y + r2.h);
  return rect_4point(xl,yl,xr,yr);
}

void drawrect(Rect r, ALLEGRO_COLOR c, double thickness) {
  if(r.w && r.h) {
    al_draw_line(r.x,       r.y,       
		 r.x + r.w, r.y,c,thickness);
    al_draw_line(r.x,       r.y,       
		 r.x,       r.y + r.h,c,thickness);
    al_draw_line(r.x + r.w, r.y,       
		 r.x + r.w, r.y + r.h,c,thickness);
    al_draw_line(r.x,       r.y + r.h, 
		 r.x + r.w, r.y + r.h,c,thickness);
  }
}

void drawrect_at(double x, double y, Rect r, ALLEGRO_COLOR c, double thickness) {
  if(r.w && r.h) {
    al_draw_line(x+r.x,       y+r.y,       
		 x+r.x + r.w, y+r.y,c,thickness);
    al_draw_line(x+r.x,       y+r.y,       
		 x+r.x,       y+r.y + r.h,c,thickness);
    al_draw_line(x+r.x + r.w, y+r.y,       
		 x+r.x + r.w, y+r.y + r.h,c,thickness);
    al_draw_line(x+r.x,       y+r.y + r.h, 
		 x+r.x + r.w, y+r.y + r.h,c,thickness);
  }
}

