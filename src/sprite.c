#include "sprite.h"
#include "resources.h"

void sprite_draw(Sprite *s, float x, float y){
  al_draw_bitmap_region(s->image->data,
			s->fw * s->curframe, 0.0,
			(float) s->fw, (float) s->fh,
			(float) x, (float) y,
			0);
}

void sprite_tick(Sprite *s){
  s->ticks++;
  if(s->ticks >= s->animdelay){
    s->ticks = 0;
    s->curframe += s->animdir;
  }
  if(s->curframe >= s->maxframe || s->curframe < 0)
    s->curframe %= s->maxframe; 
}

Sprite * sprite_copy(Sprite * spr){
  if(!spr) return NULL;
  return resource_new_sprite(spr->name,spr->image,
			     spr->fw, spr->fh,
			     spr->animdelay);
}
