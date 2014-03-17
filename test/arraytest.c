#include "CArray.h"

#include "ctest.h"

#include <stdio.h>
#include <string.h>

#define array_size(x) (sizeof(x) / sizeof(x[0]))

void print_int_array(CArray int_array) {
  CArrayFor(int *, i, int_array) {
    test_printf("%d ", *i);
  }
  test_printf("\n");
}

void print_double_array(CArray double_array) {
  CArrayFor(double *, d, double_array) {
    test_printf("%f ", *d);
  }
  test_printf("\n");
}

// Test out an array of direct ints.
int test_int_array() {
  CArray array = CArrayNew(0, sizeof(int));
  test_that(array->count == 0);

  int i = 1;
  CArrayAddElement(array, i);  // array is now [1].
  test_that(CArrayElementOfType(array, 0, int) == 1);
  test_that(array->count == 1);

  CArrayAddZeroedElements(array, 2);  // array is now [1, 0, 0].
  test_that(CArrayElementOfType(array, 2, int) == 0);
  test_that(array->count == 3);

  // Test out appending another array.
  CArray other_array = CArrayNew(4, sizeof(int));
  int other_ints[] = {2, 3, 4, 5};
  for (i = 0; i < 4; ++i) CArrayAddElement(other_array, other_ints[i]);
  test_printf("other_array has contents:\n");
  print_int_array(other_array);
  CArrayAppendContents(array, other_array);
  test_printf("Just after append, array is:\n");
  print_int_array(array);
  CArrayDelete(other_array);
  test_that(array->count == 7);
  test_that(CArrayElementOfType(array, 5, int) == 4);

  // Test out a for loop over the elements.
  int values[] = {1, 0, 0, 2, 3, 4, 5};
  i = 0;
  CArrayFor(int *, int_ptr, array) {
    test_that(*int_ptr == values[i++]);
  }

  CArrayDelete(array);

  return test_success;
}

// This tests a pattern using an array of arrays of ints.
int test_subarrays() {
  CArray array = CArrayNew(16, sizeof(CArrayStruct));
  array->releaser = CArrayRelease;

  for (int i = 0; i < 10; ++i) {
    CArray subarray = CArrayInit(CArrayNewElement(array), 16, sizeof(int));
    test_printf("subarray = %p\n", subarray);
    for (int j = 0; j < 5; ++j) {
      int newInt = j + i * 5;
      CArrayAddElement(subarray, newInt);
    }
    print_int_array(subarray);
  }

  test_printf("Starting to print out subarrays.\n");
  int i = 0;
  CArrayFor(CArray, subarray, array) {
    int elt = CArrayElementOfType(subarray, 2, int);
    test_that(elt == i * 5 + 2);
    ++i;
    print_int_array(subarray);
  }

  CArrayDelete(array);

  return test_success;
}

// Test that the releaser is properly called.
static int num_releaser_calls = 0;
void counting_releaser(void *element) {
  num_releaser_calls++;
}

int test_releaser() {
  const int num_elts = 32;
  CArray array = CArrayNew(64, sizeof(int));
  array->releaser = counting_releaser;

  for (int i = 0; i < num_elts; ++i) {
    CArrayAddElementByPointer(array, &i);
  }

  CArrayDelete(array);

  test_printf("num_releaser_calls=%d.\n", num_releaser_calls);
  test_that(num_releaser_calls == num_elts);

  return test_success;
}

int test_clear() {
  CArray array = CArrayNew(0, sizeof(double));
  double values[] = {1.0, 2.0, 3.14};
  for (int i = 0; i < array_size(values); ++i) CArrayAddElement(array, values[i]);
  test_that(array->count == 3);
  test_that(CArrayElementOfType(array, 2, double) == 3.14);
  CArrayClear(array);
  test_that(array->count == 0);
  CArrayDelete(array);
  return test_success;
}

// Test that sorting works.
int compare_doubles(void *context, const void *elt1, const void *elt2) {
  double d1 = *(double *)elt1;
  double d2 = *(double *)elt2;
  return d1 - d2;
}

int test_sort() {
  CArray array = CArrayNew(0, sizeof(double));
  double values[] = {-1.2, 2.4, 3.1, 0.0, -2.2};
  for (int i = 0; i < array_size(values); ++i) CArrayAddElement(array, values[i]);
  test_that(array->count == 5);
  test_that(CArrayElementOfType(array, 2, double) == 3.1);
  test_printf("Before sorting, array is:\n");
  print_double_array(array);
  CArraySort(array, compare_doubles, NULL);
  test_printf("After sorting, array is:\n");
  print_double_array(array);
  test_that(CArrayElementOfType(array, 0, double) == -2.2);
  test_that(CArrayElementOfType(array, 1, double) == -1.2);
  CArrayDelete(array);
  return test_success;
}

