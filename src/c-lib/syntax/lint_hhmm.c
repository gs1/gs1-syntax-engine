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
 * @file lint_hhmm.c
 *
 * @brief The `hhmm` linter ensures that the given data is meaningful time as
 * hours and minutes, in HHMM format.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to HHMM format.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT if the data is too short for HHMM format.
 * @return #GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG if the data is too long for HHMM format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_HOUR if the data contains an invalid hour.
 * @return #GS1_LINTER_ILLEGAL_MINUTE if the data contains an invalid minute.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hhmm(const char* const data, size_t* const err_pos, size_t* const err_len)
{

	size_t len, pos;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be four characters.
	 *
	 */
	if (len != 4) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return len < 4 ? GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT : GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG;
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
	 * Validate the hour.
	 *
	 */
	if ((data[0] - '0') * 10 + (data[1] - '0') > 23) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_HOUR;
	}

	/*
	 * Validate the minute.
	 *
	 */
	if ((data[2] - '0') * 10 + (data[3] - '0') > 59) {
		if (err_pos) *err_pos = 2;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_MINUTE;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_hhmm(void)
{

	UNIT_TEST_PASS(gs1_lint_hhmm, "0000");
	UNIT_TEST_PASS(gs1_lint_hhmm, "2300");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "2400", GS1_LINTER_ILLEGAL_HOUR, "*24*00");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0059");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "0060", GS1_LINTER_ILLEGAL_MINUTE, "00*60*");

	UNIT_TEST_FAIL(gs1_lint_hhmm, "x000", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*000");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "0x00", GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*00");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "00x0", GS1_LINTER_NON_DIGIT_CHARACTER, "00*x*0");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "000x", GS1_LINTER_NON_DIGIT_CHARACTER, "000*x*");

	UNIT_TEST_FAIL(gs1_lint_hhmm, "",      GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "1",     GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "11",    GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "*11*");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "111",   GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "*111*");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "11111", GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG,  "*11111*");

}

#endif  /* UNIT_TESTS */
