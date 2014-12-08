#include "misc.h"
#include "collision.h"
#include "logmap.h"
#include "logmap.h"
#include "object.h"
#include "debug.h"

Child * Child_make(void * obj, Rect rect){
  Child * c = new(Child);
  c->value = obj;
  c->area = rect;
  return c;    
}

void Child_free(Child *c){
  free(c);
}

Logmap * Logmap_new(float w, float h){
  Logmap * lm = new(Logmap);
  lm->w = w;
  lm->h = h;
  lm->submaps[0] = NULL;
  lm->children = NULL;
  return lm;
}

Logmap * Logmap_split(Logmap * lm) {
  /* non-destructive but returns the input anyway */
  lm->submaps[MAP_NW] = Logmap_new(lm->w/2,lm->h/2);
  lm->submaps[MAP_NE] = Logmap_new(lm->w/2,lm->h/2);
  lm->submaps[MAP_SE] = Logmap_new(lm->w/2,lm->h/2);
  lm->submaps[MAP_SW] = Logmap_new(lm->w/2,lm->h/2);
  return lm;
}

Logmap * Logmap_add(Logmap * lm, void * obj, Rect rect) {
  if(rect_isnone(rect))
    return lm;
  
  if(lm->submaps[0] == NULL && (lm->w <= LOGMAP_W_MIN ||
                                 lm->h <= LOGMAP_H_MIN ||
                                 lm->children == NULL)) {
    // If the logmap is an empty leaf just add the child
    lm->children = g_list_append(lm->children, Child_make(obj,rect));
  } else if(lm->submaps[0] == NULL) {
    // If the logmap is a full leaf then split it in 4 pieces
    lm = Logmap_split(lm);
    GList * childp = lm->children;
    while(childp){
      Child *child = (Child *) childp->data;
      lm = Logmap_add(lm, child->value, child->area);
      Child_free(child);
      childp = childp->next;
    }
    g_list_free(lm->children);
    lm = Logmap_add(lm, obj, rect);
  } else { 
    Rect overlap_NW = collide_rects((Rect) {0, 0, lm->w/2, lm->h/2},rect);
    Rect overlap_NE = collide_rects((Rect) {lm->w/2, 0, lm->w/2, lm->h/2},rect);
    Rect overlap_SW = collide_rects((Rect) {0, lm->h/2, lm->w/2, lm->h/2},rect);
    Rect overlap_SE = collide_rects((Rect) {lm->w/2, lm->h/2, lm->w/2, lm->h/2},rect);

    if(!rect_isnone(overlap_NW)){
      overlap_NW = rect_move(overlap_NW, 0.0, 0.0);
      Logmap_add(lm->submaps[MAP_NW], obj, overlap_NW);
    }
    if(!rect_isnone(overlap_NE)){
      overlap_NE = rect_move(overlap_NE, -lm->w/2, 0.0);
      Logmap_add(lm->submaps[MAP_NE], obj, overlap_NE);
    }
    if(!rect_isnone(overlap_SW)){
      overlap_SW = rect_move(overlap_SW, 0.0, -lm->h/2);
      Logmap_add(lm->submaps[MAP_SW], obj, overlap_SW);
    }
    if(!rect_isnone(overlap_SE)){
      overlap_SE = rect_move(overlap_SE, -lm->w/2, -lm->h/2);
      Logmap_add(lm->submaps[MAP_SE], obj, overlap_SE);
    }
  }
  return lm;
}

