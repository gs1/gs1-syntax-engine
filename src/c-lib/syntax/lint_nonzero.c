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
 * @file lint_nonzero.c
 *
 * @brief The `nonzero` linter ensures that the given data has a non-zero
 * value.
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that a numeric AI component has a non-zero value.
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
 * @return #GS1_LINTER_ILLEGAL_ZERO_VALUE if the data has a zero value.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains non-digit characters.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_nonzero(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;
	int has_nonzero = 0;

	assert(data);

	for (pos = 0; pos < data_len; pos++) {
		if (GS1_LINTER_UNLIKELY(data[pos] < '0' || data[pos] > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				pos,
				1
			);
		if (data[pos] != '0')
			has_nonzero = 1;
	}

	if (GS1_LINTER_UNLIKELY(!has_nonzero))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_ZERO_VALUE,
			0,
			data_len
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_nonzero(void)
{

	UNIT_TEST_FAIL(gs1_lint_nonzero, "0", GS1_LINTER_ILLEGAL_ZERO_VALUE, "*0*");
	UNIT_TEST_PASS(gs1_lint_nonzero, "1");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "00", GS1_LINTER_ILLEGAL_ZERO_VALUE, "*00*");
	UNIT_TEST_PASS(gs1_lint_nonzero, "01");
	UNIT_TEST_PASS(gs1_lint_nonzero, "10");
	UNIT_TEST_PASS(gs1_lint_nonzero, "11");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "000", GS1_LINTER_ILLEGAL_ZERO_VALUE, "*000*");
	UNIT_TEST_PASS(gs1_lint_nonzero, "001");
	UNIT_TEST_PASS(gs1_lint_nonzero, "010");
	UNIT_TEST_PASS(gs1_lint_nonzero, "011");
	UNIT_TEST_PASS(gs1_lint_nonzero, "100");
	UNIT_TEST_PASS(gs1_lint_nonzero, "101");
	UNIT_TEST_PASS(gs1_lint_nonzero, "110");
	UNIT_TEST_PASS(gs1_lint_nonzero, "111");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "0000", GS1_LINTER_ILLEGAL_ZERO_VALUE, "*0000*");

	UNIT_TEST_PASS(gs1_lint_nonzero, "0009");
	UNIT_TEST_PASS(gs1_lint_nonzero, "9000");

	UNIT_TEST_PASS(gs1_lint_nonzero, "01234567890");

	UNIT_TEST_FAIL(gs1_lint_nonzero, "A999", GS1_LINTER_NON_DIGIT_CHARACTER, "*A*999");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "9A99", GS1_LINTER_NON_DIGIT_CHARACTER, "9*A*99");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "99A9", GS1_LINTER_NON_DIGIT_CHARACTER, "99*A*9");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "999A", GS1_LINTER_NON_DIGIT_CHARACTER, "999*A*");

	UNIT_TEST_FAIL(gs1_lint_nonzero, "", GS1_LINTER_ILLEGAL_ZERO_VALUE, "**");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "00", GS1_LINTER_ILLEGAL_ZERO_VALUE, "*00*");
	UNIT_TEST_FAIL(gs1_lint_nonzero, "000", GS1_LINTER_ILLEGAL_ZERO_VALUE, "*000*");

}

#endif  /* UNIT_TESTS */
