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
 * @file lint_yymmdd.c
 *
 * @brief The `yymmdd` linter ensures that the data represents a meaningful
 * date, in YYMMDD format.
 *
 */


#include <assert.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to the YYMMDD format.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
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
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yymmdd(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	gs1_lint_err_t ret;

	assert(data);

	ret = gs1_lint_yymmd0(data, data_len, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_DATE_TOO_SHORT ||
	       ret == GS1_LINTER_DATE_TOO_LONG ||
	       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
	       ret == GS1_LINTER_ILLEGAL_MONTH ||
	       ret == GS1_LINTER_ILLEGAL_DAY);

	if (GS1_LINTER_UNLIKELY(ret != GS1_LINTER_OK))
		GS1_LINTER_RETURN_ERROR(
			ret,
			*err_pos,
			*err_len
		);

	if (GS1_LINTER_UNLIKELY(data[4] == '0' && data[5] == '0'))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_DAY,
			4,
			2
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_yymmdd(void)
{

	UNIT_TEST_PASS(gs1_lint_yymmdd, "000606");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "250606");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "500606");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "750606");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "990606");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200131");

	UNIT_TEST_FAIL(gs1_lint_yymmdd, "x00131", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*00131");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "2x0131", GS1_LINTER_NON_DIGIT_CHARACTER, "2*x*0131");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "20x131", GS1_LINTER_NON_DIGIT_CHARACTER, "20*x*131");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200x31", GS1_LINTER_NON_DIGIT_CHARACTER, "200*x*31");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "2001x1", GS1_LINTER_NON_DIGIT_CHARACTER, "2001*x*1");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "20013x", GS1_LINTER_NON_DIGIT_CHARACTER, "20013*x*");

	UNIT_TEST_FAIL(gs1_lint_yymmdd, "",        GS1_LINTER_DATE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "1",       GS1_LINTER_DATE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "11",      GS1_LINTER_DATE_TOO_SHORT, "*11*");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "111",     GS1_LINTER_DATE_TOO_SHORT, "*111*");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "1111",    GS1_LINTER_DATE_TOO_SHORT, "*1111*");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "11111",   GS1_LINTER_DATE_TOO_SHORT, "*11111*");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "1111111", GS1_LINTER_DATE_TOO_LONG, "*1111111*");

	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200010", GS1_LINTER_ILLEGAL_MONTH, "20*00*10");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "201310", GS1_LINTER_ILLEGAL_MONTH, "20*13*10");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "209910", GS1_LINTER_ILLEGAL_MONTH, "20*99*10");

	UNIT_TEST_PASS(gs1_lint_yymmdd, "200131");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200132", GS1_LINTER_ILLEGAL_DAY, "2001*32*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200229");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200230", GS1_LINTER_ILLEGAL_DAY, "2002*30*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200331");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200332", GS1_LINTER_ILLEGAL_DAY, "2003*32*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200430");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200431", GS1_LINTER_ILLEGAL_DAY, "2004*31*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200531");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200532", GS1_LINTER_ILLEGAL_DAY, "2005*32*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200630");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200631", GS1_LINTER_ILLEGAL_DAY, "2006*31*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200731");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200732", GS1_LINTER_ILLEGAL_DAY, "2007*32*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200831");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200832", GS1_LINTER_ILLEGAL_DAY, "2008*32*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "200930");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200931", GS1_LINTER_ILLEGAL_DAY, "2009*31*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "201031");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "201032", GS1_LINTER_ILLEGAL_DAY, "2010*32*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "201130");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "201131", GS1_LINTER_ILLEGAL_DAY, "2011*31*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "201231");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "201232", GS1_LINTER_ILLEGAL_DAY, "2012*32*");

	UNIT_TEST_PASS(gs1_lint_yymmdd, "210228");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "210229", GS1_LINTER_ILLEGAL_DAY, "2102*29*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "220228");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "220229", GS1_LINTER_ILLEGAL_DAY, "2202*29*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "230228");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "230229", GS1_LINTER_ILLEGAL_DAY, "2302*29*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "240229");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "240230", GS1_LINTER_ILLEGAL_DAY, "2402*30*");

	UNIT_TEST_PASS(gs1_lint_yymmdd, "000229");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "000230", GS1_LINTER_ILLEGAL_DAY, "0002*30*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "040229");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "040230", GS1_LINTER_ILLEGAL_DAY, "0402*30*");
	UNIT_TEST_PASS(gs1_lint_yymmdd, "960229");
	UNIT_TEST_FAIL(gs1_lint_yymmdd, "960230", GS1_LINTER_ILLEGAL_DAY, "9602*30*");

	UNIT_TEST_FAIL(gs1_lint_yymmdd, "200600", GS1_LINTER_ILLEGAL_DAY, "2006*00*");	/* dd=00 forbidden */

}

#endif  /* UNIT_TESTS */
