// arraytest.c
//
// https://github.com/tylerneylon/cstructs
//

#include "cstructs/cstructs.h"

#include "ctest.h"

#include <stdio.h>
#include <string.h>

#include "winutil.h"

#define array_size(x) (sizeof(x) / sizeof(x[0]))

void print_int_array(Array int_array) {
  array__for(int *, i, int_array, idx) {
    test_printf("%d ", *i);
  }
  test_printf("\n");
}

void print_double_array(Array double_array) {
  array__for(double *, d, double_array, i) {
    test_printf("%f ", *d);
  }
  test_printf("\n");
}

// Test out an array of direct ints.
int test_int_array() {
  Array array = array__new(0, sizeof(int));
  test_that(array->count == 0);

  int i = 1;
  array__add_item_val(array, i);  // array is now [1].
  test_that(array__item_val(array, 0, int) == 1);
  test_that(array->count == 1);

  array__add_zeroed_items(array, 2);  // array is now [1, 0, 0].
  test_that(array__item_val(array, 2, int) == 0);
  test_that(array->count == 3);

  // Test out appending another array.
  Array other_array = array__new(4, sizeof(int));
  int other_ints[] = {2, 3, 4, 5};
  for (i = 0; i < 4; ++i) array__add_item_val(other_array, other_ints[i]);
  test_printf("other_array has contents:\n");
  print_int_array(other_array);
  array__append_array(array, other_array);
  test_printf("Just after append, array is:\n");
  print_int_array(array);
  array__delete(other_array);
  test_that(array->count == 7);
  test_that(array__item_val(array, 5, int) == 4);

  // Test out a for loop over the items.
  int values[] = {1, 0, 0, 2, 3, 4, 5};
  array__for(int *, int_ptr, array, i) {
    test_that(*int_ptr == values[i]);
  }

  array__delete(array);

  return test_success;
}

// This tests a pattern using an array of arrays of ints.
int test_subarrays() {
  Array array = array__new(16, sizeof(ArrayStruct));
  array->releaser = array__release;

  for (int i = 0; i < 10; ++i) {
    Array subarray = array__init(array__new_item_ptr(array), 16, sizeof(int));
    test_printf("subarray = %p\n", subarray);
    for (int j = 0; j < 5; ++j) {
      int newInt = j + i * 5;
      array__add_item_val(subarray, newInt);
    }
    print_int_array(subarray);
  }

  test_printf("Starting to print out subarrays.\n");
  array__for(Array, subarray, array, i) {
    int elt = array__item_val(subarray, 2, int);
    test_that(elt == i * 5 + 2);
    print_int_array(subarray);
  }

  array__delete(array);

  return test_success;
}

// Test that the releaser is properly called.
static int num_releaser_calls = 0;
void counting_releaser(void *element) {
  num_releaser_calls++;
}

int test_releaser() {
  const int num_elts = 32;
  Array array = array__new(64, sizeof(int));
  array->releaser = counting_releaser;

  for (int i = 0; i < num_elts; ++i) {
    array__add_item_ptr(array, &i);
  }

  array__delete(array);

  test_printf("num_releaser_calls=%d.\n", num_releaser_calls);
  test_that(num_releaser_calls == num_elts);

  return test_success;
}

int test_clear() {
  Array array = array__new(0, sizeof(double));
  double values[] = {1.0, 2.0, 3.14};
  for (int i = 0; i < array_size(values); ++i) array__add_item_val(array, values[i]);
  test_that(array->count == 3);
  test_that(array__item_val(array, 2, double) == 3.14);
  array__clear(array);
  test_that(array->count == 0);
  array__delete(array);
  return test_success;
}

// Test that sorting works.
int compare_doubles(void *context, const void *elt1, const void *elt2) {
  double d1 = *(double *)elt1;
  double d2 = *(double *)elt2;
  if (d1 > d2) return 1;
  return d1 < d2 ? -1 : 0;
}

int test_sort() {
  Array array = array__new(0, sizeof(double));
  double values[] = {-1.2, 2.4, 3.1, 0.0, -2.2};
  for (int i = 0; i < array_size(values); ++i) array__add_item_val(array, values[i]);
  test_that(array->count == 5);
  test_that(array__item_val(array, 2, double) == 3.1);
  test_printf("Before sorting, array is:\n");
  print_double_array(array);
  array__sort(array, compare_doubles, NULL);
  test_printf("After sorting, array is:\n");
  print_double_array(array);
  test_that(array__item_val(array, 0, double) == -2.2);
  test_that(array__item_val(array, 1, double) == -1.2);
  array__delete(array);
  return test_success;
}

int test_remove() {
  Array array = array__new(0, sizeof(double));
  double values[] = {2.0, 3.0, 5.0, 7.0};
  for (int i = 0; i < array_size(values); ++i) array__add_item_val(array, values[i]);
  test_that(array->count == 4);

  double *element = array__item_ptr(array, 2);
  test_that(*element == 5.0);

  test_printf("About to call remove element.\n");
  array__remove_item(array, element);
  test_that(array->count == 3);
  test_that(array__item_val(array, 2, double) == 7.0);
  array__delete(array);
  return test_success;
}

