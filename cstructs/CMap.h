// CMap.h
//
// https://github.com/tylerneylon/cstructs
//
// C-based hash map.
// Lookups are fast, sizing grows as needed.
//

#pragma once

#include "CArray.h"

#include <stdlib.h>

typedef int    ( *Hash  )(void *);
typedef int    ( *Eq    )(void *, void*);
typedef void * ( *Alloc )(size_t);

typedef struct {
  int count;
  CArray buckets;
  Hash hash;
  Eq eq;
  Releaser keyReleaser;
  Releaser valueReleaser;
  Alloc pairAlloc;  // Default is malloc; customize to add fields per key/value.
} CMapStruct;

typedef CMapStruct *CMap;

typedef struct {
  void *key;
  void *value;
} KeyValuePair;


CMap          CMapNew    (Hash hash, Eq eq);
void          CMapDelete (CMap map);

KeyValuePair *CMapSet    (CMap map, void *key, void *value);
void          CMapUnset  (CMap map, void *key);
KeyValuePair *CMapFind   (CMap map, void *needle);

void          CMapClear  (CMap map);

// This is for use with CMapFor.
KeyValuePair *CMapNext   (CMap map, int *i, void **p);

// The variable var has type KeyValuePair *.
#define CMapFor(var, map) \
  for (int    __tmp_i = -1  ; __tmp_i == -1  ;) \
  for (void * __tmp_p = NULL; __tmp_p == NULL;) \
  for (KeyValuePair *var = CMapNext(map, &__tmp_i, &__tmp_p); \
       var; var = CMapNext(map, &__tmp_i, &__tmp_p))
