# cstructs

*Efficient low-level containers in pure C.*

## Purpose

cstructs is a small, efficient set of interfaces that
act like container classes; they are technically not classes
as classes don't exist in pure C.

The primary structures are `Array` and `Map`.

A `Array` is like a JavaScript array or a Python list. It is
always a contiguous block of items that are tightly-packed in
memory. Items are passed with the type `void *`, and can either
be actual pointers, or direct values cast into the `void *` type.

A `Map` is like a JavaScript object or a Python dictionary.
It's a hash table with arbitrary key and value types which are
passed around with `void *` pointers; again, direct values are
supported. You are responsible for providing hash
and equality functions for the keys.

The interfaces in cstructs are designed to simplify memory
management and to support nested memory-managed structures.

## Using `Array`

Here's an example use:

```
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
// Prints out:
// array[0]=hi
// array[1]=what's
// array[2]=up

// Call array__delete on every array created with array__new.
array__delete(array);
```

Other functions are described in `array.h`. See that header
file for more complete descriptions.

An `Array` struct contains a function pointer called its *releaser*
which, if set, is called when any item is removed. This simplifies
memory management and enables the use of nested containers.
The standard `free` function can be assigned as an `Array`'s
releaser.

The following is an informal summary of the remaining functions:

* `array__init` - Similar to `array__new`, but operates on an array
  whose memory has already been allocated; this is useful for nesting
  arrays within arrays.
* `array__release` - A counterpart to `array__init`, this frees all dynamic
  storage of the array, but does not free the array container itself; also
  useful for working with arrays nested in arrays.
* `array__clear` - Sets an array's item count to zero.
* `array__add_zeroed_items` - Quickly add an arbitrary number of all-zero items.
* `array__add_item_ptr` - An alternative to `array__add_item_val` that's useful
  when having a parameter dereferenced is inconvenient; e.g. if you already have
  a pointer.
* `array__sort` - Sort the array items using a custom compare function that
  you provide.
* `array__find` - Performs a binary search on the array; assumes it is already
  sorted in `memcmp`-order (note that `memcmp` order may not match your custom
  comparison sort used for `array__sort`).

## Using `Map`

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
  Map map = map__new(hash, eq);

  // This adds a map__key_value pointing directly to the passed in
  // literal "abc", and another pointer with value 0x1, which is
  // cast from the long integer type of 1L.
  // The cast is how we can directly store primitives instead of
  // pointers to other objects; this use is supported by the library.
  map__set(map, "abc", (void *)1L);
  map__set(map, "xyz", (void *)2L);

  // We can lookup any key in expected constant time.
  map__key_value *pair = map__find(map, "rgb");
  // Now pair == NULL, since "rgb" is not a key in our map.

  // This looks up and prints out the value 2 for the key "xyz".
  pair = map__find(map, "xyz");
  printf("pair->value=%ld\n", (long)pair->value);

  // Print out the entire map. The result is:
  // abc -> 1
  // xyz -> 2
  // ... although the lines may appear in any order.
  map__for(pair, map) {
    printf("%s -> %ld\n", pair->key, (long)pair->value);
  }

  // Delete every map created with map__new.
  map__delete(map);
}

```

Remaining functions:

* `map__unset` - Removes the given key from the map; does nothing if the
  key is not in the map to begin with.
* `map__clear` - Removes all items from the map.

Like `Array`, `Map` supports custom memory management on its items.
Each `Map` has two function pointers, `key_releaser` and `value_releaser` which,
if set, are called each time a key/value pair is removed from the map.

## Using `List`

This container is a lightweight singly-linked list.
It makes to use `List` instead of `Array` when:

* You have many empty lists; an empty `List` uses much less memory than an empty `Array`.
* You want to insert and remove items using iterators and don't need random access.

An empty list is just a null pointer of type `List`.

    List list = NULL;  // This is an empty list.

Here's an example use:

```
// Create the list [1, 2, 3], reverse it, examine the first item,
// and remove the first item.

List list = NULL;  // This is effectively an empty list.

// Lists support inserts rather than appends, so we insert
// the numbers in the order 3, 2, 1; the result is the list [1, 2, 3].
for (long i = 3; i >= 1; --i) {
  // Unlike Array and Map, you always pass in a "List *" object
  // as the first parameter of list functions; this is used to
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
```

Other functions are described in `list.h`.

Because `List` is focused on being extremely lightweight, it does
not keep room for a custom memory management function pointer.
If you want to add per-item memory management to a `List`, you can do so by:
* directly calling your releaser with each call to `list__remove_first`, and
* using `list__delete_and_release` instead of `list__delete`.

The following is an informal summary of the remaining functions:

* `list__remove_first` - Removes the first item in constant time.
* `list__delete_and_release` - Similar to `list__delete`, except that it
  also calls a custom releaser on each item before removing it from
  the list; useful for nested structures.
* `list__find_entry` - Finds an entry by value, returning a `List *` to
  the list's tail that begins with the value. The tail pointer is valid
  input to any other `List` function, including `list__delete`.
* `list__find_value` - The same as `list__find_entry`, but returns a pointer
  to the item itself (of type `void *`) rather than to the tail of the list.
* `list__count` - Returns the number of items in the list; takes
  linear time.
