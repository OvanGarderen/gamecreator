#pragma once

#define arraylen(A) (sizeof(A)/sizeof(A[0]))
#define err(A,...) fprintf(stderr,A "\n", ##__VA_ARGS__)
#define new(A) calloc(1,sizeof(A))
#define new_array(A,size) calloc(size,sizeof(A))
#define object(A) typedef struct A A

#define ARRAY_DECL(TYPE,NAME,...) TYPE _##NAME[] = {			\
    __VA_ARGS__								\
  };									\
  TYPE *NAME = _##NAME;							\
  size_t NAME##_size = arraylen(_##NAME)				\

// these are simple macro's _not_ protected against multiexpansion
#define _MAX(A,B) (((A)>(B))?(A):(B))
#define _MIN(A,B) (((A)<(B))?(A):(B))

#define DIRNONE 0x00

#define NORTH 0x02
#define EAST 0x20
#define SOUTH 0x01
#define WEST 0x10

#define NEAST 0x22 // NORTH | EAST
#define SEAST 0x21 // SOUTH | EAST
#define NWEST 0x12 // NORTH | WEST
#define SWEST 0x11 // SOUTH | WEST

#define HORMASK 0xF0
#define VERMASK 0x0F

typedef unsigned int uint;

#define stralloc(stringinit) malloc(strlen(stringinit)+1)
