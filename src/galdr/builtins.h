#pragma once

#include "types.h"

struct Funcdef_proto {
  char * name;
  bool macro;
  char **args;
  FuncdefCaller caller;
};

// macro's
Value * defhandler   (Scope *);
Value * sethandler   (Scope *);

Value * lithandler (Scope *);

/* Not implemented yet
Value * funchandler  (Scope *);
Value * macrohandler (Scope *); */
Value * ifhandler    (Scope *); /*
Value * lethandler   (Scope *);
Value * loophandler  (Scope *);
Value * whilehandler (Scope *);
				*/
Value * importhandler(Scope *);
Value * externhandler(Scope *);

Funcdef * Funcdef_from_proto(Funcdef_proto * proto);
void Scope_add_builtins(Scope *);
