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
 * @file lint_csetnumeric.c
 *
 * @brief The `csetnumeric` linter ensures that the given data contains only
 * digits
 *
 */

#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to validate that an AI component conforms to `N..99` syntax.
 *
 * Note: The length of the component is validated by the framework that calls
 * this function.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit
 *         character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_csetnumeric(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	size_t pos;

	assert(data);


	/*
	 * Validate digits using direct range checking
	 */
	for (pos = 0; pos < data_len; pos++) {
		if (GS1_LINTER_UNLIKELY(data[pos] < '0' || data[pos] > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				pos,
				1
			);
	}

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_csetnumeric(void)
{

	UNIT_TEST_PASS(gs1_lint_csetnumeric, "");
	UNIT_TEST_PASS(gs1_lint_csetnumeric, "0");
	UNIT_TEST_PASS(gs1_lint_csetnumeric, "9");
	UNIT_TEST_PASS(gs1_lint_csetnumeric, "00");
	UNIT_TEST_PASS(gs1_lint_csetnumeric, "09");
	UNIT_TEST_PASS(gs1_lint_csetnumeric, "90");
	UNIT_TEST_PASS(gs1_lint_csetnumeric, "99");
	UNIT_TEST_PASS(gs1_lint_csetnumeric, "0123456789");

	UNIT_TEST_FAIL(gs1_lint_csetnumeric, "/", GS1_LINTER_NON_DIGIT_CHARACTER, "*/*");
	UNIT_TEST_FAIL(gs1_lint_csetnumeric, ":", GS1_LINTER_NON_DIGIT_CHARACTER, "*:*");
	UNIT_TEST_FAIL(gs1_lint_csetnumeric, "a0", GS1_LINTER_NON_DIGIT_CHARACTER, "*a*0");
	UNIT_TEST_FAIL(gs1_lint_csetnumeric, "0a", GS1_LINTER_NON_DIGIT_CHARACTER, "0*a*");

}

#endif  /* UNIT_TESTS */
