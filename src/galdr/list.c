#include <stdio.h>
#include <ctype.h>

#include "shared.h"
#include "misc.h"

#include "list.h"
#include "value.h"

List * List_new(void * v) {
  List * l = new(List);
  l->val = v;
  return l;
}

void List_destroy(List * l) {
  if(l) {
    List * next = l->next;
    free(l);
    if(next)
      List_destroy(next);
  }
}

/* some auxilary list actions */
List * List_prepend(List *l, void *val) {
  List * nl = List_new(val);
  nl->next = l;
  return nl;
}

List * List_append(List *l, void *val) {
  if(l) {
    l->next = List_append(l->next,val);
    return l;
  }
  return List_new(val);
}

void * List_get(List *l, int index) {
  for(; l && index; index--)
    l = l->next;
  if(l)
    return l->val;
  return NULL;
}


List * List_unlink(List * l, List *link) {
  if(l == link){
    List * next = l->next;
    l->next = NULL;
    return next;
  }
  List_unlink(l->next,link);
  return l;
}

List * List_reverse(List * l) {
  List * mod = NULL;
  List * head;
  while(l) {
    head = mod;
    mod = l;
    l = l->next;
    mod->next = head;
  }
  return mod;
}

void List_exec(List *l, DestructorFunc func) {
  while(l) {
    func(l->val);
    l = l->next;
  }
}

int List_len(List *l) {
  int len = 0;
  for(; l; l=l->next)
    len++;
  return len;
}

List * List_concat(List * lr, List * ll) {
  if(!lr)
    return ll;

  List * lp = lr;
  while(lp->next)
    lp = lp->next;
  lp->next = ll;
  return lr;
}

/* from here on the List is expected to contain Value pointers */
void List_destroy_value(List * l) {
  List_exec(l, (DestructorFunc) Value_unref);
  List_destroy(l);
}


List * List_cpy(List * l) {
  List * newl = NULL;
  while(l) {
    newl = List_prepend(newl,Value_cpy(l->val));
    l = l->next;
  }
  return List_reverse(newl);
}

List * List_map(List *l, ListMapper func, Scope *context) {
  List * newl = NULL;
  while(l) {
    newl = List_prepend(newl,func(l->val,context));
    l = l->next;
  }
  return List_reverse(newl);
}

void List_print(List * l) {
  if(!l){
    printf("()");
    return;
  }
  Value_print((Value *) l->val);
  if(l->next) {
    printf(" ");
    List_print(l->next);
  }
}

List * List_read(char * raw) {
  List * cur = NULL;

  char buffer[1000] = "";
  char *bufferp = buffer;
  
  bool splittan = false;
  bool quote = false;
  bool escape = 0;
  int braces = 0;
  
  for(char *c=raw; 1; c++){
    //printf("%c  : ()%i,\"%i,\\%i,' '%i\n",*c,braces,quote,escape,splittan);
    if ((isspace(*c) || *c == '\0') &&
	splittan &&
        !braces &&
	!quote &&
	!escape) {
      //printf("-----------   %s\n",buffer);
      cur = List_prepend(cur, Value_read(buffer));
      splittan = false;
      if (*c == '\0'){
        break;
      }
    }
    else if (*c == '\0'){
      break;
    }
    else if (!isspace(*c)
	     || quote
	     || braces
	     || escape){
      if (!splittan) {
        splittan = true;
        bufferp = buffer;
      }

      if(escape){
        switch(*c){
        case 't': *c = '\t'; break;
        case 'n': *c = '\n'; break;
        case 'v': *c = '\v'; break;
        case 'f': *c = '\f'; break;
        case 'r': *c = '\r'; break;
        case 'b': *c = '\b'; break;
        }
        escape = false;

        *bufferp = *c;
        bufferp++;
        *bufferp = '\0';
      }
      else if(*c == '\\'){
        escape = true;
      }
      else {

        if(!quote){
          if(*c == '(')
            braces++;
          if(*c == ')'){
            braces--;
          }
        }
        if(*c == '"') 
          quote = !quote;
	
        *bufferp = *c;
        bufferp++;
        *bufferp = '\0';
      }
    }
  }
  
  /* in case of error gracefully exit */
  if( braces != 0
      || quote != 0
      || escape != 0
      || splittan != 0){
    List_destroy(cur);
    return NULL;
  }
  
  /* chop off empty head*/
  cur = List_reverse(cur);
  
  return cur;
}

List * List_read_file(FILE *fp){
  char buffer[10000] = "";
  char *bufferp = buffer;

  List *cur = NULL;

  int splittan = 0;
  int braces = 0;
  int quote = 0;
  int escape = 0;
  int comment = 0;

  for(char c=fgetc(fp); 1; c=fgetc(fp)){
    if(c == '\n' && comment)
      comment = 0;
    else if(!quote && c == ';')
      comment = 1;

    if(comment){
      //do nothing
    }
    else if ((isspace(c) || c == '\0' || c == EOF) && splittan &&
        !braces && !quote && !escape) {
      
      //printf("-----------   %s\n",buffer);
      cur = List_prepend(cur,Value_read(buffer));
      splittan = 0;
      if (c == '\0' || c == EOF){
        break;
      }
    }
    else if (c == '\0' || c == EOF){
      break;
    }
    else if (!isspace(c) || quote || braces || escape){
      if (!splittan) {
        splittan = 1;
        bufferp = buffer;
      }

      if(escape){
        switch(c){
        case 't': c = '\t'; break;
        case 'n': c = '\n'; break;
        case 'v': c = '\v'; break;
        case 'f': c = '\f'; break;
        case 'r': c = '\r'; break;
        case 'b': c = '\b'; break;
        }
        escape = 0;

        *bufferp = c;
        bufferp++;
        *bufferp = '\0';
      }
      else if(c == '\\'){
        escape = 1;
      }
      else {

        if(!quote){
          if(c == '(')
            braces++;
          if(c == ')'){
            braces--;
          }
        }
        if(c == '"')
          quote = !quote;

        *bufferp = c;
        bufferp++;
        *bufferp = '\0';
      }
    }
  }

  if( braces != 0 
      || quote != 0
      || escape != 0
      || splittan != 0){
    List_destroy(cur);
    printf("%i : %i : %i ; %i",braces,quote,escape,splittan);
    return NULL;
  }
  cur = List_reverse(cur);
  return cur;
}
