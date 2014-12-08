#include "lib.h"
#include <string.h>
#include <stdio.h>

Value *
equalhandler(Funcdef *func, List *args, Context *context){
  Value *a = args->value;
  Value *b = args->next->value;
  return Value_wrap_int(Value_equal(a,b));
}

Value *
gthandler(Funcdef *func, List *args, Context *context){
  Value *a = args->value;
  Value *b = args->next->value;
  return Value_wrap_int(Value_greater(a,b));
}

Value *
andhandler(Funcdef *func, List *args, Context *context){
  Value *a = args->value;
  Value *b = args->next->value;

  if( *((int*) a->get) && *((int*) b->get) )
    return Value_wrap_int(1);
  return Value_wrap_int(0);  
}

Value *
orhandler(Funcdef *func, List *args, Context *context){
  Value *a = args->value;
  Value *b = args->next->value;

  if( *((int*) a->get) || *((int*) b->get) )
    return Value_wrap_int(1);
  return Value_wrap_int(0);  
}

Value *
nothandler(Funcdef *func, List *args, Context *context){
  Value *a = args->value;

  if( ! *((int*) a->get) )
    return Value_wrap_int(1);
  return Value_wrap_int(0);  
}
