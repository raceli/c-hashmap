#include "hashmap.h"
#include <dbg.h>
#include <bstrlib.h>

/*
 * Default comparisoin function that compares two keys, which by default
 * are simply bstrings.
 */
static int default_compare(void * a, void * b) {
  return bstrcmp((bstring)a, (bstring)b);
}

/*
 * Simple Bob Jenkins's hash algorithm taken from the wikipedia description.
 *
 * http://en.wikipedia.org/wiki/Jenkins_hash_function
 */
static uint32_t default_hash(void * a) {
  size_t len = blength((bstring)a);
  char * key = bdata((bstring)a);
  uint32_t hash = 0;
  uint32_t i = 0;

  for (hash = i = 0; i < len; ++i) {
    hash += key[i];
    hash += (hash << 10);
    hash ^= (hash >> 6);
  }

  hash += (hash << 3);
  hash ^= (hash >> 11);
  hash += (hash << 15);

  return hash;
}

/*
 * Create a new Hashmap. A comparison and hashing function can be supplied,
 * or NULL, in which case the default bstring-based functions will be used.
 */
hashmap_t * hashmap_create(hashmap_compare compare, hashmap_hash hash) {
  hashmap_t * map = calloc(1, sizeof(hashmap_t));
  check_mem(map);

  /* if no compare / hash function are provided we supply default ones */
  map->compare = compare == NULL ? default_compare : compare;
  map->hash = hash == NULL ? default_hash : hash;

  /* create buckets array */
  map->buckets = darray_create(sizeof(darray_t *), DEFAULT_NUMBER_OF_BUCKETS);
  map->buckets->size = map->buckets->max; // fake out expanding it
  check_mem(map->buckets);

  return map;
error:
  if (map) {
    hashmap_destroy(map);
  }
  return NULL;
}

/*
 * Destroy the provided Hashmap.
 */
void hashmap_destroy(hashmap_t * map) {
  int b_index = 0;

  if (map) {
    if (map->buckets) {
      /*
       * iterate the map's buckets, which contain a dynamic array of bucket
       * items, and free them all!
       */
      for (b_index = 0; b_index < darray_size(map->buckets); b_index++) {
        /* Buckets themselves are dynamic arrays of "things" stored at the key */
        darray_t * bucket = darray_get(map->buckets, b_index);
        if (bucket) {
          darray_clear_destroy(bucket);
        }
      }
      darray_destroy(map->buckets);
    }

    free(map);
  }
}

/*
 * Internal function: Create a new Hashmap node.
 */
static inline hashmap_node_t * hashmap_node_create(
  int hash,
  void * key,
  void * data) {

  /* allocate memory for the new node */
  hashmap_node_t * node = calloc(1, sizeof(hashmap_node_t));
  check_mem(node);

  node->key = key;
  node->data = data;
  node->hash = hash;

  return node;

error:
  return NULL;
}


/*
 * Internal function: Find the bucket within the map provided with the key
 * provided.
 *
 * If the create flag is set to 1, a new bucket will be created at the hash
 * value's index.
 *
 * Store the hash value in the hash_out value.
 *
 * Returns NULL if no bucket is found, and the create flag is 0, otherwise
 * returns a pointer to the bucket dynamic array for the given key value.
 */
static inline darray_t * hashmap_find_bucket(
  hashmap_t * map,
  void * key,
  int create,
  uint32_t * hash_out) {

  /* get the hash value for the key provided */
  uint32_t hash = map->hash(key);
  int bucket_index = hash % DEFAULT_NUMBER_OF_BUCKETS;

  /* check that the bucket index is valid */
  check(
    bucket_index >= 0,
    "hashmap_find_bucket: invalid bucket found: %d",
    bucket_index
  );

  /* store the hash value so it can be  used by the caller */
  *hash_out = hash;

  /* get the bucket at the index determined by the hash */
  darray_t * bucket = darray_get(map->buckets, bucket_index);

  /*
   * if the bucket doesn't exist, and we have a create flag set...
   * create a new one
   */
  if (!bucket && create) {
    bucket = darray_create(sizeof(void *), DEFAULT_NUMBER_OF_BUCKETS);
    check_mem(bucket);
    darray_set(map->buckets, bucket_index, bucket);
  }

  return bucket;
error:
  return NULL;
}

/*
 * Set the key/value pair in the Hashmap provided.
 *
 * Returns 1 if the key/value pair was successfully stored.
 */
