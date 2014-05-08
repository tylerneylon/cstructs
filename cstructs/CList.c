//  CList.c
//

#include "CList.h"

#ifdef DEBUG
#include "memprofile.h"
#endif

void CListInsert(CList *list, void *element) {
  CList nextList = *list;
  *list = malloc(sizeof(CListStruct));
  (*list)->element = element;
  (*list)->next = nextList;
}

void *CListRemoveFirst(CList *list) {
  if (*list == NULL) { return NULL; }  // See note [1] below.
  CListStruct removed_item = **list;
  free(*list);
  *list = removed_item.next;
  return removed_item.element;
}

void *CListMoveFirst(CList *from, CList *to) {
  if (*from == NULL) { return NULL; }  // See note [1] below.

  // from starts [a, b, ..]; to starts [c, ..].
  CList a = *from;
  CList b = (*from)->next;
  CList c = *to;
  *to = a;
  a->next = c;
  *from = b;

  return a->element;
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

// [1] There's a bug in visual studio 2013 where variable declarations after
//     a one-line code block without braces aren't recognized. The workaround is
//     to add braces to those one-liners. See the comments here:
//     http://stackoverflow.com/a/9903698/3561