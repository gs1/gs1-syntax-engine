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
 * @file lint_mi.c
 *
 * @brief The `mi` linter ensures that the given data is a meaningful minute
 * number in an hour.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to MI format for minutes within
 * an hour.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_MINUTE_TOO_SHORT if the data is too short for MI format.
 * @return #GS1_LINTER_MINUTE_TOO_LONG if the data is too long for MI format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_MINUTE if the data contains an invalid minute.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_mi(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	int pos;

	assert(data);


	/*
	 * Data must be exactly two characters.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data_len != 2))
		GS1_LINTER_RETURN_ERROR(
			data_len < 2 ? GS1_LINTER_MINUTE_TOO_SHORT : GS1_LINTER_MINUTE_TOO_LONG,
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
	 * Validate the minute.
	 *
	 */
	if (GS1_LINTER_UNLIKELY((data[0] - '0') * 10 + (data[1] - '0') > 59))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_MINUTE,
			0,
			2
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_mi(void)
{

	UNIT_TEST_PASS(gs1_lint_mi, "00");
	UNIT_TEST_PASS(gs1_lint_mi, "01");
	UNIT_TEST_PASS(gs1_lint_mi, "02");
	UNIT_TEST_PASS(gs1_lint_mi, "03");
	UNIT_TEST_PASS(gs1_lint_mi, "04");
	UNIT_TEST_PASS(gs1_lint_mi, "05");
	UNIT_TEST_PASS(gs1_lint_mi, "06");
	UNIT_TEST_PASS(gs1_lint_mi, "07");
	UNIT_TEST_PASS(gs1_lint_mi, "08");
	UNIT_TEST_PASS(gs1_lint_mi, "09");
	UNIT_TEST_PASS(gs1_lint_mi, "10");
	UNIT_TEST_PASS(gs1_lint_mi, "11");
	UNIT_TEST_PASS(gs1_lint_mi, "12");
	UNIT_TEST_PASS(gs1_lint_mi, "13");
	UNIT_TEST_PASS(gs1_lint_mi, "14");
	UNIT_TEST_PASS(gs1_lint_mi, "15");
	UNIT_TEST_PASS(gs1_lint_mi, "16");
	UNIT_TEST_PASS(gs1_lint_mi, "17");
	UNIT_TEST_PASS(gs1_lint_mi, "18");
	UNIT_TEST_PASS(gs1_lint_mi, "19");
	UNIT_TEST_PASS(gs1_lint_mi, "20");
	UNIT_TEST_PASS(gs1_lint_mi, "21");
	UNIT_TEST_PASS(gs1_lint_mi, "22");
	UNIT_TEST_PASS(gs1_lint_mi, "23");
	UNIT_TEST_PASS(gs1_lint_mi, "24");
	UNIT_TEST_PASS(gs1_lint_mi, "25");
	UNIT_TEST_PASS(gs1_lint_mi, "26");
	UNIT_TEST_PASS(gs1_lint_mi, "27");
	UNIT_TEST_PASS(gs1_lint_mi, "28");
	UNIT_TEST_PASS(gs1_lint_mi, "29");
	UNIT_TEST_PASS(gs1_lint_mi, "30");
	UNIT_TEST_PASS(gs1_lint_mi, "31");
	UNIT_TEST_PASS(gs1_lint_mi, "32");
	UNIT_TEST_PASS(gs1_lint_mi, "33");
	UNIT_TEST_PASS(gs1_lint_mi, "34");
	UNIT_TEST_PASS(gs1_lint_mi, "35");
	UNIT_TEST_PASS(gs1_lint_mi, "36");
	UNIT_TEST_PASS(gs1_lint_mi, "37");
	UNIT_TEST_PASS(gs1_lint_mi, "38");
	UNIT_TEST_PASS(gs1_lint_mi, "39");
	UNIT_TEST_PASS(gs1_lint_mi, "40");
	UNIT_TEST_PASS(gs1_lint_mi, "41");
	UNIT_TEST_PASS(gs1_lint_mi, "42");
	UNIT_TEST_PASS(gs1_lint_mi, "43");
	UNIT_TEST_PASS(gs1_lint_mi, "44");
	UNIT_TEST_PASS(gs1_lint_mi, "45");
	UNIT_TEST_PASS(gs1_lint_mi, "46");
	UNIT_TEST_PASS(gs1_lint_mi, "47");
	UNIT_TEST_PASS(gs1_lint_mi, "48");
	UNIT_TEST_PASS(gs1_lint_mi, "49");
	UNIT_TEST_PASS(gs1_lint_mi, "50");
	UNIT_TEST_PASS(gs1_lint_mi, "51");
	UNIT_TEST_PASS(gs1_lint_mi, "52");
	UNIT_TEST_PASS(gs1_lint_mi, "53");
	UNIT_TEST_PASS(gs1_lint_mi, "54");
	UNIT_TEST_PASS(gs1_lint_mi, "55");
	UNIT_TEST_PASS(gs1_lint_mi, "56");
	UNIT_TEST_PASS(gs1_lint_mi, "57");
	UNIT_TEST_PASS(gs1_lint_mi, "58");
	UNIT_TEST_PASS(gs1_lint_mi, "59");
	UNIT_TEST_FAIL(gs1_lint_mi, "60",  GS1_LINTER_ILLEGAL_MINUTE, "*60*");

	UNIT_TEST_FAIL(gs1_lint_mi, "x0",  GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0");
	UNIT_TEST_FAIL(gs1_lint_mi, "0x",  GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*");

	UNIT_TEST_FAIL(gs1_lint_mi, "",    GS1_LINTER_MINUTE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_mi, "1",   GS1_LINTER_MINUTE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_mi, "111", GS1_LINTER_MINUTE_TOO_LONG,  "*111*");

}

#endif  /* UNIT_TESTS */
