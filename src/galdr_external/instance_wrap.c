#include "instance_wrap.h"

Type _TypeInstance = {"instance",NULL,NULL,
		      type_instance_print,
		      type_instance_getter,
		      type_instance_setter};
Type * TypeInstance = &_TypeInstance;

void type_instance_print(Value * v) {
  Instance * i = v->get;

  printf("<instance of object %s>",i->obj->name);
}

Value * type_instance_getter(Value * v, char * symbol) {
  if(!v)
    return Value_wrap_none();
  Instance * i = v->get;

  if(!strcmp(symbol,"x"))
    return Value_point_float(&i->x);
  if(!strcmp(symbol,"y"))
    return Value_point_float(&i->y);
  if(!strcmp(symbol,"xprev"))
    return Value_point_float(&i->xprev);
  if(!strcmp(symbol,"xprev"))
    return Value_point_float(&i->yprev);
  if(!strcmp(symbol,"visible"))
    return Value_point_int(&i->visible);
  if(!strcmp(symbol,"grounded"))
    return Value_point_int(&i->grounded);

  if(!strcmp(symbol,"sprite"))
    return Value_wrap_pointer(i->sprite);
  if(!strcmp(symbol,"object"))
    return Value_wrap_pointer(i->obj);

  return Value_wrap_none();
}

void type_instance_setter(Value * v, char * symbol, Value * val) {
  if(!v)
    return;
  Instance * i = v->get;

  if(!strcmp(symbol,"x")) {
    if(val->type == TypeFloat) 
      i->x = *((double*)val->get);
  }
  if(!strcmp(symbol,"y")) {
    if(val->type == TypeFloat)
      i->y = *((double*)val->get);
  }
  if(!strcmp(symbol,"visible")) {
    if(val->type == TypeInt)
      i->visible = *((int*)val->get);
  }
  if(!strcmp(symbol,"grounded")) {
    if(val->type == TypeInt)
      i->grounded = *((int*)val->get);
  }
}

Value * Value_wrap_instance(Instance * i) {
  return Value_new(i,TypeInstance);
}
