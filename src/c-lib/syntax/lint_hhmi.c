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
 * @file lint_hhmi.c
 *
 * @brief The `hhmi` linter ensures that the given data is meaningful time as
 * hours and minutes, in HHMI format.
 *
 */


#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "gs1syntaxdictionary.h"
#include "gs1syntaxdictionary-utils.h"


/**
 * Used to ensure that an AI component conforms to HHMI format.
 *
 * @param [in] data Pointer to the data to be linted. Must not be `NULL`.
 * @param [in] data_len Length of the data to be linted.
 * @param [out] err_pos To facilitate error highlighting, the start position of
 *                      the bad data is written to this pointer, if not `NULL`.
 * @param [out] err_len The length of the bad data is written to this pointer, if
 *                      not `NULL`.
 *
 * @return #GS1_LINTER_OK if okay.
 * @return #GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT if the data is too short for HHMI format.
 * @return #GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG if the data is too long for HHMI format.
 * @return #GS1_LINTER_NON_DIGIT_CHARACTER if the data contains a non-digit character.
 * @return #GS1_LINTER_ILLEGAL_HOUR if the data contains an invalid hour.
 * @return #GS1_LINTER_ILLEGAL_MINUTE if the data contains an invalid minute.
 *
 */
GS1_SYNTAX_DICTIONARY_API gs1_lint_err_t gs1_lint_hhmi(const char* const data, size_t data_len, size_t* const err_pos, size_t* const err_len)
{

	gs1_lint_err_t ret;

	assert(data);

	/*
	 * Data must be four characters.
	 *
	 */
	if (GS1_LINTER_UNLIKELY(data_len != 4))
		GS1_LINTER_RETURN_ERROR(
			data_len < 4 ? GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT : GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG,
			0,
			data_len
		);

	ret = gs1_lint_hh(data, 2, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
	       ret == GS1_LINTER_ILLEGAL_HOUR);

	if (GS1_LINTER_UNLIKELY(ret != GS1_LINTER_OK))
		GS1_LINTER_RETURN_ERROR(ret, *err_pos, *err_len);

	ret = gs1_lint_mi(data + 2, 2, err_pos, err_len);

	assert(ret == GS1_LINTER_OK ||
	       ret == GS1_LINTER_NON_DIGIT_CHARACTER ||
	       ret == GS1_LINTER_ILLEGAL_MINUTE);

	if (GS1_LINTER_UNLIKELY(ret != GS1_LINTER_OK))
		GS1_LINTER_RETURN_ERROR(ret, *err_pos + 2, *err_len);

	GS1_LINTER_RETURN_OK;

}


#ifdef UNIT_TESTS

#include "unittest.h"

void test_lint_hhmi(void)
{

	UNIT_TEST_PASS(gs1_lint_hhmi, "0000");
	UNIT_TEST_PASS(gs1_lint_hhmi, "2359");

	UNIT_TEST_PASS(gs1_lint_hhmi, "0100");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0200");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0300");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0400");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0500");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0600");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0700");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0800");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0900");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1000");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1100");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1200");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1300");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1400");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1500");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1600");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1700");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1800");
	UNIT_TEST_PASS(gs1_lint_hhmi, "1900");
	UNIT_TEST_PASS(gs1_lint_hhmi, "2000");
	UNIT_TEST_PASS(gs1_lint_hhmi, "2100");
	UNIT_TEST_PASS(gs1_lint_hhmi, "2200");
	UNIT_TEST_PASS(gs1_lint_hhmi, "2300");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "2400", GS1_LINTER_ILLEGAL_HOUR, "*24*00");

	UNIT_TEST_PASS(gs1_lint_hhmi, "0001");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0002");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0003");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0004");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0005");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0006");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0007");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0008");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0009");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0010");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0011");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0012");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0013");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0014");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0015");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0016");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0017");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0018");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0019");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0020");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0021");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0022");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0023");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0024");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0025");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0026");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0027");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0028");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0029");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0030");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0031");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0032");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0033");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0034");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0035");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0036");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0037");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0038");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0039");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0040");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0041");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0042");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0043");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0044");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0045");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0046");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0047");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0048");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0049");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0050");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0051");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0052");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0053");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0054");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0055");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0056");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0057");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0058");
	UNIT_TEST_PASS(gs1_lint_hhmi, "0059");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "0060", GS1_LINTER_ILLEGAL_MINUTE, "00*60*");

	UNIT_TEST_FAIL(gs1_lint_hhmi, "x000", GS1_LINTER_NON_DIGIT_CHARACTER, "*x*000");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "0x00", GS1_LINTER_NON_DIGIT_CHARACTER, "0*x*00");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "00x0", GS1_LINTER_NON_DIGIT_CHARACTER, "00*x*0");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "000x", GS1_LINTER_NON_DIGIT_CHARACTER, "000*x*");

	UNIT_TEST_FAIL(gs1_lint_hhmi, "",      GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "**");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "1",     GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "*1*");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "11",    GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "*11*");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "111",   GS1_LINTER_HOUR_WITH_MINUTE_TOO_SHORT, "*111*");
	UNIT_TEST_FAIL(gs1_lint_hhmi, "11111", GS1_LINTER_HOUR_WITH_MINUTE_TOO_LONG,  "*11111*");

}

#endif  /* UNIT_TESTS */
