// listtest.c
//
// https://github.com/tylerneylon/cstructs
//

#include "cstructs/cstructs.h"

#include "ctest.h"

#include <stdio.h>

void print_list(List list) {
  list__for(int *, i, list) {
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

void counted_free(void *ptr, void *ctx) {
  free(ptr);
  num_free_calls++;
}

int test_list() {
  List list = NULL;
  test_printf("Init list: ");
  print_list(list);

  for (int i = 0; i < 10; ++i) {
    int *p = malloc(sizeof(int));
    *p = i;
    list__insert(&list, p);
    test_printf("After insert: ");
    print_list(list);
  }

  test_that(list != NULL);
  test_that(*(int *)list->item == 9);
  test_that(list__count(&list) == 10);

  int needle = 3;
  int *result = (int *)list__find_value(&list, &needle, int_eq);
  test_printf("Find for %d returned %p, with value %d\n",
      needle, result, (result ? *result : -1));
  test_that(result && *result == needle);

  list__delete_and_release(&list, counted_free, NULL);  // NULL --> context
  test_that(list == NULL);
  test_that(num_free_calls == 10);

  return test_success;
}

int test_list_simple() {
  List list = NULL;
  int values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) list__insert(&list, values + i);
  test_that(list__count(&list) == 5);

  // Because insert always pushes to the front, the list is
  // the reverse order as compared to the values array.
  int i = 4;
  list__for(int *, i_ptr, list) {
    test_that(i >= 0);
    test_printf("*i_ptr=%d values[i]=%d\n", *i_ptr, values[i]);
    test_that(*i_ptr == values[i]);
    i--;
  }

  list__reverse(&list);
  test_that(list__count(&list) == 5);

  i = 0;
  list__for(int *, i_ptr, list) {
    test_that(i < 5);
    test_that(*i_ptr == values[i]);
    i++;
  }

  list__delete(&list);
  test_that(list == NULL);

  return test_success;
}

int test_remove_first() {
  List list = NULL;
  int values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) list__insert(&list, values + i);

  // Before removing, we expect the list to be [11, 7, 5, 3, 2].
  void *item = list__remove_first(&list);
  test_that(*(int *)item == 11);
  test_that(*(int *)list->item == 7);

  item = list__remove_first(&list);
  test_that(*(int *)item == 7);
  test_that(*(int *)list->item == 5);

  test_that(list__count(&list) == 3);

  list__delete(&list);

  return test_success;
}

int test_find_entry() {
  List list = NULL;
  int values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) list__insert(&list, values + i);

  int needle = 5;
  List *sublist = list__find_entry(&list, &needle, int_eq);
  // We now expect sublist to be [5, 3, 2].
  test_that(*(int *)(*sublist)->item == 5);  // Ugh, that syntax is easy to mess up.
  test_that(list__count(sublist) == 3);

  list__delete(&list);

  return test_success;
}

int test_store_primitives() {
  List list = NULL;
  list__insert(&list, (void *)10);
  list__insert(&list, (void *)20);
  test_that(list__count(&list) == 2);
  test_that((long)list->item == 20);

  list__delete(&list);

  return test_success;
}

int long_eq(void *elt1, void *elt2) {
  return (long)elt1 == (long)elt2;
}

void print_long_list(List list) {
  list__for(long, elt, list) {
    test_printf("%ld ", elt);
  }
  test_printf("\n");
}

int test_delete_mid_list() {
  List list = NULL;
  long values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) list__insert(&list, (void *)values[4 - i]);
  test_that(list__count(&list) == 5);
  test_that((long)list->item == 2);

  test_printf("The list is now:\n");
  print_long_list(list);  // [2, 3, 5, 7, 11].

  List *tail = list__find_entry(&list, (void *)5, long_eq);
  list__delete(tail);

  test_printf("The list is now:\n");
  print_long_list(list);  // [2, 3].

  test_that(list__count(&list) == 2);
  test_that((long)list->item == 2);

  return test_success;
}

int test_move_first() {
  List list1 = NULL;
  long values[5] = {2, 3, 5, 7, 11};
  for (int i = 0; i < 5; ++i) list__insert(&list1, (void *)values[4 - i]);

  // The list now contains the literal values [2, 3, 5, 7, 11].

  List list2 = NULL;
  test_printf("The lists are now:\n");
  print_long_list(list1);
  print_long_list(list2);

  // Moving from an empty list should return NULL and change nothing.
  void *item = list__move_first(&list2, &list1);
  test_that(item == NULL);
  test_that(list__count(&list1) == 5);
  test_that(list__count(&list2) == 0);

  // Move 2 over; result is list1 = [3, 5, 7, 11]; list2 = [2].
  item = list__move_first(&list1, &list2);

  test_printf("The lists are now:\n");
  print_long_list(list1);
  print_long_list(list2);

  test_that((long)item == 2);
  test_that((long)list2->item == 2);
  test_that((long)list1->item == 3);
  test_that(list__count(&list1) == 4);
  test_that(list__count(&list2) == 1);

  list__delete(&list1);
  list__delete(&list2);

  return test_success;
}

int main(int argc, char **argv) {
  start_all_tests(argv[0]);
  run_tests(
    test_list, test_list_simple, test_remove_first, test_find_entry,
    test_store_primitives, test_delete_mid_list, test_move_first
  );
  return end_all_tests();
}
