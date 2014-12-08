#pragma once

#include "lib.h"
#include "../object.h"

Type * TypeInstance;

void type_instance_print(Value * v);
Value * type_instance_getter(Value * v, char * symbol);
void type_instance_setter(Value * v, char * symbol, Value * val);

Value * Value_wrap_instance(Instance * i);
