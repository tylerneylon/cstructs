#include "CList.h"

#include <stdio.h>

void printList(CList list) {
  CListFor(int *, i, list) {
    printf("%d ", *i);
  }
  // Confirming we can compile multiple CListFor's
  // within the same variable scope.
  CListFor(int *, i, list);
  printf("\n");
}

int intEq(void *a, void *b) {
  int i = *(int *)a;
  int j = *(int *)b;
  return i == j;
}

int main() {
  CList list = NULL;
  printf("Init list: ");
  printList(list);

  for (int i = 0; i < 10; ++i) {
    int *p = malloc(sizeof(int));
    *p = rand() % 10;
    CListInsert(&list, p);
    printf("After insert: ");
    printList(list);
  }

  int needle = 3;
  int *result = (int *)CListFindValue(&list, &needle, intEq);
  printf("Find for %d returned %p, with value %d\n",
      needle, result, (result ? *result : -1));

  int n = CListReverse(&list);
  printf("After reverse: ");
  printList(list);
  printf("Reverse returned %d\n", n);

  for (int i = 0; i < 10; ++i) {
    CListRemoveFirst(&list);
    printf("After remove-first: ");
    printList(list);
  }

  CListDeleteAndRelease(&list, free);

  printf("After delete, list is now %p\n===\n", list);

  // Test some operations on empty lists.
  // This part passes if it doesn't crash :)
  list = NULL;
  CListRemoveFirst(&list);
  n = CListReverse(&list);
  printf("Reverse returned %d\n", n);

  return 0;
}
