// CArray.c
//
// Home repo: https://github.com/tylerneylon/cstructs
//

#include "CArray.h"

#ifdef DEBUG
#include "memprofile.h"
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>


CArray CArrayInit(CArray cArray, int capacity, size_t elementSize) {
  cArray->count = 0;
  cArray->capacity = capacity;
  cArray->elementSize = elementSize;
  cArray->releaser = NULL;
  if (capacity) {
    cArray->elements = malloc((int)elementSize * capacity);
  } else {
    cArray->elements = NULL;
  }
  return cArray;
}

CArray CArrayNew(int capacity, size_t elementSize) {
  CArray cArray = malloc(sizeof(CArrayStruct));
  return CArrayInit(cArray, capacity, elementSize);
}

void CArrayDelete(CArray cArray) {
  CArrayRelease(cArray);
  free(cArray);
}

void CArrayRelease(void *cArray) {
  CArray realArray = (CArray)cArray;
  CArrayClear(realArray);
  free(realArray->elements);
  realArray->capacity = 0;
}

void CArrayAddElementByPointer(CArray cArray, void *element) {
  memcpy(CArrayNewElement(cArray), element, cArray->elementSize);
}

void *CArrayNewElement(CArray cArray) {
  if (cArray->count == cArray->capacity) {
    cArray->capacity *= 2;
    if (cArray->capacity == 0) cArray->capacity = 1;
    cArray->elements = realloc(cArray->elements, cArray->capacity * (int)cArray->elementSize);
  }
  cArray->count++;
  return CArrayElement(cArray, cArray->count - 1);
}

int CArrayIndexOf(CArray cArray, void *element) {
  ptrdiff_t byte_dist = (char *)element - cArray->elements;
  return (int)(byte_dist / cArray->elementSize);
}

void CArrayClear(CArray cArray) {
  if (cArray->releaser) {
    for (int i = 0; i < cArray->count; ++i) {
      cArray->releaser(CArrayElement(cArray, i));
    }
  }
  cArray->count = 0;
}

void *CArrayElement(CArray cArray, int index) {
  return (void *)(cArray->elements + index * cArray->elementSize);
}

void CArrayAppendContents(CArray cArray, CArray source) {
  // We avoid using CArrayFor since we don't know which type of pointer to use.
  for (int i = 0; i < source->count; ++i) {
    void *elt = CArrayElement(source, i);
    CArrayAddElementByPointer(cArray, elt);
  }
}

void CArrayRemoveElement(CArray cArray, void *element) {
  if (cArray->releaser) cArray->releaser(element);
  int numLeft = --(cArray->count);
  char *eltByte = (char *)element;
  ptrdiff_t byteDist = eltByte - cArray->elements;
  int index = (int)(byteDist / cArray->elementSize);
  if (index == numLeft) return;
  memmove(eltByte, eltByte + cArray->elementSize, (numLeft - index) * cArray->elementSize);
}

void CArrayAddZeroedElements(CArray cArray, int numElements) {
  int newCount = cArray->count + numElements;
  int resizeNeeded = 0;
  while (cArray->capacity < newCount) {
    cArray->capacity *= 2;
    if (cArray->capacity == 0) cArray->capacity = 1;
    resizeNeeded = 1;
  }
  if (resizeNeeded) {
    cArray->elements = realloc(cArray->elements, cArray->capacity * (int)cArray->elementSize);
  }
  void *bytesToZero = CArrayElement(cArray, cArray->count);
  memset(bytesToZero, 0, numElements * cArray->elementSize);
  cArray->count = newCount;
}

void *CArrayEnd(CArray cArray) {
  return CArrayElement(cArray, cArray->count);
}

int CompareAsInts(void *eltSize, const void *elt1, const void *elt2) {
  size_t s = *(size_t *)eltSize;
  unsigned char *e1 = (unsigned char *)elt1;
  unsigned char *e2 = (unsigned char *)elt2;
  for (size_t i = 0; i < s; ++i) {
    int diff = *(e1 + i) - *(e2 + i);
    if (diff != 0) return diff;
  }
  return 0;
}

typedef int (*CompareFn)(void *context, const void *elt1, const void *elt2);

static CompareFn userCompare;
static void *userContext;

int CustomCompare(const void *elt1, const void *elt2) {
  return userCompare(userContext, elt1, elt2);
}

void CArraySort(CArray cArray, CompareFunction compare, void *compareContext) {
  CompareFn oldCompare = userCompare;
  void *oldContext = userContext;

  if (compare == NULL) {
    userCompare = CompareAsInts;
    userContext = &(cArray->elementSize);
  } else {
    userCompare = compare;
    userContext = compareContext;
  }
  qsort(cArray->elements, cArray->count, cArray->elementSize, CustomCompare);

  userCompare = oldCompare;
  userContext = oldContext;
}

static size_t eltSize;

int CompareForFind(const void *a, const void *b) {
  return memcmp(a, b, eltSize);
}

void *CArrayFind(CArray cArray, void *elt) {
  size_t oldSize = eltSize;
  eltSize = cArray->elementSize;
  void *foundVal = bsearch(elt, cArray->elements, cArray->count, cArray->elementSize, CompareForFind);
  eltSize = oldSize;
  return foundVal;
}
