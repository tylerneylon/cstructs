// maptest.c
//
// https://github.com/tylerneylon/cstructs
//

#include "cstructs/cstructs.h"

#include "ctest.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "winutil.h"


#define NUM_PAIRS 10000

#define false 0
#define true 1

typedef struct {
  char *str;
  int num;
} Pair;

void print_map(Map map) {
  map__for(pair, map) {
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
  int num_entries = (int)1e6;
  Map map = map__new(hash, eq);
  for (int i = 0; i < num_entries; ++i) {
    int j = rand() % NUM_PAIRS;
    //test_printf("%d: Key is %s\n", i, pairs[j].str);
    map__set(map, (char *)pairs[j].str, (void *)(long)pairs[j].num);
  }

  // Check that entries seem to be correct.
  int num_tested = 100;
  for (int i = 0; i < num_tested; ++i) {
    int j = rand() % NUM_PAIRS;
    map__key_value *pair = map__get(map, pairs[j].str);
    int found_value = (int)(long)(pair->value);
    if (found_value != pairs[j].num) {
      test_failed("For key %s, expected value %d but map returned %d\n",
          pairs[j].str, pairs[j].num, found_value);
    }
  }

  // Do 10k lookups of items probably not in the map.
  int num_found = 0;
  int num_checks = (int)1e4;
  for (int i = 0; i < num_checks; ++i) {
    char *str = make_random_str();
    if(map__get(map, str)) num_found++;
    test_that((float)num_found / num_checks < 0.01);
    free(str);
  }

  map__delete(map);

  return test_success;
}

int test_unset() {
  Map map = map__new(hash, eq);
  //map->key_releaser = free;  // But not for literals, my friend.

  map__key_value *pair = map__set(map, "book", (void *)4L);
  test_that(strcmp(pair->key, "book") == 0);
  test_that((long)pair->value == 4);

  pair = map__set(map, "book", (void *)5L);
  test_that(strcmp(pair->key, "book") == 0);
  test_that((long)pair->value == 5);

  pair = map__set(map, "games", (void *)6L);
  test_that(pair != NULL);

  pair = map__set(map, "burger", (void *)7L);
  test_that(pair != NULL);

  print_map(map);

  test_that(map__get(map, "games") != NULL);

  map__unset(map, "games");
  test_printf("After unsetting \"games\":\n");
  print_map(map);

  test_that(map__get(map, "games") == NULL);

  map__delete(map);

  return test_success;
}

static int num_free_calls = 0;
void free_with_counter(void *ptr, void *ctx) {
  test_printf("%s(%p)\n", __FUNCTION__, ptr);
  num_free_calls++;
  free(ptr);
}

int test_clear() {
  Map map = map__new(hash, eq);

  num_free_calls = 0;
  map->value_releaser = free_with_counter;

  map__set(map, "one", strdup("1"));
  map__set(map, "two", strdup("2"));
  map__set(map, "three", strdup("3"));
  print_map(map);

  test_that(map->count == 3);

  map__clear(map);

  test_that(map->count == 0);
  test_that(num_free_calls == 3);

  // Make sure we can still use the map.
  map__set(map, "five", strdup("5"));
  test_that(map->count == 1);
  test_that(map__get(map, "five") != NULL);

  map__delete(map);

  return test_success;
}

// The iterators are built so we can edit or delete
// the current element if we want to; test this.
int test_delete_in_for() {
  Map map = map__new(hash, eq);

  num_free_calls = 0;
  map->value_releaser = free_with_counter;

  map__set(map, "one", strdup("1"));
  map__set(map, "two", strdup("2"));
  map__set(map, "three", strdup("3"));
  print_map(map);

  test_that(map->count == 3);

  int i = 0;
  map__for(pair, map) {
    if (i == 1) {
      map->value_releaser(pair->value, NULL);
      pair->value = strdup("4");
      test_that(num_free_calls == 1);
    } else if (i == 2) {
      map__unset(map, pair->key);
    }
    ++i;
  }
  test_that(i == 3);

  test_that(map->count == 2);
  test_that(num_free_calls == 2);

  int four_is_a_value = false;
  map__for(pair, map) {
    if (strcmp(pair->value, "4") == 0) four_is_a_value = true;
  }
  test_that(four_is_a_value);

  map__delete(map);
  return test_success;
}

int test_empty_loop() {
  Map map = map__new(hash, eq);

  map__for(pair, map);

  map__delete(map);

  return test_success;
}

int test_releasers1() {
  Map map = map__new(hash, eq);
  num_free_calls = 0;
  map->key_releaser = free_with_counter;

  for (int i = 0; i < 10; ++i) {
    char *str_key;
    asprintf(&str_key, "%d", i % 3);
    map__set(map, str_key, (void *)1L);
  }

  // There are 3 active keys, and 10 allocated by asprintf,
  // so 7 should have been released by now.
  test_that(num_free_calls == 7);
  map__delete(map);
  test_that(num_free_calls == 10);

  return test_success;
}

int test_releasers2() {
  Map map = map__new(hash, eq);
  num_free_calls = 0;
  map->value_releaser = free_with_counter;

  char *keys[] = {"1", "2", "3"};

  for (int i = 0; i < 10; ++i) {
    char *str_key;
    asprintf(&str_key, "%d", i % 3);
    map__set(map, keys[i % 3], str_key);
    if (i >= 2) test_that(num_free_calls == i - 2);
  }
  map__delete(map);
  test_that(num_free_calls == 10);

  return test_success;
}

int main(int argc, char **argv) {
  set_verbose(0);  // Set this to 1 while debugging a test.
  start_all_tests(argv[0]);
  run_tests(test_cmap, test_unset, test_clear,
            test_delete_in_for, test_empty_loop,
            test_releasers1, test_releasers2);
  return end_all_tests();
}