int test_indexof() {
  Array array = array__new(0, sizeof(double));
  double values[] = {2.0, 3.0, 5.0, 7.0};
  for (int i = 0; i < array_size(values); ++i) array__add_item_val(array, values[i]);
  test_that(array->count == 4);

  double *element = array__item_ptr(array, 2);
  test_that(*element == 5.0);

  test_that(array__index_of(array, element) == 2);

  --element;
  test_that(array__index_of(array, element) == 1);

  array__delete(array);

  return test_success;
}

// Test the find functionality; we need to sort items in memcmp order for find to work.
int mem_compare(void *context, const void *elt1, const void *elt2) {
  return memcmp(elt1, elt2, *(size_t *)context);
}

int test_find() {
  size_t element_size = sizeof(double);

  Array array = array__new(0, element_size);
  double values[] = {2.0, 3.0, 5.0, 7.0};
  for (int i = 0; i < array_size(values); ++i) array__add_item_val(array, values[i]);
  test_that(array->count == 4);

  array__sort(array, mem_compare, &element_size);

  void *element = array__find(array, &values[1]);
  test_that(*(double *)element == values[1]);

  array__delete(array);

  return test_success;
}

int test_string_array() {
  Array array = array__new(4, sizeof(char *));

  // A nice way to add string literals.
  array__new_item_val(array, char *) = "one";
  array__new_item_val(array, char *) = "two";

  // A nice way to allocate & store runtime-created strings.
  asprintf((char **)array__new_item_ptr(array), "thr%s", "ee");

  // Note that if you mix the above methods, then you'll have
  // to be very careful about freeing the strings made by
  // asprintf but not the literals; therefore it's easier to
  // avoid mixing them!

  test_that(strcmp(array__item_val(array, 0, char *), "one") == 0);

  char *s = array__item_val(array, 2, char *);
  test_that(strcmp(s, "three") == 0);
  free(s);

  array__delete(array);

  return test_success;
}

int test_edge_cases() {
  Array array = array__new(0, sizeof(char));
  for (char c = 'a'; c <= 'z'; ++c) array__add_item_val(array, c);
  test_that(array__item_val(array, 2, char) == 'c');
  array__delete(array);

  array = array__new(8, sizeof(int));
  int values[] = {1, 3, 5, 7};
  for (int i = 0; i < array_size(values); ++i) array__add_item_val(array, values[i]);
  test_that(array->count == 4);

  size_t element_size = sizeof(int);
  array__sort(array, mem_compare, &element_size);
  array__remove_item(array, array__item_ptr(array, 3));
  test_that(array->count == 3);
  array__remove_item(array, array__item_ptr(array, 2));
  test_that(array->count == 2);
  array__remove_item(array, array__item_ptr(array, 0));
  test_that(array->count == 1);
  test_that(array__item_val(array, 0, int) == 3);
  array__remove_item(array, array__item_ptr(array, 0));
  test_that(array->count == 0);
  array__clear(array);  // Expected to do nothing, since array is empty.

  array__delete(array);

  return test_success;
}

// Make sure that array__for doesn't crash on an empty array.
int test_empty_loops() {
  Array array = array__new(0, sizeof(int));

  // These might fail by crashing, so we don't need any test_* calls.
  array__for(int *, i, array, idx);

  return test_success;
}

// Make sure that a array__for loop still works when the array it is
// iterating over is growing.
int test_loops_on_growing_arrays() {
  Array array = array__new(0, sizeof(char));
  for (char c = 'a'; c <= 'd'; ++c) array__add_item_val(array, c);

  // This is to check that c is always a valid pointer at the start
  // of every loop iteration. This may be tricky to maintain when
  // the contents of the array are reallocated due to new incoming items.
  array__for(char *, c, array, i) {
    size_t array_bytes = array->count * array->item_size;
    test_that(c >= array->items && c < (array->items + array_bytes));
    if (i < 4) {
      *(char *)array__new_item_ptr(array) = 'x';
      *(char *)array__new_item_ptr(array) = 'y';
    }
  }

  array__delete(array);

  return test_success;
}

// Make sure that array__for's temporary variables don't leak in scope;
// the fear is that something that's supposed to be defined only within
// the inner loop of a array__for may still be defined afterwards, which
// could - if something is broken - cause a compile error here.
int test_two_loops() {
  Array array = array__new(0, sizeof(char));

  array__for(char *, c, array, i);
  array__for(char *, c, array, i);

  array__delete(array);

  return test_success;
}


int main(int argc, char **argv) {
  set_verbose(0);  // Set this to 1 for additional debugging output.
  start_all_tests(argv[0]);
  run_tests(
    test_subarrays, test_int_array, test_releaser,
    test_clear, test_sort, test_remove, test_find,
    test_indexof, test_string_array, test_edge_cases,
    test_empty_loops, test_loops_on_growing_arrays,
    test_two_loops
  );
  return end_all_tests();
}
