/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL.
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
 * @file lint_zero.c
 *
 * @brief The `zero` linter ensures that the given data has a zero value.
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component has a zero value.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_NOT_ZERO if the data does not contain a zero value.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_zero(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;

	assert(data);

	if (GS1_LINTER_UNLIKELY(data_len == 0))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_NOT_ZERO,
			0,
			0
		);

	/*
	 * Data must not contain a non-zero character
	 *
	 */
	for (pos = 0; pos < data_len; pos++) {
		if (GS1_LINTER_UNLIKELY(data[pos] != '0')) {
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NOT_ZERO,
				0,
				data_len
			);
		}
	}

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_zero(void)
{

	UNIT_TEST_PASS(gs1_lint_zero, "0");
	UNIT_TEST_PASS(gs1_lint_zero, "00");
	UNIT_TEST_PASS(gs1_lint_zero, "000");

	UNIT_TEST_FAIL(gs1_lint_zero, "", GS1_LINTER_NOT_ZERO, "**");
	UNIT_TEST_FAIL(gs1_lint_zero, "1", GS1_LINTER_NOT_ZERO, "*1*");
	UNIT_TEST_FAIL(gs1_lint_zero, "01", GS1_LINTER_NOT_ZERO, "*01*");
	UNIT_TEST_FAIL(gs1_lint_zero, "10", GS1_LINTER_NOT_ZERO, "*10*");
	UNIT_TEST_FAIL(gs1_lint_zero, "001", GS1_LINTER_NOT_ZERO, "*001*");
	UNIT_TEST_FAIL(gs1_lint_zero, "010", GS1_LINTER_NOT_ZERO, "*010*");
	UNIT_TEST_FAIL(gs1_lint_zero, "011", GS1_LINTER_NOT_ZERO, "*011*");
	UNIT_TEST_FAIL(gs1_lint_zero, "100", GS1_LINTER_NOT_ZERO, "*100*");
	UNIT_TEST_FAIL(gs1_lint_zero, "101", GS1_LINTER_NOT_ZERO, "*101*");
	UNIT_TEST_FAIL(gs1_lint_zero, "110", GS1_LINTER_NOT_ZERO, "*110*");

	UNIT_TEST_FAIL(gs1_lint_zero, "X", GS1_LINTER_NOT_ZERO, "*X*");
	UNIT_TEST_FAIL(gs1_lint_zero, "0X", GS1_LINTER_NOT_ZERO, "*0X*");
	UNIT_TEST_FAIL(gs1_lint_zero, "X0", GS1_LINTER_NOT_ZERO, "*X0*");

}

#endif  /* UNIT_TESTS */
