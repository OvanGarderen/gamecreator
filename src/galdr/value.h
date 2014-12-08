#pragma once

#include <stdbool.h>
#include <stdio.h>
#include "types.h"

struct Value {
  void * get;
  Type * type;
  int ref;
  bool external;
};

Value * Value_new(void * get, Type * t);
void Value_destroy(Value * v);
Value * Value_cpy(Value * v);

Value * Value_ref(Value * v);
Value * Value_unref(Value * v);

void Value_print(Value * v);

Value * Value_wrap_int(int i);
Value * Value_wrap_float(double f);
Value * Value_wrap_string(char * s); /* copies the string */
Value * Value_wrap_symbol(char * s); /* copies the string */
Value * Value_wrap_pointer(void * p);
Value * Value_wrap_list(List * l);
Value * Value_wrap_funcdef(Funcdef * f);
Value * Value_wrap_scope(Scope * s);
Value * Value_wrap_file(FILE * fp);

Value * Value_wrap_error(Scope * context, int code, char * message, ...);
Value * Value_wrap_type(Type * t);
Value * Value_wrap_none(void);

Value * Value_point_int(int *i);
Value * Value_point_float(double *f);
Value * Value_point_string(char *s);  /* does not copy */
Value * Value_point_list(List * f);
Value * Value_point_funcdef(Funcdef * f);
Value * Value_point_scope(Scope * s);

Value * Value_wrap_literal(Value * v); /* returns (lit v) */

/* this is where parsing gets involved */

Value * Value_read(char * raw);
