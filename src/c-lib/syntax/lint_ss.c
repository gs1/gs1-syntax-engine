/*
 * GS1 Barcode Syntax Dictionary. Copyright (c) 2022-2026 GS1 AISBL.
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
 * @file lint_ss.c
 *
 * @brief The `ss` linter ensures that the given data is a meaningful second
 * number in a minute.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to SS format for seconds within
 * a minute.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_SECOND_TOO_SHORT if the data is too short for MM format.
 * @return #GS1_LINTER_SECOND_TOO_LONG if the data is too long for MM format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_SECOND if the data contains an invalid second.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_ss(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	int pos;

	assert(data);


	/*
	 * Data must be exactly two characters.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data_len != 2))
		GS1_LINTER_RETURN_ERROR(
			data_len < 2 ? GS1_LINTER_SECOND_TOO_SHORT : GS1_LINTER_SECOND_TOO_LONG,
			0,
			data_len
		);

	/*
	 * Data must consist of all digits.
	 */
	for (pos = 0; pos < 2; pos++)
		if (GS1_LINTER_UNLIKELY(data[pos] < '0' || data[pos] > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				(size_t)pos,
				1
			);

	/*
	 * Validate the second (00-59).
	 */
	if (GS1_LINTER_UNLIKELY((data[0] - '0') * 10 + (data[1] - '0') > 59))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_SECOND,
			0,
			2
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_ss(void)
{

	UNIT_TEST_PASS(gs1_lint_ss, "00");
	UNIT_TEST_PASS(gs1_lint_ss, "01");
	UNIT_TEST_PASS(gs1_lint_ss, "02");
	UNIT_TEST_PASS(gs1_lint_ss, "03");
	UNIT_TEST_PASS(gs1_lint_ss, "04");
	UNIT_TEST_PASS(gs1_lint_ss, "05");
	UNIT_TEST_PASS(gs1_lint_ss, "06");
	UNIT_TEST_PASS(gs1_lint_ss, "07");
	UNIT_TEST_PASS(gs1_lint_ss, "08");
	UNIT_TEST_PASS(gs1_lint_ss, "09");
	UNIT_TEST_PASS(gs1_lint_ss, "10");
	UNIT_TEST_PASS(gs1_lint_ss, "11");
	UNIT_TEST_PASS(gs1_lint_ss, "12");
	UNIT_TEST_PASS(gs1_lint_ss, "13");
	UNIT_TEST_PASS(gs1_lint_ss, "14");
	UNIT_TEST_PASS(gs1_lint_ss, "15");
	UNIT_TEST_PASS(gs1_lint_ss, "16");
	UNIT_TEST_PASS(gs1_lint_ss, "17");
	UNIT_TEST_PASS(gs1_lint_ss, "18");
	UNIT_TEST_PASS(gs1_lint_ss, "19");
	UNIT_TEST_PASS(gs1_lint_ss, "20");
	UNIT_TEST_PASS(gs1_lint_ss, "21");
	UNIT_TEST_PASS(gs1_lint_ss, "22");
	UNIT_TEST_PASS(gs1_lint_ss, "23");
	UNIT_TEST_PASS(gs1_lint_ss, "24");
	UNIT_TEST_PASS(gs1_lint_ss, "25");
	UNIT_TEST_PASS(gs1_lint_ss, "26");
	UNIT_TEST_PASS(gs1_lint_ss, "27");
	UNIT_TEST_PASS(gs1_lint_ss, "28");
	UNIT_TEST_PASS(gs1_lint_ss, "29");
	UNIT_TEST_PASS(gs1_lint_ss, "30");
	UNIT_TEST_PASS(gs1_lint_ss, "31");
	UNIT_TEST_PASS(gs1_lint_ss, "32");
	UNIT_TEST_PASS(gs1_lint_ss, "33");
	UNIT_TEST_PASS(gs1_lint_ss, "34");
	UNIT_TEST_PASS(gs1_lint_ss, "35");
	UNIT_TEST_PASS(gs1_lint_ss, "36");
	UNIT_TEST_PASS(gs1_lint_ss, "37");
	UNIT_TEST_PASS(gs1_lint_ss, "38");
	UNIT_TEST_PASS(gs1_lint_ss, "39");
	UNIT_TEST_PASS(gs1_lint_ss, "40");
	UNIT_TEST_PASS(gs1_lint_ss, "41");
	UNIT_TEST_PASS(gs1_lint_ss, "42");
	UNIT_TEST_PASS(gs1_lint_ss, "43");
	UNIT_TEST_PASS(gs1_lint_ss, "44");
	UNIT_TEST_PASS(gs1_lint_ss, "45");
	UNIT_TEST_PASS(gs1_lint_ss, "46");
	UNIT_TEST_PASS(gs1_lint_ss, "47");
	UNIT_TEST_PASS(gs1_lint_ss, "48");
	UNIT_TEST_PASS(gs1_lint_ss, "49");
	UNIT_TEST_PASS(gs1_lint_ss, "50");
	UNIT_TEST_PASS(gs1_lint_ss, "51");
	UNIT_TEST_PASS(gs1_lint_ss, "52");
	UNIT_TEST_PASS(gs1_lint_ss, "53");
	UNIT_TEST_PASS(gs1_lint_ss, "54");
	UNIT_TEST_PASS(gs1_lint_ss, "55");
	UNIT_TEST_PASS(gs1_lint_ss, "56");
	UNIT_TEST_PASS(gs1_lint_ss, "57");
	UNIT_TEST_PASS(gs1_lint_ss, "58");
	UNIT_TEST_PASS(gs1_lint_ss, "59");

	UNIT_TEST_FAIL(gs1_lint_ss, "60",  GS1_LINTER_ILLEGAL_SECOND, "*60*");

	UNIT_TEST_FAIL(gs1_lint_ss, "x0",  GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0");
	UNIT_TEST_FAIL(gs1_lint_ss, "0x",  GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*");

	UNIT_TEST_FAIL(gs1_lint_ss, "",    GS1_LINTER_SECOND_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_ss, "1",   GS1_LINTER_SECOND_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_ss, "111", GS1_LINTER_SECOND_TOO_LONG,  "*111*");
	UNIT_TEST_FAIL(gs1_lint_ss, "1111", GS1_LINTER_SECOND_TOO_LONG,  "*1111*");

}

#endif  /* UNIT_TESTS */