int hashmap_set(hashmap_t * map, void * key, void * data) {
  /*
   * get the bucket that maps to this key, based on its hash
   * (which we also store)
   */
  uint32_t hash = 0;
  darray_t * bucket = hashmap_find_bucket(map, key, 1, &hash);
  check(
    bucket,
    "hashmap_set: could not create or get bucket for the key provided."
  );

  /* create a new node that stores the key/value/hash info for this new item */
  hashmap_node_t * node = hashmap_node_create(hash, key, data);
  check_mem(node);

  /*
   * push the node onto the dynamic array for this key's hash index -- note that
   * it is possible for more than one node to be stored in the same bucket
   */
  darray_push(bucket, node);

  return 1;
error:
  return 0;
}

/*
 * Internal function: get the node for the given hash, within the bucket
 * provided that has the key provided.
 *
 * Returns the index of the node if it is found, or -1 on failure.
 */
static inline int hashmap_get_node(
  hashmap_t * map,
  uint32_t hash,
  darray_t * bucket,
  void * key) {

  int index = 0;

  /* search for the node */
  for (index = 0; index < darray_size(bucket); index++) {
    debug("hashmap_get_node: searching %d...", index);

    hashmap_node_t * node = darray_get(bucket, index);

    /* if we have a hit, return the index */
    if (node->hash == hash && map->compare(node->key, key) == 0) {
      return index;
    }
  }

  /* if we fall through, return -1 to indicate we did not find the node */
  return -1;
}

/*
 * Get the value for the key provided.
 *
 * Returns the value if it is found, or NULL if it doesn't exist in the map.
 */
void * hashmap_get(hashmap_t * map, void * key) {
  /*
   * get the bucket that maps to this key, based on its hash
   * (which we also store)
   */
  uint32_t hash = 0;
  darray_t * bucket = hashmap_find_bucket(map, key, 0, &hash);

  check(bucket != NULL, "hashmap_get: failed to get or create bucket for key.");

  /* find this particular node in the bucket dynamic array */
  int node_index = hashmap_get_node(map, hash, bucket, key);

  /* couldn't find it... doesn't exist! */
  if (node_index == -1) { return NULL; }

  /* get the node stored at the node index within the bucket for this key */
  hashmap_node_t * node = darray_get(bucket, node_index);

  check(
    node != NULL,
    "hashmap_get: failed to get node from bucket when it should exist."
  );

  return node->data;
error:
  return NULL;
}


/*
 * Traverse all values in the Hashmap and apply the traversal function to the
 * data.
 *
 * Returns 1 if the traversal function was successfully applied to all elements,
 * or 0 otherwise.
 */
int hashmap_traverse(hashmap_t * map, hashmap_traverse_cb traverse_cb) {
  int b_index = 0;
  int item_index = 0;

  for (b_index = 0; b_index < darray_size(map->buckets); b_index++) {
    darray_t * bucket = darray_get(map->buckets, b_index);

    if (bucket) {
      for (item_index = 0; item_index < darray_size(bucket); item_index++) {
        hashmap_node_t * node = darray_get(bucket, item_index);

        /*
         * if the traversal function "fails", short circuit the loop, bail out
         * returning 0
         */
        if ( traverse_cb(node) != 1) {
          return 0;
        }
      }
    }
  }

  return 1;
}

/*
 * Remove the value associated with the key provided in the Hashmap.
 *
 * Returns the value removed from the Hashmap, or NULL if the key doesn't
 * exist in this map.
 */
void * hashmap_remove(hashmap_t * map, void * key) {

  /*
   * get the bucket that maps to this key, based on its hash
   * (which we also store)
   */
  uint32_t hash = 0;
  darray_t * bucket = hashmap_find_bucket(map, key, 0, &hash);

  /* check that the bucket index is valid */
  check(
    bucket != NULL,
    "hashmap_remove: failed to get or create bucket for key."
  );

  int node_index = hashmap_get_node(map, hash, bucket, key);

  /* nothing to remove (that key didn't exist in our map) */
  if (node_index == -1) {
    log_info("hashmap_remove: key %s did not exist in our map.", key);
    return NULL;
  }

  /* get the node at the index of this key, grab its data, then free the node */
  hashmap_node_t * node = darray_get(bucket, node_index);
  void * data = node->data;

  /*
   * here we just use a trick to "shrink" the dynamic array, which is... pop
   * the last value in the bucket, if the node we are removing is the one at the
   * end of the Hashmap, all good, else, we set the value of the node we just
   * removed to the ending of the bucket list (which essentially swaps it down
   * and shrinking the size of the array by 1).
   */
  hashmap_node_t * ending = darray_pop(bucket);
  if (ending != node) {
    darray_set(bucket, node_index, ending);
  }

  /* free the hashmap node and return it's data */
  free(node);
  return data;
error:
  return NULL;
}


