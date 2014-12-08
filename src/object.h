#pragma once

#include "typedefs.h"
#include "collision.h"

struct Object{
  int type;
  // an object is basically a body of constants
  // and methods
  char * name;
  Object * parent;
  Sprite * sprite; //this is the starting sprite
  int visible;

  // methods
  Script * step;
  Script * draw;
  Script * collide;
};

struct Instance{
  // an instance implements an object and therefore copies some of
  // the object's constants into its own mutable variables
  Object * obj; //what is implemented

  double x,y;
  double xprev, yprev;
  double vx,vy;

  Sprite * sprite; //works with copies of the sprite --> deallocate!
  int visible;

  int grounded;
};

void object_call_step(Object * o, Instance * self);
void instance_call_step(Instance * i);
void instance_update(Instance * i);

void object_call_collide(Object * o, Instance * self, Instance * other);
void instance_call_collide(Instance * i, Instance * other);

Instance * object_implement(Object * o);
void instance_change_sprite(Instance * i, Sprite *spr);
Rect instance_collide(Instance * o1, Instance * o2);

#include "logmap.h"
Logmap * collisionmap;
