#include "CArray.h"

#include <execinfo.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/* Some practice code to help me know how to do a few things:
 * (1) Print out a stack trace on a signal like a seg fault.
 * (2) How to work with an array of arrays, if needed.
 */

void printTrace() {
  void *array[10];
  size_t size;
  char **strings;
  size_t i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
 
  printf ("Obtained %zd stack frames.\n", size);

  for (i = 0; i < size; i++) printf ("%s\n", strings[i]);

  free (strings);
}

void handleSegFault(int sig) {
  printf("Seg fault\n");
  printTrace();
  exit(1);
}

void printIntArray(CArray intArray) {
  CArrayFor(int *, i, intArray) {
    printf("%d ", *i);
  }
  printf("\n");
}

int main(int argc, char **argv) {
  signal(SIGSEGV, handleSegFault);
  CArray array = CArrayNew(16, sizeof(CArrayStruct));
  array->releaser = CArrayRelease;

  for (int i = 0; i < 10; ++i) {
    CArray subarray = CArrayInit(CArrayNewElement(array), 16, sizeof(int));
    printf("subarray = %p\n", subarray);
    for (int j = 0; j < 5; ++j) {
      int newInt = rand() % 10;
      CArrayAddElement(subarray, newInt);
    }
    printIntArray(subarray);
  }

  CArrayFor(CArray, subarray, array) {
    printf("List: ");
    printIntArray(subarray);
  }

  CArrayDelete(array);
  return 0;
}
