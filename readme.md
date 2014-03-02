# CStructs

*Efficient low-level containers in pure C.*

## Purpose

CStructs is a small, efficient set of interfaces that
act like container classes; they are technically not classes
as classes don't exist in pure C.

The primary structures are `CArray` and `CMap`.

A `CArray` is like an javascript array or a Python list. It is
always a contiguous block of elements that are tightly-packed in
memory. Elements are passed with the type `void *`, and can either
be actual pointers, or direct values cast into the `void *` type.

A `CMap` is like a javascript object or a Python dictionary.
It's a hash table with arbitrary key and value types which are
passed around with `void *` pointers; again, direct values are
supported. You are responsible for providing hash
and equality functions for the keys.

The interfaces in CStructs are designed to simplify memory
management and to support nested memory-managed structures.

## Using `CArray`

Here's an example use:

```
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
```

Other functions are described in `CArray.h`. See that header
file for more complete descriptions.

The following is an informal summary of the remaining functions:

* `CArrayInit` - Similar to `CArrayNew`, but operates on an array
  whose memory has already been allocated; this is useful for nesting
  arrays within arrays.
* `CArrayRelease` - A counterpart to `CArrayInit`, this frees all dynamic
  storage of the array, but does not free the array container itself; also
  useful for working with arrays nested in arrays.
* `CArrayClear` - Sets an array's element count to zero.
* `CArrayAddZeroedElements` - Quickly add an arbitrary number of all-zero elements.
* `CArrayAddElementByPointer` - An alternative to `CArrayAddElement` that's useful
  when having a parameter dereferenced is inconvenient; e.g. if you already have
  a pointer.
* `CArraySort` - Sort the array elements using a custom compare function that
  you provide.
* `CArrayFind` - Performs a binary search on the array; assumes it is already
  sorted in `memcmp`-order (note that `memcmp` order may not match your custom
  comparison sort used for `CArraySort`).

## Using `CMap`

Here's an example use:

```
// We'll create the map {abc: 1, xyz: 2}.
// We start by definining string hash and equality functions.
// These functions will only operate on keys, not values.

int hash(void *str_void_ptr) {
  char *str = (char *)str_void_ptr;
  int h = *str;
  while (*str) {
    h *= 234;
    h += *str++;
  }
  return h;
}

int eq(void *str_void_ptr1, void *str_void_ptr2) {
  return !strcmp(str_void_ptr1, str_void_ptr2);
}

void use_map() {
  CMap map = CMapNew(hash, eq);

  // This adds a KeyValuePair pointing directly to the passed in
  // literal "abc", and another pointer with value 0x1, which is
  // cast from the long integer type of 1L.
  // The cast is how we can directly store primitives instead of
  // pointers to other objects; this use is supported by the library.
  CMapSet(map, "abc", (void *)1L);
  CMapSet(map, "xyz", (void *)2L);

  // We can lookup any key in expected constant time.
  KeyValuePair *pair = CMapFind(map, "rgb");
  // Now pair == NULL, since "rgb" is not a key in our map.

  // This looks up and prints out the value 2 for the key "xyz".
  pair = CMapFind(map, "xyz");
  printf("pair->value=%ld\n", (long)pair->value);

  // Print out the entire map. The result is:
  // abc -> 1
  // xyz -> 2
  // ... although the lines may appear in any order.
  CMapFor(pair, map) {
    printf("%s -> %ld\n", pair->key, (long)pair->value);
  }

  // Delete every map created with CMapNew.
  CMapDelete(map);
}

```

The only `CMap` function not used in this examples is
`CMapUnset`, which accepts a key and removes that value from the hash.
It is legal to send in a key that is not in the map, in which case
nothing happens.

## Using `CList`

This container is a lightweight singly-linked list.
It makes to use `CList` instead of `CArray` when:

* You have many empty lists; an empty `CList` uses much less memory than an empty `CArray`.
* You want to insert and remove elements using iterators and don't need random access.

An empty list is just a null pointer of type "CList *".

    CList *list = NULL;  // This is an empty list.

Here's an example use:

```
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
```

Other functions are described in `CList.h`.

The following is an informal summary of the remaining functions:

* `CListRemoveFirst` - Removes the first element in constant time.
* `CListDeleteAndRelease` - Similar to `CListDelete`, except that it
  also calls a custom releaser on each element before removing it from
  the list; useful for nested structures.
* `CListFindEntry` - Finds an entry by value, returning a `CList *` to
  the list's tail that begins with the value. The tail pointer is valid
  input to any other `CList` function, including `CListDelete`.
* `CListFindValue` - The same as `CListFindEntry`, but returns a pointer
  to the element itself (of type `void *`) rather than to the tail of the list.
* `CListCount` - Returns the number of elements in the list; takes
  linear time.


