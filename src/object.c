#include "debug.h"

#include <string.h>

#include "object.h"
#include "script.h"
#include "sprite.h"
#include "resources.h"

#include "galdr/scope.h"
#include "galdr/script.h"
#include "galdr_external/instance_wrap.h"
#include "galdr_external/rect_wrap.h"

void object_call_step(Object * o, Instance * self){
  // makes sure inheritance works as expected
  if(o->step)
    script_call_method(o->step,self);
  else if(o->parent)
    object_call_step(o->parent,self);   
}

void instance_call_step(Instance * i){
  object_call_step(i->obj,i);
}

void instance_update(Instance * i) {
  i->x += i->vx;
  i->y += i->vy;

  if(i->obj->sprite){
    Rect oldpos = rect_move(i->obj->sprite->bbox,i->xprev,i->yprev);
    Rect newpos = rect_move(i->obj->sprite->bbox,i->x,i->y);
    collisionmap = Logmap_move(collisionmap, i, oldpos, newpos);
  }
  
  i->xprev = i->x;
  i->yprev = i->y;
}


// collide
void object_call_collide(Object * o, Instance * self, Instance * other){
  // makes sure inheritance works as expected
  if(o->collide) {
    script_call_method(o->collide,self);
  }
  else if(o->parent)
    object_call_collide(o->parent,self,other);   
}

void instance_call_collide(Instance * i, Instance * other){
  Rect rect = instance_collide(i,other);
  if(!rect_isnone(rect) && i != other) {
    GDScript * script = i->obj->collide->script;
    Scope_set(script->locals,"other",Value_wrap_instance(other));
    Scope_set(script->locals,"collision",Value_wrap_rect(rect));
    object_call_collide(i->obj,i,other);
  }
}

void instance_change_sprite(Instance * i, Sprite *spr){
  if(i->sprite)
    resource_kill_sprite(i->sprite);
  if(spr)
    i->sprite = sprite_copy(spr);
}

Instance * object_implement(Object * o){
  if(!o)
    return NULL;
  Instance * i = malloc(sizeof(Instance));
  i->obj = o;
  i->xprev = i->yprev = i->x = i->y = i->vx = i->vy = 0.0;
  i->sprite = NULL;
  instance_change_sprite(i,o->sprite);
  i->visible = o->visible;
  i->grounded = 0;
  
  return i;
}

Rect instance_collide(Instance * o1, Instance * o2) {
  if(!o1->sprite || !o2->sprite)
    return rectnone;

  Rect r1 = o1->sprite->bbox;
  Rect r2 = o2->sprite->bbox;

  r1.x += o1->x;
  r1.y += o1->y;

  r2.x += o2->x;
  r2.y += o2->y;

  return collide_rects(r1,r2);
}
