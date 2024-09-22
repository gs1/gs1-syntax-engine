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
#include "gs1syntaxdictionary-utils.h"


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

	size_t len;
	gs1_lint_err_t ret;
	char buf[3] = { 0 };

	assert(data);

	len = strlen(data);

	/*
	 * Data must be four characters.
	 *
	 */
	if (len != 4)
		GS1_LINTER_RETURN_ERROR(
			len < 4 ? GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT : GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG,
			0,
			len
		);

	memcpy(buf, data, 2);
	ret = gs1_lint_hh(buf, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
	       ret == GS1_LINTER_ILLEGAL_HOUR);

	if (ret != GS1_LINTER_OK)
		GS1_LINTER_RETURN_ERROR(ret, *err_pos, *err_len);

	memcpy(buf, data+2, 2);
	ret = gs1_lint_mm(buf, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
	       ret == GS1_LINTER_ILLEGAL_MINUTE);

	if (ret != GS1_LINTER_OK)
		GS1_LINTER_RETURN_ERROR(ret, *err_pos + 2, *err_len);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_hhmm(void)
{

	UNIT_TEST_PASS(gs1_lint_hhmm, "0000");
	UNIT_TEST_PASS(gs1_lint_hhmm, "2359");

	UNIT_TEST_PASS(gs1_lint_hhmm, "0100");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0200");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0300");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0400");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0500");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0600");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0700");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0800");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0900");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1000");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1100");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1200");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1300");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1400");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1500");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1600");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1700");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1800");
	UNIT_TEST_PASS(gs1_lint_hhmm, "1900");
	UNIT_TEST_PASS(gs1_lint_hhmm, "2000");
	UNIT_TEST_PASS(gs1_lint_hhmm, "2100");
	UNIT_TEST_PASS(gs1_lint_hhmm, "2200");
	UNIT_TEST_PASS(gs1_lint_hhmm, "2300");
	UNIT_TEST_FAIL(gs1_lint_hhmm, "2400", GS1_LINTER_ILLEGAL_HOUR, "*24*00");

	UNIT_TEST_PASS(gs1_lint_hhmm, "0001");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0002");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0003");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0004");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0005");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0006");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0007");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0008");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0009");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0010");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0011");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0012");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0013");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0014");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0015");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0016");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0017");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0018");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0019");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0020");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0021");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0022");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0023");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0024");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0025");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0026");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0027");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0028");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0029");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0030");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0031");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0032");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0033");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0034");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0035");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0036");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0037");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0038");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0039");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0040");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0041");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0042");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0043");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0044");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0045");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0046");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0047");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0048");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0049");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0050");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0051");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0052");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0053");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0054");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0055");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0056");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0057");
	UNIT_TEST_PASS(gs1_lint_hhmm, "0058");
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
