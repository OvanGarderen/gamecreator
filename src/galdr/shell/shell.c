#include <stdio.h>
#include <string.h>

#include "value.h"
#include "list.h"
#include "scope.h"
#include "type_internal.h"
#include "eval.h"
#include "funcdef.h"
#include "builtins.h"
#include "external.h"

Value * addcaller(Scope * context);

int main() {
  char buffer[3000] = "";

  int line = 0;

  Scope * global = Scope_new(NULL);
  Scope_add(global,"kek",Value_wrap_int(666));
  Scope_add(global,"__line",Value_point_int(&line));
  Scope_add(global,"__file",Value_point_string("galdr-shell"));
  Scope_add(global,"__lib-dir",
	    Value_point_string("/home/okke/projects/gamecreator/libs"));
  Scope_add(global,"__extern-dir",
	    Value_point_string("/home/okke/projects/gamecreator/galdr_bind"));

  Scope_add_builtins(global);
  Scope_add(global,"global",Value_point_scope(global));
  
  printf("Loading IO library\n");
  Value* iolib = external_library_load(
       "/home/okke/projects/gamecreator/libs",
       "/home/okke/projects/gamecreator/galdr_bind/io.gdlib");
  Scope_add(global,"io",iolib);
  printf("Loading lists library\n");
  Value* listslib = external_library_load(
       "/home/okke/projects/gamecreator/libs",
       "/home/okke/projects/gamecreator/galdr_bind/lists.gdlib");
  Scope_add(global,"lists",listslib);

  
  puts("--- Völva: a Galdr shell ---");
  List * res;
  do {
    /* shell */
    fflush(stdout);
    line++;
    
    printf(">>");   
    buffer[0] = '\0';
    fgets(buffer,3000,stdin);

    /* read */
    res = List_read(buffer);

    /* eval */
    List * ret = List_map(res,eval,global);
    List_destroy_value(res);
    
    /* print */
    List * cur = ret;
    while(cur) {
      Value_print(cur->val);
      if(cur->next)
	puts("");
      cur = cur->next;
    }
    puts("");

    List_destroy_value(ret);
  } while(strcmp(buffer,"quit\n") && strcmp(buffer,""));

  Scope_destroy(global);
  return 0;
}

Value * addcaller(Scope * call) {
  int * a = Scope_get(call,"a")->get;
  int * b = Scope_get(call,"b")->get;
  return Value_wrap_int(*a + *b);
}
