#include <allegro5/allegro.h>

#include "lib.h"
#include "../../keyboard.h"

Value * __inithandler(Scope * call) {
  Scope_add_global(call,"key-left",Value_wrap_int(ALLEGRO_KEY_LEFT));
  Scope_add_global(call,"key-right",Value_wrap_int(ALLEGRO_KEY_RIGHT));
  Scope_add_global(call,"key-up",Value_wrap_int(ALLEGRO_KEY_UP));
  Scope_add_global(call,"key-down",Value_wrap_int(ALLEGRO_KEY_DOWN));
  return Value_wrap_none();
}

Value * checkhandler(Scope * call) {
  int keycode = *((int*)Scope_get(call,"keycode")->get);
  return Value_wrap_int(keyboard_check(keycode));
}

Value * checkuphandler(Scope * call) {
  int keycode = *((int*)Scope_get(call,"keycode")->get);
  return Value_wrap_int(keyboard_check_up(keycode));
}

Value * checkpresshandler(Scope * call) {
  int keycode = *((int*)Scope_get(call,"keycode")->get);
  return Value_wrap_int(keyboard_check_press(keycode));
}


