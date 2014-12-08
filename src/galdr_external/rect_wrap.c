#include "rect_wrap.h"
#include "../misc.h"

Type _TypeRect = {"rect",NULL,NULL,
		      type_rect_print,
		  type_rect_resolve,
		  NULL};
Type * TypeRect = &_TypeRect;

void type_rect_print(Value * v) {
  Rect * r = v->get;
  printf("rect: (%f,%f) %f x %f)",r->x,r->y,r->w,r->h);
}

Value * type_rect_resolve(Value * v, char * symbol) {
  Rect * r = v->get;

  if(!strcmp(symbol,"x"))
    return Value_point_float(&r->x);
  if(!strcmp(symbol,"y"))
    return Value_point_float(&r->y);
  if(!strcmp(symbol,"w"))
    return Value_point_float(&r->w);
  if(!strcmp(symbol,"h"))
    return Value_point_float(&r->h);

  return Value_wrap_none();
}

Value * Value_wrap_rect(Rect r) {
  Rect * rp = new(Rect);
  *rp = r;
  return Value_new(rp,TypeRect);
}
