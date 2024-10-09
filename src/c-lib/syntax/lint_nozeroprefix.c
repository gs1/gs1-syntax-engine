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
 * @file lint_nozeroprefix.c
 *
 * @brief The `nozeroprefix` linter ensures that the given data does not start with a zero.
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that a numeric AI component does not start with zero.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_ILLEGAL_ZERO_PREFIX if the data starts with zero.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains non-digit characters.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_nozeroprefix(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;

	assert(data);

	/*
	 * Data must be all numeric
	 *
	 */
	if ((pos = strspn(data, "0123456789")) != strlen(data))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_NON_DIGIT_CHARACTER,
			pos,
			1
		);

	/*
	 * Data must not start with a zero
	 *
	 */
	if (*data == '0')
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_ZERO_PREFIX,
			0,
			1
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_nozeroprefix(void)
{

	UNIT_TEST_PASS(gs1_lint_nozeroprefix, "");
	UNIT_TEST_PASS(gs1_lint_nozeroprefix, "1");
	UNIT_TEST_PASS(gs1_lint_nozeroprefix, "10");
	UNIT_TEST_PASS(gs1_lint_nozeroprefix, "210");
	UNIT_TEST_PASS(gs1_lint_nozeroprefix, "321");

	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "0", GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*0*");
	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "01", GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*0*1");
	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "012", GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*0*12");
	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "0123", GS1_LINTER_ILLEGAL_ZERO_PREFIX, "*0*123");

	UNIT_TEST_PASS(gs1_lint_nozeroprefix, "1234567890");

	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "A999", GS1_LINTER_NON_DIGIT_CHARACTER, "*A*999");
	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "9A99", GS1_LINTER_NON_DIGIT_CHARACTER, "9*A*99");
	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "99A9", GS1_LINTER_NON_DIGIT_CHARACTER, "99*A*9");
	UNIT_TEST_FAIL(gs1_lint_nozeroprefix, "999A", GS1_LINTER_NON_DIGIT_CHARACTER, "999*A*");

}

#endif  /* UNIT_TESTS */
