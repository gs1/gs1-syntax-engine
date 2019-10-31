/*
 * GS1 Syntax Dictionary. Copyright (c) 2022 GS1 AISBL.
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
 * @file lint_yymmddhh.c
 *
 * @brief The `yymmddhh` linter ensures that the data represents a meaningful
 * date and hour of day, in YYMMDDHH format.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"


/**
 * Used to ensure that an AI component conforms to the YYMMDDHH format.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT if the data is too short for YYMMDDHH format.
 * @return #GS1_LINTER_DATE_WITH_HOUR_TOO_LONG if the data is too long for YYMMDDHH format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_MONTH if the data contains an invalid month.
 * @return #GS1_LINTER_ILLEGAL_DAY if the data contains an invalid day of the month.
 * @return #GS1_LINTER_ILLEGAL_HOUR if the data contains an illegal hour.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yymmddhh(const char *data, size_t *err_pos, size_t *err_len)
{

	char yymmdd[7] = {0};
	size_t pos, len;
	gs1_lint_err_t ret;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be eight characters.
	 *
	 */
	if (len != 8) {
		if (err_pos) *err_pos = 0;
		if (err_len) *err_len = len;
		return len < 8 ?
			GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT :
			GS1_LINTER_DATE_WITH_HOUR_TOO_LONG;
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
	 * Validate the date component.
	 *
	 */
	memcpy(yymmdd, data, 6);
	ret = gs1_lint_yymmdd(yymmdd, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_DATE_TOO_SHORT ||
	       ret == GS1_LINTER_DATE_TOO_LONG ||
	       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
	       ret == GS1_LINTER_ILLEGAL_MONTH ||
	       ret == GS1_LINTER_ILLEGAL_DAY);

	if (ret != GS1_LINTER_OK)
		return ret;

	/*
	 * Validate the hour component.
	 *
	 */
	if ((data[6] - '0') * 10 + (data[7] - '0') > 23) {
		if (err_pos) *err_pos = 6;
		if (err_len) *err_len = 2;
		return GS1_LINTER_ILLEGAL_HOUR;
	}

	return GS1_LINTER_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_yymmddhh(void)
{

	UNIT_TEST_PASS(gs1_lint_yymmddhh, "00060600");
	UNIT_TEST_PASS(gs1_lint_yymmddhh, "00060623");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "20001000", GS1_LINTER_ILLEGAL_MONTH, "20*00*1000");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "20093100", GS1_LINTER_ILLEGAL_DAY,   "2009*31*00");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "00060624", GS1_LINTER_ILLEGAL_HOUR,  "000606*24*");

	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "200131x0", GS1_LINTER_NON_DIGIT_CHARACTER, "200131*x*0");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "2001310x", GS1_LINTER_NON_DIGIT_CHARACTER, "2001310*x*");

	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "",          GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "1",         GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "11",        GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "*11*");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "111",       GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "*111*");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "1111",      GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "*1111*");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "11111",     GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "*11111*");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "111111",    GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "*111111*");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "1111111",   GS1_LINTER_DATE_WITH_HOUR_TOO_SHORT, "*1111111*");
	UNIT_TEST_FAIL(gs1_lint_yymmddhh, "111111111", GS1_LINTER_DATE_WITH_HOUR_TOO_LONG,  "*111111111*");

}

#endif  /* UNIT_TESTS */
