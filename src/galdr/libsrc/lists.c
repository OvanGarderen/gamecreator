#include "lib.h"
#include "assert.h"

Value * listhandler(Scope * call) {
  List * args = Scope_get(call,"args")->get;
  return Value_wrap_list(List_cpy(args));
}

Value * concathandler(Scope * call) {
  List * la = List_cpy(Scope_get(call,"la")->get);
  List * lb = List_cpy(Scope_get(call,"lb")->get);
  if(!la)
    return Value_wrap_list(lb);
  return Value_wrap_list(List_concat(la,lb));
}

Value * headhandler(Scope * call) {
  Value * v = Scope_get(call,"list");
  Value_print(v);
  puts("");
  List * a = v->get;
  if(!a)
    return Value_wrap_error(eArgType,"Taking the head of an empty list is not defined",
			    call);
  return Value_ref(a->val);
}

Value * butthandler(Scope * call) {
  List * a = Scope_get(call,"list")->get;
  if(!a)
    return Value_wrap_error(eArgType,"Taking the head of an empty list is not defined",
			    call);

  while(a->next)
    a = a->next;
  return Value_ref(a->val);
}

Value * tailhandler(Scope * call) {
  List * a = Scope_get(call,"list")->get;
  if(!a)
    return Value_point_list(NULL);
  return Value_point_list(a->next);
}

Value * emptyhandler(Scope * call) {
  Value * a = Scope_get(call,"list")->get;
  if(a)
    return Value_wrap_int(1);
  return Value_wrap_int(0);
}

