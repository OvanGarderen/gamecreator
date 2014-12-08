#include "lib.h"
#include <stdio.h>
#include "../type.h"

Value *
functypehandler(Funcdef *func, List *args, Context *context){
  Funcdef * f = args->value->get;
  printf("function %s has args:\n",f->name);
  for(int i=0; i< abs(f->numargs); i++)
    printf("%s has type %s\n",f->args[i],Vtype_literal(f->types[i]));
  return Value_read("NIL");
}
