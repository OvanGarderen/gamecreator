#pragma once

#include "types.h"

typedef void (*TypeMethod)(Value*);
typedef Value * (*Typegetter)(Value*,char*);
typedef void (*Typesetter)(Value*,char*,Value *);

struct Type {
  char * name;
  TypeMethod destroy;    /* method to destroy  instance of type */
  TypeMethod copy;       /* method to copy     instance of type */
  TypeMethod print;      /* method to print    instance of type */
  Typegetter getter;     /* method to get subvars       of type */
  Typesetter setter;     /* method to set subvars       of type */
};

Type * Type_new(char *name);
Type * Type_new_methods(char *name,
			TypeMethod destroy,
			TypeMethod print,
			TypeMethod copy,
			Typegetter getter,
			Typesetter setter
			);
void Type_destroy(Type * t);
void Type_print(Type * v);

void Type_call_destroy(Value * v);
void Type_call_print(Value * v);
void Type_call_copy(Value * v);
Value * Type_call_getter(Value * v, char * symbol);
void Type_call_setter(Value * v, char * symbol, Value * val);
