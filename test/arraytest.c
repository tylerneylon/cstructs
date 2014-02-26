#include "CArray.h"

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// used for test framework only (which may be true for above includes as well!)
#include <libgen.h>
#include <stdarg.h>
#include <string.h>

// TODO
// * Check values - not just waiting for a seg fault.
// * Make sure I have full code coverage.
// * Include all major use cases (not many).
// * Look for edge cases.
// * Try to keep the test framework general (within CStructs).
// * Clean up comments within this file.

/* Some practice code to help me know how to do a few things:
 * (1) Print out a stack trace on a signal like a seg fault.
 * (2) How to work with an array of arrays, if needed.
 */


////////////////////////////////////////////////////////////////////////////////
// Test framework. (I plan to move this into it's own file.)

#define LOG_SIZE 65536
static char *program_name;
static char *test_name;  // A program may run multiple tests.
static char log[LOG_SIZE];
static char *log_cursor;
static char *log_end;

typedef int (*TestFunction)();

int test_printf_(const char *format, ...) {
  static char buffer[LOG_SIZE];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, LOG_SIZE, format, args);
  va_end(args);

  // The -1 makes room for the null terminator.
  size_t size_left = log_end - log_cursor - 1;
  char *new_cursor = log + strlcat(log_cursor, buffer, size_left);
  int chars_added = new_cursor - log_cursor;
  log_cursor = new_cursor;

  return chars_added;
}

#define run_test(x) run_test_(x, #x)

#define test_printf(...) test_printf_(__VA_ARGS__)

void print_trace() {
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
 
  test_printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++) test_printf ("%s\n", strings[i]);

  free (strings);
}

void handle_seg_fault(int sig) {
  printf("\r%s - failed \n", program_name);
  print_trace();
  printf("Failed in test '%s'; log follows:\n---\n", test_name);
  printf("%s\n---\n", log);
  printf("%s failed while running test %s.\n", program_name, test_name);
  exit(1);
}

void start_all_tests(char *name) {
  program_name = basename(name);
  printf("%s - running ", program_name);
  fflush(stdout);
  signal(SIGSEGV, handle_seg_fault);
  log_end = log + LOG_SIZE;
}

void run_test_(TestFunction test_fn, char *new_test_name) {
  log[0] = '\0';
  log_cursor = log;

  test_name = new_test_name;

  test_fn();
}

int end_all_tests() {
  printf("\r%s - passed \n", program_name);
  return 0;
}


// End of test framework.
////////////////////////////////////////////////////////////////////////////////


void printIntArray(CArray intArray) {
  CArrayFor(int *, i, intArray) {
    test_printf("%d ", *i);
  }
  test_printf("\n");
}

int test_array() {
  signal(SIGSEGV, handle_seg_fault);
  CArray array = CArrayNew(16, sizeof(CArrayStruct));
  array->releaser = CArrayRelease;

  for (int i = 0; i < 10; ++i) {
    CArray subarray = CArrayInit(CArrayNewElement(array), 16, sizeof(int));
    test_printf("subarray = %p\n", subarray);
    for (int j = 0; j < 5; ++j) {
      int newInt = rand() % 10;
      CArrayAddElement(subarray, newInt);
    }
    printIntArray(subarray);
  }

  CArrayFor(CArray, subarray, array) {
    test_printf("List: ");
    printIntArray(subarray);
  }

  return 0;
}

int main(int argc, char **argv) {
  start_all_tests(argv[0]);
  run_test(test_array);
  return end_all_tests();
}