GList * Logmap_get(Logmap * lm, Rect rect) {
  if(lm->submaps[0] == NULL){
    return g_list_copy(lm->children);
  }

  Rect overlap_NW = collide_rects((Rect) {0, 0, lm->w/2, lm->h/2},rect);
  Rect overlap_NE = collide_rects((Rect) {lm->w/2, 0, lm->w/2, lm->h/2},rect);
  Rect overlap_SW = collide_rects((Rect) {0, lm->h/2, lm->w/2, lm->h/2},rect);
  Rect overlap_SE = collide_rects((Rect) {lm->w/2, lm->h/2, lm->w/2, lm->h/2},rect);

  /* - submaps are relative so change the offset
     - rect may overlap with multiple submaps  */
  GList * ret = NULL;
  if(!rect_isnone(overlap_NW)){
    ret = g_list_concat(ret,Logmap_get(lm->submaps[MAP_NW],
				       overlap_NW));
  }
  if(!rect_isnone(overlap_NE)){
    ret = g_list_concat(ret,Logmap_get(lm->submaps[MAP_NE],
				       rect_move(overlap_NE, -lm->w/2, 0.0)));
  }
  if(!rect_isnone(overlap_SW)){
    ret = g_list_concat(ret,Logmap_get(lm->submaps[MAP_SW],
				       rect_move(overlap_SW, 0.0, -lm->h/2)));
  }
  if(!rect_isnone(overlap_SE)){
    ret = g_list_concat(ret,Logmap_get(lm->submaps[MAP_SE],
				       rect_move(overlap_SE, -lm->w/2, -lm->h/2)));
  }
  return ret;
}

void Logmap_remove_child(Logmap *lm, void *obj){
  GList * childp = lm->children;
  while(childp) {
    Child * c = childp->data;
    if(c->value == obj) {
      Child_free(c);

      GList * t = childp->next;
      lm->children = g_list_remove_link(lm->children,childp);
      g_list_free1(childp);
      childp = t;
    } else {
      childp = childp->next;
    }
  }
}

#include "debug.h"

Logmap * Logmap_remove(Logmap * lm, void * obj, Rect at){
  if(!lm)
    return lm;
  if(lm->submaps[0] == NULL){
    Logmap_remove_child(lm,obj);
    return lm;
  }
  
  Rect overlap_NW = collide_rects((Rect) {0, 0, lm->w/2, lm->h/2},at);
  Rect overlap_NE = collide_rects((Rect) {lm->w/2, 0, lm->w/2, lm->h/2},at);
  Rect overlap_SW = collide_rects((Rect) {0, lm->h/2, lm->w/2, lm->h/2},at);
  Rect overlap_SE = collide_rects((Rect) {lm->w/2, lm->h/2, lm->w/2, lm->h/2},at);

  /* - submaps are relative so change the offset
     - rect may overlap with multiple submaps  */
  if(!rect_isnone(overlap_NW)){
    Logmap_remove(lm->submaps[MAP_NW], obj,
		  overlap_NW);
  }
  if(!rect_isnone(overlap_NE)){
    Logmap_remove(lm->submaps[MAP_NE], obj,
		  rect_move(overlap_NE, -lm->w/2, 0.0));
  }
  if(!rect_isnone(overlap_SW)){
    Logmap_remove(lm->submaps[MAP_SW], obj,
		  rect_move(overlap_SW, 0.0, -lm->h/2));
  }
  if(!rect_isnone(overlap_SE)){
    Logmap_remove(lm->submaps[MAP_SE], obj,
		  rect_move(overlap_SE, -lm->w/2, -lm->h/2));
  }
  return lm;
}

Logmap * Logmap_move(Logmap * lm, void * obj, Rect oldrect, Rect newrect){
  return Logmap_add(Logmap_remove(lm,obj,oldrect),obj,newrect);
}

void Logmap_draw(Logmap * lm, ALLEGRO_COLOR c, float thickness, float ofx, float ofy){
  if(!lm)
    return;
      
  if(lm->submaps[0]) {
    Logmap_draw(lm->submaps[MAP_NW],c,thickness, ofx        , ofy        );
    Logmap_draw(lm->submaps[MAP_NE],c,thickness, ofx+lm->w/2, ofy        );
    Logmap_draw(lm->submaps[MAP_SW],c,thickness, ofx        , ofy+lm->h/2);
    Logmap_draw(lm->submaps[MAP_SE],c,thickness, ofx+lm->w/2, ofy+lm->h/2);
  } else {
    GList * childp = lm->children;
    while(childp){
      Child * child = (Child *) childp->data;
      drawrect(rect_move(child->area,ofx,ofy),c,thickness);
      childp = childp->next;
    }
  }
}
