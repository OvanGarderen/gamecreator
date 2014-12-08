#include <stdlib.h>

#include "shared.h"
#include "misc.h"

#include "scope.h"
#include "list.h"
#include "eval.h"
#include "value.h"

struct GDScript {
  char * file;
  Scope * locals;
  List * body;
};

GDScript * GDScript_new(char * file, Scope * globals) {
  GDScript * s = new(GDScript);
  s->file = strclone(file);
  s->locals = Scope_new(globals);  
  return s;
}

void GDScript_load(GDScript * s) {
  if(s->body)
    List_destroy_value(s->body);
  
  FILE * fp = fopen(s->file,"r");
  if(fp) {
    s->body = List_read_file(fp);
    fclose(fp);
  }
  else
    s->body = NULL;
}

void GDScript_destroy(GDScript * s) {
  free(s->file);
  Scope_destroy(s->locals);
  List_destroy_value(s->body);
  free(s);
}

Value * GDScript_run(GDScript * s) {
  List * ls = s->body;
  Value * cur = Value_wrap_none();
  while(ls) {
    Value_unref(cur);
    cur = eval(ls->val,s->locals);
    ls = ls->next;
  }
  return cur;
}
