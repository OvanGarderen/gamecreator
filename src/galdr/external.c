#include <dlfcn.h>
#include <string.h>

#include "shared.h"
#include "misc.h"

#include "external.h"
#include "value.h"
#include "scope.h"
#include "funcdef.h"
#include "error.h"
#include "list.h"
#include "type_internal.h"

Value * external_symbol_load(Scope * context,
			     void * handle,
                             char * handler,
                             char * funcname,
                             List * args,
                             bool ismacro
                             ) {
  void * dlfunc = dlsym(handle,handler);
  if(!dlfunc)
    return Value_wrap_error(context,eIO,"Can't find symbol %s",handler);

  Funcdef * new = Funcdef_new(funcname, args, NULL);
  new->ismacro = ismacro;
  new->caller = dlfunc;
  /* types */

  return Value_wrap_funcdef(new);;
}

/*
  Loading an external library may return an error if the
  file does not exist.
  Otherwise some symbols may fail to load and will lead
  have errors as values.
 */
Value * external_library_load(char * libdir,
			      char * bindingsfile) {
  Scope * lib = Scope_new(NULL);
  int line = 0;

  Scope_quickadd(lib,"__lib-dir",Value_wrap_string(libdir));
  Scope_quickadd(lib,"__name",Value_wrap_string(bindingsfile));
  Scope_quickadd(lib,"__file",Value_wrap_string(bindingsfile));
  Scope_quickadd(lib,"__line",Value_point_int(&line));

  FILE * fp = fopen(bindingsfile,"r");
  if(fp) {
    List * specs = List_read_file(fp);
    fclose(fp);
    
    List * start = specs;
    for(; specs; specs=specs->next) {
      line++;

      if(((Value*)specs->val)->type != TypeList)
	continue;
      
      List * curspec = ((Value*)specs->val)->get;
      if(!curspec)
	continue;

      Value * curfname = curspec->val;
      if(curfname->type != TypeSymbol)
	continue;
      
      if(List_len(curspec) < 6){
        Scope_quickadd(lib, curfname->get,
                       Value_wrap_error(lib,eIO,"Invalid specifier."));
        continue;
      }

      Value * file     = curspec->next->val;
      Value * caller   = curspec->next->next->val;
      Value * args     = curspec->next->next->next->val;
      Value * types    = curspec->next->next->next->next->val;
      Value * ismacro  = curspec->next->next->next->next->next->val;

      if(   (file->type != TypeString && file->type != TypeSymbol)
            || (caller->type != TypeString && caller->type != TypeSymbol)
            || args->type != TypeList
            || types->type != TypeList
            || ismacro->type != TypeInt){
        Scope_quickadd(lib,
                       curfname->get,
                       Value_wrap_error(lib,eArgType,"Invalid specifier"));
        continue;
      }

      char * buffer = strappend(strappend(strappend(strclone(libdir),"/"),file->get),".so");
      void *handle = dlopen(buffer,RTLD_LAZY);
      if(!handle) {
	Value * e = Value_wrap_error(lib,eIO,"Can't open library %s",buffer);
	free(buffer);
	return e;
      }
      free(buffer);
      
      Value * newfunc = external_symbol_load(lib,
					     handle,
					     caller->get,
                                             curfname->get,
                                             args->get,
                                             *((int*) ismacro->get));
      Scope_add(lib,curfname->get,newfunc);
    }
    List_destroy_value(start);
  } else {
    /* note that the error depends on lib */
    Value * e = Value_wrap_error(lib,eIO,"Failed to open file file %s.",bindingsfile);
    Scope_destroy(lib);
    return e;
  }

  return Value_wrap_scope(lib);
}

/* loads the external library and loads all relevant symbols in the Scope pointed to by `scope' */
Value * external_library_load_bind(Scope * scope, char * libdir, char * bindingsfile) {
  Value * library = external_library_load(libdir,bindingsfile);
  if(library->type == TypeScope) {
    Scope * lib = library->get;
    List * vars = lib->vars;
    while(vars) {
      Var * var = vars->val;
      if(strncmp("__",var->symbol,2)) {
	Scope_add(scope,var->symbol,Value_ref(var->value));
      }
      vars = vars->next;
    } 
  }
  return library;
}
