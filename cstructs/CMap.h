//  CMap.h
//
//  C-based hash map.
//  Lookups are fast, sizing grows as needed.
//

#ifndef __CMAP_H__
#define __CMAP_H__

#include "CArray.h"

#include <stdlib.h>

typedef int (*Hash)(void *);
typedef int (*Eq)(void *, void*);
typedef void *(*Alloc)(size_t);

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


CMap CMapNew(Hash hash, Eq eq);
void CMapDelete(CMap map);

KeyValuePair *CMapSet(CMap map, void *key, void *value);
void CMapUnset(CMap map, void *key);
KeyValuePair *CMapFind(CMap map, void *needle);

void CMapClear(CMap map);

// These are for use with CMapFor.
KeyValuePair *CMapBegin(CMap map, void **iterator);
KeyValuePair *CMapNext(CMap map, void *iterator);

// These macros are to be able to get a unique token within other macros.
// See http://stackoverflow.com/questions/1597007/
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE TOKENPASTE2(unique, __LINE__)

#define CMapFor(var, map) \
  void *UNIQUE; \
  for (KeyValuePair *var = CMapBegin(map, &UNIQUE); \
       var; var = CMapNext(map, UNIQUE))

#endif

