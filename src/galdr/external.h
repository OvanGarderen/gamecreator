#include "value.h"

Value * external_symbol_load(Scope * context,
			     void * handle,
			     char * handler,
			     char * funcname,
			     List * args,
			     bool ismacro);

Value * external_library_load(char * libdir,char * bindingsfile);
Value * external_library_load_bind(Scope * scope, char * libdir, char * bindingsfile);
