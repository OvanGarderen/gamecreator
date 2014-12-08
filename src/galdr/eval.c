#include <stdio.h>

#include "eval.h"

#include "error.h"
#include "value.h"
#include "type_internal.h"
#include "scope.h"
#include "list.h"
#include "funcdef.h"

Value * eval(Value * v, Scope * context) {
  if(!v)
    return Value_wrap_error(context,eIllegal,"Tried to eval null-pointer.");

  if(v->type == TypeSymbol)
    return Value_ref(Scope_resolve(context,v->get));
  if(v->type == TypeList) {
    List * expr = v->get;
    
    if(!expr) /* () is an expression for NIL */
      return Value_wrap_none();

    /* extract the fuction as the first argument */
    Value * function = eval(expr->val, context);
    if(function->type == TypeError)
      return function;
    else if(function->type != TypeFunc) {
      Value_unref(function);
      return Value_wrap_error(context,eType,"Expression does not evaluate to a function.");
    }

    Funcdef * f = function->get;
    List * args;
    
    if(!f->ismacro) {
      List * args_reversed = func_build_arglist(expr->next,f->args, context, NULL);
      args = List_reverse(args_reversed);
    } else {
      args = expr->next;
    }
    
    Value * result = Funcdef_call(f, args, context);
    
    if(!f->ismacro)
      List_destroy_value(args);
    Value_unref(function);
    
    return result;
  }
  
  return Value_ref(v);
}
