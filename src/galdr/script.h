#pragma once

struct GDScript {
  char * file;
  Scope * locals;
  List * body;
};

GDScript * GDScript_new(char * file, Scope * globals);
void GDScript_load(GDScript * s);
void GDScript_destroy(GDScript * s);

Value * GDScript_run(GDScript * s);
