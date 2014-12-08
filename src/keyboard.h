#pragma once
#include <allegro5/allegro.h>
#define UPMAX 20
#define PRESSMAX 20

int keyboard_keys[ALLEGRO_KEY_MAX];
int keyboard_up[UPMAX];
int keyboard_press[PRESSMAX];

int keyboard_check(int keycode);

void keyboard_set_press(int keycode);
int keyboard_check_press(int keycode);
void keyboard_reset_press(void);

void keyboard_set_up(int keycode);
int keyboard_check_up(int keycode);
void keyboard_reset_up(void);

