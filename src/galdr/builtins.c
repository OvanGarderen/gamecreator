#include <dlfcn.h>
#include <string.h>
#include <stdio.h>

#include "shared.h"
#include "misc.h"

#include "builtins.h"
#include "funcdef.h"
#include "value.h"
#include "list.h"
#include "scope.h"
#include "eval.h"
#include "type_internal.h"
#include "error.h"

char * Fdef_args[] = {"name","args","@body",NULL};
Funcdef_proto Fdef = {"def", true, Fdef_args, defhandler};

char * Fset_args[] = {"name","val",NULL};
Funcdef_proto Fset = {"set", true, Fset_args, sethandler};

char * Flit_args[] = {"literal",NULL};
Funcdef_proto Flit = {"lit", true, Flit_args, lithandler};

char * Fif_args[] = {"cond","@THEN","first","@ELSE","second",NULL};
Funcdef_proto Fif = {"if", true, Fif_args, ifhandler};

char * Fimport_args[] = {"name",NULL};
Funcdef_proto Fimport = {"import", true, Fimport_args, importhandler};

char * Fextern_args[] = {"name","@IN","file",NULL};
Funcdef_proto Fextern = {"extern", true, Fextern_args, externhandler};


/* extend this from sane_latex */

Funcdef_proto *builtins[] = {
  &Fdef, &Fset, &Fif, &Flit, &Fimport, &Fextern, NULL
};

Funcdef * Funcdef_from_proto(Funcdef_proto * proto) {
  List * args = NULL;
  char **proto_args = proto->args;
  while(*proto_args) {
    args = List_prepend(args,Value_wrap_symbol(*proto_args));
    proto_args++;
  }
  args = List_reverse(args);
  
  Funcdef * fdef = Funcdef_new(proto->name,args,NULL);
  fdef->ismacro = 1;
  fdef->caller = proto->caller;
  
  List_destroy_value(args);
  return fdef;
}
  
void Scope_add_builtins(Scope * scope) {
  for(Funcdef_proto ** cur = builtins;
      *cur;
      cur++)
    Scope_add(scope, (*cur)->name,
	      Value_wrap_funcdef(Funcdef_from_proto(*cur)));
}

/* implementation */
Value * defhandler(Scope *call){
  Value * name  = Scope_get(call,"name");
  Value * fargs = Scope_get(call,"args");
  Value * body  = Scope_get(call,"body");

  /* Closures are an advanced feature which may be added later */
  /* Context *closure = Context_subcontext(context);
  for(List *cur = fargs->get; cur; cur = cur->next){
    if(cur->value->type != TypeSymbol){
      Context_destroy(closure);
      return Value_Error(eargtype,fargs,"argument should be a name (symbol)");
    }
    Context_add(closure,cur->value->get,cur->value);
  }
  List *closedbody = Context_replace(body->get,closure);
  */
  
  Funcdef * f = Funcdef_new(name->get,fargs->get,body->get);
  Value * wrapper = Value_wrap_funcdef(f);
  
  Scope_add_global(call,name->get,wrapper);
  return Value_ref(wrapper);
}

Value * sethandler(Scope * call){
  Value *name = Scope_get(call,"name");
  Value *val  = Scope_get(call,"val");

  Value * ev = eval(val,call);
  Scope_resolve_set(call,name->get,ev);

  return Value_ref(ev); /* note the second copy */
}

Value * lithandler (Scope * call) {
  Value * literal = Scope_get(call,"literal");
  return Value_ref(literal);
}

Value * ifhandler(Scope * call){
  Value * condition = Scope_get(call,"cond");
  Value * first     = Scope_get(call,"first");
  Value * second    = Scope_get(call,"second");
  
  if(first->type == TypeError ||
     second->type == TypeError) {
    return Value_wrap_error(call,eType,"If-then-else pattern not satisfied.");
  }
  
  Value * truth = eval(condition,call);

  if(truth->type != TypeInt) {
    return Value_wrap_error(call,eType,"Conditional expression expects integer.");
  }

  int boolean = *((int*)truth->get);
  Value_unref(truth);

  /* here is something potentially unstable */
  if(boolean){
    return eval(first,call->parent);
  } else {
    return eval(second,call->parent);
  }
}

