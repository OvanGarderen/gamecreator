#include <stdlib.h>
#include <stdio.h>

#include "shared.h"
#include "misc.h"

#include "type_internal.h"

#include "value.h"

/* int */
Type _TypeInt = {"int",
		 type_int_destroy,
		 type_int_cpy,
		 type_int_print,NULL,NULL};
Type * TypeInt = &_TypeInt;

void type_int_destroy(Value * v) {
  free(v->get);
}

void type_int_cpy(Value * v) {
  int * ip = new(int);
  *ip = *((int*)v->get);
  v->get = ip;
}

void type_int_print(Value * v) {
  printf("%i",*((int*)v->get));
}

/* float */
Type _TypeFloat =  {"float",
		    type_float_destroy,
		    type_float_cpy,
		    type_float_print,NULL,NULL};
Type * TypeFloat = &_TypeFloat;

void type_float_destroy(Value * v) {
  free(v->get);
}

void type_float_cpy(Value * v) {
  int * fp = new(double);
  *fp = *((double*)v->get);
  v->get = fp;
}

void type_float_print(Value * v) {
  printf("%e",*((double*)v->get));
}

/* string */
Type _TypeString =  {"string",
		     type_string_destroy,
		     type_string_cpy,
		     type_string_print,NULL,NULL};
Type * TypeString = &_TypeString;

void type_string_destroy(Value * v) {
  free(v->get);
}

void type_string_cpy(Value * v) {
  v->get = strclone(v->get);
}

void type_string_print(Value * v) {
  printf("%s",(char*) v->get);
}

/* symbol */
Type _TypeSymbol =  {"symbol",
		     type_symbol_destroy,
		     type_symbol_cpy,
		     type_symbol_print,NULL,NULL};
Type * TypeSymbol = &_TypeSymbol;

void type_symbol_destroy(Value * v) {
  free(v->get);
}

void type_symbol_cpy(Value * v) {
  v->get = strclone(v->get);
}

void type_symbol_print(Value * v) {
  printf(":%s",(char*) v->get);
}

/* pointer */
Type _TypePointer =  {"pointer",NULL,NULL,
			   type_pointer_print,NULL,NULL};
Type * TypePointer = &_TypePointer;

void type_pointer_print(Value * v) {
  printf("%p",v->get);
}


/* list */
#include "list.h"

Type _TypeList =  {"list",
		   type_list_destroy,
		   type_list_cpy,
		   type_list_print,
		   type_list_getter,NULL};
Type * TypeList = &_TypeList;

void type_list_destroy(Value * v) {
  List_destroy_value((List *) v->get);
}

void type_list_cpy(Value * v) {
  v->get = List_cpy(v->get);
}

void type_list_print(Value * v) {
printf("(");
if(v->get)
  List_print((List *) v->get);
printf(")");
}

Value * type_list_getter(Value * v, char * symbol) {
  List * l = v->get;
  Value * symb = Value_read(symbol);
  if(symb->type == TypeInt) {
    Value * ret = List_get(l,*((int*)symb->get));
    if(ret) return ret;
  }
  return Value_wrap_none();
}

Type _TypeExList =  {"expansion-list",
		   type_list_destroy,
		   type_list_cpy,
		   type_list_print,NULL,NULL};
Type * TypeExList = &_TypeExList;

/* funcdef */
#include "funcdef.h"

Type _TypeFunc =  {"func",
			type_func_destroy,NULL,
			type_func_print,NULL,NULL};
Type * TypeFunc = &_TypeFunc;

void type_func_destroy(Value * v) {
  Funcdef_destroy((Funcdef*) v->get);
}

void type_func_print(Value * v) {
  Funcdef * f = v->get;
  if(!f->ismacro)
    printf("function %s (",f->name);
  else
    printf("macro %s (",f->name);
  List_print(((Funcdef*)v->get)->args);
  printf(")");
}

/* scope */
#include "scope.h"

Type _TypeScope =  {"scope",
		    type_scope_destroy,NULL,
		    type_scope_print,
		    type_scope_getter,NULL};
Type * TypeScope = &_TypeScope;

void type_scope_destroy(Value * v) {
  Scope_destroy((Scope*) v->get);
}

void type_scope_print(Value * v) {
  Scope * s = v->get;
  Value * name = Scope_get(s,"__name");
  if(name->type == TypeString)
    printf("Scope %s :\n",(char*) name->get);
  else
    printf("Scope -- ;\n");
  for(List * vars = s->vars; vars; vars = vars->next){
    Var * var = vars->val;
    printf("\t %s : ",var->symbol);
    if(var->value->type != TypeScope)
      Value_print(var->value);
    else
      printf("<scope variable>");
    if(vars->next)
      puts("");
  }
}

Value * type_scope_getter(Value * v, char * symbol) {
  Scope * s = v->get;
  return Scope_resolve(s,symbol);
}


/* file */
Type _TypeFile =  {"file",
			type_file_destroy,NULL,
			type_file_print,NULL,NULL};
Type * TypeFile = &_TypeFile;

void type_file_destroy(Value * v) {
  fclose(v->get);
}

void type_file_print(Value * v) {
  printf("file at %p",v->get);
}

/* special nonetype */
Type _TypeNone =  {"none",NULL,NULL,
			type_none_print,NULL,NULL};
Type * TypeNone = &_TypeNone;

void type_none_print(Value * v) {
  printf("NIL");
}

/* metatype  */
Type _TypeType =  {"type",NULL,NULL,
			type_type_print,NULL,NULL};
Type * TypeType = &_TypeType;

void type_type_print(Value * v) {
  printf("::%s",((Type*) v->get)->name);
}

/* error */
#include "error.h"
Type _TypeError =  {"error",NULL,NULL,
			type_error_print,NULL,NULL};
Type * TypeError = &_TypeError;

void type_error_print(Value * v) {
  Error_print(v->get);
}
