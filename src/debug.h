#pragma once

#include <stdio.h>

#define debug(MSG,...) fprintf(stderr,MSG "\n", ##__VA_ARGS__)
