// array_example.c
//
// https://github.com/tylerneylon/cstructs
//

#include "cstructs/cstructs.h"

#include <stdio.h>

int main() {
  // Set up and use an array of strings.

  // 16 is the initial capacity;
  // sizeof(char *) indicates a single item is a char *.
  Array array = array__new(16, sizeof(char *));

  char *strs[4] = {"hi", "there", "what's", "up"};
  for (int i = 0; i < 4; ++i) {

    // Append an item to the end of the array.
    // The item should have the type sent in to array__new via sizeof,
    // and cannot be a literal as it's dereferenced and passed as a pointer.
    array__add_item_val(array, strs[i]);
  }

  // This intuitively means s = array[1];
  char *s = array__item_val(array, 1, char *);

  // This intuitively means item = array + 1; item points into the array.
  // We have *item == s.
  void *item = array__item_ptr(array, 1);

  // This function expects item to point directly into the array.
  // This fails if we send in s since s is a value, not a pointer to a value.
  array__remove_item(array, item);  // Removes array[1].

  printf("Strings in the array:\n");

  // A for loop. The 2nd parameter = the iterator.
  // The first parameter = the type of the iterator =
  // a pointer to the type sent into array__new.
  array__for(char **, str_ptr, array, index) {
    printf("array[%d]=%s\n", index, *str_ptr);
  }
  // Prints out: hi what's up

  // Call array__delete on every array created with array__new.
  array__delete(array);

  return 0;
}
