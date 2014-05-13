#include "minunit.h"
#include <hashmap.h>
#include <assert.h>
#include <bstrlib.h>

Hashmap * map = NULL;
static int traverse_called = 0;
struct tagbstring test1 = bsStatic("test data 1");
struct tagbstring test2 = bsStatic("test data 2");
struct tagbstring test3 = bsStatic("test data 3");
struct tagbstring expect1 = bsStatic("THE VALUE 1");
struct tagbstring expect2 = bsStatic("THE VALUE 2");
struct tagbstring expect3 = bsStatic("THE VALUE 3");

/*
 * a successfull callback (that simply dumps the node key value)
 */
static int traverse_good_cb(HashmapNode * node) {
  debug("KEY: %s", bdata((bstring)node->key));
  traverse_called++;
  return 1;
}

/*
 * a failing callback that just bombs out after 2 calls.
 */
static int traverse_fail_cb(HashmapNode * node) {
  debug("KEY: %s", bdata((bstring)node->key));
  traverse_called++;

  if (traverse_called == 2) {
    return 0;
  } else {
    return 1;
  }
}

/*
 * Test that we can create a new Hashmap.
 */
char * test_create() {
  map = Hashmap_create(NULL, NULL);
  mu_assert(map != NULL, "Failed to create map.");
  return NULL;
}

/*
 * Test that we can DESTROY! the Hashmap.
 */
char * test_destroy() {
  Hashmap_destroy(map);
  return NULL;
}


/*
 * Test that we can get and set key/value pairs.
 */
char * test_get_set() {
  int rc = Hashmap_set(map, &test1, &expect1);
  mu_assert(rc == 1, "hashmap_test: failed to set &test1.");
  bstring result = Hashmap_get(map, &test1);
  mu_assert(result == &expect1, "Wrong value for test1.");

  rc = Hashmap_set(map, &test2, &expect2);
  mu_assert(rc == 1, "hashmap_test: failed to set test2.");
  result = Hashmap_get(map, &test2);
  mu_assert(result == &expect2, "hashmap_test: wrong value for test2.");

  rc = Hashmap_set(map, &test3, &expect3);
  mu_assert(rc == 1, "hashmap_test: failed to set test3.");
  result = Hashmap_get(map, &test3);
  mu_assert(result == &expect3, "hashmap_test: wrong value for test3.");

  return NULL;
}

/*
 * Test that we can traverse the map, applying both a successful and failing
 * callback.
 */
char * test_traverse() {
  int rc = Hashmap_traverse(map, traverse_good_cb);
  mu_assert(rc == 1, "hashmap_test: failed to traverse.");
  mu_assert(traverse_called == 3, "hashmap_test: wrong traversal count.");

  traverse_called = 0;
  rc = Hashmap_traverse(map, traverse_fail_cb);
  mu_assert(rc == 0, "hashmap_test: failed to traverse.");
  mu_assert(traverse_called == 2, "hashmap_test: wrong traversal count.");

  return NULL;
}

/*
 * Test that we can remove an element from the map.
 */
char * test_remove() {
  /* test1 */
  bstring deleted = (bstring) Hashmap_remove(map, &test1);
  mu_assert(deleted != NULL, "hashmap_test: removed string was NULL");
  mu_assert(deleted == &expect1, "hashmap_test: removed string wasn't expect1.");
  bstring result = Hashmap_get(map, &test1);
  mu_assert(result == NULL, "hashmap_test: removed node wasn't deleted.");

  /* test2 */
  deleted = (bstring) Hashmap_remove(map, &test2);
  mu_assert(deleted != NULL, "hashmap_test: removed string was NULL");
  mu_assert(deleted == &expect2, "hashmap_test: removed string wasn't expect2.");
  result = Hashmap_get(map, &test2);
  mu_assert(result == NULL, "hashmap_test: removed node wasn't deleted.");

  /* test3 */
  deleted = (bstring) Hashmap_remove(map, &test3);
  mu_assert(deleted != NULL, "hashmap_test: removed string was NULL");
  mu_assert(deleted == &expect3, "hashmap_test: removed string wasn't expect3.");
  result = Hashmap_get(map, &test2);
  mu_assert(result == NULL, "hashmap_test: removed node wasn't deleted.");

  return NULL;
}

/*
 * Run the test suite.
 */
char * all_tests() {
  mu_suite_start();

  mu_run_test(test_create);
  mu_run_test(test_get_set);
  mu_run_test(test_traverse);
  mu_run_test(test_remove);
  mu_run_test(test_destroy);

  return NULL;
}

RUN_TESTS(all_tests);