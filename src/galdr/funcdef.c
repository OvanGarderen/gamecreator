#include <stdlib.h>

#include "shared.h"
#include "misc.h"

#include "types.h"

#include "scope.h"
#include "value.h"
#include "funcdef.h"
#include "list.h"
#include "eval.h"
#include "type_internal.h"
#include "error.h"

Funcdef * Funcdef_new(char * name, List *args, List *body) {
  Funcdef * fdef = new(Funcdef);
  fdef->name = strclone(name);

  while(args){
    Value * v = Value_cpy(args->val);
    fdef->args = List_prepend(fdef->args,v);
    args = args->next;
  }

  fdef->args = List_reverse(fdef->args);
  fdef->body = List_cpy(body);
  fdef->caller = Funcdef_stdcaller;
  return fdef;
}

void Funcdef_destroy(Funcdef * fdef) {
  free(fdef->name);
  List_destroy_value(fdef->args);
  List_destroy_value(fdef->body);
  free(fdef);
}

List * func_build_arglist(List * input, List * fargs, Scope * context, List *tail) {
  if(!input && !fargs)
    return tail;
  if(!input)
    return List_prepend(tail, Value_wrap_error(context,
					       eArgType,"Wrong number of arguments."));

  /* @ARG (uppercase) is a keyword */
  if( fargs ) {
    Value * curinput = input->val;
    Value * curfarg = fargs->val;
    if(((char*) curfarg->get)[0] == '@'
       && isallupper(((char*) curfarg->get)+1) ) {
      if (curinput->type != TypeSymbol ||
          strcmp_nocase(((char*) curfarg->get)+1,
                        (char*) curinput->type))
        List_prepend(tail,
		     Value_wrap_error(context,eArgType,"Keyword %s not satisfied.",((char*) curfarg->get)+1));
      else
        List_prepend(tail,Value_wrap_none());
    } else {
      Value * result = eval(curinput,context);
      if(result->type == TypeExList) {
	tail = List_concat(List_reverse(result->get), tail);
      } else {
	tail = List_prepend(tail, result);
      }
    }
    return func_build_arglist(input->next, fargs->next, context, tail);
  } else {
    tail = List_prepend(tail, eval(input->val,context));
    return func_build_arglist(input->next, NULL, context, tail);
  }
}

bool check_keywords(List * fargs, List * args) {
  while(fargs && args) {
    /* keywords are in upper case */
    if( ((char*)((Value*)fargs->val)->get)[0] == '@'
        && (((Value*) args->val)->type != TypeSymbol
            || strcmp_nocase(((char*)((Value*)fargs->val)->get)+1,
                             ((Value*)args->val)->get)))
      return false;
    fargs = fargs->next;
    args = args->next;
  }
  return true;
}

Value * Funcdef_call(Funcdef *func, List * arguments, Scope *context) {
  Scope * call = Scope_new(context);
  Scope_quickadd(call,"__func", Value_point_funcdef(func));

  List * argp = func->args;
  while(argp && arguments){
    Value * v = argp->val; /* checked internally to be a symbol */

    if(((char*)v->get)[0] == '@') {
      /* @ can mean either a keyword or remainder arguments */
      if( isallupper(((char*)v->get)+1) ) {
        /* keywords are allready satisfied */
      } else {
        /* @arg is a list of `remainder arguments' */
        Value * l = Value_point_list(arguments);
        Scope_quickadd(call, v->get+1 , l); /* strips off the @ */
        break;
      }
    } else {
      /* normal arguments */
      Scope_quickadd(call, v->get, Value_ref(arguments->val));
    }
    arguments = arguments->next;
    argp = argp->next;
  }

  Value * ret = func->caller(call);
  Scope_destroy(call);
  return ret;
}

/* The list  of arguments consists of values with a symbol type */
Value * Funcdef_stdcaller(Scope * call) {
  Value * func = Scope_get(call,"__func");
  Funcdef * f = func->get;
  Value * body = Value_wrap_list(f->body);
  Value * result = eval(body,call);
  Value_unref(body);
  return result;
}
