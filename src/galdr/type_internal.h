#pragma once

#include "types.h"
#include "type.h"

Type * TypeInt;
void type_int_destroy(Value * v);
void type_int_cpy(Value * v);
void type_int_print(Value * v);

Type * TypeFloat;
void type_float_destroy(Value * v);
void type_float_cpy(Value * v);
void type_float_print(Value * v);

Type * TypeString;
void type_string_destroy(Value * v);
void type_string_cpy(Value * v);
void type_string_print(Value * v);

Type * TypeSymbol;
void type_symbol_destroy(Value * v);
void type_symbol_cpy(Value * v);
void type_symbol_print(Value * v);

Type * TypePointer;
void type_pointer_print(Value * v);

Type * TypeList;
void type_list_destroy(Value * v);
void type_list_cpy(Value * v);
void type_list_print(Value * v);
Value * type_list_getter(Value * v, char * symbol);

Type * TypeExList;

Type * TypeFunc;
void type_func_destroy(Value * v);
void type_func_print(Value * v);

Type * TypeScope;
void type_scope_destroy(Value * v);
void type_scope_print(Value * v);
Value * type_scope_getter(Value * v, char * symbol);

Type * TypeFile;
void type_file_destroy(Value * v);
void type_file_print(Value * v);

Type * TypeNone;
void type_none_print(Value * v);

Type * TypeType;
void type_type_print(Value * v);

Type * TypeError;
void type_error_print(Value * v);
