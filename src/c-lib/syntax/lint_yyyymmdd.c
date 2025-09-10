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
 * @file lint_yyyymmdd.c
 *
 * @brief The `yyyymmdd` linter ensures that the data represents a meaningful
 * date, in YYYYMMDD format.
 *
 */


#include <assert.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to the YYYYMMDD format.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted. Must not
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
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_yyyymmdd(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	gs1_lint_err_t ret;

	assert(data);

	ret = gs1_lint_yyyymmd0(data, data_len, err_pos, err_len);

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

	if (GS1_LINTER_UNLIKELY(data[6] == '0' && data[7] == '0'))
		GS1_LINTER_RETURN_ERROR(
			GS1_LINTER_ILLEGAL_DAY,
			6,
			2
		);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_yyyymmdd(void)
{

	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20000606");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20250606");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20500606");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "19750606");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "19990606");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200131");

	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "x0200131", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*0200131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "2x200131", GS1_LINTER_NON_DIGIT_CHARACTER, "2*x*200131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20x00131", GS1_LINTER_NON_DIGIT_CHARACTER, "20*x*00131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "202x0131", GS1_LINTER_NON_DIGIT_CHARACTER, "202*x*0131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "2020x131", GS1_LINTER_NON_DIGIT_CHARACTER, "2020*x*131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200x31", GS1_LINTER_NON_DIGIT_CHARACTER, "20200*x*31");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "202001x1", GS1_LINTER_NON_DIGIT_CHARACTER, "202001*x*1");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "2020013x", GS1_LINTER_NON_DIGIT_CHARACTER, "2020013*x*");

	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "",          GS1_LINTER_DATE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "1",         GS1_LINTER_DATE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "11",        GS1_LINTER_DATE_TOO_SHORT, "*11*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "111",       GS1_LINTER_DATE_TOO_SHORT, "*111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "1111",      GS1_LINTER_DATE_TOO_SHORT, "*1111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "11111",     GS1_LINTER_DATE_TOO_SHORT, "*11111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "111111",    GS1_LINTER_DATE_TOO_SHORT, "*111111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "1111111",   GS1_LINTER_DATE_TOO_SHORT, "*1111111*");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "111111111", GS1_LINTER_DATE_TOO_LONG, "*111111111*");

	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200010", GS1_LINTER_ILLEGAL_MONTH, "2020*00*10");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20201310", GS1_LINTER_ILLEGAL_MONTH, "2020*13*10");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20209910", GS1_LINTER_ILLEGAL_MONTH, "2020*99*10");

	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200131");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200132", GS1_LINTER_ILLEGAL_DAY, "202001*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200230", GS1_LINTER_ILLEGAL_DAY, "202002*30*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200331");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200332", GS1_LINTER_ILLEGAL_DAY, "202003*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200430");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200431", GS1_LINTER_ILLEGAL_DAY, "202004*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200531");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200532", GS1_LINTER_ILLEGAL_DAY, "202005*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200630");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200631", GS1_LINTER_ILLEGAL_DAY, "202006*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200731");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200732", GS1_LINTER_ILLEGAL_DAY, "202007*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200831");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200832", GS1_LINTER_ILLEGAL_DAY, "202008*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20200930");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200931", GS1_LINTER_ILLEGAL_DAY, "202009*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20201031");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20201032", GS1_LINTER_ILLEGAL_DAY, "202010*32*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20201130");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20201131", GS1_LINTER_ILLEGAL_DAY, "202011*31*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20201231");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20201232", GS1_LINTER_ILLEGAL_DAY, "202012*32*");

	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20210228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20210229", GS1_LINTER_ILLEGAL_DAY, "202102*29*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20220228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20220229", GS1_LINTER_ILLEGAL_DAY, "202202*29*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20230228");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20230229", GS1_LINTER_ILLEGAL_DAY, "202302*29*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20240229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20240230", GS1_LINTER_ILLEGAL_DAY, "202402*30*");

	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20000229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20000230", GS1_LINTER_ILLEGAL_DAY, "200002*30*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "20040229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20040230", GS1_LINTER_ILLEGAL_DAY, "200402*30*");
	UNIT_TEST_PASS(gs1_lint_yyyymmdd, "19960229");
	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "19960230", GS1_LINTER_ILLEGAL_DAY, "199602*30*");

	UNIT_TEST_FAIL(gs1_lint_yyyymmdd, "20200600", GS1_LINTER_ILLEGAL_DAY, "202006*00*");	/* dd=00 forbidden */

}

#endif  /* UNIT_TESTS */
