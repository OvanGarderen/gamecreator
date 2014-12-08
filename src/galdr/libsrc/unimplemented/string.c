#include "lib.h"
#include <string.h>
#include <stdio.h>

Value *
strcathandler(Funcdef *func, List *args, Context *context){
  Value *stra = args->value;
  Value *strb = args->next->value;
  Value *res = Value_read("NIL");
  res->type = vstring;
  res->get = malloc(strlen(stra->get) + strlen(strb->get) + 1);
  strcpy(res->get,stra->get);
  strcpy(res->get + strlen(strb->get),strb->get);
  return res;
}

Value *
strgethandler(Funcdef *func, List *args, Context *context){
  Value *stra = args->next->value;
  unsigned int index = *((int*) args->value->get);
  if(strlen(stra->get) <= index)
    return Value_Error(eargtype,stra,"Index to far into string");

  Value *res = Value_read("NIL");
  res->type = vcharacter;
  res->get = malloc(sizeof(char));
  *((int*)res->get) = ((char*) stra->get)[index];
  return res;
}

Value *
strtolisthandler(Funcdef *func, List *args, Context *context){
  Value * str = args->value;
  char * cur = str->get;
  List * start, * ls;
  start = ls = List_make(NULL,NULL);
  while(*cur){
    ls->next = List_make(Value_wrap_character(*cur),NULL);
    ls = ls->next;
    ++cur;
  }
  ls = start->next;
  free(start);
  return Value_wrap_List(ls);
}

Value *
listtostringhandler(Funcdef *func, List *args, Context *context){
  List *ls = args->value->get;
  int len = List_len(ls);
  char * str = malloc(len + 1);
  char * cur = str;
  while(ls){
    if(!ls->value->type == vcharacter){
      free(str);
      return Value_Error(eargtype,Value_wrap_List(ls),
			 "Cannot move non-characters into string");
    }
    *cur = *((char*)ls->value->get);
    ++cur;
    ls = ls->next;
  }
  str[len] = '\0';
  return Value_wrap_string(str);
}
