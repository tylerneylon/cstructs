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

int main() {
  signal(SIGSEGV, handleSegFault);
  signal(11, handleSegFault);

  // Set up an array of pairs; the size is set by NUM_PAIRS.
  Pair pairs[NUM_PAIRS];
  for (int i = 0; i < NUM_PAIRS; ++i) {
    pairs[i].str = makeRandomStr();
    pairs[i].num = rand() % 1000;
  }

  printf("[[2]]\n");

  // Build an array with numEntries entries.
  int numEntries = 1e6;
  CMap map = CMapNew(hash, eq);
  for (int i = 0; i < numEntries; ++i) {
    int j = rand() % NUM_PAIRS;
    //printf("%d: Key is %s\n", i, pairs[j].str);
    CMapSet(map, (char *)pairs[j].str, (void *)(long)pairs[j].num);
  }
  printf("[[3]]\n");

  // Double check that entries seem to be correct.
  int numFound = 0;
  int numTested = 100;
  for (int i = 0; i < numTested; ++i) {
    int j = rand() % NUM_PAIRS;
    KeyValuePair *pair = CMapFind(map, pairs[j].str);
    if (pair == NULL) continue;
    numFound++;
    int foundValue = (int)(long)(pair->value);
    if (foundValue != pairs[j].num) {
      printf("Fail!  For key %s, expected value %d but map returned %d\n",
          pairs[j].str, pairs[j].num, foundValue);
    }
  }
  double singleMiss = (double)(NUM_PAIRS - 1.0) / NUM_PAIRS;
  double allMissProb = pow(singleMiss, numEntries);
  double expFindRate = 1.0 - allMissProb;
  printf("Found %d out of %d lookups (expecting about %.0f%%)\n",
      numFound, numTested, (100.0 * expFindRate));

  // Do 100k lookups of items probably in the map.
  for (int i = 0; i < 1e6; ++i) {
    int j = rand() % NUM_PAIRS;
    CMapFind(map, pairs[j].str);
  }

  // Do 100k lookups of items probably not the map.
  for (int i = 0; i < 1e6; ++i) {
    char *str = makeRandomStr();
    CMapFind(map, str);
    free(str);
  }

  CMapDelete(map);
  printf("[[4]]\n");
  
  return 0;
}
