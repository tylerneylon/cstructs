#include "cstructs/cstructs.h"

#include <stdio.h>

int main() {
  // Set up and use an array of strings.

  // 16 is the initial capacity;
  // sizeof(char *) indicates a single element is a char *.
  CArray array = CArrayNew(16, sizeof(char *));

  char *strs[4] = {"hi", "there", "what's", "up"};
  for (int i = 0; i < 4; ++i) {

    // Append an element to the end of the array.
    // The element should have the type sent in to CArrayNew via sizeof,
    // and cannot be a literal as it's dereferenced and passed as a pointer.
    CArrayAddElement(array, strs[i]);
  }

  // This intuitively means s = array[1];
  char *s = CArrayElementOfType(array, 1, char *);

  // This intuitively means elt = array + 1; elt points into the array.
  // We have *elt == s.
  void *elt = CArrayElement(array, 1);

  // This function expects elt to point directly into the array.
  // This fails if we send in s since s is a value, not a pointer to a value.
  CArrayRemoveElement(array, elt);  // Removes array[1].

  printf("Strings in the array:\n");
  // A for loop. The 2nd parameter = the iterator.
  // The first parameter = the type of the iterator =
  // a pointer to the type sent into CArrayNew.
  CArrayFor(char **, str_ptr, array) {
    printf("%s\n", *str_ptr);
  }
  // Prints out: hi what's up

  // Delete every array created with CArrayNew.
  CArrayDelete(array);

  return 0;
}
