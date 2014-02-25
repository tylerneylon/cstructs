//  CList.c
//

#include "CList.h"

#include "memprofile.h"

void CListInsert(CList *list, void *element) {
  CList nextList = *list;
  *list = malloc(sizeof(CListStruct));
  (*list)->element = element;
  (*list)->next = nextList;
}

void CListRemoveFirst(CList *list) {
  if (*list == NULL) return;
  CList secondItem = (*list)->next;
  free(*list);
  *list = secondItem;
}

void CListDelete(CList *list) {
  CListDeleteAndRelease(list, NULL);
}

void CListDeleteAndRelease(CList *list, Releaser releaser) {
  while (*list) {
    CList next = (*list)->next;
    if (releaser) releaser((*list)->element);
    free(*list);
    *list = next;
  }
  // This leaves *list == NULL, as we want.
}

CList *CListFindEntry(CList *list, void *needle, int (*valEqNeedle)(void *, void *)) {
  for (CList *iter = list; *iter; iter = &((*iter)->next)) {
    if (valEqNeedle((*iter)->element, needle)) return iter;
  }
  return NULL;
}

void *CListFindValue(CList *list, void *needle, int (*valEqNeedle)(void *, void *)) {
  CListFor(void *, elt, *list) if (valEqNeedle(elt, needle)) return elt;
  return NULL;
}

int CListReverse(CList *list) {
  int n = 0;
  CList iter = NULL;
  for (CList newNext, next = *list; next; next = newNext) {
    newNext = next->next;
    next->next = iter;
    iter = next;
    ++n;
  }
  *list = iter;
  return n;
}

int CListCount(CList *list) {
  int n = 0;
  for (CList iter = *list; iter; iter = iter->next, ++n);
  return n;
}
