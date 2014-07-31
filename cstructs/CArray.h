// CArray.h
//
// https://github.com/tylerneylon/cstructs
//
// A C structure for flexibly working with a sequence
// of elements that are kept contiguously in memory.
// The array length is flexible, and is designed to
// support nesting of data structures.
//

#pragma once

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

// Loop over an array.
// Example: CArrayFor(item_type *, item_ptr, array, index) { /* loop body */ }
// Think:   type item_ptr = &array[index];  // for each index in the array.

// If you used sizeof(x) to set up the CArray, then the type for CArrayFor is
// expected to be "x *" (a pointer to x). It is safe to continue or break, edit the
// index, and to edit the array itself, although array changes - including *any*
// additions at all - may invalidate item_ptr until the start of the next iteration.
#define CArrayFor(type, item_ptr, array, index) \
  for (int index = 0, __tmpvar = 1; __tmpvar--;) \
  for (type item_ptr = (type)CArrayElement(array, index); index < array->count; item_ptr = (type)CArrayElement(array, ++index))
// The (type) cast in CArrayFor is required by C++.

typedef int (*CompareFunction)(void *, const void *, const void *);
void CArraySort(CArray cArray, CompareFunction compare, void *compareContext);

// Assumes the array is sorted in ascending memcmp order; does a memcmp of each element
// in the array, using a binary search.
void *CArrayFind(CArray cArray, void *elt);
