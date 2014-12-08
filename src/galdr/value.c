#include <stdarg.h>
#include <stdlib.h>

#include "shared.h"
#include "misc.h"

#include "value.h"
#include "error.h"
#include "type.h"
#include "type_internal.h"

Value * Value_new(void * get, Type * t) {
  Value * v = new(Value);
  v->get = get;
  v->type = t;
  v->ref = 1;
  return v;
}

void Value_destroy(Value * v) {
  Type_call_destroy(v);
  free(v);
}

Value * Value_cpy(Value * v) {
  Value * cpy = Value_new(v->get,v->type);
  Type_call_copy(cpy);
  return cpy;
}

Value * Value_ref(Value * v) {
  v->ref++;
  return v;
}

Value * Value_unref(Value * v) {
  if( --v->ref <= 0) {
    Value_destroy(v);
    return NULL;
  }
  return v;
}

void Value_print(Value * v) {
  Type_call_print(v);
}

/* wrap values */

Value * Value_wrap_int(int i) {
  int * ip = malloc(sizeof(int));
  *ip = i;
  return Value_new(ip,TypeInt);  
}

Value * Value_wrap_float(double f) {
  double * fp = malloc(sizeof(double));
  *fp = f;
  return Value_new(fp,TypeFloat);  
}

Value * Value_wrap_string(char * s) {
  char * sc = strclone(s);
  return Value_new(sc,TypeString);
}

Value * Value_wrap_symbol(char * s) {
  char * sc = strclone(s);
  return Value_new(sc,TypeSymbol);
}

Value * Value_wrap_pointer(void * p) {
  return Value_new(p,TypePointer);
}

Value * Value_wrap_list(List * l) {
  return Value_new(l,TypeList);
}

Value * Value_wrap_funcdef(Funcdef * f) {
  return Value_new(f,TypeFunc);
}

Value * Value_wrap_scope(Scope * s) {
  return Value_new(s,TypeScope);
}

Value * Value_wrap_file(FILE * fp) {
  return Value_new(fp,TypeFile);
}

/* special values */
Value * Value_wrap_error(Scope * context, int code, char * message, ...) {
va_list args;
va_start(args,message);
Error * e = Error_new_fromcontext(context,code,message,args);
  return Value_new(e,TypeError);
}

Value * Value_wrap_type(Type * t) {
  return Value_new(t,TypeType);
}

Value * Value_wrap_none(void) {
  return Value_new(NULL,TypeNone);
}

/* external values */

Value * Value_point_int(int *i) {
  Value * v = Value_new(i,TypeInt);
  v->external = true;
  return v;
}

Value * Value_point_float(double *f) {
  Value * v = Value_new(f,TypeFloat);
  v->external = true;
  return v;
}

Value * Value_point_string(char *s) {
  Value * v = Value_new(s,TypeString);
  v->external = true;
  return v;
}

Value * Value_point_list(List * f) {
  Value * v = Value_new(f,TypeList);
  v->external = true;
  return v;
}

Value * Value_point_funcdef(Funcdef * f) {
  Value * v = Value_new(f,TypeFunc);
  v->external = true;
  return v;
}

Value * Value_point_scope(Scope * s) {
  Value * v = Value_new(s,TypeScope);
  v->external = true;
  return v;
}

#include "list.h"

Value * Value_wrap_literal(Value * v) { /* returns (lit v) */
  List * thelist = List_new(v);
  Value * litsymb = Value_wrap_symbol("lit");
  thelist = List_prepend(thelist,litsymb);
  return Value_wrap_list(thelist);
}

/* this is where parsing gets involved */

#include <string.h>

#include "list.h"

Value * Value_read(char * raw) {
  /* expects a valid nonzero C-string */
  int len = strlen(raw);
  
  char first = raw[0];
  char last = raw[len - 1];

  if(first == '(' && last == ')'){
    /* list detected, parse recursively */
    raw[len-1] = '\0';
    return Value_wrap_list(List_read(raw+1));
  } else if(!strcmp(raw,"NIL") || !strcmp(raw,"nil")) {
    /* NIL value */
    return Value_new(NULL,TypeNone);
  } else if (first == '"' && last == '"') {
    /* string detected */
    char * str = strclone(raw+1);
    str[len-2] = '\0';
    return Value_point_string(str);
  } else if (first == '\'') {
    /* literal detected */
    /* ?????????????? what is a literal ???????????????? */
    if(len == 1)
      return Value_wrap_none(); /* change to error type */
    return Value_wrap_literal(Value_read(raw+1));
  } else if (!strncmp(raw,"0x",2)) {
    /* pointer denoted by hex notation */
    void * ptr = NULL;
    sscanf(raw,"%p",&ptr);
    return Value_wrap_pointer(ptr);
  } else {
    /* distinguish int and float */
    int i;
    float f;
    if(!isinstring('.',raw) &&
       !isinstring('e',raw) &&
       sscanf(raw,"%i",&i)) {
      /* int detected */
      return Value_wrap_int(i);
    } else if(sscanf(raw,"%f",&f)) {
      /* float detected */
      double d = (double) f;
      return Value_wrap_float(d);
    } else {
      /* if no value found, resolve symbol later */
      return Value_wrap_symbol(raw);
    }
  }
}
