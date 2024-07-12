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
 * @file lint_hh.c
 *
 * @brief The `hh` linter ensures that the given data is a meaningful hour
 * number in a day.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to HH format for hours within
 * a day.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_HOUR_TOO_SHORT if the data is too short for HH format.
 * @return #GS1_LINTER_HOUR_TOO_LONG if the data is too long for HH format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_HOUR if the data contains an invalid hour.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hh(const char* const data, size_t* const err_pos, size_t* const err_len)
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
		return len < 2 ? GS1_LINTER_HOUR_TOO_SHORT : GS1_LINTER_HOUR_TOO_LONG;
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
	if ((data[0] - '0') * 10 + (data[1] - '0') > 23) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_HOUR;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_hh(void)
{

	UNIT_TEST_PASS(gs1_lint_hh, "00");
	UNIT_TEST_PASS(gs1_lint_hh, "23");
	UNIT_TEST_FAIL(gs1_lint_hh, "24",  GS1_LINTER_ILLEGAL_HOUR, "*24*");

	UNIT_TEST_FAIL(gs1_lint_hh, "x0",  GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0");
	UNIT_TEST_FAIL(gs1_lint_hh, "0x",  GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*");

	UNIT_TEST_FAIL(gs1_lint_hh, "",    GS1_LINTER_HOUR_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_hh, "1",   GS1_LINTER_HOUR_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_hh, "111", GS1_LINTER_HOUR_TOO_LONG,  "*111*");

}

#endif  /* UNIT_TESTS */
