#include <stdarg.h>
#include <stdlib.h>

#include "shared.h"
#include "misc.h"

#include "error.h"
#include "value.h"
#include "scope.h"
#include "type_internal.h"

Error * Error_new(int code, char * message, char * filename, int line) {
  Error * e = new(Error);
  e->code = code;
  e->message = strclone(message);
  e->file = strclone(filename);
  e->line = line;
  return e;
}

Error * Error_new_fromcontext(Scope * context, int code, char * message, va_list args) {   
  Value * vline = Scope_get(context,"__line");
  Value * vfile = Scope_get(context,"__file");

  int line;
  char * file;
  
  if(vline->type != TypeInt ||
     vfile->type != TypeString) {
    line = 0;
    file = "<undefined file>";
  } else {
    line = *((int*)vline->get);
    file = vfile->get;
  }

  Error * e = Error_new(code,NULL,file,line);
  vasprintf(&e->message, message, args);
  return e;
}

void Error_print(Error * e) {
  printf("!! Error in %s at line %i: %s",
	 e->file, e->line, e->message);
}
