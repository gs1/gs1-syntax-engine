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
 * @file lint_yymmd0.c
 *
 * @brief The `yymmd0` linter ensures that the data represents a meaningful
 * date, in YYMMDD format, additionally permitting YYMM00 format indicating an
 * unspecified day.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


#ifndef CURRENT_YEAR
#define CURRENT_YEAR 21  ///< 20YY: For converting YY to 19YY, 20YY, 21YY, etc. for leap year validation
#endif


/**
 * Used to ensure that an AI component conforms to the YYMMDD or YYMM00
 * formats.
 *
 * @param [in] data Pointer to the null-terminated data to be linted. Must not
 *                  be `NULL`.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_DATE_TOO_SHORT if the data is too short for YYMMDD format.
 * @return #GS1_LINTER_DATE_TOO_LONG if the data is too long for YYMMDD format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_MONTH if the data contains an invalid month.
 * @return #GS1_LINTER_ILLEGAL_DAY if the data contains an invalid day of the month.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yymmd0(const char* const data, size_t* const err_pos, size_t* const err_len)
{

/// \cond
#define YY ( (data[0] - '0') * 10 + (data[1] - '0') )
/// \endcond

	size_t pos;
	char yyyymmdd[9] = {0};
	gs1_lint_err_t ret;

	assert(data);

	/*
	 * Data must consist of all digits.
	 *
	 */
	for (pos = 0; pos < 6 && data[pos]; pos++) {
		if (GS1_LINTER_UNLIKELY(data[pos] < '0' || data[pos] > '9'))
			GS1_LINTER_RETURN_ERROR(
				GS1_LINTER_NON_DIGIT_CHARACTER,
				pos,
				1
			);
	}

	/*
	 * Data must be six characters.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(pos != 6 || data[6]))
		GS1_LINTER_RETURN_ERROR(
			(pos < 6) ? GS1_LINTER_DATE_TOO_SHORT : GS1_LINTER_DATE_TOO_LONG,
			0,
			pos + (data[pos] ? 1 : 0)
		);

	memcpy(yyyymmdd + 2, data, 6);

	/*
	 * Convert YY to a year using a horizon based on CURRENT_YEAR.
	 *
	 */
	if (YY - CURRENT_YEAR >= 51) {
		yyyymmdd[0] = '1'; yyyymmdd[1] = '9';
	} else if (YY - CURRENT_YEAR > -50) {
		yyyymmdd[0] = '2'; yyyymmdd[1] = '0';
	} else {
		yyyymmdd[0] = '2'; yyyymmdd[1] = '1';
	}

	ret = gs1_lint_yyyymmd0(yyyymmdd, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_ILLEGAL_MONTH ||
	       ret == GS1_LINTER_ILLEGAL_DAY);

	assert(!err_pos || ret == GS1_LINTER_OK || (*err_pos >= 2));
	assert(!err_pos || !err_len || ret == GS1_LINTER_OK || (*err_pos + *err_len <= 8));

	if (GS1_LINTER_UNLIKELY(ret != GS1_LINTER_OK))
		GS1_LINTER_RETURN_ERROR(
			ret,
			*err_pos - 2,
			*err_len
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_yymmd0(void)
{

	UNIT_TEST_PASS(gs1_lint_yymmd0, "000606");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "250606");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "500606");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "750606");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "990606");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200131");

	UNIT_TEST_FAIL(gs1_lint_yymmd0, "x00131", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*00131");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "2x0131", GS1_LINTER_NON_DIGIT_CHARACTER, "2*x*0131");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "20x131", GS1_LINTER_NON_DIGIT_CHARACTER, "20*x*131");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200x31", GS1_LINTER_NON_DIGIT_CHARACTER, "200*x*31");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "2001x1", GS1_LINTER_NON_DIGIT_CHARACTER, "2001*x*1");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "20013x", GS1_LINTER_NON_DIGIT_CHARACTER, "20013*x*");

	UNIT_TEST_FAIL(gs1_lint_yymmd0, "",        GS1_LINTER_DATE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "1",       GS1_LINTER_DATE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "11",      GS1_LINTER_DATE_TOO_SHORT, "*11*");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "111",     GS1_LINTER_DATE_TOO_SHORT, "*111*");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "1111",    GS1_LINTER_DATE_TOO_SHORT, "*1111*");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "11111",   GS1_LINTER_DATE_TOO_SHORT, "*11111*");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "1111111", GS1_LINTER_DATE_TOO_LONG, "*1111111*");

	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200010", GS1_LINTER_ILLEGAL_MONTH, "20*00*10");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "201310", GS1_LINTER_ILLEGAL_MONTH, "20*13*10");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "209910", GS1_LINTER_ILLEGAL_MONTH, "20*99*10");

	UNIT_TEST_PASS(gs1_lint_yymmd0, "200131");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200132", GS1_LINTER_ILLEGAL_DAY, "2001*32*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200229");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200230", GS1_LINTER_ILLEGAL_DAY, "2002*30*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200331");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200332", GS1_LINTER_ILLEGAL_DAY, "2003*32*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200430");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200431", GS1_LINTER_ILLEGAL_DAY, "2004*31*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200531");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200532", GS1_LINTER_ILLEGAL_DAY, "2005*32*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200630");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200631", GS1_LINTER_ILLEGAL_DAY, "2006*31*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200731");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200732", GS1_LINTER_ILLEGAL_DAY, "2007*32*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200831");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200832", GS1_LINTER_ILLEGAL_DAY, "2008*32*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "200930");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "200931", GS1_LINTER_ILLEGAL_DAY, "2009*31*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "201031");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "201032", GS1_LINTER_ILLEGAL_DAY, "2010*32*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "201130");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "201131", GS1_LINTER_ILLEGAL_DAY, "2011*31*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "201231");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "201232", GS1_LINTER_ILLEGAL_DAY, "2012*32*");

	UNIT_TEST_PASS(gs1_lint_yymmd0, "210228");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "210229", GS1_LINTER_ILLEGAL_DAY, "2102*29*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "220228");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "220229", GS1_LINTER_ILLEGAL_DAY, "2202*29*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "230228");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "230229", GS1_LINTER_ILLEGAL_DAY, "2302*29*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "240229");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "240230", GS1_LINTER_ILLEGAL_DAY, "2402*30*");

	UNIT_TEST_PASS(gs1_lint_yymmd0, "000229");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "000230", GS1_LINTER_ILLEGAL_DAY, "0002*30*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "040229");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "040230", GS1_LINTER_ILLEGAL_DAY, "0402*30*");
	UNIT_TEST_PASS(gs1_lint_yymmd0, "960229");
	UNIT_TEST_FAIL(gs1_lint_yymmd0, "960230", GS1_LINTER_ILLEGAL_DAY, "9602*30*");

	UNIT_TEST_PASS(gs1_lint_yymmd0, "200600");		/* dd=00 permitted */

}

#endif  /* UNIT_TESTS */
