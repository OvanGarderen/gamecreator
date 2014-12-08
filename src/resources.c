#include "resources.h"
#include <math.h>
#include <libconfig.h>
#include "debug.h"
#include <unistd.h>

#include "galdr/script.h"

// script
Resource resource_new_script(char * file, Scope * global){
  Script * r = calloc(sizeof(Script),1);
  r->type = RESSCRIPT;
  r->script = GDScript_new(file,global);

  printf("Loading script %s... ",file);
  GDScript_load(r->script);
  if(r->script->body)
    puts("succes!");
  else
    puts("failed!");
  return r;
}

void resource_kill_script(Resource r){
  GDScript_destroy(((Script *) r)->script);
  free(r);
}

// level
Resource resource_new_level(char * name, ALLEGRO_COLOR backgroundcol){
  Level * r = calloc(sizeof(Level),1);
  r->type = RESLEVEL;
  r->name = strclone(name);
  r->backgroundcol = backgroundcol;
  return r;
}

void resource_kill_level(Resource r){
  free(((Level*)r)->name);
  free(r);
}

Resource resource_load_level(char * file, Scope * global) {
  /* levels are saved in a libconfig file (.json - like)*/
  Resource r = NULL;

  config_t loader;
  config_init(&loader);
  if(config_read_file(&loader,file)){
    char * name = "<nameless room>";
    int colr = 0,colg = 0,colb = 0;
    int order = 0;

    config_lookup_string(&loader,"level.name",(const char **) &name);
    config_lookup_int(&loader,"level.backgroundcol.[0]",&colr);
    config_lookup_int(&loader,"level.backgroundcol.[1]",&colg);
    config_lookup_int(&loader,"level.backgroundcol.[2]",&colb);
    config_lookup_int(&loader,"level.order",&order);
    r = resource_new_level(name,al_map_rgb(colr,colg,colb));
    ((Level*)r)->order = order;

    GList * instancelist = NULL;

    config_setting_t * instances = config_lookup(&loader,"level.instances");
    if(instances) {
      config_setting_t * elem = NULL;
      int i = 0;
      while((elem = config_setting_get_elem(instances,i))){
        char * objname; float x,y;
        objname = (char*) config_setting_get_string_elem(elem,0);
        x = config_setting_get_float_elem(elem,1);
        y = config_setting_get_float_elem(elem,2);

        char * fullname = strappend(strclone(objname),".object");
        Object * obj = resourcelist_lookup(resources,fullname);
        InstanceSpec * ispec = malloc(sizeof(InstanceSpec));
        ispec->object = obj;
        ispec->x = x;
        ispec->y = y;
        instancelist = g_list_prepend(instancelist,ispec);
        free(fullname);
        i++;
      }
      ((Level*) r)->instances = instancelist;
    }

  } else {
    switch(config_error_type(&loader)){
    case CONFIG_ERR_FILE_IO: debug("Failed to open file %s for reading\n",file); break;
    case CONFIG_ERR_PARSE: debug("Level configuration failed to parse in file %s:%i :: %s\n",
                                 file,config_error_line(&loader),config_error_text(&loader)); break;
    case CONFIG_ERR_NONE: break;
    }
  }
  config_destroy(&loader);

  return r;
}

// image
Resource resource_new_image(char * file){
  Image * r = calloc(sizeof(Image),1);
  r->type = RESIMAGE;
  r->data = al_load_bitmap(file);
  if(!r->data){
    free(r);
    return NULL;
  }
  r->w = al_get_bitmap_width(r->data);
  r->h = al_get_bitmap_height(r->data);
  return r;
}

void resource_kill_image(Resource r){
  al_destroy_bitmap(((Image*)r)->data);
  free(r);
}

// sprite
Resource resource_new_sprite(char * name, Image * image,
                             int framewidth, int frameheight,
                             int animdelay) {
  // don't put in zero framewidth
  if(!image)
    return NULL;
  
  Sprite *r = calloc(sizeof(Sprite),1);
  r->type = RESSPRITE;
  r->name = strclone(name);
  r->image = image;

  r->fw = framewidth;
  r->fh = frameheight;

  // handle special cases to maximize size
  if(framewidth == -1)  r->fw = r->image->w;
  if(frameheight == -1) r->fh = r->image->h;

  r->bbox = (Rect) {0.0,0.0,r->fw,r->fh};
  
  if(r->fw)
    r->maxframe = fmax(1,r->image->w/r->fw);
  else
    r->maxframe = 1;
  r->curframe = 0;

  r->animdir = 1;
  r->animdelay = animdelay;
  r->ticks = 0;

  return r;
}

