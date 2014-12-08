#pragma once
#include "types.h"

struct Scope {
  // scope info to be determined
  Scope * parent;
  List * vars;
};

struct Var {
  char * symbol;
  Value * value;
};

Scope * Scope_new(Scope * parent);
void Scope_destroy(Scope * scope);

void Scope_add(Scope * scope, char* symbol, Value * value);
void Scope_add_global(Scope * scope, char* symbol, Value * value);
void Scope_quickadd(Scope * scope, char* symbol, Value * value);
void Scope_remove(Scope * scope, char* symbol);
bool Scope_set(Scope * scope, char * symbol, Value * value);
Value * Scope_get(Scope * scope, char* symbol);
Value * Scope_resolve(Scope * scope, char * symbol);
void Scope_resolve_set(Scope * scope, char * symbol, Value * v);

Var * Var_new(char * symbol, Value * value);
void Var_destroy(Var * var);
