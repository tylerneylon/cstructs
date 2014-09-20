// list_example.c
//
// https://github.com/tylerneylon/cstructs
//

#include "cstructs/cstructs.h"

#include <stdio.h>

int main() {
  // Create the list [1, 2, 3], reverse it, examine the first item,
  // and remove the first item.

  List list = NULL;  // This is effectively an empty list.

  // Lists support inserts rather than appends, so we insert
  // the numbers in the order 3, 2, 1; the result is the list [1, 2, 3].
  for (long i = 3; i >= 1; --i) {
    // Unlike Array and Map, you always pass in a "List *" object
    // as the first parameter of List* functions; this is used to
    // support NULL pointers as empty lists.
    list__insert(&list, (void *)i);
    // The 2nd param to list__insert is the value inserted, of type (void *).
    // You may store primitives cast to (void *), or store an arbitrary pointer.
  }

  // Print out the first item.
  printf("The first item is now %ld.\n", (long)list->item);  // It's 1.

  printf("Reversing the list.\n");
  list__reverse(&list);

  printf("The first item is now %ld.\n", (long)list->item);  // It's 3.

  // Delete every non-empty list when done. It's safe to delete empty lists.
  // After this call, list == NULL, and remains a valid list for future use.
  list__delete(&list);
  return 0;
}
