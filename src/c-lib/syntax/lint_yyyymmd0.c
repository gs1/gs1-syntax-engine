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
 * @file lint_yyyymmd0.c
 *
 * @brief The `yyyymmd0` linter ensures that the data represents a meaningful
 * date, in YYYYMMDD format, additionally permitting YYYYMM00 format indicating an
 * unspecified day.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to the YYYYMMDD or YYYYMM00 formats.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_DATE_TOO_SHORT if the data is too short for YYYYMMDD format.
 * @return #GS1_LINTER_DATE_TOO_LONG if the data is too long for YYYYMMDD format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_MONTH if the data contains an invalid month.
 * @return #GS1_LINTER_ILLEGAL_DAY if the data contains an invalid day of the month.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yyyymmd0(const char* const data, size_t* const err_pos, size_t* const err_len)
{

/// \cond
#define XX(d) ( (data[d] - '0') * 10 + (data[d+1] - '0') )
#define YY ( XX(0) * 100 + XX(2) )
#define MM XX(4)
#define DD XX(6)
/// \endcond

	static const unsigned char daysinmonth[] =
		{ 31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	size_t len, pos;
	unsigned char maxdd;

	assert(data);

	len = strlen(data);

	/*
	 * Data must be eight characters.
	 *
	 */
	if (len != 8)
		GS1_LINTER_RETURN_ERROR(
			len < 8 ? GS1_LINTER_DATE_TOO_SHORT : GS1_LINTER_DATE_TOO_LONG,
			0,
			len
		);

	/*
	 * Data must consist of all digits.
	 *
	 */
	if ((pos = strspn(data, "0123456789")) != len)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_NON_DIGIT_CHARACTER,
			pos,
			1
		);

	/*
	 * Validate that the month is 01 to 12.
	 *
	 */
	if (MM < 1 || MM > 12)
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_MONTH,
			4,
			2
		);

	/*
	 * Validate the day, accounting for leap years
	 *
	 */
	if (MM == 2) {		/* February; account for leap years */
		maxdd = ((YY % 4 == 0 && YY % 100 != 0) || YY % 400 == 0) ? 29 : 28;
	} else {
		maxdd = daysinmonth[MM - 1];		/* Based at 0 */
	}

	if (DD > maxdd)		/* Permit "00" */
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_DAY,
			6,
			2
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_yyyymmd0(void)
{

	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20000606");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20250606");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20500606");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20750606");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "19990606");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "19200131");

	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "x0200131", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0200131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "2x200131", GS1_LINTER_NON_DIGIT_CHARACTER, "2*x*200131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20x00131", GS1_LINTER_NON_DIGIT_CHARACTER, "20*x*00131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "202x0131", GS1_LINTER_NON_DIGIT_CHARACTER, "202*x*0131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "2020x131", GS1_LINTER_NON_DIGIT_CHARACTER, "2020*x*131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200x31", GS1_LINTER_NON_DIGIT_CHARACTER, "20200*x*31");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "202001x1", GS1_LINTER_NON_DIGIT_CHARACTER, "202001*x*1");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "2020013x", GS1_LINTER_NON_DIGIT_CHARACTER, "2020013*x*");

	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "",          GS1_LINTER_DATE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "1",         GS1_LINTER_DATE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "11",        GS1_LINTER_DATE_TOO_SHORT, "*11*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "111",       GS1_LINTER_DATE_TOO_SHORT, "*111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "1111",      GS1_LINTER_DATE_TOO_SHORT, "*1111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "11111",     GS1_LINTER_DATE_TOO_SHORT, "*11111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "111111",    GS1_LINTER_DATE_TOO_SHORT, "*111111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "1111111",   GS1_LINTER_DATE_TOO_SHORT, "*1111111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "111111111", GS1_LINTER_DATE_TOO_LONG, "*111111111*");

	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200010", GS1_LINTER_ILLEGAL_MONTH, "2020*00*10");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20201310", GS1_LINTER_ILLEGAL_MONTH, "2020*13*10");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20209910", GS1_LINTER_ILLEGAL_MONTH, "2020*99*10");

	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200132", GS1_LINTER_ILLEGAL_DAY, "202001*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200230", GS1_LINTER_ILLEGAL_DAY, "202002*30*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200331");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200332", GS1_LINTER_ILLEGAL_DAY, "202003*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200430");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200431", GS1_LINTER_ILLEGAL_DAY, "202004*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200531");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200532", GS1_LINTER_ILLEGAL_DAY, "202005*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200630");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200631", GS1_LINTER_ILLEGAL_DAY, "202006*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200731");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200732", GS1_LINTER_ILLEGAL_DAY, "202007*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200831");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200832", GS1_LINTER_ILLEGAL_DAY, "202008*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200930");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20200931", GS1_LINTER_ILLEGAL_DAY, "202009*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20201031");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20201032", GS1_LINTER_ILLEGAL_DAY, "202010*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20201130");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20201131", GS1_LINTER_ILLEGAL_DAY, "202011*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20201231");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20201232", GS1_LINTER_ILLEGAL_DAY, "202012*32*");

	// Not leap years
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20210228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20210229", GS1_LINTER_ILLEGAL_DAY, "202102*29*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20220228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20220229", GS1_LINTER_ILLEGAL_DAY, "202202*29*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20230228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20230229", GS1_LINTER_ILLEGAL_DAY, "202302*29*");

	// Leap years
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "19960229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "19960230", GS1_LINTER_ILLEGAL_DAY, "199602*30*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20000229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20000230", GS1_LINTER_ILLEGAL_DAY, "200002*30*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20040229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20040230", GS1_LINTER_ILLEGAL_DAY, "200402*30*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20240229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "20240230", GS1_LINTER_ILLEGAL_DAY, "202402*30*");

	// Not leap year (divisable by 100 but not 400)
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "21000228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "21000229", GS1_LINTER_ILLEGAL_DAY, "210002*29*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "22000228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "22000229", GS1_LINTER_ILLEGAL_DAY, "220002*29*");
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "23000228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "23000229", GS1_LINTER_ILLEGAL_DAY, "230002*29*");

	// Leap year (divisable by 400)
	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "24000229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmd0, "24000230", GS1_LINTER_ILLEGAL_DAY, "240002*30*");

	UNIT_TEST_PASS(gs1_lint_yyyymmd0, "20200600");		/* dd=00 permitted */

}

#endif  /* UNIT_TESTS */
