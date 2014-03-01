#include "CList.h"

#include <stdio.h>

int main() {
  // Create the list [1, 2, 3], reverse it, examine the first element,
  // and remove the first element.

  CList list = NULL;  // This is effectively an empty list.

  // Lists support inserts rather than appends, so we insert
  // the numbers in the order 3, 2, 1; the result is the list [1, 2, 3].
  for (long i = 3; i >= 1; --i) {
    // Unlike CArray and CMap, you always pass in a "CList *" object
    // as the first parameter of CList* functions; this is used to
    // support NULL pointers as empty lists.
    CListInsert(&list, (void *)i);
    // The 2nd param to CListInsert is the value inserted, of type (void *).
    // You may store primitives cast to (void *), or store an arbitrary pointer.
  }

  // Print out the first element.
  printf("The first element is now %ld.\n", (long)list->element);  // It's 1.

  printf("Reversing the list.\n");
  CListReverse(&list);

  printf("The first element is now %ld.\n", (long)list->element);  // It's 3.

  // Delete every non-empty list when done. It's safe to delete empty lists.
  // After this call, list == NULL, and remains a valid list for future use.
  CListDelete(&list);
  return 0;
}