int test_remove() {
  CArray array = CArrayNew(0, sizeof(double));
  double values[] = {2.0, 3.0, 5.0, 7.0};
  for (int i = 0; i < array_size(values); ++i) CArrayAddElement(array, values[i]);
  test_that(array->count == 4);

  double *element = CArrayElement(array, 2);
  test_that(*element == 5.0);

  test_printf("About to call remove element.\n");
  CArrayRemoveElement(array, element);
  test_that(array->count == 3);
  test_that(CArrayElementOfType(array, 2, double) == 7.0);
  CArrayDelete(array);
  return test_success;
}

int test_indexof() {
  CArray array = CArrayNew(0, sizeof(double));
  double values[] = {2.0, 3.0, 5.0, 7.0};
  for (int i = 0; i < array_size(values); ++i) CArrayAddElement(array, values[i]);
  test_that(array->count == 4);

  double *element = CArrayElement(array, 2);
  test_that(*element == 5.0);

  test_that(CArrayIndexOf(array, element) == 2);

  --element;
  test_that(CArrayIndexOf(array, element) == 1);

  CArrayDelete(array);

  return test_success;
}

// Test the find functionality; we need to sort elements in memcmp order for find to work.
int mem_compare(void *context, const void *elt1, const void *elt2) {
  return memcmp(elt1, elt2, *(size_t *)context);
}

int test_find() {
  size_t element_size = sizeof(double);

  CArray array = CArrayNew(0, element_size);
  double values[] = {2.0, 3.0, 5.0, 7.0};
  for (int i = 0; i < array_size(values); ++i) CArrayAddElement(array, values[i]);
  test_that(array->count == 4);

  CArraySort(array, mem_compare, &element_size);

  void *element = CArrayFind(array, &values[1]);
  test_that(*(double *)element == values[1]);

  CArrayDelete(array);

  return test_success;
}

int test_string_array() {
  CArray array = CArrayNew(4, sizeof(char *));

  // A nice way to add string literals.
  *(char **)CArrayNewElement(array) = "one";
  *(char **)CArrayNewElement(array) = "two";

  // A nice way to allocate & store runtime-created strings.
  asprintf((char **)CArrayNewElement(array), "thr%s", "ee");

  // Note that if you mix the above methods, then you'll have
  // to be very careful about freeing the strings made by
  // asprintf but not the literals; therefore it's easier to
  // avoid mixing them!

  test_that(strcmp(CArrayElementOfType(array, 0, char *), "one") == 0);

  char *s = CArrayElementOfType(array, 2, char *);
  test_that(strcmp(s, "three") == 0);
  free(s);

  CArrayDelete(array);

  return test_success;
}

int test_edge_cases() {
  CArray array = CArrayNew(0, sizeof(char));
  for (char c = 'a'; c <= 'z'; ++c) CArrayAddElement(array, c);
  test_that(CArrayElementOfType(array, 2, char) == 'c');
  CArrayDelete(array);

  array = CArrayNew(8, sizeof(int));
  int values[] = {1, 3, 5, 7};
  for (int i = 0; i < array_size(values); ++i) CArrayAddElement(array, values[i]);
  test_that(array->count == 4);

  size_t element_size = sizeof(int);
  CArraySort(array, mem_compare, &element_size);
  CArrayRemoveElement(array, CArrayElement(array, 3));
  test_that(array->count == 3);
  CArrayRemoveElement(array, CArrayElement(array, 2));
  test_that(array->count == 2);
  CArrayRemoveElement(array, CArrayElement(array, 0));
  test_that(array->count == 1);
  test_that(CArrayElementOfType(array, 0, int) == 3);
  CArrayRemoveElement(array, CArrayElement(array, 0));
  test_that(array->count == 0);
  CArrayClear(array);  // Expected to do nothing, since array is empty.

  CArrayDelete(array);

  return test_success;
}

int main(int argc, char **argv) {
  start_all_tests(argv[0]);
  run_tests(
    test_subarrays, test_int_array, test_releaser,
    test_clear, test_sort, test_remove, test_find,
    test_indexof, test_string_array, test_edge_cases
  );
  return end_all_tests();
}
