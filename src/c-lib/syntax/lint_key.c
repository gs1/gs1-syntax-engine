/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 * @file lint_key.c
 *
 * @brief The `key` linter checks whether an input starts with a GS1 Company
 * Prefix ("GCP").
 *
 * @remark The GCP is defined in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) section "GS1 Company Prefix".
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/*
 * Include a header containing a replacement lookup function, if we are told
 * to.
 *
 */
#ifdef GS1_LINTER_CUSTOM_GCP_LOOKUP_H
#define xstr(s) str(s)
#define str(s) #s
#include xstr(GS1_LINTER_CUSTOM_GCP_LOOKUP_H)
#endif


#ifndef GCP_MIN_LENGTH
#define GCP_MIN_LENGTH 4  ///< Currently the shortest GS1 Company Prefix is four digits.
#endif


/**
 * Used to ensure that an AI component starts with a GCP.
 *
 * @note To enable this linter to hook into a GS1 Company Prefix lookup service
 *       containing a record of current GCP allocations (provided by the user)
 *       the GS1_LINTER_CUSTOM_GCP_LOOKUP_H macro may be set to the name of a
 *       header file to be included that defines a custom
 *       `GS1_LINTER_CUSTOM_GCP_LOOKUP` macro.
 * @note If provided, the GS1_LINTER_CUSTOM_GCP_LOOKUP macro shall invoke whatever
 *       API is defined by the user-provided GCP lookup service, then using
 *       the result must assign to locally-scoped variables as follows:
 *         - `valid`: Set to 1 if the GCP is valid (or should be treated as
 *           such). Otherwise 0.
 *         - `offline`: Set to 1 to indicate that the GCP data source is
 *           offline and the linter must fail.  Otherwise 0.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_GCP_PREFIX if the data does not start with a GCP.
 * @return #GS1_LINTER_TOO_SHORT_FOR_KEY if the data is too short to start
 *         with a GCP.
 * @return #GS1_LINTER_GCP_DATASOURCE_OFFLINE if the user-provided GCP lookup
 *         source is unavailable and this should result in the linting failing.
 *         [IMPLEMENTATION SPECIFIC]
 *
 * @note The choice of whether or not to assign `offline = 1` in order to
 * return #GS1_LINTER_GCP_DATASOURCE_OFFLINE when a user-provided GCP lookup
 * service is unavailable is implementation specific. It depends on whether the
 * desired behaviour is to fail open (return #GS1_LINTER_OK) or to fail closed
 * (return #GS1_LINTER_GCP_DATASOURCE_OFFLINE). In the case that it is
 * desirable to fail open then in addition to assigning `offline = 0` it is
 * necessary to assign `valid = 1` to avoid returning
 * #GS1_LINTER_INVALID_GCP_PREFIX, which would be misleading in the event of
 * service outage.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_key(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t i, len;

	assert(data);

	len = strlen(data);

	/*
	 * The current minimum GCP length is defined by GCP_MIN_LENGTH.
	 *
	 */
	if (len < GCP_MIN_LENGTH)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_TOO_SHORT_FOR_KEY,
			0,
			len
		);

	/*
	 * Any character within the minimum-length GCP prefix that is outside
	 * the range '0' to '9' is illegal.
	 *
	 */
	for (i = 0; i < GCP_MIN_LENGTH; i++) {
		if (data[i] < '0' || data[i] > '9')
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_INVALID_GCP_PREFIX,
				i,
				1
			);
	}

	/*
	 * Call the custom GCP lookup routine if one has been provided.
	 *
	 */
#ifdef GS1_LINTER_CUSTOM_GCP_LOOKUP
{
	int valid = 0, offline = 0;
	GS1_LINTER_CUSTOM_GCP_LOOKUP(data);
	if (offline)
		GS1_LINTER_RETURN_ERROR(GS1_LINTER_GCP_DATASOURCE_OFFLINE, 0, 0);
	else if (!valid)
		GS1_LINTER_RETURN_ERROR(GS1_LINTER_INVALID_GCP_PREFIX, 0, 0);
}
#endif

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_key(void)
{

	char data[GCP_MIN_LENGTH + 2], expect[GCP_MIN_LENGTH + 4];
	int i;

	/*
	 *        |----- Indicates end of minimum-length GCP
	 *        v
	 *  0...45|6 => Good
	 *  0...45|  => Good
	 */
	for (i = 0; i < GCP_MIN_LENGTH + 1; i++)
		data[i] = (char)('0' + i%10);
	data[i--] = '\0';
	UNIT_TEST_PASS(gs1_lint_key, data);	// One more than minimum length
	data[i--] = '\0';
	UNIT_TEST_PASS(gs1_lint_key, data);	// Minimum length

	/*
	 *        |----- Indicates end of minimum-length GCP
	 *        v
	 *  01234 |  => Short; *01234*
	 *  0123  |  => Short; *0123*
	 *  012   |  => Short; *012*
	 *  01    |  => Short; *01*
	 *  0     |  => Short; *0*
	 *  ""    |  => Short; **
	 */
	while (i >= 0) {
		data[i--] = '\0';
		strcpy(expect, "*");
		strcat(expect, data);
		strcat(expect, "*");
		UNIT_TEST_FAIL(gs1_lint_key, data, GS1_LINTER_TOO_SHORT_FOR_KEY, expect);
	}

	/*
	 *  012345|A => Good
	 */
	for (i = 0; i < GCP_MIN_LENGTH; i++)
		data[i] = (char)('0' + i%10);
	data[i++] = 'A';
	data[i] = '\0';
	UNIT_TEST_PASS(gs1_lint_key, data);	// Minimum-length GCP is all numeric

	/*
	 *        |----- Indicates end of minimum-length GCP
	 *        v
	 *  01234A|A => Bad; 01234*A*A
	 *  0123AA|A => Bad; 0123*A*AA
	 *  012AAA|A => Bad; 012*A*AAA
	 *  01AAAA|A => Bad; 01*A*AAAA
	 *  0AAAAA|A => Bad; 0*A*AAAAA
	 *  AAAAAA|A => Bad; *A*AAAAAA
	 */
	strcpy(expect, data);
	expect[i+2] = '\0';
	i -= 2;
	while (i >= 0) {
		data[i] = 'A';
		memcpy(&expect[i--], "*A*A", 4);
		UNIT_TEST_FAIL(gs1_lint_key, data, GS1_LINTER_INVALID_GCP_PREFIX, expect);
	}

}

#endif  /* UNIT_TESTS */
