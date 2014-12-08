#include <stdio.h>

#include "shared.h"
#include "misc.h"

#include "type.h"
#include "value.h"

Type * Type_new(char *name) {
  Type * t = new(Type);
  t->name = strclone(name);
  return t;
}

Type * Type_new_wmethods(char *name,
			 TypeMethod destroy,
			 TypeMethod print,
			 Typegetter getter,
			 Typesetter setter) {
  Type * t = Type_new(name);
  t->print = print;
  t->destroy = destroy;
  t->getter = getter;
  t->setter = setter;
  return t;
}

void Type_destroy(Type * t) {
  free(t->name);
  free(t);
}

void Type_print(Type * t) {
  printf("%s",t->name);
}

void Type_call_destroy(Value * v) {
  if(v->type->destroy && !v->external)
    v->type->destroy(v);
}

void Type_call_print(Value * v) {
  if(v->type->print)
    v->type->print(v);
  else
    printf("<value of type %s at %p>",v->type->name,v->get);
}

void Type_call_copy(Value * v) {
  if(v->type->copy && !v->external)
    v->type->copy(v);
}

Value * Type_call_getter(Value * v, char * symbol) {
  if(v->type->getter)
    return v->type->getter(v,symbol);
  else
    return Value_wrap_none();
}

void Type_call_setter(Value * v, char * symbol, Value * val) {
  if(v->type->setter)
    v->type->setter(v,symbol,val);
}
