#pragma once

#include "typedefs.h"
#include "galdr/types.h"

struct Script {
  int type;
  GDScript * script; // simply wraps a galdr script
};

void script_call_method(Script *s, Instance *self);

