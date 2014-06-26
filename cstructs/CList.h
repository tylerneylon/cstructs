// CList.h
//
// Home repo: https://github.com/tylerneylon/cstructs
//
// C-based singly-linked list.
// A NULL pointer is equivalent to an empty list.
//

#ifndef __CLIST_H__
#define __CLIST_H__

#include "CArray.h"

#include <stdlib.h>

// TODO Consider trying to use a union for the element
//      in order to more easily work with ints as elements.

typedef struct CListStruct {
  void *element;
  struct CListStruct *next;
} CListStruct;
typedef CListStruct *CList;

void CListInsert(CList *list, void *element);
void *CListRemoveFirst(CList *list);  // Returns the removed element; or NULL on empty lists.
void *CListMoveFirst(CList *from, CList *to);  // Returns the moved element; NULL on empty lists.

void CListDelete(CList *list);
void CListDeleteAndRelease(CList *list, Releaser releaser);

CList *CListFindEntry(CList *list, void *needle, int (*valEqNeedle)(void *, void *));
void *CListFindValue(CList *list, void *needle, int (*valEqNeedle)(void *, void *));

int CListReverse(CList *list);
int CListCount(CList *list);

// These macros are to be able to get a unique token within other macros.
// See http://stackoverflow.com/questions/1597007/
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define UNIQUE TOKENPASTE2(unique, __LINE__)

#define CListFor(type, var, list) \
  CList UNIQUE = list; \
  for (type var = (type)(UNIQUE ? UNIQUE->element : NULL); \
      UNIQUE; \
      UNIQUE = UNIQUE->next, var = (type)(UNIQUE ? UNIQUE->element : NULL))

#endif
