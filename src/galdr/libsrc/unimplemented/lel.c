#include "lib.h"

Value *
lelhandler(Funcdef *func, List * args, Context * context){
  puts("lel");
  return Value_read("NIL");
}
