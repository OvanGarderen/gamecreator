#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>

#define draw_text(font, col, x, y, fmt, ...) do{ char *_buffer; \
  asprintf(&_buffer,fmt, ##__VA_ARGS__); \
  al_draw_text(font,col,x,y,0,_buffer);	 \
  free(_buffer);} while(false)
