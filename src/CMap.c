//  CMap.c
//
//  Internal structure:
//  An array of buckets of size s = 2^n, where n grows to keep
//  the average load below 8.
//  We look up x by finding i = hash(x), then checking buckets
//  [i % s, i % (s/2), i % (s/4), ... ] down to % MIN_BUCKETS.
//  If avg load is > 8 just after an addition, we double the
//  number of buckets.

#include "CMap.h"

#ifdef DEBUG
#include "memprofile.h"
#endif

#include "CList.h"

#define MIN_BUCKETS 16
#define MAX_LOAD 2.5


// private function declarations
// =============================

CList *FindWithHash(CMap map, void *needle, int h);
CList *BucketFind(CList *bucket, void *needle, Eq eq);
void DoubleSize(CMap map);
void ReleaseAndFreePair(CMap map, KeyValuePair *pair);
void ReleaseKeyValuePair(void *pair);
void ReleaseBucket(void *bucket);

static CMap currentMap;

// public functions
// ================

CMap CMapNew(Hash hash, Eq eq) {
  CMap map = malloc(sizeof(CMapStruct));
  map->count = 0;
  map->buckets = CArrayNew(MIN_BUCKETS, sizeof(void *));
  CArrayAddZeroedElements(map->buckets, MIN_BUCKETS);
  map->hash = hash;
  map->eq = eq;
  map->keyReleaser = NULL;
  map->valueReleaser = NULL;
  return map;
}

void CMapDelete(CMap map) {
  CMap oldMap = currentMap;
  currentMap = map;
  map->buckets->releaser = ReleaseBucket;
  CArrayDelete(map->buckets);
  free(map);
  currentMap = oldMap;
}

void CMapSet(CMap map, void *key, void *value) {
  int h = map->hash(key);
  CList *entry = FindWithHash(map, key, h);
  if (entry) {
    KeyValuePair *pair = (*entry)->element;
    if (pair->value == value) return;
    if (map->valueReleaser) map->valueReleaser(pair->value);
    pair->value = value;
  } else {
    // New pair.
    KeyValuePair *pair = malloc(sizeof(KeyValuePair));
    pair->key = key;
    pair->value = value;

    double load = (map->count + 1) / (map->buckets->count);
    if (load > MAX_LOAD) DoubleSize(map);

    int n = map->buckets->count;
    int index = ((unsigned int)h) % n;
    CList *bucket = (CList *)CArrayElement(map->buckets, index);
    CListInsert(bucket, pair);
    map->count++;
  }
}

void CMapUnset(CMap map, void *key) {
  int h = map->hash(key);
  CList *entry = FindWithHash(map, key, h);
  if (entry == NULL) return;
  ReleaseAndFreePair(map, (*entry)->element);
  CListRemoveFirst(entry);
  map->count--;
}

KeyValuePair *CMapFind(CMap map, void *needle) {
  CList *entry = FindWithHash(map, needle, map->hash(needle));
  return entry ? (*entry)->element : NULL;
}

typedef struct {
  int bucket;
  CList entry;
} Iterator;

KeyValuePair *CMapBegin(CMap map, void **iterator) {
  Iterator *it = (Iterator *)malloc(sizeof(Iterator));
  *iterator = it;
  it->bucket = -1;
  it->entry = NULL;
  return CMapNext(map, *iterator);
}

KeyValuePair *CMapNext(CMap map, void *iterator) {
  Iterator *it = (Iterator *)iterator;
  while (it->entry == NULL && it->bucket < (map->buckets->count - 1)) {
    it->bucket++;
    it->entry = *(CList *)CArrayElement(map->buckets, it->bucket);
  }
  if (it->entry == NULL && it->bucket == (map->buckets->count - 1)) {
    free(it);
    return NULL;
  }
  void *element = it->entry->element;
  it->entry = it->entry->next;
  return element;
}

// private functions
// =================

CList *FindWithHash(CMap map, void *needle, int h) {
  int n = map->buckets->count;
  int index = ((unsigned int)h) % n;
  CList *bucket = (CList *)CArrayElement(map->buckets, index);
  return BucketFind(bucket, needle, map->eq);
}

typedef struct {
  void *needle;
  Eq eq;
} NeedleInfo;

int pairMatchesNeedleInfo(void *p, void *i) {
  KeyValuePair *pair = (KeyValuePair *)p;
  NeedleInfo *info = (NeedleInfo *)i;
  return info->eq(pair->key, info->needle);
}

CList *BucketFind(CList *bucket, void *needle, Eq eq) {
  NeedleInfo info;
  info.needle = needle;
  info.eq = eq;
  return CListFindEntry(bucket, &info, pairMatchesNeedleInfo);
}

void DoubleSize(CMap map) {
  CArrayAddZeroedElements(map->buckets, map->buckets->count);
  int n = map->buckets->count;
  int index = 0;
  CArrayFor(CList *, bucket, map->buckets) {
    CList *entry = bucket;
    while (*entry) {
      KeyValuePair *pair = (*entry)->element;
      unsigned int h = map->hash(pair->key);
      int bucketIndex = h % n;
      if (bucketIndex == index) {
        entry = &((*entry)->next);
        continue;
      }
      CListRemoveFirst(entry);
      CList *newBucket = (CList *)CArrayElement(map->buckets, bucketIndex);
      CListInsert(newBucket, pair);
    }
    ++index;
    // The last half of the list is all new; no need to look at it.
    if (index >= n / 2) break;
  }
}

void ReleaseAndFreePair(CMap map, KeyValuePair *pair) {
  if (map->keyReleaser) map->keyReleaser(pair->key);
  if (map->valueReleaser) map->valueReleaser(pair->value);
  free(pair);
}

void ReleaseKeyValuePair(void *pair) {
  ReleaseAndFreePair(currentMap, (KeyValuePair *)pair);
}

void ReleaseBucket(void *bucket) {
  CListDeleteAndRelease((CList *)bucket, ReleaseKeyValuePair);
}
