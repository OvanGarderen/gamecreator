#pragma once

#include <stdbool.h>
#include "types.h"

struct Funcdef {
  char * name;
  List * args;
  List * body;
  bool ismacro;
  FuncdefCaller caller;
};

Funcdef * Funcdef_new(char * name, List *args, List *body);
void Funcdef_destroy(Funcdef * f);

bool check_keywords(List * fargs, List * args);
Value * Funcdef_call(Funcdef *func, List * arguments, Scope *context);

Value * Funcdef_stdcaller(Scope * call);

List * func_build_arglist(List * input, List * fargs, Scope * context, List *tail);
