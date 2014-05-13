#ifndef hashmap_h
#define hashmap_h

#include <stdint.h>
//#include <dynarray.h> // TODO dynamic array :P



typedef int (*Hashmap_compare) (void *a, void *b);
typedef int (*Hashmap_hash) (void *key);

typedef struct Hashmap_ {
  // DArray *buckets;
  Hashmap_compare compare;
  Hashmap_hash hash;
} Hashmap;

typedef struct HashmapNode_ {
  void *key;
  void *data;
  uint32_t hash;
} HashmapNode;




#endif /* hashmap_h */