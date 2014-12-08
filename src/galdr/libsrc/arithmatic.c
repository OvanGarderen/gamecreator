#include "lib.h"

Value * addhandler(Scope * call){
  Value * a = Scope_get(call,"a");
  Value * b = Scope_get(call,"b");

  if(a->type == TypeFloat && b->type == TypeFloat)
    return Value_wrap_float(*((double*)a->get) + *((double*)b->get));
  else
    return Value_wrap_int(*((int*)a->get) + *((int*)b->get));
}

Value * subhandler(Scope * call){
  Value * a = Scope_get(call,"a");
  Value * b = Scope_get(call,"b");

  if(a->type == TypeFloat && b->type == TypeFloat)
    return Value_wrap_float(*((double*)a->get) - *((double*)b->get));
  else
    return Value_wrap_int(*((int*)a->get) - *((int*)b->get));
}

Value * multhandler(Scope * call){
  Value * a = Scope_get(call,"a");
  Value * b = Scope_get(call,"b");

  if(a->type == TypeFloat && b->type == TypeFloat)
    return Value_wrap_float(*((double*)a->get) * *((double*)b->get));
  else
    return Value_wrap_int(*((int*)a->get) * *((int*)b->get));
}

Value * divhandler(Scope * call){
  Value * a = Scope_get(call,"a");
  Value * b = Scope_get(call,"b");

  if(a->type == TypeFloat && b->type == TypeFloat) {
    return Value_wrap_float(*((double*)a->get) / *((double*)b->get));
  } else {   
    if(*((int*) b->get) == 0)
      return Value_wrap_error(eNulDiv,"integer division by zero",call);
    return Value_wrap_int(*((int*)a->get) / *((int*)b->get));
  }
}

Value * modhandler(Scope * call){
  Value * a = Scope_get(call,"a");
  Value * b = Scope_get(call,"b");

  if(*((int*) b->get) == 0){
    return Value_wrap_error(eNulDiv,"modulo of zero",call);
  }
  return Value_wrap_int(*((int*)a->get) % *((int*)b->get));
}
