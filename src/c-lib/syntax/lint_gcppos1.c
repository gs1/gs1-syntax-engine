/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2026 GS1 AISBL
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
 * @file lint_gcppos1.c
 *
 * @brief The `gcppos1` linter checks whether an input starts with a GS1 Company
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
 *       API is defined by the user-provided GCP lookup service using the first
 *       and second arguments, then using the result must assign to third and fourth
 *       (output) arguments as follows:
 *         - `valid`: Set to 1 if the GCP is valid (or should be treated as
 *           such). Otherwise 0.
 *         - `offline`: Set to 1 to indicate that the GCP data source is
 *           offline and the linter must fail.  Otherwise 0.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INVALID_GCP_PREFIX if the data does not start with a GCP.
 * @return #GS1_LINTER_TOO_SHORT_FOR_GCP if the data is too short to start
 *         with a GCP.
 * @return #GS1_LINTER_GCP_DATASOURCE_OFFLINE if the user-provided GCP lookup
 *         source is unavailable and this should result in the linting failing.
 *         [IMPLEMENTATION SPECIFIC]
 *
 * @note When GCP lookup service is provided by the user, it will be called
 *       with strings whose prefix may be one of (1) a regular GCP, (2) a UPC
 *       Company Code normalised to a regular GCP (by prefixing "0"), or (3) a
 *       GS1-8 Prefix normalised to a regular GCP by prefixing "00000".
 * @note The choice of whether or not to assign `offline = 1` in order to
 *       return #GS1_LINTER_GCP_DATASOURCE_OFFLINE when a user-provided GCP
 *       lookup service is unavailable is implementation specific. It depends
 *       on whether the desired behaviour is to fail open (return
 *       #GS1_LINTER_OK) or to fail closed (return
 *       #GS1_LINTER_GCP_DATASOURCE_OFFLINE). In the case that it is desirable
 *       to fail open then in addition to assigning `offline = 0` it is
 *       necessary to assign `valid = 1` to avoid returning
 *       #GS1_LINTER_INVALID_GCP_PREFIX, which would be misleading in the event
 *       of service outage.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_gcppos1(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t i;

	assert(data);

	/*
	 * Any character within the minimum-length GCP prefix that is outside
	 * the range '0' to '9' is illegal.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data_len < GCP_MIN_LENGTH))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_TOO_SHORT_FOR_GCP,
			0,
			data_len
		);

	for (i = 0; i < GCP_MIN_LENGTH; i++) {
		if (GS1_LINTER_UNLIKELY(data[i] < '0' || data[i] > '9'))
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
	int valid, offline;
	GS1_LINTER_CUSTOM_GCP_LOOKUP(data, data_len, valid, offline);
	if (GS1_LINTER_UNLIKELY(offline))
		GS1_LINTER_RETURN_ERROR(GS1_LINTER_GCP_DATASOURCE_OFFLINE, 0, 0);
	else if (GS1_LINTER_UNLIKELY(!valid))
		GS1_LINTER_RETURN_ERROR(GS1_LINTER_INVALID_GCP_PREFIX, 0, 0);
}
#endif

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_gcppos1(void)
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
	UNIT_TEST_PASS(gs1_lint_gcppos1, data);	// One more than minimum length
	data[i--] = '\0';
	UNIT_TEST_PASS(gs1_lint_gcppos1, data);	// Minimum length

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
	for (i = GCP_MIN_LENGTH-1; i >= 0; i--) {
		data[i] = '\0';
		expect[0] = '*';
		memcpy(expect+1, data, (size_t)i);
		expect[i+1] = '*';
		expect[i+2] = '\0';
		UNIT_TEST_FAIL(gs1_lint_gcppos1, data, GS1_LINTER_TOO_SHORT_FOR_GCP, expect);
	}

	/*
	 *  012345|A => Good
	 */
	for (i = 0; i < GCP_MIN_LENGTH; i++)
		data[i] = (char)('0' + i%10);
	data[i++] = 'A';
	data[i] = '\0';
	UNIT_TEST_PASS(gs1_lint_gcppos1, data);	// Minimum-length GCP is all numeric

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
	memcpy(expect, data, (size_t)GCP_MIN_LENGTH);
	expect[GCP_MIN_LENGTH+3] = '\0';
	for (i = GCP_MIN_LENGTH - 1; i >= 0; i--) {
		data[i] = 'A';
		memcpy(&expect[i], "*A*A", 4);
		UNIT_TEST_FAIL(gs1_lint_gcppos1, data, GS1_LINTER_INVALID_GCP_PREFIX, expect);
	}

}

#endif  /* UNIT_TESTS */
