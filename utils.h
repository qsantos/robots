#ifndef UTILS_H
#define UTILS_H

#include <assert.h>
#include <stdlib.h>

/* TYPES */

static const char false = 0;
static const char true  = 1;
typedef char bool;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

typedef signed char    s8;
typedef signed short   s16;
typedef signed long    s32;

typedef char*          string;

/* MEMORY MANAGEMENT */

static inline void* my_assert(void* ptr)
{
  assert(ptr);
  return ptr;
}

#define ALLOC(t,n) (t*) my_assert(malloc(sizeof(t) * n));

#endif
