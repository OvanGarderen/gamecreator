#include "shared.h"
#include "debug.h"

#include "resources.h"
#include <glib.h>

#include "keyboard.h"

#include "collision.h"
#include "logmap.h"

#include "galdr/scope.h"
#include "galdr/script.h"
#include "galdr/value.h"
#include "galdr/builtins.h"
#include "galdr/external.h"
#include "galdr_external/instance_wrap.h"
#include "galdr_external/rect_wrap.h"
#include "galdr/eval.h"

int width = 620;
int height = 480;

double fps = 60;
long ticks = 1;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer;

Scope * global;

int main(int argc, char *argv[]) {
  srand(time(NULL));

  // Initialise all the allegro shit
  if(!al_init())
    return -1;

  display = al_create_display(width,height);
  if(!display)
    return -1;

  al_install_keyboard();
  al_init_image_addon();
  al_init_font_addon();
  al_init_ttf_addon();
  al_init_primitives_addon();

  event_queue = al_create_event_queue();
  timer = al_create_timer(1.0/fps);
  al_register_event_source(event_queue, al_get_timer_event_source(timer));
  al_register_event_source(event_queue, al_get_keyboard_event_source());
  al_start_timer(timer);

  ALLEGRO_FONT *stdfont = al_load_font("font/LSRegular.ttf",18,0);
  ALLEGRO_COLOR stdcolor = al_map_rgb(255,255,255);
  ALLEGRO_COLOR rectcolor = al_map_rgb(180,40,40);

  // initialise LUA state

  global = Scope_new(NULL);
  Scope_add(global,"__line",Value_wrap_int(0));
  Scope_add(global,"__file",Value_point_string("global game scope"));
  Scope_add(global,"__lib-dir",
	    Value_point_string("/home/okke/projects/gamecreator/libs"));
  Scope_add(global,"__extern-dir",
	    Value_point_string("/home/okke/projects/gamecreator/galdr_bind"));
  Scope_add_builtins(global);
  Scope_add(global,"global",Value_point_scope(global));

  Scope_add(global,"io",external_library_load_bind(global,
       "/home/okke/projects/gamecreator/libs",
       "/home/okke/projects/gamecreator/galdr_bind/io.gdlib"));
  Scope_add(global,"math",external_library_load_bind(global,
       "/home/okke/projects/gamecreator/libs",
       "/home/okke/projects/gamecreator/galdr_bind/arithmatic.gdlib"));
  Scope_add(global,"lists",external_library_load(
       "/home/okke/projects/gamecreator/libs",
       "/home/okke/projects/gamecreator/galdr_bind/lists.gdlib"));
  Scope_add(global,"keyboard",external_library_load(
       "/home/okke/projects/gamecreator/libs",
       "/home/okke/projects/gamecreator/galdr_bind/keyboard.gdlib"));

  Value * v = Value_read(strclone("(keyboard.__init)"));
  Value_unref(eval(v,global));
  
  // init resources
#define gameroot "games/pikkenzuiger/"

  resources = resourcelist_init();

  resourcelist_load_directory(resources, gameroot "sprites", RESSPRITE, global);
  resourcelist_load_directory(resources, gameroot "objects", RESOBJECT, global);
  resourcelist_load_directory(resources, gameroot "levels",  RESLEVEL, global);

  resourcelist_print(resources);

  gint find_pattern(gconstpointer str, gconstpointer spec){
    return !(g_pattern_match_string((GPatternSpec*)spec,(char *)str));
  }

  GList * levels = resourcelist_getpattern(resources,"*.level");
  if(!levels){
    debug("No levels defined in this project, aborting"); exit(1);
  }

  gint sort_levels(gconstpointer lvl1, gconstpointer lvl2){
    if(!lvl1
       || !lvl2
       || ((Level*)lvl1)->type!=RESLEVEL
       || ((Level*)lvl2)->type!=RESLEVEL)
      return -1;
    return ((Level*)lvl1)->order - ((Level*)lvl2)->order;
  }

  levels = g_list_sort(levels,sort_levels);

  Level * curlevel = levels->data;
  if(!curlevel) {
    debug("Something went terribly wrong"); exit(1);
  }

  GList * instances = NULL;
  GList * instancespecs = curlevel->instances;
  collisionmap = Logmap_new((double) width, (double) height);

  while(instancespecs && instancespecs->data){
    InstanceSpec * ispec = instancespecs->data;
    //debug("%s at %f:%f",ispec->object->name,ispec->x,ispec->y);
    Instance * i = object_implement(ispec->object);
    //debug("%p",ispec->object);
    if(i){
      i->x = ispec->x;
      i->y = ispec->y;
      instances = g_list_prepend(instances,i);
      if(i->sprite){
        collisionmap = Logmap_add(collisionmap,
				  i,
				  rect_move(i->sprite->bbox,i->x,i->y));
      }
    }
    instancespecs = instancespecs->next;
  }

  // actual game logic

  debug("Starting game");

  double old_time,new_time;
  old_time = al_get_time();

  bool done = false;

  Value * selfv = Value_wrap_instance(NULL);
  Scope_quickadd(global,"self",selfv);
  
  while(!done){
    ALLEGRO_EVENT ev;
    bool render = true;

    al_wait_for_event(event_queue, &ev);

    if(ev.type == ALLEGRO_EVENT_KEY_DOWN){
      if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
        done = true;
      keyboard_set_press(ev.keyboard.keycode);
    } else if(ev.type == ALLEGRO_EVENT_KEY_UP){
      keyboard_set_up(ev.keyboard.keycode);
    } else if(ev.type == ALLEGRO_EVENT_TIMER){
      render = true;

      ticks++;
      
      // iterate step-events
      GList * curinstance = instances;
      while(curinstance) {
        Instance * self = curinstance->data;
	selfv->get = self;
        instance_call_step(self);

	if(self->obj->sprite){
	  GList * collisions = Logmap_get(collisionmap,
					  rect_move(self->obj->sprite->bbox,self->x, self->y));
	  GList * collp = collisions;
	  while(collp){
	    Instance * c = ((Child *)collp->data)->value;
	    if(c != self && !rect_isnone(
					    collide_rects(c->obj->sprite->bbox,self->obj->sprite->bbox)))
	      instance_call_collide(self,c);
	    collp = collp->next;
	  }
	  g_list_free(collisions);
	}
        instance_update(self);
        curinstance = curinstance->next;
      }

      keyboard_reset_up();
      keyboard_reset_press();
    }

    if( render && al_is_event_queue_empty(event_queue)){
      al_set_target_backbuffer(display);

      new_time = al_get_time();
      double truefps = 1/(new_time - old_time);
      old_time = new_time;
      al_draw_textf(stdfont,stdcolor, 480, 10, 0, "FPS: %f", truefps);

      // render object instances and stuff
      al_draw_textf(stdfont, stdcolor, 10,10,0, "%s", curlevel->name);

      GList * curinstance = instances;
      while(curinstance){
        Instance * self = curinstance->data;
        sprite_tick(self->sprite);
        sprite_draw(self->sprite,self->x,self->y);
        curinstance = curinstance->next;
      }

      Logmap_draw(collisionmap,rectcolor,1.0,0.0,0.0);
      
      al_flip_display();
      al_clear_to_color(curlevel->backgroundcol);

      render = false;
    }
  }

  Scope_destroy(global);

  return 0;
}
