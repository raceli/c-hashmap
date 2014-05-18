#ifndef hashmap_h
#define hashmap_h

#include <stdint.h>
#include <darray.h>

#define DEFAULT_NUMBER_OF_BUCKETS 100

/*
 * Compare function type definition.
 */
typedef int (*hashmap_compare) (void * a, void * b);

/*
 * Hash function type definition.
 */
typedef uint32_t (*hashmap_hash) (void * key);

/*
 * Main HashMap structure
 */
typedef struct hashmap {
  darray_t * buckets;
  hashmap_compare compare;
  hashmap_hash hash;
} hashmap_t;

/*
 * A Node in a HashMap
 */
typedef struct hashmap_node {
  void * key;
  void * data;
  uint32_t hash;
} hashmap_node_t;

/*
 * Traversal callback function type definition.
 *
 * Any implementations should return 1 on success, and 0 otherwise.
 */
typedef int (*hashmap_traverse_cb) (hashmap_node_t * node);

/*******************************************************************************
 *
 * HashMap creation/management functions
 *
 ******************************************************************************/

hashmap_t * hashmap_create(hashmap_compare compare, hashmap_hash);

void hashmap_destroy(hashmap_t * map);

int hashmap_set(hashmap_t * map, void * key, void * data);

void * hashmap_get(hashmap_t * map, void * key);

int hashmap_traverse(hashmap_t * map, hashmap_traverse_cb traverse_cb);

void * hashmap_remove(hashmap_t * map, void * key);

#endif /* hashmap_h */