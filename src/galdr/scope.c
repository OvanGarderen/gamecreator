#include <string.h>
#include <stdlib.h>

#include "misc.h"
#include "shared.h"

#include "error.h"
#include "scope.h"
#include "value.h"
#include "list.h"
#include "type_internal.h"
#include "type.h"

/*     Scope     */

Scope * Scope_new(Scope * parent) {
  Scope * scope = new(Scope);
  scope->parent = parent;
  scope->vars = NULL;
  return scope;
}

void Scope_destroy(Scope * scope) {
  List_exec(scope->vars,(DestructorFunc) Var_destroy);
  List_destroy(scope->vars);
  free(scope);
}

void Scope_add(Scope * scope, char* symbol, Value * value) {
  Scope_remove(scope, symbol);
  Var * var = Var_new(symbol, value);
  scope->vars = List_prepend(scope->vars,var);
}

void Scope_add_global(Scope * scope, char* symbol, Value * value) {
  while(scope->parent)
    scope = scope->parent;
  Scope_add(scope,symbol,value);
}

/* does not check for existence of variable */
void Scope_quickadd(Scope * scope, char* symbol, Value * value){
  Var * var = Var_new(symbol, value);
  scope->vars = List_prepend(scope->vars,var);
}

void Scope_remove(Scope * scope, char * symbol){
  List * varp = scope->vars;
  while(varp){
    Var * var = varp->val;
    if(!strcmp(symbol,var->symbol)){
      Var_destroy(var);
      scope->vars = List_unlink(scope->vars,varp);
      List_destroy(varp);
      return;
    }
    varp = varp->next;
  }
}

/* returns true if succesfull */
bool Scope_set(Scope * scope, char * symbol, Value * value){ 
  List * varp = scope->vars;
  while(varp){
    Var * var = varp->val;
    if(!strcmp(symbol,var->symbol)){
      var->value = value;
      return true;
    }
    varp = varp->next;
  }
  if(scope->parent)
    return Scope_set(scope->parent,symbol,value);
  else
    return false;
}

Value * Scope_get(Scope * scope, char * symbol){
  List * varp = scope->vars;
  while(varp) {
    Var * var = varp->val;
    if(!strcmp(symbol,var->symbol)) {
      return var->value;
    }
    varp = varp->next;
  }
  if(scope->parent)
    return Scope_get(scope->parent,symbol);
  Value * e = Value_wrap_error(scope,eNotFound,"\"%s\" : no such symbol in scope.",symbol);
  return e;  
}

Value * Scope_resolve(Scope * scope, char * symbol) {
  /* works with var.subvar notation 
     and exlist notation */
  if(symbol[0] == '@') {
    Value * v = Scope_resolve(scope,symbol+1);
    if(v->type != TypeList)
      return Value_wrap_error(scope,eArgType,
	    "Trying to expand \"%s\" which is not a list", symbol);
    Value * excpy = Value_wrap_list(v->get);
    excpy->type = TypeExList;
    return excpy;
  }
  
  char * split = strchr(symbol,'.');
  if(split) {
    char * buffer = strclone(symbol);
    buffer[split-symbol] = '\0';
    Value * v = Scope_get(scope,buffer);
    if(!v->type->getter) {
      Value * e = Value_wrap_error(scope, eArgType,
			      "Cannot subvar variable %s of type %s.",buffer,v->type->name);
      free(buffer);
      return e;
    }
    return Type_call_getter(v,split+1);
  }
  return Scope_get(scope,symbol);
}

void Scope_resolve_set(Scope * scope, char * symbol, Value * val) {
  /* works with var.subvar notation */
  char * split = strchr(symbol,'.');
  if(split) {
    char * buffer = strclone(symbol);
    buffer[split-symbol] = '\0';
    Value * target = Scope_get(scope,buffer);
    Type_call_setter(target,split+1,val);
  }
  Scope_set(scope,symbol,val);
}


/*     Vars      */

Var * Var_new(char * symbol, Value * value){
  Var * var = new(Var);
  var->symbol = strclone(symbol);
  var->value = value;
  return var;
}

void Var_destroy(Var * var){
  free(var->symbol);
  Value_unref(var->value);
  free(var);
}
