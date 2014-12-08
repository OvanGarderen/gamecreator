#pragma once

#include <stdbool.h>
#include <stdlib.h>
#define _GNU_SOURCE 1
#include <stdio.h>

#include "misc.h"

int x_plus_dir(int x, char dir, int mult);
int y_plus_dir(int y, char dir, int mult);

char* strappend(char* str, char* append);
char* strgrow(char* str, int grow);
char* strclone(char* str);

bool isinstring(char c, char* string);
bool isallupper(char * string);

int strcmp_nocase(char * str1, char * str2);
