/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2025 GS1 AISBL
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
 * @file lint_gcppos2.c
 *
 * @brief The `gcppos2` linter checks whether an input starts with a GS1 Company
 * Prefix ("GCP") from its second character position.
 *
 * @remark The GCP is defined in the [GS1 General
 * Specifications](https://www.gs1.org/genspecs) section "GS1 Company Prefix".
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component starts with a GCP from its second
 * position.
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
 * @return #GS1_LINTER_TOO_SHORT_FOR_GCP if the data is too short to start
 *         with a GCP.
 * @return #GS1_LINTER_GCP_DATASOURCE_OFFLINE if the user-provided GCP lookup
 *         source is unavailable and this should result in the linting failing.
 *         [IMPLEMENTATION SPECIFIC]
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_gcppos2(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t len;
	gs1_lint_err_t ret;

	assert(data);

	len = strlen(data);

	if (len < 2)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_TOO_SHORT_FOR_GCP,
			0,
			len
		);

	ret = gs1_lint_gcppos1(data + 1, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_TOO_SHORT_FOR_GCP ||
	       ret == GS1_LINTER_INVALID_GCP_PREFIX ||
	       ret == GS1_LINTER_GCP_DATASOURCE_OFFLINE);

	if (ret != GS1_LINTER_OK)
		GS1_LINTER_RETURN_ERROR(
			ret,
			*err_pos + 1,
			*err_len
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

#ifndef GCP_MIN_LENGTH
#define GCP_MIN_LENGTH 4  // Keep aligned with instance in lint_gcppos1.c
#endif

void test_lint_gcppos2(void)
{

	char data[GCP_MIN_LENGTH + 3], expect[GCP_MIN_LENGTH + 5];
	int i;

	/*
	 *        |----- Indicates end of minimum-length GCP
	 *        v
	 *  I0...45|6 => Good
	 *  I0...45|  => Good
	 */
	data[0] = 'I';
	for (i = 1; i < GCP_MIN_LENGTH + 2; i++)
		data[i] = (char)('0' + (i-1)%10);
	data[i--] = '\0';
	UNIT_TEST_PASS(gs1_lint_gcppos2, data);	// One more than minimum length
	data[i--] = '\0';
	UNIT_TEST_PASS(gs1_lint_gcppos2, data);	// Minimum length

	/*
	 *        |----- Indicates end of minimum-length GCP
	 *        v
	 *  I01234 |  => Short; I*01234*
	 *  I0123  |  => Short; I*0123*
	 *  I012   |  => Short; I*012*
	 *  I01    |  => Short; I*01*
	 *  I0     |  => Short; I*0*
	 */
	while (i >= 2) {
		data[i--] = '\0';
		strcpy(expect, "I*");
		strcat(expect, data+1);
		strcat(expect, "*");
		UNIT_TEST_FAIL(gs1_lint_gcppos2, data, GS1_LINTER_TOO_SHORT_FOR_GCP, expect);
	}

	/*
	 *        |----- Indicates end of minimum-length GCP
	 *        v
	 *  I      |  => Short; *I*
	 *  ""     |  => Short; **
	 */
	while (i >= 0) {
		data[i--] = '\0';
		strcpy(expect, "*");
		strcat(expect, data);
		strcat(expect, "*");
		UNIT_TEST_FAIL(gs1_lint_gcppos2, data, GS1_LINTER_TOO_SHORT_FOR_GCP, expect);
	}

	/*
	 *  I012345|A => Good
	 */
	data[0] = 'I';
	for (i = 1; i < GCP_MIN_LENGTH + 1; i++)
		data[i] = (char)('0' + (i-1)%10);
	data[i++] = 'A';
	data[i] = '\0';
	UNIT_TEST_PASS(gs1_lint_gcppos2, data);	// Minimum-length GCP is all numeric

	/*
	 *        |----- Indicates end of minimum-length GCP
	 *        v
	 *  I01234A|A => Bad; I01234*A*A
	 *  I0123AA|A => Bad; I0123*A*AA
	 *  I012AAA|A => Bad; I012*A*AAA
	 *  I01AAAA|A => Bad; I01*A*AAAA
	 *  I0AAAAA|A => Bad; I0*A*AAAAA
	 *  IAAAAAA|A => Bad; I*A*AAAAAA
	 */
	strcpy(expect, data);
	expect[i+2] = '\0';
	i -= 2;
	while (i >= 1) {
		data[i] = 'A';
		memcpy(&expect[i--], "*A*A", 4);
		UNIT_TEST_FAIL(gs1_lint_gcppos2, data, GS1_LINTER_INVALID_GCP_PREFIX, expect);
	}

}

#endif  /* UNIT_TESTS */
