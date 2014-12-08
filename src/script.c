#include "typedefs.h"
#include "script.h"
#include "object.h"

#include "galdr/script.h"
#include "galdr/scope.h"
#include "galdr/value.h"
#include "galdr_external/instance_wrap.h"

#include "debug.h"

void script_call_method(Script *s, Instance *self){
GDScript * script = s->script;

Scope_set(script->locals,"self", Value_wrap_instance(self));

Value_unref(GDScript_run(script));
}
