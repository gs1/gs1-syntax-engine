/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2024-2026 GS1 AISBL.
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
 * @file lint_hasnondigit.c
 *
 * @brief The `hasnondigit` linter ensures that the given data contains a
 * non-digit character.
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component contains a non-digit character.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER if the data does not contain a non-digit character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hasnondigit(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;

	assert(data);

	/*
	 * Data must not be all numeric
	 *
	 */
	for (pos = 0; pos < data_len; pos++)
		if (data[pos] < '0' || data[pos] > '9')
			GS1_LINTER_RETURN_OK;

	GS1_LINTER_RETURN_ERROR(
		GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER,
		0,
		data_len
	);

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_hasnondigit(void)
{

	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "**");

	UNIT_TEST_PASS(gs1_lint_hasnondigit, "/");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "0", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*0*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "1", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*1*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "2", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*2*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "3", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*3*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "4", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*4*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "5", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*5*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "6", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*6*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "7", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*7*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "8", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*8*");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "9", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*9*");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, ":");

	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "0123456789", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*0123456789*");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "/123456789");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "012345678/");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, ":123456789");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "012345678:");

	UNIT_TEST_PASS(gs1_lint_hasnondigit, "AA");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "A1");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "1A");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "11", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*11*");

	UNIT_TEST_PASS(gs1_lint_hasnondigit, "AAA");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "1AA");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "A1A");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "AA1");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "11A");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "A11");
	UNIT_TEST_PASS(gs1_lint_hasnondigit, "1A1");
	UNIT_TEST_FAIL(gs1_lint_hasnondigit, "111", GS1_LINTER_REQUIRES_NON_DIGIT_CHARACTER, "*111*");

}

#endif  /* UNIT_TESTS */
