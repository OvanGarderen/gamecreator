#include "lib.h"
#include "../type.h"
#include "../eval.h"
#include "../strtools.h"

Value *
returnhandler(Funcdef *func, List *args, Context *context) {
  return Value_cpy(args->value);
}

Value *
catchhandler(Funcdef *func, List *args, Context *context) {
  Value * ErrorType = eval(args->value,context);
  Value * ErrorVar  = args->next->next->value;
  Value * Body      = args->next->next->next->next->value;
  Value * Fail      = args->next->next->next->next->next->next->value;
  
  Value * result = eval(Body,context);
  
  if(result->type == verror && Error_equal(result->get,ErrorType->get)){
      // do fail case
      Context * newcontext = Context_subcontext(context);
      Context_add(newcontext,ErrorVar->get,result);
      Value_destroy(result);
      result = eval(Fail,newcontext);
      Context_destroy(newcontext);
  }

  Value_destroy(ErrorType);
  
  return result;
}

Value *
deletehandler(Funcdef *func, List *args, Context *context) {
  Value * name = args->value;

  Context_remove_upwards(context,name->get);
  return Value_read("NIL");
}

Value *
typeofhandler(Funcdef *func, List *args, Context *context) {
  return Value_wrap_type(args->value->type);
}

Value *
typehandler(Funcdef *func, List *args, Context *context) {
  Value *name = args->value; //::symbol
  List *body = args->next->value->get; //::list

  Extype * tp = Extype_make(name->get);

  char *buffer = strclone(":");
  buffer = strappend(buffer,name->get);
  Value *result = Value_wrap_extype(tp);
  Context_add_upwards(context,buffer,result);
  free(buffer);
 
  List *nargs = List_make(NULL,NULL);
  List *start = nargs;
  while(body){
    Value * val;
    val = eval(body->value,context);
    if(val->type != vtype) {
      Value_destroy(val);
      List_destroy(nargs);
      return Value_Error(eargtype,NULL,"Body description of %s does not evaluate to a list of types.",name->get);
    }
    nargs->next = List_make(val,NULL);
    nargs = nargs->next;
    body = body->next;
  }
  nargs = start->next;
  free(start);

  Extype_addfields(tp, List_len(nargs), nargs);
  List_destroy(nargs);

  buffer = strappend(strappend(strappend(strappend(strclone("(def "),
						   name->get),
					 " (@rem) (pack :"),
			       name->get),
		     " @rem))");
  Value *make = Value_wrap_List(List_read(buffer));
  if(make->type != verror) {
    List * ls = make->get;
    Value_destroy(eval(ls->value,context));
  }
  Value_destroy(make);
  return result;
}

Value *
typepackhandler(Funcdef *func, List *args, Context *context) {
  Extype * tp = args->value->get;
  List *ls = args->next->value->get;

  if(!tp->name)
    return Value_Error(eargtype,args->value,"trying to pack a builtin type");

  int i=0;
  for(List *cur = ls; cur && i<tp->numfields; cur=cur->next){
    if(!Value_match_extype(cur->value,tp->fields[i]) && 
       !Extype_equal(cur->value->extype,tp->fields[i]) && 
       cur->value->type != vnull)
      return Value_Error(eargtype,cur->value,
			 "Trying to pack value of type %s as %ith entry into %s, expecting %s",
			 Value_type_literal(cur->value),
			 i,
			 tp->name,
			 Extype_literal(tp->fields[i]));
    i++;
  }

  if(i!= tp->numfields){
    Extype_print(tp);
    List_print_flat(args);
    puts("");
    puts("oh noes");
    return Value_read("NIL");
  }

  Value * result = Value_read("NIL");
  result->type = vextended;
  result->extype = Extype_cpy(tp);
  result->get =  Extype_implement(tp,ls);
  return result;
}

Value *
typegethandler(Funcdef *func, List *args, Context *context) {
  Value * val = args->value;
  int index = *((int*)args->next->value->get);
  
  Extype *tp = val->extype;
  if(!tp || !tp->name)
    return Value_Error(eargtype,val,"Trying to unpack a builtin value");
  
  if(index < 0 || index >= tp->numfields)
    return Value_Error(eargtype,val,"Index %i falls outside of acceptable range",index);

  return Value_cpy(((Value**)val->get)[index]);
}

Value *
suspendhandler(Funcdef *func, List *args, Context *context){
  Value *lel = Value_cpy(args->value);
  lel->suspend = *((int*)lel->get);
  return lel;
}

Value *
suspendwraphandler(Funcdef *func, List *args, Context *context){
  Value *lel = Value_cpy(args->value);
  lel->suspend = -1;
  return lel;
}

Value *
ptphandler(Funcdef *func, List *args, Context *context){
  Value * ptr = args->value;
  return Value_wrap_pointer(*((void**) ptr->get));
}

Value *
ptihandler(Funcdef *func, List *args, Context *context){
  Value * ptr = args->value;
  return Value_wrap_int(*((int*) ptr->get));
}

Value *
ptfhandler(Funcdef *func, List *args, Context *context){
  Value * ptr = args->value;
  return Value_wrap_float(*((float*) ptr->get));
}

Value *
ptchandler(Funcdef *func, List *args, Context *context){
  Value * ptr = args->value;
  return Value_wrap_character(*((char*) ptr->get));
}

Value *
ptshandler(Funcdef *func, List *args, Context *context){
  Value * ptr = args->value;
  char * buffer = strclone(*((char**) ptr->get));
  return Value_wrap_string(buffer);
}
