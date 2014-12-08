#include "keyboard.h"

int keyboard_keys[ALLEGRO_KEY_MAX] = {0};
int keyboard_up[UPMAX] = {-1};
int keyboard_press[PRESSMAX] = {-1};

int keyboard_check(int keycode){
  return keyboard_keys[keycode];
}

// DOWN
void keyboard_set_press(int keycode){
  for(int i=0; i<PRESSMAX; i++){
    if(keyboard_press[i] == -1){
      keyboard_press[i] = keycode;
      break;
    }  
  }
  keyboard_keys[keycode] = 1;
}

int keyboard_check_press(int keycode){
  for(int i=0; i<PRESSMAX; i++){
    if(keyboard_press[i] == keycode)
      return 1;
  }
  return 0;
}

void keyboard_reset_press(void){
  for(int i=0; i<PRESSMAX; i++){
    keyboard_press[i] = -1;
  }
}


// UP
void keyboard_set_up(int keycode){
  for(int i=0; i<UPMAX; i++){
    if(keyboard_up[i] == -1){
      keyboard_up[i] = keycode;
      break;
    }  
  }
  keyboard_keys[keycode] = 0;
}

int keyboard_check_up(int keycode){
  for(int i=0; i<UPMAX; i++){
    if(keyboard_up[i] == keycode)
      return 1;
  }
  return 0;
}

void keyboard_reset_up(void){
  for(int i=0; i<UPMAX; i++){
    keyboard_up[i] = -1;
  }
}
