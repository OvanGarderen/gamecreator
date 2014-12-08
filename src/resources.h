#pragma once

#include <glib.h>
#include "drawtools.h"

#include "shared.h"
#include "typedefs.h"
// resources

enum {
  RESSCRIPT,
  RESLEVEL,
  RESIMAGE,
  RESSPRITE,
  RESOBJECT,
};

#include "script.h"

struct InstanceSpec {
  Object * object;
  float x, y;
};

typedef struct InstanceSpec InstanceSpec;

struct Level{
  int type;
  char * name;
  ALLEGRO_COLOR backgroundcol;
  int order;
  GList * instances;
};

struct Image{
  int type;
  ALLEGRO_BITMAP * data;
  int w, h;
};

#include "sprite.h"

#include "object.h"

typedef void * Resource;

Resource resource_new_script(char * file, Scope * global);
void resource_kill_script(Resource r);
#define resource_load_script(file,global) resource_new_script(file,global)

Resource resource_new_level(char * name, ALLEGRO_COLOR backgroundcol);
void resource_kill_level(Resource r);
Resource resource_load_level(char * file, Scope * global);

Resource resource_new_image(char * file);
void resource_kill_image(Resource r);
#define resource_load_image(file) resource_new_image(file)

Resource resource_new_sprite(char * name, Image * image,
                             int framewidth, int frameheight, int animspeed);
void resource_kill_sprite(Resource r);
Resource resource_load_sprite(char * file);

Resource resource_new_object(char * name, Sprite * sprite);
void resource_kill_object(Resource r);
Resource resource_load_object(char * file, Scope * global);

void resource_kill(Resource r);

// resource loader

typedef GHashTable Resourcelist;

Resourcelist * resourcelist_init(void);
void resourcelist_add(Resourcelist *rlist, char *name, Resource r);
Resource resourcelist_lookup(Resourcelist *rlist, char *name);
void resourcelist_remove(Resourcelist *rlist, char *name);
void resourcelist_destroy(Resourcelist *rlist);
GList * resourcelist_getkeys(Resourcelist *rlist);
void resourcelist_print(Resourcelist *rlist);
GList * resourcelist_getpattern(Resourcelist *rlist, char * pattern);

Resource resourcelist_load_script(Resourcelist *rlist, char * file, Scope * global);
Resource resourcelist_load_level(Resourcelist *rlist, char * file, Scope * global);
Resource resourcelist_load_image(Resourcelist *rlist, char * file);
Resource resourcelist_load_sprite(Resourcelist *rlist, char * file);
Resource resourcelist_load_object(Resourcelist *rlist, char * file, Scope * global);

Resourcelist * resources;

void resourcelist_load_directory(Resourcelist *rlist, char * dir, int type, Scope * global);