void resource_kill_sprite(Resource r) {
  free(((Sprite*)r)->name);
  free(r);
}

Resource resource_load_sprite(char * file){
  /* sprites are saved in a libconfig file (.json - like)*/
  Resource r = NULL;

  config_t loader;
  config_init(&loader);
  if(config_read_file(&loader,file)){
    char * name = "<nameless sprite>";
    char * imgpath = NULL;
    int fw=0, fh=0, animdelay=0;

    config_lookup_string(&loader,"sprite.name",(const char **) &name);
    config_lookup_string(&loader,"sprite.image",(const char **) &imgpath);

    config_lookup_int(&loader,"sprite.framewidth",&fw);
    config_lookup_int(&loader,"sprite.frameheight",&fh);
    config_lookup_int(&loader,"sprite.animdelay",&animdelay);

    Image * img = resource_load_image(imgpath);

    r = resource_new_sprite(name,img,fw,fh,animdelay);
  } else {
    switch(config_error_type(&loader)){
    case CONFIG_ERR_FILE_IO: debug("Failed to open file %s for reading\n",file); break;
    case CONFIG_ERR_PARSE: debug("Level configuration failed to parse in file %s:%i :: %s\n",
                                 file,config_error_line(&loader),config_error_text(&loader)); break;
    case CONFIG_ERR_NONE: break;
    }
  }
  config_destroy(&loader);

  return r;
}

// object
Resource resource_new_object(char * name, Sprite * sprite) {
  Object * r = calloc(sizeof(Object),1);
  r->type = RESOBJECT;
  r->name = strclone(name);
  r->sprite = sprite;
  return r;
}

void resource_kill_object(Resource r) {
  free(((Object*)r)->name);
  free(r);
}

Script * loadscript(char * methods, char * name, Scope * global) {
  char *fullpath = g_build_filename("games/sample_game/objects",
                              methods,name,NULL);
  Script * s = resource_new_script(fullpath,global);
  free(fullpath);
  return s;
}


Resource resource_load_object(char * file, Scope * global){
  /* sprites are saved in a libconfig file (.json - like)*/
  Resource r = NULL;

  config_t loader;
  config_init(&loader);
  if(config_read_file(&loader,file)){
    char * name = "<nameless object>";
    char * parent = NULL;
    char * sprite = NULL; Sprite * spr;

    int visible = 1;
    char * methods = NULL;

    config_lookup_string(&loader,"object.name",(const char **) &name);
    config_lookup_string(&loader,"object.parent",(const char **) &parent); // this seems to be an issue... ?parent-tree-loader?
    config_lookup_string(&loader,"object.sprite",(const char **) &sprite);

    config_lookup_int(&loader,"object.visible",&visible);
    config_lookup_string(&loader,"object.methods",(const char **) &methods);

    /* load the sprite */
    if(sprite){
      char * full = strappend(strclone(sprite),".sprite");
      spr = resourcelist_lookup(resources,full);
      free(full);
    }
    
    r = resource_new_object(name,spr);
    Object * o = r;
    if(parent) o->parent = resourcelist_lookup(resources,parent);
    o->visible = visible;

    /* load the methods */
    if(methods){
      o->step = loadscript(methods,"step.script",global);
      o->draw = loadscript(methods,"draw.script",global);
      o->collide = loadscript(methods,"collide.script",global);
    }
  } else {
    switch(config_error_type(&loader)){
    case CONFIG_ERR_FILE_IO: debug("Failed to open file %s for reading\n",file); break;
    case CONFIG_ERR_PARSE: debug("Level configuration failed to parse in file %s:%i :: %s\n",
                                 file,config_error_line(&loader),config_error_text(&loader)); break;
    case CONFIG_ERR_NONE: break;
    }
  }
  config_destroy(&loader);

  return r;
}

/////////////////////////////////////////////

void resource_kill(Resource r) {
  switch(*
         ((int*)r)) {
  case RESSCRIPT: resource_kill_script(r); break;
  case RESLEVEL: resource_kill_level(r); break;
  case RESSPRITE: resource_kill_sprite(r); break;
  case RESOBJECT: resource_kill_object(r); break;
  }
}

// loader

void free_key(gpointer key){
  free(key);
}

