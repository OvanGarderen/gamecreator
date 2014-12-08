#pragma once

#include "types.h"

enum {eNotFound, eIllegal, eType, eIO, eArgType, eNulDiv} ErrorCodes;

struct Error {
  int code;
  char * message;
  int line;
  char * file;
};

Error * Error_new(int code, char * message, char * filename, int line);
Error * Error_new_fromcontext(Scope * context, int code, char * message, va_list args);
void Error_print(Error * e);
