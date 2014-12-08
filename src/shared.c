#include <string.h>
#include <ctype.h>

#include "shared.h"
#include "misc.h"

int x_plus_dir(int x, char dir, int mult){
switch(dir){
 case EAST: return x + mult;
 case WEST: return x - mult;
 default: return x;
}
}
int y_plus_dir(int y, char dir, int mult){
switch(dir){
 case SOUTH: return y + mult;
 case NORTH: return y - mult;
 default: return y;
}
}

char* strappend(char* str, char* append) {
return strcat( strgrow(str,strlen(append)+1), append);
}

char* strgrow(char* str, int grow) {
return realloc(str,strlen(str) + 1 + grow);
}

char* strclone(char* str) {
if(str)
  return strcpy( stralloc(str), str);
 else
   return NULL;
}

bool isinstring(char c, char* string) {
for(char *k=string; *k; k++){
if(*k == c)
  return true;
}
return false;
}

bool isallupper(char * string) {
while(*string){
if(!isupper(*string))
  return false;
string++;
}
return true;
}

int strcmp_nocase(char * str1, char * str2) {
while(*str1 && *str2){
if(toupper(*str1) - toupper(*str2))
  return *str1 - *str2;
str1++;
str2++;
}
if(*str1)
  return *str1;
if(*str2)
  return -*str2;
return 0;
}
