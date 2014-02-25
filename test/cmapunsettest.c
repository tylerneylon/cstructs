#include "CMap.h"

#include <execinfo.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_PAIRS 10000


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


typedef struct {
  char *str;
  int num;
} Pair;

char *makeRandomStr() {
  int len = rand() % 10 + 5;
  char *str = malloc(len + 1);
  int i;
  for (i = 0; i < len; ++i) {
    str[i] = rand() % 26 + 'a';
  }
  str[i] = '\0';
  return str;
}

int hash(void *voidStr) {
  char *str = (char *)voidStr;
  //return *str * 2345 + *(str + 1);
  int h = *str;
  while (*str) {
    h *= 234;
    h += *str++;
  }
  return h;
}

int eq(void *voidStr1, void *voidStr2) {
  return !strcmp(voidStr1, voidStr2);
}

void printMap(CMap map) {
  CMapFor(pair, map) {
    printf("%s -> %d\n", (char *)pair->key, (int)(long)pair->value);
  }
}

int main() {
  signal(SIGSEGV, handleSegFault);
  signal(11, handleSegFault);

  CMap map = CMapNew(hash, eq);
  //map->keyReleaser = free;  // But not for literals, my friend.

  CMapSet(map, "book", (void *)4L);
  CMapSet(map, "games", (void *)5L);
  CMapSet(map, "burger", (void *)6L);
  printMap(map);

  CMapUnset(map, "games");
  printf("After unsetting \"games\":\n");
  printMap(map);

  CMapDelete(map);

  return 0;
}
