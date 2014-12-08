#pragma once

typedef struct Value Value;
typedef struct Type Type;

typedef struct List List;
typedef struct Scope Scope;
typedef struct Var Var;

typedef struct Error Error;

typedef struct Funcdef Funcdef;
typedef struct Funcdef_proto Funcdef_proto;

typedef struct Scope Scope;
typedef struct GDScript GDScript;

typedef void (*DestructorFunc)(void *);
typedef Value *(*FuncdefCaller)(Scope *);
typedef Value *(*ListMapper)(Value *, Scope *);
