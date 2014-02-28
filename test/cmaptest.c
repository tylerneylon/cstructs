#include "CMap.h"

#include "ctest.h"

#include <stdlib.h>
#include <string.h>

#define NUM_PAIRS 10000


typedef struct {
  char *str;
  int num;
} Pair;

void print_map(CMap map) {
  CMapFor(pair, map) {
    test_printf("%s -> %d\n", (char *)pair->key, (int)(long)pair->value);
  }
}

char *make_random_str() {
  int len = rand() % 10 + 5;
  char *str = malloc(len + 1);
  int i;
  for (i = 0; i < len; ++i) {
    str[i] = rand() % 26 + 'a';
  }
  str[i] = '\0';
  return str;
}

int hash(void *str_void_ptr) {
  char *str = (char *)str_void_ptr;
  //return *str * 2345 + *(str + 1);
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

int test_cmap() {
  // Set up an array of pairs; the size is set by NUM_PAIRS.
  Pair pairs[NUM_PAIRS];
  for (int i = 0; i < NUM_PAIRS; ++i) {
    pairs[i].str = make_random_str();
    pairs[i].num = rand() % 1000;
  }

  // Build an array with num_entries entries.
  int num_entries = 1e6;
  CMap map = CMapNew(hash, eq);
  for (int i = 0; i < num_entries; ++i) {
    int j = rand() % NUM_PAIRS;
    //test_printf("%d: Key is %s\n", i, pairs[j].str);
    CMapSet(map, (char *)pairs[j].str, (void *)(long)pairs[j].num);
  }

  // Check that entries seem to be correct.
  int num_tested = 100;
  for (int i = 0; i < num_tested; ++i) {
    int j = rand() % NUM_PAIRS;
    KeyValuePair *pair = CMapFind(map, pairs[j].str);
    int found_value = (int)(long)(pair->value);
    if (found_value != pairs[j].num) {
      test_printf("Fail!  For key %s, expected value %d but map returned %d\n",
          pairs[j].str, pairs[j].num, found_value);
      test_failed();
    }
  }

  // Do 10k lookups of items probably not in the map.
  int num_found = 0;
  int num_checks = 1e4;
  for (int i = 0; i < num_checks; ++i) {
    char *str = make_random_str();
    if(CMapFind(map, str)) num_found++;
    test_that((float)num_found / num_checks < 0.01);
    free(str);
  }

  CMapDelete(map);

  return test_success;
}

int test_unset() {
  CMap map = CMapNew(hash, eq);
  //map->keyReleaser = free;  // But not for literals, my friend.

  CMapSet(map, "book", (void *)4L);
  CMapSet(map, "games", (void *)5L);
  CMapSet(map, "burger", (void *)6L);
  print_map(map);

  test_that(CMapFind(map, "games") != NULL);

  CMapUnset(map, "games");
  test_printf("After unsetting \"games\":\n");
  print_map(map);

  test_that(CMapFind(map, "games") == NULL);

  CMapDelete(map);

  return test_success;
}

int main(int argc, char **argv) {
  start_all_tests(argv[0]);
  run_tests(test_cmap, test_unset);
  return end_all_tests();
}