Value * importhandler(Scope * call) {
  Value * name = Scope_get(call,"name");

  char *buffer = strclone(name->get);
  strappend(buffer,".gdr");
  printf("importing %s\n",buffer);
  fflush(stdout);

  FILE *fp = fopen(buffer,"r");
  free(buffer);

  if(!fp)   
    return Value_wrap_error(call,eIO,"could not open file %s.",buffer);

  List * ls = List_read_file(fp);
  fclose(fp);

  if(!ls)
    return Value_wrap_error(call,eIO,"Error while parsing file %s.",buffer);

  List * cpy = List_map(ls,eval,call);
  List_destroy_value(ls);
  List_destroy_value(cpy);
  
  /* Error checking not implemented yet */
  /* Value * err = List_check_Errors(cpy); */
  /* if(err){ */
  /*   Value * erret = Value_Error(eio,err,"returns an error"); */
  /*   List_destroy(cpy); */
  /*   return erret; */
  /* } */

  return Value_wrap_none();
}

#include "external.h"

Value * externhandler(Scope * call) {
  Value * fname = Scope_get(call,"name");
  Value * file  = Scope_get(call,"file");

  FILE * fp = NULL;
  if(file->type == TypeSymbol) {
    char * buffer = NULL;
    Value *extern_dir = Scope_get(call,"__extern-dir");
    if(!extern_dir || extern_dir->type != TypeString)
      return Value_wrap_error(call,eIO,"No directory given for external file lookup.");  
    buffer = strclone(extern_dir->get);
    buffer = strappend(buffer,"/");
    buffer = strappend(buffer,file->get);
    buffer = strappend(buffer,".gdlib");
    
    printf("linking %s\n",buffer);
    fp = fopen(buffer,"r");
    if(!fp) {
      Value * e = Value_wrap_error(call,eIO,"No such file %s.",buffer);
      free(buffer);
      return e;
    }
    free(buffer);
  }
  else {
    printf("linking %s\n",(char*)file->get);
    fp = fopen(file->get,"r");
    if(!fp)
      return Value_wrap_error(call,eIO,"No such file %s.",(char*)file->get);
  }

  List * ls = List_read_file(fp);
  fclose(fp);

  Value * newfunc = NULL;

  List * start = ls;
  while(ls) {
    Value * curlist = ls->val;

    List * cls = curlist->get;
    Value * tname = cls->val;

    if(!strcmp(tname->get,fname->get) || !strcmp(fname->get,"all")){
      Value * filefrom    = cls->next->val;
      Value * nameinfile  = cls->next->next->val;
      Value * argspec     = cls->next->next->next->val;
      Value * typespec    = cls->next->next->next->next->val;
      Value * ismacro     = cls->next->next->next->next->next->val;
       
      if(!(   (filefrom->type == TypeString
	       || filefrom->type == TypeSymbol)
	   && (nameinfile->type == TypeString
	       || nameinfile->type == TypeSymbol)
	   && argspec->type == TypeList
	   && typespec->type == TypeList
	   && ismacro->type == TypeInt)){
	newfunc = Value_wrap_error(call,eArgType,"Wrong argument types expanding pattern for function %s.",(char*)tname->get);
	break;
      }

      newfunc = external_symbol_load(call,
				     filefrom->get,
				     nameinfile->get,
				     tname->get,
				     argspec->get,
				     ismacro->get
				     );
      printf("Added %s (%s)\n",
	     (char*)tname->get,
	     (char*)nameinfile->get);
      
      if(strcmp(fname->get,"all"))
	break;
    }
    
    ls = ls->next;
  }
  List_destroy_value(start);

  return Value_ref(newfunc);
}
