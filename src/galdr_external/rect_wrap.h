#pragma once

#include "lib.h"
#include "../collision.h"

Type * TypeRect;

void type_rect_print(Value * v);
Value * type_rect_resolve(Value * v, char * symbol);

Value * Value_wrap_rect(Rect r);
