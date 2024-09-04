/*
 * GS1 Syntax Dictionary. Copyright (c) 2022-2024 GS1 AISBL.
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
 * @file lint_mm.c
 *
 * @brief The `mm` linter ensures that the given data is a meaningful minute
 * number in an hour.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to MM format for minutes within
 * an hour.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_MINUTE_TOO_SHORT if the data is too short for MM format.
 * @return #GS1_LINTER_MINUTE_TOO_LONG if the data is too long for MM format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_MINUTE if the data contains an invalid minute.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_mm(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t len, pos;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be two characters.
	 *
	 */
	if (len != 2) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return len < 2 ? GS1_LINTER_MINUTE_TOO_SHORT : GS1_LINTER_MINUTE_TOO_LONG;
	}

	/*
	 * Data must consist of all digits.
	 *
	 */
	if ((pos = strspn(data, "0123456789")) != len) {
		if (err_pos) *err_pos = pos;
		if (err_len) *err_len = 1;
		return GS1_LINTER_NON_DIGIT_CHARACTER;
	}

	/*
	 * Validate the minute.
	 *
	 */
	if ((data[0] - '0') * 10 + (data[1] - '0') > 59) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_MINUTE;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_mm(void)
{

	UNIT_TEST_PASS(gs1_lint_mm, "00");
	UNIT_TEST_PASS(gs1_lint_mm, "01");
	UNIT_TEST_PASS(gs1_lint_mm, "02");
	UNIT_TEST_PASS(gs1_lint_mm, "03");
	UNIT_TEST_PASS(gs1_lint_mm, "04");
	UNIT_TEST_PASS(gs1_lint_mm, "05");
	UNIT_TEST_PASS(gs1_lint_mm, "06");
	UNIT_TEST_PASS(gs1_lint_mm, "07");
	UNIT_TEST_PASS(gs1_lint_mm, "08");
	UNIT_TEST_PASS(gs1_lint_mm, "09");
	UNIT_TEST_PASS(gs1_lint_mm, "10");
	UNIT_TEST_PASS(gs1_lint_mm, "11");
	UNIT_TEST_PASS(gs1_lint_mm, "12");
	UNIT_TEST_PASS(gs1_lint_mm, "13");
	UNIT_TEST_PASS(gs1_lint_mm, "14");
	UNIT_TEST_PASS(gs1_lint_mm, "15");
	UNIT_TEST_PASS(gs1_lint_mm, "16");
	UNIT_TEST_PASS(gs1_lint_mm, "17");
	UNIT_TEST_PASS(gs1_lint_mm, "18");
	UNIT_TEST_PASS(gs1_lint_mm, "19");
	UNIT_TEST_PASS(gs1_lint_mm, "20");
	UNIT_TEST_PASS(gs1_lint_mm, "21");
	UNIT_TEST_PASS(gs1_lint_mm, "22");
	UNIT_TEST_PASS(gs1_lint_mm, "23");
	UNIT_TEST_PASS(gs1_lint_mm, "24");
	UNIT_TEST_PASS(gs1_lint_mm, "25");
	UNIT_TEST_PASS(gs1_lint_mm, "26");
	UNIT_TEST_PASS(gs1_lint_mm, "27");
	UNIT_TEST_PASS(gs1_lint_mm, "28");
	UNIT_TEST_PASS(gs1_lint_mm, "29");
	UNIT_TEST_PASS(gs1_lint_mm, "30");
	UNIT_TEST_PASS(gs1_lint_mm, "31");
	UNIT_TEST_PASS(gs1_lint_mm, "32");
	UNIT_TEST_PASS(gs1_lint_mm, "33");
	UNIT_TEST_PASS(gs1_lint_mm, "34");
	UNIT_TEST_PASS(gs1_lint_mm, "35");
	UNIT_TEST_PASS(gs1_lint_mm, "36");
	UNIT_TEST_PASS(gs1_lint_mm, "37");
	UNIT_TEST_PASS(gs1_lint_mm, "38");
	UNIT_TEST_PASS(gs1_lint_mm, "39");
	UNIT_TEST_PASS(gs1_lint_mm, "40");
	UNIT_TEST_PASS(gs1_lint_mm, "41");
	UNIT_TEST_PASS(gs1_lint_mm, "42");
	UNIT_TEST_PASS(gs1_lint_mm, "43");
	UNIT_TEST_PASS(gs1_lint_mm, "44");
	UNIT_TEST_PASS(gs1_lint_mm, "45");
	UNIT_TEST_PASS(gs1_lint_mm, "46");
	UNIT_TEST_PASS(gs1_lint_mm, "47");
	UNIT_TEST_PASS(gs1_lint_mm, "48");
	UNIT_TEST_PASS(gs1_lint_mm, "49");
	UNIT_TEST_PASS(gs1_lint_mm, "50");
	UNIT_TEST_PASS(gs1_lint_mm, "51");
	UNIT_TEST_PASS(gs1_lint_mm, "52");
	UNIT_TEST_PASS(gs1_lint_mm, "53");
	UNIT_TEST_PASS(gs1_lint_mm, "54");
	UNIT_TEST_PASS(gs1_lint_mm, "55");
	UNIT_TEST_PASS(gs1_lint_mm, "56");
	UNIT_TEST_PASS(gs1_lint_mm, "57");
	UNIT_TEST_PASS(gs1_lint_mm, "58");
	UNIT_TEST_PASS(gs1_lint_mm, "59");
	UNIT_TEST_FAIL(gs1_lint_mm, "60",  GS1_LINTER_ILLEGAL_MINUTE, "*60*");

	UNIT_TEST_FAIL(gs1_lint_mm, "x0",  GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0");
	UNIT_TEST_FAIL(gs1_lint_mm, "0x",  GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*");

	UNIT_TEST_FAIL(gs1_lint_mm, "",    GS1_LINTER_MINUTE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_mm, "1",   GS1_LINTER_MINUTE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_mm, "111", GS1_LINTER_MINUTE_TOO_LONG,  "*111*");

}

#endif  /* UNIT_TESTS */
