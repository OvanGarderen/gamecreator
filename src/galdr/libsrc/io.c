#include "lib.h"
#include <string.h>
#include "shared.h"
#include "misc.h"

Value * printhandler(Scope * call) {
  //dealing with variadic function so args[0] is a list
  List *args = Scope_get(call,"args")->get;

  while(args){
    if(args->val){
      if(((Value*)args->val)->type == TypeString)
        puts(((Value*)args->val)->get);
      else{
        Value_print(args->val);
        puts("");
      }
    }
    args = args->next;
  }
  return Value_wrap_none();
}

Value * inputhandler(Scope * call) {
  Value * prompt = Scope_get(call,"prompt");

  printf("%s",(char*)prompt->get);

  char buffer[3000];
  fgets(buffer,3000,stdin);
  buffer[2999] = '\0';
  buffer[strlen(buffer)-1] = '\0';

  return Value_wrap_string(strclone(buffer));
}

Value * fileopenhandler(Scope * call) {
  Value * name = Scope_get(call,"name");
  Value * mode = Scope_get(call,"mode");

  FILE * fp = fopen(name->get,mode->get);
  if(!fp)
    return Value_wrap_error(eIO,"Could not open file",call);
  return Value_wrap_file(fp);
}

Value * getchandler(Scope * call) {
  Value * file = Scope_get(call,"file");

  char stor[2];
  stor[0] = getc(file->get);
  stor[1] = '\0';
  
  if(stor[0] == EOF)
    return Value_wrap_string("\0"); 
  return Value_wrap_string(stor);
}

Value * fileiseofhandler(Scope * call) {
  Value * file = Scope_get(call,"file");

  char c = getc(file->get);
  fseek(file->get,-1,SEEK_CUR);
  if(c == EOF)
    return Value_wrap_int(1);
  return Value_wrap_int(0);
}

Value * getlinehandler(Scope * call) {
  Value * file = Scope_get(call,"file");

  FILE * fp = (FILE*) file->get;

  char *buffer = malloc(1);
  buffer[0] = '\0';
  char *s = buffer;
  do {
    int off = strlen(buffer);
    buffer = strgrow(buffer,100);
    s = buffer + off;
    s = fgets(s,100,fp);
    if(!s) {
      free(buffer);
      return Value_read("NIL");
    }
  }
  while(s && strlen(s) >= 99);

  if(strlen(buffer) &&
     (buffer[strlen(buffer)] != '\0' || buffer[strlen(buffer)-1] == '\n'))
    buffer[strlen(buffer)-1] = '\0';

  return Value_wrap_string(buffer);
}
