#pragma once

#include <stdio.h>
#include "types.h"

struct List {
  void * val;
  List * next;
};

List * List_new(void * v);
void List_destroy(List * l);

List * List_prepend(List *l, void *val);
List * List_append(List *l, void *val);
List * List_unlink(List * l, List *link);
List * List_reverse(List * l);
int List_len(List *l);
List * List_concat(List * lr, List * ll);

void * List_get(List *l, int index);

void List_destroy_value(List * l);
List * List_cpy(List * l);
void List_print(List * l);
void List_exec(List *l, DestructorFunc func);
List * List_map(List *l, ListMapper func, Scope * context);

List * List_read(char * raw);
List * List_read_file(FILE *fp);
