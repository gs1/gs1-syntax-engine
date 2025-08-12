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
 * @file lint_csum.c
 *
 * @brief The `csum` linter ensures that the data has a valid numeric check
 * digit.
 *
 * @remark The process for validating the standard GS1 numeric check digit is
 * described in the [GS1 General Specifications](https://www.gs1.org/genspecs)
 * section "Standard check digit calculations for GS1 data structures".
 *
 */


#include <assert.h>
#include <string.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Use to ensure that the AI component has a valid numeric check digit.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_INCORRECT_CHECK_DIGIT if the check digit is incorrect.
 * @return #GS1_LINTER_TOO_SHORT_FOR_CHECK_DIGIT if the data is too short.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit
 *         character.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_csum(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	int weight;
	int parity = 0;
	const char *p;
	size_t len;

	assert(data);

	len = strlen(data);

	/*
	 * Data must include at least the check digit.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(*data == '\0'))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_TOO_SHORT_FOR_CHECK_DIGIT,
			0,
			0
		);

	/*
	 * Calculate the sum of the numeric values of the data, excluding the
	 * check digit, weighted by alternating ...3:1:3 values, from right to
	 * left.
	 *
	 * The check digit is valid if its value, when added to the data
	 * checksum, makes the overall sum a multiple of 10.
	 *
	 */
	weight = len % 2 == 0 ? 3 : 1;
	p = data;
	while (*(p+1)) {

		/*
		 * Data must consist of all digits.
		 *
		 */
		if (GS1_LINTER_UNLIKELY(*p < '0' || *p > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				(size_t)(p - data),
				1
			);

		parity += weight * (*p++ - '0');
		weight = 4 - weight;
	}

	/*
	 * Check character must also be a digit.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(*p < '0' || *p > '9'))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_NON_DIGIT_CHARACTER,
			len - 1,
			1
		);
	parity = (10 - parity % 10) % 10;

	if (GS1_LINTER_UNLIKELY(parity + '0' != *p))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_INCORRECT_CHECK_DIGIT,
			len - 1,
			1
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_csum(void)
{

	UNIT_TEST_FAIL(gs1_lint_csum, "", GS1_LINTER_TOO_SHORT_FOR_CHECK_DIGIT, "**");

	UNIT_TEST_PASS(gs1_lint_csum, "02345673");
	UNIT_TEST_FAIL(gs1_lint_csum, "12345673", GS1_LINTER_INCORRECT_CHECK_DIGIT, "1234567*3*");

	UNIT_TEST_PASS(gs1_lint_csum, "416000336108");
	UNIT_TEST_FAIL(gs1_lint_csum, "416000336109", GS1_LINTER_INCORRECT_CHECK_DIGIT, "41600033610*9*");

	UNIT_TEST_PASS(gs1_lint_csum, "1234567890128");
	UNIT_TEST_FAIL(gs1_lint_csum, "1234567890129", GS1_LINTER_INCORRECT_CHECK_DIGIT, "123456789012*9*");

	UNIT_TEST_PASS(gs1_lint_csum, "12345678901231");
	UNIT_TEST_FAIL(gs1_lint_csum, "12345678901232", GS1_LINTER_INCORRECT_CHECK_DIGIT, "1234567890123*2*");

	UNIT_TEST_PASS(gs1_lint_csum, "123456789012345675");
	UNIT_TEST_FAIL(gs1_lint_csum, "123456789012345670", GS1_LINTER_INCORRECT_CHECK_DIGIT, "12345678901234567*0*");

	UNIT_TEST_FAIL(gs1_lint_csum, " 23456789012345675", GS1_LINTER_NON_DIGIT_CHARACTER, "* *23456789012345675");
	UNIT_TEST_FAIL(gs1_lint_csum, "12345678 012345675", GS1_LINTER_NON_DIGIT_CHARACTER, "12345678* *012345675");
	UNIT_TEST_FAIL(gs1_lint_csum, "12345678901234567 ", GS1_LINTER_NON_DIGIT_CHARACTER, "12345678901234567* *");

}

#endif  /* UNIT_TESTS */
