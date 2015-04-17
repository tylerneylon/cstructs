// map_example.c
//
// https://github.com/tylerneylon/cstructs
//

#include "cstructs/cstructs.h"

#include <stdio.h>
#include <string.h>

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
  map__key_value *pair = map__get(map, "rgb");
  // Now pair == NULL, since "rgb" is not a key in our map.

  // This looks up and prints out the value 2 for the key "xyz".
  pair = map__get(map, "xyz");
  printf("pair->value=%ld\n", (long)pair->value);

  // Print out the entire map. The result is:
  // abc -> 1
  // xyz -> 2
  // ... although the lines may appear in any order.
  map__for(pair, map) {
    printf("%s -> %ld\n", (char *)pair->key, (long)pair->value);
  }

  // Delete every map created with map__new.
  map__delete(map);
}

int main() {
  use_map();
  return 0;
}
