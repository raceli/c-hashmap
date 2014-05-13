#ifndef hashmap_h
#define hashmap_h

#include <stdint.h>
#include <darray.h>

#define DEFAULT_NUMBER_OF_BUCKETS 100

/*
 * Compare function type definition.
 */
typedef int (*Hashmap_compare) (void * a, void * b);

/*
 * Hash function type definition.
 */
typedef uint32_t (*Hashmap_hash) (void * key);

/*
 * Main HashMap structure
 */
typedef struct Hashmap_ {
  DArray * buckets;
  Hashmap_compare compare;
  Hashmap_hash hash;
} Hashmap;

/*
 * A Node in a HashMap
 */
typedef struct HashmapNode_ {
  void * key;
  void * data;
  uint32_t hash;
} HashmapNode;

/*
 * Traversal callback function type definition.
 *
 * Any implementations should return 1 on success, and 0 otherwise.
 */
typedef int (*Hashmap_traverse_cb) (HashmapNode * node);

/*******************************************************************************
 *
 * HashMap creation/management functions
 *
 ******************************************************************************/

Hashmap * Hashmap_create(Hashmap_compare compare, Hashmap_hash);

void Hashmap_destroy(Hashmap * map);

int Hashmap_set(Hashmap * map, void * key, void * data);

void * Hashmap_get(Hashmap * map, void * key);

int Hashmap_traverse(Hashmap * map, Hashmap_traverse_cb traverse_cb);

void * Hashmap_remove(Hashmap * map, void * key);

#endif /* hashmap_h */