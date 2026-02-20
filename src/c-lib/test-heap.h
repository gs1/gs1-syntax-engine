/*
 *  Test heap management: countdown-based allocation failure injection.
 *
 *  When test_alloc_fail_at is > 0, each allocation decrements it.
 *  When it reaches 0, the allocation returns NULL.
 *  When test_alloc_fail_at is 0 (default), all allocations succeed.
 *
 *  The counter is shared across all TUs (extern) so that a test in one
 *  file can trigger allocation failures in functions defined in other
 *  files. The definition lives in gs1encoders-test.c.
 */
#ifndef TEST_HEAP_H
#define TEST_HEAP_H

#include <stdlib.h>

extern int test_alloc_fail_at;

static inline int test_should_fail(void) {
	if (test_alloc_fail_at > 0) {
		if (--test_alloc_fail_at == 0)
			return 1;
	}
	return 0;
}

#define GS1_ENCODERS_CUSTOM_MALLOC(sz) \
	(test_should_fail() ? NULL : malloc(sz))

#define GS1_ENCODERS_CUSTOM_CALLOC(nm, sz) \
	(test_should_fail() ? NULL : calloc(nm, sz))

#define GS1_ENCODERS_CUSTOM_REALLOC(p, sz) \
	(test_should_fail() ? NULL : realloc(p, sz))

#define GS1_ENCODERS_CUSTOM_FREE(p) free(p)

/* Force early realloc in gs1_populateDLkeyQualifiers */
#define DL_KEY_QUALIFIER_INITIAL_CAPACITY 2

#endif
