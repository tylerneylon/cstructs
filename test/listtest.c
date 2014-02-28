#include "CList.h"

#include "ctest.h"

#include <stdio.h>

void print_list(CList list) {
  CListFor(int *, i, list) {
    test_printf("%d ", *i);
  }
  test_printf("\n");
}

int int_eq(void *a, void *b) {
  int i = *(int *)a;
  int j = *(int *)b;
  return i == j;
}

int num_free_calls = 0;

void counted_free(void *ptr) {
  free(ptr);
  num_free_calls++;
}

int test_list() {
  CList list = NULL;
  test_printf("Init list: ");
  print_list(list);

  for (int i = 0; i < 10; ++i) {
    int *p = malloc(sizeof(int));
    *p = i;
    CListInsert(&list, p);
    test_printf("After insert: ");
    print_list(list);
  }

  test_that(list != NULL);
  test_that(*(int *)list->element == 9);
  test_that(CListCount(&list) == 10);

  int needle = 3;
  int *result = (int *)CListFindValue(&list, &needle, int_eq);
  test_printf("Find for %d returned %p, with value %d\n",
      needle, result, (result ? *result : -1));
  test_that(result && *result == needle);

  CListDeleteAndRelease(&list, counted_free);
  test_that(list == NULL);
  test_that(num_free_calls == 10);

  return test_success;
}

int test_list_simple() {
  CList list = NULL;
  int values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) CListInsert(&list, values + i);
  test_that(CListCount(&list) == 5);

  // Because insert always pushes to the front, the list is
  // the reverse order as compared to the values array.
  int i = 4;
  CListFor(int *, i_ptr, list) {
    test_that(i >= 0);
    test_printf("*i_ptr=%d values[i]=%d\n", *i_ptr, values[i]);
    test_that(*i_ptr == values[i]);
    i--;
  }

  CListReverse(&list);
  test_that(CListCount(&list) == 5);

  i = 0;
  CListFor(int *, i_ptr, list) {
    test_that(i < 5);
    test_that(*i_ptr == values[i]);
    i++;
  }

  CListDelete(&list);
  test_that(list == NULL);

  return test_success;
}

int test_remove_first() {
  CList list = NULL;
  int values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) CListInsert(&list, values + i);

  // Before removing, we expect the list to be [11, 7, 5, 3, 2].
  CListRemoveFirst(&list);
  test_that(*(int *)list->element == 7);

  CListRemoveFirst(&list);
  test_that(*(int *)list->element == 5);

  test_that(CListCount(&list) == 3);

  CListDelete(&list);

  return test_success;
}

int test_find_entry() {
  CList list = NULL;
  int values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) CListInsert(&list, values + i);

  int needle = 5;
  CList *sublist = CListFindEntry(&list, &needle, int_eq);
  // We now expect sublist to be [5, 3, 2].
  test_that(*(int *)(*sublist)->element == 5);  // Ugh, that syntax is easy to mess up.
  test_that(CListCount(sublist) == 3);

  CListDelete(&list);

  return test_success;
}

int test_store_primitives() {
  CList list = NULL;
  CListInsert(&list, (void *)10);
  CListInsert(&list, (void *)20);
  test_that(CListCount(&list) == 2);
  test_that((int)list->element == 20);

  CListDelete(&list);

  return test_success;
}

int main(int argc, char **argv) {
  start_all_tests(argv[0]);
  run_tests(
    test_list, test_list_simple, test_remove_first, test_find_entry,
    test_store_primitives
  );
  return end_all_tests();
}