Resourcelist * resourcelist_init(void){
  return g_hash_table_new_full(g_str_hash, g_str_equal,
                               free_key, (GDestroyNotify) resource_kill);
}

void resourcelist_add(Resourcelist *rlist, char *name, Resource r){
  char * namecpy = strclone(name);
  g_hash_table_insert(rlist,namecpy,r);
}

Resource resourcelist_lookup(Resourcelist *rlist, char *name){
  return g_hash_table_lookup(rlist,name);
}

void resourcelist_remove(Resourcelist *rlist, char *name){
  g_hash_table_remove(rlist,name);
}

void resourcelist_destroy(Resourcelist *rlist){
  g_hash_table_destroy(rlist);
}

GList * resourcelist_getkeys(Resourcelist *rlist){
  return g_hash_table_get_keys(rlist);
}

void resourcelist_print(Resourcelist *rlist){
  GList * keysbase, *keys;
  keysbase = keys = resourcelist_getkeys(rlist);
  while(keys){
    //debug("%s\n",(char*) keys->data);
    keys = keys->next;
  }
  g_list_free(keysbase);

}

GList * resourcelist_getpattern(Resourcelist *rlist, char * pattern){
  GList * keys, * keysbase, *newret = NULL;
  keys = keysbase = resourcelist_getkeys(rlist);

  GPatternSpec * compiled = g_pattern_spec_new(pattern);
  while(keys) {
    if(g_pattern_match_string(compiled,keys->data)){
      Resource r = resourcelist_lookup(rlist,keys->data);
      newret = g_list_prepend(newret,r);
    }
    keys = keys->next;
  }
  g_list_free(keysbase);
  return newret;
}

// combine loading with adding

Resource resourcelist_load_script(Resourcelist *rlist, char * file, Scope * global){
  Script * scr = resource_load_script(file,global);
  char * name = g_path_get_basename(file);
  resourcelist_add(rlist,name,scr);
  free(name);
  return scr;
}

Resource resourcelist_load_level(Resourcelist *rlist, char * file, Scope * global){
  Level * lvl = resource_load_level(file,global);
  char * fullname = strappend(strclone(lvl->name),".level");
  resourcelist_add(rlist,fullname,lvl);
  free(fullname);
  return lvl;
}

Resource resourcelist_load_image(Resourcelist *rlist, char * file){
  Image * img = resource_load_image(file);
  char * name = g_path_get_basename(file);
  resourcelist_add(rlist,name,img);
  free(name);
  return img;
}

Resource resourcelist_load_sprite(Resourcelist *rlist, char * file){
  Sprite * spr = resource_load_sprite(file);
  if(!spr)
    return NULL;
  char * fullname = strappend(strclone(spr->name),".sprite");
  resourcelist_add(rlist,fullname,spr);
  free(fullname);
  return spr;
}

Resource resourcelist_load_object(Resourcelist *rlist, char * file, Scope * global){
  Level * obj = resource_load_object(file,global);
  char * fullname = strappend(strclone(obj->name),".object");
  resourcelist_add(rlist,fullname,obj);
  free(fullname);
  return obj;
}

void resourcelist_load_directory(Resourcelist *rlist, char * dir, int type, Scope * global){
  char * spec;
  switch(type){
  case RESSCRIPT: spec = "*.script"; break;
  case RESLEVEL: spec = "*.level"; break;
  case RESIMAGE: spec = "*.image"; break;
  case RESSPRITE: spec = "*.sprite"; break;
  case RESOBJECT: spec = "*.object"; break;
  default: return;
  }

  GPatternSpec * compiledspec = g_pattern_spec_new(spec);
  //debug("using pattern %s in %s\n",spec,dir);

  GDir * gdir = g_dir_open(dir,0,NULL);
  const char * curfile;

  while((curfile = g_dir_read_name(gdir))){
    if( g_pattern_match_string(compiledspec,curfile) ){
      //debug("\tfound %s\n",curfile);
      char * fullpath = g_build_filename(dir,curfile,NULL);

      switch(type){
      case RESSCRIPT: resourcelist_load_script(rlist,fullpath,global); break;
      case RESLEVEL: resourcelist_load_level(rlist,fullpath,global); break;
      case RESIMAGE: resourcelist_load_image(rlist,fullpath); break;
      case RESSPRITE: resourcelist_load_sprite(rlist,fullpath); break;
      case RESOBJECT: resourcelist_load_object(rlist,fullpath,global); break;
      }
      
      free(fullpath);
    }
  }
  g_dir_close(gdir);
}
