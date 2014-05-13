#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include <dbg.h>
#include <stdlib.h>

#define mu_suite_start() char *message = NULL

#define mu_assert(test, message) if (!(test)) { log_err("\t", message); return message; }
#define mu_run_test(test) log_info("\tTest: [%s]", #test); \
  message = test(); tests_run++; if (message) return message;

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    argc = 1; \
    log_info("Test Suite: [%s]", argv[0]);\
    char *result = name();\
    if (result != 0) {\
      log_info("Failed: %s", result);\
    }\
    else {\
      log_info("All Tests Passed!");\
    }\
    log_info("Tests run: %d", tests_run);\
    exit(result != 0);\
  }


int tests_run;

#endif