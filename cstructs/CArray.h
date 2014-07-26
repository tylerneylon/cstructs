// CArray.h
//
// https://github.com/tylerneylon/cstructs
//
// A C structure for flexibly working with a sequence
// of elements that are kept contiguously in memory.
// The array length is flexible, and is designed to
// support nesting of data structures.
//

#ifndef __CARRAY_H__
#define __CARRAY_H__

#include <stdlib.h>

typedef void (*Releaser)(void *element);

typedef struct {
  int count;
  int capacity;
  size_t elementSize;
  Releaser releaser;
  char *elements;
} CArrayStruct;

typedef CArrayStruct *CArray;

// Constant-time operations.

CArray CArrayNew(int capacity, size_t elementSize);  // Allocates and initializes a new CArray.
CArray CArrayInit(CArray cArray, int capacity, size_t elementSize);  // For use on an allocated but uninit'd CArray struct.

// The next three methods are O(1) if there's no releaser; O(n) if there is.
void CArrayClear(CArray cArray);   // Releases all elements and sets count to 0.
void CArrayRelease(void *cArray);   // Clears cArray and frees all capacity; doesn't free cArray itself.
void CArrayDelete(CArray cArray);  // Releases cArray and frees cArray itself.

void *CArrayElement(CArray cArray, int index);
#define CArrayElementOfType(array, i, type) (*(type *)CArrayElement(array, i))

// Amortized constant-time operations (some are usually constant-time, sometimes linear).

void CArrayAppendContents(CArray cArray, CArray source);  // Expects cArray != source.
#define CArrayAddElement(x, y) CArrayAddElementByPointer(x, &y)
void CArrayAddElementByPointer(CArray cArray, void *element);
void *CArrayNewElement(CArray cArray);

int CArrayIndexOf(CArray cArray, void *element);

// Possibly linear time operations.

// element is expected to be an object already within cArray, i.e.,
// the location of element should be in the cArray->elements memory buffer.
void CArrayRemoveElement(CArray cArray, void *element);
void CArrayAddZeroedElements(CArray cArray, int numElements);

// Tools for nice iterations.
// If you used sizeof(x) to set up the CArray, then the type for CArrayFor
// is expected to be "x *" (a pointer to x).

void *CArrayEnd(CArray cArray);

// The (type) casts in CArrayFor and CArrayForBackwards are required by C++.

#define CArrayFor(type, var, cArray) \
for (type var = (type)CArrayElement(cArray, 0); var != CArrayEnd(cArray); ++var)

#define CArrayForBackwards(type, var, cArray) \
for (type var = (type)CArrayElement(cArray, cArray->count - 1); cArray->count && var >= (type)cArray->elements; --var)

typedef int (*CompareFunction)(void *, const void *, const void *);
void CArraySort(CArray cArray, CompareFunction compare, void *compareContext);

// Assumes the array is sorted in ascending memcmp order; does a memcmp of each element
// in the array, using a binary search.
void *CArrayFind(CArray cArray, void *elt);

#endif
