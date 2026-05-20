/*
 *  Test GCP lookup: controllable GCP validation for unit testing.
 *
 *  Injected via -DGS1_LINTER_CUSTOM_GCP_LOOKUP_H=test-gcp-lookup.h
 *
 *  Controls:
 *    test_gcp_lookup_result == 0: valid (default)
 *    test_gcp_lookup_result == 1: invalid GCP prefix
 *    test_gcp_lookup_result == 2: data source offline
 *
 *  When test_gcp_lookup_countdown > 0, each call decrements the
 *  countdown and returns valid.  The configured result only takes
 *  effect once the countdown reaches 0.
 *
 *  The variables are shared across all TUs (extern) so that a test in
 *  one file can trigger lookup failures in functions defined in other
 *  files.  The definitions live in gs1syntaxdictionary-test.c.
 */
#ifndef TEST_GCP_LOOKUP_H
#define TEST_GCP_LOOKUP_H

extern int test_gcp_lookup_result;
extern int test_gcp_lookup_countdown;

#define GS1_LINTER_CUSTOM_GCP_LOOKUP(data, data_len, valid, offline) do {	\
	(void)(data); (void)(data_len);						\
	if (test_gcp_lookup_countdown > 0) {					\
		test_gcp_lookup_countdown--;					\
		valid = 1; offline = 0;						\
	} else {								\
		valid   = (test_gcp_lookup_result == 0) ? 1 : 0;		\
		offline = (test_gcp_lookup_result == 2) ? 1 : 0;		\
	}									\
} while (0)

#endif
